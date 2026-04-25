#ifndef INVENTORY_H
#define INVENTORY_H

#include "ability.h"


//TODO: read in the file data
void inventory_component_init(Inventory_Component* inventory_component)
{
    memset(inventory_component, 0, sizeof(Inventory_Component));
}

void Inventory_Component_destroy(Inventory_Component* inventory_component)
{
}


#endif//INVENTORY_H
