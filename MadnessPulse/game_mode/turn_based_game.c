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
    game->madness_ai = madness_ai_init(game);


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
    game->player_units = allocator_alloc(&game->allocator, sizeof(Unit*) * temp_unit_max_count);
    game->enemy_units = allocator_alloc(&game->allocator, sizeof(Unit*) * temp_unit_max_count);

    game->unit_names = allocator_alloc(&game->allocator, sizeof(Character_Name) * temp_unit_max_count);
    game->player_names = allocator_alloc(&game->allocator, sizeof(Character_Name) * temp_unit_max_count);
    game->enemy_names = allocator_alloc(&game->allocator, sizeof(Character_Name) * temp_unit_max_count);
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

    //init AI
    madness_ai_instantiate_enemy(game, game->madness_ai);


    game->turn_queue = ring_queue_create(sizeof(Character_Name), game->units_count);
    game->starting_turn_initiative = Turn_Initiative_Player;
    // will cause turn start to pick the proper character
    game->current_units_turn = (Character_Name){Character_Name_Invalid};
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
        battle_inventory_add_debug_ability(&game->units[i]->battle_inventory_component,
                                           Ability_Name_DEBUG_DAMAGE_SELF_AND_ENEMY);
        battle_inventory_add_debug_ability(&game->units[i]->battle_inventory_component, Ability_Name_DEBUG_MADNESS);
        battle_inventory_add_debug_ability(&game->units[i]->battle_inventory_component,
                                           Ability_Name_DEBUG_INSTAKILL_SELF);
        battle_inventory_add_debug_ability(&game->units[i]->battle_inventory_component,
                                           Ability_Name_DEBUG_INSTAKILL_TARGET);
        battle_inventory_add_debug_ability(&game->units[i]->battle_inventory_component,
                                           Ability_Name_DEBUG_INSTAKILL_PLAYERS);
        battle_inventory_add_debug_ability(&game->units[i]->battle_inventory_component,
                                           Ability_Name_DEBUG_INSTAKILL_ENEMYS);
        battle_inventory_add_debug_ability(&game->units[i]->battle_inventory_component,
                                           Ability_Name_DEBUG_INSTAKILL_ALL);
        battle_inventory_add_debug_ability(&game->units[i]->battle_inventory_component,
                                           Ability_Name_DEBUG_INSTAKILL_ALL);
        battle_inventory_add_debug_ability(&game->units[i]->battle_inventory_component,
                                           Ability_Name_DEBUG_PASS_ALL_PLAYER_UNITS_TURNS);
        battle_inventory_add_debug_ability(&game->units[i]->battle_inventory_component,
                                           Ability_Name_DEBUG_GIVE_MYSELF_ACTIONS);
    }

    //create the abilities
    for (u32 i = 0; i < game->units_count; ++i)
    {
        create_starting_abilties(&game->units[i]->battle_inventory_component, game->ability_registry);
    }

    game->turn_phase = Turn_Phase_Turn_Start;
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
    if (!can_current_unit_act(game))
    {
        //TODO: Put in the NoActionsCommand, which will display that the turn player is out of actions and then move on
        // ActionHandlerComponent->AddAction(TurnEndAction);
        game->turn_phase = Turn_Phase_Turn_End;
    }
    else
    {
        if (madness_pulse_get_unit(game, game->current_units_turn)->character_type == Character_Type_Player)
        {
            DEBUG("Turn Start For Player");

            // CameraManagerComponent->PossessCamera(ECameraType::ECS_TurnPlayerCamera, game->current_units_turn);
            // BattleSpotManager->SetUnitToTurnDecisionLocation(game->current_units_turn);

            //if the player cant act then have turn end trigger


            // UIBattleComponent->UpdateBattleUIPlayerTurn(TurnBasedGameState.CurrentUnitsTurn);

            //TODO: UpdateFusionList
            // UIBattleComponent->UpdateFusionUI(FusionComponent->ConstructFusionList(TurnBasedGameState.CurrentUnitsTurn),
            // TurnBasedGameState.CurrentUnitsTurn);
            game->turn_phase = Turn_Phase_Ability_Selection;
        }
        if (madness_pulse_get_unit(game, game->current_units_turn)->character_type == Character_Type_Enemy)
        {
            DEBUG("Turn Start For Enemy");

            // CameraManagerComponent->PossessCamera(ECameraType::ECS_EnemyTurnCamera, TurnBasedGameState.CurrentUnitsTurn);
            // game->turn_phase = Turn_Phase_Enemy_Turn;

            //TODO: temp, might move logic calculations to a different frame


            madness_ai_take_turn(game);
            game->turn_phase = Turn_Phase_Enemy_Show_Decision_To_Player;

            // ActionHandlerComponent->AddAction(ProcessEnemyAIAction);

            //ProcessEnemyAI();
        }
    }

    // ActionHandlerComponent->ExecuteNextAction();
}

void turn_end(Madness_Pulse_Game* game)
{
    DEBUG("Turn End Called");


    // we can call this here, as it doesn't affect who the current unit is, it only affects who is next
    //this only changes the queue up, it doesn't do actually change the current units turn, thats turn start responsibility
    // TODO: were not checking if a unit is dead, which i might change, and just altogether skip dead units
    // rn we want turn start/end actions to go off on dead units, status effects should persist
    if (!can_current_unit_act(game))
    {
        // pop from the front of the queue, there is guaranteed to be someone there
        Character_Name debug_unit_pop = {0};
        ring_dequeue(game->turn_queue, &debug_unit_pop);
        DEBUG("Unit Popped From the Queue: %s", Character_Name_enum_string[debug_unit_pop]);

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

        madness_ui_set_window_pos(100, 100);
        madness_ui_set_window_size(500, 400);
        madness_ui_window_begin(STRING("Ability Select"));
        {
            madness_ui_c_string(Character_Name_enum_string[unit->name]);
            madness_ui_s32(STRING("Actions Left:"), &unit->action_component.actions_available, 0);
            madness_ui_string(STRING("Overflow Bar"));


            u32 overflow_val = battle_inventory_calculate_value_from_usage(
                &unit->battle_inventory_component,
                game->ability_registry);

            madness_ui_progress_bar(STRING("Progress bar"), overflow_val, unit->battle_inventory_component.overflow_threshold);


            madness_scroll_box_begin(STRING("Ability Scroll list"));
            {
                for (u32 i = 0; i < unit->battle_inventory_component.battle_list->num_items; ++i)
                {
                    Ability_Name name =
                        dynamic_array_get(unit->battle_inventory_component.battle_list, Ability_Name, i);
                    Ability_Info ability_info = ability_registry_get_ability_info(game->ability_registry, name);
                    u16 ability_count = dynamic_array_get(unit->battle_inventory_component.ability_count, u16, i);
                    u16 overflow_usage_count = dynamic_array_get(unit->battle_inventory_component.overflow_usage_count,
                                                                 u16, i);

                    if (madness_ui_button(STRING_STRLEN(Ability_Name_enum_string[name])))
                    {
                        FATAL("A THINGS SELECTED");
                        game->currently_selected_ability_by_player = ability_info.ability_name;
                        targeting_handler_create_targeting_info(game, game->currently_selected_ability_by_player);
                        game->turn_phase = Turn_Phase_Target_Select;
                    }
                    madness_ui_same_line();
                    madness_ui_u16(STRING("Count:"), &ability_count, 0);
                    madness_ui_same_line();


                    String* usage_id = string_concat(&(STRING("Usage")),
                                                     &STRING_STRLEN(c_string_from_int(i,&game->frame_allocator)),
                                                     &game->frame_allocator);
                    madness_ui_slider_arrow_u16(*usage_id, &overflow_usage_count, 0, ability_count);


                    //TODO: TEMP: display ability text
                    Ability* ability = ability_registry_get_ability(game->ability_registry, name);
                    //this function also handles the ui function call
                    ability_text_table(game, ability);


                    //update the usage count
                    dynamic_array_set(unit->battle_inventory_component.overflow_usage_count, &overflow_usage_count, i);
                }
            }
            madness_scroll_box_end();
        }
        madness_ui_window_end();

        break;
    case Turn_Phase_Target_Select:
        madness_ui_window_begin(STRING("Target Select"));
        {
            Ability_Info ability_info = ability_registry_get_ability_info(
                game->ability_registry, game->currently_selected_ability_by_player);

            //we want to grab the target lock information, from the target handler
            String pretext = STRING("Selected Target: ");

            String* selected_target_string = string_concat(
                &pretext, &STRING_STRLEN(
                    madness_pulse_get_unit_name(game, game->targeting_handler->current_lock_on_target)),
                &game->frame_allocator);


            madness_ui_string(*selected_target_string);

            madness_ui_string(STRING("Available Targets: "));

            //temp until we reimplement the ui system
            for (int i = 0; i < game->targeting_handler->targets_available_array->num_items; ++i)
            {
                if (madness_ui_button(STRING_STRLEN(madness_pulse_get_unit_name(game, dynamic_array_get(
                    game->targeting_handler->targets_available_array, Character_Name, i)))))
                {
                    //we have selected our target
                    game->targeting_handler->current_lock_on_target = dynamic_array_get(
                        game->targeting_handler->targets_available_array, Character_Name, i);

                    game->targeting_handler->targeting_count = i;

                    game->turn_phase = Turn_Phase_Ability_Process;
                }
            }
        }
        madness_ui_window_end();

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
        ability_handler_process_ability(game, game->currently_selected_ability_by_player,
                                        target_handler_return_attack_targets(game->targeting_handler, game));
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
    case Turn_Phase_Enemy_Show_Decision_To_Player:
        //TODO: wait for player input
        for (u32 i = 0; i < game->madness_ai->ai_decision->num_items; ++i)
        {
            Madness_AI_Decision* decision =
                dynamic_array_get_ptr(game->madness_ai->ai_decision, Madness_AI_Decision, i);

            madness_ui_string(STRING_STRLEN(Ability_Name_enum_string[decision->ability_info->ability_name]));
        }
        game->turn_phase = Turn_Phase_Enemy_Execute_Abilties;

        break;
    case Turn_Phase_Enemy_Execute_Abilties:

        /* TODO: at some point have this operate on the main state of the game, while the copy state is for the ui
        for (u32 i = 0; i < game->madness_ai->ai_decision->num_items; ++i)
        {
            Madness_AI_Decision* decision =
                dynamic_array_get_ptr(game->madness_ai->ai_decision, Madness_AI_Decision, i);

            Array* ai_targets = dynamic_array_copy_fixed_size(decision->chosen_targets, &game->frame_allocator);
            ability_handler_process_ability(game, decision->ability_info->ability_name, ai_targets);
        }*/
        game->turn_phase = Turn_Phase_Turn_End;


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
        //display character info
        madness_ui_set_window_pos(1400, 50);
        madness_ui_set_window_size(400, 800);
        madness_ui_window_begin(STRING("Units Info"));
        {
            madness_ui_string(STRING("Characters Turn"));
            madness_ui_same_line();
            madness_ui_c_string(Character_Name_enum_string[game->current_units_turn]);

            madness_ui_string(STRING("Turn Phase:"));
            madness_ui_same_line();
            madness_ui_string(STRING_STRLEN(Turn_Phase_enum_string[game->turn_phase]));


            madness_ui_string(STRING("PLAYER"));
            for (u32 i = 0; i < game->player_count; i++)
            {
                Unit* unit = madness_pulse_get_unit(game, game->player_names[i]);
                madness_ui_c_string(Character_Name_enum_string[unit->name]);

                //health
                madness_ui_float(STRING("Health"), &unit->health_component.current_health, 0);
                madness_ui_same_line();
                madness_ui_string(STRING("\\"));
                madness_ui_same_line();
                madness_ui_float(STRING(""), &unit->health_component.max_health, 0);

                //mp
                madness_ui_float(STRING("MP"), &unit->mp_component.current_mp, 0);
                madness_ui_same_line();
                madness_ui_string(STRING("\\"));
                madness_ui_same_line();
                madness_ui_float(STRING(""), &unit->mp_component.max_mp, 0);
            }
            madness_ui_padding("player to enemy padding");
            madness_ui_string(STRING("ENEMIES"));
            for (u32 i = 0; i < game->enemy_count; i++)
            {
                Unit* unit = madness_pulse_get_unit(game, game->enemy_names[i]);
                madness_ui_c_string(Character_Name_enum_string[unit->name]);

                madness_ui_float(STRING("Health"), &unit->health_component.current_health, 0);
                madness_ui_same_line();
                madness_ui_string(STRING("\\"));
                madness_ui_same_line();
                madness_ui_float(STRING(""), &unit->health_component.max_health, 0);

                madness_ui_float(STRING("MP"), &unit->mp_component.current_mp, 0);
                madness_ui_same_line();
                madness_ui_string(STRING("\\"));
                madness_ui_same_line();
                madness_ui_float(STRING(""), &unit->mp_component.max_mp, 0);

            }
        }


        for (u32 i = 0; i < game->madness_ai->ai_decision->num_items; ++i)
        {
            Madness_AI_Decision* decision =
                dynamic_array_get_ptr(game->madness_ai->ai_decision, Madness_AI_Decision, i);

            madness_ui_string(STRING_STRLEN(Ability_Name_enum_string[decision->ability_info->ability_name]));
        }


        madness_ui_window_end();
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
            ring_enqueue(game->turn_queue, &game->player_names[i]);
        }
        for (u32 i = 0; i < game->enemy_count; i++)
        {
            ring_enqueue(game->turn_queue, &game->enemy_names[i]);
        }

        break;
    case Turn_Initiative_Enemy:
        for (u32 i = 0; i < game->enemy_count; i++)
        {
            ring_enqueue(game->turn_queue, &game->enemy_names[i]);
        }
        for (u32 i = 0; i < game->player_count; i++)
        {
            ring_enqueue(game->turn_queue, &game->player_names[i]);
        }
        break;
    }
}

void game_resolve_unit_character_list_types(Madness_Pulse_Game* game)
{
    //puts units into the proper player or enemy lists
    game->player_count = 0;
    game->enemy_count = 0;


    for (int i = 0; i < game->units_count; ++i)
    {
        Unit* current_unit = game->units[i];;
        game->unit_names[i] = current_unit->name;


        switch (current_unit->character_type)
        {
        case Character_Type_Player:
            game->player_units[game->player_count] = current_unit;
            game->player_names[game->player_count] = current_unit->name;
            game->player_count++;
            break;
        case Character_Type_Enemy:
            game->enemy_units[game->enemy_count] = current_unit;
            game->enemy_names[game->enemy_count] = current_unit->name;
            game->enemy_count++;
            break;
        }
    }
}

bool can_current_unit_act(Madness_Pulse_Game* game)
{
    //can act if the unit if the unit has actions and is alive
    Unit* unit = madness_pulse_get_unit(game, game->current_units_turn);

    return action_component_is_action_available(&unit->action_component) && health_component_is_alive(
        &unit->health_component);
}

Unit* madness_pulse_get_unit(const Madness_Pulse_Game* game, const Character_Name name)
{
    for (u32 i = 0; i < game->units_count; i++)
    {
        if (game->units[i]->name == name)
        {
            return game->units[i];
        };
    }
    MASSERT(false);
    return NULL;
}

const char* madness_pulse_get_unit_name(const Madness_Pulse_Game* game, const Character_Name name)
{
    return Character_Name_enum_string[name];
}
