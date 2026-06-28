#ifndef ABILITIES_REGISTRY_H
#define ABILITIES_REGISTRY_H


#include "game_structs.h"
#include "hash_set.h"

//hold information about the ability, (ability_info), and what components are on the ability
//each unit keeps a list of the ability name, and how many of those abilities they have


Ability_Registry* ability_registry_init(Madness_Pulse_Game* game)
{
    Ability_Registry* ability_registry = allocator_alloc(&game->allocator, sizeof(Ability_Registry));
    //TODO: make this dynamic at some point
    ability_registry->registered_abilities = hash_set_init(sizeof(Ability_Name), 1000);
    return ability_registry;
}

void ability_registry_shutdown()
{
}



bool ability_registry_does_ability_exist(Ability_Registry* ability_registry, Ability_Name ability_name)
{
    return hash_set_contains(ability_registry->registered_abilities, &ability_name);
}


Ability* ability_registry_get_new_ability(Ability_Registry* ability_registry, Ability_Name ability_name)
{
    //TODO: have a sparse/packed array for quick traversal if i need it
    // return &ability_registry->ability_list[ability_registry->ability_count++];

    Ability* out_ability = &ability_registry->ability_list[ability_name];
    out_ability->id = ability_name;
    hash_set_insert(ability_registry->registered_abilities, &ability_name);
    return out_ability;
}

Ability_Info* ability_registry_get_new_ability_info(Ability_Registry* ability_registry, Ability_Name ability_name)
{
    //TODO: name the ability before giving it out
    return &ability_registry->ability_info[ability_name];
}


Ability* ability_registry_get_ability(Ability_Registry* ability_registry, Ability_Name name_query)
{
    if (ability_registry_does_ability_exist(ability_registry, name_query))
    {
        return &ability_registry->ability_list[name_query];
    }
    MASSERT(false);
    return NULL;
}

Ability_Info ability_registry_get_ability_info(Ability_Registry* ability_registry, Ability_Name name_query)
{
    return ability_registry->ability_info[name_query];
}

u32 ability_registry_get_ability_overflow_value(Ability_Registry* ability_registry, Ability_Name name_query)
{
    return overflow_value_type_lut[ability_registry->ability_info[name_query].overflow_value];
}

const char* ability_get_name(const Ability_Name name)
{
    return Ability_Name_enum_string[name];
}

String ability_get_name_string(const Ability_Name name)
{
    return STRING_STRLEN(Ability_Name_enum_string[name]);
}


#endif //ABILITIES_REGISTRY_H
