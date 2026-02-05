#include "application.h"

#include "app_types.h"
#include "audio.h"
#include "defines.h"
#include "event.h"
#include "input.h"
#include "core/platform/platform.h"
#include "game_entry.c"
#include "core/memory/memory_tracker.h"

typedef struct application_state
{
    game_app* game;
    renderer_app* renderer;

    platform_state platform;
    Arena* application_memory_arena;

    b8 is_running;
    b8 is_suspended;

    i16 width;
    i16 height;
} application_state;

static application_state app_state;

//defined below
bool application_on_event(event_type code, void* sender, void* listener_inst, event_context context);

bool application_on_key(event_type code, void* sender, void* listener_inst, event_context context);


void application_get_framebuffer_size(u32* width, u32* height)
{
    *width = app_state.width;
    *height = app_state.height;
}

bool application_game_create(struct game_app* game)
{
    // HMODULE dll_handle;
    // reload_dll("libMADNESSGAME.dll", "libMADNESSGAME_TEMP.dll", &dll_handle);
    //
    // update_game_reload = (update_game*)load_function_from_dll(&dll_handle, "update_game_fake");
    // update_game_reload(app_state.game_fake);
    //
    // return 0;


    //init subsystems
    app_state.game = game;


    //on the stack, we want this first to make sure everything else if working
    memory_tracker_init();

    app_state.application_memory_arena = arena_init_malloc(MB(64));
    INFO("APPLICATION MEMORY SUCCESSFULLY ALLOCATED")

    app_state.is_running = true;


    // Initialize subsystems.
    event_init(app_state.application_memory_arena);
    input_init(app_state.application_memory_arena);


    event_register(EVENT_APP_QUIT, 10, application_on_event);
    event_register(EVENT_KEY_RELEASED, 10, application_on_key);
    event_register(EVENT_KEY_PRESSED, 12, application_on_key);


    //start the platform
    if (!platform_startup(
        &app_state.platform,
        game->app_config.name,
        game->app_config.start_pos_x,
        game->app_config.start_pos_y,
        game->app_config.start_width,
        game->app_config.start_height))
    {
        return FALSE;
    }

    //create the game
    app_state.game->initialize(app_state.game);

    //run the app/game
    application_game_run();

    //TODO: shutdown (go in reverse order)


    //shutdown subsystems
    input_shutdown();
    event_shutdown();
    memory_tracker_shutdown();
    return true;
}

//TODO: move out into platform layer
bool has_file_changed(const char* filename, FILETIME* last_write_time)
{
    WIN32_FILE_ATTRIBUTE_DATA file_info;
    if (!GetFileAttributesExA(filename, GetFileExInfoStandard, &file_info))
        return false;

    //an alternative way of doing this
    // WIN32_FIND_DATA find_data;
    // FindFirstFileA(filename, &find_data);
    // find_data.ftLastWriteTime

    if (CompareFileTime(&file_info.ftLastWriteTime, last_write_time) != 0)
    {
        *last_write_time = file_info.ftLastWriteTime;
        return true;
    }
    return false;
}

void application_game_run()
{
    const char* filename = "libMADNESSGAME.dll";
    FILETIME last_write_time = {0};
    while (app_state.is_running)
    {
        platform_pump_messages(&app_state.platform);

        //todo: a file watcher
        if (has_file_changed(filename, &last_write_time))
        {
            INFO("File changed! Reloading...\n");
            game_reload(app_state.game);
            Sleep(1000);
            continue;
        }

        app_state.game->update(app_state.game);
    }
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
        uint16_t key_code = context.data.u16[0];
        INFO("pressed key %hu", key_code);


        if (key_code == KEY_ESCAPE)
        {
            // NOTE: Technically firing an event to itself, but there may be other listeners.
            event_context data = {};
            event_fire(EVENT_APP_QUIT, 0, data);

            // Block anything else from processing this.
            return TRUE;
        }
        if (key_code == KEY_P)
        {
            //TODO: REMOVE
            INFO("pressed key P, we are reloading ");

            game_reload(app_state.game);

            Sleep(1000);
        }
    }
    if (code == EVENT_KEY_RELEASED)
    {
        uint16_t key_code = context.data.u16[0];
        INFO("released key %hu", key_code);

        if (key_code == KEY_D)
        {
            memory_tracker_print_memory_usage();
        }
    }
}
