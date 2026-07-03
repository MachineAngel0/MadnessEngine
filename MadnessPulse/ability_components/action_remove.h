#ifndef ACTION_REMOVE_H
#define ACTION_REMOVE_H


void action_remove_component_create(Ability_Component* ac, const u32 amount)
{
    ac->type = Ability_Component_TYPE_ACTION_REMOVE;


    ac->data.action_remove = (Action_Remove_Component){
        .amount = amount,
    };
}

void action_remove_ability(Unit* unit, const Action_Remove_Component* component)
{
    action_component_decrease_actions(&unit->action_component, component->amount);
}

void action_remove_component_text(const Action_Remove_Component* component, String_Builder* string_builder)
{
    string_builder_append_string(string_builder,
                                 string_format(string_builder->allocator, "remove %d action",
                                               component->amount));
}

#endif
