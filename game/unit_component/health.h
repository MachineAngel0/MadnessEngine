#ifndef HEALTH_H
#define HEALTH_H
#include "maths/math_lib.h"


typedef struct health_component
{
    float current_health;
    float max_health;
    //An upper limit so that health does not go past a certain point like 9999
    float max_health_limit;
    float min_health_limit;
}health_component;

void health_component_init_default(health_component* health)
{
    health->current_health = 100;
    health->max_health = 100;
    health->max_health_limit = health->max_health * 2.0f;
    health->min_health_limit = -(health->max_health * 2.0f);

}

void health_component_take_damage(health_component* health, const float damage)
{
    //NOTE: not a functional/pure function, even though it could be
    health->current_health -= damage;
    health->current_health = clamp_float( health->current_health, health->min_health_limit, health->max_health_limit);
}


#endif