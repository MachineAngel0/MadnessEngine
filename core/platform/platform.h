#ifndef PLATFORM_H
#define PLATFORM_H


#include "vk_device.h"
#include "defines.h"


typedef struct platform_state
{
    void* internal_state;
} platform_state;


bool platform_startup(
    platform_state* plat_state,
    const char* application_name,
    i32 x, i32 y,
    i32 width, i32 height);


void platform_shutdown(platform_state* plat_state);


bool platform_pump_messages(platform_state* plat_state);


//AUDIO
bool platform_audio_init(platform_state* plat_state, int32_t buffer_size, int32_t samples_per_second);

bool platform_audio_shutdown(platform_state* plat_state);

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

//DLL
char* platform_get_dynamic_library_extension(void);
char* platform_get_static_library_extension(void);

DLL_HANDLE platform_load_dynamic_library(const char* file_name);
bool platform_unload_dynamic_library(DLL_HANDLE handle);
bool platform_reload_dynamic_library(DLL_HANDLE handle);


//has to get cast into the function pointer
void* platform_get_function_address(DLL_HANDLE handle, const char* function_name);

//FILE SYSTEM
void platform_file_watch(const char* file_name);
bool platform_has_filed_changed(const char* file_name);
bool platform_file_copy(const char* source_file, char* new_file);


// RENDERER
void platform_get_vulkan_extension_names(const char*** extension_name_array);

bool platform_create_vulkan_surface(platform_state* plat_state, vulkan_context* vulkan_context);


#endif //PLATFORM_H
