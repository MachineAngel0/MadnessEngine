#ifndef INVENTORY_H
#define INVENTORY_H

#include "ability.h"


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
    dynamic_array_resize(battle_inventory_component->battle_list_dynamic , inventory_component->battle_list_size);
    dynamic_array_resize(battle_inventory_component->ability_count , inventory_component->battle_list_size);
    for (u32 i = 0; i < inventory_component->battle_list_size; i++)
    {
        dynamic_array_push(battle_inventory_component->battle_list_dynamic, &inventory_component->battle_list_starting[i]);
        u32 start_count = 1;
        dynamic_array_push(battle_inventory_component->ability_count, &start_count);
    }
}

//Battle Inventory

void battle_inventory_component_init(Madness_Pulse_Game* game, Battle_Inventory_Component* battle_inventory_component)
{
    memset(battle_inventory_component, 0, sizeof(Battle_Inventory_Component));

    battle_inventory_component->battle_list_dynamic = dynamic_array_create(Ability_Name, INVENTORY_MAX_BATTLE_LIST, &game->heap_allocator);
    battle_inventory_component->ability_count = dynamic_array_create(u32, INVENTORY_MAX_BATTLE_LIST, &game->heap_allocator);


}


void battle_component_destroy(Battle_Inventory_Component* inventory_component)
{

    dynamic_array_free(inventory_component->battle_list_dynamic);
    dynamic_array_free(inventory_component->ability_count);

}


void battle_inventory_add_debug_ability(Battle_Inventory_Component* battle_inventory_component, const Ability_Name ability_name)
{
    dynamic_array_push(battle_inventory_component->battle_list_dynamic, &ability_name);
}



#endif//INVENTORY_H
