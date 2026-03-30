#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H


#include "defines.h"
#include "vulkan_types.h"
#include "maths/math_types.h"

#define Animation_Struct_Create(type)\
typedef struct Animation_##type{ \
float time; \
type value; \
}Animation_##type;

Animation_Struct_Create(vec2);

typedef struct Particle_Animation_Float
{
    float time;
    float value;
}Particle_Animation_Float;
typedef struct Particle_Animation_vec3
{
    float time;
    vec3 value;
}Particle_Animation_vec3;
typedef struct Particle_Animation_vec2
{
    float time;
    vec2 value;
}Particle_Animation_vec2;

typedef struct Particle_Emitter
{
    vec3 Spawn_area;
    Particle_Animation_vec3 Color_over_lifetime;
    vec3 color_variance;
    Particle_Animation_Float Scale_over_lifetime;
    float Scale_variance;
    Particle_Animation_Float Emitter_wide_velocity_over_time;
    vec3 Per_particle_velocity_variance;
    vec3 Gravity;
    Particle_Animation_vec3 Emitter_wide_rotational_velocity_over_time;
    vec3 Per_particle_rotation_variance;
    vec3 Per_particle_rotation_rate_variance;
    Particle_Animation_Float Emission_per_second;
    float Emitter_lifetime;
    float Particle_lifetime;
    float Particle_lifetime_variance;
    int Blend_mode;
    //(either 3D or 2D billboard)
    vec3 Facing_type_3D;
    vec3 Facing_type_2D;
    float Depth_softening;
    // Material intensity over lifetime (AnimatedValue);
    // Material intensity variance;
    Particle_Animation_vec2 UV_scrolling;
    vec2 UV_offsets;
    vec2 uv_variance;
    Texture_Handle Textures;
    // Animated Textures
    u32 Maximum_Particles;
    u32 Minimum_Particles;
    bool loop;
    bool toggle_visibility;
}Particle_Emitter;

typedef struct Particle
{
    vec3 position;
    vec3 rotation;
    vec3 scale;

    float lifetime_left;


    //draw data?
    //Texture
    vec2 tex_offset;
}Particle;


typedef  struct Particle_System
{
    //memory, arena, frame_arena

    //individual particles
    //animation data

    //vertex/index buffer


}Particle_System;



#endif //PARTICLE_SYSTEM_H