
#include "entry_game.h"

#include <stdio.h>
#include <stdbool.h>

MAPI void update_game_fake(game_fake* game_f);
MAPI void game_fake_initialize(game_fake* game_f);

static HMODULE game_dll_handle;

typedef void (game_update)(game_fake*);
typedef void (game_init)(game_fake*);

// typedef void(game_update)(game_fake*);
// typedef void(game_init)(game_fake*);


void create_game(game_fake* game_out){
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


}

//TODO: this is specific to windows, so it should definetly get changed to be cross platform as some point
MAPI void game_reload(game_fake* game_f)
{
    // alt + ; -> will reload the dll
    load_dll("libMADNESSGAME.dll", "libMADNESSGAME_TEMP.dll", &game_dll_handle);
    if (game_dll_handle)
    {
        game_f->initialize = (game_init*)GetProcAddress(game_dll_handle, "game_fake_initialize");
        game_f->update = (game_update*)GetProcAddress(game_dll_handle, "update_game_fake");
    }
}


MAPI void game_fake_initialize(game_fake* game_f)
{
    printf("game init\n");
}

MAPI void update_game_fake(game_fake* game_f)
{
    game_f->testing_switch = false;
    if (game_f->testing_switch)
    {
        printf("game updating\n");
    }
    else
    {
        printf("game other update 565\n");
    }
}