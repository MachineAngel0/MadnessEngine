#include "application.h"


#include "app_types.h"
#include "arena.h"
#include "clock.h"
#include "defines.h"
#include "event.h"
#include "input.h"
#include "core/platform/platform.h"
#include "memory_tracker.h"

typedef struct application_state
{
    struct renderer* renderer;

    platform_state platform;

    Arena* application_memory_arena;

    Clock clock;


    b8 is_running;
    //rn used for when the window is minimized
    b8 is_suspended;

    i16 width;
    i16 height;
} application_state;

static application_state app_state;

bool application_on_event(event_type code, void* sender, void* listener_inst, event_context context);

bool application_on_key(event_type code, void* sender, void* listener_inst, event_context context);

bool application_on_resized(u16 code, void* sender, void* listener_inst, event_context context);


bool application_renderer_create(struct renderer* renderer)
{
    //TODO: refactored out, just here now for testing
    // init_windows_audio();




    //set the renderer
    app_state.renderer = renderer;

    //on the stack, we want this first to make sure everything else if working
    memory_tracker_init();

    app_state.application_memory_arena = arena_init_malloc(MB(64));
    INFO("APPLICATION MEMORY SUCCESSFULLY ALLOCATED")

    app_state.is_running = true;

    // Initialize subsystems.
    event_init(app_state.application_memory_arena);
    input_init(app_state.application_memory_arena);
    audio_system_init();


    event_register(EVENT_APP_QUIT, 10, application_on_event);
    event_register(EVENT_APP_RESIZE, 0, application_on_resized);
    event_register(EVENT_KEY_RELEASED, 10, application_on_key);
    event_register(EVENT_KEY_PRESSED, 12, application_on_key);


    //start the platform
    if (!platform_startup(
        &app_state.platform,
        renderer->app_config.name,
        renderer->app_config.start_pos_x,
        renderer->app_config.start_pos_y,
        renderer->app_config.start_width,
        renderer->app_config.start_height))
    {
        return false;
    }
    app_state.renderer->plat_state = &app_state.platform;

    if (!app_state.renderer->renderer_initialize(app_state.renderer))
    {
        FATAL("Failed to initialize the renderer")
        return false;
    };
    // renderer_init(app_state.renderer); // here for debugging

    //run the renderer
    //MAIN LOOP

    clock_start(&app_state.clock);

    application_renderer_run();


    /***SHUTDOWN***/
    //NOTE: (go in reverse order)
    app_state.renderer->renderer_shutdown(app_state.renderer);


    //shutdown subsystems
    audio_system_shutdown();
    input_shutdown();
    event_shutdown();


    arena_free(app_state.application_memory_arena);
    memory_tracker_shutdown();


    return true;
}


void application_renderer_run()
{
    while (app_state.is_running)
    {

        platform_pump_messages(&app_state.platform);


        //vulkan will crash if you minimize the window
        if (app_state.is_suspended)
        {
            continue;
        }

        clock_update_frame_start(&app_state.clock);

        // clock_print_info(&app_state.clock);

        app_state.renderer->renderer_run(app_state.renderer);

        clock_update_frame_end(&app_state.clock);

    }
}


bool application_on_event(event_type code, void* sender, void* listener_inst, event_context context)
{
    switch (code)
    {
        case EVENT_APP_QUIT:
            INFO("EVENT_APP_QUIT: SHUTTING DOWN APP")
            app_state.is_running = false;
            return true;
    }
}


bool application_on_key(event_type code, void* sender, void* listener_inst, event_context context)
{
    if (code == EVENT_KEY_PRESSED)
    {
        uint16_t key_code = context.data.u16[0];
        INFO("pressed key %hu", key_code);


        if (key_code == KEY_ESCAPE)
        {
            // NOTE: Technically firing an event to itself, but there may be other listeners.
            event_context data = {};
            event_fire(EVENT_APP_QUIT, 0, data);

            // Block anything else from processing this.
            return TRUE;
        }
        if (key_code == KEY_M)
        {
            //for seeing what our memory is doing
            memory_tracker_print_memory_usage();
        }
    }
    if (code == EVENT_KEY_RELEASED)
    {
        uint16_t key_code = context.data.u16[0];
        INFO("released key %hu", key_code);

        if (key_code == KEY_D)
        {
            memory_tracker_print_memory_usage();
        }
    }

    return false;
}


bool application_on_resized(u16 code, void* sender, void* listener_inst, event_context context)
{
    if (code == EVENT_APP_RESIZE)
    {
        u16 width = context.data.u16[0];
        u16 height = context.data.u16[1];
        // Check if different. If so, trigger a resize event.


        if (width != app_state.width || height != app_state.height)
        {
            app_state.width = width;
            app_state.height = height;

            DEBUG("Window resize: %i, %i", width, height);

            // Handle minimization
            if (width == 0 || height == 0)
            {
                INFO("Window minimized, suspending application.");
                app_state.is_suspended = TRUE;
                return TRUE;
            }
            else
            {
                if (app_state.is_suspended)
                {
                    INFO("Window restored, resuming application.");
                    app_state.is_suspended = FALSE;
                }

                app_state.renderer->on_resize(app_state.renderer, width, height);
            }
        }
    }


    // Event purposely not handled to allow other listeners to get this.
    return false;
}
