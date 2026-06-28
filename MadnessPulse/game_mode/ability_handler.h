#ifndef ABILITY_HANDLER_H
#define ABILITY_HANDLER_H
#include "actions.h"
#include "mp.h"
#include "targeting_handler.h"
#include "turn_based_game.h"


Ability_Handler* ability_handler_init(Madness_Pulse_Game* game)
{
    Ability_Handler* ability_handler = allocator_alloc(&game->allocator, sizeof(Ability_Handler));

    ability_handler->turn_start_components =
        dynamic_array_create(Turn_Trigger_Component_Info, 1, &game->heap_allocator);
    ability_handler->turn_end_components = dynamic_array_create(Turn_Trigger_Component_Info, 1, &game->heap_allocator);
    ability_handler->turn_start_end_components = dynamic_array_create(Turn_Trigger_Component_Info, 1,
                                                                      &game->heap_allocator);
    ability_handler->turn_first_start_components = dynamic_array_create(Turn_Trigger_Component_Info, 1,
                                                                        &game->heap_allocator);
    ability_handler->turn_final_end_components = dynamic_array_create(Turn_Trigger_Component_Info, 1,
                                                                      &game->heap_allocator);


    ability_handler->reversal_once_components = dynamic_array_create(Reversal_Component_Info, 1, &game->heap_allocator);
    ability_handler->reversal_units_turn_start_components = dynamic_array_create(
        Reversal_Component_Info, 1, &game->heap_allocator);
    ability_handler->reversal_permanent_components = dynamic_array_create(
        Reversal_Component_Info, 1, &game->heap_allocator);

    return ability_handler;
}


/*
// These are actions, its to show when one of thse might occur, but these dont really need to exist
UVFXSubsystem* VFXSubsystem = nullptr;
AClearStatusTriggersAction* ClearStatusAction;
AClearReversalsAction* ClearReversalAction;
AStatusTriggerAction* StatusTriggerAction;
*/


void ability_handler_process_normal_components(Madness_Pulse_Game* game,
                                               Ability_Target_Execution_Info* ability_target_info,
                                               Ability_Component* normal_components, const u32 normal_component_count,
                                               u32 overflow_count)
{
    DEBUG("Processing Normal Components");

    //NOTE: since the same ability will be activated multiple times in a row,
    // it would just make sense to just query the top of the replay action queue
    //  and just increment a number for how many times that thing needs to occur,
    //  instead of adding a bunch of redundant data to display the overflow

    for (u32 component_number = 0; component_number < normal_component_count; component_number++)
    {
        Ability_Component ability_component = normal_components[component_number];
        //TODO: get the targets that the component is targeting
        for (int i = 0; i < overflow_count; ++i)
        {
            for (u32 target_number = 0; target_number < ability_target_info->targets->num_items; target_number++)
            {
                Unit* current_target = array_get(ability_target_info->targets, Unit*, target_number);

                ability_component_process_effect(game, ability_target_info, current_target, &ability_component);
            }
        }
    }
}

bool IsThereAReversalTrigger(Madness_Pulse_Game* game)
{
    /*
    for (const auto& ReversalInfo : TurnReversalList)
    {
        if (ReversalInfo.ReversalComponent->TriggerReversal(ReversalInfo.ReversalTarget))
        {
            return true;
        }
    }*/
    return false;
}


/* High level Components Logic Functions */
void ability_handler_process_ability(Madness_Pulse_Game* game)
{
    Ability* ability = ability_registry_get_ability(game->ability_registry, game->currently_selected_ability);
    Ability_Info ability_info = ability_registry_get_ability_info(game->ability_registry,
                                                                  game->currently_selected_ability);

    Unit* unit_caster = madness_pulse_get_unit(game, game->current_units_turn);
    Ability_Target_Execution_Info ability_target_info = {
        .caster = unit_caster,
        .targets = target_handler_return_attack_targets(game->targeting_handler, game),
        .caster_allies = NULL,
        .caster_enemies = NULL,
        .ally_count = 0,
        .enemy_count = 0,
    };

    switch (ability_target_info.caster->character_type)
    {
    case Character_Type_Player:
        ability_target_info.caster_allies = game->player_units;
        ability_target_info.caster_enemies = game->enemy_units;
        ability_target_info.ally_count = game->player_count;
        ability_target_info.enemy_count = game->enemy_count;
        break;
    case Character_Type_Enemy:
        ability_target_info.caster_allies = game->enemy_units;
        ability_target_info.caster_enemies = game->player_units;
        ability_target_info.ally_count = game->enemy_count;
        ability_target_info.enemy_count = game->player_count;
        break;
    }


    // use up action, if the player is dumb and uses more than they can afford, that's on them
    if (ability_info.ability_action_cost == 0)
    {
        // DEBUG("Action cost is zero for %s", *ability_get_name(ability_info.ability_name);
    }
    else
    {
        action_component_decrease_actions(&unit_caster->action_component, ability_info.ability_action_cost);
        // command_handler_add_action() ???
        // NewActionQueue.Emplace(UnitCaster->ActionComponent);
    }

    // TODO: MP Check and unit caster mp flag check
    // if mp cost is greater than that available && use mp ability mp is off
    if (!mp_component_can_afford_ability_cost(unit_caster, ability_info.mp_cost))
    {
        // GameState.CurrentUnitsTurn->SpawnFailedPopUp();
        // return NewActionQueue;
        return;
    }
    {
        // use up mp
        mp_component_change_mp(unit_caster, &unit_caster->mp_component, -(float)ability_info.mp_cost);
        // NewActionQueue.Emplace(UnitCaster->MPComponent);
    }

    //checking for mirage, which will negate the ability used

    for (u32 i = 0; i < ability_target_info.targets->num_items; i++)
    {
        if (can_use_mirage(&unit_caster->special_ability_flag_list_component))
        {
            //we end the turn right there
            DEBUG("Mirage was used");
            // GameState.CurrentUnitsTurn->SpawnFailedPopUp();
            // return NewActionQueue;
        }
    }
    //checking for dance in the dark, 25% for the ability to fail
    if (unit_caster->special_ability_flag_list_component.DanceInTheDark)
    {
        float random_number = rand_range_i(0, 4);
        //roll our dice, if 4, then failed, it's a 25% chance to fail
        if (random_number >= 4)
        {
            //we end the turn right there
            DEBUG("Mirage was used");
            // GameState.CurrentUnitsTurn->SpawnFailedPopUp();
            // return NewActionQueue;
        }
    }


    //components that tell us more info about the turn/reversal component, like duration or what would cause the reversal
    Turn_Base_Component* turn_info_component = &ability->turn_info_component;
    Reversal_Base_Component* reversal_info_component = &ability->reversal_info_components;


    // must check our conditionals first
    /*
    if (ability->conditional_component.was_set)
    {
        for (int i = 0; i < conditional_components->num_items; ++i)
        {
            if (!ConditionalComponent->RequestIsConditionalSuccessful_Implementation(GameState, Targets))
            {
                // if this ever gets reached the ability failed
                // GameState.CurrentUnitsTurn->SpawnFailedPopUp(); // TODO: this should be a command
                return;
            }
        }
    }*/

    //some sanity checks
    if (ability->reversal_info_components.was_set == false && ability->reversal_component_count > 0)
    {
        MASSERT_MSG_FALSE("REVERSAL COMPONENT CONDITIONS WERE NOT SET FOR ABILITY");
    }
    if (ability->turn_info_component.was_set == false && ability->turn_component_count > 0)
    {
        MASSERT_MSG_FALSE("TURN COMPONENT INFO WAS NOT SET FOR ABILITY");
    }

    //get our overflow value. TODO: the visual for the overflow
    //we set it to 1 so that the ability executes at least once,
    u32 overflow_count = 1;
    overflow_count += overflow_component_get_overflow_trigger_count(&unit_caster->overflow_component,
                                                                    &unit_caster->battle_inventory_component,
                                                                    game->ability_registry);

    overflow_component_use_up_overflow(&unit_caster->overflow_component,
                                       &unit_caster->battle_inventory_component,
                                       game->ability_registry);


    ability_handler_process_normal_components(game, &ability_target_info, ability->normal_components,
                                              ability->normal_component_count, overflow_count);

    //NOTE: there are no more status trigger components, if an ability happens, it happens as is, with the overflow


    /* TODO: reversal and turn based components, they need a redesign anyway
    //mandatory we check for any reversals, as they must trigger after we go through the normal and status components
    if (IsThereAReversalTrigger())
    {
        NewActionQueue.Append(ProcessReversal(GameState));
    }

    if (ReversalComponent != nullptr)
    {
        AddReversalComponentToList(AbilityToProcess, reversal_components,
                                   ReversalComponent, GameState,
                                   Targets);

        //ideally this shouldn't be here but on the reversal component instead, or have the reversal component return
        ReversalPlayBackAction->Init(Targets, AbilityToProcess->AbilityInfo.AbilityTargetType,
                                     GameState.CurrentUnitsTurn);
        NewActionQueue.Emplace(ReversalPlayBackAction);
    }

    if (!TurnComponents.IsEmpty())
    {
        AddTurnComponentToList(TurnComponents, turn_trigger_components,
                               status_trigger_components, AbilityToProcess, UnitCaster, Targets);
    }

    //clear status and clear reversals
    NewActionQueue.Emplace(ClearStatusAction);

    NewActionQueue.Emplace(ClearReversalAction);

    //clear charge list if applicable
    GameState.CurrentUnitsTurn->ChargeListComponent->ClearActiveChargeList();
    NewActionQueue.Emplace(GameState.CurrentUnitsTurn->ChargeListComponent); // TODO: make this an action
*/
}


/*

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
