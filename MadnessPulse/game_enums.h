#ifndef GAME_ENUMS_H
#define GAME_ENUMS_H

// UNIT//
typedef enum Character_Name
{
    Character_Name_None,
    //DEBUG ENEMEIS
    Character_Name_Red_Jester,
    Character_Name_Clown,
    Character_Name_Puppet,
    Character_Name_Doll,

    //Players
    Character_Name_Madness_Progenitor,
    Character_Name_Madness_ButterFly,
    Character_Name_Madness_Wolf, //TODO: replace with something else
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


typedef enum Character_State
{
    Character_State_Alive,
    Character_State_Dead,
    Character_State_Revive,
} Character_State;

// ABILITIES //

typedef enum Ability_Component_Type
{
    Ability_Component_TYPE_INVALID,
    Ability_Component_TYPE_HEAL,
    Ability_Component_Type_Damage,
    Ability_Component_TYPE_MP_CHANGE,
    Ability_Component_TYPE_AUGMENT_CHANGE,
    Ability_Component_TYPE_CHARGE,
    Ability_Component_TYPE_CONJURE,
    Ability_Component_TYPE_DRAIN,
    Ability_Component_TYPE_ACTION_CHANGE,
    Ability_Component_TYPE_ABILITY_CHANGE,
    Ability_Component_TYPE_RESISTANCE_CHANGE,
    Ability_Component_TYPE_STATUS_CHANGE,
    Ability_Component_TYPE_STATUS_THRESHOLD_CHANGE,
    Ability_Component_TYPE_MAX,
} Ability_Component_Type;


typedef enum Ability_Name
{
    Ability_Name_INVALID,

    //sort by missions

    //test/debug
    Ability_Name_DEBUG_HEAL,
    Ability_Name_DEBUG_DAMAGE,
    Ability_Name_Madness_Test,
    Ability_Name_INSANITY_Test,
    Ability_Name_REVERSAL_TEST,

    // Starting abilties
    Ability_Name_Pass,

    //mission 1
    //mission 2
    //mission 3


    //AI abilities


    Ability_Name_MAX,
} Ability_Name;


typedef enum Reversal_Duration
{
    Reversal_Duration_Once,
    Reversal_Duration_Casters_Turn, // resets when the casters turn begins
    Reversal_Duration_Permanent,
}Reversal_Duration;


typedef enum Target_Area_Affect
{
    Target_Area_Affect_Single_Target,
    Target_Area_Affect_Target_All,
} Target_Area_Affect;

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

typedef enum Ability_Icon_Type
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
} Ability_Icon_Type;

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


typedef enum Element_Type
{
    Element_Type_Madness,
    Element_Type_Insanity,

    Element_Type_Physical,
    Element_Type_Fire,
    Element_Type_Ice,

    Element_Type_Poison,
    Element_Type_Blood,
    Element_Type_Heavenly,
    Element_Type_Abyss,

    //Element_Type_Colorless //kinda like almight, but the main point of it is that it gets around a lot of reversals
    //Element_Type_Psychic
    //Element_Type_Chaos, (the element of true randomness, also for the sequel)
    //Element_Type_Lightning, both of these are for the sequel, does spread damage basically
    //Element_Type_Wind, //
    //Element_Type_Time, //
    //Element_Type_God,
    Element_Type_MAX,
} Element_Type;


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


typedef enum Heal_Types
{
    Heal_Types_Heal_Amount,
    Heal_Types_Heal_Setter,
    Heal_Types_Heal_Percent,
    Heal_Types_Heal_To_Full,
    Heal_Types_Heal_By_Multiplication,
    Heal_Types_MAX,
} Heal_Types;

typedef enum Status_Change_Types
{
    Status_Change_Types_StatusAmount,
    Status_Change_Types_StatusPercent,
    Status_Change_Types_StatusToSpecificPercent,
    Status_Change_Types_StatusToZero,
} Status_Change_Types;

typedef enum Status_Threshold_Types
{
    Status_Threshold_Types_Threshold_Amount,
    Status_Threshold_Types_Threshold_Specific_Value,
    Status_Threshold_Types_Threshold_By_Multiplication,
    Status_Threshold_Types_MAX,
}Status_Threshold_Types;

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

typedef enum Conjure_Type
{
    Conjure_Type_IceDemon,
    Conjure_Type_MiracleDemon,
    Conjure_Type_DamageDemon,
    //TODO: add all the other ones I intend to have
    Conjure_Type_Max,
} Conjure_Type;

typedef enum EPassiveTransferType
{
    ECS_CasterToTarget_PositiveValue,
    ECS_CasterToTarget_NegativeValue,
    ECS_CasterToTarget_DoubleLossGain,

    ECS_TargetToCaster,
    ECS_TargetToCasterNegativeValue,
    ECS_TargetToCaster_DoubleLossGain,
}EPassiveTransferType;

// Turn Based Game //

typedef enum Targeting_Direction
{
    Targeting_Direction_Left,
    Targeting_Direction_Right,
}Targeting_Direction;

typedef enum Turn_Initiative
{
    Turn_Initiative_Player,
    Turn_Initiative_Enemy,
} Turn_Initiative;

typedef enum Turn_Phase
{
    Turn_Phase_None,
    Turn_Phase_Turn_Start,
    Turn_Phase_Ability_Selection,
    Turn_Phase_Target_Select,
    Turn_Phase_Ability_Process,
    Turn_Phase_Queue_Process,
    Turn_Phase_Turn_End,
    Turn_Phase_Enemy_Turn,

    Turn_Phase_Dialogue,
    Turn_Phase_Event, // might want to specify what type of event

    Turn_Phase_Battle_Over,
} Turn_Phase;

typedef enum Action_Handler_Process_Stage
{
    ECS_None,
    ECS_Normal,
    ECS_Status,
    ECS_ReversalTrigger,
    ECS_Reversal,
    ECS_TurnComponent,
    ECS_TurnTrigger,
    ECS_TurnEnd,
} Action_Handler_Process_Stage;


typedef enum Game_UI_States
{
    Game_UI_State_None,
    Game_UI_State_Battle_Log,
    Game_UI_State_Unit_Info,
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


// probably won't use this, maybe for conjure or reversal checks
typedef enum Battle_Info_Life_Time_Type
{
    Battle_Info_Life_Time_Type_TurnEndLifeTime,
    Battle_Info_Life_Time_Type_TurnStartLifeTime,
    Battle_Info_Life_Time_Type_RewindLifeTime,
    Battle_Info_Life_Time_Type_FightLifeTime,
} Battle_Info_Life_Time_Type;


#endif //GAME_ENUMS_H
