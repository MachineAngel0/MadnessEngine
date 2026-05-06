

#include "targeting_handler.h"

#include "logger.h"



void SetTargetingLockSingleOrMulti(Target_Area_Affect single_or_multi_target)
{
	//
	CurrentAbilityTargetCanAffectType = single_or_multi_target;

	switch (single_or_multi_target)
	{
	case ETargetsCanAffect::ECS_SingleTarget:
		CurrentLockOnTarget = TargetsAvailable[0];
		CurrentLockOnTarget->ShowTargetLock();
		CurrentLockOnTarget->ShowTargetLockResistanceDisplay();
		CanMoveTargeting = true;
		break;
	case ETargetsCanAffect::ECS_TargetAll:
		for (const auto& Targets : TargetsAvailable)
		{
			Targets->ShowTargetLock();
		}
		// look at the first one
		CurrentLockOnTarget = TargetsAvailable[0];
		//only have one unit show their resistance display
		CurrentLockOnTarget->ShowTargetLockResistanceDisplay();
		CanMoveTargeting = true;
		//CanMoveTargeting = false;
		break;
	}


}








void CreateAITargeting(FTurnBasedGameState GameState, AAbilityBase* AbilityChosen,
                                            AAIAction* ChosenAction)
{
	ProcessAITargets(GameState, AbilityChosen);
	ProcessAITargetsCanAffect(GameState, ChosenAction, AbilityChosen);
}

void ProcessAITargets(FTurnBasedGameState GameState, const AAbilityBase* AbilityChosen)
{
	TArray<Unit*> TargetsToSendToSetTargets;
	switch (AbilityChosen->AbilityInfo.AbilityTargetType)
	{
	case EAbilityTargetType::ECS_Self:
		TargetsToSendToSetTargets.Add(GameState.CurrentUnitsTurn);
		AddToAIAbilityTargetsAvailable(GameState.CurrentUnitsTurn, TargetsToSendToSetTargets, false);
		break;
	case EAbilityTargetType::ECS_Allies:
		AddToAIAbilityTargetsAvailable(GameState.CurrentUnitsTurn, GameState.EnemyUnits, true);
		break;
	case EAbilityTargetType::ECS_SelfAndAllies:
		AddToAIAbilityTargetsAvailable(GameState.CurrentUnitsTurn, GameState.EnemyUnits, false);
		break;
	case EAbilityTargetType::ECS_Enemies:
		AddToAIAbilityTargetsAvailable(GameState.CurrentUnitsTurn, GameState.PlayerUnits, true);
		break;
	case EAbilityTargetType::ECS_SelfAndEnemies:
		TargetsToSendToSetTargets.Add(GameState.CurrentUnitsTurn);
		TargetsToSendToSetTargets.Append(GameState.PlayerUnits);
		AddToAIAbilityTargetsAvailable(GameState.CurrentUnitsTurn, TargetsToSendToSetTargets, false);
		break;
	case EAbilityTargetType::ECS_AlliesAndEnemies:
		TargetsToSendToSetTargets.Append(GameState.PlayerUnits);
		TargetsToSendToSetTargets.Append(GameState.EnemyUnits);
		AddToAIAbilityTargetsAvailable(GameState.CurrentUnitsTurn, TargetsToSendToSetTargets, true);
		break;
	case EAbilityTargetType::ECS_All:
		TargetsToSendToSetTargets.Append(GameState.PlayerUnits);
		TargetsToSendToSetTargets.Append(GameState.EnemyUnits);
		AddToAIAbilityTargetsAvailable(GameState.CurrentUnitsTurn, TargetsToSendToSetTargets, false);
		break;
	default:
		UE_LOG(Log_AIController, Display, TEXT("Using Intended Enemy Target"));
		break;
	}
}


void ProcessAITargetsCanAffect(FTurnBasedGameState GameState, AAIAction* ChosenAction, const AAbilityBase* AbilityChosen)
{
	// note: it is up to the consideration if they want to target a dead unit, otherwise, if the pick is random, then filter out dead targets
	if (!AITargetsAvailableByAffect.IsEmpty())
	{
		AITargetsAvailableByAffect.Empty();
	}

	switch (AbilityChosen->AbilityInfo.TargetsCanAffect)
	{
	// were saying is we are targeting an enemy and consideration is valid and its intended enemy target is valid then pick intended target
	case ETargetsCanAffect::ECS_SingleTarget:
		if ((AbilityChosen->AbilityInfo.AbilityTargetType == EAbilityTargetType::ECS_Enemies
				|| AbilityChosen->AbilityInfo.AbilityTargetType == EAbilityTargetType::ECS_SelfAndEnemies) &&
			(ChosenAction && ChosenAction->IntendedUnitTarget))
		{
			// choose our target based on what the consideration wanted;
			UE_LOG(Log_AIController, Display, TEXT("Using Intended Enemy Target"));
			AITargetsAvailableByAffect.Emplace(ChosenAction->IntendedUnitTarget);
		}
		// were saying is we are targeting an ally and consideration is valid and its intended friendly target is valid then pick intended target
		else if ((AbilityChosen->AbilityInfo.AbilityTargetType == EAbilityTargetType::ECS_Allies
				|| AbilityChosen->AbilityInfo.AbilityTargetType == EAbilityTargetType::ECS_SelfAndAllies) &&
			(ChosenAction && ChosenAction->IntendedUnitTarget))
		{
			UE_LOG(Log_AIController, Display, TEXT("Using Intended Friendly Target"));
			AITargetsAvailableByAffect.Emplace(ChosenAction->IntendedUnitTarget);
		}
		else
		{
			//filter out dead targets
			TArray<Unit*> TargetsToRemove;
			for (const auto& Target : AITargetsAvailableByType)
			{
				if (Target->HealthComponent->IsDead())
				{
					TargetsToRemove.Emplace(Target);
				}
			}
			for (const auto& Target : TargetsToRemove)
			{
				AITargetsAvailableByType.Remove(Target);
			}

			UE_LOG(LogTemp, Display, TEXT("Random Single Unit Picked"));
			AITargetsAvailableByAffect.Add(ReturnRandomUnit(GameState, AITargetsAvailableByType));
		}
		break;
	case ETargetsCanAffect::ECS_TargetAll:
		AITargetsAvailableByAffect.Append(AITargetsAvailableByType);
	default: break;
	}
}

void AddToAIAbilityTargetsAvailable(Unit* CurrentUnitsTurn, TArray<Unit*> TargetsToAdd, bool ExcludeSelf)
{
	AITargetsAvailableByType.Empty();
	AITargetsAvailableByType.Append(TargetsToAdd);
	if (ExcludeSelf)
	{
		int Index = AITargetsAvailableByType.Find(CurrentUnitsTurn);
		if (Index != INDEX_NONE)
		{
			AITargetsAvailableByType.Remove(CurrentUnitsTurn);
		}
	}
}


Unit* ReturnRandomUnit(FTurnBasedGameState GameState, TArray<Unit*> Units)
{
	// rand() by % length of array returns numbers only within the array
	if (Units.IsEmpty()) return GameState.PlayerUnits[0];

	return Units[rand() % Units.Num()];
}
