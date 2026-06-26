#ifndef GAME_TYPES_H
#define GAME_TYPES_H

#include <stdbool.h>

#include "game_constants.h"
#include "game_enums.h"


typedef struct Nonsense_Struct
{
    bool hi;
} Nonsense_Struct;


/// ABILITIES ///
typedef struct Reflection_Test_Struct
{
    //type -> name
    //type -> name->[array]
    //type* -> name
    //name(struct/enum) -> name
    //name(struct/enum) -> name[array]
    //name(struct/enum)* -> name

    float type;
    float* type_ptr;
    float type_stack_ptr[10];
    // float type_stack_ptr[Constant]; // there is the case of constants, that I should load in as well

    Character_Name enum_type;
    Character_Name* enum_type_ptr;
    Character_Name enum_type_stack_ptr[10];


    Nonsense_Struct struct_type;
    Nonsense_Struct* struct_type_ptr;
    Nonsense_Struct struct_type_stack_ptr[10];
} Reflection_Test_Struct;


//SAVE GAME

/* Save data related things*/

typedef struct Game_Settings
{
    //game specific settings
    bool unimplemented;
} Game_Settings;

typedef struct Save_Meta_Data
{
    u8 magic_number[4];
    float version;
    //used for the load game screen, so the player can see which save file they want
    u8 slot_number; //255 game save is more than enough
    u8 missions_beaten;
    float game_time;
} Save_Meta_Data;


typedef struct Player_Save_Data
{
    Character_Name unit_id;

    //TODO; replace with dynamic array at some point
    u32 battle_list_count;
    u32 reserve_list_count;

    Ability_Name* ability_battle_list_save;
    Ability_Name* ability_reserve_list;

    //TODO: load outs for bieng able to quickly select a build
} Player_Save_Data;

typedef struct Save_Game
{
    Save_Meta_Data meta_data;

    Player_Save_Data player_save_info[MAX_PLAYER_UNIT_COUNT];

    //TODO: set the values needed to false
    // stores whether a level is unlocked or not,
    // they need to be the same name as the level
    bool unlocked_levels[Level_Name_MAX];

    //TODO: make fusion available during the fusion fight, and save the setting permanently from there
    bool allowed_to_fusion;
} Save_Game;


typedef struct Level_Unlock_Rewards
{
    u32 unlock_count;
    Ability_Name* ability_unlock_array_ids;
} Level_Unlock_Rewards;

// This is information set by a data table,
// If a mission is unlocked, we check it by getting the map on the save data TMap<Emissionname, bool>
//TODO: figure out how you want to display and read prebattle dialogue
typedef struct Game_Level_Data
{
    Level_Name mission_name;
    Level_Name mission_to_unlock;

    String Display_Name;

    //enemies to load in for the fight
    u32 enemy_count;
    Character_Name* enemy_units;


    //decide who gets to go first
    Turn_Initiative starting_turn_initiative;

    bool is_debug_level;
    bool ready_for_playtest;
} Game_Level_Data;


// ABILITY COMPONENTS

typedef struct Heal_Component
{
    // Ability_Component_Type type;
    Heal_Types heal_type;
    float heal_amount;
    //should be false by default
    bool heal_only_if_dead;
} Heal_Component;

typedef struct Damage_Component
{
    // Ability_Component_Type type;
    Element_Type element;
    float damage; // TODO: remove, its here just for testing
} Damage_Component;


typedef struct Ability_Adder_Component
{
    Ability_Name abilities_to_add;
    u32 count;
} Ability_Adder_Component;

typedef struct Ability_Remover_Component
{
    Ability_Name abilities_to_remove;
    u32 count;
} Ability_Remover_Component;

typedef struct Remove_All_Ability_Component
{
    Ability_Name ability_to_remove;
    // bool RemoveAllAbilities;
} Remove_All_Ability_Component;

typedef struct Action_Remove_Component
{
    u32 action_amount_to_remove;
} Action_Remove_Component;

typedef struct Action_Add_Component
{
    u32 action_amount_to_add;
} Action_Add_Component;

typedef struct Augment_Changer_Component
{
    // TMap<EDamageType, int> augment_type_to_change_value;
    Element_Type damage_type;
    int augment_change_value;
} Augment_Changer_Component;

typedef struct Charge_Attack_Component
{
    // TMap<EDamageType, EChargeState> ElementsToCharge;
    Element_Type damage_type;
    Charge_State charge_state;
} Charge_Attack_Component;

typedef struct Conjure_Component
{
    Conjure_Type conjure_type;
} Conjure_Component;

typedef struct Summoner_Component
{
    Summoner_Type summoner_type;
} Summoner_Component;


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
    bool heal_only_if_dead; // = false;
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

typedef struct Resistance_Changer_Component
{
    // TMap<EDamageType, EResistanceType> ResistanceToChangeMap;
    Resistance_Type resistance_to_change_map[Element_Type_MAX];
} Resistance_Changer_Component;

typedef struct Resistance_Changer_Random_Component
{
    int random_amount; // = 1;
    //basically saying the resistance we are changing to is not random
    Resistance_Type resistance_to_change_to;
} Resistance_Changer_Random_Component;

typedef struct Resistance_Resetter_Component
{
    //NOTE: this straight up had nothing in it
    bool unimplemented;
} Resistance_Resetter_Component;


typedef struct Reversal_Component
{
    //TODO: this might actually make sense as bitflags
    // and there is no reason to have reversals on things like fucking heal types, just on a heal in general
    Reversal_Duration reversal_duration;

    bool AnythingNotDamage; // = false;
    bool AnyDamage; // = false;

    bool Heal;
    bool Drain;
    bool Mp;
    bool DamageConditions[Element_Type_MAX];
    bool AugmentCondition[Element_Type_MAX];
    bool ActionChangeCondition[Action_Changer_Type_MAX];
    bool AbilityChangeCondition[Ability_Changer_Type_MAX];
    bool NegationCondition; //= false;
    bool DamagePassiveCondition; // = false;
} Reversal_Component;


typedef struct Status_Changer_Component
{
    // EStatusChangeTypes StatusChangeType = EStatusChangeTypes::ECS_StatusPercent;
    // TMap<EDamageType, float> StatusTypeToChangeAmount;
    Status_Change_Types status_change_types;
    Element_Type damage_type;
    float status_change_amount;
} Status_Changer_Component;

typedef struct Special_Ability_Flag_Setter_Component
{
    bool unimplemented;
    // Special_Ability_Flag_List_Component flags_to_set; //TODO:
    //FSpecialAbilityFlagList FlagsToDisable;//TODO: if i did want this, it would be its own special type of component
} Special_Ability_Flag_Setter_Component;

typedef struct Status_Threshold_Changer
{
    Status_Threshold_Types status_threshold_type;
    // TMap<EDamageType, float> StatusThresholdChangeMap;
    Element_Type damage_type;
    float amount;
} Status_Threshold_Changer;


typedef struct Turn_Component_Base
{
    Turn_Activation_Type turn_activation;
    u8 effect_length;
    u8 turns_until_triggered;
} Turn_Component_Base;

//Battle Info
typedef struct Damage_Bounce_Component
{
    bool impact_mirror; // = false;
    float damage_multiplier; // = 1.0f;
} Damage_Bounce_Component;

typedef struct Drain_Bounce_Component
{
    //TODO: split these into different components
    //only select one
    bool drain_twice; // = false;
    bool heal_after_drain; // = false;
} Drain_Bounce_Component;

typedef struct Heal_Bounce_Component
{
    /* will damage the unit by the health gain multiplied by a multiplier */
    float heal_bounce_multiplier; // = 2.0f;
} Heal_Bounce_Component;

typedef struct Rewind_Stats_Component
{
    bool rewind_health; // = false;
    bool rewind_status; // = false;
    bool rewind_damage; // = false;
    bool rewind_negation; // = false;
} Rewind_Stats_Component;

typedef struct Heal_Based_On_Damage_Taken_Component
{
    /*meant to be used as a reversal, heals for any damage the caster just took*/
    float heal_percentage; // = 0.8;
} Heal_Based_On_Damage_Taken_Component;


//Boss Specific

typedef struct Ability_Swapper_Component
{
    int number_of_abilities_to_swap; // = 1;
} Ability_Swapper_Component;

typedef struct Dead_Unit_Damage_Component
{
    bool unimplemented;
    // FDeadAllyBonus DeadAllyBonus; //TODO:
    // EDamageType DamageType; ??idk what this is for and doesn't make much sense anyway
} Dead_Unit_Damage_Component;

typedef struct Emperor_Madness_Component
{
    /*Removes all abilities that were not originally in the players inventory ex: madness abilities or fusion abilities */
    //NOTE: FOUND IT LIKE THAT
    bool unimplemented;
} Emperor_Madness_Component;

typedef struct Emperors_Insanity_Component
{
    /*Removes all abilities from the target based on how many times that ability was used */
    int count_for_ability_to_be_removed; // = 3;
} Emperors_Insanity_Component;

typedef struct Emperor_Zero_Component
{
    /*Resets everything on the battleinfo, except maybe turn info */
    //NOTE: special ability flag
    bool unimplemented;
} Emperor_Zero_Component;

typedef struct Kill_By_Condition_Component
{
    //TODO: split these out into their own components
    /*checks a condition and then determines if it should kill the target */
    bool use_kill_by_health_above_percent; // = false;
    float health_percent_for_kill; // = 0.7f;
    bool use_kill_by_mana_above_percent; // = false;
    float mp_percent_for_kill; // = 0.7f;

    /*
    bool HasNotTakenDamage= false;
    bool HadNotChangedStatus= false;
    bool HadNotChangedNegation = false;
*/
} Kill_By_Condition_Component;


typedef struct MP_Action_Gainer_Component
{
    Action_Changer_Type action_type; // = EActionChangerType::ECS_ActionAdd;
    float mp_per_action_conversion; // = 1000.f;
} MP_Action_Gainer_Component;

typedef struct MP_Damage_Component
{
    /*
 * TODO:
 * Does a flat damage number based on the casters Mp with a damage multiplier like *10 on the targets
 * this does not use any mp, use mp changer to have mp go down
 */
    MP_Types mp_type;
    float mp_value;
    float damage_multiplier; // = 2.0f;
} MP_Damage_Component;

typedef struct MP_Heal_Component
{
    /*
  * Note: does not use up mp. if you want to use the mp changer
  * uses the casters mp as the heal value on the target(which will most likely be yourself)
  */
    float heal_multiplier; // = 1.0;
} MP_Heal_Component;

typedef struct MP_Stealer_Component
{
    /*
     * Takes Mp from the target and add it to the caster
    */

    //do not choose to full
    MP_Types mp_steal_type; // = EMPTypes::ECS_MPPercent;
    //make a positive value
    float mp_steal_amount;
} MP_Stealer_Component;

typedef struct Primavera_Bloom_Component
{
    /*Deal damage for each resistance on the caster that is not changed from superweak to redirect (or something similar) */
    Resistance_Type resistance_type_to_check; // = EResistanceType::ECS_SuperWeak;
    float damage_amount; // = 1000.0f;
} Primavera_Bloom_Component;

typedef struct Primavera_Faith_Component
{
    /**
  *checks the opponents inventory for any copies of primavera worship, removes all of them, and for each one removed does damage
  */
    //TODO: these can both just be changed to ability name
    String ability_name; // = "Primavera Faith";
    // primavera faith here
    // TSubclassOf<AAbilityBase> AbilityClass;
    float damage_amount; // = 1000.0f;
} Primavera_Faith_Component;


typedef struct Primavera_Flower_Component
{
    //Gain health based on the damage and negation, meant to have the damage and negation set to zero when it's done, use another ability
    float heal_multiplier; // = 100.0f;
} Primavera_Flower_Component;

typedef struct Primavera_Light_Component
{
    //every time primavera is hit, this will check what element she was hit by and change the corresponding resistances to redirect
    bool unimplemented;
} Primavera_Light_Component;


//Conditional
typedef struct Conditional_Fat_Component
{
    bool test_condition; // = false;

    bool caster_mp_at_zero; // = false;

    bool caster_has_positive_damage; // = false;
} Conditional_Fat_Component;


//Custom
typedef struct Damage_Based_On_Passive_Component
{
    bool use_status;
    // TSet<Element_Type> DamageTypeToCheckFor; //TODO:
    bool use_damage;
    bool use_negation;


    float damage_amount_per_point; // = 100;
    int apply_damage_every_point_amount; // = 1;
} Damage_Based_On_Passive_Component;

typedef struct Drain_Battle_Heal_Damage_Component
{
    /*
     *Values are based on all allies and then we use that to heal or damage the target
     */

    // TSet<EBattleInfoLifeTimeType> LifeTimeType; //TODO:

    bool must_be_dead; // = true;

    //only one of these should be active
    bool should_heal; // = false;
    bool should_damage; // = false;
} Drain_Battle_Heal_Damage_Component;


typedef struct Drain_Battle_Info_Component
{
    //TODO: All this can be moved out to a its own typedef struct for reuse, since this is going to keep coming up
    // the only thing is having two bool specifically set for use on what values we want to use
    // TSet<EBattleInfoLifeTimeType> LifeTimeType; //TODO:

    //translate to +1 to someonething for health Drained
    float division_value; // = 0;
    // TSet<EDamageType> StatusToGain; //TODO:
    bool gain_defense;
    bool gain_damage;
    bool deal_damage_of_drain_lost;
} Drain_Battle_Info_Component;

typedef struct Fusion_Mania_Component
{
    //TODO: im assuming this sets a special ability flag
    bool unimplemented;
    float kill_damage_amount; // = 999999.0f;
} Fusion_Mania_Component;


typedef struct Heal_Asphyxiation_Component
{
    /*
     * Targets a dead party member and take all their health until they hit the max negative value
     */
    bool unimplemented;
} Heal_Asphyxiation_Component;

typedef struct Heal_By_Target_Health_Component
{
    bool unimplemented;
} Heal_By_Target_Health_Component;

typedef struct Health_Pooling_Transfer
{
    /**
 * take health from the targets and give them to the caster until their health is full
 */
    bool unimplemented;
} Health_Pooling_Transfer;

typedef struct Health_Sacrifice
{
    /* Will sacrifice casters health until the is at 100% HP */
    bool unimplemented;
} Health_Sacrifice;

typedef struct High_Low_Type_Augment_Component
{
    // TMap<EDamageType, int> augment_type_to_change_value;
    int augment_type_to_change_value[Element_Type_MAX];
    High_Low_Type high_low;
} High_Low_Type_Augment_Component;


typedef struct Passive_Anti_Pooling_Component
{
    /**
     * we take the casters passives, and then reduce the opponents passive by that amount
     */
    // TSet<EDamageType> elements_to_check_for;
    bool elements_to_check_for[Element_Type_MAX];

    bool pool_damage; // = false;
    bool pool_status; // = false;
    bool pool_negation; // = false;
} Passive_Anti_Pooling_Component;


typedef struct Passive_Pooling_Component
{
    /*
 * Takes the combined points of a passive from their allies and adds to onto the caster
 */
    // TSet<EDamageType> elements_to_check_for;
    bool elements_to_check_for[Element_Type_MAX];

    bool pool_damage; // = false;
    bool pool_status; // = false;
    bool pool_negation; // = false;
} Passive_Pooling_Component;


typedef struct Passive_Transfer_Component
{
    //TODO: it would be funny to transfer any negative augments onto another character,
    // like if you debuff the enemy then they will use this ability and debuff you instead
    // Passive_Transfer_Type TransferFlow;//TODO:

    int min_max_value_for_transfer; // = 0;
    // void PositiveTransfer(AUnitBase* FromTarget, AUnitBase* ToTarget);
    // void NegativeTransfer(AUnitBase* FromTarget, AUnitBase* ToTarget);
    // void AnyTransfer(AUnitBase* FromTarget, AUnitBase* ToTarget);

    //Basically just make sure that the passive type we check for a positive value
    // TSet<EDamageType> status_passive_to_transfer;
    bool status_passive_to_transfer[Element_Type_MAX];
    bool damage_passives_to_transfer; // = false;
    bool negation_passive_to_transfer; // = false;
} Passive_Transfer_Component;

typedef struct Random_Ability_Remover_Component
{
    int removal_amount; // = 1;
} Random_Ability_Remover_Component;


typedef struct Random_Augment_Component
{
    int augment_change_amount; // = 0;
} Random_Augment_Component;

typedef struct Random_Damage_Component /*: public UDamageComponent*/ // TODO:
{
    bool exclude_insanity; // = false;
    Damage_Status_Type intended_damage_amount; // = EDamageStatusType::ECS_High;
    Damage_Status_Type intended_damage_build_up_amount; // = EDamageStatusType::ECS_High;
} Random_Damage_Component;

typedef struct Sacrificial_Bomb_Component
{
    /**
 * will kill all allies excpet caster and inflict the health as damage to the target
 */
    Element_Type element_type; // = EDamageType::ECS_Fire;
    //set this to either 2.0f for double or 0.5f for half damage
    float damage_multiplier; // = 1.0f;
} Sacrificial_Bomb_Component;


typedef struct Status_Bar_Drain_Component
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
    float division_value_bonus = 1.0f;

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
    bool gain_status; // = false;
    bool gain_damage; // = false;
    bool gain_negation; // = false;
    bool gain_health; // = false;
    bool deal_damage; // = false;
} Status_Bar_Drain_Component;


typedef struct Status_Drain_Disfigured_Mass_Component
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
    float division_value_bonus; // = 1.0f;

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
    bool affect_status; // = false;
    bool affect_damage; // = false;
    bool affect_negation; // = false;
    bool deal_damage; // = false;
} Status_Drain_Disfigured_Mass_Component;

typedef struct Trade_Action_Changer_Component
{
    // will make the add/lose action change irrelevant, will remove all from caster and add them to one target
    bool lose_all; // = true;

    //if we dont lose all then we use this amount, ideally we clamp it to the max moves we have left,
    //but maybe going into turn debt is funny as well
    // TMap<EActionChangerType, int> action_type_to_action_change;
    int action_type_to_action_change[Action_Changer_Type_MAX];
} Trade_Action_Changer_Component;

typedef struct Augment_Redistribution_Component
{
    /*
 * TODO: wouldn't be the worse idea to make this a passive redistribution, if needed
 */

    //NOTE: from what i can tell this averages all the augment values and redistributed them evenly
    bool unimplemented;
} Augment_Redistribution_Component;

typedef struct Augment_Setter_Component
{
    // TMap<EDamageType, int> augment_type_to_setter_value;
    int augment_type_to_setter_value[Element_Type_MAX];
    bool special_condition; // = false;
    int must_be_below_certain_value; // = 0;
} Augment_Setter_Component;

//Passive


typedef struct Augment_Swapper_Component
{
    /*Swap the values of the targets status values at random*/
    int swap_attempts; // = 3;
} Augment_Swapper_Component;


typedef struct Bonus_Based_On_Status_Component
{
    /*we gain bonuses based on the status point value of the selected elements, bonus is specifically given to the caster*/

    // TSet<EDamageType> elements_to_gain_bonus_from;
    bool elements_to_gain_bonus_from[Element_Type_MAX];


    //this gets multiplied by the points we get from the status values
    float bonus_per_point; // = 1;

    bool gain_damage; // = false;
    bool gain_negation; // = false;

    // TSet<EDamageType> status_bar_element_to_increase;
    bool status_bar_element_to_increase[Element_Type_MAX];
    bool gain_status_bar; // = false; //TODO:, not done yet
} Bonus_Based_On_Status_Component;

typedef struct Damage_Negation_Component
{
    //values are intended to be percents
    int damage_negation_change_amount; // = 0;
} Damage_Negation_Component;

typedef struct Damage_Passive_Component
{
    //values are intended to be percents
    // these need to be small values like 0.05 = 5%
    int damage_passive_change_amount; // = 0;
} Damage_Passive_Component;

typedef struct High_Low_Augment_Component
{
    /**
 * Gets the highest augment in general and changes it by the amount we specify
 */

    High_Low_Type high_low_type;

    bool to_zero; // = false;
    int value_change; // = 0;
} High_Low_Augment_Component;

typedef struct High_Low_Damage_Component
{
    /*
     * Finds the target with the highest defense or attack and reduces it to zero or by a set amount
     *
     */

    bool to_zero; // = false;
    int value_change; //= 0;
} High_Low_Damage_Component;

typedef struct High_Low_Negation_Component
{
    bool to_zero; // = false;
    int value_change; // = 0;
} High_Low_Negation_Component;

typedef struct Negation_Based_On_Health_Component
{
    /*Will give targets defense boost based on casters health */


    int division_value; // = 1;
} Negation_Based_On_Health_Component;


typedef struct Negation_Passive_Setter_Component
{
    /*will set negation to the specific value*/

    //values are intended to be percents
    // these need to be small values like 0.05 = 5%
    int set_to_value; // = 0;
} Negation_Passive_Setter_Component;


typedef struct Passive_Reverse_Component
{
    /*Turn any negative passive (status, damage, negation) positive */
    bool reverse_status; // = false;
    bool reverse_damage; // = false;
    bool reverse_negation; //= false;
} Passive_Reverse_Component;


typedef struct Ability_Component
{
    //is it a normal, reversal etc
    Ability_Activation_Type activation_type;
    //single or multitarget
    Target_Area_Affect target_can_affect;
    //who is this targeting, enemies, allies, self etc
    Ability_Target_Type ability_target;

    Ability_Component_Type type;

    //ability switch lookup to find which function to execute
    union data
    {
        // NOTE: default to this if nothing is set, and 0 is an invalid type
        // NOTE: all ability components must have ability component type on them, as type if implicit based on the component
        Heal_Component heal;
        Damage_Component damage;
        Reversal_Component reversal;
        Turn_Component_Base turn_base;
    } data;
} Ability_Component;


#define Component_Size (BYTES_M(64))
_Static_assert(sizeof(Heal_Component) <= Component_Size);
_Static_assert(sizeof(Damage_Component) <= Component_Size);
_Static_assert(sizeof(Passive_Reverse_Component) <= Component_Size);

//damage enemy, and heal self ability
// ability
// -> damage (activation type/area affect/target type) -> damage info
// -> heal (activation type/area affect/target type)
// -> {type, index} -> heal info

typedef struct Ability_Component_List
{
    DYNAMIC_ARRAY_TYPE(Heal_Component)* heal_component[10];
    Damage_Component damage_component[10];
    Passive_Reverse_Component passive_reverse_component[10];
    //madness or insanity type

    //how do you even get this back from a function call????
    // Dynamic_Array* get_list(type); // i guess??
    //we can make each component a 32kb data blob and just take the data from a pool
    //if we have 1000 abilties loaded in, and we use 5 component for each 64*1000*5 - 312.5kb
} Ability_Component_List;

typedef struct Ability
{
    Ability_Name id;
    Ability_Component components[MAX_ABILITY_COMPONENTS];
    u32 component_count;
    //madness or insanity type
} Ability;


typedef struct Ability_Info
{
    Ability_Name ability_name; // = "Ability Not Named";
    String ability_text; // "Implement Text Please"; //TODO: i should look into auto generated ability text
    String overflow_trigger_text; // = "NA";

    String lore_text; //= "NA";


    //Primary Type is either madness or insanity
    //Secondary type is just to tell the player at a glance what an ability does
    Ability_Primary_Type ability_primary_type;
    Ability_Secondary_Type ability_secondary_type;
    // this would also be the fusion type, if i do add fusion into the game

    Ability_Target_Type ability_target_type;
    Target_Area_Affect ability_target_area;

    Ability_Action_Cost_Type ability_action_cost; // = 1; //TODO: honestly i think all abilites should be 1 cost
    u32 mp_cost; // = 1.0f;

    //how much an ability contributes to the overflow bar
    Ability_Overflow_Value_Type overflow_value;
} Ability_Info;





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
    float current_mp;
    float max_mp;
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

    s32 StatusTriggerOccurrence;

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
    //Used for the abiltity select screen

    //NOTE: these should not get touched but copied, as many things can
    // Ability* ability_battle_list; // list containing default abilities usable in the battle
    // Ability* ability_reserve; // list that contains all moves unlocked
    // Ability* Modifiable_AbilityBattleList; // list containing all abilities usable in the battle

    //NOTE : TEMP SORT OF
    // list containing all abilities the player intends to use in the battle
    Ability_Name battle_list_starting[INVENTORY_MAX_BATTLE_LIST];
    u8 battle_list_size;
    // Ability ability_battle_list_ability[INVENTORY_MAX_BATTLE_LIST];

    //list of ids to seperate tables
    Ability_Name ability_reserve[Ability_Name_MAX]; // list that contains all unused abilties
    u32 ability_reserve_size; // list that contains all unused abilties
    //TODO: it would honestly be easier for me to just grey out abilties that are in the battle inventory,
    // rather than removing them, and this lets me do validations much more easily
} Inventory_Component;


typedef struct Battle_Inventory_Component
{
    //In game, battle inventory, that is meant to manipulated


    DYNAMIC_ARRAY_TYPE(Ability_Name)* battle_list_dynamic; // list containing all abilities usable in the battle

    DYNAMIC_ARRAY_TYPE(u32)* ability_count; // how many of these abilties do we have, should correspond to the index in the battle list

} Battle_Inventory_Component;

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

    Inventory_Component inventory_component;
    Battle_Inventory_Component battle_inventory_component;

    Action_Component action_component;
    Health_Component health_component;
    MP_Component mp_component;


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
    Turn_Component_Base turn_component_base_info;
    Ability* Ability;
    Ability_Component* TurnComponent;
    DYNAMIC_ARRAY_TYPE(Ability_Component_array)* StatusComponents;
    DYNAMIC_ARRAY_TYPE(Ability_Component_array)* ComponentsWithTurnTag;
    Unit* Caster;
    Unit* TurnTarget;
} Turn_Trigger_Component_Info;

typedef struct Reversal_Component_Info
{
    //TODO: dont store pointers
    //who is affected, reversal component, components with reversal tag
    Ability* Ability;
    Reversal_Component* ReversalComponent;
    DYNAMIC_ARRAY_TYPE(Ability_Component_array)* ComponentsWithReversalTag;
    Unit* Caster;
    Unit* ReversalTarget; // who we put the reversal on
    bool IsPermanent;
} Reversal_Component_Info;

typedef struct Unit_Handle
{
    u32 index; // points back to the units array
    Character_Name name; // extra info that doesn't really need to live here

    //it will be impossible for two characters with the same name to share the same index
}Unit_Handle;



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

    //ARRAYS
    DYNAMIC_ARRAY_TYPE(Turn_Trigger_Component_Info)* turn_start_components;
    DYNAMIC_ARRAY_TYPE(Turn_Trigger_Component_Info)* turn_end_components;
    DYNAMIC_ARRAY_TYPE(Turn_Trigger_Component_Info)* turn_start_end_components;
    DYNAMIC_ARRAY_TYPE(Turn_Trigger_Component_Info)* turn_first_start_components;
    DYNAMIC_ARRAY_TYPE(Turn_Trigger_Component_Info)* turn_final_end_components;
} Ability_Handler;


typedef struct Command
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
    DYNAMIC_ARRAY_TYPE(Unit_Handle)* targets_available_array;
    //TODO: you can just calculate targets available size, and if its 1 or less, dont move the target lock


    //NOTE: regardless if single or multi target, we want to let the player know the information of the unit they are hovering over
    Unit_Handle current_lock_on_target;
    // current targets count that we are locked onto, only applicable to single target
    u8 targeting_count;
} Targeting_Handler;


typedef struct Overflow_Handler
{
    //unit -> ability and that abilties usage count
    Unit_Handle character_name;
    //
    Ability_Name ability_to_consume[Ability_Name_MAX];
    u32 usage_count[Ability_Name_MAX];
    //

} Overflow_Handler;


typedef struct Madness_Pulse_Game
{
    Allocator allocator;
    // Arena Current_State_Arena; //NOTE: to release data specific to the game state, but not unload literally everything
    Frame_Allocator frame_allocator;
    Heap_Allocator heap_allocator;

    //refs
    Resource_System* resource_system;
    Madness_UI* madness_ui;
    Event_System* event_system;
    Input_System* input_system;


    // Save_Meta_Data save_meta_data;
    Madness_Pulse_Game_State game_state;


    //MAIN MENU//x
    //TODO: Load Save File Meta Data, expand into 255 slots
    Save_Meta_Data save_meta_data[MAX_SAVE_SLOTS];
    u8 saves_found;
    u32 current_save_slot_number;

    Save_Game* save_game;

    //ABILITY SELECT / LEVEL SELECT OPTIONS MENU //

    //ABILITY SELECT //


    //LEVEL SELECT //


    //TURN BASED GAME//

    //Turn Based States
    Turn_Phase turn_phase;
    Turn_Based_UI_States ui_state;

    Ability_Registry* ability_registry;

    Targeting_Handler* targeting_handler;
    Ability_Handler* ability_handler;
    Command_Handler* command_handler;

    //PLAYER AND ENEMY UNITS
    //NOTE: there can only be one named character on each side of the
    //TODO: replace with dynamic arrays
    Unit** units;
    u8 units_count;

    //this is really just a convienence
    Unit_Handle* unit_handles;
    u8 unit_handles_count;

    //references that point back into the units array
    //NOTE: no generation index's rn, but i might need them for minion/summons
    Unit_Handle* players;
    u8 player_count;

    Unit_Handle* enemies;
    u8 enemy_count;


    //TODO: this will likely need to realloc or put a hard limit on what can be summoned into the game
    // im thinking if a unit does get added mid fight, that they will have summoners sickness,
    // meaning they cant act until the turn queue gets reset
    RING_QUEUE_TYPE(Unit_Handle)* turn_queue;
    // NOTE: right there will only be unique enemies in the game, and only a single instance allowed in play
    // FUTURE: if i need to update this in the future then do so to accomadate multiple of the same enemy
    Unit_Handle current_units_turn;
    Ability_Name currently_selected_ability;

    u32 picked_ability;
    Turn_Initiative starting_turn_initiative;


    //
} Madness_Pulse_Game;

#endif //GAME_TYPES_H
