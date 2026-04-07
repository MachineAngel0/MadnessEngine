#ifndef RESISTANT_STAT_COMPONENT_H
#define RESISTANT_STAT_COMPONENT_H
#include <stdbool.h>

#include "game_enums.h"
#include "game_structs.h"


Resistance_Type ReturnResistanceType(Resistance_Stats_Component* resistance_component, Damage_Type DamageType)
{
    if(Resistance.Contains(DamageType))
    {
        return Resistance[DamageType];
    }
    return EResistanceType::ECS_Neutral;
}

float ReturnResistanceValue(Resistance_Stats_Component* resistance_component, Damage_Type DamageType)
{
    if(Resistance.Contains(DamageType))
    {
        return CalculateResistanceValue(Resistance[DamageType]);
    }
    return 1.0f;
}

bool IsDamageTypeRedirectOrSpread(Resistance_Stats_Component* resistance_component, const Damage_Type DamageType)
{
    if(Resistance.Contains(DamageType))
    {
        return Resistance[DamageType] == EResistanceType::ECS_Redirect || Resistance[DamageType] == EResistanceType::ECS_Spread;
    }


    return false;
}

static float CalculateResistanceValue(Resistance_Stats_Component* resistance_component, Resistance_Type ResistanceType)
{
    switch (ResistanceType)
    {
    case(EResistanceType::ECS_SuperWeak):
        return 1.4f;
    case(EResistanceType::ECS_Weak):
        return 1.2f;
    case(EResistanceType::ECS_Neutral):
        return 1.0f;
    case(EResistanceType::ECS_Strong):
        return 0.8f;
    case(EResistanceType::ECS_Resistant):
        return 0.7f;
    default:
        return 1.0f;
    }
}

void ChangeResistanceType(Resistance_Stats_Component* resistance_component, Damage_Type DamageType,
                          Resistance_Type NewResistanceType)
{
	Resistance[DamageType] = NewResistanceType;
}

void ChangeAllResistancesToType(Resistance_Stats_Component* resistance_component, Resistance_Type NewResistanceType)
{
    for (TPair<EDamageType, EResistanceType> Conditional : Resistance)
    {
        Resistance[Conditional.Key] = NewResistanceType;
    }
}


/*TODO:
//this is here for when we change a resistance but we want to change it back to what it was before
TMap<Damage_Type, Resistance_Type> DefaultResistance;
void ResetResistanceToDefault();
*/

#endif
