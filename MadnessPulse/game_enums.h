#ifndef GAME_ENUMS_H
#define GAME_ENUMS_H


// abilities //
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

    //ECS_MultiElemental, idk about this
    ECS_Reversal,
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


    //ECS_Psychic
    //ECS_Chaos, (the element of true randomness, also for the sequel)
    //ECS_Lightning, both of these are for the sequel
    //ECS_Wind,
    //ECS_Time,
    //ECS_God,
    Damage_Type_MAX,
} Damage_Type;

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
}Drain_Types;

typedef enum MP_Types
{
    MP_Types_MP_Amount,
    MP_Types_MP_Percent,
    MP_Types_MP_To_Full,
    MP_Types_MP_To_Zero,
}MP_Types;

// probably won't use this, maybe for conjure or reversal checks
typedef enum Ability_Changer_Type
{
    Ability_Changer_Type_Add_Ability,
    Ability_Changer_Type_Remove_Ability,
}Ability_Changer_Type;

typedef enum Action_Changer_Type
{
    Action_Changer_Type_ActionAdd,
    Action_Changer_Type_ActionRemove,
} Action_Changer_Type;

typedef enum High_Low_Type
{
    High_Low_Type_Low,
    High_Low_Type_High,
}High_Low_Type;


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
}Resistance_Type;

typedef enum Turn_Activation_Type
{
    ECS_None,
    ECS_Start,
    ECS_End,
    ECS_Start_End,
    ECS_First_Start,
    ECS_Final_End,
}Turn_Activation_Type;

typedef enum Ability_Activation_Type
{
    Ability_Activation_Type_Normal,
    Ability_Activation_Type_Status,
    Ability_Activation_Type_Turn,
    Ability_Activation_Type_Reversal,
    // component type that gets checked first, to make sure the ability is allowed to activate, otherwise the ability will fail
    Ability_Activation_Type_Conditional,
    // no need to put conjure here, as it just activates normally
}Ability_Activation_Type;

typedef enum Ability_Trigger_Target
{
    Ability_Trigger_Target_Caster,
    Ability_Trigger_Target_Target,
    Ability_Trigger_Target_Allies,
    Ability_Trigger_Target_Enemies,
    Ability_Trigger_Target_All,
}Ability_Trigger_Target;

typedef enum Conjure_Type
{
    Conjure_Type_None,
    Conjure_Type_IceDemon,
    Conjure_Type_MiracleDemon,
    Conjure_Max,
}Conjure_Type;


// Battle Manager


// Camera Perspective
//TODO: have this replace the current blueprint Camera Type
typedef enum Turned_Based_Camera_Type
{
    Turned_Based_Camera_Type_None,
    // for turn player
    Turned_Based_Camera_Type_TurnPlayerCamera,
    // who we are targeting and who is bieng damaged
    Turned_Based_Camera_Type_AllyTargetingCamera,
    Turned_Based_Camera_Type_EnemyTurnCamera,
    Turned_Based_Camera_Type_TargetEveryoneCamera,
    //Look at attacker
    Turned_Based_Camera_Type_AttackCamera,
}Turned_Based_Camera_Type;

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
}Action_Consideration_Type;


typedef enum Character_State
{
    Character_State_None,
    Character_State_Alive,
    Character_State_Dead,
    Character_State_Revive,
}Character_State;


// probably won't use this, maybe for conjure or reversal checks
typedef enum Battle_Info_Life_Time_Type
{
    Battle_Info_Life_Time_Type_TurnEndLifeTime,
    Battle_Info_Life_Time_Type_TurnStartLifeTime,
    Battle_Info_Life_Time_Type_RewindLifeTime,
    Battle_Info_Life_Time_Type_FightLifeTime,
}Battle_Info_Life_Time_Type;


// UNIT//


typedef enum Character_Name
{
    CharacterName_None,
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
    CharacterName_MAX
} Character_Name;


typedef enum Character_Type
{
    ECS_Player,
    ECS_Enemy,
} Character_Type;



#endif //GAME_ENUMS_H
