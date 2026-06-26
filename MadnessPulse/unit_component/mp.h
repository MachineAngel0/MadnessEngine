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
    mp_component->current_mp = clamp_float(mp_component->current_mp, -mp_component->max_mp * 2, mp_component->max_mp * 2);
}

float mp_component_GetMPPercent(MP_Component* mp_component)
{
    return mp_component->current_mp / mp_component->max_mp;
}

void mp_component_ChangeMP(Unit* unit, MP_Component* mp_component, float MPChangeValue)
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

    float AdjustedMPValue = MPChangeValue;
    if (RampartDestruction)
    {
        AdjustedMPValue = MPChangeValue * 2;
    }

    mp_component->current_mp += AdjustedMPValue;
    mp_component_clamp(mp_component);
}

void mp_component_reduce_mp(MP_Component* mp_component, float MPChangeValue)
{
   DEBUG("Called Reduce MP by Amount");


    mp_component->current_mp -= MPChangeValue;
    mp_component_clamp(mp_component);
}

void mp_component_change_mp_by_percent(MP_Component* mp_component, float MPPercentValue)
{
    DEBUG("Called Change MP by Percent");

    mp_component->current_mp += mp_component->current_mp * MPPercentValue;
    mp_component_clamp(mp_component);
}

void mp_component_reduce_mp_by_percent(MP_Component* mp_component, float MPPercentValue)
{
    DEBUG("Called Reduce MP by Percent");

    mp_component->current_mp -= mp_component->current_mp * MPPercentValue;
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

bool mp_component_can_afford_ability_cost(Unit* caster, float MPCost)
{
    // if our mp is greater than the cost, or we have the infinite mp flag on

    bool InfiniteMPFlag = caster->special_ability_flag_list_component.InfiniteMPFlag;
    bool PermanentMPFlag = caster->special_ability_flag_list_component.PermanentInfiniteMPFlag;
    bool RampartDestruction = caster->special_ability_flag_list_component.RampartTaunt;


    //make any adjustments to MP cost based on any set abilities
    float AdjustedMPCost = MPCost;
    if (RampartDestruction)
    {
        //100% increase
        AdjustedMPCost = MPCost * 2.0f;
    }

    if (caster->mp_component.current_mp >= AdjustedMPCost || InfiniteMPFlag || PermanentMPFlag)
    {
        return true;
    }
    return false;
}


#endif //MP_H
