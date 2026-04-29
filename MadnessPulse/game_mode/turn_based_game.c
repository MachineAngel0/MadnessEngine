#include "turn_based_game.h"

#include "ability.h"
#include "logger.h"


void turn_based_game_init(Madness_Pulse_Game* game)
{
    // Load Player Save Data
    // Save_Game save_game = save_game_load();
    // MASSERT(save_game);

    //TODO: LOAD MISSION/ENEMY DATA
    // Get player Save
    // TArray<TSubclassOf<AUnitBase>> EnemyUnitsClass = MissionManagerComponent->ReturnEnemyUnitsFromMissionTable();


    //Unlock Fusion, if we are in the fusion mania map, also gets saved to file
    // MissionManagerComponent->UnlockFusion(CustomGameInstanceReference, SaveGameReference);

    //TODO: LOAD IN ABILITIES FOR BOTH THE PLAYER AND THE ENEMY
    //TODO: LOAD IN ENEMY AI

    //TODO: DECIDE WHO GOES FIRST
    //get who takes the first action and fill up the queue
    // TurnBasedGameState.StartingTurnInitiative = MissionManagerComponent->ReturnStartingTurnInitiative();


    //TODO: INITIALIZE THE ACTION QUEUE
    // ResetTurnQueue(true);
    //init any action components
    /*
    AttackAction = GetWorld()->SpawnActor<AAttackAnimationAction>();
    TurnEndAction = GetWorld()->SpawnActor<ATurnEndAction>();
    VictoryCheckAction = GetWorld()->SpawnActor<AVictoryCheckAction>();
    ProcessEnemyAIAction = GetWorld()->SpawnActor<AProcessEnemyAIAction>();
    TurnStartAction = GetWorld()->SpawnActor<ATurnStartAction>();
    FusionAnimation = GetWorld()->SpawnActor<AFusionAnimation>();
*/

    //TODO: MANAGING SPAWN LOCATIONS
    /* Battle Spots Initialization */
    // BattleSpotManager->GetBattleSpots();
    // BattleSpotManager->InitializeUnitsAndBattleSpots(TurnBasedGameState);


    //TODO: IDK WHAT THIS IS
    /*BINDINGS */
    // FusionComponent->OnFusion.AddUniqueDynamic(this, &ATurnBasedGameMode::CreateFusionData);


    //TODO: UI
    /* UI INITIALIZATION */
    // UIBattleComponent->InitializeBattleUI(this);

    //might do some of these at startup instead of here, and just keep them in memory
    //TODO: load ability texture icons for the ui
    //TODO: load vfx/ vfx pools
    //TODO: load audio needed


    // Start the game
    // OnTurnBasedGameModeStarted.Broadcast(this);


    //NOTE: testing code

    game->players[0] = *unit_create_default(game);
    game->players[1] = *unit_create_default(game);
    game->player_count = 2;


    game->enemies = arena_alloc(&game->arena, sizeof(Unit) * 2);
    game->enemies[0] = *unit_create_default(game);
    game->enemies[1] = *unit_create_default(game);
    game->enemy_count = 2;
    //allocate based on how many units are in the level
    game->total_unit_count = game->player_count + game->enemy_count;
    game->turn_queue = ring_queue_create(sizeof(Character_Name), game->total_unit_count);
    game->starting_turn_initiative = Turn_Initiative_Player;
    turn_based_reset_turn_queue(game);

    //TODO: init some abilities and have it display
    // game->players[0].inventory_component.ability_battle_list;
    game->players[0].inventory_component.ability_battle_list_ability[0] = get_default_heal_ability();
    game->players[0].inventory_component.ability_battle_list_ability[1] = get_default_damage_ability();
    game->players[0].inventory_component.battle_list_size = 2;
}

void turn_start(Madness_Pulse_Game* game)
{
    /*
    DEBUG("Turn Start Processing");

    //First Turn Start
    Character_Name top_of_queue_name;
    ring_queue_peek(game->turn_queue, &top_of_queue_name);
    if (game->current_units_turn != top_of_queue_name)
    {
        //update to the new units turn
        game->current_units_turn  = top_of_queue_name;
        Unit* current_unit =  madness_pulse_get_unit(game, game->current_units_turn);
        if (health_component_is_alive(&current_unit->health_component))
        {
            //refresh the actions for the turn player
            action_component_refresh_actions_by_addition(&current_unit->action_component);
        }

        //Resets special ability flags
        special_ability_flag_list_reset_flags(&current_unit->special_ability_flag_list_component);

        //TODO: add something like a ui change
        // add turn start actions to the queue
        ActionHandlerComponent->AppendActions(
            ActionManagerComponent->ProcessTurnType(TurnBasedGameState, Turn_Activation_Type_First_Start));
        TurnBasedGameState.CurrentUnitsTurn->BattleTurnInformationComponent->UpdateFirstTurnStartInfo();
        FusionComponent->ResetFirstTimeFusion();
    }

    ActionHandlerComponent->AppendActions(
        ActionManagerComponent->ProcessTurnType(TurnBasedGameState, Turn_Activation_Type_Start));

    ActionHandlerComponent->AppendActions(DeathReviveChecks());
    ActionHandlerComponent->AddAction(GetVictoryCheck());


    //TODO: can unit act check and failed action/ out of moves shower

    if (TurnBasedGameState.CurrentUnitsTurn->CharacterType == Character_Type_Player)
    {
        DEBUG("Turn Start For Player");

        CameraManagerComponent->PossessCamera(ECameraType::ECS_TurnPlayerCamera, TurnBasedGameState.CurrentUnitsTurn);
        BattleSpotManager->SetUnitToTurnDecisionLocation(TurnBasedGameState.CurrentUnitsTurn);

        //if the player cant act then have turn end trigger
        if (!CanCurrentUnitAct())
        {
            //TODO: Put in the NoActionsCommand, which will display that the turn player is out of actions and then move on
            ActionHandlerComponent->AddAction(TurnEndAction);
        }
        else
        {
            UIBattleComponent->UpdateBattleUIPlayerTurn(TurnBasedGameState.CurrentUnitsTurn);

            //TODO: UpdateFusionList
            UIBattleComponent->UpdateFusionUI(FusionComponent->ConstructFusionList(TurnBasedGameState.CurrentUnitsTurn),
                                              TurnBasedGameState.CurrentUnitsTurn);


            TurnBasedGameState.TurnPhase = ETurnPhase::ECS_AbilitySelection;
        }
    }
    else if (TurnBasedGameState.CurrentUnitsTurn->CharacterType == ECharacterType::ECS_Enemy)
    {
        DEBUG("Turn Start For Enemy");

        CameraManagerComponent->PossessCamera(ECameraType::ECS_EnemyTurnCamera, TurnBasedGameState.CurrentUnitsTurn);
        TurnBasedGameState.TurnPhase = ETurnPhase::ECS_EnemyTurn;

        ActionHandlerComponent->AddAction(ProcessEnemyAIAction);

        //ProcessEnemyAI();
    }

    ActionHandlerComponent->ExecuteNextAction();
*/
    /*
    //runs once a frame
    DEBUG("Turn Start Processing");

    //First Turn Start
    if (TurnBasedGameState.CurrentUnitsTurn != TurnBasedGameState.TurnQueue[0])
    {
        //update to the new units turn

        TurnBasedGameState.CurrentUnitsTurn = TurnBasedGameState.TurnQueue[0];
        if (TurnBasedGameState.CurrentUnitsTurn->HealthComponent->IsAlive())
        {
            TurnBasedGameState.CurrentUnitsTurn->ActionComponent->RefreshForTurnStart();
        }

        //Resets special ability flags
        TurnBasedGameState.CurrentUnitsTurn->SpecialAbilityFlagComponent->ResetFlags();

        //TODO: add something like a ui change
        // add turn start actions to the queue
        ActionHandlerComponent->AppendActions(
            ActionManagerComponent->ProcessTurnType(TurnBasedGameState, ETurnActivationType::ECS_First_Start));
        TurnBasedGameState.CurrentUnitsTurn->BattleTurnInformationComponent->UpdateFirstTurnStartInfo();
        FusionComponent->ResetFirstTimeFusion();
    }

    ActionHandlerComponent->AppendActions(
        ActionManagerComponent->ProcessTurnType(TurnBasedGameState, ETurnActivationType::ECS_Start));

    ActionHandlerComponent->AppendActions(DeathReviveChecks());
    ActionHandlerComponent->AddAction(GetVictoryCheck());


    //TODO: can unit act check and failed action/ out of moves shower


    if (TurnBasedGameState.CurrentUnitsTurn->CharacterType == Character_Type_Player)
    {
        DEBUG("Turn Start For Player");

        CameraManagerComponent->PossessCamera(ECameraType::ECS_TurnPlayerCamera, TurnBasedGameState.CurrentUnitsTurn);
        BattleSpotManager->SetUnitToTurnDecisionLocation(TurnBasedGameState.CurrentUnitsTurn);

        //if the player cant act then have turn end trigger
        if (!CanCurrentUnitAct())
        {
            //TODO: Put in the NoActionsCommand, which will display that the turn player is out of actions and then move on
            ActionHandlerComponent->AddAction(TurnEndAction);
        }
        else
        {
            UIBattleComponent->UpdateBattleUIPlayerTurn(TurnBasedGameState.CurrentUnitsTurn);

            //TODO: UpdateFusionList
            UIBattleComponent->UpdateFusionUI(FusionComponent->ConstructFusionList(TurnBasedGameState.CurrentUnitsTurn),
                                              TurnBasedGameState.CurrentUnitsTurn);


            TurnBasedGameState.TurnPhase = ETurnPhase::ECS_AbilitySelection;
        }
    }
    else if (TurnBasedGameState.CurrentUnitsTurn->CharacterType == ECharacterType::ECS_Enemy)
    {
        DEBUG("Turn Start For Enemy");

        CameraManagerComponent->PossessCamera(ECameraType::ECS_EnemyTurnCamera, TurnBasedGameState.CurrentUnitsTurn);
        TurnBasedGameState.TurnPhase = ETurnPhase::ECS_EnemyTurn;

        ActionHandlerComponent->AddAction(ProcessEnemyAIAction);

        //ProcessEnemyAI();
    }

    ActionHandlerComponent->ExecuteNextAction();
    */
}

void turn_end(Madness_Pulse_Game* game)
{
    DEBUG("Turn End Called");

    game->turn_phase = Turn_Phase_Turn_End;

    // we can call this here, as it doesn't affect who the current unit is, it only affects who is next
    //this only changes the queue up, it doesn't do actually change the current units turn, thats turn start responsibility
    // TODO: were not checking if a unit is dead, which i might change, and just altogether skip dead units
    // rn we want turn start/end actions to go off on dead units, status effects should persist
    if (!can_current_unit_act(game))
    {
        // pop from the front of the queue, there is guaranteed to be someone there
        u32 debug_unit_pop;
        ring_dequeue(game->turn_queue, &debug_unit_pop);
        DEBUG("Unit Popped From the Queue: %d", debug_unit_pop);

        //
        /*
        BattleSpotManager->SetUnitToBattleSpot(TurnBasedGameState.CurrentUnitsTurn);
        ActionHandlerComponent->AppendActions(
            ActionManagerComponent->ProcessTurnType(TurnBasedGameState, Turn_Activation_Type_Final_End));
        TurnBasedGameState.CurrentUnitsTurn->BattleTurnInformationComponent->UpdateTurnEndInfo();
*/
        if (ring_queue_is_empty(game->turn_queue))
        {
            turn_based_reset_turn_queue(game);
        }
    }


    //hides any displayed abilities
    // UIBattleComponent->HideAbility();


    // ActionHandlerComponent->AppendActions(ActionManagerComponent->ProcessTurnType(TurnBasedGameState, ETurnActivationType::ECS_End));

    /*
    ActionHandlerComponent->AppendActions(DeathReviveChecks());
    ActionHandlerComponent->AddAction(GetVictoryCheck());
    //then we start the next turn after everything resolves
    ActionHandlerComponent->AddAction(TurnStartAction);
    ActionHandlerComponent->ExecuteNextAction();
    */
}


void turn_based_reset_turn_queue(Madness_Pulse_Game* game)
{
    //determines who goes into the queue first
    switch (game->starting_turn_initiative)
    {
    case Turn_Initiative_Player:
        for (u32 i = 0; i < game->player_count; i++)
        {
            ring_enqueue(game->turn_queue, &game->players[i]);
        }
        for (u32 i = 0; i < game->enemy_count; i++)
        {
            ring_enqueue(game->turn_queue, &game->enemies[i]);
        }
        break;
    case Turn_Initiative_Enemy:
        for (u32 i = 0; i < game->enemy_count; i++)
        {
            ring_enqueue(game->turn_queue, &game->enemies[i]);
        }
        for (u32 i = 0; i < game->player_count; i++)
        {
            ring_enqueue(game->turn_queue, &game->players[i]);
        }
        break;
    }
}

bool can_current_unit_act(Madness_Pulse_Game* game)
{
    //can act if the unit if the unit has actions and is alive
    Unit* unit = madness_pulse_get_unit(game, game->current_units_turn);

    return action_component_is_action_available(&unit->action_component) && health_component_is_alive(&unit->health_component);
}

Unit* madness_pulse_get_unit(Madness_Pulse_Game* game, Character_Name name)
{

    for (u32 i = 0; i < game->player_count; i++)
    {
        if ((game->players[i].name = name))
        {
            return &game->players[i];
        };
    }
    for (u32 i = 0; i < game->enemy_count; i++)
    {
        if ((game->enemies[i].name = name))
        {
            return &game->enemies[i];
        };
    }

}
