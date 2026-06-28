#ifndef CHARGE_H
#define CHARGE_H


#include "charge_list.h"
#include "game_structs.h"


void charge_component_create(Ability_Component* ac, const Charge_State charge_state)
{
    ac->type = Ability_Component_TYPE_CHARGE;

    if (charge_state == Charge_State_MAX)
    {
        MASSERT(false);
    }

    ac->data.charge = (Charge_Component){
        .charge_state = charge_state,
    };
}

void charge_ability(Unit* unit, const Charge_Component* component)
{
    charge_list_component_add_charge_states(&unit->charge_list_component, component->charge_state);
}

void charge_component_text(const Charge_Component* component, String_Builder* string_builder)
{
    switch (component->charge_state)
    {
    case Charge_State_Charge:
        string_builder_append_string(string_builder,
                                     string_format(string_builder->allocator,
                                                   "Gain a %d multiplier based on the type of ability used",
                                                   Charge_Percent));
        break;
    case Charge_State_HighCharge:
        string_builder_append_string(string_builder,
                                     string_format(string_builder->allocator,
                                                   "Gain a %d multiplier based on the type of ability used",
                                                   High_Charge_Percent));
        break;
    case Charge_State_MAX:
        break;
    }
}


#endif
