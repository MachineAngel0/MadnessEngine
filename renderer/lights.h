
#ifndef LIGHTS_H 
#define LIGHTS_H

//TODO:
typedef struct Light
{
    vec3 position;
    vec3 colors;
    float ambient;
    float specular_strength;
}Light;

Light* light_init(Arena* a)
{
    Light* light = arena_alloc(a, sizeof(Light));
    light->position = vec3_zero();
    light->colors = vec3_zero();
    light->ambient = 1.0f;
    light->specular_strength = 1.0f;
    return light;
}


#endif //LIGHTS_H
