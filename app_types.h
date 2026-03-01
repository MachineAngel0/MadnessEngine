#ifndef APP_TYPES_H
#define APP_TYPES_H
#include <stdbool.h>

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


typedef bool (*renderer_initialize)(Application_Base* application_base);
typedef void (*renderer_run)( Clock* clock);
typedef void (*renderer_terminate)(void);
typedef void (*renderer_resize)(u32, u32);

typedef struct Renderer_Application
{

    renderer_initialize renderer_initialize;
    renderer_run renderer_run;
    renderer_terminate renderer_terminate;
    renderer_resize renderer_resize;

    // bool (*renderer_initialize)(Application_Base* application_base);
    // void (*renderer_run)(Clock* clock);
    // void (*renderer_shutdown)(void);
    // void (*on_resize)(u32 width, u32 height);
} Renderer_Application;


typedef struct Editor_Application
{
    Application_Base application_base;
} Editor_Application;




// SPECIFIC APPLICATIONS
typedef struct Tetris_Application
{
    Application_Base application_base;
    Renderer_Application* renderer_application;
    Tetris_Game_State* tetris_state;
}Tetris_Application;


typedef struct Renderer_Dev_Application
{
    Application_Base application_base;
    Renderer_Application renderer_application;
}Renderer_Dev_Application;



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


typedef struct editor_app
{
    // The application configuration.
    Application_Config app_config;
    platform_state* plat_state;

    // Function pointer to game's update function.
    void (*update)(struct editor_app* editor_inst);
} editor_app;


typedef struct renderer_app
{
    // The application configuration.
    Application_Config app_config;
    platform_state* plat_state;

    bool (*renderer_initialize)(struct renderer_app* renderer_inst);
    void (*renderer_run)(struct renderer_app* renderer_inst, Clock* clock);
    void (*renderer_shutdown)(struct renderer_app* renderer_inst);

    void (*on_resize)(struct renderer_app* renderer_inst, u32 width, u32 height);
} renderer_app;

#endif
