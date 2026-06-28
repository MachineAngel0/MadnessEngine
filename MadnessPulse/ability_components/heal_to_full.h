#ifndef HEAL_TO_FULL_H
#define HEAL_TO_FULL_H


#include "game_structs.h"
#include "health.h"


void heal_to_full_component_create(Ability_Component* ac)
{
    ac->type = Ability_Component_TYPE_HEAL_TO_FULL;


    ac->data.heal_to_full = (Heal_To_Full_Component){
        .nothing_to_do = true,
    };
}

void heal_to_full_ability(Unit* unit, const Heal_To_Full_Component* component)
{
    health_component_heal_to_full(&unit->health_component);
}

void heal_to_full_component_text(const Heal_To_Full_Component* component, String_Builder* string_builder)
{
    string_builder_append_string(string_builder, &(STRING("Heal Target To Full")));
}


#endif //HEAL_TO_FULL_H
