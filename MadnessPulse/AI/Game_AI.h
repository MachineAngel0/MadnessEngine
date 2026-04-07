#ifndef ABILITY_LIST_H
#define ABILITY_LIST_H

#include "game_enums.h"
#include "game_structs.h"
#include "logger.h"



//up to a child class if they want to use this
typedef enum EAIConsiderationTargetType
{
	ECS_AISelf,
	ECS_PlayerCharacter,
	ECS_AIUnits,
	ECS_All,
}EAIConsiderationTargetType;

//TODO: NOTE: I AM MOST DEFINETLY NOT GONNA BE ABLE TO IMPLEMENT SOMETHING LIKE A CURVE
typedef enum ECurveType
{
	// IMPORTANT NOTE: these are in the context of defense so that there is a concrete explanation for how to use these
	ECS_Normal, // what you would expect the ai to do

	ECS_SuperAggressive, // we want to be REALLY aggressive about buffing our defense, we want that defense at 90% or 100%
	ECS_Aggressive, // we want to be aggressive about buffing our defense, but not to the point of 100
	ECS_Passive, // we want to be a bit more passive about buffing our defense, when it's getting a bit low, but we might want to do other things
	ECS_SuperPassive, // we want to be really passive about buffing our defense, basically only when we absolutely must
	ECS_ExtremeEnds, // we want to be buff only if its super low or super high, we don't really care if it's in the middle
	ECS_ExtremeMiddle, // we want to be buff only if its super low or super high, we don't really care if it's in the middle

	//These are here to spice up the ai behavior and make them less predicatable
	ECS_OscillationHighEnds,
	ECS_OscillationLowEnds,


}ECurveType;


typedef struct FUnitScore
{
	u32 unit_id;
	float Score;
}FUnitScore;



struct UAIConsideration
{

	//intended target, and score
	TPair<AUnitBase*, float> IntendedTargetAndScore;

	// NOTE: since we are in the perspective of the AI, player units are the players, and enemy units are the ai units
	/*SINGLE TARGET*/
	virtual float ReturnSingleTargetScore(TArray<AUnitBase*> PlayerUnits, TArray<AUnitBase*> EnemyUnits, AUnitBase* AICaster, const TMap<ECharacterName, float>&
	                                      TargetSkew, FUnitScore& UnitScore);

	/*MULTI TARGET*/
	virtual float ReturnMultiTargetScore(TArray<AUnitBase*> PlayerUnits, TArray<AUnitBase*> EnemyUnits, AUnitBase* AICaster);

	/* UTILITY */
	static float GetNameSkew(const TMap<ECharacterName, float>& TargetSkew, ECharacterName CharacterName);

	void SetCurveBasedOnType();
	ECurveType CurveType = ECurveType::ECS_Normal;
	TObjectPtr<UCurveFloat> CurrentCurveType;

	//these account for a majority of use cases, if I need a custom one, just override one of them, preferably the normal one
	TObjectPtr<UCurveFloat> NormalCurveType;
	TObjectPtr<UCurveFloat> SuperPassiveCurveType;
	TObjectPtr<UCurveFloat> SuperAggressiveCurveType;
	TObjectPtr<UCurveFloat> AggressiveCurveType;
	TObjectPtr<UCurveFloat> PassiveCurveType;
	TObjectPtr<UCurveFloat> ExtremeEndsCurveType;
	TObjectPtr<UCurveFloat> ExtremeMiddleCurveType;
	TObjectPtr<UCurveFloat> OscillationHighEndsCurveType;
	TObjectPtr<UCurveFloat> OscillationLowEndsCurveType;


	static TArray<AUnitBase*> ReturnConsiderationArrayType(EAIConsiderationTargetType ConsiderationTargetType, const TArray<AUnitBase*>& PlayerUnits, const TArray<AUnitBase*>& EnemyUnits, AUnitBase
	                                                       * AICaster);
	TArray<AUnitBase*> ReturnAbilityConsiderationArrayType(const TArray<AUnitBase*>& PlayerUnits, const TArray<AUnitBase*>& EnemyUnits, AUnitBase
													   * AICaster) const;

	//mostly to gain access to what ability we are using
	AAIAction* ActionOwner;

	AAbilityBase* GetAbility();

};


//TODO: these two structs are not needed, since we can just look into a table to see what abilties a unit has
struct FAIAbilityList
{
	//we literally do not need a map, this is looping throght literally 3 indexes at most, and at worst 10-11
	int AbilityTurnIndex = -1;
	TArray<TSubclassOf<AAbilityBase>> AbilityList;
};

struct FAIAbilityLists
{
	ECharacterName AIName;

	TArray<FAIAbilityList> AbilityLists;

	TArray<TSubclassOf<AAbilityBase>> ExtraAbilityList;
};




struct AAIControllerBase
{

	//TODO: i might, just for clean up, make an array targeting component
	UTargetingComponent* TargetingComponent;

	// interface
	virtual void RequestInfoFromBattleManager_Implementation(FTurnBasedGameState GameState, AAbilityBase*& AbilityToUse, TArray<AUnitBase*>& Targets) override;

	//TODO: add checks to make sure nothing return empty or invalid

	//Sort Abilities
	// Main Aspect:
	// damage/status abilities
	// heal abilities
	// augment change abilities
	// damage reduction abilities
	// Start/End Turn
	// Sub Aspect:
	// status trigger abilties

	void ChooseAbility(const TArray<AUnitBase*>& PlayerUnits, const TArray<AUnitBase*>& EnemyUnits, AAbilityBase*& AbilityChosen, AAIAction*&
	                      ChosenAction);

	AUnitBase* UnitReference;

	int UnitsMaxActionsAvailable = 0;

	static AAIAction* PickRandomAction(TArray<AAIAction*> ActionsAvailable, const FString& ReasonText);

	// Random Consideration Map
	bool ReturnShouldUseRandomAbility(int ActionsLeft);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UseRandomAbility")
	TMap<int, bool> ShouldPickRandomAbility
	{
		{0, false},
		{1, false},
		{2, false},
		{3, false},
		{4, false},
		{5, false},
		{6, false},
		{7, false},
		{8, false},
		{9, false},
		{10, false},
	};

	void DebugPrintActionsArray();


	TArray<TSubclassOf<AAIAction>> Actions;
	TArray<AAIAction*> SpawnedActions;

	//Utility AI, the considerations are responsible for adding themselves to the arrays
	TMap<int, TArray<AAIAction*>> ActionArray{
			{1, {}},
			{2, {}},
			{3, {}},
			{4, {}},
			{5, {}},
			{6, {}},
			{7, {}},
			{8, {}},
			{9, {}},
			{10, {}},
			{11, {}},
			{12, {}},
			//i shouldn't need for than this
		};

	//Utility AI, the considerations are responsible for adding themselves to the arrays
	TArray<AAIAction*> ExtraActionsArray;


	TArray<AUnitBase*> TargetsAvailableByType;
	TArray<AUnitBase*> TargetsAvailableByAffect;


	void InjectAbility(int AbilityMapLocation, TArray<TSubclassOf<AAbilityBase>> AbilitiesToAdd);
	// removes abilities
	void RemoveAbility(int AbilityMapLocation, TArray<TSubclassOf<AAbilityBase>> AbilitiesToRemove);
	void RemoveAllAbilitiesAtIndex(int AbilityMapLocation);


	void InjectActions(TArray<TSubclassOf<AAIAction>> ActionsToAdd);
	//TODO: doesn't remove from the extras array
	void RemoveAction(TArray<TSubclassOf<AAIAction>> ActionsToRemove);

	//Rn every child has a binding to every single needed thing, not really a performance for this
	void CreateBindings(ATurnBasedGameMode* GameMode);

	// helper functions that create bindings
	void CreateEnemyDeathBinding(ATurnBasedGameMode* GameMode);
	void CreatePawnHealthChangeBinding(ATurnBasedGameMode* GameMode);
	void CreateTurnChangeBinding(ATurnBasedGameMode* GameMode);
	void CreateAbilityUsedBinding(ATurnBasedGameMode* GameMode);

	// these are the functions that will trigger based on what the binding is
	// these are all up to the child class on how to implement
	void OnEnemyDeathBinding();
	void OnPawnHealthChangeBinding(float HealthPercent, float HealthAmount);
	void OnTurnChangeBinding(int TurnNumber);
	void OnAbilityUsedBinding(TSubclassOf<AAbilityBase> AbilityUsed);
};



#endif //ABILITY_LIST_H
