#ifndef ABILITIES_H
#define ABILITIES_H

#include "game_structs.h"
#include "game_enums.h"
#include "heal.h"
#include "damage.h"
#include "str.h"

#define MAX_ABILITY_COMPONENTS 10

typedef enum Ability_Component_Type
{
    Ability_Component_TYPE_HEAL,
    Ability_Component_TYPE_DAMAGE,
    Ability_Component_TYPE_MP_CHANGE,
    Ability_Component_TYPE_AUGMENT_CHANGE,
    Ability_Component_TYPE_CHARGE,
    Ability_Component_TYPE_CONJURE,
    Ability_Component_TYPE_DRAIN,
    Ability_Component_TYPE_ACTION_CHANGE,
    Ability_Component_TYPE_ABILITY_CHANGE,
    Ability_Component_TYPE_RESISTANCE_CHANGE,
    Ability_Component_TYPE_STATUS_CHANGE,
    Ability_Component_TYPE_STATUS_THRESHOLD_CHANGE,
    Ability_Component_TYPE_MAX,
} Ability_Component_Type;

typedef void (*ability_effect)(Unit*, void*);


static ability_effect ability_vtable[Ability_Component_TYPE_MAX] = {
    [Ability_Component_TYPE_HEAL] = heal_ability,
    [Ability_Component_TYPE_DAMAGE] = damage_ability,
};

typedef struct Ability_Component
{
    Ability_Component_Type type;
    void* data;
} Ability_Component;

typedef struct Ability
{
    u32 id;
    Ability_Component components[MAX_ABILITY_COMPONENTS];
    u32 component_count;
} Ability;

typedef struct Ability_Info
{
    String ability_name; // = "Ability Not Named";
    String ability_text; // "Implement Text Please";
    String status_trigger_text; // = "NA";

    String lore_text; //= "NA";

    Fusion_Type fusion_type; // = EFusionType::ECS_Fire;

    //every ability only gets one normal attack, primary tag, primarily used to determine the icon type
    Ability_Type primary_ability_type; // = EAbilityType::ECS_Physical;

    Ability_Target_Type ability_target_type;
    Target_Can_Affect targets_can_affect;

    int ability_action_cost; // = 1;
    float mp_cost; // = -1.0f;
} Ability_Info;

void ability_add_component(Ability* ability, Ability_Component* component)
{
    ability->components[ability->component_count] = *component;
    ability->component_count++;
}

void ability_process(Ability* ability, Unit* unit)
{
    for (u32 i = 0; i < ability->component_count; i++)
    {
        ability_vtable[ability->components[i].type](unit, ability->components[i].data);
    }
}


void ability_testing()
{
    Unit unit;
    // unit.health_component = health_component_create();
    unit.health_component = (Health_Component){
        .current_health = 100, .max_health = 100, .max_health_limit = 100, .min_health_limit = 0,
        .death_animation_flag = false, .revive_animation_flag = false
    };

    Ability ability;
    memset(&ability, 0, sizeof(ability));

    Heal_Ability heal_component = heal_ability_create(Heal_Types_HealAmount, 10, false);

    Damage_Ability damage_component = damage_ability_create(15);

    Ability_Component a1 = {
        .type = Ability_Component_TYPE_HEAL, .data = &heal_component
    };
    Ability_Component a2 = (Ability_Component){
        .type = Ability_Component_TYPE_DAMAGE, .data = &damage_component
    };

    ability_add_component(&ability, &a1);
    ability_add_component(&ability, &a2);

    ability_process(&ability, &unit);
}


#endif
