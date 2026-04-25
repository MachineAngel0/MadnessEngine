#include "madness_pulse_app.h"

#include "ability.h"
#include "madness_pulse_game.h"


bool application_on_resized(const event_type code, u32 sender, u32 listener_inst, event_context context);
bool application_on_event(const event_type code, u32 sender, u32 listener_inst, const event_context context);


static Madness_Pulse_Application* app_internal;

bool madness_pulse_run(Madness_Pulse_Application* madness_pulse_app)
{
    app_internal = madness_pulse_app;
    Application_Core* application_core = &madness_pulse_app->application_core;
    Renderer_Plugin* renderer_plugin = &madness_pulse_app->renderer_plugin;

    Platform_Config platform_config;
    platform_config_use_defaults(&platform_config);
    platform_config.name = "Madness Engine Editor";


    u64 memory_request_size = GB(4);
    memory_system_init(&app_internal->application_core.memory_system, memory_request_size);
    INFO("APPLICATION MEMORY SUCCESSFULLY ALLOCATED")
    clock_init(&app_internal->application_core.clock);


    app_internal->application_core.is_running = true;

    // Initialize subsystems.
    application_core->event_system = event_init(&application_core->memory_system);
    application_core->input_system = input_init(application_core->event_system, &application_core->memory_system);
    application_core->resource_system = resource_system_init(&application_core->memory_system);
    application_core->audio_system = audio_system_init(&application_core->memory_system,
                                                       application_core->resource_system);


    //register events needed for this application
    event_register(application_core->event_system, EVENT_APP_QUIT, 10, application_on_event);
    event_register(application_core->event_system, EVENT_APP_RESIZE, 0, application_on_resized);

    //start the platform
    platform_startup(
        &application_core->plat_state,
        application_core->input_system,
        application_core->event_system,
        platform_config);


    //start the renderer and UI
    renderer_plugin->renderer = renderer_init(&application_core->plat_state,
                                              platform_config, &application_core->memory_system,
                                              application_core->input_system,
                                              application_core->event_system,
                                              application_core->resource_system);

    renderer_plugin->madness_ui = madness_ui_init(&application_core->memory_system,
                                                  application_core->input_system,
                                                  application_core->resource_system);

    Madness_Pulse_Game* madness_pulse_game = madness_pulse_game_init(&application_core->memory_system,
                                                                     renderer_plugin->madness_ui,
                                                                     application_core->event_system,
                                                                     application_core->input_system,
                                                                     application_core->resource_system);


    //MAIN LOOP

    clock_start(&application_core->clock);


    while (application_core->is_running)
    {
        clock_update_frame_start(&application_core->clock);
        clock_print_info(&application_core->clock);

        input_update(application_core->input_system);
        platform_pump_messages(&application_core->plat_state);

        //vulkan will crash if you minimize the window
        if (application_core->is_suspended)
        {
            continue;
        }
        sprite_system_begin(application_core->resource_system->sprite_system,
                            renderer_plugin->renderer->context.framebuffer_width_new,
                            renderer_plugin->renderer->context.framebuffer_height_new);


        madness_ui_begin(renderer_plugin->madness_ui, renderer_plugin->renderer->context.framebuffer_width_new,
                         renderer_plugin->renderer->context.framebuffer_height_new);


        //game
        madness_pulse_game_update(madness_pulse_game, application_core->clock.delta_time);


        madness_ui_end(renderer_plugin->madness_ui, application_core->resource_system);

        //render packet
        resource_system_update_and_create_render_packet(application_core->resource_system);
        //render
        renderer_update(renderer_plugin->renderer,
                        application_core->clock.delta_time);


        clock_update_frame_end(&application_core->clock);
    }


    /***SHUTDOWN***/
    //NOTE: (go in reverse order)

    //look at memory before shutdown
    memory_tracker_system_print_all_memory_usage(application_core->memory_system.memory_tracker_system);

    madness_pulse_game_shutdown(madness_pulse_game, &application_core->memory_system);


    madness_ui_shutdown(renderer_plugin->madness_ui);

    renderer_shutdown(renderer_plugin->renderer);


    //shutdown subsystems
    audio_system_shutdown(application_core->audio_system);

    resource_system_shutdown(application_core->resource_system, &application_core->memory_system);

    input_shutdown(application_core->input_system);
    event_shutdown(application_core->event_system);


    memory_system_shutdown(&application_core->memory_system);

    return true;
}

bool application_on_event(const event_type code, u32 sender, u32 listener_inst, const event_context context)
{
    switch (code)
    {
    case EVENT_APP_QUIT:
        INFO("EVENT_APP_QUIT: SHUTTING DOWN APP")
        app_internal->application_core.is_running = false;
        return true;
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


        if (width != app_internal->application_core.width || height != app_internal->application_core.height)
        {
            app_internal->application_core.width = width;
            app_internal->application_core.height = height;

            DEBUG("Window resize: %i, %i", width, height);

            // Handle minimization
            if (width == 0 || height == 0)
            {
                INFO("Window minimized, suspending application.");
                app_internal->application_core.is_suspended = true;
                return true;
            }
            else
            {
                if (app_internal->application_core.is_suspended)
                {
                    INFO("Window restored, resuming application.");
                    app_internal->application_core.is_suspended = false;
                }

                renderer_on_resize(app_internal->renderer_plugin.renderer, width, height);
            }
        }
    }

    // Event purposely not handled to allow other listeners to get this.
    return false;
}
