#include "turn_based_game.h"

#include "ability.h"
#include "ability_handler.h"
#include "logger.h"
#include "command/command.h"


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

    game->targeting_handler = targeting_handler_init(game);
    game->ability_handler = ability_handler_init(game);
    game->command_handler = command_handler_init(game);
    game->ability_registry = ability_registry_init(game);


    game->players[0] = *unit_create(game, Character_Name_Madness_Progenitor);
    game->players[1] =  *unit_create(game, Character_Name_Madness_ButterFly);
    game->players[2] =  *unit_create(game, Character_Name_Madness_Wolf);
    game->players[3] =  *unit_create(game, Character_Name_Madness_Envoy);
    game->player_character_names[0] = game->players[0].name;
    game->player_character_names[1] = game->players[1].name;
    game->player_character_names[2] = game->players[2].name;
    game->player_character_names[3] = game->players[3].name;
    game->player_count = 4;



    game->enemies = allocator_alloc(&game->allocator, sizeof(Unit) * 2);
    game->enemies[0] = *unit_create(game, Character_Name_Red_Jester);
    game->enemies[1] = *unit_create(game, Character_Name_Clown);
    game->enemies[2] = *unit_create(game, Character_Name_Puppet);
    game->enemies[3] = *unit_create(game, Character_Name_Doll);
    game->enemy_character_names[0] = game->enemies[0].name;
    game->enemy_character_names[1] = game->enemies[1].name;
    game->enemy_character_names[2] = game->enemies[2].name;
    game->enemy_character_names[3] = game->enemies[3].name;
    game->enemy_count = 4;

    //list of all the units
    game->units_count = game->player_count + game->enemy_count;
    game->units = allocator_alloc(&game->allocator, sizeof(Unit*) * game->units_count);
    for (int i = 0; i < game->player_count; ++i)
    {
        game->units[i] = game->players[i];
    }
    for (int i = game->player_count; i < game->player_count + game->enemy_count; ++i)
    {
        game->units[i] = game->enemies[i];
    }

    game->turn_queue = ring_queue_create(sizeof(Character_Name), game->units_count);
    game->starting_turn_initiative = Turn_Initiative_Player;
    game->current_units_turn = Character_Name_None; // will cause turn start to pick the proper character
    turn_based_reset_turn_queue(game);


    //TODO: init some abilities and have it display
    // game->players[0].inventory_component.ability_battle_list;
    inventory_component_add_to_starting_battle_list(&game->players[0].inventory_component, Ability_Name_DEBUG_DAMAGE);
    inventory_component_add_to_starting_battle_list(&game->players[0].inventory_component, Ability_Name_DEBUG_HEAL);

    //
    create_starting_abilties(&game->players[0].inventory_component, game->ability_registry);



    turn_start(game);
}


void turn_start(Madness_Pulse_Game* game)
{
    DEBUG("Turn Start Processing");

    //First Turn Start
    Character_Name top_of_queue_name;
    ring_queue_peek(game->turn_queue, &top_of_queue_name);
    if (game->current_units_turn != top_of_queue_name)
    {
        //update to the new units turn
        game->current_units_turn = top_of_queue_name;
        Unit* current_unit = madness_pulse_get_unit(game, game->current_units_turn);
        if (health_component_is_alive(&current_unit->health_component))
        {
            //refresh the actions for the turn player
            action_component_refresh_actions_by_addition(&current_unit->action_component);
        }

        //Resets special ability flags
        special_ability_flag_list_reset_flags(&current_unit->special_ability_flag_list_component);

        //TODO: add something like a ui change
        // add turn start actions to the queue

        // game->command_handler;
        // action_handler_add_multiple_action(game->command_handler, Command_array* command_array);
        // ActionHandlerComponent->AppendActions(
        // ActionManagerComponent->ProcessTurnType(game, Turn_Activation_Type_First_Start));

        // TurnBasedGameState.CurrentUnitsTurn->BattleTurnInformationComponent->UpdateFirstTurnStartInfo();
        // FusionComponent->ResetFirstTimeFusion();
    }

    // action_handler_add_multiple_actions(game->command_handler, command_array);
    // ActionHandlerComponent->AppendActions(
    // ActionManagerComponent->ProcessTurnType(game, Turn_Activation_Type_Start));

    // ActionHandlerComponent->AppendActions(DeathReviveChecks());
    // ActionHandlerComponent->AddAction(GetVictoryCheck());


    //TODO: can unit act check and failed action/ out of moves shower

    if (madness_pulse_get_unit(game, game->current_units_turn)->character_type == Character_Type_Player)
    {
        DEBUG("Turn Start For Player");

        // CameraManagerComponent->PossessCamera(ECameraType::ECS_TurnPlayerCamera, game->current_units_turn);
        // BattleSpotManager->SetUnitToTurnDecisionLocation(game->current_units_turn);

        //if the player cant act then have turn end trigger
        if (!can_current_unit_act(game))
        {
            //TODO: Put in the NoActionsCommand, which will display that the turn player is out of actions and then move on
            // ActionHandlerComponent->AddAction(TurnEndAction);
        }
        else
        {
            // UIBattleComponent->UpdateBattleUIPlayerTurn(TurnBasedGameState.CurrentUnitsTurn);

            //TODO: UpdateFusionList
            // UIBattleComponent->UpdateFusionUI(FusionComponent->ConstructFusionList(TurnBasedGameState.CurrentUnitsTurn),
            // TurnBasedGameState.CurrentUnitsTurn);
            game->turn_phase = Turn_Phase_Ability_Selection;
        }
    }
    if (madness_pulse_get_unit(game, game->current_units_turn)->character_type == Character_Type_Enemy)
    {
        DEBUG("Turn Start For Enemy");

        // CameraManagerComponent->PossessCamera(ECameraType::ECS_EnemyTurnCamera, TurnBasedGameState.CurrentUnitsTurn);
        game->turn_phase = Turn_Phase_Enemy_Turn;

        // ActionHandlerComponent->AddAction(ProcessEnemyAIAction);

        //ProcessEnemyAI();
    }

    // ActionHandlerComponent->ExecuteNextAction();
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

void turn_update(Madness_Pulse_Game* game)
{
    Unit* unit = madness_pulse_get_unit(game, game->current_units_turn);

    for (int i = 0; i < unit->inventory_component.battle_list_size; ++i)
    {
        String ability_name = ability_registry_get_ability_info(game->ability_registry, unit->inventory_component.battle_list_starting[i]).ability_name;
        string_println(&ability_name);
    }



    if (input_is_mouse_button_released_unique(game->input_system, KEY_RIGHT))
    {
        targeting_handler_move_unit_targeting(game, Targeting_Direction_Right);
    }
    else if (input_is_mouse_button_released_unique(game->input_system, KEY_LEFT))
    {
        targeting_handler_move_unit_targeting(game, Targeting_Direction_Left);
    }
}


void turn_based_reset_turn_queue(Madness_Pulse_Game* game)
{
    //determines who goes into the queue first
    switch (game->starting_turn_initiative)
    {
    case Turn_Initiative_Player:
        for (u32 i = 0; i < game->player_count; i++)
        {
            ring_enqueue(game->turn_queue, &game->player_character_names[i]);
        }
        for (u32 i = 0; i < game->enemy_count; i++)
        {
            ring_enqueue(game->turn_queue, &game->enemy_character_names[i]);
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

    return action_component_is_action_available(&unit->action_component) && health_component_is_alive(
        &unit->health_component);
}

Unit* madness_pulse_get_unit(Madness_Pulse_Game* game, Character_Name name)
{
    for (int i = 0; i < game->units_count; ++i)
    {
        if (game->units[i].name == name)
        {
            {
                return &game->units[i];
            }
        }
    }
    MASSERT(false);
    return NULL;
}
