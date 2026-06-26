#ifndef ABILITIES_H
#define ABILITIES_H


#include "heal.h"
#include "damage.h"
#include "asserts.h"
#include "game_enums.h"
#include "game_structs.h"


void ability_component_set_properties(Ability_Component* component,
                                      const Ability_Activation_Type activation_type,
                                      const Target_Area_Affect target_can_affect,
                                      const Ability_Target_Type ability_target)
{
    component->activation_type = activation_type;
    component->target_can_affect = target_can_affect;
    component->ability_target = ability_target;
}


void ability_add_component(Ability* ability, Ability_Component* component)
{
    ability->components[ability->component_count] = *component;
    ability->component_count++;
}


void ability_add_turn_info_component(Ability* ability,
                                     const Turn_Activation_Type turn_activation,
                                     const u8 effect_length,
                                     const u8 turns_until_triggered)
{
    Ability_Component component = {0};
    component.ability_target = Ability_Target_Type_Self;
    component.target_can_affect = Ability_Target_Type_Self;
    component.activation_type = Ability_Activation_Type_Turn_Info;

    Turn_Component_Base turn_component_base = {
        .turn_activation = turn_activation,
        .effect_length = effect_length,
        .turns_until_triggered = turns_until_triggered,
    };
    component.data.turn_base = turn_component_base;


    ability_add_component(ability, &component);
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
        heal_ability(unit, ability_data->data.heal);
        break;
    case Ability_Component_Type_Damage:
        damage_ability(unit, ability_data->data.damage);
        break;
    /*
case Ability_Component_TYPE_MP_CHANGE:
    break;
case Ability_Component_TYPE_AUGMENT_CHANGE:
    break;
case Ability_Component_TYPE_CHARGE:
    break;
case Ability_Component_TYPE_CONJURE:
    break;
case Ability_Component_TYPE_DRAIN:
    break;
case Ability_Component_TYPE_ACTION_CHANGE:
    break;
case Ability_Component_TYPE_ABILITY_CHANGE:
    break;
case Ability_Component_TYPE_RESISTANCE_CHANGE:
    break;
case Ability_Component_TYPE_STATUS_CHANGE:
    break;
case Ability_Component_TYPE_STATUS_THRESHOLD_CHANGE:
    break;
case Ability_Component_TYPE_MAX:
    break;
    */
    default:
        MASSERT_MSG(false, "ABILITY PROCESS: ABILITY TYPE NOT IMPLEMENTED IN ");
        break;
    }
}


void ability_process(Ability* ability, Unit* unit)
{
    for (u32 i = 0; i < ability->component_count; i++)
    {
        Ability_Component* ability_component = &ability->components[i];
        ability_component_process_effect(unit, ability_component);
    }
}


#endif
