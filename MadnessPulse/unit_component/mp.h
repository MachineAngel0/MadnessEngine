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
    mp_component.CurrentMP = MP_DEFAULT_AMOUNT;
    mp_component.MaxMP = MP_DEFAULT_AMOUNT;
    mp_component.MaxAllowedMP = mp_component.MaxMP*2;
}


void ClampMP(MP_Component* mp_component)
{
    mp_component->CurrentMP = clamp_float(mp_component->CurrentMP, -mp_component->MaxMP * 2, mp_component->MaxMP * 2);
}

float GetMPPercent(MP_Component* mp_component)
{
    return mp_component->CurrentMP / mp_component->MaxMP;
}

void ChangeMP(MP_Component* mp_component, float MPChangeValue)
{
    DEBUG("Called Change MP by Amount");

    bool InfiniteMPFlag = UnitOwner->SpecialAbilityFlagComponent->SpecialAbilityFlagList.InfiniteMPFlag;
    bool PermanentMPFlag = UnitOwner->SpecialAbilityFlagComponent->SpecialAbilityFlagList.PermanentInfiniteMPFlag;
    bool RampartDestruction = UnitOwner->SpecialAbilityFlagComponent->SpecialAbilityFlagList.RampartTaunt;

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

    mp_component->CurrentMP += AdjustedMPValue;
    ClampMP(mp_component);
}

void ReduceMP(MP_Component* mp_component, float MPChangeValue)
{
   DEBUG("Called Reduce MP by Amount");


    mp_component->CurrentMP -= MPChangeValue;
    ClampMP(mp_component);
}

void ChangeMPByPercent(MP_Component* mp_component, float MPPercentValue)
{
    DEBUG("Called Change MP by Percent");

    mp_component->CurrentMP += mp_component->CurrentMP * MPPercentValue;
    ClampMP(mp_component);
}

void ReduceMPByPercent(MP_Component* mp_component, float MPPercentValue)
{
    DEBUG("Called Reduce MP by Percent");

    mp_component->CurrentMP -= mp_component->CurrentMP * MPPercentValue;
    ClampMP(mp_component);
}

void MPToFull(MP_Component* mp_component)
{
    DEBUG("Called Set MP To Full");

    mp_component->CurrentMP = mp_component->MaxMP;
    ClampMP(mp_component);
}

void MPToZero(MP_Component* mp_component)
{
    DEBUG("Called Set MP To Zero");

    mp_component->CurrentMP = 0;
    ClampMP(mp_component);
}

bool CanAffordAbilityCost(Unit* caster, MP_Component* mp_component, float MPCost)
{
    // if our mp is greater than the cost, or we have the infinite mp flag on

    bool InfiniteMPFlag = caster->SpecialAbilityFlagComponent->SpecialAbilityFlagList.InfiniteMPFlag;
    bool PermanentMPFlag = caster->SpecialAbilityFlagComponent->SpecialAbilityFlagList.PermanentInfiniteMPFlag;
    bool RampartDestruction = caster->SpecialAbilityFlagComponent->SpecialAbilityFlagList.RampartTaunt;


    //make any adjustments to MP cost based on any set abilities
    float AdjustedMPCost = MPCost;
    if (RampartDestruction)
    {
        //100% increase
        AdjustedMPCost = MPCost * 2.0f;
    }

    if (mp_component->CurrentMP >= AdjustedMPCost || InfiniteMPFlag || PermanentMPFlag)
    {
        return true;
    }
    return false;
}


#endif //MP_H
