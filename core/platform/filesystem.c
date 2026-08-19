#include "filesystem.h"


bool filesystem_open(const char* path, File_Modes mode, bool binary, CFile* file)
{
    file->file_mode = mode;
    file->file_state = FILE_STATE_NONE;

    const char* file_mode;
    switch (mode)
    {
    case FILE_MODE_READ:
        file_mode = "r";
        break;
    case FILE_MODE_WRITE:
        file_mode = "w";
        break;
    case FILE_MODE_READ_WRITE:
        //open for read write and at the begining. does not create file
        // mode[0] = "r+";

        //open for r/w. file is created if not found
        file_mode = "w+";
        break;
    case FILE_MODE_WRITE_APPEND:
        file_mode = "a";
        break;
    case FILE_MODE_READ_WRITE_APPEND:
        //file also created if does not exist
        file_mode = "a+";
        break;
    }

    MASSERT(file_mode);


    file->fptr = fopen(path, file_mode);

    if (!file->fptr)
    {
        file->file_state = FILE_STATE_FAILED_OPEN;
        MASSERT(false); // TODO: should probably remove later
        return false;
    }
    file->file_state = FILE_STATE_OPEN;
    return true;
}

bool filesystem_close(CFile* file)
{
    if (file->file_state != FILE_STATE_OPEN)
    {
        WARN("FILE SYSTEM CLOSE: TRYING TO CLOSE A FILE THAT IS NOT VALID")
        return false;
    }
    fclose(file->fptr);
    return true;
}

bool filesystem_read(CFile* file, u64 data_size, void* out_data)
{
    if (file->file_state != FILE_STATE_OPEN)
    {
        return false;
    }

    if (file->file_mode != FILE_MODE_READ ||
        file->file_mode != FILE_MODE_READ_WRITE)
    {
        return false;
    }


    fread(out_data, data_size, 1, file->fptr);
    return true;
}

bool filesystem_write(CFile* file, void* data, u64 data_size)
{
    if (file->file_state != FILE_STATE_OPEN)
    {
        return false;
    }

    if (file->file_mode != FILE_MODE_WRITE ||
        file->file_mode != FILE_MODE_WRITE_APPEND ||
        file->file_mode != FILE_MODE_READ_WRITE)
    {
        return false;
    }


    fwrite(data, data_size, 1, file->fptr);
    return true;
}



bool filesystem_read_all_bytes(CFile* file, u8** out_bytes, u64* out_bytes_read)
{
    if (file->file_state != FILE_STATE_OPEN)
    {
        return false;
    }

    u64 file_size = filesystem_get_file_size(file);
    fread(out_bytes, file_size, 1, file->fptr);

    *out_bytes_read = file_size;
    return true;
}

bool filesystem_read_line(CFile* file, u8* buffer, u64 buffer_size)
{
    fgets((char*)buffer,  buffer_size, file->fptr);
    return true;
}

u64 filesystem_get_file_size(CFile* file)
{
    fseek(file->fptr, 0, SEEK_END);
    const size_t size = ftell(file->fptr);
    fseek(file->fptr, 0, SEEK_SET);
    return size;
}


//old
void file_read_data_free(file_read_data* file_data)
{
    free(file_data->data);
}

bool filesystem_open_and_return_bytes(const char* file_path, file_read_data* file_data)
{
    //TODO: use an arena
    FILE* fptr = fopen(file_path, "rb");
    if (!fptr)
    {
        WARN("FILE OPEN AND RETURN BYTES: FAILED TO OPEN FILE %s", file_path);
        return false;
    }

    // go to the end of the file and get the size
    fseek(fptr, 0, SEEK_END);
    file_data->size = ftell(fptr);
    file_data->data = malloc(file_data->size);
    rewind(fptr); // goes to the start of the file

    fread(file_data->data, 1, file_data->size, fptr);

    fclose(fptr);

    return true;
}

bool filesystem_open_and_get_file_size(const char* file_path, u64* out_file_size)
{
    FILE* fptr = fopen(file_path, "rb");
    if (!fptr)
    {
        WARN("FILE SIZE: FAILED TO OPEN FILE %s", file_path);
        return false;
    }

    // go to the end of the file and get the size
    fseek(fptr, 0, SEEK_END);
    *out_file_size = ftell(fptr);

    fclose(fptr);

    return true;
}

void filesystem_go_to_start(FILE* fptr)
{
    fseek(fptr, 0, SEEK_SET);
}

size_t filesystem_file_size(FILE* fptr)
{
    MASSERT(fptr);
    fseek(fptr, 0, SEEK_END);
    const size_t size = ftell(fptr);
    fseek(fptr, 0, SEEK_SET);
    return size;
}

bool filesystem_create_file(const char* file_path)
{
    FILE* fptr = fopen(file_path, "r");

    if (fptr)
    {
        //file exists
        fclose(fptr);
        return true;
    }

    fptr = fopen(file_path, "w");
    if (fptr)
    {
        fclose(fptr);
        return true;
    }


    return false;
}

bool filesystem_does_file_exists(const char* file_path)
{
    return platform_does_file_exist(file_path);
}

bool filesystem_does_directory_exists(const char* directory_path)
{
    return platform_does_directory_exist(directory_path);
}

bool filsystem_create_directory(const char* file_path)
{
    return platform_create_directory(file_path);
}

bool filesystem_create_directory_recursive(const char* directory_path)
{
    if (directory_path == NULL || directory_path[0] == '\0')
    {
        return false;
    }

    size_t len = strlen(directory_path);
    char buffer[4096];
    if (len >= sizeof(buffer))
    {
        return false;
    }
    memcpy(buffer, directory_path, len + 1);

    size_t start = 0;

    // Skip drive letter prefix on Windows-style paths, e.g. "C:\" or "C:/"
    if (len >= 2 && buffer[1] == ':')
    {
        start = 2;
    }

    for (size_t i = start; i < len; i += 1)
    {
        if (buffer[i] == '/' || buffer[i] == '\\')
        {
            if (i == 0) { continue; }  // leading separator, nothing to create yet

            char saved = buffer[i];
            buffer[i] = '\0';

            if (!platform_create_directory(buffer))
            {
                return false;
            }

            buffer[i] = saved;
        }
    }

    return platform_create_directory(buffer);
}


Asset_List_Scan* asset_lists_generate(Memory_System* memory_system, u32 max_asset_count,
                                      const char* relative_asset_path)
{
    Asset_List_Scan* asset_list = memory_system_alloc(memory_system,
                                                      sizeof(Asset_List_Scan),
                                                      MEMORY_SUBSYSTEM_RESOURCE);
    asset_list->allocator = memory_system_allocator_create(memory_system,
                                                           (sizeof(String) * max_asset_count) + (256 /*string count*/
                                                               * max_asset_count),
                                                           MEMORY_SUBSYSTEM_RESOURCE);
    asset_list->strings = allocator_alloc(asset_list->allocator,
                                          sizeof(String) * max_asset_count);
    asset_list->max_count = max_asset_count;

    platform_get_assets_from_directory(relative_asset_path, asset_list);

    return asset_list;
}

bool asset_lists_free(Asset_List_Scan* asset_list_scan, Memory_System* memory_system)
{
    memory_system_allocator_free(memory_system, asset_list_scan->allocator, MEMORY_SUBSYSTEM_RESOURCE);
    memory_system_memory_free(memory_system, asset_list_scan, MEMORY_SUBSYSTEM_RESOURCE);
    return true;
}


File_Watch_Handle filesystem_register_file(const char* file_path)
{
    return platform_register_file_watch(file_path);
}

bool filesystem_has_file_changed(File_Watch_Handle file_handle)
{
    return platform_has_filed_changed(file_handle);
}

bool filesystem_has_directory_changed(File_Watch_Handle file_handle)
{
    return platform_has_directory_changed(file_handle);
}

///////////////// FILE WATCHER /////////////////


bool filewatcher_init(Memory_System* memory_system)
{
    madness_file_watcher = memory_system_alloc(memory_system, sizeof(Madness_File_Watcher), MEMORY_SUBSYSTEM_MISC);
    madness_file_watcher->allocator = memory_system_allocator_create(memory_system, KB(64), MEMORY_SUBSYSTEM_MISC);

    MASSERT(madness_file_watcher);
    return true;
}

bool filewatcher_update(void)
{
    MASSERT(madness_file_watcher);
    for (int i = 0; i < madness_file_watcher->directory_handle_count; ++i)
    {
        platform_update_directory(madness_file_watcher->directory_handles[i]);

        File_Watch_Event event;

        while (platform_poll_directory_changes(
            madness_file_watcher->directory_handles[i],
            &event))
        {
            // Process event
            INFO("FILE EVENT: %s %d", event.path, event.action)
            //TODO: send out an event
        }
    }


    return true;
}


bool filewatcher_deinit(void)
{
    MASSERT(madness_file_watcher);
    return true;
}


void filewatcher_directory_register(const char* directory_path)
{
    MASSERT(madness_file_watcher);

    File_Watch_Handle* handle = &madness_file_watcher->directory_handles[madness_file_watcher->directory_handle_count
        ++];
    *handle = platform_register_directory_watch(directory_path);
}

void filewatcher_directory_unregister(const char* directory_path)
{
    //TODO:
    MASSERT_FALSE();
}


bool filewatcher_poll(Madness_File_Watcher* watcher, File_Watch_Event event)
{
}
