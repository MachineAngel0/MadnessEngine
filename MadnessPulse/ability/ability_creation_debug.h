#ifndef ABILITIES_CREATION_DEBUG_H
#define ABILITIES_CREATION_DEBUG_H

#include "ability.h"
#include "damage.h"
#include "game_enums.h"
#include "game_structs.h"


void debug_instakill_self(Ability* ability, Ability_Info* ability_info)
{
    *ability_info = (Ability_Info){
        .ability_name = Ability_Name_DEBUG_INSTAKILL_SELF,
        .lore_text = STRING("NA"),
        .ability_target_type = Ability_Target_Type_Self,
        .ability_target_area = Target_Area_Affect_Single_Target,
        .ability_action_cost = Ability_Action_Cost_Type_1,
        .mp_cost = 1,
        .ability_primary_type = ABILITY_PRIMARY_TYPE_MADNESS,
        .ability_secondary_type = ABILITY_SECONDARY_TYPE_Poison,
        .overflow_value = Ability_Overflow_Value_Type_Base,
    };


    //fire ability for now
    Ability_Component ac1 = {0};
    Ability_Component ac2 = {0};

    damage_component_create(&ac1, Element_Type_Madness, INSTA_KILL_DAMAGE);
    ability_add_component(ability, &ac1, Ability_Activation_Type_Normal, Ability_Component_Target_Type_Target);
    damage_component_create(&ac2, Element_Type_Insanity, INSTA_KILL_DAMAGE);
    ability_add_component(ability, &ac2, Ability_Activation_Type_Normal, Ability_Component_Target_Type_Target);

}


void debug_instakill_target(Ability* ability, Ability_Info* ability_info)
{
    *ability_info = (Ability_Info){
        .ability_name = Ability_Name_DEBUG_INSTAKILL_TARGET,
        .lore_text = STRING("NA"),
        .ability_target_type = Ability_Target_Type_All,
        .ability_target_area = Target_Area_Affect_Single_Target,
        .ability_action_cost = Ability_Action_Cost_Type_1,
        .mp_cost = 1,
        .ability_primary_type = ABILITY_PRIMARY_TYPE_MADNESS,
        .ability_secondary_type = ABILITY_SECONDARY_TYPE_Poison,
        .overflow_value = Ability_Overflow_Value_Type_Base,
    };


    Ability_Component ac1 = {0};
    Ability_Component ac2 = {0};

    damage_component_create(&ac1, Element_Type_Madness, INSTA_KILL_DAMAGE);
    ability_add_component(ability, &ac1, Ability_Activation_Type_Normal, Ability_Component_Target_Type_Target);
    damage_component_create(&ac2, Element_Type_Insanity, INSTA_KILL_DAMAGE);
    ability_add_component(ability, &ac2, Ability_Activation_Type_Normal, Ability_Component_Target_Type_Target);

}

void debug_instakill_players(Ability* ability, Ability_Info* ability_info)
{
    *ability_info = (Ability_Info){
        .ability_name = Ability_Name_DEBUG_INSTAKILL_PLAYERS,
        .lore_text = STRING("NA"),
        .ability_target_type = Ability_Target_Type_Self_And_Allies,
        .ability_target_area = Target_Area_Affect_Target_All,
        .ability_action_cost = Ability_Action_Cost_Type_1,
        .mp_cost = 1,
        .ability_primary_type = ABILITY_PRIMARY_TYPE_MADNESS,
        .ability_secondary_type = ABILITY_SECONDARY_TYPE_Poison,
        .overflow_value = Ability_Overflow_Value_Type_Base,
    };


    Ability_Component ac1 = {0};
    Ability_Component ac2 = {0};

    damage_component_create(&ac1, Element_Type_Madness, INSTA_KILL_DAMAGE);
    ability_add_component(ability, &ac1, Ability_Activation_Type_Normal, Ability_Component_Target_Type_Target);
    damage_component_create(&ac2, Element_Type_Insanity, INSTA_KILL_DAMAGE);
    ability_add_component(ability, &ac2, Ability_Activation_Type_Normal, Ability_Component_Target_Type_Target);

}


void debug_instakill_enemies(Ability* ability, Ability_Info* ability_info)
{
    *ability_info = (Ability_Info){
        .ability_name = Ability_Name_DEBUG_INSTAKILL_ENEMYS,
        .lore_text = STRING("NA"),
        .ability_target_type = Ability_Target_Type_Enemies,
        .ability_target_area = Target_Area_Affect_Target_All,
        .ability_action_cost = Ability_Action_Cost_Type_1,
        .mp_cost = 1,
        .ability_primary_type = ABILITY_PRIMARY_TYPE_MADNESS,
        .ability_secondary_type = ABILITY_SECONDARY_TYPE_Poison,
        .overflow_value = Ability_Overflow_Value_Type_Base,
    };


    Ability_Component ac1 = {0};
    Ability_Component ac2 = {0};

    damage_component_create(&ac1, Element_Type_Madness, INSTA_KILL_DAMAGE);
    ability_add_component(ability, &ac1, Ability_Activation_Type_Normal, Ability_Component_Target_Type_Target);
    damage_component_create(&ac2, Element_Type_Insanity, INSTA_KILL_DAMAGE);
    ability_add_component(ability, &ac2, Ability_Activation_Type_Normal, Ability_Component_Target_Type_Target);

}

void debug_instakill_all(Ability* ability, Ability_Info* ability_info)
{
    *ability_info = (Ability_Info){
        .ability_name = Ability_Name_DEBUG_INSTAKILL_ALL,
        .lore_text = STRING("NA"),
        .ability_target_type = Ability_Target_Type_All,
        .ability_target_area = Target_Area_Affect_Target_All,
        .ability_action_cost = Ability_Action_Cost_Type_1,
        .mp_cost = 1,
        .ability_primary_type = ABILITY_PRIMARY_TYPE_MADNESS,
        .ability_secondary_type = ABILITY_SECONDARY_TYPE_Poison,
        .overflow_value = Ability_Overflow_Value_Type_Base,
    };


    Ability_Component ac1 = {0};
    Ability_Component ac2 = {0};

    damage_component_create(&ac1, Element_Type_Madness, INSTA_KILL_DAMAGE);
    ability_add_component(ability, &ac1, Ability_Activation_Type_Normal, Ability_Component_Target_Type_Target);
    damage_component_create(&ac2, Element_Type_Insanity, INSTA_KILL_DAMAGE);
    ability_add_component(ability, &ac2, Ability_Activation_Type_Normal, Ability_Component_Target_Type_Target);

}


void debug_pass_all_player_turns_ability_create(Ability* ability, Ability_Info* ability_info)
{
    *ability_info = (Ability_Info){
        .ability_name = Ability_Name_DEBUG_PASS_ALL_PLAYER_UNITS_TURNS,
        .lore_text = STRING("NA"),
        .ability_target_type = Ability_Target_Type_Self_And_Allies,
        .ability_target_area = Target_Area_Affect_Target_All,
        .ability_action_cost = Ability_Action_Cost_Type_1,
        .mp_cost = 1,
        .ability_primary_type = ABILITY_PRIMARY_TYPE_INSANITY,
        .ability_secondary_type = ABILITY_SECONDARY_TYPE_HEAL,
        .overflow_value = Ability_Overflow_Value_Type_Base,
    };


    Ability_Component ac1 = {0};

    action_remove_component_create(&ac1,3);
    ability_add_component(ability, &ac1, Ability_Activation_Type_Normal, Ability_Component_Target_Type_Target);



}

void debug_give_myself_actions_ability_create(Ability* ability, Ability_Info* ability_info)
{
    *ability_info = (Ability_Info){
        .ability_name = Ability_Name_DEBUG_GIVE_MYSELF_ACTIONS,
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

    action_add_component_create(&ac1, 10);
    ability_add_component(ability, &ac1, Ability_Activation_Type_Normal, Ability_Component_Target_Type_Target);


}


#endif //ABILITIES_CREATION_DEBUG_H
