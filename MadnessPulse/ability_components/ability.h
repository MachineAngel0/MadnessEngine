#ifndef ABILITIES_H
#define ABILITIES_H


#include "asserts.h"
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


void ability_component_set_base_properties(Ability_Component* component,
                                           const Target_Area_Affect target_can_affect,
                                           const Ability_Target_Type ability_target)
{
    component->target_can_affect = target_can_affect;
    component->ability_target = ability_target;
}


void ability_add_component(Ability* ability, Ability_Component* component, Ability_Activation_Type activation_type)
{
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


void ability_component_process_effect(Unit* unit, Ability_Component* ability_data)
{
    //all components have type as their first param, so if for some reason its not set it will be considered invalid
    // switch (ability_data->type)
    switch (ability_data->type)
    {
    case Ability_Component_TYPE_INVALID:
        MASSERT_MSG(false, "INVALID ABILITY COMPONENT TYPE");
        break;
    case Ability_Component_TYPE_HEAL:
        heal_ability(unit, &ability_data->data.heal);
        break;
    case Ability_Component_TYPE_HEAL_SETTER:
        health_setter_ability(unit, &ability_data->data.health_setter);
        break;
    case Ability_Component_TYPE_HEAL_PERCENT:
        heal_percent_ability(unit, &ability_data->data.heal_percent);
        break;
    case Ability_Component_TYPE_HEAL_TO_FULL:
        heal_to_full_ability(unit, &ability_data->data.heal_to_full);
        break;
    case Ability_Component_Type_Damage:
        damage_ability(unit, &ability_data->data.damage);
        break;

    case Ability_Component_Type_INSTAKILL:
        break;


    case Ability_Component_TYPE_DRAIN:
        drain_ability(unit, &ability_data->data.drain);
        break;
    case Ability_Component_TYPE_DRAIN_PERCENT:
        drain_percent_ability(unit, &ability_data->data.drain_percent);
        break;
    case Ability_Component_TYPE_DRAIN_ALL:
        drain_all_ability(unit, &ability_data->data.drain_all);
        break;


    case Ability_Component_TYPE_MP_ADD:
        mp_adder_ability(unit, &ability_data->data.mp_add);
        break;
    case Ability_Component_TYPE_MP_REMOVE:
        mp_remover_ability(unit, &ability_data->data.mp_remove);
        break;
    case Ability_Component_TYPE_MP_FULL:
        mp_full_ability(unit, &ability_data->data.mp_full);
        break;
    case Ability_Component_TYPE_MP_ZERO:
        mp_zero_ability(unit, &ability_data->data.mp_zero);
        break;


    case Ability_Component_TYPE_AUGMENT_CHANGE:
        break;
    case Ability_Component_TYPE_CHARGE:
        break;
    case Ability_Component_TYPE_CONJURE:
        break;
    case Ability_Component_TYPE_RESISTANCE_CHANGE:
        break;
    case Ability_Component_TYPE_STATUS_CHANGE:
        break;
    case Ability_Component_TYPE_STATUS_THRESHOLD_CHANGE:
        break;
    case Ability_Component_TYPE_MAX:
        break;

    case Ability_Component_TYPE_ABILITY_ADDER:
        break;
    case Ability_Component_TYPE_ABILITY_REMOVER:
        break;
    case Ability_Component_TYPE_ABILITY_REMOVE_ALL:
        break;
    case Ability_Component_TYPE_ACTION_ADD:
        break;
    case Ability_Component_TYPE_ACTION_REMOVE:
        break;
    case Ability_Component_TYPE_SUMMONER:
        break;
    }
}


void ability_process(Ability* ability, Unit* unit)
{
    for (u32 i = 0; i < ability->normal_component_count; i++)
    {
        Ability_Component* ability_component = &ability->normal_components[i];
        ability_component_process_effect(unit, ability_component);
    }
}



String_Builder* ability_text_table(Madness_Pulse_Game* game, Ability* ability)
{
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

        case Ability_Component_TYPE_AUGMENT_CHANGE:
            break;
        case Ability_Component_TYPE_CHARGE:
            break;
        case Ability_Component_TYPE_CONJURE:
            break;
        case Ability_Component_TYPE_DRAIN:
            break;

        case Ability_Component_TYPE_RESISTANCE_CHANGE:
            break;
        case Ability_Component_TYPE_STATUS_CHANGE:
            break;
        case Ability_Component_TYPE_STATUS_THRESHOLD_CHANGE:
            break;
        case Ability_Component_TYPE_MAX:
            break;
        }

        //seperator for the text
        string_builder_append_c_string(ability_text, ". ");
    }


    return ability_text;
}


#endif
