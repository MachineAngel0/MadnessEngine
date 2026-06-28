#ifndef HEALTH_COMPONENT_H
#define HEALTH_COMPONENT_H



#include "game_structs.h"

Health_Component health_component_create();
void health_component_init_default(Health_Component* health);



//Changes State
void health_component_clamp(Health_Component* health_component);


//TODO: death and revive flags
void health_component_check_for_death_or_revive(Unit* unit, Health_Component* health_component);

float health_component_get_health_percent(Health_Component* health_component);


void heal_by_amount(Health_Component* health_component, float HealAmountValue);


void health_component_take_damage(Health_Component* health_component, float DamageAmount);

void health_component_health_to_zero(Health_Component* health_component);

void health_component_heal_to_full(Health_Component* health_component);

float health_component_heal_to_full_value_change(Health_Component* health_component);


void health_component_set_health(Health_Component* health_component, float HealValue);

void health_component_heal_by_percent(Health_Component* health_component, float HealPercentValue);

float health_component_percent_value_change(Health_Component* health_component, float HealPercentValue);


void health_component_drain_to_full(Health_Component* health_component);

void health_component_drain_by_amount(Health_Component* health_component, float DrainAmountValue);

void health_component_drain_by_percent(Health_Component* health_component, float DrainPercentValue);


/* Utility Functions */

bool health_component_is_dead(Health_Component* health_component);

bool health_component_is_alive(Health_Component* health_component);

bool health_component_is_health_below_percent(Health_Component* health_component, float Percent);

bool health_component_is_health_above_percent(Health_Component* health_component, float Percent);

float health_component_get_health_lost_from_max(Health_Component* health_component);

//
void health_component_log_health(Health_Component* health_component);



#endif
