#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H


#include "defines.h"
#include "memory_system.h"
#include "resource_types.h"
#include "maths/math_types.h"

//TODO: move out when its appropriate
typedef struct Render_Packet_Particle
{
    bool nothing;
} Render_Packet_Particle;


typedef struct Particle_Animation_vec3
{
    float time;
    vec3s value;
} Particle_Animation_vec3;

typedef struct Particle_Animation_vec2
{
    float time;
    vec2s value;
} Particle_Animation_vec2;

typedef struct Particle_Animation_float
{
    float time;
    float value;
} Particle_Animation_float;


// i guess we just fat struct the emitter for now
typedef struct Particle_Emitter
{
    vec3s Spawn_area;
    Particle_Animation_vec3 Color_over_lifetime;
    vec3s color_variance;
    Particle_Animation_float Scale_over_lifetime;
    float Scale_variance;
    Particle_Animation_float Emitter_wide_velocity_over_time;
    vec3s Per_particle_velocity_variance;
    vec3s Gravity;
    Particle_Animation_vec3 Emitter_wide_rotational_velocity_over_time;
    vec3s Per_particle_rotation_variance;
    vec3s Per_particle_rotation_rate_variance;
    Particle_Animation_float Emission_per_second;
    float Emitter_lifetime;
    float Particle_lifetime;
    float Particle_lifetime_variance;
    int Blend_mode;
    //(either 3D or 2D billboard)
    vec3s Facing_type_3D;
    vec3s Facing_type_2D;
    float Depth_softening;
    // Material intensity over lifetime (AnimatedValue);
    // Material intensity variance;
    Particle_Animation_vec2 UV_scrolling;
    vec2s UV_offsets;
    vec2s uv_variance;
    Texture_Handle Textures;
    // Animated Textures
    u32 Maximum_Particles;
    u32 Minimum_Particles;
    bool loop;
    bool toggle_visibility;
} Particle_Emitter;

typedef struct Particle
{
    vec3s position;
    vec3s rotation;
    vec3s scale;

    float life_left;


    //draw data?
    //Texture
    Texture_Handle Textures;
    vec2s tex_offset;
} Particle;

typedef struct Particle_Mesh
{
    vec3s position;
    vec3s rotation;
    vec3s scale;

    float lifetime_left;


    //draw data?
    //Texture
    Mesh_Asset_Handle Mesh;
    Texture_Handle Textures;
    vec2s tex_offset;
} Particle_Mesh;

typedef struct Particle_System
{
    //memory, arena, frame_arena

    //individual particles
    //animation data

    //vertex/index buffer

    Particle* particles;
    u32 particles_count;
} Particle_System;


#define PARTICLE_COUNT 1000

void particle_system_init(Memory_System* memory_system)
{
    Particle_System* ps = memory_system_alloc(memory_system, sizeof(Particle_System), MEMORY_SUBSYSTEM_PARTICLE);

    ps->particles = memory_system_alloc(memory_system, sizeof(Particle) * PARTICLE_COUNT, MEMORY_SUBSYSTEM_PARTICLE);
}

void particle_system_destroy(Particle_System* ps)
{
}

//TODO: lets just have particles move up
void particle_system_update(Particle_System* ps, float dt)
{


}

void particle_system_generate_render_packet(Particle_System* ps)
{

}


void particle_system_fire_forget()
{

}




#endif //PARTICLE_SYSTEM_H
