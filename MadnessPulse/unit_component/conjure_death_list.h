#ifndef CONJURE_DEATH_LIST_COMPONENT_H
#define CONJURE_DEATH_LIST_COMPONENT_H

#include "game_enums.h"
#include "game_structs.h"


void conjure_death_list_create_default(Conjure_List_Component* conjure_death_list_component)
{
    for (u32 i = 0; i < Conjure_Type_Max; ++i)
    {
        conjure_death_list_component->active_conjure_list[i]; // tells us if a conjure type is active
    }
    conjure_death_list_component->kill_player;
    // this could potentially be a flag on the unit ex: bool Conjure_Kill_Flag)
}

void conjure_death_list_add_death_condition(Conjure_List_Component* conjure_death_list_component,
                                            const Conjure_Type ConjureTypeToAdd)
{
    conjure_death_list_component->active_conjure_list[ConjureTypeToAdd] = true;
}

//Kill player is to be set when a death condition is triggered, will be reset right after the player is killed
void CheckKillPlayerCondition(Conjure_List_Component* conjure_death_list_component, Unit* Caster)
{
    if (!conjure_death_list_component->kill_player)
        return;
    // Caster->HealthComponent->HealthToZero();
    conjure_death_list_component->kill_player = false;
}

bool conjure_is_active(Conjure_List_Component* conjure_death_list_component, const Conjure_Type conjure_type)
{
    return conjure_death_list_component->active_conjure_list[conjure_type];
}

//TODO: i dont need to solve it rn, but we should have some way to check if one of these demons is active,
// and then if it needs to kill you because of what you did
void conjure_ice_demon_check();
void conjure_miracle_demon_check();
void conjure_damage_demon_check();


#endif
