#ifndef MP_FULL_H
#define MP_FULL_H


#include "../game_structs.h"
#include "mp.h"


void mo_component_create(Ability_Component* ac)
{
    ac->type = Ability_Component_TYPE_MP_FULL;


    ac->data.mp_full = (MP_Full_Component){
        .amount = true,
    };
}

void mp_full_ability(Unit* unit, const MP_Full_Component* component)
{
    mp_component_mp_to_full(&unit->mp_component);
}

void mo_full_component_text(const MP_Full_Component* component, String_Builder* string_builder)
{
    string_builder_append_string(string_builder, &(STRING("Set targets MP to full")));
}


#endif
