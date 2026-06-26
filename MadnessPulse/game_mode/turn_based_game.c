#include "turn_based_game.h"

#include "ability.h"
#include "ability_handler.h"
#include "logger.h"
#include "ui_madness.h"
#include "unit_base.h"
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


    game->targeting_handler = targeting_handler_init(game);
    game->ability_handler = ability_handler_init(game);
    game->command_handler = command_handler_init(game);
    game->ability_registry = ability_registry_init(game);


    //NOTE: testing code
    Game_Level_Data level_data = get_level_data(Level_Name_Sandbox);
    // level_data.enemy_units;
    // level_data.enemy_count;
    // level_data.starting_turn_initiative;

    for (u32 i = 0; i < MAX_PLAYER_UNIT_COUNT; ++i)
    {
        game->save_game->player_save_info[i];

        //add abilites to players thang
        //TODO: when you wake up, should the inventory/abilites available be seperated from the player, then loaded into the units inventory
        // rn im thinking yes, this also lets us validate the inventory if there is ever a save corruption on the inventory
    }


    //TODO: replace with a proper free list allocator, also this is more than enough for testing
    u32 temp_unit_max_count = 20;
    game->units = allocator_alloc(&game->allocator, sizeof(Unit*) * temp_unit_max_count);
    game->unit_handles = allocator_alloc(&game->allocator, sizeof(Unit_Handle) * temp_unit_max_count);
    game->players = allocator_alloc(&game->allocator, sizeof(Unit_Handle) * temp_unit_max_count);
    game->enemies = allocator_alloc(&game->allocator, sizeof(Unit_Handle) * temp_unit_max_count);
    //load player
    unit_create(game, Character_Name_Madness_Progenitor);
    unit_create(game, Character_Name_Madness_ButterFly);
    unit_create(game, Character_Name_Madness_Wolf);
    unit_create(game, Character_Name_Madness_Envoy);

    //load enemies
    unit_create(game, Character_Name_Red_Jester);
    unit_create(game, Character_Name_Clown);
    unit_create(game, Character_Name_Puppet);
    unit_create(game, Character_Name_Doll);

    //puts units into their proper enemy or player list
    game_resolve_unit_character_list_types(game);


    game->turn_queue = ring_queue_create(sizeof(Unit_Handle), game->units_count);
    game->starting_turn_initiative = Turn_Initiative_Player;
    // will cause turn start to pick the proper character
    game->current_units_turn = (Unit_Handle){INT_MAX, Character_Name_None};
    turn_based_reset_turn_queue(game);


    //TODO: init some abilities and have it display
    // game->players[0].inventory_component.ability_battle_list;
    game->units_count = game->player_count + game->enemy_count;
    for (u32 i = 0; i < game->units_count; ++i)
    {
        inventory_component_copy_to_battle_inventory(&game->units[i]->battle_inventory_component,
                                                     &game->units[i]->inventory_component);

        //TODO: remove these later
        battle_inventory_add_debug_ability(&game->units[i]->battle_inventory_component, Ability_Name_DEBUG_DAMAGE);
        battle_inventory_add_debug_ability(&game->units[i]->battle_inventory_component, Ability_Name_DEBUG_HEAL);
    }

    //create the abilities
    for (u32 i = 0; i < game->units_count; ++i)
    {
        create_starting_abilties(&game->units[i]->inventory_component, game->ability_registry);
    }

    game->turn_phase = Turn_Phase_Turn_Start;
}


void turn_start(Madness_Pulse_Game* game)
{
    DEBUG("Turn Start Processing");

    //First Turn Start
    Unit_Handle top_of_queue_name;
    ring_queue_peek(game->turn_queue, &top_of_queue_name);
    if (game->current_units_turn.index != top_of_queue_name.index)
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

    game->turn_phase = Turn_Phase_Turn_Start;
}

void turn_update(Madness_Pulse_Game* game)
{
    switch (game->turn_phase)
    {
    case Turn_Phase_None:
        turn_based_game_init(game);
        break;
    case Turn_Phase_Turn_Start:
        turn_start(game);
        break;
    case Turn_Phase_Ability_Selection:

        //show the player their ui and handle any inputs
        Unit* unit = madness_pulse_get_unit(game, game->current_units_turn);


        madness_ui_set_window_pos(game->madness_ui, 100, 400);
        madness_ui_set_window_size(game->madness_ui, 500, 400);
        madness_ui_window_begin(game->madness_ui, STRING("Ability Select"));
        {
            Unit* cur_unit = madness_pulse_get_unit(game, game->current_units_turn);
            madness_ui_c_string(game->madness_ui, Character_Name_enum_string[cur_unit->name]);
            madness_scroll_box_begin(game->madness_ui, STRING("Ability Scroll list"));
            {
                for (int i = 0; i < cur_unit->battle_inventory_component.battle_list_dynamic->num_items; ++i)
                {
                    Ability_Info ability_info = ability_registry_get_ability_info(game->ability_registry,
                        dynamic_array_get(cur_unit->battle_inventory_component.battle_list_dynamic, Ability_Name, i));

                    if (madness_ui_button(game->madness_ui,
                                          STRING_STRLEN(Ability_Name_enum_string[ability_info.ability_name])))
                    {
                        FATAL("A THINGS SELECTED");
                        game->currently_selected_ability = ability_info.ability_name;
                        targeting_handler_create_targeting_info(game);
                        game->turn_phase = Turn_Phase_Target_Select;
                    }
                    madness_ui_same_line(game->madness_ui);
                    float temp_madness_counter = 0;
                    madness_ui_float(game->madness_ui, STRING("Count:"), &temp_madness_counter, 0);
                }
            }
            madness_scroll_box_end(game->madness_ui);
        }
        madness_ui_window_end(game->madness_ui);

        break;
    case Turn_Phase_Target_Select:
        madness_ui_window_begin(game->madness_ui, STRING("Target Select"));
        {
            Ability_Info ability_info = ability_registry_get_ability_info(
                game->ability_registry, game->currently_selected_ability);

            //we want to grab the target lock information, from the target handler
            String pretext = STRING("Selected Target: ");

            String* selected_target_string = string_concat(
                &pretext, &STRING_STRLEN(
                    madness_pulse_get_unit_name(game,game->targeting_handler->current_lock_on_target)),
                &game->frame_allocator);

            madness_ui_string(game->madness_ui, *selected_target_string);

            madness_ui_string(game->madness_ui, STRING("Available Targets: "));

            for (int i = 0; i < game->targeting_handler->targets_available_array->num_items; ++i)
            {
                if (madness_ui_button(game->madness_ui,
                                      STRING_STRLEN(madness_pulse_get_unit_name(game, dynamic_array_get(
                                          game->targeting_handler->targets_available_array, Unit_Handle, i)))))
                {
                    //we have selected our target
                    game->targeting_handler->current_lock_on_target = dynamic_array_get(
                        game->targeting_handler->targets_available_array, Unit_Handle, i);

                    game->turn_phase = Turn_Phase_Ability_Process;
                }
            }
        }
        madness_ui_window_end(game->madness_ui);

        //TODO:
        /*
        if (input_is_mouse_button_released_unique(game->input_system, KEY_RIGHT))
        {
            targeting_handler_move_unit_targeting(game, Targeting_Direction_Right);
        }
        if (input_is_mouse_button_released_unique(game->input_system, KEY_LEFT))
        {
            targeting_handler_move_unit_targeting(game, Targeting_Direction_Left);
        }
        if (input_is_mouse_button_released_unique(game->input_system, KEY_C))
        {
            //process our ability
        }*/
        break;
    case Turn_Phase_Ability_Process:
        ability_handler_process_ability(game);
        // TODO: this should go to the queue process first, then end turn from the queue
        game->turn_phase = Turn_Phase_Turn_End;
        break;
    case Turn_Phase_Queue_Process:
        break;
    case Turn_Phase_Turn_End:
        turn_end(game);
        break;
    case Turn_Phase_Enemy_Turn:
        break;
    case Turn_Phase_Dialogue:
        break;
    case Turn_Phase_Event:
        break;
    case Turn_Phase_Battle_Over:
        break;
    }


    if (game->turn_phase != Turn_Phase_None)
    {
        turn_based_game_display_debug_info(game);

        //display character info
        madness_ui_set_window_pos(game->madness_ui, 1400, 50);
        madness_ui_set_window_size(game->madness_ui, 400, 800);
        madness_ui_window_begin(game->madness_ui, STRING("Units Info"));
        {
            madness_ui_string(game->madness_ui, STRING("PLAYER"));
            for (u32 i = 0; i < game->player_count; i++)
            {
                Unit* unit = madness_pulse_get_unit(game, game->players[i]);
                madness_ui_c_string(game->madness_ui, Character_Name_enum_string[unit->name]);

                //health
                madness_ui_float(game->madness_ui, STRING("Health"), &unit->health_component.current_health, 0);
                madness_ui_same_line(game->madness_ui);
                madness_ui_string(game->madness_ui, STRING("\\"));
                madness_ui_same_line(game->madness_ui);
                madness_ui_float(game->madness_ui, STRING(""), &unit->health_component.max_health, 0);

                //mp
                madness_ui_float(game->madness_ui, STRING("MP"), &unit->mp_component.current_mp, 0);
                madness_ui_same_line(game->madness_ui);
                madness_ui_string(game->madness_ui, STRING("\\"));
                madness_ui_same_line(game->madness_ui);
                madness_ui_float(game->madness_ui, STRING(""), &unit->mp_component.max_mp, 0);
            }
            madness_ui_padding(game->madness_ui, "player to enemy padding");
            madness_ui_string(game->madness_ui, STRING("ENEMIES"));
            for (u32 i = 0; i < game->enemy_count; i++)
            {
                Unit* unit = madness_pulse_get_unit(game, game->enemies[i]);
                madness_ui_c_string(game->madness_ui, Character_Name_enum_string[unit->name]);

                madness_ui_float(game->madness_ui, STRING("Health"), &unit->health_component.current_health, 0);
                madness_ui_same_line(game->madness_ui);
                madness_ui_string(game->madness_ui, STRING("\\"));
                madness_ui_same_line(game->madness_ui);
                madness_ui_float(game->madness_ui, STRING(""), &unit->health_component.max_health, 0);
            }
        }
        madness_ui_window_end(game->madness_ui);
    }

    //TODO: seperate the ui and put it here
}


void turn_based_reset_turn_queue(Madness_Pulse_Game* game)
{
    //fixes the list for any minion type units
    game_resolve_unit_character_list_types(game);


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

void game_resolve_unit_character_list_types(Madness_Pulse_Game* game)
{
    //puts units into the proper player or enemy lists
    game->unit_handles_count = game->units_count;
    game->player_count = 0;
    game->enemy_count = 0;


    for (int i = 0; i < game->units_count; ++i)
    {
        Unit* current_unit = game->units[i];;
        game->unit_handles[i] = (Unit_Handle){i, current_unit->name};


        switch (current_unit->character_type)
        {
        case Character_Type_Player:
            game->players[game->player_count++] = (Unit_Handle){i, current_unit->name};
            break;
        case Character_Type_Enemy:
            game->enemies[game->enemy_count++] = (Unit_Handle){i, current_unit->name};
            break;
        }
    }


    //idk the enemy unit count at start up since its different every map
    Unit* enemies;
    u8 enemy_count;
}

bool can_current_unit_act(Madness_Pulse_Game* game)
{
    //can act if the unit if the unit has actions and is alive
    Unit* unit = madness_pulse_get_unit(game, game->current_units_turn);

    return action_component_is_action_available(&unit->action_component) && health_component_is_alive(
        &unit->health_component);
}

Unit* madness_pulse_get_unit(const Madness_Pulse_Game* game, const Unit_Handle handle)
{
    return game->units[handle.index];
}

const char* madness_pulse_get_unit_name(const Madness_Pulse_Game* game, Unit_Handle handle)
{
    return Character_Name_enum_string[madness_pulse_get_unit(game, handle)->name];
}


bool character_handle_compare(const Unit_Handle a, const Unit_Handle b)
{
    if (a.name != b.name) return false;
    if (a.index != b.index) return false;

    //TODO:
    // if (a.generation != b.generation) return false;

    return true;
}


void turn_based_game_display_debug_info(Madness_Pulse_Game* game)
{
    madness_ui_set_window_pos(game->madness_ui, 0, 0);
    madness_ui_window_begin(game->madness_ui, STRING("GAME DEBUG INFO"));
    {
        madness_ui_string(game->madness_ui, STRING_STRLEN(Turn_Phase_enum_string[game->turn_phase]));

        for (int i = 0; i < game->units_count; ++i)
        {
            madness_ui_string(game->madness_ui, STRING_STRLEN(Character_Name_enum_string[game->units[i]->name]));
        }
    }
    madness_ui_window_end(game->madness_ui);
}
