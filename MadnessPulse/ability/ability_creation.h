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

    heal_component_create(&ac1, 15, false);


    ability_add_component(ability, &ac1, Ability_Activation_Type_Normal, Ability_Component_Target_Type_Target);
}

void pass_ability_create(Ability* ability, Ability_Info* ability_info)
{
    *ability_info = (Ability_Info){
        .ability_name = Ability_Name_Pass,
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

    heal_component_create(&ac1, 15, false);
}

void damage_debug_create(Ability* ability, Ability_Info* ability_info)
{
    *ability_info = (Ability_Info){
        .ability_name = Ability_Name_DEBUG_DAMAGE,
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


    damage_component_create(&ac1, Element_Type_Fire, 15);

    ability_add_component(ability, &ac1, Ability_Activation_Type_Normal, Ability_Component_Target_Type_Target);
}


void debug_turn_ability_create(Ability* ability, Ability_Info* ability_info)
{
    *ability_info = (Ability_Info){
        .ability_name = Ability_Name_DEBUG_TURN_EFFECT,
        .lore_text = STRING("NA"),
        .ability_target_type = Ability_Target_Type_Enemies,
        .ability_target_area = Target_Area_Affect_Single_Target,
        .ability_action_cost = Ability_Action_Cost_Type_1,
        .mp_cost = 1,
        .ability_primary_type = ABILITY_PRIMARY_TYPE_MADNESS,
        .ability_secondary_type = ABILITY_SECONDARY_TYPE_Poison,
        .overflow_value = Ability_Overflow_Value_Type_Base,
    };


    ability_set_turn_info_component(ability, Turn_Activation_Type_First_Start, 2, 0);

    //fire ability for now
    Ability_Component ac1 = {0};

    damage_component_create(&ac1, Element_Type_Fire, 15);
    ability_add_component(ability, &ac1, Ability_Activation_Type_Turn, Ability_Component_Target_Type_Target);
}

void debug_damage_self_and_enemy_ability_create(Ability* ability, Ability_Info* ability_info)
{
    *ability_info = (Ability_Info){
        .ability_name = Ability_Name_DEBUG_DAMAGE_SELF_AND_ENEMY,
        .lore_text = STRING("NA"),
        .ability_target_type = Ability_Target_Type_Enemies,
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

    damage_component_create(&ac1, Element_Type_Fire, 15);
    ability_add_component(ability, &ac1, Ability_Activation_Type_Normal, Ability_Component_Target_Type_Target);


    damage_component_create(&ac2, Element_Type_Fire, 15);
    ability_add_component(ability, &ac2, Ability_Activation_Type_Normal, Ability_Component_Target_Type_Caster);
}

void debug_madness_ability_create(Ability* ability, Ability_Info* ability_info)
{
    *ability_info = (Ability_Info){
        .ability_name = Ability_Name_DEBUG_MADNESS,
        .lore_text = STRING("NA"),
        .ability_target_type = Ability_Target_Type_Enemies,
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

    damage_component_create(&ac1, Element_Type_Fire, 15);
    ability_add_component(ability, &ac1, Ability_Activation_Type_Normal, Ability_Component_Target_Type_Target);


    ability_adder_component_create(&ac2, Ability_Name_DEBUG_MADNESS, 1);
    ability_add_component(ability, &ac2, Ability_Activation_Type_Normal, Ability_Component_Target_Type_Caster);
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

    case Ability_Name_DEBUG_STARTING_1:
        break;
    case Ability_Name_DEBUG_STARTING_2:
        break;
    case Ability_Name_DEBUG_STARTING_3:
        break;
    case Ability_Name_DEBUG_STARTING_4:
        break;

    //DEBUG ABILTIES
    case Ability_Name_DEBUG_HEAL:
        heal_debug_create(ability_to_fill_out, ability_info);
        break;
    case Ability_Name_DEBUG_DAMAGE:
        damage_debug_create(ability_to_fill_out, ability_info);
        break;
    case Ability_Name_DEBUG_DAMAGE_SELF_AND_ENEMY:
        debug_damage_self_and_enemy_ability_create(ability_to_fill_out, ability_info);
        break;
    case Ability_Name_DEBUG_MADNESS:
        debug_madness_ability_create(ability_to_fill_out, ability_info);
        break;
    case Ability_Name_DEBUG_INSANITY:
        break;
    case Ability_Name_REVERSAL_TEST:
        break;

    case Ability_Name_DEBUG_TURN_EFFECT:
        debug_turn_ability_create(ability_to_fill_out, ability_info);
        break;
    case Ability_Name_DEBUG_POISON:
        break;

    case Ability_Name_DEBUG_INSTAKILL_SELF:
        debug_instakill_self(ability_to_fill_out, ability_info);
        break;
    case Ability_Name_DEBUG_INSTAKILL_TARGET:
        debug_instakill_target(ability_to_fill_out, ability_info);
        break;
    case Ability_Name_DEBUG_INSTAKILL_PLAYERS:
        debug_instakill_players(ability_to_fill_out, ability_info);
        break;
    case Ability_Name_DEBUG_INSTAKILL_ENEMYS:
        debug_instakill_enemies(ability_to_fill_out, ability_info);
        break;
    case Ability_Name_DEBUG_INSTAKILL_ALL:
        debug_instakill_all(ability_to_fill_out, ability_info);
        break;
    case Ability_Name_DEBUG_PASS_ALL_PLAYER_UNITS_TURNS:
        debug_pass_all_player_turns_ability_create(ability_to_fill_out, ability_info);
        break;
    case Ability_Name_DEBUG_GIVE_MYSELF_ACTIONS:
        debug_give_myself_actions_ability_create(ability_to_fill_out, ability_info);
        break;
    //DEBUG Component ABILTIES
    case Ability_Name_HEAL:
        break;
    case Ability_Name_HEAL_SETTER:
        break;
    case Ability_Name_HEAL_PERCENT:
        break;
    case Ability_Name_HEAL_TO_FULL:
        break;
    case Ability_Name_DRAIN:
        break;
    case Ability_Name_DRAIN_PERCENT:
        break;
    case Ability_Name_DRAIN_ALL:
        break;
    case Ability_Name_Damage:
        break;
    case Ability_Name_DEBUG_MP_ADD:
        break;
    case Ability_Name_DEBUG_MP_REMOVE:
        break;
    case Ability_Name_DEBUG_MP_FULL:
        break;
    case Ability_Name_DEBUG_MP_ZERO:
        break;
    case Ability_Name_DEBUG_ABILITY_ADDER:
        break;
    case Ability_Name_DEBUG_ABILITY_REMOVER:
        break;
    case Ability_Name_DEBUG_ABILITY_REMOVE_ALL:
        break;
    case Ability_Name_DEBUG_ACTION_ADD:
        break;
    case Ability_Name_DEBUG_ACTION_REMOVE:
        break;
    case Ability_Name_CHARGE:
        break;
    case Ability_Name_CONJURE:
        break;
    case Ability_Name_SUMMONER:
        break;
    //Starting Abilties
    case Ability_Name_Pass:
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
