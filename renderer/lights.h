
#ifndef LIGHTS_H 
#define LIGHTS_H

//TODO: light buffer, and add it to the global uniform buffer
typedef struct Light
{
    vec3 position;
    vec3 color;
    float ambient;
    float specular_strength;
}Light;

Light* light_init(Arena* a)
{
    Light* light = arena_alloc(a, sizeof(Light));
    light->position = vec3_zero();
    light->color = vec3_zero();
    light->ambient = 1.0f;
    light->specular_strength = 1.0f;
    return light;
}






#endif //LIGHTS_H
