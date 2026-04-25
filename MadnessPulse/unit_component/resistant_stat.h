#ifndef RESISTANT_STAT_COMPONENT_H
#define RESISTANT_STAT_COMPONENT_H
#include <stdbool.h>

#include "game_enums.h"
#include "game_structs.h"


void Resistance_Stats_Component_init_default(Resistance_Stats_Component* resistance_component)
{
    for (int i = 0; i < Damage_Type_MAX; ++i)
    {
        resistance_component->Resistance[i] = Resistance_Type_Neutral;
    }
}

Resistance_Type ReturnResistanceType(Resistance_Stats_Component* resistance_component, Damage_Type DamageType)
{
    return resistance_component->Resistance[DamageType];
}

static float CalculateResistanceValue(Resistance_Type ResistanceType)
{
    switch (ResistanceType)
    {
    case(Resistance_Type_SuperWeak):
        return 1.4f;
    case(Resistance_Type_Weak):
        return 1.2f;
    case(Resistance_Type_Neutral):
        return 1.0f;
    case(Resistance_Type_Strong):
        return 0.8f;
    case(Resistance_Type_Resistant):
        return 0.7f;
    default:
        return 1.0f;
    }
}

float ReturnResistanceValue(Resistance_Stats_Component* resistance_component, Damage_Type DamageType)
{
    return CalculateResistanceValue(resistance_component->Resistance[DamageType]);
}

bool IsDamageTypeRedirectOrSpread(Resistance_Stats_Component* resistance_component, const Damage_Type DamageType)
{
    return resistance_component->Resistance[DamageType] == Resistance_Type_Redirect
        || resistance_component->Resistance[DamageType] == Resistance_Type_Spread;
}

void ChangeResistanceType(Resistance_Stats_Component* resistance_component, Damage_Type DamageType,
                          Resistance_Type NewResistanceType)
{
    resistance_component->Resistance[DamageType] = NewResistanceType;
}

void ChangeAllResistancesToType(Resistance_Stats_Component* resistance_component, Resistance_Type new_resistance_type)
{
    for (int i = 0; i < Damage_Type_MAX; ++i)
    {
        resistance_component->Resistance[i] = new_resistance_type;
    }

    // for (TPair<EDamageType, EResistanceType> Conditional : Resistance)
    // {
    // Resistance[Conditional.Key] = new_resistance_type;
    // }
}


/*TODO:
//this is here for when we change a resistance but we want to change it back to what it was before
TMap<Damage_Type, Resistance_Type> DefaultResistance;
void ResetResistanceToDefault();
*/

#endif
