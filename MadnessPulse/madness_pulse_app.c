#include "madness_pulse_app.h"

#include "ability.h"
#include "madness_pulse_game.h"
#include "madness_txt.h"
#include "ui_madness.h"


bool application_on_resized(const Event_Type code, String sender, String listener_inst, Event_Data context);
bool application_on_event(const Event_Type code, String sender, String listener_inst, const Event_Data context);


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

    //TODO: testing lexer/parser stuff
    //TODO: make sure the lexer free's its data
    // Reflection_System* reflection_system = reflection_system_init(&app_internal->application_core.memory_system);
    // reflection_game_data(reflection_system);

    //testing text format
    // Madness_txt* txt = madness_txt_init(NULL);

    Reflection_Registry* reflection_registry = reflection_registry_init(&app_internal->application_core.memory_system);
    generate_runtime_enums(reflection_registry);
    generate_runtime_structs(reflection_registry);
    reflection_registry_load_meta_data(reflection_registry, Reflection_Runtime_Meta_Data_File_Path);
    reflection_registry_runtime_load_data_from_txt(reflection_registry);


    Heal_Component heal_comp_write = {
        .heal_type = Heal_Types_Heal_To_Full,
        .heal_amount = 10.8f,
        .heal_only_if_dead = false
    };
    reflection_registry_to_txt_format(reflection_registry, "Heal_Component", "1", &heal_comp_write,
                                      "../z_assets/abilities/abilities.yaml");
    Heal_Component heal_comp_read = {0};
    reflection_registry_read_from_txt_format(reflection_registry, "Heal_Component", "1", &heal_comp_read,
                                             "../z_assets/abilities/abilities.yaml");

    app_internal->application_core.is_running = true;

    // Initialize subsystems.
    application_core->event_system = event_init(&application_core->memory_system);
    application_core->input_system = input_init(application_core->event_system, &application_core->memory_system);
    application_core->resource_system = resource_system_init(&application_core->memory_system);
    application_core->audio_system = audio_system_init(&application_core->memory_system,
                                                       application_core->resource_system);


    //register events needed for this application
    event_register(application_core->event_system, EVENT_APP_QUIT, STRING("application"), application_on_event);
    event_register(application_core->event_system, EVENT_APP_RESIZE, STRING("application"), application_on_resized);

    //start the platform
    platform_startup(
        &application_core->plat_state,
        application_core->input_system,
        application_core->event_system,
        platform_config);

    //start the job system
    // Job_System* job_system = job_system_initialize(&application_core->memory_system);


    //start the renderer and UI
    renderer_plugin->renderer = renderer_init(&application_core->plat_state,
                                              platform_config, &application_core->memory_system,
                                              application_core->input_system,
                                              application_core->event_system,
                                              application_core->resource_system);

    //UI
    insanity_ui_init(&application_core->memory_system, application_core->input_system,
                     application_core->resource_system);

    renderer_plugin->madness_ui = madness_ui_init(&application_core->memory_system,
                                                  application_core->input_system,
                                                  application_core->resource_system);

    /*
    mesh_load_gltf_new(application_core->resource_system->mesh_system, "../z_assets/models/cube_gltf/Cube.gltf",
                       &renderer_plugin->renderer->arena, &renderer_plugin->renderer->frame_arena,
                       renderer_plugin->renderer->resource_system);
    mesh_load_gltf_new(application_core->resource_system->mesh_system,
                       "../z_assets/models/FlightHelmet_gltf/FlightHelmet.gltf",
                       &renderer_plugin->renderer->arena, &renderer_plugin->renderer->frame_arena,
                       renderer_plugin->renderer->resource_system);
                       */


    Madness_Pulse_Game* madness_pulse_game = madness_pulse_game_init(&application_core->memory_system,
                                                                     renderer_plugin->madness_ui,
                                                                     application_core->event_system,
                                                                     application_core->input_system,
                                                                     application_core->resource_system);

    Editor* editor = editor_init(&application_core->memory_system, renderer_plugin->renderer,
                                 renderer_plugin->madness_ui, application_core->resource_system,
                                 &application_core->clock, reflection_registry);

    //MAIN LOOP

    clock_start(&application_core->clock);

    typedef enum DEBUG_APP_STATE
    {
        DEBUG_APP_STATE_GAME,
        DEBUG_APP_STATE_EDITOR,
        DEBUG_APP_STATE_MAX,
    } DEBUG_APP_STATE;

    DEBUG_APP_STATE debug_game_mode = DEBUG_APP_STATE_EDITOR;


    while (application_core->is_running)
    {
        clock_update_frame_start(&application_core->clock);
        // clock_print_info(&application_core->clock);

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
        insanity_ui_begin(renderer_plugin->renderer->context.framebuffer_width_new,
                          renderer_plugin->renderer->context.framebuffer_height_new);

        //game and editor switch between
        if (input_key_released_unique(application_core->input_system, KEY_TAB))
        {
            debug_game_mode = (debug_game_mode + 1) % (DEBUG_APP_STATE_MAX);
        }
        switch (debug_game_mode)
        {
        case DEBUG_APP_STATE_GAME:
            madness_pulse_game_update(madness_pulse_game, application_core->clock.delta_time);
            break;
        case DEBUG_APP_STATE_EDITOR:
            editor_update(editor);
            break;
        case DEBUG_APP_STATE_MAX:
            FATAL("DEBUG GAME STATE should not be here");
            break;
        }



        madness_ui_end(renderer_plugin->madness_ui);
        insanity_ui_end();

        //render packet
        resource_system_update_and_create_render_packet(application_core->resource_system);

        application_core->resource_system->render_packet->ui_data_packet.madness_ui_render_packet =
            madness_ui_get_ui_render_data(renderer_plugin->madness_ui);
        //TODO:
        // application_core->resource_system->render_packet->ui_data_packet.insanity_ui_render_packet =
        // insanity_get_render_data();

        //render
        renderer_update(renderer_plugin->renderer,
                        application_core->clock.delta_time);


        clock_update_frame_end(&application_core->clock);
    }


    /***SHUTDOWN***/
    //NOTE: (go in reverse order)

    //look at memory before shutdown


    editor_shutdown(editor);
    madness_pulse_game_shutdown(madness_pulse_game, &application_core->memory_system);


    madness_ui_shutdown(renderer_plugin->madness_ui);

    renderer_shutdown(renderer_plugin->renderer);


    //shutdown subsystems
    audio_system_shutdown(application_core->audio_system);

    resource_system_shutdown(application_core->resource_system, &application_core->memory_system);

    input_shutdown(application_core->input_system);
    event_shutdown(application_core->event_system, &application_core->memory_system);


    memory_system_shutdown(&application_core->memory_system);

    return true;
}

bool application_on_event(const Event_Type code, String sender, String listener_inst, const Event_Data context)
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

bool application_on_resized(const Event_Type code, String sender, String listener_inst, const Event_Data context)
{
    if (code == EVENT_APP_RESIZE)
    {
        u16 width = context.data.event_data_window_resize.width;
        u16 height = context.data.event_data_window_resize.height;
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
