#ifndef UNIT_BASE_H
#define UNIT_BASE_H

#include "game_enums.h"
#include "game_structs.h"

//NOTE: the problem with this is that some values are meant to be set by the create function
Health_Component health_component_list[Character_Name_MAX] = {

    [Character_Name_Madness_Progenitor] = {.current_health = 100, .max_health = 100},
    [Character_Name_Burning_Soul] = {.current_health = 100, .max_health = 100, .max_health_limit = 100, .min_health_limit = 100, .death_animation_flag = false, .revive_animation_flag = false},
};

//TODO: actions are always the same for every character, so no need for a lookup table
Action_Component action_component[Character_Name_MAX] = {
    [Character_Name_Madness_Progenitor] = {.MaxActionsAvailable = 3, .ActionsAvailable = 2}
};


Unit* create_unit(const Character_Name name){



}



#endif
