#ifndef DAMAGE_H
#define DAMAGE_H

#include "game_enums.h"



float damage_value_look_up_table[Damage_Status_Type_MAX] =
{
    [Damage_Status_Type_Dreamy] = 10.0f,
    [Damage_Status_Type_Low] = 25.0f,
    [Damage_Status_Type_Delusion] = 50.0f,
    [Damage_Status_Type_High] = 75.0f,
    [Damage_Status_Type_Visionary] = 100.0f,
    [Damage_Status_Type_Imaginary] = 200.0f,

    //literally just for physical abilities
    [Damage_Status_Type_Physical] = 800.0f,
};

#define SINGLE_TARGET_STATUS_MODIFIER_VALUE 3.0f;
#define MULTI_TARGET_STATUS_MODIFIER_VALUE 0.40f;

/*typedef struct Damage_Ability
{
    //Final Values Used in Calculations, use custom enum below, if I want to set these manually
    float DamageTypeToDamage[Element_Type_MAX];
    float StatusValues[Element_Type_MAX];


    //Enums used to make creating abilities go quicker, anything set to custom will need to be set manually in the above maps
    Damage_Status_Type DamageToType[Element_Type_MAX];
    Damage_Status_Type StatusToType[Element_Type_MAX]; /* = {
        [Damage_Type_Physical] = Damage_Status_Type_Physical,
        [Damage_Type_Fire] = Damage_Status_Type_Low,
        [Damage_Type_Ice] = Damage_Status_Type_Low,
        [Damage_Type_Poison] = Damage_Status_Type_Low,
        [Damage_Type_Blood] = Damage_Status_Type_Low,
        [Damage_Type_Heavenly] = Damage_Status_Type_Low,
        [Damage_Type_Abyss] = Damage_Status_Type_Low,
        [Damage_Type_Madness] = Damage_Status_Type_Low,
        [Damage_Type_Insanity] = Damage_Status_Type_Low,
    };#1#




} Damage_Ability;*/


void damage_component_create(Ability_Component* Ability_component, const Element_Type element, const float damage_amount)
{
    Ability_component->type = Ability_Component_Type_Damage;
    Ability_component->data.damage = (Damage_Component){.element = element, .damage = damage_amount};
}


void damage_ability(Unit* unit, Damage_Component damage_ability)
{
    unit->health_component.current_health -= damage_ability.damage;
}

/*
void ActivateComponentAbility_Implementation(Game_State* GameState, TArray<Unit*>& Targets,
                                             int StatusCount = 1);


// Damage Stuff //
void SetDamageAndStatusValues();


// Calculates Damage
void ProcessDamage(Unit*& UnitCaster,
                   TArray<Unit*>& Targets,
                   TArray<Unit*>& SpreadTargets);
void DamageCalculation(TTuple<Damage_Type, float> DamageMap,
                       Unit*& UnitCaster,
                       Unit*& CurrentTarget, bool IsRedirect = false,
                       Unit* RedirectUnit = nullptr);

bool IsMultiElemental() const;

//TODO: have a validation of some kind so the action manager is not accessing an empty map
Damage_Type ReturnHighestDamageType();
float ReturnHighestDamageValue();
float ReturnDamageValueByType(Damage_Type DamageTypeToSearch);
bool HasDamageType(const Damage_Type DamageTypeToSearch) const;


// Calculates status build up
void ProcessStatus(Unit*& UnitCaster,
                   TArray<Unit*>& Targets);


void ReturnHighestStatusTypeAndValue(Damage_Type& DamageType, float& BuildUpAmount);
float ReturnHighestStatusBuildUpAmount();
float ReturnStatusValueByType(Damage_Type DamageTypeToSearch);
*/

#endif //DAMAGE_H
