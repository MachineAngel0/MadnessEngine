#ifndef PLATFORM_H
#define PLATFORM_H


#include "vk_device.h"

typedef struct Platform_Config
{
    // Window starting position x axis, if applicable.
    s16 start_pos_x;

    // Window starting position y axis, if applicable.
    s16 start_pos_y;

    // Window starting width, if applicable.
    s16 start_width;

    // Window starting height, if applicable.
    s16 start_height;

    // The application name used in windowing, if applicable.
    char* name;
} Platform_Config;

void platform_config_use_defaults(Platform_Config* config)
{
    config->start_pos_x = 100;
    config->start_pos_y = 100;
    // config->start_width = 1280;
    // config->start_height = 720;

    config->start_width = 1920;
    config->start_height = 1080;

    config->name = "Madness Engine";
}


typedef struct Platform_State
{
    void* internal_state;
} Platform_State;


bool platform_startup(
    Platform_State* plat_state,
    Platform_Config platform_config);


void platform_shutdown(Platform_State* plat_state);


bool platform_pump_messages(Platform_State* plat_state);


//AUDIO
bool platform_audio_init(Platform_State* plat_state, int32_t buffer_size, int32_t samples_per_second);

bool platform_audio_shutdown(Platform_State* plat_state);

//MEMORY
void* platform_reserve_memory(u64 size, bool aligned);
void* platform_allocate(u64 size, bool aligned);

void platform_free(void* block);

void* platform_zero_memory(void* block, u64 size);

void* platform_copy_memory(void* dest, const void* source, u64 size);

void* platform_set_memory(void* dest, s32 value, u64 size);

//TIME
f64 platform_get_absolute_time(void);

// Sleep on the thread for the provided ms. This blocks the main thread.
// Should only be used for giving time back to the OS for unused update power.
// Therefore it is not exported.
void platform_sleep(u64 ms);

typedef struct DLL_HANDLE
{
    uint64_t handle;
    const char* file_name;
} DLL_HANDLE;




//DLL
char* platform_get_dynamic_library_extension(void);
char* platform_get_static_library_extension(void);

DLL_HANDLE platform_load_dynamic_library(const char* file_name);
bool platform_unload_dynamic_library(DLL_HANDLE handle);
bool platform_load_dynamic_library_from_handle(DLL_HANDLE handle, const char* file_name);
bool platform_reload_dynamic_library(DLL_HANDLE handle);

//has to get cast into the function pointer
void* platform_get_function_address(DLL_HANDLE handle, const char* function_name);

//FILE SYSTEM
File_Watch_Handle platform_register_file_watch(const char* file_name); // registers a file to be watched
// bool platform_unregister_file_watch(File_Handle file_handle); // remove from the list

bool platform_has_filed_changed(File_Watch_Handle file_watch_handle); // checks if that file has been changed

File_Watch_Handle platform_register_directory_watch(const char* directory_name);
bool platform_has_directory_changed(File_Watch_Handle directory_watch_handle);


bool platform_open_file_dialogue(char* out_path, char* start_file_absolute_path);


bool platform_update_directory(File_Watch_Handle directory_watch_handle);
bool platform_poll_directory_changes(File_Watch_Handle directory_watch_handle, File_Watch_Event* out_event);
bool platform_is_directory_empty(const char* directory_path);
bool platform_get_assets_from_directory(const char* directory_path, Asset_List_Scan* asset_list_scan);


//FILE SYSTEM
bool platform_file_open(Madness_File_Platform* file, const char* path, File_Modes file_modes, Allocator* allocator);
bool platform_file_close(Madness_File_Platform* file);
bool platform_file_read(Madness_File_Platform* file);
bool platform_file_write(Madness_File_Platform* file);
//FILE SYSTEM - ASYNC
bool platform_file_read_async();
bool platform_file_write_async();

bool platform_file_copy(const char* source_file, char* destination_file);



// RENDERER
void platform_get_vulkan_extension_names(const char*** extension_name_array);

bool platform_create_vulkan_surface(Platform_State* plat_state, Vulkan_Context* vulkan_context);

void platform_get_window_size(s32* width, s32* height);
void platform_get_window_pos(s32* x, s32* y);

//Misc
void platform_set_cursor_pos(int x, int y);
void platform_get_cursor_pos(int* out_x, int* out_y);

void platform_windows_resize(Platform_State* platform_state, int width, int height);

void platform_copy_to_clipboard(const char* c_string);

// UUID
void platform_generate_uuid(u64* high, u64* low);




#endif //PLATFORM_H
