
#include <stdio.h>
#include <stdbool.h>
#include "game_entry.h"
#include "game.h"


typedef void (game_init)(game_app*);
typedef void (game_update)(game_app*);

static HMODULE game_dll_handle;



void create_game(game_app* game_out){

    game_out->app_config.start_pos_x = 100;
    game_out->app_config.start_pos_y = 100;
    game_out->app_config.start_width = 1280;
    game_out->app_config.start_height = 720;
    game_out->app_config.name = "Madness Engine";
    game_out->testing_switch = false;


    printf("game created\n");



    load_dll("libMADNESSGAME.dll", "libMADNESSGAME_TEMP.dll", &game_dll_handle);

    game_out->initialize = (game_init*)GetProcAddress(game_dll_handle, "game_fake_initialize");
    game_out->update = (game_update*)GetProcAddress(game_dll_handle, "update_game_fake");

    if (!game_out->initialize)
    {
        FATAL("FAILED TO SET FUNCTION POINTER GAME INITIALIZATE")
    }
    if (!game_out->update)
    {
        FATAL("FAILED TO SET FUNCTION POINTER GAME UPDATE ")
    }


}

//TODO: this is specific to windows, so it should definetly get changed to be cross platform as some point
void game_reload(game_app* game_f)
{
    // ctrl + numpad 0 -> will reload the dll
    load_dll("libMADNESSGAME.dll", "libMADNESSGAME_TEMP.dll", &game_dll_handle);
    if (game_dll_handle)
    {
        game_f->initialize = (game_init*)GetProcAddress(game_dll_handle, "game_fake_initialize");
        game_f->update = (game_update*)GetProcAddress(game_dll_handle, "update_game_fake");
    }
}


