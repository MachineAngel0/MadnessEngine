#ifndef APP_TYPES_H
#define APP_TYPES_H

#include "renderer_packet.h"
#include "renderer/UI.h"
#include "editor/editor.h"
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
    Platform_State plat_state;
    Clock clock;

    Memory_System memory_system;
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

void application_base_init(Application_Base* application_base, char* app_name)
{
    application_base->app_config.start_pos_x = 100;
    application_base->app_config.start_pos_y = 100;
    application_base->app_config.start_width = 1280;
    application_base->app_config.start_height = 720;
    // application_base->app_config.name = "Madness Engine";
    application_base->app_config.name = app_name;
}

void application_base_shutdown(Application_Base* application_base)
{
    UNIMPLEMENTED()
}

// PLUGINS

typedef bool (*renderer_initialize)(struct Renderer_Plugin*, struct Application_Base*);
typedef void (*renderer_run)(struct Renderer_Plugin*, struct Application_Base*);
typedef void (*renderer_terminate)(struct Renderer_Plugin*);
typedef void (*renderer_resize)(struct Renderer_Plugin*, u32, u32);

typedef Madness_UI* (*UI_init)(Memory_System* memory_system, Renderer* renderer);
typedef bool (*UI_shutdown)(Madness_UI* madness_ui);
typedef void (*UI_begin)(Madness_UI* madness_ui, i32 screen_size_x, i32 screen_size_y);
typedef void (*UI_end)(Madness_UI* madness_ui);


typedef struct Renderer_Plugin
{
    //keep a reference to the renderer for things like application level events
    Renderer renderer;
#define RENDER_PACKETS_SIZE 20 // TODO: make configurable later, also shouldn't have more than 20 subsystems running anyway
    Render_Packet* render_packet;


    DLL_HANDLE renderer_dll_handle;
    //function pointers
    renderer_initialize renderer_initialize;
    renderer_run renderer_run;
    renderer_terminate renderer_terminate;
    renderer_resize renderer_resize;


    Madness_UI* madness_ui;

    UI_init ui_init;
    UI_shutdown ui_shutdown;
    UI_begin ui_begin;
    UI_end ui_end;

} Renderer_Plugin;
bool renderer_plugin_set_default_fpn(Renderer_Plugin* renderer_plugin);


typedef Editor* (*editor_init_fpn)(Memory_System* memory_system, Renderer* renderer, Madness_UI* madness_ui);
typedef void (*editor_run_fpn)(Editor* editor);
typedef void (*editor_shutdown_fpn)(Editor* editor);

typedef struct Editor_Plugin
{
    Editor* editor;
    DLL_HANDLE editor_dll_handle;
    editor_init_fpn editor_init;
    editor_run_fpn editor_run;
    editor_shutdown_fpn editor_shutdown;
} Editor_Plugin;
bool editor_plugin_set_default_fpn(Editor_Plugin* editor_plugin);



typedef bool (*game_init)(struct Game_Application*, struct Application_Base*);
typedef void (*game_run)(struct Game_Application*, struct Application_Base*);
typedef void (*game_shutdown)(struct Game_Application*, struct Application_Base*);

typedef struct Game_Application
{
    game_init game_init;
    game_run game_run;
    game_shutdown game_shutdown;
} Game_Plugin;


/*APPLICATION - just a collection of plugins*/



// SPECIFIC APPLICATIONS
typedef struct Tetris_Application
{
    Application_Base application_base;
    Renderer_Plugin renderer_application;
    Tetris_Game_State tetris_state;
} Tetris_Application;
bool tetris_app_run(Tetris_Application* tetris_app);


typedef struct Renderer_Dev_Application
{
    Application_Base application_base;
    Renderer_Plugin renderer_plugin;
} Renderer_Dev_Application;
bool renderer_dev_run(Renderer_Dev_Application* render_dev_app);

typedef struct UI_Test_Application
{
    Application_Base application_base;
    Renderer_Plugin renderer_plugin;
} UI_Test_Application;
bool UI_app_run(UI_Test_Application* ui_app);

typedef struct Madness_Pulse_Application
{
    Application_Base application_base;
    Renderer_Plugin renderer_application;
    Game_Plugin game_application; // TODO: probably gonna have to get changed
} Madness_Pulse_Application;
bool madness_pulse_run(Madness_Pulse_Application* madness_pulse_app);

typedef struct Editor_Application
{
    Application_Base application_base;
    Editor_Plugin editor_plugin;
    Renderer_Plugin renderer_plugin;
    Game_Plugin game_plugin;
} Editor_Application;
bool editor_app_run(Editor_Application* editor_app);

#endif
