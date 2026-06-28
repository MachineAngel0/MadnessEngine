#ifndef OVERFLOW_COMPONENT_H
#define OVERFLOW_COMPONENT_H


#include "ability_registry.h"
#include "game_structs.h"


void overflow_component_init_default(Overflow_Component* overflow)
{
    overflow->current_overflow = 0;
    overflow->overflow_threshold = OVERFLOW_MAX;
}

u32 overflow_component_calculate_value_from_usage(const Overflow_Component* overflow,
                                                   const Battle_Inventory_Component* battle_inventory, Ability_Registry* ability_registry)
{
    u32 out_overflow_value = overflow->current_overflow;

    for (int i = 0; i < battle_inventory->battle_list->num_items; ++i)
    {

        const u32 val = ability_registry_get_ability_overflow_value(ability_registry, dynamic_array_get(battle_inventory->battle_list, Ability_Name, i));
        const u16 count = dynamic_array_get(battle_inventory->overflow_usage_count, u16, i);

        out_overflow_value += val * count ;
    }


    return out_overflow_value + overflow->current_overflow;
}


u32 overflow_component_get_overflow_trigger_count(const Overflow_Component* overflow,
                                                   const Battle_Inventory_Component* battle_inventory, Ability_Registry* ability_registry)
{
    u32 overflow_bar_usage = overflow->current_overflow;

    for (int i = 0; i < battle_inventory->battle_list->num_items; ++i)
    {

        const u32 val = ability_registry_get_ability_overflow_value(ability_registry, dynamic_array_get(battle_inventory->battle_list, Ability_Name, i));
        const u16 count = dynamic_array_get(battle_inventory->overflow_usage_count, u16, i);

        overflow_bar_usage += val * count;
    }

    u32 overflow_count = (overflow_bar_usage + overflow->current_overflow) / overflow->overflow_threshold;

    return overflow_count;
}

void overflow_component_use_up_overflow(Overflow_Component* overflow, const Battle_Inventory_Component* battle_inventory, Ability_Registry* ability_registry)
{
    u32 overflow_bar_usage = overflow->current_overflow;

    for (int i = 0; i < battle_inventory->battle_list->num_items; ++i)
    {

        const u32 val = ability_registry_get_ability_overflow_value(ability_registry, dynamic_array_get(battle_inventory->battle_list, Ability_Name, i));
        const u16 count = dynamic_array_get(battle_inventory->overflow_usage_count, u16, i);

        overflow_bar_usage += val * count;
    }

    u32 final_value = overflow_bar_usage + overflow->current_overflow;

    while (final_value >= overflow->overflow_threshold)
    {
        final_value -= overflow->overflow_threshold;
    }

    overflow->current_overflow = final_value;

}




#endif //OVERFLOW_COMPONENT_H
