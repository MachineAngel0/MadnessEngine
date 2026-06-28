#ifndef MP_H
#define MP_H


#include <stdbool.h>
#include "game_structs.h"
#include "logger.h"
#include "math_lib.h"

#define MP_DEFAULT_AMOUNT 100

MP_Component mp_component_create()
{
    MP_Component mp_component;
    mp_component.current_mp = MP_DEFAULT_AMOUNT;
    mp_component.max_mp = MP_DEFAULT_AMOUNT;
    mp_component.MaxAllowedMP = mp_component.max_mp*2;
    return mp_component;
}

void mp_component_create_default(MP_Component* mp_component)
{
    mp_component->current_mp = MP_DEFAULT_AMOUNT;
    mp_component->max_mp = MP_DEFAULT_AMOUNT;
    mp_component->MaxAllowedMP = mp_component->max_mp*2;
}



void mp_component_clamp(MP_Component* mp_component)
{
    mp_component->current_mp = clamp_int(mp_component->current_mp, -mp_component->max_mp * 2, mp_component->max_mp * 2);
}

float mp_component_get_mp_percent(MP_Component* mp_component)
{
    return mp_component->current_mp / mp_component->max_mp;
}

void mp_component_change_mp(Unit* unit, MP_Component* mp_component, const f32 mp_change_value)
{
    DEBUG("Called Change MP by Amount");

    bool InfiniteMPFlag = unit->special_ability_flag_list_component.InfiniteMPFlag;
    bool PermanentMPFlag = unit->special_ability_flag_list_component.PermanentInfiniteMPFlag;
    bool RampartDestruction = unit->special_ability_flag_list_component.RampartTaunt;

    // if infinite mp flag is set, then we don't need to do anything
    if (InfiniteMPFlag || PermanentMPFlag)
    {
        return;
    }

    f32 AdjustedMPValue = mp_change_value;
    if (RampartDestruction)
    {
        AdjustedMPValue = mp_change_value * 2;
    }

    mp_component->current_mp += AdjustedMPValue;
    mp_component_clamp(mp_component);
}

void mp_component_mp_to_full(MP_Component* mp_component)
{
    DEBUG("Called Set MP To Full");

    mp_component->current_mp = mp_component->max_mp;
    mp_component_clamp(mp_component);
}
void mp_component_mp_to_zero(MP_Component* mp_component)
{
    DEBUG("Called Set MP To Zero");

    mp_component->current_mp = 0;
    mp_component_clamp(mp_component);
}


bool mp_component_can_afford_ability_cost(Unit* caster, float mp_cost)
{
    // if our mp is greater than the cost, or we have the infinite mp flag on

    bool infinite_mp_flag = caster->special_ability_flag_list_component.InfiniteMPFlag;
    bool permanent_mp_flag = caster->special_ability_flag_list_component.PermanentInfiniteMPFlag;
    bool rampart_destruction = caster->special_ability_flag_list_component.RampartTaunt;


    //make any adjustments to MP cost based on any set abilities
    float adjusted_mp_cost = mp_cost;
    if (rampart_destruction)
    {
        //100% increase
        adjusted_mp_cost = mp_cost * 2.0f;
    }

    if (caster->mp_component.current_mp >= adjusted_mp_cost || infinite_mp_flag || permanent_mp_flag)
    {
        return true;
    }
    return false;
}


#endif //MP_H
