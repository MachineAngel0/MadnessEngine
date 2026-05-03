#ifndef ABILITIES_H
#define ABILITIES_H

#include "heal.h"
#include "damage.h"





void ability_type_process(Ability_Component_Type ability_component_type, Unit* unit, void* ability_data)
{
    switch (ability_component_type)
    {
    case Ability_Component_TYPE_HEAL:
        heal_ability(unit, ability_data);
        break;
    case Ability_Component_TYPE_DAMAGE:
        damage_ability(unit, ability_data);
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




void ability_add_component(Ability* ability, Ability_Component* component)
{
    ability->components[ability->component_count] = *component;
    ability->component_count++;
}

void ability_process(Ability* ability, Unit* unit)
{
    for (u32 i = 0; i < ability->component_count; i++)
    {
        Ability_Component* ability_component = &ability->components[i];
        ability_type_process(ability_component->type, unit, ability_component->data);
    }
}


void ability_testing()
{
    Unit unit;
    // unit.health_component = health_component_create();
    unit.health_component = (Health_Component){
        .current_health = 100, .max_health = 100, .max_health_limit = 100, .min_health_limit = 0,
        .death_animation_flag = false, .revive_animation_flag = false
    };

    Ability ability;
    memset(&ability, 0, sizeof(ability));

    Heal_Component heal_component = heal_component_create(Heal_Types_HealAmount, 10, false);

    Damage_Ability damage_component = damage_ability_create(15);

    Ability_Component a1 = {
        .type = Ability_Component_TYPE_HEAL, .data = &heal_component
    };
    Ability_Component a2 = (Ability_Component){
        .type = Ability_Component_TYPE_DAMAGE, .data = &damage_component
    };

    ability_add_component(&ability, &a1);
    ability_add_component(&ability, &a2);

    ability_process(&ability, &unit);
}


Ability get_default_heal_ability()
{
    Ability ability;

    Heal_Component heal_ability = heal_component_create(Heal_Types_HealAmount, 10, false);
    Damage_Ability damage_component = damage_ability_create(15);
    Ability_Component a1 = {
        .type = Ability_Component_TYPE_HEAL, .data = &heal_ability
    };

    ability_add_component(&ability, &a1);
    return ability;
}

Ability get_default_damage_ability()
{
    Ability ability;

    Damage_Ability damage_component = damage_ability_create(15);
    Ability_Component a1 = {
        .type = Ability_Component_TYPE_DAMAGE, .data = &damage_component
    };

    ability_add_component(&ability, &a1);
    return ability;
}




#endif
