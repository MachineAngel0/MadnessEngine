#ifndef ABILITY_ADDER_H
#define ABILITY_ADDER_H




void ability_adder_component_create(Ability_Component* ac, Ability_Name name, u32 count)
{
    ac->type = Ability_Component_TYPE_ABILITY_ADDER;


    ac->data.ability_add = (Ability_Adder_Component){
        .abilities_to_add = name,
        .count = count,
    };
}

void ability_adder_ability(Unit* unit, const Ability_Adder_Component* component)
{
    battle_inventory_add_ability(&unit->battle_inventory_component, component->abilities_to_add, component->count);
}

void ability_adder_component_text(const Ability_Adder_Component* component, String_Builder* string_builder)
{
    string_builder_append_string(string_builder,
                                 string_format(string_builder->allocator, "Add %d abilites with the name %s",
                                               component->count,
                                               Ability_Name_enum_string[component->abilities_to_add]));
}


#endif
