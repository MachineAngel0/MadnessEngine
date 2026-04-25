#ifndef UNIT_BASE_H
#define UNIT_BASE_H

#include "game_enums.h"
#include "game_structs.h"

//NOTE: the problem with this is that some values are meant to be set by the create function
Health_Component health_component_list[Character_Name_MAX] = {

    [Character_Name_Madness_Progenitor] = {.current_health = 100, .max_health = 100},
    [Character_Name_Burning_Soul] = {
        .current_health = 100, .max_health = 100, .max_health_limit = 100, .min_health_limit = 100,
        .death_animation_flag = false, .revive_animation_flag = false
    },
};

//TODO: actions are always the same for every character, so no need for a lookup table
Action_Component action_component[Character_Name_MAX] = {
    [Character_Name_Madness_Progenitor] = {.MaxActionsAvailable = 3, .ActionsAvailable = 2}
};


Unit* unit_create_default(Madness_Pulse_Game* game)
{
    Unit* unit = arena_alloc(&game->arena, sizeof(Unit));
    unit->name = Character_Name_None;
    unit->character_type = Character_Type_Player;
    unit->character_state = Character_State_Alive;

    action_component_init_default(&unit->action_component);
    health_component_init_default(&unit->health_component);
    mp_component_create_default(&unit->mp_component);
    inventory_component_init(&unit->inventory_component);

    augment_component_create_default(&unit->augment_component);

    Resistance_Stats_Component_init_default(&unit->resistance_stats_component);
    Status_Stat_Component_init_default(&unit->status_stat_component);

    conjure_death_list_create_default(&unit->conjure_death_list_component);
    reversal_list_component_create_default(&unit->reversal_list_component);

    // UBattleTurnInformation BattleTurnInformationComponent;
    charge_list_component_create_default(&unit->charge_list_component);

    special_ability_flag_list_component_create_default(&unit->special_ability_flag_list_component);

    return unit;
}


#endif
