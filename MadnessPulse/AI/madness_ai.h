#ifndef GAME_AI_H
#define GAME_AI_H

#include "darray.h"
#include "game_enums.h"
#include "game_structs.h"
#include "logger.h"
#include "targeting_handler.h"


//conceptually we should use a utility score UI, makes the most sense,
//actually it might be better to look into conditional lists, like if (health below 50%) -> use ability x
// https://www.reddit.com/r/gamedev/comments/enabqz/how_does_the_ai_in_turn_based_rpgs_work/ 

//how do ai units decide on the abilties to use that specific turn?
//should ai cycle throught their abilties, like in a turn order, like smt does
//but is predictable AI really part of this game, to a certain extend they should be predictable for the player
//should the ai choose whats best for it rn, or after each ability is used(a bit cursed since we need to present the entire list)


Madness_AI* madness_ai_init(Madness_Pulse_Game* game)
{
    Madness_AI* ai = allocator_alloc(&game->allocator, sizeof(Madness_AI));
    ai->ai_count = 0;
    ai->ai_max = MAX_ENEMY_UNIT_COUNT;

    ai->ai_decision = dynamic_array_create(Madness_AI_Decision,1, &game->heap_allocator);
    return ai;
}


Madness_AI_Unit_Info* madness_ai_get_character_ai(Madness_AI* ai_brain, Character_Name character_name)
{
    for (u32 i = 0; i < ai_brain->ai_count; ++i)
    {
        Madness_AI_Unit_Info* ai = &ai_brain->ai_list[i];
        if (ai->character_name == character_name)
        {
            return ai;
        }
    }

    MASSERT(false);
    return NULL;
}


DYNAMIC_ARRAY_TYPE(AI_Ability)* madness_ai_get_character_ai_ability_list(
    Madness_AI* ai_brain, Character_Name character_name)
{
    for (u32 i = 0; i < ai_brain->ai_count; ++i)
    {
        Madness_AI_Unit_Info* ai = &ai_brain->ai_list[i];
        if (ai->character_name == character_name)
        {
            return ai->ability_list;
        }
    }
    MASSERT(false);
    return NULL;
}


void madness_ai_add_ability(Madness_AI* ai, DYNAMIC_ARRAY_TYPE(AI_Ability)* ability_list,
                            Ability_Name ability_name, u8 turn_index,
                            bool ability_allowed_to_overflow)
{
    MASSERT(turn_index > 0);

    AI_Ability new_ability_info = {
        .ability_name = ability_name,
        .ability_allowed_to_overflow = ability_allowed_to_overflow,
        .turn_index = turn_index,
    };
    dynamic_array_push(ability_list, &new_ability_info);
}


AI_Ability* madness_ai_get_ability(Madness_AI* ai, Character_Name character_name, Ability_Name ability_name,
                                   u8 turn_index)
{
    MASSERT(turn_index > 0);

    Dynamic_Array* ability_list = madness_ai_get_character_ai_ability_list(ai, character_name);

    for (u32 i = 0; i < ability_list->num_items; ++i)
    {
        AI_Ability* ability = dynamic_array_get_ptr(ability_list, AI_Ability, i);
        if (ability->turn_index == turn_index && ability->ability_name == ability_name)
        {
            return ability;
        }
    }
}

void madness_ai_add_heal_consideration(Madness_AI* madness_ai, Character_Name ai_name, Ability_Name name, u8 turn_index,
                                       AI_Heal_Consideration heal_consideration)
{
    AI_Ability* ability = madness_ai_get_ability(madness_ai, ai_name, name, turn_index);

    ability->ai_consideration[ability->ai_consideartion_count].type = AI_Consideration_Type_Heal;
    ability->ai_consideration[ability->ai_consideartion_count].data.heal = heal_consideration;
    ability->ai_consideartion_count++;
}


void madness_ai_create_from_table(Madness_AI* madness_ai, Character_Name ai_name,
                                  DYNAMIC_ARRAY_TYPE(AI_Ability)* ability_list)
{
    if (ai_name == Character_Name_Clown)
    {
        madness_ai_add_ability(madness_ai, ability_list, Ability_Name_DEBUG_DAMAGE, 1, true);
        /*madness_ai_add_heal_consideration(madness_ai, ai_name, Ability_Name_DEBUG_DAMAGE, 1, (AI_Heal_Consideration){
                                              .target_type = AI_Target_Type_Ability, .health_percent = 0.2f,
                                              .bias = AI_MultiTarget_Health_Bias_Lowest_Health,
                                              .sign = AI_Consideration_Sign_Greater_Than
                                          });*/


        madness_ai_add_ability(madness_ai, ability_list, Ability_Name_DEBUG_DAMAGE, 2, false);
        madness_ai_add_ability(madness_ai, ability_list, Ability_Name_DEBUG_DAMAGE, 3, false);
    }
    if (ai_name == Character_Name_Red_Jester)
    {
        madness_ai_add_ability(madness_ai, ability_list, Ability_Name_DEBUG_HEAL, 1, true);
        madness_ai_add_ability(madness_ai, ability_list, Ability_Name_DEBUG_DAMAGE, 2, false);
        madness_ai_add_ability(madness_ai, ability_list, Ability_Name_DEBUG_HEAL, 3, false);
    }
    if (ai_name == Character_Name_Puppet)
    {
        madness_ai_add_ability(madness_ai, ability_list, Ability_Name_DEBUG_DAMAGE, 1, true);
        madness_ai_add_ability(madness_ai, ability_list, Ability_Name_DEBUG_DAMAGE, 2, false);
        madness_ai_add_ability(madness_ai, ability_list, Ability_Name_DEBUG_DAMAGE, 3, false);
    }
    if (ai_name == Character_Name_Doll)
    {
        madness_ai_add_ability(madness_ai, ability_list, Ability_Name_DEBUG_DAMAGE, 1, true);
        madness_ai_add_ability(madness_ai, ability_list, Ability_Name_DEBUG_DAMAGE, 2, false);
        madness_ai_add_ability(madness_ai, ability_list, Ability_Name_DEBUG_DAMAGE, 3, false);
    }
}

void madness_ai_instantiate_enemy(Madness_Pulse_Game* game, Madness_AI* madness_ai)
{
    for (u32 i = 0; i < game->enemy_count; ++i)
    {
        madness_ai->ai_list[i].character_name = game->enemy_units[i]->name;
        madness_ai->ai_list[i].overflow_points = 0;
        //NOTE: completely abritratry number
        madness_ai->ai_list[i].ability_list = dynamic_array_create(AI_Ability, 12, &game->heap_allocator);
        madness_ai->ai_count++;

        madness_ai_create_from_table(madness_ai, game->enemy_units[i]->name, madness_ai->ai_list[i].ability_list);
    }
}


//NOTE: its probably better that the ai isn't allowed have abilities that can fail
// Ability_Name filter_abilities_with_conditions(Madness_Pulse_Game* game, Ability_Name ai_name);

ARRAY_TYPE(AI_Ability)* madness_ai_get_abilties_for_turn_index(Madness_Pulse_Game* game, Madness_AI* madness_ai,
                                                               Character_Name ai_name, u8 turn_index)
{
    //TODO:
}

ARRAY_TYPE(AI_Ability)* madness_ai_filter_abilities_with_cooldowns(Madness_Pulse_Game* game);

//high level choose ability function
AI_Ability* choose_ability(Madness_Pulse_Game* game);
ARRAY_TYPE(Unit_Handle)* choose_target(Madness_Pulse_Game* game);

// since we want to present to the player all the abilites the AI will be using, they have to first present the abilties
void madness_ai_take_turn(Madness_Pulse_Game* game)
{
    //rn lets just pick a random ability
    Madness_AI* madness_ai = game->madness_ai;

    Unit* current_ai_unit = madness_pulse_get_unit(game, game->current_units_turn);
    dynamic_array_free(madness_ai->ai_decision);

    madness_ai->ai_decision = dynamic_array_create(Madness_AI_Decision, current_ai_unit->action_component.actions_available, &game->heap_allocator);



    //TODO: ai keeps taking actions as until they run out of moves
    while (current_ai_unit->action_component.actions_available > 0)
    {
        Madness_AI_Unit_Info* unit_info = madness_ai_get_character_ai(madness_ai, current_ai_unit->name);


        u32 random_index = rand_range_i(0, unit_info->ability_list->num_items-1);
        AI_Ability* ai_ability = &dynamic_array_get(unit_info->ability_list, AI_Ability, random_index);
        Ability_Name selected_ability = ai_ability->ability_name;

        targeting_handler_create_targeting_info(game, selected_ability);
        ARRAY_TYPE(Unit*)* unit_targets = target_handler_return_copy_available_targets_for_ai(game, game->targeting_handler, &game->frame_allocator);

        Madness_AI_Decision decision = {0};
        decision.ability_info = ai_ability;

        switch (ability_registry_get_new_ability_info(game->ability_registry, selected_ability)->ability_target_area)
        {
        case Target_Area_Affect_Single_Target:
            u32 random_target_index = rand_range_i(0, unit_targets->num_items-1);
            decision.chosen_targets[decision.chosen_units_count++] = array_get(unit_targets, Unit*, random_target_index);
            break;
        case Target_Area_Affect_Target_All:
            for (u32 i = 0; i < unit_targets->num_items; i++)
            {
                decision.chosen_targets[decision.chosen_units_count++] = (Unit*)_array_get(unit_targets, i);
            }

            break;
        }


        dynamic_array_push(madness_ai->ai_decision, &decision);
        //TODO: at some point this needs to operate on a copy of the game data, that is hidden from the player and is ahead in game state
        ability_handler_process_ability(game, decision.ability_info->ability_name,
                                       target_handler_return_attack_targets(game->targeting_handler, game));

    }
}


//one for each type of event we care about
void listen_for_x_event();
//this should be queued for a first turn start, of that unit (maybe)
void ai_swap_inventory();


#endif //GAME_AI_H
