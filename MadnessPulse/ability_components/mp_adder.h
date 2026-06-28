#ifndef MP_ADDER_H
#define MP_ADDER_H


#include "game_structs.h"
#include "mp.h"


void mp_adder_component_create(Ability_Component* ac, f32 amount)
{
    ac->type = Ability_Component_TYPE_MP_ADD;

    ac->data.mp_add = (MP_Adder_Component){
        .amount = amount,
    };
}

void mp_adder_ability(Unit* unit, const MP_Adder_Component* component)
{
    mp_component_change_mp(unit, &unit->mp_component, component->amount);
}

void mp_adder_component_text(const MP_Adder_Component* component, String_Builder* string_builder)
{
    string_builder_append_string(string_builder,
                                 string_format(string_builder->allocator, "add %f mp to target",
                                               component->amount));
}


#endif
