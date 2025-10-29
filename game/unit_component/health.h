#ifndef HEALTH_H
#define HEALTH_H


typedef struct health_component
{
    float CurrentHealth;
    float MaxHealth;
    //An upper limit so that health does not go past a certain point like 9999
    float MaxHealthLimit;
}health_component;

void health_component_init_default(health_component health)
{
    health.CurrentHealth = 100;
    health.MaxHealth = 100;
    health.MaxHealthLimit = health.MaxHealth * 2.0f;
}




#endif