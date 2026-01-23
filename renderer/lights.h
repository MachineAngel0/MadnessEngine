#ifndef LIGHTS_H
#define LIGHTS_H

//TODO: light buffer, and add it to the global uniform buffer
typedef struct Point_Light
{
    vec4 position;
    vec4 color;
    float diffuse;
    float ambient;
    float specular;
    float intensity;
    float radius;
} Point_Light;

typedef struct Directional_Light
{
    vec3 direction;
    vec3 color;
    float diffuse;
    float specular;
} Directional_Light;

typedef struct Spot_Light
{
    vec3 position;
} Spot_Light;

typedef struct Area_Light
{
    vec3 position;
} Area_Light;

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


#endif //LIGHTS_H
