#include "health.h"
#include "ability.h"
#include "logger.h"



Health_Component health_component_create()
{
    Health_Component health;
    health.current_health = CURRENT_HEALTH_DEFAULT;
    health.max_health = MAX_HEALTH_DEFAULT;
    health.max_health_limit = health.max_health * 2.0f;
    health.min_health_limit = -(health.max_health * 2.0f);



    return health;
}

void health_component_init_default(Health_Component* health)
{
    health->current_health = CURRENT_HEALTH_DEFAULT;
    health->max_health = MAX_HEALTH_DEFAULT;
    health->max_health_limit = health->max_health * 2.0f;
    health->min_health_limit = -(health->max_health * 2.0f);

}


bool health_component_is_dead(Health_Component* health_component)
{
    // true if less than zero
    return health_component->current_health <= 0;
}

bool health_component_is_alive(Health_Component* health_component)
{
    return health_component->current_health > 0;
}

void health_component_check_for_death_or_revive(Unit* unit, Health_Component* health_component)
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

float health_component_get_health_percent(Health_Component* health_component)
{
    if (health_component_is_dead(health_component)) return 0.0f;
    return health_component->current_health / health_component->max_health;
}

void health_component_log_health(Health_Component* health_component)
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

void health_component_take_damage(Health_Component* health_component, float DamageAmount)
{
    health_component->current_health -= DamageAmount;

    health_component_clamp(health_component);
    // CheckForDeathOrRevive();
}




void health_component_health_to_zero(Health_Component* health_component)
{
    health_component->current_health = 0;
    health_component_clamp(health_component);
    // CheckForDeathOrRevive(health_component);
}

void health_component_heal_to_full(Health_Component* health_component)
{
    health_component->current_health = health_component->max_health;

    health_component_clamp(health_component);
    // CheckForDeathOrRevive(health_component);
}

float health_component_heal_to_full_value_change(Health_Component* health_component)
{
    return health_component->max_health * health_component->current_health;
}

void heal_by_amount(Health_Component* health_component, float HealAmountValue)
{
    health_component->current_health += HealAmountValue;
    health_component_clamp(health_component);
    // CheckForDeathOrRevive(health_component);
}

void health_component_set_health(Health_Component* health_component, float HealValue)
{
    health_component->current_health = HealValue;
    health_component_clamp(health_component);
    // CheckForDeathOrRevive(health_component);
}

void health_component_heal_by_multiplication(Health_Component* health_component, float HealMultiplyValue)
{
    health_component->current_health *= HealMultiplyValue;

    health_component_clamp(health_component);
    // CheckForDeathOrRevive(health_component);
}

void health_component_heal_by_percent(Health_Component* health_component, float HealPercentValue)
{
    health_component->current_health += health_component->max_health * HealPercentValue;

    health_component_clamp(health_component);
    // CheckForDeathOrRevive(health_component);
}


float health_component_percent_value_change(Health_Component* health_component, float HealPercentValue)
{
    return health_component->max_health * HealPercentValue;
}


void health_component_drain_to_full(Health_Component* health_component)
{
    health_component->current_health = 0;
    health_component_clamp(health_component);
    // CheckForDeathOrRevive(health_component);
}

void health_component_drain_by_amount(Health_Component* health_component, float DrainAmountValue)
{
    health_component->current_health -= DrainAmountValue;
    health_component_clamp(health_component);

    // CheckForDeathOrRevive(health_component);
}

void health_component_drain_by_percent(Health_Component* health_component, float DrainPercentValue)
{
    health_component->current_health -= health_component->max_health * DrainPercentValue;
    health_component_clamp(health_component);
    // CheckForDeathOrRevive(health_component);
}


/* Utility Functions */

/** \param Percent Values between 0 - 1
 */
bool health_component_is_health_below_percent(Health_Component* health_component, float Percent)
{
    float Value = health_component_get_health_percent(health_component);
    return Value <= Percent;
}

bool health_component_is_health_above_percent(Health_Component* health_component, float Percent)
{
    float Value = health_component_get_health_percent(health_component);
    return Value >= Percent;
}


float health_component_get_health_lost_from_max(Health_Component* health_component)
{
    if (health_component->max_health > health_component->current_health) return 0.0f;

    return health_component->max_health - health_component->current_health;
}


