#include "app_types.h"
#include "platform/platform.h"
#include "clock.h"
#include "core/platform/event.h"
#include "core/platform/input.h"
#include "core/platform/audio.h"

/*
typedef struct application_state
{
    struct renderer_app* renderer;

    platform_state platform;

    Clock clock;

    bool is_running;
    //rn used for when the window is minimized
    bool is_suspended;

    i16 width;
    i16 height;
} application_state;

static application_state app_state;
*/

static Renderer_Dev_Application* app_internal;



bool application_on_event(const event_type code, u32 sender, u32 listener_inst, event_context context);

bool application_on_key(const event_type code, u32 sender, u32 listener_inst, event_context context);

bool application_on_resized(const event_type  code, u32 sender, u32 listener_inst, event_context context);

bool renderer_dev_run(Renderer_Dev_Application* render_dev_app)
{
    app_internal = render_dev_app;


    application_base_init(&render_dev_app->application_base);

    //just for convience
    Renderer* renderer = &render_dev_app->renderer_application.renderer;

    //initialize the applications memory
    // Memory_System_Config memory_config;
    // memory_config.memory_request_size = GB(4);
    // memory_config.file_config = "What's up Boy";
    memory_tracker_init();

    u64 memory_request_size = GB(4);
    memory_system_init(&render_dev_app->application_base.memory_system, memory_request_size);

    INFO("APPLICATION MEMORY SUCCESSFULLY ALLOCATED")





    render_dev_app->application_base.is_running = true;

    // Initialize subsystems.
    event_init(&render_dev_app->application_base.memory_system);
    input_init(&render_dev_app->application_base.input_system, &render_dev_app->application_base.memory_system);
    // audio_system_init();


    //register events needed for this application
    event_register(EVENT_APP_QUIT, 10, application_on_event);
    event_register(EVENT_APP_RESIZE, 0, application_on_resized);
    event_register(EVENT_KEY_RELEASED, 10, application_on_key);
    event_register(EVENT_KEY_PRESSED, 12, application_on_key);


    //start the platform
    if (!platform_startup(
        &render_dev_app->application_base.plat_state,
        &render_dev_app->application_base.input_system,
        render_dev_app->application_base.app_config.name,
        render_dev_app->application_base.app_config.start_pos_x,
        render_dev_app->application_base.app_config.start_pos_y,
        render_dev_app->application_base.app_config.start_width,
        render_dev_app->application_base.app_config.start_height))
    {
        return false;
    }

    //init the render packets
    render_dev_app->renderer_application.render_packet = render_packet_init(&render_dev_app->application_base.memory_system.application_arena);
    Render_Packet* render_packet = render_dev_app->renderer_application.render_packet;

    //start the renderer
    if (!render_dev_app->renderer_application.renderer_initialize(&render_dev_app->renderer_application, &render_dev_app->application_base))
    {
        FATAL("Failed to initialize the renderer")
        return false;
    };

    Madness_UI* madness_ui = madness_ui_init(renderer);

    //run the renderer
    //MAIN LOOP

    clock_start(&render_dev_app->application_base.clock);

    while (render_dev_app->application_base.is_running)
    {
        clock_update_frame_start(&render_dev_app->application_base.clock);
        clock_print_info(&render_dev_app->application_base.clock);

        input_update(&render_dev_app->application_base.input_system);
        platform_pump_messages(&render_dev_app->application_base.plat_state);

        //vulkan will crash if you minimize the window
        if (render_dev_app->application_base.is_suspended)
        {
            continue;
        }
        madness_ui_begin(madness_ui, renderer->context.framebuffer_width_new, renderer->context.framebuffer_height_new);
        madness_ui_test(madness_ui);


        //Grab all the render data
        render_packet_clear(render_packet);
        madness_ui_generate_render_data(madness_ui, render_packet);

        //render
        render_dev_app->renderer_application.renderer_run(&render_dev_app->renderer_application, &render_dev_app->application_base);



        madness_ui_end(madness_ui);
        clock_update_frame_end(&render_dev_app->application_base.clock);

    }


    /***SHUTDOWN***/
    //NOTE: (go in reverse order)

    madness_ui_shutdown(madness_ui, renderer);


    render_dev_app->renderer_application.renderer_terminate(&render_dev_app->renderer_application);


    //shutdown subsystems
    // audio_system_shutdown();

    input_shutdown(&render_dev_app->application_base.input_system);
    event_shutdown();


    memory_tracker_shutdown();
    memory_system_shutdown(&render_dev_app->application_base.memory_system);

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
            event_fire(EVENT_APP_QUIT, 0, data);

            // Block anything else from processing this.
            return true;
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

                app_internal->renderer_application.renderer_resize(&app_internal->renderer_application.renderer, width, height);
            }
        }
    }

    // Event purposely not handled to allow other listeners to get this.
    return false;
}






