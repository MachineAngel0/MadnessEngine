#ifndef SAVE_GAME_H
#define SAVE_GAME_H
#include <stdbool.h>

#include "arena.h"

/* Save data related things*/
typedef struct Player_Save_Data
{
	TArray<TSubclassOf<AAbilityBase>> AbilityListSave;
	TArray<TSubclassOf<AAbilityBase>> AbilityReserveSave;
}Player_Save_Data;

typedef struct Save_Meta_Data
{
	float GameTime;
	int MissionsBeaten;
}Save_Meta_Data;

/**
 *	The place where all save info variables are to be placed and accessed
 */
typedef struct Save_Game
{
	TMap<TSubclassOf<AUnitBase>, Player_Save_Data> PlayerSaveInfo;

	//TODO: set the values needed to false
	bool MissionDataSaveUnlocksNames[Mission_Name_MAX] = {
		[Mission_Names_Tutoria] = false,
		{EMissionNames::ECS_Sandbox, false},

		//for the demo
		{Mission_Name_Worshipper, true},
		{EMissionNames::ECS_BurningSoul, true},
		{EMissionNames::ECS_IceQueen, true},
		{EMissionNames::ECS_SunMoonTwin, true},
		{EMissionNames::ECS_FusionMania, true},

	};
	// stores whether a mission is unlocked or not,
	// they need to be the same name as the level


	//TODO: make fusion available during the fusion fight, and save the setting permanently from there
	bool AllowedToFusion = false;

	//TODO:
	/*SAVE SETTING DATA ONLY*/

	//TODO: this is here for displaying information like the players progress and gametime
	TArray<TMap<FString, Save_Meta_Data>> SaveGameSlotsToSlotInfoDisplay;
}Save_Game;

//NOTE: THIS FUNCTION IS NEW
Save_Game* save_game_load(Arena* arena);

void SaveAllData(TArray<AActor*> SavableObject);
void LoadDataForRequester(AActor* Requester);
void SavePlayerData(TArray<AActor*> SavableObject);

	void DebugUnlockAllMissions();

//dont use this function
void StoreAbilities(TSubclassOf<AUnitBase> Unit, TArray<TSubclassOf<AAbilityBase>> Abilities);

void AddAbilitiesToAllUnitsReserveList(TArray<TSubclassOf<AAbilityBase>> AbilitiesToAdd);
TArray<TSubclassOf<AAbilityBase>> ReturnUnitAbilities(TSubclassOf<AUnitBase> Unit);




#endif //SAVE_GAME_H