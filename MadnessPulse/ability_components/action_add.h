
#ifndef ACTION_ADD_H
#define ACTION_ADD_H


void action_add_component_create(Ability_Component* ac, const u32 amount)
{
    ac->type = Ability_Component_TYPE_ACTION_ADD;


    ac->data.action_add = (Action_Add_Component){
        .amount = amount,
    };
}

void action_add_ability(Unit* unit, const Action_Add_Component* component)
{
    action_component_increase_actions(&unit->action_component, component->amount);
}

void action_add_component_text(const Action_Add_Component* component, String_Builder* string_builder)
{
    string_builder_append_string(string_builder,
                                 string_format(string_builder->allocator, "Add %d action",
                                               component->amount));
}

#endif
