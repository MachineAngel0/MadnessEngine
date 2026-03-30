#include "renderer_dev.h"
#include "app_types.h"
#include "platform/platform.h"
#include "clock.h"
#include "core/platform/event.h"
#include "core/platform/input.h"
#include "core/platform/audio.h"


extern void renderer_dev_create_fpn(Renderer_Dev_Application* renderer_app)
{
    renderer_plugin_set_default_fpn(&renderer_app->renderer_plugin);
}


static Renderer_Dev_Application* app_internal;


bool application_on_event(const event_type code, u32 sender, u32 listener_inst, event_context context);

bool application_on_key(const event_type code, u32 sender, u32 listener_inst, event_context context);

bool application_on_resized(const event_type code, u32 sender, u32 listener_inst, event_context context);


bool renderer_dev_run(Renderer_Dev_Application* render_dev_app)
{
    app_internal = render_dev_app;


    application_base_init(&app_internal->application_base);

    //just for convinience
    Renderer* renderer = &app_internal->renderer_plugin.renderer;
    Memory_System* memory_system = &app_internal->application_base.memory_system;
    Event_System* event_system = &app_internal->application_base.event_system;
    Input_System* input_system = &app_internal->application_base.input_system;


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

    //init the render packets
    app_internal->renderer_plugin.render_packet = render_packet_init(
        &app_internal->application_base.memory_system.application_arena);
    Render_Packet* render_packet = app_internal->renderer_plugin.render_packet;

    //start the renderer
    if (!render_dev_app->renderer_plugin.renderer_initialize(&app_internal->renderer_plugin,
                                                             &app_internal->application_base))
    {
        FATAL("Failed to initialize the renderer")
        return false;
    };

    Madness_UI* madness_ui = app_internal->renderer_plugin.madness_ui;
    madness_ui = app_internal->renderer_plugin.ui_init(memory_system, renderer);

    //run the renderer
    //MAIN LOOP

    clock_start(&app_internal->application_base.clock);

    //set up file watch
    //TODO:  .dll cant be a thing on linux
    File_Watch_Handle renderer_thing_handle = platform_register_file_watch("./MADNESSRENDERER.dll");


    while (app_internal->application_base.is_running)
    {
        //hot reload:
        if (platform_has_filed_changed(renderer_thing_handle))
        {
            FATAL("File Has Changed");
            platform_reload_dynamic_library(app_internal->renderer_plugin.renderer_dll_handle);
        }

        clock_update_frame_start(&app_internal->application_base.clock);
        clock_print_info(&app_internal->application_base.clock);

        input_update(input_system);
        platform_pump_messages(&app_internal->application_base.plat_state);

        //vulkan will crash if you minimize the window
        if (app_internal->application_base.is_suspended)
        {
            continue;
        }
        app_internal->renderer_plugin.ui_begin(madness_ui, renderer->context.framebuffer_width_new, renderer->context.framebuffer_height_new);
        madness_ui_test(madness_ui);


        //Grab all the render data
        render_packet_clear(render_packet);
        madness_ui_generate_render_data(madness_ui, render_packet);

        //render
        app_internal->renderer_plugin.renderer_run(&app_internal->renderer_plugin,
                                                     &app_internal->application_base);


        app_internal->renderer_plugin.ui_end(madness_ui);
        clock_update_frame_end(&app_internal->application_base.clock);
    }


    /***SHUTDOWN***/
    //NOTE: (go in reverse order)

    //look at memory before shutdown
    memory_tracker_system_print_all_memory_usage(app_internal->application_base.memory_system.memory_tracker_system);


    // madness_ui_shutdown(madness_ui);
    app_internal->renderer_plugin.ui_shutdown(madness_ui);

    render_dev_app->renderer_plugin.renderer_terminate(&app_internal->renderer_plugin);


    //shutdown subsystems
    // audio_system_shutdown();

    input_shutdown(input_system);
    event_shutdown(event_system);


    memory_system_shutdown(memory_system);

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
