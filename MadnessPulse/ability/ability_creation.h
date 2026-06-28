#ifndef ABILITIES_CREATION_H
#define ABILITIES_CREATION_H

#include "heal.h"
#include "damage.h"
#include "game_enums.h"
#include "game_structs.h"


//NOTE: file where we create our abilties
// ability.h is where we plug in the ability creation logic and how they get executed when selected for usage by the player

//TODO: create an ability with each component for testing

void heal_debug_create(Ability* ability, Ability_Info* ability_info)
{
    *ability_info = (Ability_Info){
        .ability_name = Ability_Name_DEBUG_HEAL,
        .ability_text = STRING("does a heal"),
        .lore_text = STRING("NA"),
        .ability_target_type = Ability_Target_Type_Self_And_Allies,
        .ability_target_area = Target_Area_Affect_Single_Target,
        .ability_action_cost = Ability_Action_Cost_Type_1,
        .mp_cost = 1,
        .ability_primary_type = ABILITY_PRIMARY_TYPE_INSANITY,
        .ability_secondary_type = ABILITY_SECONDARY_TYPE_HEAL,
        .overflow_value = Ability_Overflow_Value_Type_Base,
    };




    Ability_Component ac1 = {0};

    ability_component_set_base_properties(&ac1, Target_Area_Affect_Single_Target, Ability_Target_Type_Enemies);
    heal_component_create(&ac1, 15, false);



    ability_add_component(ability, &ac1, Ability_Activation_Type_Normal);
}

void damage_debug_create(Ability* ability, Ability_Info* ability_info)
{
    *ability_info = (Ability_Info){
        .ability_name = Ability_Name_DEBUG_DAMAGE, .ability_text = STRING("does a Damage"),
        .lore_text = STRING("NA"),
        .ability_target_type = Ability_Target_Type_Enemies,
        .ability_target_area = Target_Area_Affect_Single_Target,
        .ability_action_cost = Ability_Action_Cost_Type_1,
        .mp_cost = 1,
        .ability_primary_type = ABILITY_PRIMARY_TYPE_INSANITY,
        .ability_secondary_type = ABILITY_SECONDARY_TYPE_FIRE,
        .overflow_value = Ability_Overflow_Value_Type_Base,
    };


    Ability_Component ac1 = {0};


    ability_component_set_base_properties(&ac1, Target_Area_Affect_Single_Target, Ability_Target_Type_Enemies);
    damage_component_create(&ac1, Element_Type_Fire, 15);

    ability_add_component(ability, &ac1, Ability_Activation_Type_Normal);
}


void debug_turn_ability_create(Ability* ability, Ability_Info* ability_info)
{
    *ability_info = (Ability_Info){
        .ability_name = Ability_Name_DEBUG_TURN_EFFECT, .ability_text = STRING("does a thing in x turns"),
        .lore_text = STRING("NA"),
        .ability_target_type = Ability_Target_Type_Enemies,
        .ability_target_area = Target_Area_Affect_Single_Target,
        .ability_action_cost = 1,
        .mp_cost = 1,
        .ability_primary_type = ABILITY_PRIMARY_TYPE_MADNESS,
        .ability_secondary_type = ABILITY_SECONDARY_TYPE_Poison,
        .overflow_value = Ability_Overflow_Value_Type_Base,
    };


    ability_set_turn_info_component(ability, Turn_Activation_Type_First_Start, 2, 0);

    //fire ability for now
    Ability_Component ac1 = {0};

    ability_component_set_base_properties(&ac1, Target_Area_Affect_Single_Target, Ability_Target_Type_Enemies);
    damage_component_create(&ac1, Element_Type_Fire, 15);
    ability_add_component(ability, &ac1, Ability_Activation_Type_Turn);
}




void ability_creation_table(Ability_Registry* registry, const Ability_Name ability_name)
{
    if (ability_registry_does_ability_exist(registry, ability_name))
    {
        return;
    }

    //to be filled out
    Ability* ability_to_fill_out = ability_registry_get_new_ability(registry, ability_name);
    Ability_Info* ability_info = ability_registry_get_new_ability_info(registry, ability_name);


    switch (ability_name)
    {
    case Ability_Name_INVALID:
        MASSERT_MSG(false, "Ability_Name_INVALID: FIX");
        break;
    case Ability_Name_MAX:
        MASSERT_MSG(false, "Ability_Name_MAX: FIX");
        break;
    case Ability_Name_DEBUG_HEAL:
        heal_debug_create(ability_to_fill_out, ability_info);
        break;
    case Ability_Name_DEBUG_DAMAGE:
        damage_debug_create(ability_to_fill_out, ability_info);
        break;
    case Ability_Name_Madness_Test:
        break;
    case Ability_Name_INSANITY_Test:
        break;
    case Ability_Name_REVERSAL_TEST:
        break;
    case Ability_Name_Pass:
        break;
    case Ability_Name_DEBUG_1:
        break;
    case Ability_Name_DEBUG_2:
        break;
    case Ability_Name_DEBUG_3:
        break;
    case Ability_Name_DEBUG_4:
        break;
    case Ability_Name_DEBUG_TURN_EFFECT:
        debug_turn_ability_create(ability_to_fill_out, ability_info);
        break;
    case Ability_Name_DEBUG_POISON:
        break;
    case Ability_Name_:
        break;
    }
}



void create_starting_abilties(Battle_Inventory_Component* battle_inventory, Ability_Registry* ability_registry)
{
    for (int i = 0; i < battle_inventory->battle_list->num_items; ++i)
    {
        ability_creation_table(ability_registry, dynamic_array_get(battle_inventory->battle_list, Ability_Name, i));
    }
}


#endif //ABILITIES_CREATION_H
