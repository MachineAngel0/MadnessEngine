#ifndef AUGMENT_COMPONENT_H
#define AUGMENT_COMPONENT_H

#include <stdbool.h>

#include "game_enums.h"




#define DAMAGE_AND_NEGATION_UPPER_LIMITS 100
#define DAMAGE_AND_NEGATION_LOWER_LIMITS -100
#define STATUS_UPPER_LIMITS 100
#define STATUS_LOWER_LIMITS 0

Augment_Component augment_component_create()
{
    Augment_Component augment_component;

    augment_component.StatusPoints = {0};
    augment_component.damage_points = 0;
    augment_component.negation_points = 0;

    augment_component.DamageAndNegationUpperLimitPoints = DAMAGE_AND_NEGATION_UPPER_LIMITS;
    augment_component.DamageAndNegationLowerLimitPoints = DAMAGE_AND_NEGATION_LOWER_LIMITS;

    augment_component.StatusUpperLimitPoints = STATUS_UPPER_LIMITS;
    augment_component.StatusLowerLimitPoints = STATUS_LOWER_LIMITS;
}


/*STATUS POINTS*/
int augment_component_clamp(Augment_Component augment_component, int Value, int LowerLimit, int UpperLimit);


void ChangeStatus(Damage_Type AugmentTypeToAffect, int ChangeAmount);
void SetStatus(Damage_Type AugmentTypeToAffect, int ChangeAmount);
void ResetStatus(Damage_Type damage_type);
void ResetAllStatus();
int ReturnStatusPoints(Damage_Type AugmentDamageType);
void MaxAllStatus();


float ReturnStatusModifierForDamage(Damage_Type AugmentDamageType);
float ReturnStatusModifierForStatus(Damage_Type AugmentDamageType);

float ReturnStatusModifier(Damage_Type AugmentDamageType);

int ReturnAllAugmentSum();
int ReturnAllAugmentAverage();

void SwapStatusValues(Damage_Type DamageType1, Damage_Type DamageType2);


/*STATUS POINTS UTILITY*/

//TODO: make these generic since they can definetly be made generic
bool IsStatusMax(Damage_Type AugmentType);
bool IsStatusMin(Damage_Type AugmentType);

void ReturnHighestStatusMap(Damage_Type HighestType, int HighestValue);
Damage_Type ReturnHighestStatusType();
void ReturnLowestStatusMap(Damage_Type LowestType, int LowestValue);
Damage_Type ReturnLowestStatusType();
float ReturnAverageForStatusMap();


/*DAMAGE POINTS*/
void ChangeDamage(int ChangeAmount);
void SetDamage(int PercentAmount);
void ResetDamage();
float ReturnDamage();
int ReturnDamagePoints();

void MaxDamage();

/*NEGATION POINTS*/
void ChangeNegation(int ChangeAmount);
void SetNegation(int NewNegationAmount);
void ResetNegation();

float ReturnDamageNegation();
int ReturnNegationPoints();


void MaxNegation();


#endif //AUGMENT_COMPONENT_H
