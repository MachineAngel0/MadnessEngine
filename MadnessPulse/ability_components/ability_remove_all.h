#ifndef ABILITY_REMOVE_ALL_H
#define ABILITY_REMOVE_ALL_H




void ability_remove_all_component_create(Ability_Component* ac, Ability_Name name)
{
    ac->type = Ability_Component_TYPE_ABILITY_REMOVE_ALL;


    ac->data.ability_remove_all = (Ability_Remove_All_Component){
      .ability_to_remove = name,
    };
}

void ability_remove_all_ability(Unit* unit, const Ability_Remove_All_Component* component)
{
    battle_inventory_remove_all_ability(&unit->battle_inventory_component, component->ability_to_remove);
}

void ability_remover_all_component_text(const Ability_Remove_All_Component* component, String_Builder* string_builder)
{
    string_builder_append_string(string_builder,
                                 string_format(string_builder->allocator, "Remove all abilites with the name %s",
                                 Ability_Name_enum_string[component->ability_to_remove]));
}






#endif
