

#include "targeting.h"

#include "logger.h"


TArray<Unit*> GetAITargetsAvailableByAffect() const
{
	return AITargetsAvailableByAffect;
}

TArray<Unit*> GetAITargetsAvailableByType() const
{
	return AITargetsAvailableByType;
}






void CreateTargeting(FTurnBasedGameState GameState, AAbilityBase* AbilityChosen)
{

	CreateAbilityTargetingArray(GameState, AbilityChosen);
	SetTargetingLockSingleOrMulti(AbilityChosen->AbilityInfo.TargetsCanAffect);
}

void CreateAbilityTargetingArray(FTurnBasedGameState GameState, const AAbilityBase* AbilityChoosen)
{
	CurrentAbilityTargetType = AbilityChoosen->AbilityInfo.AbilityTargetType;

	TArray<Unit*> TargetsToSendToSetTargets;
	switch (AbilityChoosen->AbilityInfo.AbilityTargetType)
	{
	case EAbilityTargetType::ECS_Self:
		TargetsToSendToSetTargets.Add(GameState.CurrentUnitsTurn);
		AddToAbilityTargetsAvailable(GameState.CurrentUnitsTurn, TargetsToSendToSetTargets, false);
		break;
	case EAbilityTargetType::ECS_Allies:
		AddToAbilityTargetsAvailable(GameState.CurrentUnitsTurn, GameState.PlayerUnits, true);
		break;
	case EAbilityTargetType::ECS_SelfAndAllies:
		AddToAbilityTargetsAvailable(GameState.CurrentUnitsTurn, GameState.PlayerUnits, false);
		break;
	case EAbilityTargetType::ECS_Enemies:
		AddToAbilityTargetsAvailable(GameState.CurrentUnitsTurn, GameState.EnemyUnits, true);
		break;
	case EAbilityTargetType::ECS_SelfAndEnemies:
		TargetsToSendToSetTargets.Add(GameState.CurrentUnitsTurn);
		TargetsToSendToSetTargets.Append(GameState.EnemyUnits);
		AddToAbilityTargetsAvailable(GameState.CurrentUnitsTurn, TargetsToSendToSetTargets, false);
		break;
	case EAbilityTargetType::ECS_AlliesAndEnemies:
		TargetsToSendToSetTargets.Append(GameState.PlayerUnits);
		TargetsToSendToSetTargets.Append(GameState.EnemyUnits);
		AddToAbilityTargetsAvailable(GameState.CurrentUnitsTurn, TargetsToSendToSetTargets, true);
		break;
	case EAbilityTargetType::ECS_All:
		TargetsToSendToSetTargets.Append(GameState.PlayerUnits);
		TargetsToSendToSetTargets.Append(GameState.EnemyUnits);
		AddToAbilityTargetsAvailable(GameState.CurrentUnitsTurn, TargetsToSendToSetTargets, false);
		break;
	default:
		FATAL("Failed To Create Ability Targeting Array");
		break;
	}
}

void AddToAbilityTargetsAvailable(Unit* CurrentUnit, TArray<Unit*> TargetsToAdd, bool ExcludeSelf)
{
	TargetsAvailable.Empty();
	TargetsAvailable.Append(TargetsToAdd);
	if (ExcludeSelf)
	{
		TargetsAvailable.Remove(CurrentUnit);
	}
}

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




TArray<Unit*> UTargetingComponent::ReturnTargetsForActionManager(AAbilityBase* AbilityChoosen)
{
	TArray<Unit*> TempArray;
	if (AbilityChoosen->AbilityInfo.TargetsCanAffect == ETargetsCanAffect::ECS_SingleTarget)
	{
		TempArray.Add(CurrentLockOnTarget);
		return TempArray;
	}
	if (AbilityChoosen->AbilityInfo.TargetsCanAffect == ETargetsCanAffect::ECS_TargetAll)
	{
		return TargetsAvailable;
	}
	return TempArray;
}

void UTargetingComponent::ClearAllTargetLocks()
{
	if (CurrentLockOnTarget)
	{
		CurrentLockOnTarget->HideTargetLock();
		CurrentLockOnTarget->HideTargetLockResistanceDisplay();
	}
	if (!TargetsAvailable.IsEmpty())
	{
		for (const auto& LockOnTargets : TargetsAvailable)
		{
			LockOnTargets->HideTargetLock();
			CurrentLockOnTarget->HideTargetLockResistanceDisplay();
		}
	}
}


void UTargetingComponent::MoveUnitTargeting(const ETargetingDirection MoveTargetDirection)
{

	// TODO:
	// check if its a single target or multitarget move
	// check if its for a allies, then we need to flip the move direction

	if (!CanMoveTargeting) return;

	// if its single target, we want to hide the target lock on and resistance display
	// if its multli target, we want to hide only the resistance display
	switch (CurrentAbilityTargetCanAffectType)
	{
	case ETargetsCanAffect::ECS_SingleTarget:
		CurrentLockOnTarget->HideTargetLock();
		CurrentLockOnTarget->HideTargetLockResistanceDisplay();
		break;
	case ETargetsCanAffect::ECS_TargetAll:
		CurrentLockOnTarget->HideTargetLockResistanceDisplay();
		break;
	}

	// if we are targeting our allies in anyway, then we want to flip the direction of the move targeting, due to the camera direction and player order
	if(CurrentAbilityTargetType == EAbilityTargetType::ECS_Allies || CurrentAbilityTargetType == EAbilityTargetType::ECS_SelfAndAllies)
	{
		switch (MoveTargetDirection)
		{
		case ETargetingDirection::ECS_Left:
			MovRightUnitTargeting();
			break;
		case ETargetingDirection::ECS_Right:
			MoveLeftUnitTargeting();
			break;
		}
	}
	else
	{
		switch (MoveTargetDirection)
		{
		case ETargetingDirection::ECS_Left:
			MoveLeftUnitTargeting();
			break;
		case ETargetingDirection::ECS_Right:
			MovRightUnitTargeting();
			break;
		}
	}

	CurrentLockOnTarget = TargetsAvailable[TargetingCount];
	CurrentLockOnTarget->ShowTargetLock();
	CurrentLockOnTarget->ShowTargetLockResistanceDisplay();
}

void UTargetingComponent::MoveLeftUnitTargeting()
{
	if (TargetingCount <= 0)
	{
		TargetingCount = TargetsAvailable.Num() - 1;
	}
	else
	{
		TargetingCount--;
	}
}

void UTargetingComponent::MovRightUnitTargeting()
{
	if (TargetingCount >= TargetsAvailable.Num() - 1)
	{
		TargetingCount = 0;
	}
	else
	{
		TargetingCount++;
	}
}

void UTargetingComponent::ReturnToAbilitySelect()
{
	ClearAllTargetLocks();
}

void UTargetingComponent::CreateAITargeting(FTurnBasedGameState GameState, AAbilityBase* AbilityChosen,
                                            AAIAction* ChosenAction)
{
	ProcessAITargets(GameState, AbilityChosen);
	ProcessAITargetsCanAffect(GameState, ChosenAction, AbilityChosen);
}

void UTargetingComponent::ProcessAITargets(FTurnBasedGameState GameState, const AAbilityBase* AbilityChosen)
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


void UTargetingComponent::ProcessAITargetsCanAffect(FTurnBasedGameState GameState, AAIAction* ChosenAction, const AAbilityBase* AbilityChosen)
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

void UTargetingComponent::AddToAIAbilityTargetsAvailable(Unit* CurrentUnitsTurn, TArray<Unit*> TargetsToAdd, bool ExcludeSelf)
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


Unit* UTargetingComponent::ReturnRandomUnit(FTurnBasedGameState GameState, TArray<Unit*> Units)
{
	// rand() by % length of array returns numbers only within the array
	if (Units.IsEmpty()) return GameState.PlayerUnits[0];

	return Units[rand() % Units.Num()];
}
