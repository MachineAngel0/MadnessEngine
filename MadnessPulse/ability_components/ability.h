#ifndef ABILITIES_H
#define ABILITIES_H


#include "heal.h"
#include "damage.h"
#include "asserts.h"
#include "game_enums.h"
#include "game_structs.h"




//ability registry






void ability_add_component(Ability* ability, Ability_Component* component)
{
    ability->components[ability->component_count] = *component;
    ability->component_count++;
}


void ability_type_process(Unit* unit, Ability_Component* ability_data)
{
    //all components have type as their first param, so if for some reason its not set it will be considered invalid
    switch (ability_data->data.type)
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
        ability_type_process(unit, ability_component);
    }
}







#endif
