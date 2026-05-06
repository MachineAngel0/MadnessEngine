#ifndef INVENTORY_H
#define INVENTORY_H

#include "ability.h"


//TODO: read in the file data
void inventory_component_init(Inventory_Component* inventory_component)
{
    memset(inventory_component, 0, sizeof(Inventory_Component));
}

void inventory_component_destroy(Inventory_Component* inventory_component)
{
}


void inventory_component_spawn(Inventory_Component* inventory_component)
{

}

void inventory_component_add_to_starting_battle_list(Inventory_Component* inventory_component, const Ability_Name ability_name)
{
    inventory_component->battle_list_starting[inventory_component->battle_list_size++] = ability_name;
}

void inventory_component_add_to_battle_list(Inventory_Component* inventory_component, const Ability_Name ability_name)
{
    inventory_component->battle_list_dynamic[inventory_component->battle_list_dynamic_size++] = ability_name;
}


#endif//INVENTORY_H
