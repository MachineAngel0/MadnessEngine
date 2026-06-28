#ifndef ABILITY_REMOVER_H
#define ABILITY_REMOVER_H

#include "game_structs.h"



void ability_remover_component_create(Ability_Component* ac, Ability_Name name, u32 count)
{
    ac->type = Ability_Component_TYPE_ABILITY_REMOVER;


    ac->data.ability_remove = (Ability_Remover_Component){
        .ability_to_remove = name,
        .count = count,
    };
}

void ability_remover_ability(Unit* unit, const Ability_Remover_Component* component)
{
    battle_inventory_remove_ability(&unit->battle_inventory_component, component->ability_to_remove,
                                    component->count);
}

void ability_remover_component_text(const Ability_Remover_Component* component, String_Builder* string_builder)
{
    string_builder_append_string(string_builder,
                                 string_format(string_builder->allocator, "Remove %d abilites with the name %s",
                                               component->count,
                                               Ability_Name_enum_string[component->ability_to_remove]));
}






#endif
