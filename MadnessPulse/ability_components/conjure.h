#ifndef CONJURE_H
#define CONJURE_H




void conjure_component_create(Ability_Component* ac, Conjure_Type type)
{
    ac->type = Ability_Component_TYPE_CONJURE;

    ac->data.conjure =  (Conjure_Component){.conjure_type = type};
}


void conjure_component_ability(Unit* unit, Conjure_Component* component)
{
    // clear inventory, add the abilties, and then add the conjure type into the death list componentd
    /*
    unit_caster->InventoryManagerComponent->ClearBattleInventory();
    UnitCaster->InventoryManagerComponent->AddMultipleToBattleInventory(AbilitiesToAdd);

    UnitCaster->ConjureDeathListComponent->AddDeathCondition(ConjureType);

    ConjurePlayBack = UConjurePlayback::CreateConjurePlayBack(UnitCaster, ConjureType);
    */
}


void conjure_component_text(const Conjure_Component* component, String_Builder* string_builder)
{
    string_builder_append_string(string_builder,
                                 string_format(string_builder->allocator,
                                               "Conjure the demon of %s. Caster's abilities will be replaced by the conjured demons abilities.",
                                               Conjure_Type_enum_string[component->conjure_type]));
}


#endif
