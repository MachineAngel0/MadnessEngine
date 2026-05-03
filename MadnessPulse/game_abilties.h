#ifndef GAME_ABILITIES_H
#define GAME_ABILITIES_H


#include "hash_map.h"



typedef struct Ability_Component
{
    //ability vtable lookup
    Ability_Component_Type type;
    void* data;

    //is it a normal, reversal
    Ability_Activation_Type activation_type;
    //single or multitarget
    Target_Can_Affect target_can_affect;
    //who is this targeting
    Ability_Target_Type ability_target;
} Ability_Component;

typedef struct Ability
{
    u32 id;
    Ability_Component components[MAX_ABILITY_COMPONENTS];
    u32 component_count;

    //madness or insanity type
} Ability;

typedef struct Ability_Info
{
    String ability_name; // = "Ability Not Named";
    String ability_text; // "Implement Text Please";
    String status_trigger_text; // = "NA";

    String lore_text; //= "NA";

    Fusion_Type fusion_type; // = EFusionType::ECS_Fire;

    //every ability only gets one normal attack, primary tag, primarily used to determine the icon type
    Ability_Type primary_ability_type; // = EAbilityType::ECS_Physical;

    Ability_Target_Type ability_target_type;
    Target_Can_Affect targets_can_affect;

    int ability_action_cost; // = 1;
    float mp_cost; // = 1.0f;
} Ability_Info;



//Base Abilities

typedef struct Heal_Component
{
    Heal_Types heal_type;
    float heal_amount;
    //should be false by default
    bool heal_only_if_dead;
} Heal_Component;

typedef struct Damage_Component
{
    Damage_Type type;
    float damage; // TODO: remove, its here just for testing
} Damage_Component;

struct UAbilityAdderComponent
{
    Ability_Name AbilitiesToAdd;
    u32 count;
};

struct UAbilityRemoverComponent
{
    Ability_Name AbilitiesToRemove;
    u32 count;
};

struct URemoveAllAbilityComponent
{
    Ability_Name AbilityToRemove;
    // bool RemoveAllAbilities;
};

struct UActionRemoveComponent
{
    u32 action_amount_to_remove;
};

struct UActionAddComponent
{
    u32 action_amount_to_add;
};

struct UAugmentChangerComponent
{
    // TMap<EDamageType, int> AugmentTypeToChangeValue;
    Damage_Type damage_type;
    int augment_change_value;
};

struct Charge_Attack_Component
{
    // TMap<EDamageType, EChargeState> ElementsToCharge;
    Damage_Type damage_type;
    Charge_State charge_state;
};

struct Conjure_Component
{
    Conjure_Type ConjureType;
};

struct Drain_Component
{
    // TMap<EDrainTypes, float> DrainTypes;
    Drain_Types drain_type;
    float drain_amount;
};

struct UHealComponent
{
    // TMap<EHealTypes, float> HealTypes;
    Heal_Types heal_type;
    float heal_amount;
    bool HealOnlyIfDead; // = false;
};

struct UMPChangerComponent
{
    // TMap<EMPTypes, float> MPMap;
    MP_Types mp_type;
    float mp_amount;
};


struct UPermanentActionChangerComponent
{
    Action_Changer_Type ActionType;
    u32 amount_to_change;
};

struct UResistanceChangerComponent
{
    TMap<EDamageType, EResistanceType> ResistanceToChangeMap;
};

struct UResistanceChangerRandomComponent
{
    int RandomAmount = 1;
    //basically saying the resistance we are changing to is not random
    Resistance_Type ResistanceToChangeTo;
};

struct UResistanceResetterComponent
{
    //NOTE: this straight up had nothing in it
    bool unimplemented;
};


struct UReversalComponent
{
    bool UseOnce;// = false;
    bool IsPermanent;// = false;
    bool AnythingNotDamage;// = false;
    bool AnyDamage;// = false;

    // TSet<EDamageType> DamageConditions;
    // TSet<EHealTypes> HealConditions;
    // TSet<EDrainTypes> DrainConditions;
    // TSet<EMPTypes> MpConditions;
    // TSet<EDamageType> AugmentCondition;
    // TSet<EActionChangerType> ActionChangeCondition;
    // TSet<EAbilityChangerType> AbilityChangeCondition;
    bool DamageConditions[Damage_Type_MAX];
    bool HealConditions[Heal_Types_MAX];
    bool DrainConditions[Drain_Types_MAX];
    bool MpConditions[MP_Types_MAX];
    bool AugmentCondition[Damage_Type_MAX];
    bool ActionChangeCondition[Action_Changer_Type_MAX];
    bool AbilityChangeCondition[Ability_Changer_Type_MAX];
    bool NegationCondition; //= false;
    bool DamagePassiveCondition;// = false;

};


struct UStatusChangerComponent
{
    // EStatusChangeTypes StatusChangeType = EStatusChangeTypes::ECS_StatusPercent;
    // TMap<EDamageType, float> StatusTypeToChangeAmount;
    Status_Change_Types status_change_types;
    Damage_Type damage_type;
    float status_change_amount;
};

struct USpecialAbilityFlagSetterComponent
{
    Special_Ability_Flag_List_Component flags_to_set;
    //FSpecialAbilityFlagList FlagsToDisable;//TODO: if i did want this, it would be its own special type of component
};

struct UStatusThresholdChanger
{
    Status_Threshold_Types StatusThresholdType;
    // TMap<EDamageType, float> StatusThresholdChangeMap;
    Damage_Type damage_type;
    float amount;
};

typedef struct FTurnComponentDuration
{
    int EffectLength;
    int TurnsUntilTriggered;
} FTurnComponentDuration;

struct UTurnComponentBase
{
    Turn_Activation_Type TurnActivation;
    FTurnComponentDuration TurnComponentDuration;
};

//Battle Info
struct DamageBounce
{
    bool ImpactMirror; // = false;
    float DamageMultiplier; // = 1.0f;
};

struct UDrainBounce
{
    //TODO: split these into different components
    //only select one
    bool DrainTwice = false;
    bool HealAfterDrain = false;
};

struct UHealBounce
{
    /* will damage the unit by the health gain multiplied by a multiplier */
    float HealBounceMultiplier = 2.0f;
};

struct URewindStats
{
    bool RewindHealth = false;
    bool RewindStatus = false;
    bool RewindDamage = false;
    bool RewindNegation = false;
};

struct Heal_Based_On_Damage_Taken_Component
{
    /*meant to be used as a reversal, heals for any damage the caster just took*/
    float HealPercentage; // = 0.8;
};


//Boss Specific

struct UAbilitySwapper
{
    int NumberOfAbilitiesToSwap;// = 1;
};

struct UDeadUnitDamage
{
    FDeadAllyBonus DeadAllyBonus;
    // EDamageType DamageType; ??idk what this is for and doesn't make much sense anyway
};

struct UEmperorMadness
{
    /*Removes all abilities that were not originally in the players inventory ex: madness abilities or fusion abilities */
    //NOTE: FOUND IT LIKE THAT
    bool unimplemented;
};

struct UEmperorsInsanity
{
    /*Removes all abilities from the target based on how many times that ability was used */
    int CountForAbilityToBeRemoved = 3;
};

struct UEmperorZero
{
    /*Resets everything on the battleinfo, except maybe turn info */
    //NOTE: special ability flag
    bool unimplemented;
};

struct UKillByCondition
{
    //TODO: split these out into their own components
    /*checks a condition and then determines if it should kill the target */
    bool UseKillByHealthAbovePercent = false;
    float HealthPercentForKill = 0.7f;
    bool UseKillByManaAbovePercent = false;
    float MPPercentForKill = 0.7f;

    /*
    bool HasNotTakenDamage= false;
    bool HadNotChangedStatus= false;
    bool HadNotChangedNegation = false;
*/
};


struct UMPActionGainer
{
    Action_Changer_Type ActionType;// = EActionChangerType::ECS_ActionAdd;
    float MPPerActionConversion;// = 1000.f;
};

struct UMPDamage
{
    /*
 * TODO:
 * Does a flat damage number based on the casters Mp with a damage multiplier like *10 on the targets
 * this does not use any mp, use mp changer to have mp go down
 */
    MP_Types MPType;
    float MPValue;
    float DamageMultiplier = 2.0f;
};

struct UMPHeal
{
    /*
  * Note: does not use up mp. if you want to use the mp changer
  * uses the casters mp as the heal value on the target(which will most likely be yourself)
  */
    float HealMultiplier = 1.0;
};

struct UMPStealer
{
    /*
     * Takes Mp from the target and add it to the caster
    */

    //do not choose to full
    MP_Types MPStealType;// = EMPTypes::ECS_MPPercent;
    //make a positive value
    float MPStealAmount;;
};

struct UPrimaveraBloom
{
    /*Deal damage for each resistance on the caster that is not changed from superweak to redirect (or something similar) */
    Resistance_Type ResistanceTypeToCheck;// = EResistanceType::ECS_SuperWeak;
    float DamageAmount;// = 1000.0f;
};

struct UPrimaveraFaith
{
    /**
  *checks the opponents inventory for any copies of primavera worship, removes all of them, and for each one removed does damage
  */
    //TODO: these can both just be changed to ability name
    FString AbilityName = "Primavera Faith";
    // primavera faith here
    TSubclassOf<AAbilityBase> AbilityClass;
    float DamageAmount = 1000.0f;
};


struct UPrimaveraFlower
{
    //Gain health based on the damage and negation, meant to have the damage and negation set to zero when it's done, use another ability
    float HealMultiplier = 100.0f;
};

struct UPrimaveraLight
{
    //every time primavera is hit, this will check what element she was hit by and change the corresponding resistances to redirect
    bool unimplemented;
};


//Conditional
struct UConditionalFatComponent
{
    bool TestCondition = false;

    bool CasterMPAtZero = false;

    bool CasterHasPositiveDamage = false;
};


//Custom
struct UDamageBasedOnPassive
{
    bool UseStatus;
    TSet<EDamageType> DamageTypeToCheckFor;
    bool UseDamage;
    bool UseNegation;


    float DamageAmountPerPoint = 100;
    int ApplyDamageEveryPointAmount = 1;
};

struct UDrainBattleHealDamage
{
    /*
     *Values are based on all allies and then we use that to heal or damage the target
     */

    TSet<EBattleInfoLifeTimeType> LifeTimeType;

    bool MustBeDead = true;

    //only one of these should be active
    bool ShouldHeal = false;
    bool ShouldDamage = false;
};


struct UDrainBattleInfoComponents
{
    //TODO: All this can be moved out to a its own struct for reuse, since this is going to keep coming up
    // the only thing is having two bool specifically set for use on what values we want to use
    TSet<EBattleInfoLifeTimeType> LifeTimeType;

    //translate to +1 to someonething for health Drained
    float DivisionValue = 0;
    TSet<EDamageType> StatusToGain;
    bool GainDefense;
    bool GainDamage;
    bool DealDamageOfDrainLost;
};

struct UFusionManiaComponent
{
    //TODO: im assuming this sets a special ability flag
    bool unimplemented;
    float KillDamageAmount = 999999.0f;
};


struct UHealAsphyxiation
{
    /*
     * Targets a dead party member and take all their health until they hit the max negative value
     */
    bool unimplemented;
};

struct UHealByTargetHealth
{
    bool unimplemented;
};

struct UHealthPoolingTransfer
{
    /**
 * take health from the targets and give them to the caster until their health is full
 */
    bool unimplemented;
};

struct UHealthSacrifice
{
    /* Will sacrifice casters health until the is at 100% HP */
};

struct UHighLowTypeAugmentComponent
{
    TMap<EDamageType, int> AugmentTypeToChangeValue;
    EHighLow HighLow;
};


struct UPassiveAntiPooling
{
    /**
     * we take the casters passives, and then reduce the opponents passive by that amount
     */
    TSet<EDamageType> ElementsToCheckFor;

    bool PoolDamage = false;
    bool PoolStatus = false;
    bool PoolNegation = false;
};


struct UPassivePooling
{
    /*
 * Takes the combined points of a passive from their allies and adds to onto the caster
 */
    TSet<EDamageType> ElementsToCheckFor;

    bool PoolDamage = false;
    bool PoolStatus = false;
    bool PoolNegation = false;
};


struct UPassiveTransferComponent
{
    enum EPassiveTransferType
    {
        ECS_CasterToTarget_PositiveValue,
        ECS_CasterToTarget_NegativeValue,
        ECS_CasterToTarget_DoubleLossGain,

        ECS_TargetToCaster,
        ECS_TargetToCasterNegativeValue,
        ECS_TargetToCaster_DoubleLossGain,
    };

    //TODO: it would be funny to transfer any negative augments onto another character,
    // like if you debuff the enemy then they will use this ability and debuff you instead
    EPassiveTransferType TransferFlow;

    int MinMaxValueForTransfer = 0;
    void PositiveTransfer(AUnitBase* FromTarget, AUnitBase* ToTarget);
    void NegativeTransfer(AUnitBase* FromTarget, AUnitBase* ToTarget);
    void AnyTransfer(AUnitBase* FromTarget, AUnitBase* ToTarget);

    //Basically just make sure that the passive type we check for a positive value
    TSet<EDamageType> StatusPassiveToTransfer;
    bool DamagePassivesToTransfer = false;
    bool NegationPassiveToTransfer = false;
};

struct URandomAbilityRemover
{
    int RemovalAmount = 1;
};


struct URandomAugmentComponent
{
    int AugmentChangeAmount = 0;
};

struct URandomDamageComponent /*: public UDamageComponent*/ // TODO:
{
    bool ExcludeInsanity = false;
    EDamageStatusType IntendedDamageAmount = EDamageStatusType::ECS_High;
    EDamageStatusType IntendedDamageBuildUpAmount = EDamageStatusType::ECS_High;
};

struct USacrificialBomb
{
    /**
 * will kill all allies excpet caster and inflict the health as damage to the target
 */
    EDamageType DamageType = EDamageType::ECS_Fire;
    //set this to either 2.0f for double or 0.5f for half damage
    float DamageMultiplier = 1.0f;
};


struct UStatusBarDrainComponent
{
    TSet<EDamageType> StatusBarsToDrain{
        {EDamageType::ECS_Physical},
        {EDamageType::ECS_Fire},
        {EDamageType::ECS_Ice},
        {EDamageType::ECS_Poison},
        {EDamageType::ECS_Blood},
        {EDamageType::ECS_Heavenly},
        {EDamageType::ECS_Abyss},
        {EDamageType::ECS_Madness},
        {EDamageType::ECS_Insanity},
    };
    float DivisionValueBonus = 1.0f;

    TSet<EDamageType> TypesForGainStatus{
        {EDamageType::ECS_Physical},
        {EDamageType::ECS_Fire},
        {EDamageType::ECS_Ice},
        {EDamageType::ECS_Poison},
        {EDamageType::ECS_Blood},
        {EDamageType::ECS_Heavenly},
        {EDamageType::ECS_Abyss},
        {EDamageType::ECS_Madness},
        {EDamageType::ECS_Insanity},
    };
    bool GainStatus = false;
    bool GainDamage = false;
    bool GainNegation = false;
    bool GainHealth = false;
    bool DealDamage = false;
};


struct UStatusDrainDisfiguredMass
{
    /*Drains from Allies and does some sort of effect onto the Targets(most likely your opponents)*/
    TSet<EDamageType> StatusBarsToDrain{
        {EDamageType::ECS_Physical},
        {EDamageType::ECS_Fire},
        {EDamageType::ECS_Ice},
        {EDamageType::ECS_Poison},
        {EDamageType::ECS_Blood},
        {EDamageType::ECS_Heavenly},
        {EDamageType::ECS_Abyss},
        {EDamageType::ECS_Madness},
        {EDamageType::ECS_Insanity},
    };
    float DivisionValueBonus = 1.0f;

    TSet<EDamageType> TypesForGainStatus{
        {EDamageType::ECS_Physical},
        {EDamageType::ECS_Fire},
        {EDamageType::ECS_Ice},
        {EDamageType::ECS_Poison},
        {EDamageType::ECS_Blood},
        {EDamageType::ECS_Heavenly},
        {EDamageType::ECS_Abyss},
        {EDamageType::ECS_Madness},
        {EDamageType::ECS_Insanity},
    };
    bool AffectStatus = false;
    bool AffectDamage = false;
    bool AffectNegation = false;
    bool DealDamage = false;
};

struct UTradeActionChanger
{
    TArray<FActionChangerPlayBackInfo> ActionChangerFinal;

    TObjectPtr<UNiagaraSystem> ActionVFX;

    // will make the add/lose action change irrelevant, will remove all from caster and add them to one target
    bool LoseAll = true;

    //if we dont lose all then we use this amount, ideally we clamp it to the max moves we have left,
    //but maybe going into turn debt is funny as well
    TMap<EActionChangerType, int> ActionTypeToActionChange;
};

struct UAugmentRedistribution
{
    TArray<FAugmentPlayBackInfo> NewAugmentFinal;
    TObjectPtr<UNiagaraSystem> AugmentVFX;

    /*
 * TODO: wouldn't be the worse idea to make this a passive redistribution, if needed
 */

    //NOTE: from what i can tell this averages all the augment values and redistributed them evenly
};

struct UAugmentSetterComponent
{
    TMap<EDamageType, int> AugmentTypeToSetterValue;
    bool SpecialCondition = false;
    int MustBeBelowCertainValue = 0;
};

//Passive


struct UAugmentSwapper
{
    /*Swap the values of the targets status values at random*/
    int SwapAttempts = 3;
};


struct UBonusBasedOnStatus
{
    /*we gain bonuses based on the status point value of the selected elements, bonus is specifically given to the caster*/

    TSet<EDamageType> ElementsToGainBonusFrom;


    //this gets multiplied by the points we get from the status values
    float BonusPerPoint = 1;

    bool GainDamage = false;
    bool GainNegation = false;

    TSet<EDamageType> StatusBarElementToIncrease;
    bool GainStatusBar = false; //TODO:, not done yet
};

struct UDamageNegationComponent
{
    //values are intended to be percents
    int DamageNegationChangeAmount = 0;
};

struct UDamagePassive
{
    //values are intended to be percents
    // these need to be small values like 0.05 = 5%
    int DamagePassiveChangeAmount = 0;
};

struct UHighLowAugmentComponent
{
    /**
 * Gets the highest augment in general and changes it by the amount we specify
 */

    EHighLow HighLow;

    bool ToZero = false;
    int ValueChange = 0;
};

struct UHighLowDamage
{
    /*
     * Finds the target with the highest defense or attack and reduces it to zero or by a set amount
     *
     */

    bool ToZero = false;
    int ValueChange = 0;
};

struct UHighLowNegation
{
    bool ToZero = false;
    int ValueChange = 0;
};

struct UNegationBasedOnHealth
{
    /*Will give targets defense boost based on casters health */


    int DivisionValue = 1;
};


struct UNegationPassiveSetter
{
    /*will set negation to the specific value*/

    //values are intended to be percents
    // these need to be small values like 0.05 = 5%
    int SetToValue = 0;
};


typedef struct UPassiveReverse
{
    /*Turn any negative passive (status, damage, negation) positive */
    bool ReverseStatus = false;
    bool ReverseDamage = false;
    bool ReverseNegation = false;
} UPassiveReverse;


#endif //GAME_ABILITIES_H
