#ifndef ABILITIES_H
#define ABILITIES_H


#include "ability_adder.h"
#include "ability_remove.h"
#include "action_add.h"
#include "ability_remove_all.h"
#include "action_remove.h"
#include "action_trade.h"
#include "asserts.h"
#include "charge.h"
#include "conjure.h"
#include "game_enums.h"
#include "game_structs.h"

#include "heal.h"
#include "heal_percent.h"
#include "heal_setter.h"
#include "heal_to_full.h"
#include "damage.h"
#include "drain.h"
#include "drain_all.h"
#include "drain_percent.h"
#include "mp_adder.h"
#include "mp_full.h"
#include "mp_remove.h"
#include "mp_zero.h"
#include "summoner.h"




void ability_add_component(Ability* ability, Ability_Component* component, Ability_Activation_Type activation_type, Ability_Component_Target_Type target_override)
{
    component->target_override =target_override;

    switch (activation_type)
    {
    case Ability_Activation_Type_Normal:
        ability->normal_components[ability->normal_component_count++] = *component;
        break;
    case Ability_Activation_Type_Reversal:
        ability->reversal_components[ability->reversal_component_count++] = *component;
        break;
    case Ability_Activation_Type_Turn:
        ability->turn_components[ability->turn_component_count++] = *component;
        break;
    }
}


void ability_set_turn_info_component(Ability* ability,
                                     const Turn_Activation_Type turn_activation,
                                     const u8 effect_length,
                                     const u8 turns_until_triggered)
{
    ability->turn_info_component = (Turn_Base_Component){
        .turn_activation = turn_activation,
        .effect_length = effect_length,
        .turns_until_triggered = turns_until_triggered,
        .was_set = true,
    };
}

void ability_set_reversal_info_component(Ability* ability, Reversal_Duration duration)
{
    //TODO: make this into a bunch of bit flags
    ability->reversal_info_components = (Reversal_Base_Component){0};
    ability->reversal_info_components.reversal_duration = duration;
    ability->reversal_info_components.was_set = true;
}


void ability_component_process_effect(Madness_Pulse_Game* game, Ability_Target_Execution_Info* ability_target_info,
                                      Unit* current_target,
                                      Ability_Component* ability_data)
{
    //all components have type as their first param, so if for some reason its not set it will be considered invalid
    // switch (ability_data->type)


    switch (ability_data->type)
    {
    case Ability_Component_TYPE_INVALID:
        MASSERT_MSG(false, "INVALID ABILITY COMPONENT TYPE");
        break;
    case Ability_Component_TYPE_HEAL:
        heal_ability(current_target, &ability_data->data.heal);
        break;
    case Ability_Component_TYPE_HEAL_SETTER:
        health_setter_ability(current_target, &ability_data->data.health_setter);
        break;
    case Ability_Component_TYPE_HEAL_PERCENT:
        heal_percent_ability(current_target, &ability_data->data.heal_percent);
        break;
    case Ability_Component_TYPE_HEAL_TO_FULL:
        heal_to_full_ability(current_target, &ability_data->data.heal_to_full);
        break;
    case Ability_Component_Type_Damage:
        damage_ability(current_target, current_target, &ability_data->data.damage);
        break;


    case Ability_Component_TYPE_DRAIN:
        drain_ability(current_target, &ability_data->data.drain);
        break;
    case Ability_Component_TYPE_DRAIN_PERCENT:
        drain_percent_ability(current_target, &ability_data->data.drain_percent);
        break;
    case Ability_Component_TYPE_DRAIN_ALL:
        drain_all_ability(current_target, &ability_data->data.drain_all);
        break;


    case Ability_Component_TYPE_MP_ADD:
        mp_adder_ability(current_target, &ability_data->data.mp_add);
        break;
    case Ability_Component_TYPE_MP_REMOVE:
        mp_remover_ability(current_target, &ability_data->data.mp_remove);
        break;
    case Ability_Component_TYPE_MP_FULL:
        mp_full_ability(current_target, &ability_data->data.mp_full);
        break;
    case Ability_Component_TYPE_MP_ZERO:
        mp_zero_ability(current_target, &ability_data->data.mp_zero);
        break;

    case Ability_Component_TYPE_AUGMENT_CHANGE:
        break;
    case Ability_Component_TYPE_CHARGE:
        charge_ability(current_target, &ability_data->data.charge);
        break;

    case Ability_Component_TYPE_ABILITY_ADDER:
        ability_adder_ability(current_target, &ability_data->data.ability_add);
        break;
    case Ability_Component_TYPE_ABILITY_REMOVER:
        ability_remover_ability(current_target, &ability_data->data.ability_remove);
        break;
    case Ability_Component_TYPE_ABILITY_REMOVE_ALL:
        ability_remove_all_ability(current_target, &ability_data->data.ability_remove_all);
        break;

    case Ability_Component_TYPE_ACTION_ADD:
        action_add_ability(current_target, &ability_data->data.action_add);
        break;
    case Ability_Component_TYPE_ACTION_REMOVE:
        action_remove_ability(current_target, &ability_data->data.action_remove);
        break;

    case Ability_Component_TYPE_RESISTANCE_CHANGE:
        break;
    case Ability_Component_TYPE_STATUS_CHANGE:
        break;
    case Ability_Component_TYPE_STATUS_THRESHOLD_CHANGE:
        break;

    case Ability_Component_TYPE_CONJURE:
        conjure_component_ability(current_target, &ability_data->data.conjure);
        break;

    case Ability_Component_TYPE_ACTION_CHANGE_PERMANENT:
        break;
    case Ability_Component_TYPE_ACTION_TRADE:
        action_trade_ability(game, ability_target_info->caster, ability_target_info->ability_targets,
                             &ability_data->data.action_trade);
        break;
    case Ability_Component_TYPE_MAX:
        break;
    }
}


String_Builder* ability_text_table(Madness_Pulse_Game* game, Ability* ability)
{

    //TODO: components need to show what their targets are
    String_Builder* ability_text = string_builder_create(KB(1), &game->frame_allocator);

    for (int i = 0; i < ability->normal_component_count; ++i)
    {
        Ability_Component* component = &ability->normal_components[i];
        switch (component->type)
        {
        case Ability_Component_TYPE_INVALID:
            break;
        case Ability_Component_TYPE_HEAL:
            heal_component_text(&component->data.heal, ability_text);
            break;
        case Ability_Component_TYPE_HEAL_SETTER:
            break;
        case Ability_Component_TYPE_HEAL_PERCENT:
            heal_percent_component_text(&component->data.heal_percent, ability_text);
            break;
        case Ability_Component_TYPE_HEAL_TO_FULL:
            heal_to_full_component_text(&component->data.heal_to_full, ability_text);
            break;

        case Ability_Component_Type_Damage:
            damage_component_text(&component->data.damage, ability_text);
            break;
        case Ability_Component_TYPE_DRAIN:
            drain_component_text(&component->data.drain, ability_text);
            break;
        case Ability_Component_TYPE_DRAIN_PERCENT:
            drain_percent_component_text(&component->data.drain_percent, ability_text);
            break;
        case Ability_Component_TYPE_DRAIN_ALL:
            drain_percent_component_text(&component->data.drain_percent, ability_text);
            break;

            case Ability_Component_TYPE_MP_ADD:
            mp_adder_component_text(&component->data.mp_add, ability_text);
            break;
        case Ability_Component_TYPE_MP_REMOVE:
            mp_adder_component_text(&component->data.mp_add, ability_text);
            break;
        case Ability_Component_TYPE_MP_FULL:
            mo_full_component_text(&component->data.mp_full, ability_text);
            break;
        case Ability_Component_TYPE_MP_ZERO:
            mp_zero_component_text(&component->data.mp_zero, ability_text);
            break;
        case Ability_Component_TYPE_ABILITY_ADDER:
            ability_adder_component_text(&component->data.ability_add, ability_text);
            break;
        case Ability_Component_TYPE_ABILITY_REMOVER:
            ability_remover_component_text(&component->data.ability_remove, ability_text);
            break;
        case Ability_Component_TYPE_ABILITY_REMOVE_ALL:
            ability_remover_all_component_text(&component->data.ability_remove_all, ability_text);
            break;
        case Ability_Component_TYPE_ACTION_ADD:
            action_add_component_text(&component->data.action_add, ability_text);
            break;
        case Ability_Component_TYPE_ACTION_REMOVE:
            action_remove_component_text(&component->data.action_remove, ability_text);
            break;
        case Ability_Component_TYPE_AUGMENT_CHANGE:
            break;
        case Ability_Component_TYPE_CHARGE:
            charge_component_text(&component->data.charge, ability_text);
            break;
        case Ability_Component_TYPE_CONJURE:
            conjure_component_text(&component->data.conjure, ability_text);
            break;

        case Ability_Component_TYPE_RESISTANCE_CHANGE:
            break;
        case Ability_Component_TYPE_STATUS_CHANGE:
            break;
        case Ability_Component_TYPE_STATUS_THRESHOLD_CHANGE:
            break;
        case Ability_Component_TYPE_ACTION_CHANGE_PERMANENT:
            break;
        case Ability_Component_TYPE_ACTION_TRADE:
            action_trade_component_text(&component->data.action_trade, ability_text);
            break;
        case Ability_Component_TYPE_MAX:
            break;
        }

        madness_ui_string(game->madness_ui, *string_builder_to_string(ability_text));
        string_builder_clear(ability_text);

    }


    return ability_text;
}


#endif
