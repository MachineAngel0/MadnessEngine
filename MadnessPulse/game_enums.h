#ifndef GAME_ENUMS_H
#define GAME_ENUMS_H


// UNIT//
typedef enum Character_Name
{
    Character_Name_None,
    Character_Name_Red_Jester,

    //Players
    Character_Name_Madness_Progenitor,
    Character_Name_Madness_ButterFly,
    Character_Name_Madness_Wolf,
    Character_Name_Madness_Envoy,

    //
    Character_Name_Worshipper,

    //
    Character_Name_Burning_Soul,

    //
    Character_Name_Ice_Queen,

    //
    Character_Name_Sun_Twin,
    Character_Name_Moon_Twin,
    Character_Name_Sun_Envoy,
    Character_Name_Moon_Envoy,

    //
    Character_Name_Fusion_Mania,

    //
    Character_Name_Slime,

    //
    Character_Name_Metal_Star,

    //
    Character_Name_Persona,
    Character_Name_Mask_of_Fire,
    Character_Name_Mask_of_Ice,
    Character_Name_Mask_of_Blood,
    Character_Name_Mask_of_Poison,
    Character_Name_Mask_of_Heavenly,
    Character_Name_Mask_of_Abyss,

    //
    Character_Name_Dancer,

    //
    Character_Name_Outer_Angel,
    Character_Name_Outer_God,

    Character_Name_MAX
} Character_Name;


typedef enum Character_Type
{
    Character_Type_Player,
    Character_Type_Enemy,
} Character_Type;


// ABILITIES //

typedef enum Ability_Name
{
    Ability_Name_Madness_Test,
    Ability_Name_Madness_Buff_Test,
    Ability_Name_Shield,
    Ability_Name_BloodShield,
    Ability_Name_MAX,
} Ability_Name;


typedef enum Target_Can_Affect
{
    Target_Can_Affect_SingleTarget,
    Target_Can_Affect_TargetAll,
} Target_Can_Affect;

typedef enum Ability_Target_Type
{
    Ability_Target_Type_Self,
    Ability_Target_Type_Allies,
    Ability_Target_Type_Enemies,
    Ability_Target_Type_SelfAndAllies,
    Ability_Target_Type_SelfAndEnemies,
    Ability_Target_Type_AlliesAndEnemies,
    Ability_Target_Type_All,
} Ability_Target_Type;

typedef enum Ability_Type
{
    //all the damage based types
    Ability_Type_Physical,
    Ability_Type_Fire,
    Ability_Type_Ice,
    Ability_Type_Poison,
    Ability_Type_Blood,
    Ability_Type_Heavenly,
    Ability_Type_Abyss,
    Ability_Type_Madness,
    Ability_Type_Insanity,

    Ability_Type_MultiElemental,

    // special? yes, normal special, then special special
    Ability_Type_Abnormal, // nothing abnormal just stuff that doesn't fit in the other catergories
    Ability_Type_Conjure,
    Ability_Type_Reversal,
    Ability_Type_SpecialFlag,
    Ability_Type_Mana,
} Ability_Type;

typedef enum Fusion_Type
{
    // for things like heal they go with their primary element type so in this case heavenly
    // drain -> blood
    // augment -> augment type

    Fusion_Type_Physical,
    Fusion_Type_Fire,
    Fusion_Type_Ice,
    Fusion_Type_Poison,
    Fusion_Type_Blood,
    Fusion_Type_Heavenly,
    Fusion_Type_Abyss,
    Fusion_Type_Madness,
    Fusion_Type_Insanity,

    //Fusion_Type_MultiElemental, idk about this
    Fusion_Type_Reversal,
    Fusion_Type_MAX,
} Fusion_Type;


typedef enum Damage_Type
{
    Damage_Type_Physical,
    Damage_Type_Fire,
    Damage_Type_Ice,

    Damage_Type_Poison,
    Damage_Type_Blood,
    Damage_Type_Heavenly,
    Damage_Type_Abyss,
    Damage_Type_Madness,
    Damage_Type_Insanity,


    //Damage_Type_Psychic
    //Damage_Type_Chaos, (the element of true randomness, also for the sequel)
    //Damage_Type_Lightning, both of these are for the sequel
    //Damage_Type_Wind,
    //Damage_Type_Time,
    //Damage_Type_God,
    Damage_Type_MAX,
} Damage_Type;

typedef enum Heal_Types
{
    Heal_Types_HealAmount,
    Heal_Types_HealSetter,
    Heal_Types_HealPercent,
    Heal_Types_HealToFull,
    Heal_Types_HealByMultiplication,
    Heal_Types_MAX,
} Heal_Types;

typedef enum Status_Change_Types
{
    Status_Change_Types_StatusAmount,
    Status_Change_Types_StatusPercent,
    Status_Change_Types_StatusToSpecificPercent,
    Status_Change_Types_StatusToZero,
} Status_Change_Types;

typedef enum Drain_Types
{
    Drain_Types_DrainAmount,
    Drain_Types_DrainPercent,
    Drain_Types_DrainToFull,
    Drain_Types_DrainSetter,
    Drain_Types_MAX,
} Drain_Types;

typedef enum MP_Types
{
    MP_Types_MP_Amount,
    MP_Types_MP_Percent,
    MP_Types_MP_To_Full,
    MP_Types_MP_To_Zero,
    MP_Types_MAX,
} MP_Types;

typedef enum Charge_State
{
    Charge_State_None,
    Charge_State_Charge,
    Charge_State_HighCharge,
    Charge_State_MAX,
} Charge_State;


// probably won't use this, maybe for conjure or reversal checks
typedef enum Ability_Changer_Type
{
    Ability_Changer_Type_Add_Ability,
    Ability_Changer_Type_Remove_Ability,
    Ability_Changer_Type_MAX,
} Ability_Changer_Type;

typedef enum Action_Changer_Type
{
    Action_Changer_Type_ActionAdd,
    Action_Changer_Type_ActionRemove,
    Action_Changer_Type_MAX,
} Action_Changer_Type;

typedef enum High_Low_Type
{
    High_Low_Type_Low,
    High_Low_Type_High,
} High_Low_Type;


typedef enum Resistance_Type
{
    Resistance_Type_SuperWeak,
    Resistance_Type_Weak,
    Resistance_Type_Neutral,
    Resistance_Type_Strong,
    Resistance_Type_Resistant,
    // special types of resistances
    Resistance_Type_Redirect,
    Resistance_Type_Spread,
} Resistance_Type;

typedef enum Turn_Activation_Type
{
    Turn_Activation_Type_None,
    Turn_Activation_Type_Start,
    Turn_Activation_Type_End,
    Turn_Activation_Type_Start_End,
    Turn_Activation_Type_First_Start,
    Turn_Activation_Type_Final_End,
} Turn_Activation_Type;

typedef enum Ability_Activation_Type
{
    Ability_Activation_Type_Normal,
    Ability_Activation_Type_Status,
    Ability_Activation_Type_Turn,
    Ability_Activation_Type_Reversal,
    // component type that gets checked first, to make sure the ability is allowed to activate, otherwise the ability will fail
    Ability_Activation_Type_Conditional,
    // no need to put conjure here, as it just activates normally
} Ability_Activation_Type;

typedef enum Ability_Trigger_Target
{
    Ability_Trigger_Target_Caster,
    Ability_Trigger_Target_Target,
    Ability_Trigger_Target_Allies,
    Ability_Trigger_Target_Enemies,
    Ability_Trigger_Target_All,
} Ability_Trigger_Target;

typedef enum Conjure_Type
{
    Conjure_Type_IceDemon,
    Conjure_Type_MiracleDemon,
    Conjure_Type_DamageDemon,
    Conjure_Type_Max,
} Conjure_Type;


// Battle Manager //

typedef enum Turn_Initiative
{
    Turn_Initiative_Player,
    Turn_Initiative_Enemy,
} Turn_Initiative;

typedef enum TurnPhase
{
    TurnPhase_None,
    TurnPhase_TurnStart,
    TurnPhase_AbilitySelection,
    TurnPhase_TargetSelection,
    TurnPhase_AbilityProcessing,
    TurnPhase_QueueProcessing,
    TurnPhase_TurnEnd,
    TurnPhase_EnemyTurn,

    TurnPhase_Dialogue,
    TurnPhase_Event, // might want to specify what type of event

    TurnPhase_BattleOver,
} TurnPhase;

typedef enum Game_UI_States
{
    Game_UI_State_None,
    Game_UI_State_BattleLog,
    Game_UI_State_UnitInfo,
    Game_UI_State_Fusion,
    Game_UI_State_System,
}Turn_Based_UI_States;

// Camera Perspective
//TODO: have this replace the current blueprint Camera Type
typedef enum Turned_Based_Camera_Type
{
    Turned_Based_Camera_Type_None,
    // for turn player
    Turned_Based_Camera_Type_TurnPlayerCamera,
    // who we are targeting and who is being damaged
    Turned_Based_Camera_Type_AllyTargetingCamera,
    Turned_Based_Camera_Type_EnemyTurnCamera,
    Turned_Based_Camera_Type_TargetEveryoneCamera,
    //Look at attacker
    Turned_Based_Camera_Type_AttackCamera,
} Turned_Based_Camera_Type;

/* Enemy AI*/
//can always add more later
typedef enum Action_Consideration_Type
{
    Action_Consideration_Type_None,
    Action_Consideration_Type_Damage,
    Action_Consideration_Type_StatusBuildUp,
    Action_Consideration_Type_StatusTrigger,
    Action_Consideration_Type_Augment,
    Action_Consideration_Type_Heal,
    Action_Consideration_Type_MAX,
} Action_Consideration_Type;


typedef enum Character_State
{
    Character_State_Alive,
    Character_State_Dead,
    Character_State_Revive,
} Character_State;


// probably won't use this, maybe for conjure or reversal checks
typedef enum Battle_Info_Life_Time_Type
{
    Battle_Info_Life_Time_Type_TurnEndLifeTime,
    Battle_Info_Life_Time_Type_TurnStartLifeTime,
    Battle_Info_Life_Time_Type_RewindLifeTime,
    Battle_Info_Life_Time_Type_FightLifeTime,
} Battle_Info_Life_Time_Type;


#endif //GAME_ENUMS_H
