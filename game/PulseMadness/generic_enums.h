#ifndef GENERIC_ENUMS_H
#define GENERIC_ENUMS_H


// abilities
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

    ECS_Physical,
    ECS_Fire,
    ECS_Ice,
    ECS_Poison,
    ECS_Blood,
    ECS_Heavenly,
    ECS_Abyss,
    ECS_Madness,
    ECS_Insanity,

    //ECS_MultiElemental, idk about this
    ECS_Reversal,
} EFusionType;


typedef enum Damage_Type
{
    Damage_Type_Physical,
    Damage_Type_Fire,
    Damage_Type_Ice,
    //ECS_Lightning, both of these are for the sequel
    //ECS_Wind,
    Damage_Type_Poison,
    //ECS_Psychic
    Damage_Type_Blood,
    Damage_Type_Heavenly,
    Damage_Type_Abyss,
    //ECS_Chaos, (the element of true randomness, also for the sequel)
    Damage_Type_Madness,
    Damage_Type_Insanity,
    //ECS_Time,
    //ECS_God,
} Damage_Type;

typedef enum Status_Change_Types
{
    Status_Change_Types_StatusAmount,
    Status_Change_Types_StatusPercent,
    Status_Change_Types_StatusToSpecificPercent,
    Status_Change_Types_StatusToZero,
}Status_Change_Types;

enum Drain_Types
{
    Drain_Types_DrainAmount,
    Drain_Types_DrainPercent,
    Drain_Types_DrainToFull,
    Drain_Types_DrainSetter,
};

enum MP_Types
{
    MP_Types_MP_Amount,
    MP_Types_MP_Percent,
    MP_Types_MP_To_Full,
    MP_Types_MP_To_Zero,
};

// probably won't use this, maybe for conjure or reversal checks
enum Ability_Changer_Type
{
    Ability_Changer_Type_Add_Ability,
    Ability_Changer_Type_Remove_Ability,
};

enum Action_Changer_Type
{
    ECS_ActionAdd,
    ECS_ActionRemove,
};

enum High_Low_Type
{
    High_Low_Type_Low,
    High_Low_Type_High,
};


enum Resistance_Type
{
    Resistance_Type_SuperWeak,
    Resistance_Type_Weak,
    Resistance_Type_Neutral,
    Resistance_Type_Strong,
    Resistance_Type_Resistant,
    // special types of resistances
    Resistance_Type_Redirect,
    Resistance_Type_Spread,
};

enum TurnActivationType
{
    ECS_None,
    ECS_Start,
    ECS_End,
    ECS_Start_End,
    ECS_First_Start,
    ECS_Final_End,
};

enum AbilityActivationType
{
    ECS_Normal,
    ECS_Status,
    ECS_Turn,
    ECS_Reversal,
    // component type that gets checked first, to make sure the ability is allowed to activate, otherwise the ability will fail
    ECS_Conditional,
    // no need to put conjure here, as it just activates normally
};

enum Ability_Trigger_Target
{
    Ability_Trigger_Target_Caster,
    Ability_Trigger_Target_Target,
    Ability_Trigger_Target_Allies,
    Ability_Trigger_Target_Enemies,
    Ability_Trigger_Target_All,
};

enum Conjure_Type
{
    Conjure_Type_None,
    Conjure_Type_IceDemon,
    Conjure_Type_MiracleDemon,
    Conjure_Max,
};


// Battle Manager


// Camera Perspective
//TODO: have this replace the current blueprint Camera Type
enum Turned_Based_Camera_Type
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
};

/* Enemy AI*/
//can always add more later
enum ActionConsiderationType
{
    ECS_None,
    ECS_Damage,
    ECS_StatusBuildUp,
    ECS_StatusTrigger,
    ECS_Augment,
    ECS_Heal,
};


enum Character_State
{
    Character_State_None,
    Character_State_Alive,
    Character_State_Dead,
    Character_State_Revive,
};


// probably won't use this, maybe for conjure or reversal checks
enum Battle_Info_Life_Time_Type
{
    Battle_Info_Life_Time_Type_TurnEndLifeTime,
    Battle_Info_Life_Time_Type_TurnStartLifeTime,
    Battle_Info_Life_Time_Type_RewindLifeTime,
    Battle_Info_Life_Time_Type_FightLifeTime,
};


#endif //GENERIC_ENUMS_H
