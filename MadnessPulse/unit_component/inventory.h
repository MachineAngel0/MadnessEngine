#ifndef INVENTORY_H
#define INVENTORY_H



//Inventory


//TODO: read in the file data
void inventory_component_init(Inventory_Component* inventory_component)
{
    memset(inventory_component, 0, sizeof(Inventory_Component));
}

void inventory_component_destroy(Inventory_Component* inventory_component)
{
}



//Transition
void inventory_component_copy_to_battle_inventory(Battle_Inventory_Component* battle_inventory_component, Inventory_Component* inventory_component)
{
    dynamic_array_resize(battle_inventory_component->battle_list , inventory_component->battle_list_size);
    dynamic_array_resize(battle_inventory_component->ability_count , inventory_component->battle_list_size);
    dynamic_array_resize(battle_inventory_component->overflow_usage_count , inventory_component->battle_list_size);

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

    battle_inventory_component->battle_list = dynamic_array_create(Ability_Name, INVENTORY_MAX_BATTLE_LIST, &game->heap_allocator);
    battle_inventory_component->ability_count = dynamic_array_create(u16, INVENTORY_MAX_BATTLE_LIST, &game->heap_allocator);
    battle_inventory_component->overflow_usage_count = dynamic_array_create(u16, INVENTORY_MAX_BATTLE_LIST, &game->heap_allocator);

}


void battle_component_destroy(Battle_Inventory_Component* inventory_component)
{

    dynamic_array_free(inventory_component->battle_list);

}

void battle_inventory_usage_count_zero_all(Battle_Inventory_Component* battle_inventory_component)
{
    dynamic_array_zero(battle_inventory_component->overflow_usage_count);


}


void battle_inventory_add_ability(Battle_Inventory_Component* battle_inventory_component, Ability_Name ability_name, u32 count)
{

}

void battle_inventory_remove_ability(Battle_Inventory_Component* battle_inventory_component, Ability_Name ability_name, u32 count)
{

}

void battle_inventory_remove_all_ability(Battle_Inventory_Component* battle_inventory_component, Ability_Name ability_name)
{



}



void battle_inventory_add_debug_ability(Battle_Inventory_Component* battle_inventory_component, Ability_Name ability_name)
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





#endif//INVENTORY_H
