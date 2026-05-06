#ifndef MISSION_H
#define MISSION_H

#include "defines.h"
#include "game_enums.h"
#include "str.h"

//TODO: a string lookup table for the names
// keeps a list of mission names
typedef enum Level_Name
{
    Level_Name_Sandbox,

    Level_Name_Tutorial,
    Level_Name_Worshipper,
    Level_Name_BurningSoul,
    Level_Name_IceQueen,
    Level_Name_SunMoonTwin,
    Level_Name_BrotherInArms,
    Level_Name_BloodBrothers,
    Level_Name_HealSquad,
    Level_Name_Freezer,
    Level_Name_MetalStar,
    Level_Name_Abyssal,
    Level_Name_Slimes,
    Level_Name_CountDown,
    Level_Name_ReversalReversal,
    Level_Name_Angel,
    Level_Name_Demon,
    Level_Name_FusionMania,
    Level_Name_Persona,
    Level_Name_DisfiguredMass,
    Level_Name_InsanityProgenitor,
    Level_Name_Dancer,
    Level_Name_Player,
    Level_Name_Law,
    Level_Name_Shield,
    Level_Name_Time1,
    Level_Name_Time2,
    Level_Name_Time3,
    Level_Name_Time4,
    Level_Name_Time5,
    Level_Name_Tribe,
    Level_Name_Conjure,
    Level_Name_3SisterFather,
    Level_Name_MultiElemental,
    Level_Name_MPDrainer,
    Level_Name_BlackBloodStar,
    Level_Name_Planets,
    Level_Name_OuterGodsAngels,
    Level_Name_OuterGod,
    Level_Name_MAX,
} Level_Name;

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

    String mission_display_name;

    //enemies to load in for the fight
    Character_Name* enemy_units;
    u32 enemy_count;

    String LevelName; // TODO: based on how i structure levels and scenes

    //decide who gets to go first
    Turn_Initiative starting_turn_initiative;

    bool ready_for_playtest;
} Game_Level_Data;


Game_Level_Data get_level_data(Level_Name level_name);

Level_Unlock_Rewards get_level_unlock_rewards_data();

void unlock_next_level();




#endif //MISSION_H
