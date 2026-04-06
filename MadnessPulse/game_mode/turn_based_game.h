#ifndef MADNESSPULSEGAME_H
#define MADNESSPULSEGAME_H

#include "defines.h"
#include "game_structs.h"
#include "unit_base.h"


#define MAX_PLAYER_COUNT 4

struct Game_State
{

    //PLAYER AND ENEMY UNTIS
    Unit players[MAX_PLAYER_COUNT];
    u32 player_count;
    //idk the enemy unit count at start up since its different every map
    Unit* enemies;
    u32 enemy_count;


} Game_State;

void Battle_Manager_Init()
{

}
void Battle_Manager_Turn_Start(){}

void MadnessBattle_Manager_Turn_End(){}



#endif //MADNESSPULSEGAME_H
