#ifndef MP_REMOVER_H
#define MP_REMOVER_H


#include "../game_structs.h"
#include "mp.h"


void mp_remover_component_create(Ability_Component* ac, f32 amount)
{
    ac->type = Ability_Component_TYPE_MP_REMOVE;


    ac->data.mp_remove = (MP_Remover_Component){
        .amount = amount,
    };
}

void mp_remover_ability(Unit* unit, const MP_Remover_Component* component)
{
    mp_component_change_mp(unit, &unit->mp_component, -component->amount);
}

void mp_remover_component_text(const MP_Remover_Component* component, String_Builder* string_builder)
{
    string_builder_append_string(string_builder,
                                 string_format(string_builder->allocator, "remove %f mp from target",
                                               component->amount));
}


#endif
