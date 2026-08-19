#pragma once

#include "defines.h"
#include "platform.h"

//NEW API

typedef enum file_modes {
    FILE_MODE_READ,
    FILE_MODE_WRITE,
    FILE_MODE_READ_WRITE,
    FILE_MODE_WRITE_APPEND,
    FILE_MODE_READ_WRITE_APPEND
} File_Modes;

typedef enum File_State {
    FILE_STATE_NONE,
    FILE_STATE_FAILED_OPEN,
    FILE_STATE_OPEN,
    FILE_STATE_CLOSED,
} File_State;


typedef struct CFile
{
    File_State file_state;
    File_Modes file_mode;
    FILE* fptr; // might need to be void* platform_specifics;
} CFile;



///// Standard C File Operations ////
bool filesystem_open(const char* path, File_Modes mode, bool binary, CFile* out_file);
bool filesystem_close(CFile* file);
bool filesystem_read(CFile* file, u64 data_size, void* out_data);
bool filesystem_write(CFile* file, void* data, u64 data_size);
bool filesystem_read_all_bytes(CFile* file, u8** out_bytes, u64* out_bytes_read);
bool filesystem_read_line(CFile* file, u8* buffer, u64 buffer_size);
u64 filesystem_get_file_size(CFile* file);


///// platform based synchronize operations ////
/* NOTE: these are probably not needed
bool filesystem_open_platform(const char* path, File_Modes mode, bool binary, Madness_File_Platform* out_file/*, bool use_os#1#);
bool filesystem_close_platform(Madness_File_Platform* file);
bool filesystem_read_platform(Madness_File_Platform* handle, u64 data_size, void* out_data, u64* out_bytes_read);
bool filesystem_write_platform(Madness_File_Platform* handle, u64 data_size, void* out_data, u64* out_bytes_read);
bool filesystem_read_all_bytes_platform(Madness_File_Platform* handle, u8** out_bytes, u64* out_bytes_read);
*/


///// async operations ////
// these are implied to be platform based operations
typedef struct Madness_File_Platform
{
    File_State file_state;
    File_Modes file_mode;
    // FILE* fptr; // might need to be void* platform_specifics;
    void* internal_data;
} Madness_File_Platform;

bool filesystem_open_aync(Madness_File_Platform* handle, u64 data_size, void* out_data, u64* out_bytes_read);
bool filesystem_close_async(Madness_File_Platform* handle, u64 data_size, void* out_data, u64* out_bytes_read);
bool filesystem_read_async(Madness_File_Platform* handle, u64 data_size, void* out_data, u64* out_bytes_read);
// bool filesystem_write_async(Madness_File_Platform* handle, u64 data_size, void* out_data, u64* out_bytes_read);




//Old stuff
typedef struct file_read_data
{
    u8* data;
    u64 size;
} file_read_data;

void file_read_data_free(file_read_data* file_data);
bool filesystem_open_and_return_bytes(const char* file_path, file_read_data* file_data);
bool filesystem_open_and_get_file_size(const char* file_path, u64* out_file_size);
void filesystem_go_to_start(FILE* fptr);
size_t filesystem_file_size(FILE* fptr);
bool filesystem_create_file(const char* file_path);




//FILE UTILITY
bool filesystem_does_file_exists(const char* file_path);
bool filesystem_does_directory_exists(const char* directory_path);


bool filsystem_create_file(const char* file_path);
bool filsystem_create_directory(const char* file_path);
bool filesystem_create_directory_recursive(const char* directory_path);
bool platform_create_directory_recursive(const char* directory_path);


bool filesystem_scan_directory(const char* directory_path);
bool filesystem_scan_directory_recursive(const char* directory_path);

typedef struct Asset_List_Scan
{
    u32 max_count;
    u32 count;
    String* strings;
    Allocator* allocator;
}Asset_List_Scan;


Asset_List_Scan* asset_lists_generate(Memory_System* memory_system, u32 max_asset_count,
                                      const char* relative_asset_path);
bool asset_lists_free(Asset_List_Scan* asset_list_scan, Memory_System* memory_system);



//FILE WATCHER

typedef struct File_Watch_Handle
{
    uint64_t handle;
} File_Watch_Handle;

//returns a file handle, 0 if an invalid file, but should be safe since it won't crash anything
File_Watch_Handle filesystem_register_file(const char* file_path);
void filesystem_unregister_file_by_handle(File_Watch_Handle file_handle);
void filesystem_unregister_file_by_name(const char* file_path);

bool filesystem_has_file_changed(File_Watch_Handle file_handle);
bool filesystem_has_directory_changed(File_Watch_Handle file_handle);



typedef enum File_Watch_Action
{
    FILE_WATCH_ACTION_ADDED,
    FILE_WATCH_ACTION_REMOVED,
    FILE_WATCH_ACTION_MODIFIED,
    FILE_WATCH_ACTION_RENAMED,
    FILE_WATCH_ACTION_RENAMED_OLD,
    FILE_WATCH_ACTION_RENAMED_NEW,
}File_Watch_Action;

typedef struct File_Watch_Event
{
    File_Watch_Action action;
    char path[260];
} File_Watch_Event;

typedef struct Madness_File_Watcher
{
    Allocator* allocator;

    File_Watch_Handle directory_handles[32]; // abritrary amount
    u32 directory_handle_count; // abritrary amount

    void* platform_internals;
} Madness_File_Watcher;


static Madness_File_Watcher* madness_file_watcher;

bool filewatcher_init(Memory_System* memory_system);
bool filewatcher_deinit(void);
bool filewatcher_update(void);

void filewatcher_directory_register(const char* directory_path);
void filewatcher_directory_unregister(const char* directory_path);