#ifndef LIGHTS_H
#define LIGHTS_H

#include "vulkan_types.h"

//TODO: light buffer, and add it to the global uniform buffer


Point_Light* point_light_init(Arena* a)
{
    Point_Light* light = arena_alloc(a, sizeof(Point_Light));
    light->position = vec4_zero();
    light->color = vec4_zero();

    light->diffuse = 1.0f;
    light->ambient = 1.0f;
    light->specular = 1.0f;

    light->intensity = 1.0f;
    light->radius = 1.0f;
    return light;
}


Directional_Light* directional_light_init(Arena* a)
{
    Directional_Light* light = arena_alloc(a, sizeof(Directional_Light));
    light->direction = vec3_zero();
    light->color = vec3_zero();

    light->diffuse = 1.0f;
    light->specular = 1.0f;

    return light;
}


#endif //LIGHTS_H
