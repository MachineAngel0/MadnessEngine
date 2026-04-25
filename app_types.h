#ifndef APP_TYPES_H
#define APP_TYPES_H

#include "renderer.h"
// #include "renderer/UI.h"
#include "editor/editor.h"
#include "Tetris/Tetris.h"



typedef struct Application_Core
{
    Platform_Config plat_config;
    Platform_State plat_state;
    Clock clock;

    Memory_System memory_system;
    //TODO: event and input might need to be application specific,
    // say if we want the renderer and the ui to respond to specific events and input from specific windows,
    // rn its not an issue so im just leaving it here
    Event_System* event_system;
    Input_System* input_system;
    Resource_System* resource_system;
    Madness_Audio* audio_system;

    bool is_running;
    //rn used for when the window is minimized
    bool is_suspended;

    i16 width;
    i16 height;
} Application_Core;


// PLUGINS

typedef struct Renderer_Plugin
{
    Renderer* renderer;
    Madness_UI* madness_ui;

    DLL_HANDLE renderer_dll_handle;
    //function pointers
    renderer_initialize renderer_initialize;
    renderer_run renderer_run;
    renderer_terminate renderer_terminate;
    renderer_resize renderer_resize;

    UI_init ui_init;
    UI_shutdown ui_shutdown;
    UI_begin ui_begin;
    UI_end ui_end;
} Renderer_Plugin;

bool renderer_plugin_set_default_fpn(Renderer_Plugin* renderer_plugin);




typedef struct Editor_Plugin
{
    Editor* editor;
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
    Application_Core application_core;
    Renderer_Plugin renderer_plugin;
    Tetris_Game_State tetris_state;
} Tetris_Application;

bool tetris_app_run(Tetris_Application* tetris_app);


typedef struct Renderer_Dev_Application
{
    Application_Core application_core;
    Renderer_Plugin renderer_plugin;
} Renderer_Dev_Application;

bool renderer_dev_run(Renderer_Dev_Application* render_dev_app);

typedef struct UI_Test_Application
{
    Application_Core application_core;
    Renderer_Plugin renderer_plugin;
} UI_Test_Application;

bool UI_app_run(UI_Test_Application* ui_app);

typedef struct Madness_Pulse_Application
{
    Application_Core application_core;
    Renderer_Plugin renderer_plugin;
    Game_Plugin game_application; // TODO: probably gonna have to get changed
} Madness_Pulse_Application;

bool madness_pulse_run(Madness_Pulse_Application* madness_pulse_app);

typedef struct Editor_Application
{
    Application_Core application_core;
    Editor_Plugin editor_plugin;
    Renderer_Plugin renderer_plugin;
    Game_Plugin game_plugin;
} Editor_Application;

bool editor_app_run(Editor_Application* editor_app);

#endif
