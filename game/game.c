
#include "game.h"


//TODO: this is specific to windows, so it should definetly get changed to be cross platform as some point
void game_fake_initialize(game* game_f)
{
    printf("game init\n");
}

void update_game_fake(game* game_f)
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