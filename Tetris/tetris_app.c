#include "tetris_app.h"
#include "clock.h"
#include "event.h"
#include "input.h"
#include "../renderer/UI.h"

//this is here mainly for making handling events easier
static Tetris_Application* app_internal;


bool application_on_event(const event_type code, u32 sender, u32 listener_inst, event_context context);

bool application_on_key(const event_type code, u32 sender, u32 listener_inst, event_context context);

bool application_on_resized(const event_type code, u32 sender, u32 listener_inst, event_context context);


void tetris_dev_set_function_pointers(Tetris_Application* tetris_application)
{
    DLL_HANDLE render_lib_handle = platform_load_dynamic_library("./MADNESSRENDERER");
    if (render_lib_handle.handle == 0)
    {
        FATAL("FAILED TO LOAD MADNESSRENDERER DLL");
    }

    Renderer_Plugin* renderer_out = &tetris_application->renderer_plugin;
    renderer_out->renderer_initialize = (renderer_initialize)platform_get_function_address(
        render_lib_handle, "renderer_init");
    renderer_out->renderer_run = (renderer_run)platform_get_function_address(render_lib_handle, "renderer_update");
    renderer_out->renderer_terminate = (renderer_terminate)platform_get_function_address(
        render_lib_handle, "renderer_shutdown");
    renderer_out->renderer_resize = (renderer_resize)platform_get_function_address(
        render_lib_handle, "renderer_on_resize");


    if (!renderer_out->renderer_initialize)
    {
        FATAL("FAILED TO SET FUNCTION POINTER RENDER INITIALIZATION")
    }
    if (!renderer_out->renderer_run)
    {
        FATAL("FAILED TO SET FUNCTION POINTER RENDER RUN")
    }
    if (!renderer_out->renderer_terminate)
    {
        FATAL("FAILED TO SET FUNCTION POINTER RENDER TERMINATE")
    }
    if (!renderer_out->renderer_resize)
    {
        FATAL("FAILED TO SET FUNCTION POINTER RENDER RESIZE")
    }
}


bool tetris_game_run(Tetris_Application* tetris_application)
{
    app_internal = tetris_application;


    application_base_init(&app_internal->application_base, "Madness Engine Renderer");

    //just for convinience
    Renderer* renderer = &app_internal->renderer_plugin.renderer;
    Memory_System* memory_system = &app_internal->application_base.memory_system;
    Event_System* event_system = &app_internal->application_base.event_system;
    Input_System* input_system = &app_internal->application_base.input_system;
    Resource_System* resource_system = &app_internal->application_base.resource_system;

    app_internal = tetris_application;
    application_base_init(&app_internal->application_base, "Tetris");
    //initialize the applications memory
    // Memory_System_Config memory_config;
    // memory_config.memory_request_size = GB(4);
    // memory_config.file_config = "What's up Boy";

    u64 memory_request_size = GB(4);
    memory_system_init(&app_internal->application_base.memory_system, memory_request_size);
    INFO("APPLICATION MEMORY SUCCESSFULLY ALLOCATED")


    app_internal->application_base.is_running = true;

    // Initialize subsystems.
    event_init(event_system, memory_system);
    input_init(input_system, event_system, memory_system);

    resource_system_init(resource_system, memory_system);

    // audio_system_init();


    //register events needed for this application
    event_register(event_system, EVENT_APP_QUIT, 10, application_on_event);
    event_register(event_system, EVENT_APP_RESIZE, 0, application_on_resized);
    event_register(event_system, EVENT_KEY_RELEASED, 10, application_on_key);
    event_register(event_system, EVENT_KEY_PRESSED, 12, application_on_key);

    //start the platform
    if (!platform_startup(
        &app_internal->application_base.plat_state,
        &app_internal->application_base.input_system,
        app_internal->application_base.app_config.name,
        app_internal->application_base.app_config.start_pos_x,
        app_internal->application_base.app_config.start_pos_y,
        app_internal->application_base.app_config.start_width,
        app_internal->application_base.app_config.start_height))
    {
        return false;
    }

    //start the renderer
    if (!app_internal->renderer_plugin.renderer_initialize(&app_internal->renderer_plugin,
                                                           &app_internal->application_base))
    {
        FATAL("Failed to initialize the renderer")
        return false;
    };

    Madness_UI* madness_ui = app_internal->renderer_plugin.madness_ui;
    madness_ui = app_internal->renderer_plugin.ui_init(memory_system, renderer);

    //start the game
    Tetris_Game_State* tetris_game_state = tetris_init(&app_internal->application_base.memory_system,
                                                       resource_system);


    //QUESTION: should the renderer reach into the game state or should the game state send its draw info to the renderer
    // should the renderer have a draw api to be used

    //MAIN LOOP

    clock_start(&app_internal->application_base.clock);


    while (app_internal->application_base.is_running)
    {
        clock_update_frame_start(&app_internal->application_base.clock);
        clock_print_info(&app_internal->application_base.clock);

        //clear the render_packets then each system will add to it
        render_packet_clear(app_internal->renderer_plugin.render_packet);

        platform_pump_messages(&app_internal->application_base.plat_state);
        input_update(&app_internal->application_base.input_system);

        tetris_update(tetris_game_state, app_internal->application_base.clock.delta_time);

        sprite_system_begin(resource_system->sprite_system, renderer->context.framebuffer_width_new,
                     renderer->context.framebuffer_height_new);
        madness_ui_begin(madness_ui, app_internal->renderer_plugin.renderer.context.framebuffer_width_new,
                         app_internal->renderer_plugin.renderer.context.framebuffer_height_new);
        //TODO: remove the test later on
        madness_ui_test(madness_ui);

        //vulkan will crash if you minimize the window
        if (app_internal->application_base.is_suspended)
        {
            continue;
        }


        //Render
        app_internal->renderer_plugin.renderer_run(&app_internal->renderer_plugin,
                                                   &app_internal->application_base);

        madness_ui_end(madness_ui);
        clock_update_frame_end(&app_internal->application_base.clock);
    }


    /***SHUTDOWN***/
    //NOTE: (go in reverse order)
    app_internal->renderer_plugin.renderer_terminate(&app_internal->renderer_plugin);


    //shutdown subsystems
    // audio_system_shutdown();
    input_shutdown(&app_internal->application_base.input_system);
    event_shutdown(&app_internal->application_base.event_system);


    memory_system_shutdown(&app_internal->application_base.memory_system);

    return true;
}


bool application_on_event(const event_type code, u32 sender, u32 listener_inst, const event_context context)
{
    switch (code)
    {
    case EVENT_APP_QUIT:
        INFO("EVENT_APP_QUIT: SHUTTING DOWN APP")
        app_internal->application_base.is_running = false;
        return true;
    }
    return false;
}


bool application_on_key(const event_type code, u32 sender, u32 listener_inst, const event_context context)
{
    if (code == EVENT_KEY_PRESSED)
    {
        uint16_t key_code = context.data.u16[0];
        INFO("pressed key %hu", key_code);


        if (key_code == KEY_ESCAPE)
        {
            // NOTE: Technically firing an event to itself, but there may be other listeners.
            event_context data = {};
            event_fire(&app_internal->application_base.event_system, EVENT_APP_QUIT, 0, data);

            // Block anything else from processing this.
            return true;
        }
        if (key_code == KEY_M)
        {
            //for seeing what our memory is doing
            memory_tracker_system_print_all_memory_usage(
                app_internal->application_base.memory_system.memory_tracker_system);
        }
    }
    if (code == EVENT_KEY_RELEASED)
    {
        uint16_t key_code = context.data.u16[0];
        INFO("released key %hu", key_code);

        // if (key_code == KEY_M)
        // {
        //     memory_tracker_print_memory_usage();
        // }
    }

    return false;
}


bool application_on_resized(const event_type code, u32 sender, u32 listener_inst, const event_context context)
{
    if (code == EVENT_APP_RESIZE)
    {
        u16 width = context.data.u16[0];
        u16 height = context.data.u16[1];
        // Check if different. If so, trigger a resize event.


        if (width != app_internal->application_base.width || height != app_internal->application_base.height)
        {
            app_internal->application_base.width = width;
            app_internal->application_base.height = height;

            DEBUG("Window resize: %i, %i", width, height);

            // Handle minimization
            if (width == 0 || height == 0)
            {
                INFO("Window minimized, suspending application.");
                app_internal->application_base.is_suspended = true;
                return true;
            }
            else
            {
                if (app_internal->application_base.is_suspended)
                {
                    INFO("Window restored, resuming application.");
                    app_internal->application_base.is_suspended = false;
                }

                app_internal->renderer_plugin.renderer_resize(&app_internal->renderer_plugin, width, height);
            }
        }
    }

    // Event purposely not handled to allow other listeners to get this.
    return false;
}
