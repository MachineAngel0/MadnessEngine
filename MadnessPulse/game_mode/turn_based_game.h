#ifndef TURN_BASED_GAME_H
#define TURN_BASED_GAME_H


#include "game_enums.h"
#include "game_structs.h"


void turn_based_game_init(Madness_Pulse_Game* game);

void turn_start(Madness_Pulse_Game* game);

void turn_end(Madness_Pulse_Game* game);

//called once per frame to respond to any changes in teh game state, like user input
void turn_update(Madness_Pulse_Game* game);


void turn_based_reset_turn_queue(Madness_Pulse_Game* game);


void game_resolve_unit_character_list_types(Madness_Pulse_Game* game);


bool can_current_unit_act(Madness_Pulse_Game* game);

//NOTE: It will be a rule that all characters and summoned characters are uniquely named
Unit* madness_pulse_get_unit(const Madness_Pulse_Game* game, Unit_Handle handle);
const char* madness_pulse_get_unit_name(const Madness_Pulse_Game* game, Unit_Handle handle);

bool character_handle_compare(Unit_Handle a, Unit_Handle b);

/*FUTURE:
    void reset_battle(); //does a hard reset of the game state
 */

//DEBUG STUFF

void turn_based_game_display_debug_info(Madness_Pulse_Game* game);





#endif //TURN_BASED_GAME_H
