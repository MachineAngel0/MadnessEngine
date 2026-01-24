#ifndef LIGHTS_H
#define LIGHTS_H

#include "vulkan_types.h"

//TODO: light buffer, and add it to the global uniform buffer


void point_light_init(Point_Light* light)
{
    light->position = vec4_zero();
    light->color = vec4_zero();

    light->diffuse = 1.0f;
    light->ambient = 1.0f;
    light->specular = 1.0f;

    light->intensity = 1.0f;
    light->radius = 1.0f;
}


void directional_light_init(Directional_Light* light)
{
    light->direction = vec3_zero();
    light->color = vec3_zero();

    light->diffuse = 1.0f;
    light->specular = 1.0f;
}


Light_System* light_system_init(renderer* renderer)
{
    Light_System* out_light_system = arena_alloc(&renderer->arena, sizeof(Light_System));

    //TODO: TEMP FOR NOW
    out_light_system->directional_light_count = 1;
    out_light_system->point_light_count = 1;

    out_light_system->point_lights =
        arena_alloc(&renderer->arena, sizeof(Point_Light) * out_light_system->point_light_count);

    out_light_system->directional_lights =
        arena_alloc(&renderer->arena, sizeof(Directional_Light) * out_light_system->directional_light_count);


    for (int point_light_idx = 0; point_light_idx < out_light_system->point_light_count; point_light_idx++)
    {
        point_light_init(&out_light_system->point_lights[point_light_idx]);
    }
    for (int directional_light_idx = 0; directional_light_idx < out_light_system->directional_light_count; directional_light_idx++)
    {
        directional_light_init(&out_light_system->directional_lights[directional_light_idx]);
    }


    return out_light_system;
}


#endif //LIGHTS_H
