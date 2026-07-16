#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H



#include "resource_types.h"


#define PARTICLE_COUNT 1000


Particle_System* particle_system_init(Memory_System* memory_system, Resource_System* resource_system)
{
    Particle_System* ps = memory_system_alloc(memory_system, sizeof(Particle_System), MEMORY_SUBSYSTEM_PARTICLE);

    ps->particles = memory_system_alloc(memory_system, sizeof(Particle) * PARTICLE_COUNT, MEMORY_SUBSYSTEM_PARTICLE);
    ps->particles_count = PARTICLE_COUNT;


    for (u32 i = 0; i < ps->particles_count; i++)
    {
        texture_system_load_texture(resource_system->texture_system, "../z_assets/textures/test_particle.png",  &ps->particles[i].texture_handle);
         ps->particles[i].scale = (vec2s){10.0f,10.0f};
    }





    return ps;
}

void particle_system_shutdown(Particle_System* ps, Memory_System* memory_system)
{
    //TODO:
}

//TODO: lets just have particles move up
void particle_system_update(Particle_System* ps, float dt)
{
    for (u32 i = 0; i < ps->particles_count; i++)
    {
        // ps->particles[i].position.x += 1;
        ps->particles[i].position.y += sinf(1.f) * dt;
        // ps->particles[i].position.z += 1;
    }

}

Render_Packet_Particle particle_system_generate_render_packet(Particle_System* ps)
{
     return (Render_Packet_Particle){
        .particles = ps->particles,
        .particle_count = ps->particles_count,
    };
}

void particle_system_fire_forget()
{

}




#endif //PARTICLE_SYSTEM_H
