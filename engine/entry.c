


/* TODO: these are the platform specific codes
* #ifdef _WIN32 // note the underscore: without it, it's not msdn official!
    // Windows (x64 and x86)
#elif __unix__ // all unices, not all compilers
    // Unix
#elif __linux__
    // linux
#elif __APPLE__
    // Mac OS, not sure if this is covered by __posix__ and/or __unix__ though...
#endif
 */

#include "game.h"
#include "entry.h"
#include "platform.h"

//define the function to create a game
b8 create_game(game* out_game) {
    // Application configuration.
    out_game->app_config.start_pos_x = 100;
    out_game->app_config.start_pos_y = 100;
    out_game->app_config.start_width = 1280;
    out_game->app_config.start_height = 720;
    out_game->app_config.name = "Madness Engine";
    out_game->update = game_update;
    out_game->render = game_render;
    out_game->initialize = game_initialize;
    out_game->on_resize = game_on_resize;

    // Create the game state.
    out_game->state = platform_allocate(sizeof(game_state), FALSE);

    return TRUE;
}

