#ifndef DAMAGE_H
#define DAMAGE_H
#include <stdbool.h>

#include "game_enums.h"


typedef enum Damage_Status_Type
{
    ECS_Custom,

    Damage_Status_Type_Dreamy,
    Damage_Status_Type_Low,
    Damage_Status_Type_Delusion,
    Damage_Status_Type_High,
    Damage_Status_Type_Visionary,
    Damage_Status_Type_Imaginary,

    Damage_Status_Type_Physical,
    Damage_Status_Type_MAX,

    /*
    ECS_Illusion UMETA(DisplayName = "Illusion"),
    ECS_Asphixiation UMETA(DisplayName = "Fogotten"),
    ECS_Demise UMETA(DisplayName = "Fogotten"),
    ECS_armageddon UMETA(DisplayName = "Fogotten"),
    ECS_lunacy UMETA(DisplayName = "lunacy"),
    */

    // Power scaling
    // Imaginary > Visionary > Heavy > Moderate > Competent > Weak > Brittle
    // Damage and Status
} Damage_Status_Type;

float DamageValueLookUpTable[Damage_Status_Type_MAX] =
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

//Note: these values are indicating how many times a status trigger will happen given everything at neutral - so heavy is 4 status triggers guaranteed
float StatusValueLookUpTable[Damage_Status_Type_MAX] =
{
    //0.5,1,2,3,4,5,10
    [Damage_Status_Type_Dreamy] = 0.5f,
    [Damage_Status_Type_Low] = 1.0f,
    [Damage_Status_Type_Delusion] = 2.0f,
    [Damage_Status_Type_High] = 4.0f,
    [Damage_Status_Type_Visionary] = 6.0f,
    [Damage_Status_Type_Imaginary] = 10.0f,
    // buff and debuff abilties bieng at high status build up just feels right
    // damage abilties bieng at low just feels right

    //literally just for physical abilities
    [Damage_Status_Type_Physical] = 0.0f,
};

typedef struct Damage_Ability
{
    //Final Values Used in Calculations, use custom enum below, if I want to set these manually
    float DamageTypeToDamage[Damage_Type_MAX];
    float StatusValues[Damage_Type_MAX];


    //Enums used to make creating abilities go quicker, anything set to custom will need to be set manually in the above maps
    Damage_Status_Type DamageToType[Damage_Type_MAX];
    Damage_Status_Type StatusToType[Damage_Type_MAX]; /* = {
        [Damage_Type_Physical] = Damage_Status_Type_Physical,
        [Damage_Type_Fire] = Damage_Status_Type_Low,
        [Damage_Type_Ice] = Damage_Status_Type_Low,
        [Damage_Type_Poison] = Damage_Status_Type_Low,
        [Damage_Type_Blood] = Damage_Status_Type_Low,
        [Damage_Type_Heavenly] = Damage_Status_Type_Low,
        [Damage_Type_Abyss] = Damage_Status_Type_Low,
        [Damage_Type_Madness] = Damage_Status_Type_Low,
        [Damage_Type_Insanity] = Damage_Status_Type_Low,
    };*/


    // float SingleTargetStatusModifierValue = 3.0f;
    // float MultiTargetStatusModifierValue = 0.40f;
    float damage; // TODO: remove, its here just for testing

} Damage_Ability;

Damage_Ability damage_ability_create(float damage_amount)
{
    Damage_Ability damage_ability = {0};
    damage_ability.damage = damage_amount;
    return damage_ability;
}

void damage_ability(Unit* unit, void* data)
{
    Damage_Ability* damage_ability = (Damage_Ability*) data;

    unit->health_component.current_health -= damage_ability->damage;
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
