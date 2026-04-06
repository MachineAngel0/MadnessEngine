#ifndef ABILITIES_H
#define ABILITIES_H
#include <stdio.h>

#include "game_structs.h"
#include "game_enums.h"
#include "health.h"
#include "str.h"

#define MAX_ABILITY_COMPONENTS 10


typedef struct Ability_Component
{
    void (*effect)(Unit*, void*);
    void* data;
} Ability_Component;

typedef struct Ability_Info
{
    u32 id;

    String ability_name; // = "Ability Not Named";
    String ability_text; // "Implement Text Please";
    String status_trigger_text; // = "NA";

    String lore_text; //= "NA";

    Fusion_Type fusion_type; // = EFusionType::ECS_Fire;

    //every ability only gets one normal attack, primary tag, primarly used to determine the icon type
    Ability_Type primary_ability_type; // = EAbilityType::ECS_Physical;

    Ability_Target_Type ability_target_type;
    Target_Can_Affect targets_can_affect;

    int ability_action_cost; // = 1;
    float mp_cost; // = -1.0f;


} Ability_Info;


typedef struct Ability
{
    u32 id;
    Ability_Component components[MAX_ABILITY_COMPONENTS];
    u32 component_count;
}Ability;



void ability_add_component(Ability* ability, Ability_Component* component)
{
    ability->components[ability->component_count] = *component;
    ability->component_count++;
}

void ability_process(Ability* ability, Unit* unit)
{
    for (u32 i = 0; i < ability->component_count; i++)
    {
        ability->components[i].effect(unit, ability->components[i].data);
    }
}

void ability_testing()
{
    Unit unit;
    unit.HealthComponent = health_component_create();

    Ability ability;
    memset(&ability, 0, sizeof(ability));

    Health_Component heal_component;
    heal_component.heal_amount = 10;


    Damage_Component damage_component;
    damage_component.damage = 15;

    Ability_Component a1 = {
        .effect = heal_c, .data = &heal_component
    };
    Ability_Component a2 = (Ability_Component){
        .effect = damage_c, .data = &damage_component
    }; // TODO: this should be damage_component

    ability_add_component(&ability, &a1);
    ability_add_component(&ability, &a2);

    ability_process(&ability, &unit);
}


#endif
