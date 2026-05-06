#ifndef GAME_TYPES_H
#define GAME_TYPES_H

#include <stdbool.h>

//array enums
ARRAY_GENERATE_TYPE(Character_Name)
DARRAY_GENERATE_TYPE(Character_Name)


/// ABILITIES ///


typedef struct Heal_Component
{
    Ability_Component_Type type;

    Heal_Types heal_type;
    float heal_amount;
    //should be false by default
    bool heal_only_if_dead;
} Heal_Component;

typedef struct Damage_Component
{
    Ability_Component_Type type;
    Element_Type element;
    float damage; // TODO: remove, its here just for testing
} Damage_Component;

typedef struct UAbilityAdderComponent
{
    Ability_Name AbilitiesToAdd;
    u32 count;
} UAbilityAdderComponent;

typedef struct UAbilityRemoverComponent
{
    Ability_Name AbilitiesToRemove;
    u32 count;
} UAbilityRemoverComponent;

typedef struct URemoveAllAbilityComponent
{
    Ability_Name AbilityToRemove;
    // bool RemoveAllAbilities;
} URemoveAllAbilityComponent;

typedef struct UActionRemoveComponent
{
    u32 action_amount_to_remove;
} UActionRemoveComponent;

typedef struct UActionAddComponent
{
    u32 action_amount_to_add;
} UActionAddComponent;

typedef struct UAugmentChangerComponent
{
    // TMap<EDamageType, int> AugmentTypeToChangeValue;
    Element_Type damage_type;
    int augment_change_value;
} UAugmentChangerComponent;

typedef struct Charge_Attack_Component
{
    // TMap<EDamageType, EChargeState> ElementsToCharge;
    Element_Type damage_type;
    Charge_State charge_state;
} Charge_Attack_Component;

typedef struct Conjure_Component
{
    Conjure_Type ConjureType;
} Conjure_Component;

typedef struct Drain_Component
{
    // TMap<EDrainTypes, float> DrainTypes;
    Drain_Types drain_type;
    float drain_amount;
} Drain_Component;

typedef struct UHealComponent
{
    // TMap<EHealTypes, float> HealTypes;
    Heal_Types heal_type;
    float heal_amount;
    bool HealOnlyIfDead; // = false;
} UHealComponent;

typedef struct UMPChangerComponent
{
    // TMap<EMPTypes, float> MPMap;
    MP_Types mp_type;
    float mp_amount;
} UMPChangerComponent;

typedef struct UPermanentActionChangerComponent
{
    Action_Changer_Type ActionType;
    u32 amount_to_change;
} UPermanentActionChangerComponent;

typedef struct UResistanceChangerComponent
{
    // TMap<EDamageType, EResistanceType> ResistanceToChangeMap;
    Resistance_Type ResistanceToChangeMap[Element_Type_MAX];
} UResistanceChangerComponent;

typedef struct UResistanceChangerRandomComponent
{
    int RandomAmount; // = 1;
    //basically saying the resistance we are changing to is not random
    Resistance_Type ResistanceToChangeTo;
} UResistanceChangerRandomComponent;

typedef struct UResistanceResetterComponent
{
    //NOTE: this straight up had nothing in it
    bool unimplemented;
} UResistanceResetterComponent;


typedef struct UReversalComponent
{
    //TODO: this might actually make sense as bitflags
    // and there is no reason to have reversals on things like fucking heal types, just on a heal in general
    Reversal_Duration reversal_duration;

    bool AnythingNotDamage; // = false;
    bool AnyDamage; // = false;

    bool DamageConditions[Element_Type_MAX];
    bool Heal;
    bool Drain;
    bool Mp;
    bool AugmentCondition[Element_Type_MAX];
    bool ActionChangeCondition[Action_Changer_Type_MAX];
    bool AbilityChangeCondition[Ability_Changer_Type_MAX];
    bool NegationCondition; //= false;
    bool DamagePassiveCondition; // = false;
} UReversalComponent;


typedef struct UStatusChangerComponent
{
    // EStatusChangeTypes StatusChangeType = EStatusChangeTypes::ECS_StatusPercent;
    // TMap<EDamageType, float> StatusTypeToChangeAmount;
    Status_Change_Types status_change_types;
    Element_Type damage_type;
    float status_change_amount;
} UStatusChangerComponent;

typedef struct USpecialAbilityFlagSetterComponent
{
    bool unimplemented;
    // Special_Ability_Flag_List_Component flags_to_set; //TODO:
    //FSpecialAbilityFlagList FlagsToDisable;//TODO: if i did want this, it would be its own special type of component
} USpecialAbilityFlagSetterComponent;

typedef struct UStatusThresholdChanger
{
    Status_Threshold_Types StatusThresholdType;
    // TMap<EDamageType, float> StatusThresholdChangeMap;
    Element_Type damage_type;
    float amount;
} UStatusThresholdChanger;

typedef  struct FTurnComponentDuration
{
    int EffectLength;
    int TurnsUntilTriggered;
} FTurnComponentDuration;

typedef struct UTurnComponentBase
{
    Turn_Activation_Type TurnActivation;
    FTurnComponentDuration TurnComponentDuration;
} UTurnComponentBase;

//Battle Info
typedef struct DamageBounce
{
    bool ImpactMirror; // = false;
    float DamageMultiplier; // = 1.0f;
} DamageBounce;

typedef struct UDrainBounce
{
    //TODO: split these into different components
    //only select one
    bool DrainTwice; // = false;
    bool HealAfterDrain; // = false;
} UDrainBounce;

typedef struct UHealBounce
{
    /* will damage the unit by the health gain multiplied by a multiplier */
    float HealBounceMultiplier; // = 2.0f;
} UHealBounce;

typedef struct URewindStats
{
    bool RewindHealth; // = false;
    bool RewindStatus; // = false;
    bool RewindDamage; // = false;
    bool RewindNegation; // = false;
} URewindStats;

typedef struct Heal_Based_On_Damage_Taken_Component
{
    /*meant to be used as a reversal, heals for any damage the caster just took*/
    float HealPercentage; // = 0.8;
} Heal_Based_On_Damage_Taken_Component;


//Boss Specific

typedef struct UAbilitySwapper
{
    int NumberOfAbilitiesToSwap; // = 1;
} UAbilitySwapper;

typedef struct UDeadUnitDamage
{
    bool unimplemented;
    // FDeadAllyBonus DeadAllyBonus; //TODO:
    // EDamageType DamageType; ??idk what this is for and doesn't make much sense anyway
} UDeadUnitDamage;

typedef struct UEmperorMadness
{
    /*Removes all abilities that were not originally in the players inventory ex: madness abilities or fusion abilities */
    //NOTE: FOUND IT LIKE THAT
    bool unimplemented;
} UEmperorMadness;

typedef struct UEmperorsInsanity
{
    /*Removes all abilities from the target based on how many times that ability was used */
    int CountForAbilityToBeRemoved; // = 3;
} UEmperorsInsanity;

typedef struct UEmperorZero
{
    /*Resets everything on the battleinfo, except maybe turn info */
    //NOTE: special ability flag
    bool unimplemented;
} UEmperorZero;

typedef struct UKillByCondition
{
    //TODO: split these out into their own components
    /*checks a condition and then determines if it should kill the target */
    bool UseKillByHealthAbovePercent; // = false;
    float HealthPercentForKill; // = 0.7f;
    bool UseKillByManaAbovePercent; // = false;
    float MPPercentForKill; // = 0.7f;

    /*
    bool HasNotTakenDamage= false;
    bool HadNotChangedStatus= false;
    bool HadNotChangedNegation = false;
*/
} UKillByCondition;


typedef struct UMPActionGainer
{
    Action_Changer_Type ActionType; // = EActionChangerType::ECS_ActionAdd;
    float MPPerActionConversion; // = 1000.f;
} UMPActionGainer;

typedef struct UMPDamage
{
    /*
 * TODO:
 * Does a flat damage number based on the casters Mp with a damage multiplier like *10 on the targets
 * this does not use any mp, use mp changer to have mp go down
 */
    MP_Types MPType;
    float MPValue;
    float DamageMultiplier; // = 2.0f;
} UMPDamage;

typedef struct UMPHeal
{
    /*
  * Note: does not use up mp. if you want to use the mp changer
  * uses the casters mp as the heal value on the target(which will most likely be yourself)
  */
    float HealMultiplier; // = 1.0;
} UMPHeal;

typedef struct UMPStealer
{
    /*
     * Takes Mp from the target and add it to the caster
    */

    //do not choose to full
    MP_Types MPStealType; // = EMPTypes::ECS_MPPercent;
    //make a positive value
    float MPStealAmount;
} UMPStealer;

typedef struct UPrimaveraBloom
{
    /*Deal damage for each resistance on the caster that is not changed from superweak to redirect (or something similar) */
    Resistance_Type ResistanceTypeToCheck; // = EResistanceType::ECS_SuperWeak;
    float DamageAmount; // = 1000.0f;
} UPrimaveraBloom;

typedef struct UPrimaveraFaith
{
    /**
  *checks the opponents inventory for any copies of primavera worship, removes all of them, and for each one removed does damage
  */
    //TODO: these can both just be changed to ability name
    String AbilityName; // = "Primavera Faith";
    // primavera faith here
    // TSubclassOf<AAbilityBase> AbilityClass;
    float DamageAmount; // = 1000.0f;
} UPrimaveraFaith;


typedef struct UPrimaveraFlower
{
    //Gain health based on the damage and negation, meant to have the damage and negation set to zero when it's done, use another ability
    float HealMultiplier; // = 100.0f;
} UPrimaveraFlower;

typedef struct UPrimaveraLight
{
    //every time primavera is hit, this will check what element she was hit by and change the corresponding resistances to redirect
    bool unimplemented;
} UPrimaveraLight;


//Conditional
typedef struct UConditionalFatComponent
{
    bool TestCondition; // = false;

    bool CasterMPAtZero; // = false;

    bool CasterHasPositiveDamage; // = false;
} UConditionalFatComponent;


//Custom
typedef struct UDamageBasedOnPassive
{
    bool UseStatus;
    // TSet<Element_Type> DamageTypeToCheckFor; //TODO:
    bool UseDamage;
    bool UseNegation;


    float DamageAmountPerPoint; // = 100;
    int ApplyDamageEveryPointAmount; // = 1;
} UDamageBasedOnPassive;

typedef struct UDrainBattleHealDamage
{
    /*
     *Values are based on all allies and then we use that to heal or damage the target
     */

    // TSet<EBattleInfoLifeTimeType> LifeTimeType; //TODO:

    bool MustBeDead; // = true;

    //only one of these should be active
    bool ShouldHeal; // = false;
    bool ShouldDamage; // = false;
} UDrainBattleHealDamage;


typedef struct UDrainBattleInfoComponents
{
    //TODO: All this can be moved out to a its own typedef struct for reuse, since this is going to keep coming up
    // the only thing is having two bool specifically set for use on what values we want to use
    // TSet<EBattleInfoLifeTimeType> LifeTimeType; //TODO:

    //translate to +1 to someonething for health Drained
    float DivisionValue; // = 0;
    // TSet<EDamageType> StatusToGain; //TODO:
    bool GainDefense;
    bool GainDamage;
    bool DealDamageOfDrainLost;
} UDrainBattleInfoComponents;

typedef struct UFusionManiaComponent
{
    //TODO: im assuming this sets a special ability flag
    bool unimplemented;
    float KillDamageAmount; // = 999999.0f;
} UFusionManiaComponent;


typedef struct UHealAsphyxiation
{
    /*
     * Targets a dead party member and take all their health until they hit the max negative value
     */
    bool unimplemented;
} UHealAsphyxiation;

typedef struct UHealByTargetHealth
{
    bool unimplemented;
} UHealByTargetHealth;

typedef struct UHealthPoolingTransfer
{
    /**
 * take health from the targets and give them to the caster until their health is full
 */
    bool unimplemented;
} UHealthPoolingTransfer;

typedef struct UHealthSacrifice
{
    /* Will sacrifice casters health until the is at 100% HP */
    bool unimplemented;
} UHealthSacrifice;

typedef struct UHighLowTypeAugmentComponent
{
    // TMap<EDamageType, int> AugmentTypeToChangeValue;
    int AugmentTypeToChangeValue[Element_Type_MAX];
    High_Low_Type HighLow;
} UHighLowTypeAugmentComponent;


typedef struct UPassiveAntiPooling
{
    /**
     * we take the casters passives, and then reduce the opponents passive by that amount
     */
    // TSet<EDamageType> ElementsToCheckFor;
    bool ElementsToCheckFor[Element_Type_MAX];

    bool PoolDamage; // = false;
    bool PoolStatus; // = false;
    bool PoolNegation; // = false;
} UPassiveAntiPooling;


typedef struct UPassivePooling
{
    /*
 * Takes the combined points of a passive from their allies and adds to onto the caster
 */
    // TSet<EDamageType> ElementsToCheckFor;
    bool ElementsToCheckFor[Element_Type_MAX];

    bool PoolDamage; // = false;
    bool PoolStatus; // = false;
    bool PoolNegation; // = false;
} UPassivePooling;


typedef struct UPassiveTransferComponent
{


    //TODO: it would be funny to transfer any negative augments onto another character,
    // like if you debuff the enemy then they will use this ability and debuff you instead
    // EPassiveTransferType TransferFlow;//TODO:

    int MinMaxValueForTransfer;// = 0;
    // void PositiveTransfer(AUnitBase* FromTarget, AUnitBase* ToTarget);
    // void NegativeTransfer(AUnitBase* FromTarget, AUnitBase* ToTarget);
    // void AnyTransfer(AUnitBase* FromTarget, AUnitBase* ToTarget);

    //Basically just make sure that the passive type we check for a positive value
    // TSet<EDamageType> StatusPassiveToTransfer;
    bool StatusPassiveToTransfer[Element_Type_MAX];
    bool DamagePassivesToTransfer; // = false;
    bool NegationPassiveToTransfer; // = false;
} UPassiveTransferComponent;

typedef struct URandomAbilityRemover
{
    int RemovalAmount; // = 1;
} URandomAbilityRemover;


typedef struct URandomAugmentComponent
{
    int AugmentChangeAmount; // = 0;
} URandomAugmentComponent;

typedef struct URandomDamageComponent /*: public UDamageComponent*/ // TODO:
{
    bool ExcludeInsanity; // = false;
    Damage_Status_Type IntendedDamageAmount; // = EDamageStatusType::ECS_High;
    Damage_Status_Type IntendedDamageBuildUpAmount; // = EDamageStatusType::ECS_High;
} URandomDamageComponent;

typedef struct USacrificialBomb
{
    /**
 * will kill all allies excpet caster and inflict the health as damage to the target
 */
    Element_Type element_type; // = EDamageType::ECS_Fire;
    //set this to either 2.0f for double or 0.5f for half damage
    float DamageMultiplier; // = 1.0f;
} USacrificialBomb;


typedef struct UStatusBarDrainComponent
{
    /*
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
    };*/
    bool GainStatus; // = false;
    bool GainDamage; // = false;
    bool GainNegation; // = false;
    bool GainHealth; // = false;
    bool DealDamage; // = false;
} UStatusBarDrainComponent;


typedef struct UStatusDrainDisfiguredMass
{
    /*Drains from Allies and does some sort of effect onto the Targets(most likely your opponents)*/
    /*TSet<EDamageType> StatusBarsToDrain{
        {EDamageType::ECS_Physical},
        {EDamageType::ECS_Fire},
        {EDamageType::ECS_Ice},
        {EDamageType::ECS_Poison},
        {EDamageType::ECS_Blood},
        {EDamageType::ECS_Heavenly},
        {EDamageType::ECS_Abyss},
        {EDamageType::ECS_Madness},
        {EDamageType::ECS_Insanity},
    };*/
    float DivisionValueBonus; // = 1.0f;

    /*TSet<EDamageType> TypesForGainStatus{
        {EDamageType::ECS_Physical},
        {EDamageType::ECS_Fire},
        {EDamageType::ECS_Ice},
        {EDamageType::ECS_Poison},
        {EDamageType::ECS_Blood},
        {EDamageType::ECS_Heavenly},
        {EDamageType::ECS_Abyss},
        {EDamageType::ECS_Madness},
        {EDamageType::ECS_Insanity},
    };*/
    bool AffectStatus; // = false;
    bool AffectDamage; // = false;
    bool AffectNegation; // = false;
    bool DealDamage; // = false;
} UStatusDrainDisfiguredMass;

typedef struct UTradeActionChanger
{
    // will make the add/lose action change irrelevant, will remove all from caster and add them to one target
    bool LoseAll; // = true;

    //if we dont lose all then we use this amount, ideally we clamp it to the max moves we have left,
    //but maybe going into turn debt is funny as well
    // TMap<EActionChangerType, int> ActionTypeToActionChange;
    int ActionTypeToActionChange[Action_Changer_Type_MAX];
} UTradeActionChanger;

typedef struct UAugmentRedistribution
{
    /*
 * TODO: wouldn't be the worse idea to make this a passive redistribution, if needed
 */

    //NOTE: from what i can tell this averages all the augment values and redistributed them evenly
    bool unimplemented;
} UAugmentRedistribution;

typedef struct UAugmentSetterComponent
{
    // TMap<EDamageType, int> AugmentTypeToSetterValue;
    int AugmentTypeToSetterValue[Element_Type_MAX];
    bool SpecialCondition; // = false;
    int MustBeBelowCertainValue; // = 0;
} UAugmentSetterComponent;

//Passive


typedef struct UAugmentSwapper
{
    /*Swap the values of the targets status values at random*/
    int SwapAttempts; // = 3;
} UAugmentSwapper;


typedef struct UBonusBasedOnStatus
{
    /*we gain bonuses based on the status point value of the selected elements, bonus is specifically given to the caster*/

    // TSet<EDamageType> ElementsToGainBonusFrom;
    bool ElementsToGainBonusFrom[Element_Type_MAX];


    //this gets multiplied by the points we get from the status values
    float BonusPerPoint; // = 1;

    bool GainDamage; // = false;
    bool GainNegation; // = false;

    // TSet<EDamageType> StatusBarElementToIncrease;
    bool StatusBarElementToIncrease[Element_Type_MAX];
    bool GainStatusBar; // = false; //TODO:, not done yet
} UBonusBasedOnStatus;

typedef struct UDamageNegationComponent
{
    //values are intended to be percents
    int DamageNegationChangeAmount; // = 0;
} UDamageNegationComponent;

typedef struct UDamagePassive
{
    //values are intended to be percents
    // these need to be small values like 0.05 = 5%
    int DamagePassiveChangeAmount; // = 0;
} UDamagePassive;

typedef struct UHighLowAugmentComponent
{
    /**
 * Gets the highest augment in general and changes it by the amount we specify
 */

    High_Low_Type HighLow;

    bool ToZero; // = false;
    int ValueChange; // = 0;
} UHighLowAugmentComponent;

typedef struct UHighLowDamage
{
    /*
     * Finds the target with the highest defense or attack and reduces it to zero or by a set amount
     *
     */

    bool ToZero; // = false;
    int ValueChange; //= 0;
} UHighLowDamage;

typedef struct UHighLowNegation
{
    bool ToZero; // = false;
    int ValueChange; // = 0;
} UHighLowNegation;

typedef struct UNegationBasedOnHealth
{
    /*Will give targets defense boost based on casters health */


    int DivisionValue; // = 1;
} UNegationBasedOnHealth;


typedef struct UNegationPassiveSetter
{
    /*will set negation to the specific value*/

    //values are intended to be percents
    // these need to be small values like 0.05 = 5%
    int SetToValue; // = 0;
} UNegationPassiveSetter;


typedef  struct UPassiveReverse
{
    /*Turn any negative passive (status, damage, negation) positive */
    bool ReverseStatus; // = false;
    bool ReverseDamage; // = false;
    bool ReverseNegation; //= false;
} UPassiveReverse;



typedef struct Ability_Component
{

    //is it a normal, reversal etc
    Ability_Activation_Type activation_type;
    //single or multitarget
    Target_Area_Affect target_can_affect;
    //who is this targeting, enemies, allies, self etc
    Ability_Target_Type ability_target;

    //ability switch lookup to find which function to execute
    union
    {
        // NOTE: default to this if nothing is set, and 0 is an invalid type
        // NOTE: all ability components must have ability component type on them, as type if implicit based on the component
        Ability_Component_Type type;

        Heal_Component heal;
        Damage_Component damage;
    }data;

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
    String overflow_trigger_text; // = "NA";

    String lore_text; //= "NA";

    // Fusion_Type fusion_type; // = EFusionType::ECS_Fire;

    //every ability only gets one normal attack, primary tag, primarily used to determine the icon type
    // Ability_Type primary_ability_type; // = EAbilityType::ECS_Physical;

    Ability_Target_Type ability_target_type;
    Target_Area_Affect ability_target_area;

    u32 ability_action_cost; // = 1;
    float mp_cost; // = 1.0f;
} Ability_Info;

struct Ability_UI_INFO
{
    String ability_name; // = "Ability Not Named";
    String ability_text; // "Implement Text Please";
    String status_trigger_text; // = "NA";

    //every ability only gets one normal attack, primary tag, primarily used to determine the icon type
    Ability_Icon_Type Icon_Type;
    Ability_Icon_Type Secondary_Icon_Type;

    Ability_Target_Type ability_target_type;
    Target_Area_Affect targets_can_affect;

    u32 ability_action_cost; // = 1;
    float mp_cost; // = 1.0f;
};

ARRAY_GENERATE_TYPE(Ability)
ARRAY_GENERATE_TYPE(Ability_Component)


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

typedef struct MP_Component
{
    //MP Struct
    float CurrentMP;
    float MaxMP;
    float MaxAllowedMP;
} MP_Component;


typedef struct Augment_Component
{
    Element_Type StatusPoints[Element_Type_MAX];
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


typedef struct Resistance_Stats_Component
{
    Resistance_Type Resistance[Element_Type_MAX];
    /*= {
	    {EDamageType::ECS_Physical, EResistanceType::ECS_Neutral},
        {EDamageType::ECS_Fire, EResistanceType::ECS_Neutral},
        {EDamageType::ECS_Ice, EResistanceType::ECS_Neutral},
        {EDamageType::ECS_Poison, EResistanceType::ECS_Neutral},
        {EDamageType::ECS_Blood, EResistanceType::ECS_Neutral},
        {EDamageType::ECS_Heavenly, EResistanceType::ECS_Neutral},
        {EDamageType::ECS_Abyss, EResistanceType::ECS_Neutral},
        {EDamageType::ECS_Madness, EResistanceType::ECS_Neutral},
        {EDamageType::ECS_Insanity, EResistanceType::ECS_Neutral},
        };
*/
} Resistance_Stats_Component;

typedef struct Status_Stat_Component
{
    f32 StatusInfo[Element_Type_MAX];
    /*=
    {
            {EDamageType::ECS_Physical, 0},
            {EDamageType::ECS_Fire, 0},
            {EDamageType::ECS_Ice, 0},
            {EDamageType::ECS_Poison, 0},
            {EDamageType::ECS_Blood, 0},
            {EDamageType::ECS_Heavenly, 0},
            {EDamageType::ECS_Abyss, 0},
            {EDamageType::ECS_Madness, 0},
            {EDamageType::ECS_Insanity, 0},
        };*/

    f32 StatusThreshold[Element_Type_MAX];
    /*=
    {
            {EDamageType::ECS_Physical, 100},
            {EDamageType::ECS_Fire, 100},
            {EDamageType::ECS_Ice, 100},
            {EDamageType::ECS_Poison, 100},
            {EDamageType::ECS_Blood, 100},
            {EDamageType::ECS_Heavenly, 100},
            {EDamageType::ECS_Abyss, 100},
            {EDamageType::ECS_Madness, 100},
            {EDamageType::ECS_Insanity, 100},
        };*/

    f32 LowerClampValue;
    f32 UpperClampValue;

    i32 StatusTriggerOccurrence;

    f32 LowerThresholdClampValue;
    f32 UpperThresholdClampValue;
} Status_Stat_Component;

typedef struct Character_Flags_Component
{
    bool death_animation_flag;
    bool revive_animation_flag;
} Character_Flags_Component;

typedef struct Inventory_Component
{
    //NOTE: these should not get touched but copied, as many things can
    // Ability* ability_battle_list; // list containing default abilities usable in the battle
    // Ability* ability_reserve; // list that contains all moves unlocked
    // Ability* Modifiable_AbilityBattleList; // list containing all abilities usable in the battle


    //NOTE : TEMP SORT OF
    // list containing all abilities usable in the battle
    Ability_Name battle_list_starting[INVENTORY_MAX_BATTLE_LIST];
    u8 battle_list_size;
    // Ability ability_battle_list_ability[INVENTORY_MAX_BATTLE_LIST];

    //list of ids to seperate tables
    Ability_Name ability_reserve[Ability_Name_MAX]; // list that contains all moves unlocked

    //TODO: this actually has to be a dynamic array
    Ability_Name* battle_list_dynamic; // list containing all abilities usable in the battle
    u8 battle_list_dynamic_size;

} Inventory_Component;



typedef struct Conjure_List_Component
{
    bool active_conjure_list[Conjure_Type_Max]; // tells us if a conjure type is active
    bool kill_player; // this could potentially be a flag on the unit ex: bool Conjure_Kill_Flag)
} Conjure_List_Component;


typedef struct Reversal_List_Component
{
    //id of the unit who hit us
    u32 unit_who_hit_us;

    // there shouldn't be a list for conjure, and turn based abilties
    bool DamageReversal[Element_Type_MAX];

    // //todo: heal, drain, buff/debuff, action changer, mp change,
    // how do we deal with things like, amount of damage done? or reversing how much someone healed by?
    // global damage list, reversal triggered. something like that

    bool HealReversal[Heal_Types_MAX];

    bool DrainReversal[Drain_Types_MAX];

    bool MPReversal[MP_Types_MAX];

    bool AugmentReversal[Element_Type_MAX];
    bool NegationPassiveReversal;
    bool DamagePassiveReversal;

    bool ActionChangeReversal[Action_Changer_Type_MAX];

    bool AbilityChangeReversal[Ability_Changer_Type_MAX];
} Reversal_List_Component;

typedef struct Charge_List_Component
{
    Charge_State charge_list[Element_Type_MAX];
    /*=
{
        {EDamageType::ECS_Physical, EChargeState::ECS_None},
        {EDamageType::ECS_Fire, EChargeState::ECS_None},
        {EDamageType::ECS_Ice, EChargeState::ECS_None},
        {EDamageType::ECS_Poison, EChargeState::ECS_None},
        {EDamageType::ECS_Blood, EChargeState::ECS_None},
        {EDamageType::ECS_Heavenly, EChargeState::ECS_None},
        {EDamageType::ECS_Abyss, EChargeState::ECS_None},
        {EDamageType::ECS_Madness, EChargeState::ECS_None},
        {EDamageType::ECS_Insanity, EChargeState::ECS_None},
    };
*/
    //after a charge is used, it should be marked, so that it cant be used again
    //or we can just reset it the moment its used
    bool ChargeTypesMarkedForReset[Element_Type_MAX];

    //percent based bonus
    float ChargeDamagePercent;
    float HighChargeDamagePercent;

    //flat bonuses for things like augments
    int ChargePassiveAmount;
    int HighChargePassiveAmount;
} Charge_List_Component;

typedef struct Special_Ability_Flag_List_Component
{
    //for now it resets at the characters first turn start


    //user dies if they do not fusion
    bool FusionMania;
    //increases damage based on damage taken by 10%
    bool BlackSun;
    //take 100% more damage
    bool EmptySun;
    //reduce damage by 50%
    bool AbyssalSun;

    //increases Mp usage by 100%
    bool RampartTaunt;

    //allows for abilities to be used without mp
    bool InfiniteMPFlag;
    bool PermanentInfiniteMPFlag;

    bool DesperateMagic;

    //will negate an ability used on the caster, up to the mirage count
    bool Mirage;
    int MirageCount; // default of 0

    //25% chance any used move will fail, does not get reset, it's permanent
    bool DanceInTheDark;

    //stops any damage but leaves buildup
    bool NonExistence;
    //stops any build up but leaves damage
    bool TrueExistence;
} Special_Ability_Flag_List_Component;

typedef struct Unit
{
    Character_Type character_type;
    Character_Name name;
    Character_State character_state;

    Action_Component action_component;
    Health_Component health_component;
    MP_Component mp_component;
    Inventory_Component inventory_component;

    Augment_Component augment_component;
    Resistance_Stats_Component resistance_stats_component;
    Status_Stat_Component status_stat_component;
    Conjure_List_Component conjure_death_list_component;
    Reversal_List_Component reversal_list_component;
    // UBattleTurnInformation BattleTurnInformationComponent;
    Charge_List_Component charge_list_component;
    Special_Ability_Flag_List_Component special_ability_flag_list_component;


    /*
    //TODO: this should just transforms
    //VFX Component

    UNiagaraComponent* VFX_Buff;
    UNiagaraComponent* VFX_Damage;
    UNiagaraComponent* VFX_Fusion;
    UNiagaraComponent* VFX_Redirect;

    //POPUPS
    //TODO: these should take in a transform and be spawned in
    void SpawnDamagePopUp(EResistanceType ResistanceType, float PopUpValue);
    void SpawnAugmentPopUp(EDamageType DamageType, float PopUpValue);
    void SpawnResistancePopUp(EDamageType DamageType, EResistanceType ResistanceType);
    void SpawnGenericPopUp(float Value, FColor Color, const FString& PopUpName);
    void SpawnFailedPopUp();

    */


    Transform_Handle unit_transform;

    //pop up locations
    Transform_Handle pop_up_transform;

    //vfx locations
    Transform_Handle vfx_buff_transform;
    Transform_Handle vfx_damage_transform;
    Transform_Handle vfx_fusion_transform;
    Transform_Handle vfx_redirect_transform;


    /*
     //TODO: these should be stored in a list somewhere else
    UAnimMontage* AttackAnimation;
    UAnimMontage* DamagedAnimation;
    UAnimMontage* FusionAnimation;
    UAnimMontage* IdleAnimation;
    UAnimMontage* DeathAnimation;
    UAnimMontage* ReviveAnimation;
    UAnimMontage* VictoryAnimation;
*/
} Unit;

ARRAY_GENERATE_TYPE(Unit)

//TURN BASED GAME

typedef struct Ability_Registry
{
    //TODO: if memory becomes an issue, we can always rework this

    HASH_SET(Ability_Name)* registered_abilities;
    // HASH_SET(Ability_Name)* registered_ability_components; //TODO:  ? idk if i can identify these

    Ability_Info ability_info[Ability_Name_MAX];
    u32 ability_info_count;

    Ability ability_list[Ability_Name_MAX];
    u32 ability_count;
    // Ability_Component ability_component_list[Ability_Name_MAX * 10];
    // u32 ability_component_count;



} Ability_Registry;


//State the Ability Handler needs to use turn and reversal effects
typedef struct Turn_Trigger_Component_Info
{
    //TODO: dont store pointers
    FTurnComponentDuration TurnComponentDuration;
    Ability* Ability;
    Ability_Component* TurnComponent;
    Ability_Component_array* StatusComponents;
    Ability_Component_array* ComponentsWithTurnTag;
    Unit* Caster;
    Unit* TurnTarget;
} Turn_Trigger_Component_Info;

typedef struct Reversal_Component_Info
{
    //TODO: dont store pointers
    //who is affected, reversal component, components with reversal tag
    Ability* Ability;
    UReversalComponent* ReversalComponent;
    Ability_Component_array* ComponentsWithReversalTag;
    Unit* Caster;
    Unit* ReversalTarget; // who we put the reversal on
    bool IsPermanent;
} Reversal_Component_Info;


// this component is only responsible for telling the abilities to process themselves
// also responsible for managing any conditional or turn based effects like status trigger, poison, or reversals

// Order: Normal components, -> Status triggers -> Reversal Triggers -> Reversal Add -> turn components Add -> Turn End
typedef struct Ability_Handler
{
    bool unimplememted;
    //TODO: temp values, should probably be a dynamic array
    // Active turn and reversal components
    // Turn_Trigger_Component_Info turn_trigger_component_list[100];
    // Reversal_Component_Info reversal_component_list[100];


    // AReversalPlayBackAction* ReversalPlayBackAction;
} Ability_Handler;


//ACTION INTERFACE:
typedef void (*action_execute)(struct Madness_Pulse_Game*);
typedef void (*action_skip)(struct Madness_Pulse_Game*);

typedef struct command
{
    action_execute execute;
    action_skip skip;
    String name; // TODO: this might just be an enum type instead
    bool is_skippable; //TODO:

    //TODO: figure this out when you have an animation system in place
    // FQueueTimer QueueTimer;

    // void ExecuteNextAction();
    // void StartQueueTimer();

    // void ExecuteNextActionWithActionOverride(ATurnBasedGameMode* GameMode);
    // void StartQueueTimerWithOverride(ATurnBasedGameMode* GameMode);
    // void BeforeExecuteNextActionOverride(ATurnBasedGameMode* GameMode);
} Command;

ARRAY_GENERATE_TYPE(Command)


typedef struct Command_Handler
{
    /*FUNCTIONS AND VARIABLES*/

    //TYPE: Action
    ring_queue* command_queue;
    Command* current_command;

    // our already processed actions
    stack* command_stack;
} Command_Handler;


typedef struct Targeting_Handler
{
    //current targets available
    Character_Name_array* targets_available;
    // Character_Name_dynamic_array* targets_available;

    //target we are locked onto
    Character_Name current_lock_on_target; // TODO: make it a u32 or Character Name

    // current targets count that we are locked onto, only applicable to single target
    u8 targeting_count;



    //TODO: you can just calculate targets available size, and if its 1 or less, dont move the target lock
    // bool CanMoveTargeting;// = false;
} Targeting_Handler;

/// GAME MODE/STATE ///
//NOTE: I dont have a decent name for this rn
typedef enum Madness_Pulse_Game_State
{
    Game_State_Enum_Main_Menu,
    Game_State_Enum_Ability_Select,
    Game_State_Enum_Level_Select,
    Game_State_Enum_Turn_Based,
    Game_State_Enum_MAX,
} Madness_Pulse_Game_State;

typedef enum Main_Menu_State
{
    Main_Menu_State_Load_Save,
    Main_Menu_State_New_Save,
    Main_Menu_State_Settings,
} Main_Menu_State;


typedef struct Madness_Pulse_Game
{
    Allocator allocator;
    // Arena Current_State_Arena; //NOTE: to release data specific to the game state, but not unload literally everything
    Frame_Allocator frame_allocator;

    //refs
    Resource_System* resource_system;
    Madness_UI* madness_ui;
    Event_System* event_system;
    Input_System* input_system;


    Madness_Pulse_Game_State game_state;


    //MAIN MENU//
    //TODO: Load Save File Meta Data

    //ABILITY SELECT / LEVEL SELECT OPTIONS MENU //

    //ABILITY SELECT //


    //LEVEL SELECT //


    //TURN BASED GAME//

    //Turn Based States
    Turn_Phase turn_phase;
    Turn_Based_UI_States ui_state;


    //PLAYER AND ENEMY UNTIS
    Unit* units;
    u8 units_count;


    Unit players[MAX_PLAYER_COUNT];
    Character_Name player_character_names[MAX_PLAYER_COUNT];
    u8 player_count;
    //idk the enemy unit count at start up since its different every map
    Unit* enemies;
    Character_Name enemy_character_names[MAX_PLAYER_COUNT];
    u8 enemy_count;


    //TODO: this will likely need to realloc or put a hard limit on what can be summoned into the game
    // im thinking if a unit does get added mid fight, that they will have summoners sickness,
    // meaning they cant act until the turn queue gets reset
    ring_queue* turn_queue;
    // NOTE: right there will only be unique enemies in the game, and only a single instance allowed in play
    // FUTURE: if i need to update this in the future then do so to accomadate multiple of the same enemy
    Character_Name current_units_turn;
    Ability_Name currently_selected_ability;

    u32 picked_ability;
    Turn_Initiative starting_turn_initiative;

    Ability_Registry* ability_registry;

    Targeting_Handler* targeting_handler;
    Ability_Handler* ability_handler;
    Command_Handler* command_handler;
} Madness_Pulse_Game;


#endif //GAME_TYPES_H
