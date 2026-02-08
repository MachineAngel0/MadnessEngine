#include "platform.h"

#if MPLATFORM_LINUX

#include "logger.h"

#include <xcb/xcb.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h>
#include <X11/Xlib.h> //mostly deprecated in favor of Xlib-xcb
#include <X11/Xlib-xcb.h>
#include <sys/time.h>

#if _POSIX_C_SOURCE >= 199309L
#include <time.h> //nanosleep
#else
#include <unistd.h> //usleep
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct internal_state
{
    Display* display;
    xcb_connection_t* connection;
    xcb_window_t window;
    xcb_screen_t* screen;
    xcb_atom_t wm_protocol;
    xcb_atom_t wm_delete_window;
} internal_state;


MAPI b8 platform_startup(
    platform_state* plat_state,
    const char* application_name,
    i32 x,
    i32 y,
    i32 width,
    i32 height)
{
    plat_state->internal_state = malloc(sizeof(internal_state));
    internal_state* state = (internal_state*) plat_state->internal_state;

    // Connect to x
    state->display = XOpenDisplay(NULL);

    // Turn off key repeats
    XAutoRepeatOff(state->display);

    state->connection = XGetXCBConnection(state->display);

    if (xcb_connection_has_error(state->connection))
    {
        MFATAL("Failed to connect to X server via XCB");
        return FALSE;
    }

    // get data from the x server
    const struct xcb_setup_t* setup = xcb_get_setup(state->connection);

    // loop through screens using iterator
    xcb_screen_iterator_t it = xcb_setup_roots_iterator(setup);
    int screen_p = 0;
    for (i32 s = screen_p; s >= 0; s--)
    {
        xcb_screen_next(&it);
    }

    // after screens have been loop through, assign it
    state->screen = it.data;

    // allocate a XID for the window to be created
    state->window = xcb_generate_id(state->connection);

    // register event types
    // XCB_CW_BACK_PIXEL = filling then window bg with a single color
    // XCB_CW_EVENT_MASK is required
    u32 event_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;

    u32 event_values = XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE |
                       XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE |
                       XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_POINTER_MOTION |
                       XCB_EVENT_MASK_STRUCTURE_NOTIFY;

    u32 value_list[] = { state->screen->black_pixel, event_values };

    xcb_void_cookie_t cookie = xcb_create_window(
        state->connection,
        XCB_COPY_FROM_PARENT, //depth
        state->window,
        state->screen->root, // parent
        x,
        y,
        width,
        height,
        0, //no border
        XCB_WINDOW_CLASS_INPUT_OUTPUT, //class
        state->screen->root_visual,
        event_mask,
        value_list);

    // change title
    xcb_change_property(
        state->connection,
        XCB_PROP_MODE_REPLACE,
        state->window,
        XCB_ATOM_WM_NAME,
        XCB_ATOM_STRING,
        8, //data should be viewed 8 bits at a time
        strlen(application_name),
        application_name);

    //TODO: THIS IS WHERE I STOPPED 17:20 mark


}

void platform_shutdown(platform_state* plat_state)
{
}

b8 platform_pump_messages(platform_state* plat_state)
{
}

void* platform_allocate(u64 size, b8 aligned)
{
}

void platform_free(void* block, b8 aligned)
{
}

void* platform_zero_memory(void* block, u64 size)
{
}

void* platform_copy_memory(void* dest, const void* source, u64 size)
{
}

void* platform_set_memory(void* dest, i32 value, u64 size)
{
}

void platform_console_write(const char* message, u8 colour)
{
}

void platform_console_write_error(const char* message, u8 colour)
{
}

f64 platform_get_absolute_time()
{
}

void platform_sleep(u64 ms)
{
}


#endif
