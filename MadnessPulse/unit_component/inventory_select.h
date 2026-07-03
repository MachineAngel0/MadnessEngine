#ifndef INVENTORY_SELECT_H
#define INVENTORY_SELECT_H

#include "darray.h"


//Inventory


//TODO: read in the file data
void inventory_component_init(Inventory_Component* inventory_component)
{
    memset(inventory_component, 0, sizeof(Inventory_Component));
}

void inventory_component_destroy(Inventory_Component* inventory_component)
{
}

void inventory_add_ability(Inventory_Component* battle_inventory_component, Ability_Name ability_name, u32 count)
{

}

void inventory_remove_ability(Inventory_Component* battle_inventory_component, Ability_Name ability_name, u32 count)
{

}

void inventory_remove_all_ability(Inventory_Component* battle_inventory_component, Ability_Name ability_name)
{

}



#endif//INVENTORY_SELECT_H
