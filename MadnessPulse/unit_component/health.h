#ifndef HEALTH_COMPONENT_H
#define HEALTH_COMPONENT_H



#include "game_structs.h"

Health_Component health_component_create();
void health_component_init_default(Health_Component* health);


bool health_component_is_dead(Health_Component* health_component);

bool IsAlive(Health_Component* health_component);

void CheckForDeathOrRevive(Unit* unit, Health_Component* health_component);

float GetHealthPercent(Health_Component* health_component);

void LogHealth(Health_Component* health_component);

void health_component_clamp(Health_Component* health_component);

void TakeDamage(Health_Component* health_component, float DamageAmount);

void HealthToZero(Health_Component* health_component);

void HealToFullHealth(Health_Component* health_component);

float HealToFullValueChange(Health_Component* health_component);

void HealByAmount(Health_Component* health_component, float HealAmountValue);

void SetHealth(Health_Component* health_component, float HealValue);

void HealByMultiplication(Health_Component* health_component, float HealMultiplyValue);

void HealByPercent(Health_Component* health_component, float HealPercentValue);

float HealthPercentValueChange(Health_Component* health_component, float HealPercentValue);


void DrainToFull(Health_Component* health_component);

void DrainByAmount(Health_Component* health_component, float DrainAmountValue);

void DrainByPercent(Health_Component* health_component, float DrainPercentValue);


/* Utility Functions */

/** \param Percent Values between 0 - 1
 */
bool IsHealthBelowPercent(Health_Component* health_component, float Percent);

bool IsHealthAbovePercent(Health_Component* health_component, float Percent);

float HealthLostFromMax(Health_Component* health_component);


#endif
