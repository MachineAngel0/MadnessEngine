// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitBaseComponent.h"
#include "Abilities/AbilityBase.h"
#include "ActionCommand/Interface_Action.h"
#include "Components/ActorComponent.h"
#include "InventoryManagerComponent.generated.h"




//TODO: add an event dispatcher in the event anything in the inventory changed so that the UI can update itself
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryRefresh);

class AAbilityBase;

// inventory needs to know about ability base but ability base doens't need to know about inventory

UCLASS( ClassGroup=(Unit), meta=(BlueprintSpawnableComponent), Blueprintable )
class MADNESSPULSE_API UInventoryManagerComponent : public UUnitBaseComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryManagerComponent();



	// delegates
	UPROPERTY(BlueprintAssignable, BlueprintCallable, BlueprintReadWrite, BlueprintReadWrite)
	FOnInventoryRefresh OnInventoryRefresh;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//TODO: There is no delegates for these rn, might want to look into it 

	virtual void Execute_Implementation(ATurnBasedGameMode* GameMode) override;
	

	UFUNCTION()
	void UpdateInventoryHistory();
	
	
	// TODO: inventory implementation
	// two inventories, one for the moves you are to use in the battle, and one for the moves in your reserve

	// list of abilities currently equipped onto the character meant to be used in battle
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory.Array")
	TArray<TSubclassOf<AAbilityBase>> AbilityBattleList;
	// this list is used in the character prep for all non-equipped abilities available to the character
	// to equip or unequip from the abilities battle list
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory.Array")
	TArray<TSubclassOf<AAbilityBase>> AbilityReserve;

	
	//For Battles

	//this is here so we can still undo any actions
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Inventory.Array")
	TArray<AAbilityBase*> AllSpawnedAbilityList;
	//this is the available list for the ui's to use
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Inventory.Array")
	TArray<AAbilityBase*> SpawnedAbilityList;

	UFUNCTION()
	void SpawnAbilitiesStartBattle();
	UFUNCTION()
	AAbilityBase* SpawnSingleAddedAbilities(const TSubclassOf<AAbilityBase>& AbilityToSpawn);
	UFUNCTION()
	void SpawnAddedAbilities(TArray<TSubclassOf<AAbilityBase>> AbilitiesToSpawn);
	UFUNCTION()
	void DespawnRemoveAbility(AAbilityBase* AbilityToDespawn);

	//returns the number of abilities removed
	UFUNCTION()
	int RemoveAllAbilityFromSpawnedListByClassType(TSubclassOf<AAbilityBase> AbilityClass);
	UFUNCTION()
	void RemoveAllAbilityFromSpawnedList();

	//check our spawned ability list and remove the ability if we find its name
	UFUNCTION()
	void RemoveAbilityByName(const FName& AbilityName);
	//compare our BattleList and SpawnedAbilityList and remove anything we do not find in the battle list
	UFUNCTION()
	void RemoveAbilitiesThatWeDidNotStartWith();
	
	
	//INSANITY
	void RemoveRandomAbility();
	
	
	
	/*
	 * In the future I just need to add new arrays for a generic item type like weapons
	 * ex: TArray<AItems> ItemsArray;
	 * and then probably just us a template: template <typename T>
	 * probably with some sort of assert so their isn't junk data bieng added to the inventory
	 */


	/* add to inventory utility begin*/
	// TODO: Add checks for adding and removing 

	

	
	UFUNCTION(Blueprintable, BlueprintCallable, Category="Inventory.Utility")
	void AddToBattleInventory(TSubclassOf<AAbilityBase> AbilityToAdd);
	
	UFUNCTION(Blueprintable, BlueprintCallable, Category="Inventory.Utility")
	void AddMultipleToBattleInventory(TArray<TSubclassOf<AAbilityBase>> AbilitiesToAdd);
	
	UFUNCTION(Blueprintable, BlueprintCallable, Category="Inventory.Utility")
	void AddToReserveInventory(TSubclassOf<AAbilityBase> AbilityToAdd);

	UFUNCTION(Blueprintable, BlueprintCallable, Category="Inventory.Utility")
	void AddMultipleToReserveInventory(TArray<TSubclassOf<AAbilityBase>> AbilitiesToAdd);

	
	
	/* Remove from inventory utility begin*/

	UFUNCTION(Blueprintable, BlueprintCallable, Category="Inventory.Utility")
	void ClearBattleInventory();
	
	UFUNCTION(Blueprintable, BlueprintCallable, Category="Inventory.Utility")
	void RemoveFromBattleInventory(TSubclassOf<AAbilityBase> AbilityToRemove);

	UFUNCTION(Blueprintable, BlueprintCallable, Category="Inventory.Utility")
	void RemoveMultipleFromBattleInventory(TArray<TSubclassOf<AAbilityBase>> AbilitiesToRemove);

	UFUNCTION(Blueprintable, BlueprintCallable, Category="Inventory.Utility")
	void RemoveFromReserveInventory(TSubclassOf<AAbilityBase> AbilityToRemove);
	
	UFUNCTION(Blueprintable, BlueprintCallable, Category="Inventory.Utility")
	void RemoveMultipleFromReserveInventory(TArray<TSubclassOf<AAbilityBase>> AbilitiesToRemove);
	

	
	// refresh inventory (for ui reasons)
	UFUNCTION(Blueprintable, BlueprintCallable, Category="Inventory.Utility")
	void RefreshInventory();

	//TODO: check for duplicates in the array and probably make a quantity limit for each move
	// meant to be a helper function 
	void DeleteDuplicatesInInventory(TArray<TSubclassOf<AAbilityBase>> ArrayToRemoveDuplicates);

	//todo::delete later
	void DeleteLater();

	// UI Functionality	
	UFUNCTION(Blueprintable, BlueprintCallable, Category="Inventory.Utility")
	void QuickMoveItemFromBattleToReserve(int BattleListIndex);
	UFUNCTION(Blueprintable, BlueprintCallable, Category="Inventory.Utility")
	void QuickMoveItemFromReserveToBattle(int ReserveIndex);

	UFUNCTION(Blueprintable, BlueprintCallable, Category="Inventory.Utility")
	void ReorderBattleList(int IndexOne, int IndexTwo);

	UFUNCTION(Blueprintable, BlueprintCallable, Category="Inventory.Utility")
	void ReorderReserveList(int IndexOne, int IndexTwo);

	UFUNCTION(Blueprintable, BlueprintCallable, Category="Inventory.Utility")
	void SwapAbilitiesBetweenLists(int BattleListIndex, int ReserveListIndex);

	//sends all abilites in battle list to reserve
	UFUNCTION(Blueprintable, BlueprintCallable, Category="Inventory.Utility")
	void ClearBattleList();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory.Utility")
	FString BattleListID{"BattleList"};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory.Utility")
	FString ReserveListID{"ReserveList"};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory.Utility")
	TMap<FString, int> SwapMap;
	
	UFUNCTION(Blueprintable, BlueprintCallable, Category="Inventory.Utility")
	void QuerySwapMap(FString ID, int AbilityIndex);
	// true if it swaps
	UFUNCTION(Blueprintable, BlueprintCallable, Category="Inventory.Utility")
	bool QuerySwapMapWithBoolReturn(FString ID, int AbilityIndex);
	UFUNCTION(Blueprintable, BlueprintCallable, Category="Inventory.Utility")
	void ClearSwapMap();
	UFUNCTION(Blueprintable, BlueprintCallable, Category="Inventory.Utility")
	bool IsSwapMapClear();

	UFUNCTION(Blueprintable, BlueprintCallable, Category="Inventory.Utility")
	void SortBySubclassName();
	UFUNCTION(Blueprintable, BlueprintCallable, Category="Inventory.Utility")
	void SortByPrimaryAbilityType();


	//Pass Ability
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AAbilityBase> PassAbility;
	void AddPassAbility();
	void IsAbilityListEmptyAddPassAbility();


	//FOR ABILITY SELECT
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<TSubclassOf<AAbilityBase>, FAbilityInfo> SubclassToAbilityBattleList;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<TSubclassOf<AAbilityBase>, FAbilityInfo> SubclassToAbilityReserveList;
	UFUNCTION(Blueprintable, BlueprintCallable, Category="Inventory.Utility")
	void SortByName();
};
