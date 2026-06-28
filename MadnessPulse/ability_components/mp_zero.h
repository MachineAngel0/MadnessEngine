#ifndef MP_ZERO_H
#define MP_ZERO_H


#include "game_structs.h"
#include "mp.h"


void mp_zero_component_create(Ability_Component* ac)
{
    ac->type = Ability_Component_TYPE_MP_ZERO;


    ac->data.mp_zero = (MP_Zero_Component){
        .amount = true,
    };
}

void mp_zero_ability(Unit* unit, const MP_Zero_Component* component)
{
    mp_component_mp_to_zero(&unit->mp_component);
}

void mp_zero_component_text(const MP_Zero_Component* component, String_Builder* string_builder)
{
    string_builder_append_string(string_builder, &(STRING("Set Target's MP To Zero")));
}


#endif
