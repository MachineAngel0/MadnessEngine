#ifndef PLATFORM_H
#define PLATFORM_H


#include "vk_device.h"

typedef struct Platform_Config
{
    // Window starting position x axis, if applicable.
    i16 start_pos_x;

    // Window starting position y axis, if applicable.
    i16 start_pos_y;

    // Window starting width, if applicable.
    i16 start_width;

    // Window starting height, if applicable.
    i16 start_height;

    // The application name used in windowing, if applicable.
    char* name;
} Platform_Config;

void platform_config_use_defaults(Platform_Config* config)
{
    config->start_pos_x = 100;
    config->start_pos_y = 100;
    config->start_width = 1280;
    config->start_height = 720;
    config->name = "Madness Engine";
}


typedef struct Platform_State
{
    void* internal_state;
    Input_System* input_system; // just a pointer, does not own the memory
    Event_System* event_system; // just a pointer, does not own the memory
} Platform_State;


bool platform_startup(
    Platform_State* plat_state,
    Input_System* input_system,
    Event_System* event_system,
    Platform_Config platform_config);


void platform_shutdown(Platform_State* plat_state);


bool platform_pump_messages(Platform_State* plat_state);


//AUDIO
bool platform_audio_init(Platform_State* plat_state, int32_t buffer_size, int32_t samples_per_second);

bool platform_audio_shutdown(Platform_State* plat_state);

//MEMORY
void* platform_allocate(u64 size, bool aligned);

void platform_free(void* block);

void* platform_zero_memory(void* block, u64 size);

void* platform_copy_memory(void* dest, const void* source, u64 size);

void* platform_set_memory(void* dest, i32 value, u64 size);

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

typedef struct File_Watch_Handle
{
    uint64_t handle;
    const char* file_name;
} File_Watch_Handle;


//DLL
char* platform_get_dynamic_library_extension(void);
char* platform_get_static_library_extension(void);

DLL_HANDLE platform_load_dynamic_library(const char* file_name);
bool platform_unload_dynamic_library(DLL_HANDLE handle);
bool platform_reload_dynamic_library(DLL_HANDLE handle);
bool platform_file_copy(const char* source_file, char* new_file);

//has to get cast into the function pointer
void* platform_get_function_address(DLL_HANDLE handle, const char* function_name);

//FILE SYSTEM
File_Watch_Handle platform_register_file_watch(const char* file_name); // registers a file to be watched
// bool platform_unregister_file_watch(File_Handle file_handle); // remove from the list

bool platform_has_filed_changed(File_Watch_Handle file_watch_handle); // checks if that file has been changed

File_Watch_Handle platform_register_directory_watch(const char* directory_name);
void platform_has_directory_changed(File_Watch_Handle directory_watch_handle);


// RENDERER
void platform_get_vulkan_extension_names(const char*** extension_name_array);

bool platform_create_vulkan_surface(Platform_State* plat_state, vulkan_context* vulkan_context);

//Misc
void platform_set_cursor_pos(int x, int y);
void platform_get_cursor_pos(int* out_x, int* out_y);

void platform_copy_to_clipboard(const char* c_string);


#endif //PLATFORM_H
