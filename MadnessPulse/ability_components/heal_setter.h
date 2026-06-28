#ifndef HEAL_SETTER_H
#define HEAL_SETTER_H


#include "game_structs.h"
#include "health.h"


void health_setter_component_create(Ability_Component* ac, const float health_to_set)
{
    ac->type = Ability_Component_TYPE_HEAL;
    ac->data.health_setter = (Health_Setter_Component){
        .health_to_set = health_to_set,
    };
}

void health_setter_ability(Unit* unit, const Health_Setter_Component* component)
{
    health_component_set_health(&unit->health_component, component->health_to_set);
}

void health_setter_ability_text(const Health_Setter_Component* health_setter_component, String_Builder* string_builder)
{
    string_builder_append_string(string_builder,
                                 string_format(string_builder->allocator, "Set target health to  %f",
                                               health_setter_component->health_to_set));
}



#endif //HEAL_SETTER_H
