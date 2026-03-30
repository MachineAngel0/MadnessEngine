#ifndef ABILITIES_H
#define ABILITIES_H
#include <stdio.h>


typedef enum ability_name
{
    madness_sphere_name,
    madness_spear_name,


    max_names,
} ability_name;

typedef enum damage_type
{
    madness_damage_type,
    insanity_damage_type,

    max_damage_type
} damage_type;

typedef enum ability_type
{
    damage_ability_type,
    heal_ability_type,
    action_ability_type,

    max_ability_type
} ability_type;


typedef struct ability_base_component
{
    int id;
    int ActionsAvailable;
    void (*ability)(int id);
} ability_base_component;


typedef struct ability_vtable
{
    void (*ability[10])(int id);
} ability_vtable;


static ability_vtable vtable;

void damage(int id)
{
    printf("ABILITY DAMAGE ID IS: %d\n", id);
}

void heal(int id)
{
    printf("ABILITY heal ID IS: %d\n", id);
}


void ability_vtable_testing()
{
    //so both methods work
    vtable.ability[0] = damage;
    vtable.ability[1] = heal;


    ability_base_component damage_ability;
    damage_ability.id = 0;
    damage_ability.ability = damage;

    ability_base_component heal_ability;
    heal_ability.id = 1;
    heal_ability.ability = heal;

    damage_ability.ability(damage_ability.id);
    heal_ability.ability(heal_ability.id);

    vtable.ability[damage_ability.id](damage_ability.id);
    vtable.ability[heal_ability.id](heal_ability.id);
}


#endif
