#ifndef APP_TYPES_H
#define APP_TYPES_H
#include <stdbool.h>

#include "renderer_packet.h"
#include "Tetris/Tetris.h"

//startup information
typedef struct Application_Config
{
    // Window starting position x axis, if applicable.
    i16 start_pos_x;

    // Window starting position y axis, if applicable.
    i16 start_pos_y;

    // Window starting width, if applicable.
    i16 start_width;

    // Window starting height, if applicable.
    i16 start_height;

    // The application name used in windowing, if applicable.
    char* name;

    //TODO:
    //u64 application_memory_requirement;
} Application_Config;


typedef struct Application_Base
{
    Application_Config app_config;
    platform_state plat_state;
    Clock clock;

    Memory_System memory_system;
    Memory_Tracker memory_tracker;
    //TODO: event and input might need to be application specific,
    // say if we want the renderer and the ui to respond to specific events and input from specific windows,
    // rn its not an issue so im just leaving it here
    Event_System event_system;
    Input_System input_system;

    bool is_running;
    //rn used for when the window is minimized
    bool is_suspended;

    i16 width;
    i16 height;
} Application_Base;

void application_base_init(Application_Base* application_base)
{
    application_base->app_config.start_pos_x = 100;
    application_base->app_config.start_pos_y = 100;
    application_base->app_config.start_width = 1280;
    application_base->app_config.start_height = 720;
    application_base->app_config.name = "Madness Engine Renderer";
}

void application_base_shutdown(Application_Base* application_base)
{
    UNIMPLEMENTED()
}


typedef bool (*renderer_initialize)(struct Renderer_Application*, struct Application_Base*);
typedef void (*renderer_run)(struct Renderer_Application*, struct Application_Base*);
typedef void (*renderer_terminate)(struct Renderer_Application*);
typedef void (*renderer_resize)(struct Renderer_Application*, u32, u32);


typedef struct Renderer_Application
{
    //keep a reference to the renderer for things like application level events
    Renderer renderer;
#define RENDER_PACKETS_SIZE 20 // TODO: make configurable later, also shouldn't have more than 20 subsystems running anyway
    Render_Packet* render_packet;

    //function pointers
    renderer_initialize renderer_initialize;
    renderer_run renderer_run;
    renderer_terminate renderer_terminate;
    renderer_resize renderer_resize;

    // bool (*renderer_initialize)(Application_Base* application_base);
    // void (*renderer_run)(Clock* clock);
    // void (*renderer_shutdown)(void);
    // void (*on_resize)(u32 width, u32 height);
} Renderer_Application;


/*EVERYTHING BELOW IS THE APPLICATION THAT IS MEANT TO BE USED*/

typedef struct Editor_Application
{
    Application_Base application_base;
} Editor_Application;
bool editor_app_run(Editor_Application* editor_app);


// SPECIFIC APPLICATIONS
typedef struct Tetris_Application
{
    Application_Base application_base;
    Renderer_Application* renderer_application;
    Tetris_Game_State* tetris_state;
} Tetris_Application;
bool tetris_app_run(Tetris_Application* tetris_app);


typedef struct Renderer_Dev_Application
{
    Application_Base application_base;
    Renderer_Application renderer_application;
} Renderer_Dev_Application;
bool renderer_dev_run(Renderer_Dev_Application* render_dev_app);


//TODO: everything below is old and should be replace
typedef struct game_app
{
    // The application configuration.
    Application_Config app_config;
    platform_state* plat_state;

    // Function pointer to game's update function.
    void (*initialize)(struct game_app* game_inst);
    void (*update)(struct game_app* game_inst);


    void (*renderer_initialize)(struct renderer_app* renderer_inst);
    void (*renderer_update)(struct renderer_app* renderer_inst);
    void (*renderer_shutdown)(struct renderer_app* renderer_inst);

    bool testing_switch;
    void* memory_reserve; // might move to app config
} game_app;




#endif
