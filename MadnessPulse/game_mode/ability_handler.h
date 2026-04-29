#ifndef ABILITY_HANDLER_H
#define ABILITY_HANDLER_H

#include "game_enums.h"
#include "game_structs.h"
#include "command/command.h"


typedef struct FTurnComponentDuration
{
    int EffectLength = 0;
    int TurnsUntilTriggered = 0;
}FTurnComponentDuration;

typedef struct Turn_Trigger_Component_Info
{
    //TODO: dont store pointers
    FTurnComponentDuration TurnComponentDuration;
    AAbilityBase* Ability;
    UAbilityBaseComponent* TurnComponent;
    TArray<UAbilityBaseComponent*> StatusComponents;
    TArray<UAbilityBaseComponent*> ComponentsWithTurnTag;
    Unit* Caster;
    Unit* TurnTarget;
}Turn_Trigger_Component_Info;

typedef struct Reversal_Component_Info
{
    //TODO: dont store pointers
    //who is affected, reversal component, components with reversal tag
    AAbilityBase* Ability;
    UReversalComponent* ReversalComponent;
    TArray<UAbilityBaseComponent*> ComponentsWithReversalTag;
    Unit* Caster;
    Unit* ReversalTarget; // who we put the reversal on
    bool IsPermanent = false;
}Reversal_Component_Info;

typedef enum Action_Handler_Process_Stage
{
    ECS_None,
    ECS_Normal,
    ECS_Status,
    ECS_ReversalTrigger,
    ECS_Reversal,
    ECS_TurnComponent,
    ECS_TurnTrigger,
    ECS_TurnEnd,
}Action_Handler_Process_Stage;


// this component is only responsible for telling the abilities to process themselves
// also responsible for managing any conditional or turn based effects like status trigger, poison, or reversals

// Order: Normal components, -> Status triggers -> Reversal Triggers -> Reversal Add -> turn components Add -> Turn End
typedef struct Ability_Handler
{
    void action_handler_init();


    /*
    // DELEGATES ?? //NOTE: IDK WHAT I WAS ON
    UVFXSubsystem* VFXSubsystem = nullptr;
    AClearStatusTriggersAction* ClearStatusAction;
    AClearReversalsAction* ClearReversalAction;
    AStatusTriggerAction* StatusTriggerAction;
    */

    /* High level Components Logic Functions */
    TArray<Command*> ProcessAbility( AAbilityBase*& AbilityToProcess, FTurnBasedGameState GameState,
                                    TArray<Unit*>& Targets);

    /*
    TArray<Command*> ProcessNormalComponents( FTurnBasedGameState& GameState,
                                             TArray<Unit*>& Targets,
                                             TArray<UAbilityBaseComponent*> NormalComponents);


    TArray<Command*> ProcessStatusTrigger( FTurnBasedGameState GameState,
                                          TArray<AUnitBase*> Targets,
                                          TArray<UAbilityBaseComponent*> StatusTriggerComponents);

    bool IsThereAReversalTrigger();
    TArray<Command*> ProcessReversal( FTurnBasedGameState GameState);

    AReversalPlayBackAction* ReversalPlayBackAction;

    void AddReversalComponentToList(AAbilityBase* Ability,
                                    TArray<UAbilityBaseComponent*>& ReversalTriggerComponents,
                                    UReversalComponent*& ReversalComponent,
                                    FTurnBasedGameState& GameState,
                                    TArray<Unit*>& Targets);


    void AddTurnComponentToList(TArray<UTurnComponentBase*>& TurnComponents,
                                TArray<UAbilityBaseComponent*>& TurnTriggerComponents,
                                TArray<UAbilityBaseComponent*>& StatusTriggerComponents,
                                AAbilityBase*& AbilityToProcess,
                                Unit* UnitCaster,
                                TArray<Unit*>& Targets);


    // turn start/ends checks //

    //check the turn type for any turntype specific function calls, then process turn components
    TArray<Command*> ProcessTurnType(
        FTurnBasedGameState& GameState, ETurnActivationType TurnType);

    //handles checking is a turn component can activate
    TArray<Command*> ProcessTurnComponents(
        FTurnBasedGameState& GameState, ETurnActivationType TurnType);


    //called only on first turn start
    void RemovalReversalFromTurnUnit(Unit* CurrentUnitsTurn);


    TArray<Reversal_Component_Info> TurnReversalList;

    TArray<Turn_Trigger_Component_Info> TurnComponentsInfo;


    //TODO:
    void ResetActionManager();*/
}Ability_Handler;


#endif //ABILITY_HANDLER_H
