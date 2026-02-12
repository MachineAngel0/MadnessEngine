#ifndef UNIT_BASE_H
#define UNIT_BASE_H

typedef enum ECharacterName
{
    ECS_None,
    ECS_Red_Jester,

    ECS_Madness_Progenitor,
    ECS_Madness_ButterFly,
    ECS_Madness_Wolf,
    ECS_Madness_Envoy,

    ECS_Worshipper,

    ECS_Burning_Soul,

    ECS_Ice_Queen,

    ECS_Sun_Twin,
    ECS_Moon_Twin,
    ECS_Sun_Envoy,
    ECS_Moon_Envoy,

    // fusion mania
    ECS_Secret_Of_The_Tribe,

    ECS_Slime,

    ECS_Metal_Star,

    ECS_Persona,
    ECS_Mask_of_Fire,
    ECS_Mask_of_Ice,
    ECS_Mask_of_Blood,
    ECS_Mask_of_Poison,
    ECS_Mask_of_Heavenly,
    ECS_Mask_of_Abyss,

    ECS_Dancer,

    ECS_Outer_Angel,
    ECS_Outer_God_Something,
}ECharacterName;


typedef enum ECharacterType
{
    ECS_Player,
    ECS_Enemy,
}ECharacterType;


typedef struct unit
{
    TObjectPtr<UActionComponent> ActionComponent;
    TObjectPtr<UHealthComponent> HealthComponent;
    TObjectPtr<UInventoryManagerComponent> InventoryManagerComponent;
    TObjectPtr<UMPComponent> MPComponent;
    TObjectPtr<UAugmentStageComponent> AugmentStageComponent;
    TObjectPtr<UResistanceStatsComponent> ResistanceStatsComponent;
    TObjectPtr<UStatusStatComponent> StatusStatComponent;
    TObjectPtr<UConjureDeathListComponent> ConjureDeathListComponent;
    TObjectPtr<UReversalList> ReversalListComponent;
    TObjectPtr<UBattleTurnInformation> BattleTurnInformationComponent;
    TObjectPtr<UChargeListComponent> ChargeListComponent;
    TObjectPtr<USpecialAbilityFlagsComponent> SpecialAbilityFlagComponent;


    //VFX Component
    UNiagaraComponent* VFX_Buff;
    UNiagaraComponent* VFX_Damage;
    UNiagaraComponent* VFX_Fusion;
    UNiagaraComponent* VFX_Redirect;




    ECharacterType CharacterType = ECharacterType::ECS_Enemy;


    ECharacterName Name = ECharacterName::ECS_None;


    ECharacterState CharacterState = ECharacterState::ECS_Alive;












    //TODO: might want some soft asserts at begin play for these, making sure they are set

    UAnimMontage* AttackAnimation;

    UAnimMontage* DamagedAnimation;

    UAnimMontage* FusionAnimation;
    /* these animations don't need to get called directly, they are part of the anim blueprint / anim state machine
    UAnimMontage* IdleAnimation;
    UAnimMontage* DeathAnimation;
    UAnimMontage* ReviveAnimation;
    UAnimMontage* VictoryAnimation;
    */

    //POPUPS
    void SpawnDamagePopUp(EResistanceType ResistanceType, float PopUpValue);
    void SpawnAugmentPopUp(EDamageType DamageType, float PopUpValue);
    void SpawnResistancePopUp(EDamageType DamageType, EResistanceType ResistanceType);
    void SpawnGenericPopUp(float Value, FColor Color, const FString& PopUpName);
    void SpawnFailedPopUp();

} unit;


#endif
