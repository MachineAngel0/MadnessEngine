#include "app_types.h"
#include "shader_creation_system.h"


bool application_on_event(const event_type code, u32 sender, u32 listener_inst, event_context context);

bool application_on_key(const event_type code, u32 sender, u32 listener_inst, event_context context);

bool application_on_resized(const event_type code, u32 sender, u32 listener_inst, event_context context);


static Editor_Application* app_internal;

bool editor_app_run(Editor_Application* editor_app)
{
    app_internal = editor_app;
    Application_Core* application_core = &editor_app->application_core;
    Renderer_Plugin* renderer_plugin = &editor_app->renderer_plugin;
    Editor_Plugin* editor_plugin = &editor_app->editor_plugin;

    renderer_plugin_set_default_fpn(renderer_plugin);

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
    event_register(application_core->event_system, EVENT_KEY_RELEASED, 10, application_on_key);
    event_register(application_core->event_system, EVENT_KEY_PRESSED, 12, application_on_key);


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

    editor_plugin->editor = editor_init(&application_core->memory_system, renderer_plugin->renderer,
                                        renderer_plugin->madness_ui, application_core->resource_system);

    Shader_Creation_System* shader_creation_system = shader_creation_system_init(&application_core->memory_system);
    shader_creation_system_test(shader_creation_system);


    //MAIN LOOP
    //TODO: theres a bug if i try to load in the same object twiceqq
    mesh_load_gltf_new(application_core->resource_system->mesh_system, "../z_assets/models/cube_gltf/Cube.gltf",
                       &renderer_plugin->renderer->arena, &renderer_plugin->renderer->frame_arena,
                       renderer_plugin->renderer->resource_system);


    mesh_load_gltf_new(application_core->resource_system->mesh_system,
                       "../z_assets/models/FlightHelmet_gltf/FlightHelmet.gltf",
                       &renderer_plugin->renderer->arena, &renderer_plugin->renderer->frame_arena,
                       renderer_plugin->renderer->resource_system);


    clock_start(&application_core->clock);

    //set up file watch
    //TODO:  .dll cant be a thing on linux
    File_Watch_Handle renderer_thing_handle = platform_register_file_watch("./MADNESSRENDERER.dll");


    while (application_core->is_running)
    {
        //hot reload:
        if (platform_has_filed_changed(renderer_thing_handle))
        {
            FATAL("File Has Changed");
            platform_reload_dynamic_library(renderer_plugin->renderer_dll_handle);
        }

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


        editor_update(editor_plugin->editor);


        madness_ui_end(renderer_plugin->madness_ui, application_core->resource_system);
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


    editor_shutdown(editor_plugin->editor);

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
            event_fire(app_internal->application_core.event_system, EVENT_APP_QUIT, 0, data);

            // Block anything else from processing this.
            return true;
        }
        if (key_code == KEY_M)
        {
            //for seeing what our memory is doing
            memory_tracker_system_print_all_memory_usage(
                app_internal->application_core.memory_system.memory_tracker_system);
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

                app_internal->renderer_plugin.renderer_resize(app_internal->renderer_plugin.renderer, width, height);
            }
        }
    }

    // Event purposely not handled to allow other listeners to get this.
    return false;
}
