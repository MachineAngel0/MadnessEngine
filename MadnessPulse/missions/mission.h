#ifndef MISSION_H
#define MISSION_H

#include "defines.h"
#include "game_enums.h"
#include "str.h"

//TODO: a string lookup table for the names
// keeps a list of mission names
typedef enum Mission_Name
{
    Mission_Name_Sandbox,

    Mission_Name_Tutorial,
    Mission_Name_Worshipper,
    Mission_Name_BurningSoul,
    Mission_Name_IceQueen,
    Mission_Name_SunMoonTwin,
    Mission_Name_BrotherInArms,
    Mission_Name_BloodBrothers,
    Mission_Name_HealSquad,
    Mission_Name_Freezer,
    Mission_Name_MetalStar,
    Mission_Name_Abyssal,
    Mission_Name_Slimes,
    Mission_Name_CountDown,
    Mission_Name_ReversalReversal,
    Mission_Name_Angel,
    Mission_Name_Demon,
    Mission_Name_FusionMania,
    Mission_Name_Persona,
    Mission_Name_DisfiguredMass,
    Mission_Name_InsanityProgenitor,
    Mission_Name_Dancer,
    Mission_Name_Player,
    Mission_Name_Law,
    Mission_Name_Shield,
    Mission_Name_Time1,
    Mission_Name_Time2,
    Mission_Name_Time3,
    Mission_Name_Time4,
    Mission_Name_Time5,
    Mission_Name_Tribe,
    Mission_Name_Conjure,
    Mission_Name_3SisterFather,
    Mission_Name_MultiElemental,
    Mission_Name_MPDrainer,
    Mission_Name_BlackBloodStar,
    Mission_Name_Planets,
    Mission_Name_OuterGodsAngels,
    Mission_Name_OuterGod,
    Mission_Name_MAX,
} Mission_Name;

typedef struct Mission_Unlock_Rewards
{
    Ability_Name* ability_unlock_array_ids;
    u32 unlock_count;
} Mission_Unlock_Rewards;

// This is information set by a data table,
// If a mission is unlocked, we check it by getting the map on the save data TMap<Emissionname, bool>
//TODO: figure out how you want to display and read prebattle dialogue
typedef struct Mission_Data
{
    Mission_Name mission_name;
    Mission_Name mission_to_unlock;

    String mission_display_name;

    //enemies to load in for the fight
    Character_Name* enemy_units;
    u32 enemy_count;

    String LevelName; // TODO: based on how i structure levels and scenes

    //decide who gets to go first
    Turn_Initiative starting_turn_initiative;

    bool ready_for_playtest;
} Mission_Data;


//called only during the fusion mania fight
void UnlockFusion(Save_Game* SaveGame);

Mission_Data GetMissionData(Mission_Name mission_name);

TArray<TSubclassOf<AAbilityBase>> GetMissionDataRewards();

void UnlockNextMission();

TArray<Unit*> ReturnEnemyUnitsFromMissionTable();
Turn_Initiative ReturnStartingTurnInitiative();


#endif //MISSION_H
