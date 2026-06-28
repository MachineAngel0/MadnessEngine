#ifndef DRAIN_ALL_H
#define DRAIN_ALL_H


#include "game_structs.h"
#include "health.h"


void drain_all_component_create(Ability_Component* ac)
{
    ac->type = Ability_Component_TYPE_DRAIN_ALL;


    ac->data.drain_all = (Drain_All_Component){
        .amount = true,
    };
}

void drain_all_ability(Unit* unit, const Drain_All_Component* component)
{
    health_component_health_to_zero(&unit->health_component);
}

void drain_all_component_text(const Drain_All_Component* component, String_Builder* string_builder)
{
    string_builder_append_string(string_builder, &STRING("Drain all of Casters health"));
}


#endif //DRAIN_ALL_H
