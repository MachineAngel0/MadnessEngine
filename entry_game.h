#ifndef ENTRY_GAME_H
#define ENTRY_GAME_H

#include "application.h"
#include "app_types.h"


//NOTE: create game will not be the same thing as initialize game
// create game, is meant to create the information needed for the platform state
extern void create_game(game* game_out);



int main()
{
    game game;
    create_game(&game);

    application_game_create(&game);
    application_game_run();

    return 0;
}


#endif