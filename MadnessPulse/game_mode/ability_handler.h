#ifndef ABILITY_HANDLER_H
#define ABILITY_HANDLER_H




Ability_Handler* ability_handler_init(Madness_Pulse_Game* game)
{
    Ability_Handler* ability_handler =  allocator_alloc(&game->allocator, sizeof(Ability_Handler));
    return ability_handler;
}


/*
// DELEGATES ?? //NOTE: IDK WHAT I WAS ON
UVFXSubsystem* VFXSubsystem = nullptr;
AClearStatusTriggersAction* ClearStatusAction;
AClearReversalsAction* ClearReversalAction;
AStatusTriggerAction* StatusTriggerAction;
*/

/* High level Components Logic Functions */
Command_array* ability_handler_process_ability(Madness_Pulse_Game* game)
{

}

/*
Command_array* ProcessNormalComponents(Madness_Pulse_Game* game,
                                         TArray<Unit*>& Targets,
                                         TArray<Ability_Component*> NormalComponents);


Command_array* ProcessStatusTrigger(Madness_Pulse_Game* game,
                                      TArray<Unit*> Targets,
                                      TArray<Ability_Component*> StatusTriggerComponents);

bool IsThereAReversalTrigger();
Command_array* ProcessReversal(Madness_Pulse_Game* game);





void AddReversalComponentToList(Madness_Pulse_Game* game, Ability* Ability,
                                TArray<Ability_Component*> ReversalTriggerComponents,
                                UReversalComponent* ReversalComponent,
                                TArray<Unit*>& Targets);


void AddTurnComponentToList(Madness_Pulse_Game* game,
    TArray<UTurnComponentBase*> TurnComponents,
                            TArray<Ability_Component*> TurnTriggerComponents,
                            TArray<Ability_Component*> StatusTriggerComponents,
                            Ability* AbilityToProcess,
                            Unit* UnitCaster,
                            TArray<Unit*> Targets);


// turn start/ends checks //

//check the turn type for any turntype specific function calls, then process turn components
Command_array* ProcessTurnType(Madness_Pulse_Game* game, Turn_Activation_Type TurnType);

//handles checking is a turn component can activate
Command_array* ProcessTurnComponents(Madness_Pulse_Game* game, Turn_Activation_Type TurnType);


//called only on first turn start
void RemovalReversalFromTurnUnit(Unit* CurrentUnitsTurn);


//TODO:
void ResetActionManager();
*/
#endif //ABILITY_HANDLER_H
