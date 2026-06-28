#ifndef UNIT_BASE_H
#define UNIT_BASE_H

#include "game_enums.h"
#include "game_structs.h"

Unit* unit_create_default(Madness_Pulse_Game* game, Character_Name character_name)
{
    Unit* unit = allocator_alloc(&game->allocator, sizeof(Unit));
    unit->name = character_name;
    unit->character_type = Character_Type_Player;
    unit->character_state = Character_State_Alive;

    action_component_init_default(&unit->action_component);
    health_component_init_default(&unit->health_component);
    mp_component_create_default(&unit->mp_component);

    inventory_component_init(&unit->inventory_component);
    battle_inventory_component_init(game, &unit->battle_inventory_component);
    overflow_component_init_default(&unit->overflow_component);

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


Unit* unit_create(Madness_Pulse_Game* game, Character_Name character_name)
{
    //if i want anything on the unit modified, then do so in the switch
    Unit* unit = unit_create_default(game, character_name);


    if (character_name == Character_Name_Madness_Progenitor ||
        character_name == Character_Name_Madness_ButterFly ||
        character_name == Character_Name_Madness_Wolf ||
        character_name == Character_Name_Madness_Envoy)
    {
        unit->character_type = Character_Type_Player;
    }
    else
    {
        unit->character_type = Character_Type_Enemy;
    }


    switch (character_name)
    {
    case Character_Name_None:
        MASSERT(false);
        break;
    case Character_Name_MAX:
        MASSERT(false);
        break;
    case Character_Name_Madness_Progenitor:
        break;
    case Character_Name_Madness_ButterFly:
        break;
    case Character_Name_Madness_Wolf:
        break;
    case Character_Name_Madness_Envoy:
        break;
    case Character_Name_Red_Jester:
        break;
    case Character_Name_Clown:
        break;
    case Character_Name_Puppet:
        break;
    case Character_Name_Doll:
        break;
    case Character_Name_Worshipper:
        break;
    case Character_Name_Burning_Soul:
        break;
    case Character_Name_Ice_Queen:
        break;
    case Character_Name_Sun_Twin:
        break;
    case Character_Name_Moon_Twin:
        break;
    case Character_Name_Sun_Envoy:
        break;
    case Character_Name_Moon_Envoy:
        break;
    case Character_Name_Fusion_Mania:
        break;
    case Character_Name_Slime:
        break;
    case Character_Name_Metal_Star:
        break;
    case Character_Name_Persona:
        break;
    case Character_Name_Mask_of_Fire:
        break;
    case Character_Name_Mask_of_Ice:
        break;
    case Character_Name_Mask_of_Blood:
        break;
    case Character_Name_Mask_of_Poison:
        break;
    case Character_Name_Mask_of_Heavenly:
        break;
    case Character_Name_Mask_of_Abyss:
        break;
    case Character_Name_Dancer:
        break;
    case Character_Name_Outer_Angel:
        break;
    case Character_Name_Outer_God:
        break;
    }


    game->units[game->units_count++] = unit;

    return unit;
}

Unit* unit_minion_create(Madness_Pulse_Game* game, Character_Name character_name, Character_Type character_type)
{
    Unit* unit = unit_create(game, character_name);
    unit->character_type = character_type;

    return unit;
}




#endif
