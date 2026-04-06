#ifndef GAME_TYPES_H
#define GAME_TYPES_H
#include <stdbool.h>

#include "game_enums.h"


// UNITS //

typedef struct Health_Component
{
    float current_health;
    float max_health;
    //An upper limit so that health does not go past a certain point like 9999
    float max_health_limit;
    float min_health_limit;

    //flags
    bool death_animation_flag;
    bool revive_animation_flag;
} Health_Component;

typedef struct Action_Component
{
    int MaxActionsAvailable;
    int ActionsAvailable;
} Action_Component;


typedef struct Augment_Component
{
    Damage_Type StatusPoints[Damage_Type_MAX];
    /*
 = {
     [Damage_Type_Physical] = {0},
     [Damage_Type_Fire] = {0},
     [Damage_Type_Ice] = {0},
     [Damage_Type_Poison] = {0},
     [Damage_Type_Blood] = {0},
     [Damage_Type_Heavenly] = {0},
     [Damage_Type_Abyss] = {0},
     [Damage_Type_Madness] = {0},
     [Damage_Type_Insanity] = {0},
 };*/

    int damage_points;
    int negation_points;

    int DamageAndNegationUpperLimitPoints;
    int DamageAndNegationLowerLimitPoints;

    int StatusUpperLimitPoints;
    int StatusLowerLimitPoints;
} Augment_Component;




typedef struct Unit
{
    Character_Type CharacterType;
    Character_Name Name;
    Character_State CharacterState;


    Action_Component action_component;
    Health_Component HealthComponent;
    /*
    inventory InventoryManagerComponent;
    UMPComponent MPComponent;
    UAugmentStageComponent AugmentStageComponent;
    UResistanceStatsComponent ResistanceStatsComponent;
    UStatusStatComponent StatusStatComponent;
    UConjureDeathListComponent ConjureDeathListComponent;
    UReversalList ReversalListComponent;
    UBattleTurnInformation BattleTurnInformationComponent;
    UChargeListComponent ChargeListComponent;
    USpecialAbilityFlagsComponent SpecialAbilityFlagComponent;


    //VFX Component
    UNiagaraComponent* VFX_Buff;
    UNiagaraComponent* VFX_Damage;
    UNiagaraComponent* VFX_Fusion;
    UNiagaraComponent* VFX_Redirect;
*/



    /*
    UAnimMontage* AttackAnimation;

    UAnimMontage* DamagedAnimation;

    UAnimMontage* FusionAnimation;

    UAnimMontage* IdleAnimation;
    UAnimMontage* DeathAnimation;
    UAnimMontage* ReviveAnimation;
    UAnimMontage* VictoryAnimation;

    //POPUPS
    void SpawnDamagePopUp(EResistanceType ResistanceType, float PopUpValue);
    void SpawnAugmentPopUp(EDamageType DamageType, float PopUpValue);
    void SpawnResistancePopUp(EDamageType DamageType, EResistanceType ResistanceType);
    void SpawnGenericPopUp(float Value, FColor Color, const FString& PopUpName);
    void SpawnFailedPopUp();

    */

} Unit;


// ABILITIES //




#endif //GAME_TYPES_H
