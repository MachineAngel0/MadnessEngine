#ifndef GAME_TYPES_H
#define GAME_TYPES_H
#include <stdbool.h>

#include "game_enums.h"


// ABILITIES //





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


typedef struct Resistance_Stats_Component
{
    Resistance_Type Resistance[Damage_Type_MAX];
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
    float StatusInfo[Damage_Type_MAX];
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

    float StatusThreshold[Damage_Type_MAX];
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

    float LowerClampValue;
    float UpperClampValue;

    int StatusTriggerOccurrence;

    float LowerThresholdClampValue;
    float UpperThresholdClampValue;
} Status_Stat_Component;

typedef struct Character_Flags_Component
{
    bool death_animation_flag;
    bool revive_animation_flag;
} Character_Flags_Component;

typedef struct Inventory_Component
{
#define INVENTORY_MAX_BATTLE_LIST 20

    //NOTE: these should not get touched but copied, as many things can
    // Ability* ability_battle_list; // list containing all abilities usable in the battle
    // Ability* ability_reserve; // list that contains all moves unlocked
    // Ability* Modifiable_AbilityBattleList; // list containing all abilities usable in the battle

    //list of ids to seperate tables
    Ability_Name ability_battle_list[INVENTORY_MAX_BATTLE_LIST]; // list containing all abilities usable in the battle
    Ability_Name ability_reserve[Ability_Name_MAX]; // list that contains all moves unlocked

    //this actually has to be dynamic or just large
    Ability_Name* in_battle_ability_list; // list containing all abilities usable in the battle
} Inventory_Component;


typedef struct Conjure_List_Component
{
    bool active_conjure_list[Conjure_Type_Max]; // tells us if a conjure type is active
    bool KillPlayer; // this could potentially be a flag on the unit ex: bool Conjure_Kill_Flag)
} Conjure_List_Component;


typedef struct Reversal_List_Component
{
    //id of the unit who hit us
    u32 unit_who_hit_us;

    // there shouldn't be a list for conjure, and turn based abilties
    bool DamageReversal[Damage_Type_MAX];

    // //todo: heal, drain, buff/debuff, action changer, mp change,
    // how do we deal with things like, amount of damage done? or reversing how much someone healed by?
    // global damage list, reversal triggered. something like that

    bool HealReversal[Heal_Types_MAX];

    bool DrainReversal[Drain_Types_MAX];

    bool MPReversal[MP_Types_MAX];

    bool AugmentReversal[Damage_Type_MAX];
    bool NegationPassiveReversal;
    bool DamagePassiveReversal;

    bool ActionChangeReversal[Action_Changer_Type_MAX];

    bool AbilityChangeReversal[Ability_Changer_Type_MAX];
} Reversal_List_Component;

typedef struct Charge_List_Component
{
    Charge_State ChargeList[Damage_Type_MAX];
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
    bool ChargeTypesMarkedForReset[Damage_Type_MAX];

    //percent based bonus
    float ChargeDamagePercent;
    float HighChargeDamagePercent;

    //flat bonuses for things like augments
    int ChargePassiveAmount;
    int HighChargePassiveAmount;
} Charge_List_Component;

typedef struct SpecialAbilityFlagList
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
    Character_Type CharacterType;
    Character_Name name;
    Character_State CharacterState;

    Action_Component action_component;
    Health_Component health_component;
    MP_Component mp_component;
    Inventory_Component inventory_component;

    Augment_Component augment_component;
    Resistance_Stats_Component resistance_stats_component;
    Status_Stat_Component StatusStatComponent;
    Conjure_List_Component ConjureDeathListComponent;
    Reversal_List_Component ReversalListComponent;
    // UBattleTurnInformation BattleTurnInformationComponent;
    Charge_List_Component ChargeListComponent;
    Special_Ability_Flag_List_Component SpecialAbilityFlagComponent;


    /*
    //TODO: this should just transforms
    //VFX Component

    UNiagaraComponent* VFX_Buff;
    UNiagaraComponent* VFX_Damage;
    UNiagaraComponent* VFX_Fusion;
    UNiagaraComponent* VFX_Redirect;
    */


    /*
     //TODO: these should be stored in a list somewhere else
    UAnimMontage* AttackAnimation;
    UAnimMontage* DamagedAnimation;
    UAnimMontage* FusionAnimation;
    UAnimMontage* IdleAnimation;
    UAnimMontage* DeathAnimation;
    UAnimMontage* ReviveAnimation;
    UAnimMontage* VictoryAnimation;

    //POPUPS
    //TODO: these should also just be transforms
    void SpawnDamagePopUp(EResistanceType ResistanceType, float PopUpValue);
    void SpawnAugmentPopUp(EDamageType DamageType, float PopUpValue);
    void SpawnResistancePopUp(EDamageType DamageType, EResistanceType ResistanceType);
    void SpawnGenericPopUp(float Value, FColor Color, const FString& PopUpName);
    void SpawnFailedPopUp();

    */
} Unit;




//GAME MODE
//NOTE: I dont have a decent name for this rn
typedef enum Madness_Pulse_Game_State
{
    Game_State_Enum_None,
    Game_State_Enum_Main_Menu,
    Game_State_Enum_Ability_Select,
    Game_State_Enum_Mission_Select,
    Game_State_Enum_Turn_Based,
}Madness_Pulse_Game_State;

typedef enum Main_Menu_State
{
    Main_Menu_State_Load_Save,
    Main_Menu_State_New_Save,
    Main_Menu_State_Settings,
}Main_Menu_State;


typedef struct Madness_Pulse_Game
{
    Arena arena;
    // Arena Current_State_Arena; //NOTE: to release data specific to the game state, but not unload literally everything
    Frame_Arena frame_arena;
    Madness_UI* madness_ui; // ref


    Madness_Pulse_Game_State game_state;


    //MAIN MENU//

    //ABILITY SELECT//

    //MISSION SELECT//

    //TURN BASED//


    //PLAYER AND ENEMY UNTIS
    #define MAX_PLAYER_COUNT 4
    Unit players[MAX_PLAYER_COUNT];
    u32 player_count;
    //idk the enemy unit count at start up since its different every map
    Unit* enemies;
    u32 enemy_count;

    u32 total_unit_count;

    u32* turn_queue;
    u32 current_units_turn;

    //TODO: action queue for the game and do and undo those actions
    u32 picked_ability;
    Turn_Based_UI_States ui_state;

} Madness_Pulse_Game;

#endif //GAME_TYPES_H
