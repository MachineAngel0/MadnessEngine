#ifndef TARGETING_H
#define TARGETING_H

#include "game_structs.h"
#include "turn_based_game.h"

Targeting_Handler* targeting_handler_init(Madness_Pulse_Game* game)
{
    Targeting_Handler* targeting_handler = allocator_alloc(&game->allocator, sizeof(Targeting_Handler));
    u8 max_targets_available = 10; //TODO: completely abritrary value, will either set a hard limit or dynamic allocate
    targeting_handler->targets_available_array = dynamic_array_create(Character_Name, max_targets_available,
                                                                      &game->heap_allocator);
    return targeting_handler;
}

void targeting_handler_shutdown(Targeting_Handler* targeting_handler)
{
}

void targeting_handler_create_targeting_info(Madness_Pulse_Game* game, Ability_Name ability_name)
{
    Ability_Info info = ability_registry_get_ability_info(game->ability_registry, ability_name);

    //create a bunch of info needed for the ability to function


    dynamic_array_clear(game->targeting_handler->targets_available_array);

    switch (info.ability_target_type)
    {
    case Ability_Target_Type_Self:
        dynamic_array_push(game->targeting_handler->targets_available_array, &game->current_units_turn);
        break;
    case Ability_Target_Type_Allies:
        for (int i = 0; i < game->player_count; ++i)
        {
            if (game->current_units_turn == game->player_names[i])
            {
                continue;
            }
            dynamic_array_push(game->targeting_handler->targets_available_array, &game->player_names[i]);
        }

        break;
    case Ability_Target_Type_Self_And_Allies:
        dynamic_array_push_multi(game->targeting_handler->targets_available_array, game->player_names,
                                 game->player_count);
        break;
    case Ability_Target_Type_Enemies:
        dynamic_array_push_multi(game->targeting_handler->targets_available_array, game->enemy_names,
                                 game->enemy_count);
        break;
    case Ability_Target_Type_Self_And_Enemies:
        dynamic_array_push(game->targeting_handler->targets_available_array, &game->current_units_turn);
        dynamic_array_push_multi(game->targeting_handler->targets_available_array, game->enemy_names,
                                 game->enemy_count);
        break;
    case Ability_Target_Type_Allies_And_Enemies:
        for (int i = 0; i < game->player_count; ++i)
        {
            if (game->current_units_turn == game->player_names[i])
            {
                continue;
            }
            dynamic_array_push(game->targeting_handler->targets_available_array, &game->player_names[i]);
        }
        for (int i = 0; i < game->player_count; ++i)
        {
            if (game->current_units_turn == game->player_names[i])
            {
                continue;
            }
            dynamic_array_push(game->targeting_handler->targets_available_array, &game->player_names[i]);
        }
        break;

    case Ability_Target_Type_All:
        dynamic_array_push_multi(game->targeting_handler->targets_available_array, game->player_names,
                                 game->player_count);
        dynamic_array_push_multi(game->targeting_handler->targets_available_array, game->enemy_names,
                                 game->enemy_count);
        break;
    default:
        FATAL("Failed To Create Ability Targeting Array");
        break;
    }


    //set target lock and targeting display info

    //look at the first target
    game->targeting_handler->current_lock_on_target = dynamic_array_get(
        game->targeting_handler->targets_available_array, Character_Name, 0);

    switch (info.ability_target_area)
    {
    case Target_Area_Affect_Single_Target:
        // CurrentLockOnTarget->ShowTargetLock();
        // CurrentLockOnTarget->ShowTargetLockResistanceDisplay();
        break;
    case Target_Area_Affect_Target_All:
        for (int i = 0; i < game->targeting_handler->targets_available_array->num_items; i++)
        {
            //TODO: show target lock
            // targeting_handler->targets_available->data[i] = targeting_handler->targets_available->data[i];
        }
        //only have one unit show their resistance display
        // CurrentLockOnTarget->ShowTargetLockResistanceDisplay();
        break;
    }
}


ARRAY_TYPE(Unit*)* targeting_handler_create_targeting_for_component(Madness_Pulse_Game* game,
                                                                    Ability_Component_Target_Type component_target,
                                                                    Ability_Target_Execution_Info* ability_target_info)
{
    //
    ARRAY_TYPE(Unit*)* out_target_units;
    switch (component_target)
    {
    case Ability_Component_Target_Type_Target:
        out_target_units = ability_target_info->ability_targets;
        break;
    case Ability_Component_Target_Type_Caster:
        out_target_units = array_create(Unit*, 1, &game->frame_allocator);
        array_push(out_target_units, &ability_target_info->caster);
        break;
    case Ability_Component_Target_Type_Allies:
        out_target_units = array_create(Unit*, ability_target_info->ally_count, &game->frame_allocator);
        _array_push_c_array(out_target_units, ability_target_info->caster_allies, ability_target_info->ally_count);
        break;
    case Ability_Component_Target_Type_Enemies:
        out_target_units = array_create(Unit*, ability_target_info->enemy_count, &game->frame_allocator);
        _array_push_c_array(out_target_units, ability_target_info->caster_enemies, ability_target_info->enemy_count);
        break;
    case Ability_Component_Target_Type_All:
        out_target_units = array_create(Unit*, game->units_count, &game->frame_allocator);
        _array_push_c_array(out_target_units, game->units, game->units_count);
        break;
    }


    return out_target_units;
}


void targeting_handler_move_targeting_left(Targeting_Handler* targeting_handler)
{
    if (targeting_handler->targeting_count <= 0)
    {
        targeting_handler->targeting_count = targeting_handler->targets_available_array->num_items - 1;
    }
    else
    {
        targeting_handler->targeting_count--;
    }
}

void targeting_handler_move_targeting_right(Targeting_Handler* targeting_handler)
{
    if (targeting_handler->targeting_count >= targeting_handler->targets_available_array->num_items - 1)
    {
        targeting_handler->targeting_count = 0;
    }
    else
    {
        targeting_handler->targeting_count++;
    }
}

void targeting_handler_move_unit_targeting(Madness_Pulse_Game* game, const Targeting_Direction MoveTargetDirection)
{
    // TODO:
    // check if its a single target or multitarget move
    // check if its for a allies, then we need to flip the move direction


    // if its single target, we want to hide the target lock on and resistance display
    // if its multli target, we want to hide only the resistance display
    Ability_Info info = ability_registry_get_ability_info(game->ability_registry, game->currently_selected_ability_by_player);
    switch (info.ability_target_area)
    {
    case Target_Area_Affect_Single_Target:
        // CurrentLockOnTarget->HideTargetLock();
        // CurrentLockOnTarget->HideTargetLockResistanceDisplay();
        break;
    case Target_Area_Affect_Target_All:
        // CurrentLockOnTarget->HideTargetLockResistanceDisplay();
        break;
    }

    // if we are targeting our allies in anyway, then we want to flip the direction of the move targeting, due to the camera direction and player order
    if (info.ability_target_type == Ability_Target_Type_Allies || info.ability_target_type ==
        Ability_Target_Type_Self_And_Allies)
    {
        switch (MoveTargetDirection)
        {
        case Targeting_Direction_Left:
            targeting_handler_move_targeting_right(game->targeting_handler);
            break;
        case Targeting_Direction_Right:
            targeting_handler_move_targeting_left(game->targeting_handler);
            break;
        }
    }
    else
    {
        switch (MoveTargetDirection)
        {
        case Targeting_Direction_Left:
            targeting_handler_move_targeting_left(game->targeting_handler);
            break;
        case Targeting_Direction_Right:
            targeting_handler_move_targeting_right(game->targeting_handler);
            break;
        }
    }

    // CurrentLockOnTarget = game->targeting_handler->targets_available[game->targeting_handler->targeting_count];
    // CurrentLockOnTarget->ShowTargetLock();
    // CurrentLockOnTarget->ShowTargetLockResistanceDisplay();
}


void target_handler_clear_all_target_locks(Targeting_Handler* targeting_handler, Madness_Pulse_Game* game)
{
    for (int i = 0; i < targeting_handler->targets_available_array->num_items; ++i)
    {
        Unit* unit = madness_pulse_get_unit(
            game, dynamic_array_get(targeting_handler->targets_available_array, Character_Name, i));
        // LockOnTargets->HideTargetLock();
        // LockOnTargets->HideTargetLockResistanceDisplay();
    }
}

// Character_Name_array* ReturnTargetsForActionManager(Targeting_Handler* targeting_handler, Madness_Pulse_Game* game, Ability* AbilityChoosen)
ARRAY_TYPE(Unit*)* target_handler_return_attack_targets(Targeting_Handler* targeting_handler, Madness_Pulse_Game* game)
{
    Ability_Info info = ability_registry_get_ability_info(game->ability_registry, game->currently_selected_ability_by_player);

    if (info.ability_target_area == Target_Area_Affect_Target_All)
    {
        ARRAY_TYPE(Unit*)* out_array = array_create(Unit*, targeting_handler->targets_available_array->num_items,
                                                    &game->frame_allocator);
        for (u32 i = 0; i < targeting_handler->targets_available_array->num_items; i++)
        {
            Unit* unit = madness_pulse_get_unit(
                game, dynamic_array_get(targeting_handler->targets_available_array, Character_Name, i));
            array_push(out_array, &unit);
        }

        return out_array;
        // return dynamic_array_copy_fixed_size(targeting_handler->targets_available_array, &game->allocator);
    }

    //implied that this is single target if it reaches this point
    if (info.ability_target_area == Target_Area_Affect_Single_Target)
    {
        Array* single_target_array = array_create(Unit*, 1, &game->frame_allocator);
        Unit* unit = madness_pulse_get_unit(game, dynamic_array_get(targeting_handler->targets_available_array,
                                                                    Character_Name,
                                                                    targeting_handler->targeting_count));
        array_push(single_target_array, &unit);
        return single_target_array;
    }

    MASSERT(false);
    return NULL;
}

ARRAY_TYPE(Unit*)* target_handler_return_copy_available_targets_for_ai(Madness_Pulse_Game* game,
                                                                       Targeting_Handler* targeting_handler,
                                                                       Allocator* allocator)
{
    Array* out_array = array_create(Unit*, targeting_handler->targets_available_array->num_items, allocator);

    for (u32 i = 0; i < targeting_handler->targets_available_array->num_items; i++)
    {
        array_push(out_array, madness_pulse_get_unit(
                       game, dynamic_array_get(targeting_handler->targets_available_array, Character_Name, i)));
    }

    return out_array;
}


/*
// AI Targeting
void CreateAITargeting(Madness_Pulse_Game* GameState, Ability* AbilityChosen, AAIAction* ChosenAction);

void ProcessAITargets(Madness_Pulse_Game GameState, const Ability* AbilityChosen);
void ProcessAITargetsCanAffect(Madness_Pulse_Game GameState, AAIAction* ChosenAction,
                               const Ability* AbilityChosen);

void AddToAIAbilityTargetsAvailable(Unit* CurrentUnitsTurn, TArray<Unit*> TargetsToAdd, bool ExcludeSelf);

Unit* ReturnRandomUnit(Madness_Pulse_Game GameState, TArray<Unit*> Units);
*/

#endif //TARGETING_H
