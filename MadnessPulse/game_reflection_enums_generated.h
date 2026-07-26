#include <stddef.h>
#include "game_constants.h"
#include "game_enums.h"
#include "game_structs.h"

#include "runtime_registry.h"

const char* Ability_Changer_Type_enum_string[] = {
    [Ability_Changer_Type_Add_Ability] = "Ability_Changer_Type_Add_Ability",
    [Ability_Changer_Type_Remove_Ability] = "Ability_Changer_Type_Remove_Ability",
    [Ability_Changer_Type_MAX] = "Ability_Changer_Type_MAX",
};

const char* Charge_State_enum_string[] = {
    [Charge_State_Charge] = "Charge_State_Charge",
    [Charge_State_HighCharge] = "Charge_State_HighCharge",
    [Charge_State_MAX] = "Charge_State_MAX",
};

const char* Game_UI_States_enum_string[] = {
    [Game_UI_State_None] = "Game_UI_State_None",
    [Game_UI_State_Battle_Log] = "Game_UI_State_Battle_Log",
    [Game_UI_State_Unit_Info] = "Game_UI_State_Unit_Info",
    [Game_UI_State_Fusion] = "Game_UI_State_Fusion",
    [Game_UI_State_System] = "Game_UI_State_System",
};

const char* Ability_Secondary_Type_enum_string[] = {
    [ABILITY_SECONDARY_TYPE_MADNESS] = "ABILITY_SECONDARY_TYPE_MADNESS",
    [ABILITY_SECONDARY_TYPE_INSANITY] = "ABILITY_SECONDARY_TYPE_INSANITY",
    [ABILITY_SECONDARY_TYPE_FIRE] = "ABILITY_SECONDARY_TYPE_FIRE",
    [ABILITY_SECONDARY_TYPE_Physical] = "ABILITY_SECONDARY_TYPE_Physical",
    [ABILITY_SECONDARY_TYPE_Fire] = "ABILITY_SECONDARY_TYPE_Fire",
    [ABILITY_SECONDARY_TYPE_Ice] = "ABILITY_SECONDARY_TYPE_Ice",
    [ABILITY_SECONDARY_TYPE_Poison] = "ABILITY_SECONDARY_TYPE_Poison",
    [ABILITY_SECONDARY_TYPE_Blood] = "ABILITY_SECONDARY_TYPE_Blood",
    [ABILITY_SECONDARY_TYPE_Heavenly] = "ABILITY_SECONDARY_TYPE_Heavenly",
    [ABILITY_SECONDARY_TYPE_Abyss] = "ABILITY_SECONDARY_TYPE_Abyss",
    [ABILITY_SECONDARY_TYPE_MultiElemental] = "ABILITY_SECONDARY_TYPE_MultiElemental",
    [ABILITY_SECONDARY_TYPE_HEAL] = "ABILITY_SECONDARY_TYPE_HEAL",
    [ABILITY_SECONDARY_TYPE_MANA] = "ABILITY_SECONDARY_TYPE_MANA",
    [ABILITY_SECONDARY_TYPE_REVERSAL] = "ABILITY_SECONDARY_TYPE_REVERSAL",
    [ABILITY_SECONDARY_TYPE_Conjure] = "ABILITY_SECONDARY_TYPE_Conjure",
    [ABILITY_SECONDARY_TYPE_SpecialFlag] = "ABILITY_SECONDARY_TYPE_SpecialFlag",
    [ABILITY_SECONDARY_TYPE_MAX] = "ABILITY_SECONDARY_TYPE_MAX",
};

const char* Turn_Phase_enum_string[] = {
    [Turn_Phase_None] = "Turn_Phase_None",
    [Turn_Phase_Turn_Start] = "Turn_Phase_Turn_Start",
    [Turn_Phase_Ability_Selection] = "Turn_Phase_Ability_Selection",
    [Turn_Phase_Target_Select] = "Turn_Phase_Target_Select",
    [Turn_Phase_Ability_Process] = "Turn_Phase_Ability_Process",
    [Turn_Phase_Queue_Process] = "Turn_Phase_Queue_Process",
    [Turn_Phase_Turn_End] = "Turn_Phase_Turn_End",
    [Turn_Phase_Enemy_Turn] = "Turn_Phase_Enemy_Turn",
    [Turn_Phase_Enemy_Show_Decision_To_Player] = "Turn_Phase_Enemy_Show_Decision_To_Player",
    [Turn_Phase_Enemy_Execute_Abilties] = "Turn_Phase_Enemy_Execute_Abilties",
    [Turn_Phase_Dialogue] = "Turn_Phase_Dialogue",
    [Turn_Phase_Event] = "Turn_Phase_Event",
    [Turn_Phase_Battle_Over] = "Turn_Phase_Battle_Over",
};

const char* Character_Name_enum_string[] = {
    [Character_Name_Invalid] = "Character_Name_None",
    [Character_Name_Red_Jester] = "Character_Name_Red_Jester",
    [Character_Name_Clown] = "Character_Name_Clown",
    [Character_Name_Puppet] = "Character_Name_Puppet",
    [Character_Name_Doll] = "Character_Name_Doll",
    [Character_Name_Madness_Progenitor] = "Character_Name_Madness_Progenitor",
    [Character_Name_Madness_ButterFly] = "Character_Name_Madness_ButterFly",
    [Character_Name_Madness_Wolf] = "Character_Name_Madness_Wolf",
    [Character_Name_Madness_Envoy] = "Character_Name_Madness_Envoy",
    [Character_Name_Worshipper] = "Character_Name_Worshipper",
    [Character_Name_Burning_Soul] = "Character_Name_Burning_Soul",
    [Character_Name_Ice_Queen] = "Character_Name_Ice_Queen",
    [Character_Name_Sun_Twin] = "Character_Name_Sun_Twin",
    [Character_Name_Moon_Twin] = "Character_Name_Moon_Twin",
    [Character_Name_Sun_Envoy] = "Character_Name_Sun_Envoy",
    [Character_Name_Moon_Envoy] = "Character_Name_Moon_Envoy",
    [Character_Name_Fusion_Mania] = "Character_Name_Fusion_Mania",
    [Character_Name_Slime] = "Character_Name_Slime",
    [Character_Name_Metal_Star] = "Character_Name_Metal_Star",
    [Character_Name_Persona] = "Character_Name_Persona",
    [Character_Name_Mask_of_Fire] = "Character_Name_Mask_of_Fire",
    [Character_Name_Mask_of_Ice] = "Character_Name_Mask_of_Ice",
    [Character_Name_Mask_of_Blood] = "Character_Name_Mask_of_Blood",
    [Character_Name_Mask_of_Poison] = "Character_Name_Mask_of_Poison",
    [Character_Name_Mask_of_Heavenly] = "Character_Name_Mask_of_Heavenly",
    [Character_Name_Mask_of_Abyss] = "Character_Name_Mask_of_Abyss",
    [Character_Name_Dancer] = "Character_Name_Dancer",
    [Character_Name_Outer_Angel] = "Character_Name_Outer_Angel",
    [Character_Name_Outer_God] = "Character_Name_Outer_God",
    [Character_Name_MAX] = "Character_Name_MAX",
};


const char* Element_Type_enum_string[] = {
    [Element_Type_Madness] = "Element_Type_Madness",
    [Element_Type_Insanity] = "Element_Type_Insanity",
    [Element_Type_Physical] = "Element_Type_Physical",
    [Element_Type_Fire] = "Element_Type_Fire",
    [Element_Type_Ice] = "Element_Type_Ice",
    [Element_Type_Poison] = "Element_Type_Poison",
    [Element_Type_Blood] = "Element_Type_Blood",
    [Element_Type_Heavenly] = "Element_Type_Heavenly",
    [Element_Type_Abyss] = "Element_Type_Abyss",
    [Element_Type_MAX] = "Element_Type_MAX",
};



const char* Turned_Based_Camera_Type_enum_string[] = {
    [Turned_Based_Camera_Type_None] = "Turned_Based_Camera_Type_None",
    [Turned_Based_Camera_Type_TurnPlayerCamera] = "Turned_Based_Camera_Type_TurnPlayerCamera",
    [Turned_Based_Camera_Type_AllyTargetingCamera] = "Turned_Based_Camera_Type_AllyTargetingCamera",
    [Turned_Based_Camera_Type_EnemyTurnCamera] = "Turned_Based_Camera_Type_EnemyTurnCamera",
    [Turned_Based_Camera_Type_TargetEveryoneCamera] = "Turned_Based_Camera_Type_TargetEveryoneCamera",
    [Turned_Based_Camera_Type_AttackCamera] = "Turned_Based_Camera_Type_AttackCamera",
};

const char* Resistance_Type_enum_string[] = {
    [Resistance_Type_SuperWeak] = "Resistance_Type_SuperWeak",
    [Resistance_Type_Weak] = "Resistance_Type_Weak",
    [Resistance_Type_Neutral] = "Resistance_Type_Neutral",
    [Resistance_Type_Strong] = "Resistance_Type_Strong",
    [Resistance_Type_Resistant] = "Resistance_Type_Resistant",
    [Resistance_Type_Redirect] = "Resistance_Type_Redirect",
    [Resistance_Type_Spread] = "Resistance_Type_Spread",
};

const char* Status_Change_Types_enum_string[] = {
    [Status_Change_Types_StatusAmount] = "Status_Change_Types_StatusAmount",
    [Status_Change_Types_StatusPercent] = "Status_Change_Types_StatusPercent",
    [Status_Change_Types_StatusToSpecificPercent] = "Status_Change_Types_StatusToSpecificPercent",
    [Status_Change_Types_StatusToZero] = "Status_Change_Types_StatusToZero",
};

const char* Ability_Trigger_Target_enum_string[] = {
    [Ability_Trigger_Target_Caster] = "Ability_Trigger_Target_Caster",
    [Ability_Trigger_Target_Target] = "Ability_Trigger_Target_Target",
    [Ability_Trigger_Target_Allies] = "Ability_Trigger_Target_Allies",
    [Ability_Trigger_Target_Enemies] = "Ability_Trigger_Target_Enemies",
    [Ability_Trigger_Target_All] = "Ability_Trigger_Target_All",
};

const char* Character_Type_enum_string[] = {
    [Character_Type_Player] = "Character_Type_Player",
    [Character_Type_Enemy] = "Character_Type_Enemy",
};

const char* Ability_Name_enum_string[] = {
    [Ability_Name_INVALID] = "Ability_Name_INVALID",
    [Ability_Name_DEBUG_STARTING_1] = "Ability_Name_DEBUG_1",
    [Ability_Name_DEBUG_STARTING_2] = "Ability_Name_DEBUG_2",
    [Ability_Name_DEBUG_STARTING_3] = "Ability_Name_DEBUG_3",
    [Ability_Name_DEBUG_STARTING_4] = "Ability_Name_DEBUG_4",
    [Ability_Name_DEBUG_HEAL] = "Ability_Name_DEBUG_HEAL",
    [Ability_Name_DEBUG_DAMAGE] = "Ability_Name_DEBUG_DAMAGE",
    [Ability_Name_DEBUG_MADNESS] = "Ability_Name_Madness_Test",
    [Ability_Name_DEBUG_INSANITY] = "Ability_Name_INSANITY_Test",
    [Ability_Name_REVERSAL_TEST] = "Ability_Name_REVERSAL_TEST",
    [Ability_Name_DEBUG_DAMAGE_SELF_AND_ENEMY] = "Ability_Name_DEBUG_DAMAGE_SELF_AND_ENEMY",
    [Ability_Name_DEBUG_TURN_EFFECT] = "Ability_Name_DEBUG_TURN_EFFECT",
    [Ability_Name_DEBUG_POISON] = "Ability_Name_DEBUG_POISON",
    [Ability_Name_DEBUG_INSTAKILL_SELF] = "Ability_Name_DEBUG_INSTAKILL_SELF",
    [Ability_Name_DEBUG_INSTAKILL_TARGET] = "Ability_Name_DEBUG_INSTAKILL_TARGET",
    [Ability_Name_DEBUG_INSTAKILL_PLAYERS] = "Ability_Name_DEBUG_INSTAKILL_PLAYERS",
    [Ability_Name_DEBUG_INSTAKILL_ENEMYS] = "Ability_Name_DEBUG_INSTAKILL_ENEMYS",
    [Ability_Name_DEBUG_INSTAKILL_ALL] = "Ability_Name_DEBUG_INSTAKILL_ALL",
    [Ability_Name_DEBUG_PASS_ALL_PLAYER_UNITS_TURNS] = "Ability_Name_DEBUG_PASS_ALL_PLAYER_UNITS_TURNS",
    [Ability_Name_DEBUG_GIVE_MYSELF_ACTIONS] = "Ability_Name_DEBUG_GIVE_MYSELF_ACTIONS",
    [Ability_Name_Pass] = "Ability_Name_Pass",
    [Ability_Name_MAX] = "Ability_Name_MAX",
};

const char* Action_Changer_Type_enum_string[] = {
    [Action_Changer_Type_ActionAdd] = "Action_Changer_Type_ActionAdd",
    [Action_Changer_Type_ActionRemove] = "Action_Changer_Type_ActionRemove",
    [Action_Changer_Type_MAX] = "Action_Changer_Type_MAX",
};

const char* Level_Name_enum_string[] = {
    [Level_Name_None] = "Level_Name_None",
    [Level_Name_Sandbox] = "Level_Name_Sandbox",
    [Level_Name_Tutorial] = "Level_Name_Tutorial",
    [Level_Name_Worshipper] = "Level_Name_Worshipper",
    [Level_Name_BurningSoul] = "Level_Name_BurningSoul",
    [Level_Name_IceQueen] = "Level_Name_IceQueen",
    [Level_Name_SunMoonTwin] = "Level_Name_SunMoonTwin",
    [Level_Name_BrotherInArms] = "Level_Name_BrotherInArms",
    [Level_Name_BloodBrothers] = "Level_Name_BloodBrothers",
    [Level_Name_HealSquad] = "Level_Name_HealSquad",
    [Level_Name_Freezer] = "Level_Name_Freezer",
    [Level_Name_MetalStar] = "Level_Name_MetalStar",
    [Level_Name_Abyssal] = "Level_Name_Abyssal",
    [Level_Name_Slimes] = "Level_Name_Slimes",
    [Level_Name_CountDown] = "Level_Name_CountDown",
    [Level_Name_ReversalReversal] = "Level_Name_ReversalReversal",
    [Level_Name_Angel] = "Level_Name_Angel",
    [Level_Name_Demon] = "Level_Name_Demon",
    [Level_Name_FusionMania] = "Level_Name_FusionMania",
    [Level_Name_Persona] = "Level_Name_Persona",
    [Level_Name_DisfiguredMass] = "Level_Name_DisfiguredMass",
    [Level_Name_InsanityProgenitor] = "Level_Name_InsanityProgenitor",
    [Level_Name_Dancer] = "Level_Name_Dancer",
    [Level_Name_Player] = "Level_Name_Player",
    [Level_Name_Law] = "Level_Name_Law",
    [Level_Name_Shield] = "Level_Name_Shield",
    [Level_Name_Time1] = "Level_Name_Time1",
    [Level_Name_Time2] = "Level_Name_Time2",
    [Level_Name_Time3] = "Level_Name_Time3",
    [Level_Name_Time4] = "Level_Name_Time4",
    [Level_Name_Time5] = "Level_Name_Time5",
    [Level_Name_Tribe] = "Level_Name_Tribe",
    [Level_Name_Conjure] = "Level_Name_Conjure",
    [Level_Name_3SisterFather] = "Level_Name_3SisterFather",
    [Level_Name_MultiElemental] = "Level_Name_MultiElemental",
    [Level_Name_MPDrainer] = "Level_Name_MPDrainer",
    [Level_Name_BlackBloodStar] = "Level_Name_BlackBloodStar",
    [Level_Name_Planets] = "Level_Name_Planets",
    [Level_Name_OuterGodsAngels] = "Level_Name_OuterGodsAngels",
    [Level_Name_OuterGod] = "Level_Name_OuterGod",
    [Level_Name_MAX] = "Level_Name_MAX",
};

const char* Ability_Target_Type_enum_string[] = {
    [Ability_Target_Type_Self] = "Ability_Target_Type_Self",
    [Ability_Target_Type_Allies] = "Ability_Target_Type_Allies",
    [Ability_Target_Type_Enemies] = "Ability_Target_Type_Enemies",
    [Ability_Target_Type_Self_And_Allies] = "Ability_Target_Type_SelfAndAllies",
    [Ability_Target_Type_Self_And_Enemies] = "Ability_Target_Type_SelfAndEnemies",
    [Ability_Target_Type_Allies_And_Enemies] = "Ability_Target_Type_AlliesAndEnemies",
    [Ability_Target_Type_All] = "Ability_Target_Type_All",
};

const char* Reversal_Duration_enum_string[] = {
    [Reversal_Duration_Once] = "Reversal_Duration_Once",
    [Reversal_Duration_Casters_Turn] = "Reversal_Duration_Casters_Turn",
    [Reversal_Duration_Permanent] = "Reversal_Duration_Permanent",
};

const char* Status_Threshold_Types_enum_string[] = {
    [Status_Threshold_Types_Threshold_Amount] = "Status_Threshold_Types_Threshold_Amount",
    [Status_Threshold_Types_Threshold_Specific_Value] = "Status_Threshold_Types_Threshold_Specific_Value",
    [Status_Threshold_Types_Threshold_By_Multiplication] = "Status_Threshold_Types_Threshold_By_Multiplication",
    [Status_Threshold_Types_MAX] = "Status_Threshold_Types_MAX",
};

const char* Target_Area_Affect_enum_string[] = {
    [Target_Area_Affect_Single_Target] = "Target_Area_Affect_Single_Target",
    [Target_Area_Affect_Target_All] = "Target_Area_Affect_Target_All",
};

const char* Ability_Action_Cost_Type_enum_string[] = {
    [Ability_Action_Cost_Type_1] = "Ability_Action_Cost_Type_1",
    [Ability_Action_Cost_Type_2] = "Ability_Action_Cost_Type_2",
    [Ability_Action_Cost_Type_3] = "Ability_Action_Cost_Type_3",
    [Ability_Action_Cost_Type_Max] = "Ability_Action_Cost_Type_Max",
};

const char* Ability_Overflow_Value_Type_enum_string[] = {
    [Ability_Overflow_Value_Type_Base] = "Ability_Overflow_Value_Type_Base",
    [Ability_Overflow_Value_Type_Created] = "Ability_Overflow_Value_Type_Created",
    [Ability_Overflow_Value_Type_Other1] = "Ability_Overflow_Value_Type_Other1",
    [Ability_Overflow_Value_Type_Other2] = "Ability_Overflow_Value_Type_Other2",
    [Ability_Overflow_Value_Type_Max] = "Ability_Overflow_Value_Type_Max",
};

const char* Turn_Initiative_enum_string[] = {
    [Turn_Initiative_Player] = "Turn_Initiative_Player",
    [Turn_Initiative_Enemy] = "Turn_Initiative_Enemy",
};

const char* Ability_Activation_Type_enum_string[] = {
    [Ability_Activation_Type_Normal] = "Ability_Activation_Type_Normal",
    [Ability_Activation_Type_Turn] = "Ability_Activation_Type_Turn",
    [Ability_Activation_Type_Reversal] = "Ability_Activation_Type_Reversal",
};

const char* Ability_Component_Type_enum_string[] = {
    [Ability_Component_TYPE_INVALID] = "Ability_Component_TYPE_INVALID",
    [Ability_Component_TYPE_HEAL] = "Ability_Component_TYPE_HEAL",
    [Ability_Component_Type_Damage] = "Ability_Component_Type_Damage",
    [Ability_Component_TYPE_AUGMENT_CHANGE] = "Ability_Component_TYPE_AUGMENT_CHANGE",
    [Ability_Component_TYPE_CHARGE] = "Ability_Component_TYPE_CHARGE",
    [Ability_Component_TYPE_CONJURE] = "Ability_Component_TYPE_CONJURE",
    [Ability_Component_TYPE_DRAIN] = "Ability_Component_TYPE_DRAIN",

    [Ability_Component_TYPE_RESISTANCE_CHANGE] = "Ability_Component_TYPE_RESISTANCE_CHANGE",
    [Ability_Component_TYPE_STATUS_CHANGE] = "Ability_Component_TYPE_STATUS_CHANGE",
    [Ability_Component_TYPE_STATUS_THRESHOLD_CHANGE] = "Ability_Component_TYPE_STATUS_THRESHOLD_CHANGE",
    [Ability_Component_TYPE_MAX] = "Ability_Component_TYPE_MAX",
};

const char* Character_State_enum_string[] = {
    [Character_State_Alive] = "Character_State_Alive",
    [Character_State_Dead] = "Character_State_Dead",
    [Character_State_Revive] = "Character_State_Revive",
};

const char* Summoner_Type_enum_string[] = {
    [Summoner_Type_Doll] = "Summoner_Type_Doll",
    [Summoner_Type_RedJester] = "Summoner_Type_RedJester",
    [Summoner_Type_Clown] = "Summoner_Type_Clown",
    [Summoner_Type_Puppet] = "Summoner_Type_Puppet",
    [Summoner_Type_Max] = "Summoner_Type_Max",
};

const char* Turn_Activation_Type_enum_string[] = {
    [Turn_Activation_Type_None] = "Turn_Activation_Type_None",
    [Turn_Activation_Type_Start] = "Turn_Activation_Type_Start",
    [Turn_Activation_Type_End] = "Turn_Activation_Type_End",
    [Turn_Activation_Type_Start_End] = "Turn_Activation_Type_Start_End",
    [Turn_Activation_Type_First_Start] = "Turn_Activation_Type_First_Start",
    [Turn_Activation_Type_Final_End] = "Turn_Activation_Type_Final_End",
};


const char* Conjure_Type_enum_string[] = {
    [Conjure_Type_IceDemon] = "Conjure_Type_IceDemon",
    [Conjure_Type_MiracleDemon] = "Conjure_Type_MiracleDemon",
    [Conjure_Type_DamageDemon] = "Conjure_Type_DamageDemon",
    [Conjure_Type_Max] = "Conjure_Type_Max",
};


const char* Passive_Transfer_Type_enum_string[] = {
    [Passive_Transfer_Type_Caster_To_Target] = "Passive_Transfer_Type_Caster_To_Target",
    [Passive_Transfer_Type_Target_To_Caster] = "Passive_Transfer_Type_Target_To_Caster",
};

const char* Madness_Pulse_Game_State_enum_string[] = {
    [Game_State_Enum_Main_Menu] = "Game_State_Enum_Main_Menu",
    [Game_State_Enum_Turn_Based] = "Game_State_Enum_Turn_Based",
    [Game_State_Load_Save] = "Game_State_Load_Save",
    [Game_State_New_Save] = "Game_State_New_Save",
    [Game_State_Enum_Ability_Select] = "Game_State_Enum_Ability_Select",
    [Game_State_Enum_Level_Select] = "Game_State_Enum_Level_Select",
    [Game_State_Settings] = "Game_State_Settings",
    [Game_State_Enum_MAX] = "Game_State_Enum_MAX",
};

const char* High_Low_Type_enum_string[] = {
    [High_Low_Type_Low] = "High_Low_Type_Low",
    [High_Low_Type_High] = "High_Low_Type_High",
};


const char* Ability_Primary_Type_enum_string[] = {
    [ABILITY_PRIMARY_TYPE_MADNESS] = "ABILITY_PRIMARY_TYPE_MADNESS",
    [ABILITY_PRIMARY_TYPE_INSANITY] = "ABILITY_PRIMARY_TYPE_INSANITY",
};

const char* Targeting_Direction_enum_string[] = {
    [Targeting_Direction_Left] = "Targeting_Direction_Left",
    [Targeting_Direction_Right] = "Targeting_Direction_Right",
};

const char* Damage_Status_Type_enum_string[] = {
    [Damage_Status_Type_Dreamy] = "Damage_Status_Type_Dreamy",
    [Damage_Status_Type_Low] = "Damage_Status_Type_Low",
    [Damage_Status_Type_Delusion] = "Damage_Status_Type_Delusion",
    [Damage_Status_Type_High] = "Damage_Status_Type_High",
    [Damage_Status_Type_Visionary] = "Damage_Status_Type_Visionary",
    [Damage_Status_Type_Imaginary] = "Damage_Status_Type_Imaginary",
    [Damage_Status_Type_Physical] = "Damage_Status_Type_Physical",
    [Damage_Status_Type_MAX] = "Damage_Status_Type_MAX",
};

void generate_runtime_enums(Reflection_Registry* reflection_registry)
{
    const Reflection_Runtime_Enum Ability_Changer_Type_enum =
    {
        .name = "Ability_Changer_Type",
        .enum_names = Ability_Changer_Type_enum_string,
        .count = ARRAY_SIZE(Ability_Changer_Type_enum_string),
    };
    reflection_registry_add_enums(reflection_registry, Ability_Changer_Type_enum);

    const Reflection_Runtime_Enum Charge_State_enum =
    {
        .name = "Charge_State",
        .enum_names = Charge_State_enum_string,
        .count = ARRAY_SIZE(Charge_State_enum_string),
    };
    reflection_registry_add_enums(reflection_registry, Charge_State_enum);

    const Reflection_Runtime_Enum Game_UI_States_enum =
    {
        .name = "Game_UI_States",
        .enum_names = Game_UI_States_enum_string,
        .count = ARRAY_SIZE(Game_UI_States_enum_string),
    };
    reflection_registry_add_enums(reflection_registry, Game_UI_States_enum);

    const Reflection_Runtime_Enum Ability_Secondary_Type_enum =
    {
        .name = "Ability_Secondary_Type",
        .enum_names = Ability_Secondary_Type_enum_string,
        .count = ARRAY_SIZE(Ability_Secondary_Type_enum_string),
    };
    reflection_registry_add_enums(reflection_registry, Ability_Secondary_Type_enum);

    const Reflection_Runtime_Enum Turn_Phase_enum =
    {
        .name = "Turn_Phase",
        .enum_names = Turn_Phase_enum_string,
        .count = ARRAY_SIZE(Turn_Phase_enum_string),
    };
    reflection_registry_add_enums(reflection_registry, Turn_Phase_enum);

    const Reflection_Runtime_Enum Character_Name_enum =
    {
        .name = "Character_Name",
        .enum_names = Character_Name_enum_string,
        .count = ARRAY_SIZE(Character_Name_enum_string),
    };
    reflection_registry_add_enums(reflection_registry, Character_Name_enum);


    const Reflection_Runtime_Enum Element_Type_enum =
    {
        .name = "Element_Type",
        .enum_names = Element_Type_enum_string,
        .count = ARRAY_SIZE(Element_Type_enum_string),
    };
    reflection_registry_add_enums(reflection_registry, Element_Type_enum);



    const Reflection_Runtime_Enum Turned_Based_Camera_Type_enum =
    {
        .name = "Turned_Based_Camera_Type",
        .enum_names = Turned_Based_Camera_Type_enum_string,
        .count = ARRAY_SIZE(Turned_Based_Camera_Type_enum_string),
    };
    reflection_registry_add_enums(reflection_registry, Turned_Based_Camera_Type_enum);

    const Reflection_Runtime_Enum Resistance_Type_enum =
    {
        .name = "Resistance_Type",
        .enum_names = Resistance_Type_enum_string,
        .count = ARRAY_SIZE(Resistance_Type_enum_string),
    };
    reflection_registry_add_enums(reflection_registry, Resistance_Type_enum);

    const Reflection_Runtime_Enum Status_Change_Types_enum =
    {
        .name = "Status_Change_Types",
        .enum_names = Status_Change_Types_enum_string,
        .count = ARRAY_SIZE(Status_Change_Types_enum_string),
    };
    reflection_registry_add_enums(reflection_registry, Status_Change_Types_enum);

    const Reflection_Runtime_Enum Ability_Trigger_Target_enum =
    {
        .name = "Ability_Trigger_Target",
        .enum_names = Ability_Trigger_Target_enum_string,
        .count = ARRAY_SIZE(Ability_Trigger_Target_enum_string),
    };
    reflection_registry_add_enums(reflection_registry, Ability_Trigger_Target_enum);

    const Reflection_Runtime_Enum Character_Type_enum =
    {
        .name = "Character_Type",
        .enum_names = Character_Type_enum_string,
        .count = ARRAY_SIZE(Character_Type_enum_string),
    };
    reflection_registry_add_enums(reflection_registry, Character_Type_enum);

    const Reflection_Runtime_Enum Ability_Name_enum =
    {
        .name = "Ability_Name",
        .enum_names = Ability_Name_enum_string,
        .count = ARRAY_SIZE(Ability_Name_enum_string),
    };
    reflection_registry_add_enums(reflection_registry, Ability_Name_enum);

    const Reflection_Runtime_Enum Action_Changer_Type_enum =
    {
        .name = "Action_Changer_Type",
        .enum_names = Action_Changer_Type_enum_string,
        .count = ARRAY_SIZE(Action_Changer_Type_enum_string),
    };
    reflection_registry_add_enums(reflection_registry, Action_Changer_Type_enum);

    const Reflection_Runtime_Enum Level_Name_enum =
    {
        .name = "Level_Name",
        .enum_names = Level_Name_enum_string,
        .count = ARRAY_SIZE(Level_Name_enum_string),
    };
    reflection_registry_add_enums(reflection_registry, Level_Name_enum);

    const Reflection_Runtime_Enum Ability_Target_Type_enum =
    {
        .name = "Ability_Target_Type",
        .enum_names = Ability_Target_Type_enum_string,
        .count = ARRAY_SIZE(Ability_Target_Type_enum_string),
    };
    reflection_registry_add_enums(reflection_registry, Ability_Target_Type_enum);

    const Reflection_Runtime_Enum Reversal_Duration_enum =
    {
        .name = "Reversal_Duration",
        .enum_names = Reversal_Duration_enum_string,
        .count = ARRAY_SIZE(Reversal_Duration_enum_string),
    };
    reflection_registry_add_enums(reflection_registry, Reversal_Duration_enum);

    const Reflection_Runtime_Enum Status_Threshold_Types_enum =
    {
        .name = "Status_Threshold_Types",
        .enum_names = Status_Threshold_Types_enum_string,
        .count = ARRAY_SIZE(Status_Threshold_Types_enum_string),
    };
    reflection_registry_add_enums(reflection_registry, Status_Threshold_Types_enum);

    const Reflection_Runtime_Enum Target_Area_Affect_enum =
    {
        .name = "Target_Area_Affect",
        .enum_names = Target_Area_Affect_enum_string,
        .count = ARRAY_SIZE(Target_Area_Affect_enum_string),
    };
    reflection_registry_add_enums(reflection_registry, Target_Area_Affect_enum);

    const Reflection_Runtime_Enum Ability_Action_Cost_Type_enum =
    {
        .name = "Ability_Action_Cost_Type",
        .enum_names = Ability_Action_Cost_Type_enum_string,
        .count = ARRAY_SIZE(Ability_Action_Cost_Type_enum_string),
    };
    reflection_registry_add_enums(reflection_registry, Ability_Action_Cost_Type_enum);

    const Reflection_Runtime_Enum Ability_Overflow_Value_Type_enum =
    {
        .name = "Ability_Overflow_Value_Type",
        .enum_names = Ability_Overflow_Value_Type_enum_string,
        .count = ARRAY_SIZE(Ability_Overflow_Value_Type_enum_string),
    };
    reflection_registry_add_enums(reflection_registry, Ability_Overflow_Value_Type_enum);

    const Reflection_Runtime_Enum Turn_Initiative_enum =
    {
        .name = "Turn_Initiative",
        .enum_names = Turn_Initiative_enum_string,
        .count = ARRAY_SIZE(Turn_Initiative_enum_string),
    };
    reflection_registry_add_enums(reflection_registry, Turn_Initiative_enum);

    const Reflection_Runtime_Enum Ability_Activation_Type_enum =
    {
        .name = "Ability_Activation_Type",
        .enum_names = Ability_Activation_Type_enum_string,
        .count = ARRAY_SIZE(Ability_Activation_Type_enum_string),
    };
    reflection_registry_add_enums(reflection_registry, Ability_Activation_Type_enum);

    const Reflection_Runtime_Enum Ability_Component_Type_enum =
    {
        .name = "Ability_Component_Type",
        .enum_names = Ability_Component_Type_enum_string,
        .count = ARRAY_SIZE(Ability_Component_Type_enum_string),
    };
    reflection_registry_add_enums(reflection_registry, Ability_Component_Type_enum);

    const Reflection_Runtime_Enum Character_State_enum =
    {
        .name = "Character_State",
        .enum_names = Character_State_enum_string,
        .count = ARRAY_SIZE(Character_State_enum_string),
    };
    reflection_registry_add_enums(reflection_registry, Character_State_enum);

    const Reflection_Runtime_Enum Summoner_Type_enum =
    {
        .name = "Summoner_Type",
        .enum_names = Summoner_Type_enum_string,
        .count = ARRAY_SIZE(Summoner_Type_enum_string),
    };
    reflection_registry_add_enums(reflection_registry, Summoner_Type_enum);

    const Reflection_Runtime_Enum Turn_Activation_Type_enum =
    {
        .name = "Turn_Activation_Type",
        .enum_names = Turn_Activation_Type_enum_string,
        .count = ARRAY_SIZE(Turn_Activation_Type_enum_string),
    };
    reflection_registry_add_enums(reflection_registry, Turn_Activation_Type_enum);


    const Reflection_Runtime_Enum Conjure_Type_enum =
    {
        .name = "Conjure_Type",
        .enum_names = Conjure_Type_enum_string,
        .count = ARRAY_SIZE(Conjure_Type_enum_string),
    };
    reflection_registry_add_enums(reflection_registry, Conjure_Type_enum);


    const Reflection_Runtime_Enum Passive_Transfer_Type_enum =
    {
        .name = "Passive_Transfer_Type",
        .enum_names = Passive_Transfer_Type_enum_string,
        .count = ARRAY_SIZE(Passive_Transfer_Type_enum_string),
    };
    reflection_registry_add_enums(reflection_registry, Passive_Transfer_Type_enum);

    const Reflection_Runtime_Enum Madness_Pulse_Game_State_enum =
    {
        .name = "Madness_Pulse_Game_State",
        .enum_names = Madness_Pulse_Game_State_enum_string,
        .count = ARRAY_SIZE(Madness_Pulse_Game_State_enum_string),
    };
    reflection_registry_add_enums(reflection_registry, Madness_Pulse_Game_State_enum);

    const Reflection_Runtime_Enum High_Low_Type_enum =
    {
        .name = "High_Low_Type",
        .enum_names = High_Low_Type_enum_string,
        .count = ARRAY_SIZE(High_Low_Type_enum_string),
    };
    reflection_registry_add_enums(reflection_registry, High_Low_Type_enum);


    const Reflection_Runtime_Enum Ability_Primary_Type_enum =
    {
        .name = "Ability_Primary_Type",
        .enum_names = Ability_Primary_Type_enum_string,
        .count = ARRAY_SIZE(Ability_Primary_Type_enum_string),
    };
    reflection_registry_add_enums(reflection_registry, Ability_Primary_Type_enum);

    const Reflection_Runtime_Enum Targeting_Direction_enum =
    {
        .name = "Targeting_Direction",
        .enum_names = Targeting_Direction_enum_string,
        .count = ARRAY_SIZE(Targeting_Direction_enum_string),
    };
    reflection_registry_add_enums(reflection_registry, Targeting_Direction_enum);

    const Reflection_Runtime_Enum Damage_Status_Type_enum =
    {
        .name = "Damage_Status_Type",
        .enum_names = Damage_Status_Type_enum_string,
        .count = ARRAY_SIZE(Damage_Status_Type_enum_string),
    };
    reflection_registry_add_enums(reflection_registry, Damage_Status_Type_enum);
}
