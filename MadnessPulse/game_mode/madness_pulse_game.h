#ifndef MADNESS_PULSE_GAME_H
#define MADNESS_PULSE_GAME_H

#include "ability.h"
#include "game_structs.h"

//when we first load the application
Madness_Pulse_Game* madness_pulse_game_init(Memory_System* memory_system, Madness_UI* madness_ui, Event_System* event_system, Input_System* input, Resource_System*
                                            resource_system);

bool madness_pulse_game_shutdown(Madness_Pulse_Game* game, Memory_System* memory_system);

bool madness_pulse_game_update(Madness_Pulse_Game* game, float delta_time);

void madness_pulse_main_menu(Madness_Pulse_Game* game);
void madness_pulse_level_select(Madness_Pulse_Game* game);
void madness_pulse_ability_select(Madness_Pulse_Game* game);


#endif //MADNESS_PULSE_GAME_H
