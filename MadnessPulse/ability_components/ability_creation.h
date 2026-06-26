#ifndef ABILITIES_CREATION_H
#define ABILITIES_CREATION_H

#include "heal.h"
#include "damage.h"
#include "game_enums.h"
#include "game_structs.h"


//NOTE: file where we create our abilties
// ability.h is where we plug in the ability creation logic and how they get executed when selected for usage by the player

void heal_debug_create(Ability* ability, Ability_Info* ability_info)
{
    *ability_info = (Ability_Info){
        .ability_name = Ability_Name_DEBUG_HEAL, .ability_text = STRING("does a heal"),
        .lore_text = "NA",
        .overflow_trigger_text = STRING("NA"),
        .ability_target_type = Ability_Target_Type_Enemies,
        .ability_target_area = Target_Area_Affect_Single_Target,
        .ability_action_cost = Ability_Action_Cost_Type_1,
        .mp_cost = 1,
    };


    Ability_Component ac1 = {0};

    ability_component_set_properties(&ac1, Ability_Activation_Type_Normal, Target_Area_Affect_Single_Target,
                                     Ability_Target_Type_Enemies);
    heal_component_create(&ac1, Heal_Types_Heal_Amount, 10, false);


    ability_add_component(ability, &ac1);
}

void damage_debug_create(Ability* ability, Ability_Info* ability_info)
{
    *ability_info = (Ability_Info){
        .ability_name = Ability_Name_DEBUG_DAMAGE, .ability_text = STRING("does a Damage"),
        .lore_text = "NA",
        .overflow_trigger_text = STRING("NA"),
        .ability_target_type = Ability_Target_Type_Enemies,
        .ability_target_area = Target_Area_Affect_Single_Target,
        .ability_action_cost = 1,
        .mp_cost = 1,
    };


    Ability_Component ac1 = {0};


    ability_component_set_properties(&ac1, Ability_Activation_Type_Normal, Target_Area_Affect_Single_Target,
                                     Ability_Target_Type_Enemies);
    damage_component_create(&ac1, Element_Type_Fire, 15);

    ability_add_component(ability, &ac1);
}


void debug_turn_ability_create(Ability* ability, Ability_Info* ability_info)
{
    *ability_info = (Ability_Info){
        .ability_name = Ability_Name_DEBUG_TURN_EFFECT, .ability_text = STRING("does a thing in x turns"),
        .lore_text = "NA",
        .overflow_trigger_text = STRING("NA"),
        .ability_target_type = Ability_Target_Type_Enemies,
        .ability_target_area = Target_Area_Affect_Single_Target,
        .ability_action_cost = 1,
        .mp_cost = 1,
    };


    ability_add_turn_info_component(ability, Turn_Activation_Type_First_Start, 2, 0);


    //fire ability for now
    Ability_Component ac1 = {0};


    ability_component_set_properties(&ac1, Ability_Activation_Type_Turn, Target_Area_Affect_Single_Target,
                                     Ability_Target_Type_Enemies);
    damage_component_create(&ac1, Element_Type_Fire, 15);

    ability_add_component(ability, &ac1);
}



void ability_creation_table(Ability_Registry* registry, const Ability_Name ability_name)
{
    if (ability_registry_does_ability_exist(registry, ability_name))
    {
        return;
    }

    //to be filled out
    Ability* ability_to_fill_out = ability_registry_get_new_ability(registry, ability_name);
    Ability_Info* info = ability_registry_get_new_ability_info(registry, ability_name);


    switch (ability_name)
    {
    case Ability_Name_INVALID:
        MASSERT_MSG(false, "Ability_Name_INVALID: FIX");
        break;
    case Ability_Name_MAX:
        MASSERT_MSG(false, "Ability_Name_MAX: FIX");
        break;
    case Ability_Name_DEBUG_HEAL:
        heal_debug_create(ability_to_fill_out, info);
        break;
    case Ability_Name_DEBUG_DAMAGE:
        damage_debug_create(ability_to_fill_out, info);
        break;
    case Ability_Name_Madness_Test:
        break;
    case Ability_Name_INSANITY_Test:
        break;
    case Ability_Name_REVERSAL_TEST:
        break;
    case Ability_Name_Pass:
        break;
    }
}


void create_starting_abilties(Inventory_Component* inventory, Ability_Registry* ability_registry)
{
    for (int i = 0; i < inventory->battle_list_size; ++i)
    {
        ability_creation_table(ability_registry, inventory->battle_list_starting[i]);
    }
}


#endif //ABILITIES_CREATION_H
