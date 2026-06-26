#include "filesystem.h"


#if MPLATFORM_WINDOWS



//FILE SYSTEM
typedef struct Windows_File_Data
{
    const char* file_name;
    FILETIME last_write_time;
    HANDLE directory_windows_handle;

} Windows_File_Data;

Windows_File_Data windows_file_data[1000];
static int windows_file_count = 1;


File_Watch_Handle platform_register_file_watch(const char* file_name)
{
    WIN32_FILE_ATTRIBUTE_DATA file_info;
    if (!GetFileAttributesExA(file_name, GetFileExInfoStandard, &file_info))
        return (File_Watch_Handle){0, file_name};

    //write the file time
    Windows_File_Data* file_data = &windows_file_data[windows_file_count];
    file_data->file_name = file_name;
    file_data->last_write_time = file_info.ftLastWriteTime;

    //hand out the handle
    File_Watch_Handle out_handle = {windows_file_count, file_name};
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

    HANDLE dir = CreateFileA(
        directory_name,
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS,
        NULL
    );

    //get a file struct
    Windows_File_Data* file_data = &windows_file_data[windows_file_count];
    file_data->file_name = directory_name;
    file_data->directory_windows_handle = dir;

    if (dir == INVALID_HANDLE_VALUE)
    {
        M_ERROR("Failed to open directory\n");
        return (File_Watch_Handle){0, directory_name};
    }

    File_Watch_Handle out_handle = {windows_file_count, directory_name};
    windows_file_count++;

    return out_handle;
}

void platform_has_directory_changed(File_Watch_Handle directory_watch_handle)
{
    //get a file struct
    Windows_File_Data* file_data = &windows_file_data[directory_watch_handle.handle];

    char buffer[2048];
    DWORD bytesReturned;

    if (ReadDirectoryChangesW(
            file_data->directory_windows_handle,
            buffer,
            sizeof(buffer),
            TRUE, // watch subdirectories
            FILE_NOTIFY_CHANGE_FILE_NAME |
            FILE_NOTIFY_CHANGE_DIR_NAME |
            FILE_NOTIFY_CHANGE_LAST_WRITE,
            &bytesReturned,
            NULL,
            NULL))
    {
        FILE_NOTIFY_INFORMATION* info = (FILE_NOTIFY_INFORMATION*)buffer;

        do
        {
            wprintf(L"Changed: %.*s\n",
                    info->FileNameLength / 2,
                    info->FileName);

            if (info->NextEntryOffset == 0)
                break;

            info = (FILE_NOTIFY_INFORMATION*)((char*)info + info->NextEntryOffset);

        } while (1);
    }
}


bool filesystem_does_file_exists(const char* file_path)
{
    DWORD attrib = GetFileAttributesA(file_path);

    // Check if the path is valid and ensure it is not a directory
    return (attrib != INVALID_FILE_ATTRIBUTES && !(attrib & FILE_ATTRIBUTE_DIRECTORY));
}

bool filesystem_does_directory_exists(const char* directory_path)
{
    DWORD dwAttrib = GetFileAttributesA(directory_path);

    // Check if the path is valid and matches a directory attribute
    return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
           (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}


bool filesystem_create_directory(const char* directory_path)
{
    if (CreateDirectory(directory_path, NULL))
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

    FATAL("filesystem_create_directory WIN32: DID NOT created directory");
    return false;
}

bool filesystem_create_file_platform(const char* file_path)
{
    HANDLE handle = CreateFileA(
        file_path,
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_NEW,           // Fails if file exists
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (handle == INVALID_HANDLE_VALUE) {
        if (GetLastError() == ERROR_FILE_EXISTS) {
            printf("File already exists\n");
            return 1;  // File exists - not an error
        }
        printf("Error creating file\n");
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

    if (findHandle == INVALID_HANDLE_VALUE) {
        printf("Error: Could not open directory %s\n", directory_path);
        return false;
    }

    do {
        // Skip "." and ".."
        if (strcmp(findFileData.cFileName, ".") == 0 ||
            strcmp(findFileData.cFileName, "..") == 0) {
            continue;
            }

        // Check if it's a file or directory
        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            printf("[DIR]  %s\n", findFileData.cFileName);
        } else {
            printf("[FILE] %s (%ld bytes)\n",
                   findFileData.cFileName,
                   findFileData.nFileSizeLow);
        }

    } while (FindNextFileA(findHandle, &findFileData));

    FindClose(findHandle);
    return true;
}




bool filesystem_is_directory_empty(const char* directory_path) {
    WIN32_FIND_DATAA findFileData;
    HANDLE findHandle;
    char search_path[256];

    sprintf(search_path, "%s/*", directory_path);

    findHandle = FindFirstFileA(search_path, &findFileData);

    if (findHandle == INVALID_HANDLE_VALUE) {
        return true;  // Directory doesn't exist or is empty
    }

    do {
        // If we find anything other than "." and "..", it's not empty
        if (strcmp(findFileData.cFileName, ".") != 0 &&
            strcmp(findFileData.cFileName, "..") != 0) {
            FindClose(findHandle);
            return false;  // Not empty
            }
    } while (FindNextFileA(findHandle, &findFileData));

    FindClose(findHandle);
    return true;  // Empty
}



#endif
