#ifndef HEAL_PERCENT_H
#define HEAL_PERCENT_H


#include "game_structs.h"
#include "health.h"


void heal_percent_component_create(Ability_Component* ac, const float heal_percent)
{
    ac->type = Ability_Component_TYPE_HEAL;

    ac->data.heal_percent = (Heal_Percent_Component){
        .heal_percent = heal_percent
    };
}


void heal_percent_ability(Unit* unit, const Heal_Percent_Component* component)
{
    health_component_heal_by_percent(&unit->health_component, component->heal_percent);
}

void heal_percent_component_text(const Heal_Percent_Component* component, String_Builder* string_builder)
{
    string_builder_append_string(string_builder,
                                 string_format(string_builder->allocator, "Heal target by %f percent of max health",
                                               component->heal_percent));
}

#endif //HEAL_PERCENT_H
