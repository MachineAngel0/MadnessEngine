#include <io.h>

#include "filesystem.h"

#if MPLATFORM_WINDOWS


//FILE SYSTEM

typedef struct Windows_IO
{
    HANDLE file_handle;
} Windows_IO;

bool platform_file_open(Madness_File_Platform* file, const char* path, File_Modes file_modes, Allocator* allocator)
{
    file->internal_data = allocator_alloc(allocator, sizeof(Windows_IO));
    Windows_IO* windows_io = (Windows_IO*)file->internal_data;

    DWORD win_file_mode;
    DWORD win_share_mode;
    switch (file->file_mode)
    {
    case FILE_MODE_READ:
        win_file_mode = GENERIC_READ;
        win_share_mode = FILE_SHARE_READ;
        break;
    case FILE_MODE_WRITE:
        win_file_mode = GENERIC_WRITE;
        win_share_mode = FILE_SHARE_WRITE;
        break;
    case FILE_MODE_READ_WRITE:
        win_file_mode = GENERIC_WRITE | GENERIC_READ;
        win_share_mode = FILE_SHARE_READ | FILE_SHARE_WRITE;
        break;
    case FILE_MODE_WRITE_APPEND:
        win_file_mode = FILE_APPEND_DATA;
        win_share_mode = FILE_SHARE_WRITE;
        break;
    case FILE_MODE_READ_WRITE_APPEND:
        win_file_mode = GENERIC_READ | FILE_APPEND_DATA;
        win_share_mode = FILE_SHARE_READ | FILE_SHARE_WRITE;
        break;
    }

    windows_io->file_handle = CreateFileA(path, win_file_mode, win_share_mode,
                                     NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL,
                                     NULL);


    if (windows_io->file_handle == INVALID_HANDLE_VALUE)
    {
        // error
        return false;
    }


    return true;
}

bool platform_file_close(Madness_File_Platform* file)
{
    Windows_IO* windows_io = (Windows_IO*)file->internal_data;
    return CloseHandle(windows_io->file_handle);
}

bool platform_file_read(Madness_File_Platform* madness_file)
{
    Windows_IO* windows_io = (Windows_IO*)madness_file->internal_data;
     char* data = "Hello from Native Win32 API!";
    DWORD bytesWritten = 0;
    return ReadFile(windows_io->file_handle, data, (DWORD)strlen(data), &bytesWritten, NULL);
}

bool platform_file_write(Madness_File_Platform* madness_file)
{
    Windows_IO* windows_io = (Windows_IO*)madness_file->internal_data;
    const char* data = "Hello from Native Win32 API!";
    DWORD bytesWritten = 0;
    return WriteFile(windows_io->file_handle, data, (DWORD)strlen(data), &bytesWritten, NULL);
}


typedef struct Windows_File_Data
{
    const char* file_name;
    FILETIME last_write_time;
    HANDLE file_windows_handle;
} Windows_File_Data;

typedef struct Windows_Directory_Data
{
    const char* directory_path;
    HANDLE directory_handle;
    OVERLAPPED overlapped;

    BYTE buffer[64 * 1024];

    File_Watch_Event event_queue[256];

    u32 queue_read;
    u32 queue_write;


    bool pending;
} Windows_Directory_Data;


Windows_File_Data windows_file_data[1000];
static int windows_file_count = 1;


Windows_Directory_Data windows_directory_data[1000];
static int windows_directory_count = 1;

// RING_QUEUE_TYPE(File_Watch_Event) queued_file_events;


File_Watch_Handle platform_register_file_watch(const char* file_name)
{
    WIN32_FILE_ATTRIBUTE_DATA file_info;
    if (!GetFileAttributesExA(file_name, GetFileExInfoStandard, &file_info))
        return (File_Watch_Handle){0};

    //write the file time
    Windows_File_Data* file_data = &windows_file_data[windows_file_count];
    file_data->file_name = file_name;
    file_data->last_write_time = file_info.ftLastWriteTime;

    //hand out the handle
    File_Watch_Handle out_handle = {windows_file_count};
    windows_file_count++;

    return out_handle;
}

bool platform_has_filed_changed(File_Watch_Handle file_watch_handle)
{
    Windows_File_Data* file_data = &windows_file_data[file_watch_handle.handle];

    WIN32_FILE_ATTRIBUTE_DATA file_info;
    if (!GetFileAttributesExA(file_data->file_name, GetFileExInfoStandard, &file_info))
        return false;
    //an alternative way of doing this
    // WIN32_FIND_DATA find_data;
    // FindFirstFileA(filename, &find_data);
    // find_data.ftLastWriteTime

    if (CompareFileTime(&file_info.ftLastWriteTime, &file_data->last_write_time) != 0)
    {
        file_data->last_write_time = file_info.ftLastWriteTime;
        return true;
    }
    return false;
}


File_Watch_Handle platform_register_directory_watch(const char* directory_name)
{
    HANDLE dir_win_handle = CreateFileA(
        directory_name,
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ |
        FILE_SHARE_WRITE |
        FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
        NULL
    );

    if (dir_win_handle == INVALID_HANDLE_VALUE)
    {
        M_ERROR("Failed to open directory\n");
        return (File_Watch_Handle){0};
    }

    //get a file struct
    Windows_Directory_Data* directory_data = &windows_directory_data[windows_directory_count];
    directory_data->directory_path = c_string_duplicate(directory_name); // TODO: allocator
    directory_data->directory_handle = dir_win_handle;

    directory_data->overlapped.hEvent =
        CreateEventA(
            NULL,
            TRUE,
            FALSE,
            NULL
        );

    if (!directory_data->overlapped.hEvent)
    {
        CloseHandle(directory_data->directory_handle);
        M_ERROR("Failed to create overlap event");
        return (File_Watch_Handle){0};
    }


    File_Watch_Handle out_handle = {windows_directory_count};
    windows_directory_count++;

    return out_handle;
}


bool platform_update_directory(File_Watch_Handle directory_watch_handle)
{
    Windows_Directory_Data* file_data =
        &windows_directory_data[directory_watch_handle.handle];

    DWORD bytes_transferred;

    /*
     * Start a new asynchronous directory read.
     */
    if (!file_data->pending)
    {
        ResetEvent(file_data->overlapped.hEvent);

        BOOL result =
            ReadDirectoryChangesW(
                file_data->directory_handle,
                file_data->buffer,
                sizeof(file_data->buffer),
                TRUE,

                FILE_NOTIFY_CHANGE_FILE_NAME |
                FILE_NOTIFY_CHANGE_DIR_NAME |
                FILE_NOTIFY_CHANGE_LAST_WRITE |
                FILE_NOTIFY_CHANGE_SIZE,

                NULL,
                &file_data->overlapped,
                NULL
            );

        if (!result)
        {
            DWORD error = GetLastError();
            M_ERROR("PLATFORM UPDATE DIRECTORY: ReadDirectoryChangesW RESULT %d", error);
            if (error != ERROR_IO_PENDING)
                return false;
        }

        file_data->pending = true;

        return true;
    }

    /*
     * Check if the asynchronous read has completed.
     * FALSE means do not block.
     */
    BOOL result =
        GetOverlappedResult(
            file_data->directory_handle,
            &file_data->overlapped,
            &bytes_transferred,
            FALSE
        );

    if (!result)
    {
        DWORD error = GetLastError();

        if (error == ERROR_IO_INCOMPLETE)
            return true;

        file_data->pending = false;

        return false;
    }

    file_data->pending = false;

    if (bytes_transferred == 0)
        return true;

    /*
     * Parse all FILE_NOTIFY_INFORMATION records.
     */
    DWORD offset = 0;

    while (offset < bytes_transferred)
    {
        FILE_NOTIFY_INFORMATION* info =
            (FILE_NOTIFY_INFORMATION*)
            (file_data->buffer + offset);

        File_Watch_Event event = {0};

        switch (info->Action)
        {
        case FILE_ACTION_ADDED:
            event.action = FILE_WATCH_ACTION_ADDED;
            break;

        case FILE_ACTION_REMOVED:
            event.action = FILE_WATCH_ACTION_REMOVED;
            break;

        case FILE_ACTION_MODIFIED:
            event.action = FILE_WATCH_ACTION_MODIFIED;
            break;

        case FILE_ACTION_RENAMED_OLD_NAME:
            event.action = FILE_WATCH_ACTION_RENAMED;
            break;

        case FILE_ACTION_RENAMED_NEW_NAME:
            event.action = FILE_WATCH_ACTION_RENAMED_NEW;
            break;

        default:
            goto next_event;
        }

        int filename_length =
            WideCharToMultiByte(
                CP_UTF8,
                0,
                info->FileName,
                info->FileNameLength / sizeof(WCHAR),
                event.path,
                sizeof(event.path) - 1,
                NULL,
                NULL
            );

        if (filename_length > 0)
        {
            event.path[filename_length] = '\0';

            u32 next_write =
                (file_data->queue_write + 1) %
                ARRAY_SIZE(file_data->event_queue);

            /*
             * Queue is not full.
             */
            if (next_write != file_data->queue_read)
            {
                file_data->event_queue[
                    file_data->queue_write
                ] = event;

                file_data->queue_write =
                    next_write;
            }
        }

    next_event:

        if (info->NextEntryOffset == 0)
            break;

        offset += info->NextEntryOffset;
    }

    return true;
}

bool platform_poll_directory_changes(
    File_Watch_Handle directory_watch_handle,
    File_Watch_Event* out_event)
{
    Windows_Directory_Data* file_data =
        &windows_directory_data[directory_watch_handle.handle];


    if (file_data->queue_read ==
        file_data->queue_write)
    {
        return false;
    }

    *out_event =
        file_data->event_queue[
            file_data->queue_read
        ];

    file_data->queue_read =
        (file_data->queue_read + 1) %
        ARRAY_SIZE(file_data->event_queue);

    return true;
}


bool platform_has_directory_changed(File_Watch_Handle directory_watch_handle)
{
    //get a file struct
    Windows_File_Data* file_data = &windows_file_data[directory_watch_handle.handle];

    char buffer[64 * 1024];
    DWORD bytesReturned;

    b32 result = ReadDirectoryChangesW(
        file_data->file_windows_handle,
        buffer,
        sizeof(buffer),
        TRUE, // watch subdirectories
        FILE_NOTIFY_CHANGE_FILE_NAME |
        FILE_NOTIFY_CHANGE_DIR_NAME |
        FILE_NOTIFY_CHANGE_LAST_WRITE |
        FILE_NOTIFY_CHANGE_SIZE,
        &bytesReturned,
        NULL,
        NULL);

    if (!result) { return false; }

    FILE_NOTIFY_INFORMATION* info = (FILE_NOTIFY_INFORMATION*)buffer;
    for (;;)
    {
        wchar_t path[MAX_PATH];

        DWORD length = info->FileNameLength / sizeof(wchar_t);

        memcpy(path, info->FileName, info->FileNameLength);
        path[length] = L'\0';

        switch (info->Action)
        {
        case FILE_ACTION_ADDED:
            // queue added
            break;

        case FILE_ACTION_REMOVED:
            // queue removed
            break;

        case FILE_ACTION_MODIFIED:
            // queue modified
            break;

        case FILE_ACTION_RENAMED_OLD_NAME:
            // old name
            break;

        case FILE_ACTION_RENAMED_NEW_NAME:
            // new name
            break;
        }

        if (info->NextEntryOffset == 0)
            break;

        info = (FILE_NOTIFY_INFORMATION*)
            ((BYTE*)info + info->NextEntryOffset);
    }

    return true;
}


bool platform_does_file_exist(const char* file_path)
{
    DWORD attrib = GetFileAttributesA(file_path);

    // Check if the path is valid and ensure it is not a directory
    return (attrib != INVALID_FILE_ATTRIBUTES && !(attrib & FILE_ATTRIBUTE_DIRECTORY));
}

bool platform_does_directory_exist(const char* directory_path)
{
    DWORD dwAttrib = GetFileAttributesA(directory_path);

    // Check if the path is valid and matches a directory attribute
    return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
        (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}


bool platform_create_directory(const char* directory_path)
{
    if (CreateDirectoryA(directory_path, NULL))
    {
        DEBUG("filesystem_create_directory WIN32: successfully created directory");
        return true;
    }
    DWORD error = GetLastError();
    if (error == ERROR_ALREADY_EXISTS)
    {
        DEBUG("filesystem_create_directory WIN32: directory already created");
        return true;
    }

    FATAL("filesystem_create_directory WIN32: DID NOT created directory, Reason %d", GetLastError());
    return false;
}

bool platform_create_directory_recursive(const char* directory_path)
{
    char buffer[MAX_PATH];
    strcpy_s(buffer, sizeof(buffer), directory_path);

    for (char* p = buffer; *p; p++)
    {
        if (*p == '/' || *p == '\\')
        {
            char separator = *p;
            *p = '\0';

            if (buffer[0] != '\0')
            {
                if (!CreateDirectoryA(buffer, NULL))
                {
                    DWORD error = GetLastError();

                    if (error != ERROR_ALREADY_EXISTS)
                    {
                        FATAL(
                            "Failed creating directory '%s', error %lu",
                            buffer,
                            error);
                        return false;
                    }
                }
            }

            *p = separator;
        }
    }

    if (!CreateDirectoryA(buffer, NULL))
    {
        DWORD error = GetLastError();

        if (error != ERROR_ALREADY_EXISTS)
        {
            FATAL(
                "Failed creating directory '%s', error %lu",
                buffer,
                error);
            return false;
        }
    }

    return true;


}

bool platform_create_file(const char* file_path)
{
    HANDLE handle = CreateFileA(
        file_path,
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_NEW, // Fails if file exists
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (handle == INVALID_HANDLE_VALUE)
    {
        WARN("PLATFORM CREATE FILE WIN32: Error creating file %d", GetLastError());
        if (GetLastError() == ERROR_FILE_EXISTS)
        {
            printf("File already exists\n");
            return 1; // File exists - not an error
        }
        ;
        return 0;
    }

    CloseHandle(handle);
    printf("File created\n");
    return 1;
}

bool filesystem_scan_directory(const char* directory_path)
{
    WIN32_FIND_DATAA findFileData;
    HANDLE findHandle;
    char search_path[256];

    // Create search pattern: "directory/*"
    sprintf(search_path, "%s/*", directory_path);

    findHandle = FindFirstFileA(search_path, &findFileData);

    if (findHandle == INVALID_HANDLE_VALUE)
    {
        printf("Error: Could not open directory %s\n", directory_path);
        return false;
    }

    do
    {
        // Skip "." and ".."
        if (strcmp(findFileData.cFileName, ".") == 0 ||
            strcmp(findFileData.cFileName, "..") == 0)
        {
            continue;
        }

        // Check if it's a file or directory
        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            printf("[DIR]  %s\n", findFileData.cFileName);
        }
        else
        {
            printf("[FILE] %s (%ld bytes)\n",
                   findFileData.cFileName,
                   findFileData.nFileSizeLow);
        }
    }
    while (FindNextFileA(findHandle, &findFileData));

    FindClose(findHandle);
    return true;
}

bool filesystem_scan_directory_recursive(const char* directory_path)
{
    WIN32_FIND_DATAA findFileData;
    HANDLE findHandle;
    char search_path[MAX_PATH];
    char full_path[MAX_PATH];

    if (snprintf(search_path, sizeof(search_path), "%s/*", directory_path) >= (int)sizeof(search_path))
    {
        printf("Error: path too long: %s\n", directory_path);
        return false;
    }

    findHandle = FindFirstFileA(search_path, &findFileData);

    if (findHandle == INVALID_HANDLE_VALUE)
    {
        printf("Error: Could not open directory %s\n", directory_path);
        return false;
    }

    do
    {
        // Skip "." and ".."
        if (strcmp(findFileData.cFileName, ".") == 0 ||
            strcmp(findFileData.cFileName, "..") == 0)
        {
            continue;
        }

        if (snprintf(full_path, sizeof(full_path), "%s/%s", directory_path, findFileData.cFileName)
            >= (int)sizeof(full_path))
        {
            printf("Error: path too long, skipping %s\n", findFileData.cFileName);
            continue;
        }

        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
            {
                printf("[LINK] %s (skipped)\n", full_path);
                continue;
            }

            printf("[DIR]  %s\n", full_path);
            filesystem_scan_directory_recursive(full_path); // recurse into it
        }
        else
        {
            printf("[FILE] %s (%ld bytes)\n", full_path, findFileData.nFileSizeLow);
        }
    }
    while (FindNextFileA(findHandle, &findFileData));

    FindClose(findHandle);
    return true;
}

bool platform_get_assets_from_directory(const char* directory_path, Asset_List_Scan* asset_list_scan)
{
    MASSERT(asset_list_scan)
    MASSERT(asset_list_scan->allocator)
    MASSERT(asset_list_scan->strings)

    WIN32_FIND_DATAA findFileData;
    HANDLE findHandle;
    char search_path[MAX_PATH];
    char full_path[MAX_PATH];

    if (snprintf(search_path, sizeof(search_path), "%s/*", directory_path) >= sizeof(search_path))
    {
        printf("Error: path too long: %s\n", directory_path);
        return false;
    }

    findHandle = FindFirstFileA(search_path, &findFileData);

    if (findHandle == INVALID_HANDLE_VALUE)
    {
        WARN("filesystem_get_assets_from_directory: Could not open directory %s\n", directory_path);
        return false;
    }

    do
    {
        // Skip "." and ".."
        if (strcmp(findFileData.cFileName, ".") == 0 ||
            strcmp(findFileData.cFileName, "..") == 0)
        {
            continue;
        }

        if (snprintf(full_path, sizeof(full_path), "%s/%s", directory_path, findFileData.cFileName)
            >= sizeof(full_path))
        {
            WARN("Error: path too long, skipping %s\n", findFileData.cFileName);
            continue;
        }

        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
            {
                INFO("[LINK] %s (skipped)", full_path);
                continue;
            }

            INFO("[DIR]  %s", full_path);
            // recurse into the directory to find the other files in it
            platform_get_assets_from_directory(full_path, asset_list_scan);
        }
        else if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
        {
            String* string = &asset_list_scan->strings[asset_list_scan->count++];
            string->length = strlen(full_path);
            string->chars = c_string_duplicate_allocator(full_path, asset_list_scan->allocator);
            INFO("[FILE] %s (%ld bytes)", full_path, findFileData.nFileSizeLow);
        }
        else
        {
            WARN("[OTHER] %s (%ld bytes)", full_path, findFileData.nFileSizeLow);
        }
    }
    while (FindNextFileA(findHandle, &findFileData));

    FindClose(findHandle);
    return true;
}


bool platform_is_directory_empty(const char* directory_path)
{
    WIN32_FIND_DATAA findFileData;
    HANDLE findHandle;
    char search_path[256];

    sprintf(search_path, "%s/*", directory_path);

    findHandle = FindFirstFileA(search_path, &findFileData);

    if (findHandle == INVALID_HANDLE_VALUE)
    {
        return true; // Directory doesn't exist or is empty
    }

    do
    {
        // If we find anything other than "." and "..", it's not empty
        if (strcmp(findFileData.cFileName, ".") != 0 &&
            strcmp(findFileData.cFileName, "..") != 0)
        {
            FindClose(findHandle);
            return false; // Not empty
        }
    }
    while (FindNextFileA(findHandle, &findFileData));

    FindClose(findHandle);
    return true; // Empty
}


bool platform_file_open_async()
{
    return false;
}


#endif
