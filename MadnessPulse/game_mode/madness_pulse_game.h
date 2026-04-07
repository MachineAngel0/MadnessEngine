#ifndef MADNESS_PULSE_GAME_H
#define MADNESS_PULSE_GAME_H

#include "ability.h"
#include "game_structs.h"

//when we first load the application
Madness_Pulse_Game* Madness_Pulse_Game_Init(Memory_System* memory_system)
{
    Madness_Pulse_Game* game = memory_system_alloc(memory_system, sizeof(Madness_Pulse_Game), MEMORY_SUBSYSTEM_GAME);
    game->game_state = Game_State_Enum_None;

    u64 game_memory_size = GB(0.5);
    void* game_memory = memory_system_alloc(memory_system, game_memory_size, MEMORY_SUBSYSTEM_GAME);
    arena_init(&game->arena, game_memory, game_memory_size, NULL);


    game->game_state = Game_State_Enum_Main_Menu;
    return game;
}


void Main_Menu_Run(Madness_Pulse_Game* game)
{
    //run the ui
    Madness_UI* ui = game->madness_ui;
    madness_ui_begin_layout(ui, "Main Menu", (vec2){50, 50}, (vec2){50, 50});


    if (madness_button_text(ui, "Load Game", STRING("Load Game")))
    {
        //set new state for the load menu and load in menu metadata
    }
    if (madness_button_text(ui, "Settings", STRING("Settings")))
    {
    }
    if (madness_button_text(ui, "Quit", STRING("Quit")))
    {
    }
}


void Turn_Based_Game_Init(Madness_Pulse_Game* game)
{
    // Game_State* game_state = memory_system_alloc(memory_system, sizeof(Game_State));
    // Game_State* game_state = memory_system_alloc(memory_system, sizeof(Game_State));


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

    // Start the game
    // OnTurnBasedGameModeStarted.Broadcast(this);
}

void Turn_Start(Madness_Pulse_Game* game_state)
{
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
}

void Turn_End()
{
}


#endif //MADNESS_PULSE_GAME_H
