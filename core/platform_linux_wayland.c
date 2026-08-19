#include "platform.h"
#include "input.h"

// Linux platform layer.
#if MPLATFORM_LINUX


#include <wayland-client.h>
#include "wayland-client-protocol.h"
#include "xdg-shell-client-protocol.h"

#include <sys/time.h>
#if _POSIX_C_SOURCE >= 199309L
#include <time.h>  // nanosleep
#else
#include <unistd.h>  // usleep
#endif

#include <sys/random.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "darray.h"
#include "input.h"
#include "event.h"
#include "c_string.h"

#include <sys/sendfile.h>
#include <dlfcn.h>

// For surface creation
#define VK_USE_PLATFORM_WAYLAND_KHR
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_wayland.h>
#include "vk_device.h"


// I found an example for wayland
// https://github.com/karl-zylinski/karl2d/blob/master/platform_linux_window_wayland.odin


typedef struct Linux_Wawyland_Internal_State
{
    struct wl_display* display;
    struct wl_surface* surface;

    VkSurfaceKHR vk_surface;
} Linux_Wawyland_Internal_State;


// Key translation
keys translate_keycode(u32 x_keycode);

static Platform_State* linux_plat_state;

bool platform_startup(
    Platform_State* plat_state,
    Input_System* input_system,
    Event_System* event_system,
    Platform_Config platform_config)
{
    //set pointers needed by the platform
    MASSERT(event_system);
    MASSERT(input_system);
    plat_state->event_system = event_system;
    plat_state->input_system = input_system;

    const char* session = getenv("XDG_SESSION_TYPE");
    if (session && strcmp(session, "wayland") == 0)
    {
        INFO("USING WAYLAND")
    }
    else if (session && strcmp(session, "x11") == 0)
    {
        INFO("USING x11")
    }
    else
    {
        FATAL("UNDEFINED LINUX BACKEND TYPE");
    }

    linux_plat_state = plat_state;

    // Create the internal state.
    plat_state->internal_state = malloc(sizeof(Linux_Wawyland_Internal_State));
    memset(plat_state->internal_state, 0, (sizeof(Linux_Wawyland_Internal_State)));
    Linux_Wawyland_Internal_State* state = (Linux_Wawyland_Internal_State*)plat_state->internal_state;

    // Connect to X
    state->display = wl_display_connect(NULL);
    if (!state->display)
    {
        FATAL("Failed to open X display.");
        return false;
    }

    struct wl_registry* registry = wl_display_get_registry(state->display);
    const struct wl_registry_listener* listener;
    int result_listener = wl_registry_add_listener(registry, listener, NULL);

    wl_display_roundtrip(state->display);
    const struct wl_seat_listener* seat_listener;
    struct wl_seat* wl_seat;
    wl_seat_add_listener(seat, seat_listener, NULL);
    wl_display_roundtrip(state->display);

    struct wl_compositor* compositor;
    wl_compositor_create_surface(compositor);

    // xdg_toplevel_set_title()
    xdg

    return true;
}

void platform_shutdown(Platform_State* plat_state)
{
    // Simply cold-cast to the known type.
    Linux_Wawyland_Internal_State* state = (Linux_Wawyland_Internal_State*)plat_state->internal_state;


}

bool platform_pump_messages(Platform_State* plat_state)
{
    //comment
    // Simply cold-cast to the known type.
    Linux_Wawyland_Internal_State* state = (Linux_Wawyland_Internal_State*)plat_state->internal_state;


    bool quit_flagged = false;



    return !quit_flagged;
}

void* platform_allocate(u64 size, bool aligned)
{
    //mmap(); //TODO:
    void* mem_block = malloc(size);
    MASSERT(mem_block);
    return mem_block;
}

void platform_free(void* block)
{
    free(block);
}

void* platform_zero_memory(void* block, u64 size)
{
    return memset(block, 0, size);
}

void* platform_copy_memory(void* dest, const void* source, u64 size)
{
    return memcpy(dest, source, size);
}

void* platform_set_memory(void* dest, s32 value, u64 size)
{
    return memset(dest, value, size);
}

f64 platform_get_absolute_time(void)
{
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return now.tv_sec + now.tv_nsec * 0.000000001;
}

void platform_sleep(u64 ms)
{
#if _POSIX_C_SOURCE >= 199309L
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000 * 1000;
    nanosleep(&ts, 0);
#else
    if (ms >= 1000)
    {
        sleep(ms / 1000);
    }
    usleep((ms % 1000) * 1000);
#endif
}

char* platform_get_dynamic_library_extension(void)
{
    return ".so";
}

char* platform_get_static_library_extension(void)
{
    return ".a";
}

typedef struct linux_file_handle
{
    void* file_handle;
    const char* file_name; //doesnt seem super needed
} linux_file_handle;

linux_file_handle file_handles[100];
static u32 file_handle_count = 1;


DLL_HANDLE platform_load_dynamic_library(const char* file_name)
{
    DLL_HANDLE out_handle = {file_handle_count, file_name};

    //probably gonna have to have some sort of internal index for this
    linux_file_handle* file_info = &file_handles[file_handle_count];
    file_info->file_name = file_name;
    file_handle_count++;

    const char* dll_extension_name = platform_get_dynamic_library_extension();
    const char* final_file_name = c_string_concat(file_name, dll_extension_name, NULL);

    // Load the library
    file_info->file_handle = dlopen(final_file_name, RTLD_NOW);
    if (!file_info->file_handle)
    {
        const char* err = dlerror();
        fprintf(stderr, "dlopen failed: %s\n", err);
    }

    out_handle.file_name = file_info->file_name;

    return out_handle;
}

bool platform_unload_dynamic_library(DLL_HANDLE handle)
{
    linux_file_handle* file = &file_handles[handle.handle];
    if (file->file_handle)
    {
        dlclose(file->file_handle);
        return true;
    }

    WARN("LINUX: UNLOAD DYNAMIC LIBRARY, not a valid file handle")
    return false;
}

bool platform_reload_dynamic_library(DLL_HANDLE handle)
{
    platform_unload_dynamic_library(handle);
    platform_load_dynamic_library(handle.file_name);
}

void* platform_get_function_address(DLL_HANDLE handle, const char* function_name)
{
    linux_file_handle* file = &file_handles[handle.handle];
    void* out_data = dlsym(file->file_handle, function_name);
    const char* err = dlerror();
    if (err)
    {
        fprintf(stderr, "dlsym failed: %s\n", err);
    }

    return out_data;
}

File_Watch_Handle platform_register_file_watch(const char* file_name)
{
    UNIMPLEMENTED();
    return (File_Watch_Handle){.handle = 0, .file_name = ""};
}

bool platform_has_filed_changed(File_Watch_Handle file_watch_handle)
{
    UNIMPLEMENTED();
    false;
}

bool platform_file_copy(const char* source_file, char* new_file)
{
    //mostly just a windows thing
    /*
    //TODO: these are suppose to be file pointers, meaning i have to do a file open on them
    size_t offset = 0;
    size_t file_copy_byte_size;
    ssize_t bytes_copied = sendfile(source_file, new_file, offset,file_copy_byte_size);
    perror ("sendfile");
    // if(errno == ????)

    */
}


void platform_get_vulkan_extension_names(const char*** extension_name_array)
{
    darray_push(*extension_name_array, &VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME);
}

// Surface creation for Vulkan
bool platform_create_vulkan_surface(Platform_State* plat_state, vulkan_context* vulkan_context)
{
    // Simply cold-cast to the known type.
    Linux_Wawyland_Internal_State* state = (Linux_Wawyland_Internal_State*)plat_state->internal_state;

    VkWaylandSurfaceCreateInfoKHR create_info = {.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR};
    create_info.display = state->display;
    create_info.surface = state->surface;
    create_info.flags = 0;
    create_info.pNext = NULL;

    VkResult result = vkCreateWaylandSurfaceKHR(
        vulkan_context->instance,
        &create_info,
        vulkan_context->allocator,
        &state->vk_surface);
    if (result != VK_SUCCESS)
    {
        FATAL("Vulkan surface creation failed.");
        return false;
    }

    vulkan_context->surface = state->vk_surface;
    return true;
}

void platform_get_window_size(s32* width, s32* height)
{
    Linux_Wawyland_Internal_State* linux_internal_state = (Linux_Wawyland_Internal_State*)linux_plat_state->internal_state;

}

void platform_get_window_pos(s32* x, s32* y)
{
    Linux_Wawyland_Internal_State* linux_internal_state = (Linux_Wawyland_Internal_State*)linux_plat_state->internal_state;

}

void platform_set_cursor_pos(int x, int y)
{
    Linux_Wawyland_Internal_State* linux_internal_state = (Linux_Wawyland_Internal_State*)linux_plat_state->internal_state;
}

void platform_generate_uuid(u64* high, u64* low)
{
    u8 bytes[16];

    ssize_t result = getrandom(bytes, sizeof(bytes), 0);

    if (result != sizeof(bytes))
    {
        high = 0;
        low = 0;
        MASSERT(false);
        return;
    }

    memcpy(high, bytes, sizeof(u64));
    memcpy(low, bytes + 8, sizeof(u64));
}

// Key translation
keys translate_keycode(u32 x_keycode)
{

}


#endif
