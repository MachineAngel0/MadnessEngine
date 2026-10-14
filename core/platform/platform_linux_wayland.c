#include "platform.h"
#include "input.h"

// Linux platform layer.
#if MPLATFORM_LINUX


#include <wayland-client.h>
#include "wayland-client-protocol.h"
// #include "xdg-shell-client-protocol.h"
#include <sys/time.h>

#include "../logger.h"
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
#include "../c_string.h"

#include <sys/sendfile.h>
#include <dlfcn.h>

// For surface creation
#define VK_USE_PLATFORM_WAYLAND_KHR
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_wayland.h>
#include "vk_device.h"


// I found an example for wayland
// https://github.com/karl-zylinski/karl2d/blob/master/platform_linux_window_wayland.odin


typedef struct Linux_Wayland_Internal_State
{
    struct wl_display* display;
    struct wl_surface* surface;

    VkSurfaceKHR vk_surface;
} Linux_Wayland_Internal_State;


// static Platform_State* linux_plat_state;

bool platform_startup_wayland(
    Platform_State* plat_state,
    Platform_Window_Config platform_config)
{
    /*//set pointers needed by the platform


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

    INFO("linux wayland connected to display");

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
    // xdg*/

    return true;
}

void platform_shutdown_wayland(Platform_State* plat_state)
{
    // Simply cold-cast to the known type.
    Linux_Wayland_Internal_State* state = (Linux_Wayland_Internal_State*)plat_state->internal_state;
}

bool platform_pump_messages_wayland(Platform_State* plat_state)
{
    //comment
    // Simply cold-cast to the known type.
    Linux_Wayland_Internal_State* state = (Linux_Wayland_Internal_State*)plat_state->internal_state;


    bool quit_flagged = false;


    return !quit_flagged;
}

void platform_get_vulkan_extension_names_wayland(const char*** extension_name_array)
{
    darray_push(*extension_name_array, &VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME);
}

#endif
