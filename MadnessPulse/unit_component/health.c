#include "health.h"
#include "ability.h"
#include "logger.h"

#define MAX_HEALTH_DEFAULT 100
#define CURRENT_HEALTH_DEFAULT 100


Health_Component health_component_create()
{
    Health_Component health;
    health.current_health = CURRENT_HEALTH_DEFAULT;
    health.max_health = MAX_HEALTH_DEFAULT;
    health.max_health_limit = health.max_health * 2.0f;
    health.min_health_limit = -(health.max_health * 2.0f);

    health.death_animation_flag = false;
    health.revive_animation_flag = false;

    return health;
}

void health_component_init_default(Health_Component* health)
{
    health->current_health = CURRENT_HEALTH_DEFAULT;
    health->max_health = MAX_HEALTH_DEFAULT;
    health->max_health_limit = health->max_health * 2.0f;
    health->min_health_limit = -(health->max_health * 2.0f);

    health->death_animation_flag = false;
    health->revive_animation_flag = false;

};


bool health_component_is_dead(Health_Component* health_component)
{
    // true if less than zero
    return health_component->current_health <= 0;
}

bool IsAlive(Health_Component* health_component)
{
    return health_component->current_health > 0;
}

void CheckForDeathOrRevive(Unit* unit, Health_Component* health_component)
{
    /*
    if (health_component_is_dead(health_component) && unit->CharacterState == Character_State_Alive)
    {
        DEBUG("Unit is Dead as Fuck, setting death animation flag");
        health_component->death_animation_flag = true;
    }
    if (IsAlive(health_component) && unit->CharacterState == Character_State_Dead)
    {
        DEBUG("Unit is holy, setting revive animation flag");
        health_component->revive_animation_flag = true;
    }*/
}

float GetHealthPercent(Health_Component* health_component)
{
    if (health_component_is_dead(health_component)) return 0.0f;
    return health_component->current_health / health_component->max_health;
}

void LogHealth(Health_Component* health_component)
{
    DEBUG("Health: %f", health_component->current_health);
}

void health_component_clamp(Health_Component* health_component)
{
    // The lowest a health value can go is negative their max health, and for now the max is just max health

    //allowed to be at double your max health
    if (health_component->current_health >= health_component->max_health_limit)
    {
        health_component->current_health = health_component->max_health_limit;
    }
    //allowed to damage an enemy up to their max health in negative terms
    if (health_component->current_health <= -health_component->max_health)
    {
        health_component->current_health = -health_component->max_health;
    }
}

void TakeDamage(Health_Component* health_component, float DamageAmount)
{
    health_component->current_health -= DamageAmount;

    health_component_clamp(health_component);
    // CheckForDeathOrRevive();
}




void HealthToZero(Health_Component* health_component)
{
    health_component->current_health = 0;
    health_component_clamp(health_component);
    // CheckForDeathOrRevive(health_component);
}

void HealToFullHealth(Health_Component* health_component)
{
    health_component->current_health = health_component->max_health;

    health_component_clamp(health_component);
    // CheckForDeathOrRevive(health_component);
}

float HealToFullValueChange(Health_Component* health_component)
{
    return health_component->max_health * health_component->current_health;
}

void HealByAmount(Health_Component* health_component, float HealAmountValue)
{
    health_component->current_health += HealAmountValue;
    health_component_clamp(health_component);
    // CheckForDeathOrRevive(health_component);
}

void SetHealth(Health_Component* health_component, float HealValue)
{
    health_component->current_health = HealValue;
    health_component_clamp(health_component);
    // CheckForDeathOrRevive(health_component);
}

void HealByMultiplication(Health_Component* health_component, float HealMultiplyValue)
{
    health_component->current_health *= HealMultiplyValue;

    health_component_clamp(health_component);
    // CheckForDeathOrRevive(health_component);
}

void HealByPercent(Health_Component* health_component, float HealPercentValue)
{
    health_component->current_health += health_component->max_health * HealPercentValue;

    health_component_clamp(health_component);
    // CheckForDeathOrRevive(health_component);
}


float HealthPercentValueChange(Health_Component* health_component, float HealPercentValue)
{
    return health_component->max_health * HealPercentValue;
}


void DrainToFull(Health_Component* health_component)
{
    health_component->current_health = 0;
    health_component_clamp(health_component);
    // CheckForDeathOrRevive(health_component);
}

void DrainByAmount(Health_Component* health_component, float DrainAmountValue)
{
    health_component->current_health -= DrainAmountValue;
    health_component_clamp(health_component);

    // CheckForDeathOrRevive(health_component);
}

void DrainByPercent(Health_Component* health_component, float DrainPercentValue)
{
    health_component->current_health -= health_component->max_health * DrainPercentValue;
    health_component_clamp(health_component);
    // CheckForDeathOrRevive(health_component);
}


/* Utility Functions */

/** \param Percent Values between 0 - 1
 */
bool IsHealthBelowPercent(Health_Component* health_component, float Percent)
{
    float Value = GetHealthPercent(health_component);
    return Value <= Percent;
}

bool IsHealthAbovePercent(Health_Component* health_component, float Percent)
{
    float Value = GetHealthPercent(health_component);
    return Value >= Percent;
}


float HealthLostFromMax(Health_Component* health_component)
{
    if (health_component->max_health > health_component->current_health) return 0.0f;

    return health_component->max_health - health_component->current_health;
}


