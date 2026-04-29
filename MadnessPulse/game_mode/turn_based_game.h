#ifndef TURN_BASED_GAME_H
#define TURN_BASED_GAME_H


#include "game_enums.h"
#include "game_structs.h"


void turn_based_game_init(Madness_Pulse_Game* game);

void turn_start(Madness_Pulse_Game* game);

void turn_end(Madness_Pulse_Game* game);

void turn_based_reset_turn_queue(Madness_Pulse_Game* game);

bool can_current_unit_act(Madness_Pulse_Game* game);

//NOTE: It will be a rule that all characters and summoned characters are uniquely named
Unit* madness_pulse_get_unit(Madness_Pulse_Game* game, Character_Name name);


/*FUTURE:
    void reset_battle(); //does a hard reset of the game state

 */

#endif //TURN_BASED_GAME_H
