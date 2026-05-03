#ifndef TARGETING_H
#define TARGETING_H
#include "game_abilties.h"
#include "game_structs.h"


typedef enum ETargetingDirection
{
    ECS_Left,
    ECS_Right,
}ETargetingDirection;


struct UTargetingComponent
{
    //target we are locked onto
    Unit* CurrentLockOnTarget; // TODO: make it a u32 or Character Name

    // current targets count that we are locked onto
    int TargetingCount;

    //current targets available
    TArray<Unit*> TargetsAvailable;

    bool CanMoveTargeting = false;

    Target_Area_Affect CurrentAbilityTargetCanAffectType;
    Ability_Target_Type CurrentAbilityTargetType;


    TArray<Unit*> AITargetsAvailableByAffect;
    TArray<Unit*> AITargetsAvailableByType;
};

TArray<Unit*> GetAITargetsAvailableByAffect();
TArray<Unit*> GetAITargetsAvailableByType();


//called by the battle manager
//TODO: Probably have it pass in all the units, makes it easier to do any wierd mid battle enemy changes
void CreateTargeting(Madness_Pulse_Game* GameState, Ability* AbilityChosen);


void CreateAbilityTargetingArray(Madness_Pulse_Game* GameState, const Ability* AbilityChoosen);
void AddToAbilityTargetsAvailable(Unit* CurrentUnit, TArray<Unit*> TargetsToAdd, bool ExcludeSelf);

void SetTargetingLockSingleOrMulti(Target_Area_Affect single_or_multi_target);


TArray<Unit*> ReturnTargetsForActionManager(Ability* AbilityChoosen);
void ClearAllTargetLocks();


void MoveUnitTargeting( ETargetingDirection MoveTargetDirection);
void MoveLeftUnitTargeting();
void MovRightUnitTargeting();


void ReturnToAbilitySelect();

// AI Targeting
void CreateAITargeting(Madness_Pulse_Game* GameState, Ability* AbilityChosen, AAIAction* ChosenAction);

void ProcessAITargets(Madness_Pulse_Game GameState, const Ability* AbilityChosen);
void ProcessAITargetsCanAffect(Madness_Pulse_Game GameState, AAIAction* ChosenAction,
                               const Ability* AbilityChosen);

void AddToAIAbilityTargetsAvailable(Unit* CurrentUnitsTurn, TArray<Unit*> TargetsToAdd, bool ExcludeSelf);

Unit* ReturnRandomUnit(Madness_Pulse_Game GameState, TArray<Unit*> Units);


#endif //TARGETING_H
