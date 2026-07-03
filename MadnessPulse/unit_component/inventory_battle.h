#ifndef INVENTORY_H
#define INVENTORY_H
#include "ability_registry.h"


//Transition inventory to battle inventory
void inventory_component_copy_to_battle_inventory(Battle_Inventory_Component* battle_inventory_component,
                                                  Inventory_Component* inventory_component)
{
    dynamic_array_resize(battle_inventory_component->battle_list, inventory_component->battle_list_size);
    dynamic_array_resize(battle_inventory_component->ability_count, inventory_component->battle_list_size);
    dynamic_array_resize(battle_inventory_component->overflow_usage_count, inventory_component->battle_list_size);

    u16 starting_ability_count = STARTING_ABILITY_COUNT;
    u16 default_usage_count = 0;

    for (u32 i = 0; i < inventory_component->battle_list_size; i++)
    {
        dynamic_array_push(battle_inventory_component->battle_list, &inventory_component->battle_list_starting[i]);
        dynamic_array_push(battle_inventory_component->ability_count, &starting_ability_count);
        dynamic_array_push(battle_inventory_component->ability_count, &default_usage_count);
    }
}

//Battle Inventory

void battle_inventory_component_init(Madness_Pulse_Game* game, Battle_Inventory_Component* battle_inventory_component)
{
    memset(battle_inventory_component, 0, sizeof(Battle_Inventory_Component));

    battle_inventory_component->battle_list = dynamic_array_create(Ability_Name, INVENTORY_MAX_BATTLE_LIST,
                                                                   &game->heap_allocator);
    battle_inventory_component->ability_count = dynamic_array_create(u16, INVENTORY_MAX_BATTLE_LIST,
                                                                     &game->heap_allocator);
    battle_inventory_component->overflow_usage_count = dynamic_array_create(
        u16, INVENTORY_MAX_BATTLE_LIST, &game->heap_allocator);
    battle_inventory_component->current_overflow = 0;
    battle_inventory_component->overflow_threshold = OVERFLOW_MAX;
}


void battle_component_destroy(Battle_Inventory_Component* inventory_component)
{
    dynamic_array_free(inventory_component->battle_list);
}

void battle_inventory_usage_count_zero_all(Battle_Inventory_Component* battle_inventory_component)
{
    dynamic_array_zero(battle_inventory_component->overflow_usage_count);
}


void battle_inventory_add_ability(Battle_Inventory_Component* battle_inventory_component, Ability_Name ability_name,
                                  u32 count)
{
    for (int i = 0; i < battle_inventory_component->battle_list->num_items; ++i)
    {
        if (dynamic_array_get(battle_inventory_component->battle_list, Ability_Name, i) == ability_name)
        {

            dynamic_array_get(battle_inventory_component->ability_count, u16, i) += count;
            return;
        }
    }

    //reaching this points means we need to add the ability into the inventory
    dynamic_array_push(battle_inventory_component->battle_list, &ability_name);
    dynamic_array_push(battle_inventory_component->ability_count, &count);
    u16 overflow_usage_count = 0;
    dynamic_array_push(battle_inventory_component->overflow_usage_count, &overflow_usage_count);
}

void battle_inventory_remove_ability(Battle_Inventory_Component* battle_inventory_component, Ability_Name ability_name,
                                     u32 count)
{

}

void battle_inventory_remove_all_ability(Battle_Inventory_Component* battle_inventory_component,
                                         Ability_Name ability_name)
{
}


void battle_inventory_add_debug_ability(Battle_Inventory_Component* battle_inventory_component,
                                        Ability_Name ability_name)
{
    //OPTIMIZE: hash set, or even a hash map
    for (u32 i = 0; i < battle_inventory_component->battle_list->num_items; i++)
    {
        if (dynamic_array_get(battle_inventory_component->battle_list, Ability_Name, i) == ability_name)
        {
            return;
        }
    }

    u16 starting_ability_count = STARTING_ABILITY_COUNT;
    u16 default_usage_count = 0;
    dynamic_array_push(battle_inventory_component->battle_list, &ability_name);

    dynamic_array_push(battle_inventory_component->ability_count, &starting_ability_count);
    dynamic_array_push(battle_inventory_component->overflow_usage_count, &default_usage_count);
}


//OVERFLOW STUFF

void battle_inventory_consume_overflow_abilties(Battle_Inventory_Component* battle_inventory_component,
                                                Ability_Name ability_name)
{
    //OPTIMIZE: hash set, or even a hash map
    for (u32 i = 0; i < battle_inventory_component->battle_list->num_items; i++)
    {
        if (dynamic_array_get(battle_inventory_component->battle_list, Ability_Name, i) == ability_name)
        {
            return;
        }
    }

    u16 starting_ability_count = STARTING_ABILITY_COUNT;
    u16 default_usage_count = 0;
    dynamic_array_push(battle_inventory_component->battle_list, &ability_name);

    dynamic_array_push(battle_inventory_component->ability_count, &starting_ability_count);
    dynamic_array_push(battle_inventory_component->overflow_usage_count, &default_usage_count);
}


u32 battle_inventory_calculate_value_from_usage(
    const Battle_Inventory_Component* battle_inventory, Ability_Registry* ability_registry)
{
    u32 out_overflow_value = battle_inventory->current_overflow;

    for (int i = 0; i < battle_inventory->battle_list->num_items; ++i)
    {
        const u32 val = ability_registry_get_ability_overflow_value(ability_registry,
                                                                    dynamic_array_get(
                                                                        battle_inventory->battle_list, Ability_Name,
                                                                        i));
        const u16 count = dynamic_array_get(battle_inventory->overflow_usage_count, u16, i);

        out_overflow_value += val * count;
    }


    return out_overflow_value + battle_inventory->current_overflow;
}


u32 battle_inventory_get_overflow_trigger_count(const Battle_Inventory_Component* battle_inventory,
                                                Ability_Registry* ability_registry)
{
    u32 overflow_bar_usage = battle_inventory->current_overflow;

    for (int i = 0; i < battle_inventory->battle_list->num_items; ++i)
    {
        const u32 val = ability_registry_get_ability_overflow_value(ability_registry,
                                                                    dynamic_array_get(
                                                                        battle_inventory->battle_list, Ability_Name,
                                                                        i));
        const u16 count = dynamic_array_get(battle_inventory->overflow_usage_count, u16, i);

        overflow_bar_usage += val * count;
    }

    u32 overflow_count = (overflow_bar_usage + battle_inventory->current_overflow) / battle_inventory->
        overflow_threshold;

    return overflow_count;
}

void battle_inventory_use_up_overflow_and_abilities(Battle_Inventory_Component* battle_inventory,
                                                    Ability_Registry* ability_registry)
{
    //get the overflow value and then use it up
    u32 overflow_bar_usage = battle_inventory->current_overflow;

    for (int i = 0; i < battle_inventory->battle_list->num_items; ++i)
    {
        const u32 val = ability_registry_get_ability_overflow_value(ability_registry,
                                                                    dynamic_array_get(
                                                                        battle_inventory->battle_list, Ability_Name,
                                                                        i));
        const u16 count = dynamic_array_get(battle_inventory->overflow_usage_count, u16, i);

        overflow_bar_usage += val * count;
    }

    u32 final_value = overflow_bar_usage + battle_inventory->current_overflow;

    while (final_value >= battle_inventory->overflow_threshold)
    {
        final_value -= battle_inventory->overflow_threshold;
    }

    battle_inventory->current_overflow = final_value;

    //Optimize: if this pops up in a profiler, then you can reduces the passes needed to do this
    //go through our inventory and reduce the abilties inside it
    for (int i = 0; i < battle_inventory->battle_list->num_items; ++i)
    {
        u16* cur_ability_count = _dynamic_array_get(battle_inventory->ability_count, i);
        u16* cur_overflow_usage = _dynamic_array_get(battle_inventory->overflow_usage_count, i);

        *cur_ability_count -= *cur_overflow_usage;

        //the count should never drop below zero
        MASSERT(*cur_ability_count >= 0);

        *cur_overflow_usage = 0;
    }


    //we copy from the current battle list, only the abilities whos count are not zero, this is to fill the holes in the array
    Dynamic_Array* replacement_battle_list = dynamic_array_create(Ability_Name,
                                                                  battle_inventory->battle_list->num_items,
                                                                  battle_inventory->battle_list->allocator);

    Dynamic_Array* replacement_ability_count = dynamic_array_create(u16, battle_inventory->battle_list->num_items,
                                                                    battle_inventory->battle_list->allocator);

    for (int i = 0; i < battle_inventory->battle_list->num_items; ++i)
    {
        //Optimize (Memory and Fragmentation):
        // you can use a two pointer approach to copy the data in place, instead of creating the copy

        //NOTE: it would be interesting to instead just swap the ability to the back of tha array instead of removing it,
        // showing the player the abilties they used to have, and could generate

        if (dynamic_array_get(battle_inventory->ability_count, u16, i) <= 0)
        {
            continue;
        }

        dynamic_array_push(replacement_battle_list, _dynamic_array_get(battle_inventory->battle_list, i));
        dynamic_array_push(replacement_ability_count, _dynamic_array_get(battle_inventory->ability_count, i));
    }

    //swap the new data in and free the old data

    Dynamic_Array* old_battle_list = battle_inventory->battle_list;
    Dynamic_Array* old_overflow_count = battle_inventory->ability_count;

    battle_inventory->battle_list = replacement_battle_list;
    battle_inventory->ability_count = replacement_ability_count;

    dynamic_array_free(old_battle_list);
    dynamic_array_free(old_overflow_count);
}


#endif//INVENTORY_H
