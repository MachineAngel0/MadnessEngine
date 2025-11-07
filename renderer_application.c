#include "application.h"



#include "app_types.h"
#include "audio.h"
#include "defines.h"
#include "event.h"
#include "input.h"
#include "core/platform/platform.h"
#include "renderer_entry.c"
#include "memory_tracker.h"
#include "core/defines.h"

typedef struct application_state {
    struct renderer* renderer;

    platform_state platform;

    b8 is_running;
    b8 is_suspended;

    i16 width;
    i16 height;

} application_state;

static application_state app_state;

bool application_on_event(event_type code, void* sender, void* listener_inst, event_context context);
bool application_on_key(event_type code, void* sender, void* listener_inst, event_context context);

 bool application_renderer_create(struct renderer* renderer)
{

    //set the renderer
    app_state.renderer = renderer;

    app_state.is_running = true;

    // Initialize subsystems.



    memory_subsystem_init();
    event_init();
    input_init();


    event_register(EVENT_APP_QUIT,10, application_on_event);
    event_register(EVENT_KEY_RELEASED,10, application_on_key);
    event_register(EVENT_KEY_PRESSED,12, application_on_key);


    //start the platform
    if (!platform_startup(
        &app_state.platform,
        renderer->app_config.name,
        renderer->app_config.start_pos_x,
        renderer->app_config.start_pos_y,
        renderer->app_config.start_width,
        renderer->app_config.start_height)) {
        return false;
        }
    app_state.renderer->plat_state = &app_state.platform;

    //create the renderer
    if (!app_state.renderer->renderer_initialize(app_state.renderer))
    {
        FATAL("Failed to initialize the renderer")
        return false;
    };
    // renderer_init(app_state.renderer);

    //run the renderer
    application_renderer_run();





    //shutdown
    //NOTE: (go in reverse order)
    app_state.renderer->renderer_shutdown(app_state.renderer);


    //shutdown subsystems
    input_shutdown();
    event_shutdown();
    memory_shutdown();


    return true;
}


void application_renderer_run()
{

    // while (app_state.is_running)
    // {
        // platform_pump_messages(&app_state.platform);

        app_state.renderer->renderer_run(app_state.renderer);
    // }

     return;
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
         uint16_t key_code =  context.data.u16[0];
         INFO("pressed key %hu", key_code);


         if (key_code == KEY_ESCAPE)
         {
             // NOTE: Technically firing an event to itself, but there may be other listeners.
             event_context data = {};
             event_fire(EVENT_APP_QUIT, 0, data);

             // Block anything else from processing this.
             return TRUE;
         }


     }
     if (code == EVENT_KEY_RELEASED)
     {
         uint16_t key_code =  context.data.u16[0];
         INFO("released key %hu", key_code);

         if (key_code == KEY_D)
         {
             memory_tracker_debug_print();
         }
     }

 }


