#ifndef DRAIN_H
#define DRAIN_H


#include "game_structs.h"
#include "health.h"


void drain_component_create(Ability_Component* ac, const float drain_amount)
{
    ac->type = Ability_Component_TYPE_DRAIN;


    ac->data.drain = (Drain_Component){
        .amount = drain_amount
    };
}

void drain_ability(Unit* unit, const Drain_Component* component)
{
    health_component_drain_by_amount(&unit->health_component, component->amount);
}

void drain_component_text(const Drain_Component* component, String_Builder* string_builder)
{
    string_builder_append_string(string_builder,
                                 string_format(string_builder->allocator, "Drains health by %f",
                                               component->amount));
}






#endif //DRAIN_H
