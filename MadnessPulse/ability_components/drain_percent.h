#ifndef DRAIN_PERCENT_H
#define DRAIN_PERCENT_H


#include "game_structs.h"
#include "health.h"


void drain_percent_component_create(Ability_Component* ac, const float percent)
{
    ac->type = Ability_Component_TYPE_DRAIN_PERCENT;


    ac->data.drain_percent = (Drain_Percent_Component){
        .percent = percent,
    };
}

void drain_percent_ability(Unit* unit, const Drain_Percent_Component* component)
{
    health_component_drain_by_percent(&unit->health_component, component->percent);

}

void drain_percent_component_text(const Drain_Percent_Component* component, String_Builder* string_builder)
{
    string_builder_append_string(string_builder,
                                 string_format(string_builder->allocator, "Drain by %f percent of your health",
                                               component->percent));
}






#endif //DRAIN_PERCENT_H
