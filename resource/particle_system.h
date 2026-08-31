#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include "asset_system.h"
#include "resource_types.h"

#define PARTICLE_COUNT 1000
#define EMITTER_COUNT 1


//requirements:
// works with the transform system so it can be moved
// threadable for performance gains
// one giant pool of particles
// behavior for emitting, burst spawning,
//


//future requirements:
// multithread or compute updates on the particle
// collision handling (do it with compute for sure)
//


//TODO: forward declare remove later
void particle_system_emitter_basic_update(Particle_System* ps, Particle_Emitter* emitter, float dt);
void particle_system_emitter_spawn(Particle_System* ps, Particle_Emitter* emitter);


Particle_System* particle_system_init(Asset_System* resource_system, Memory_System* memory_system)
{
    Particle_System* ps = memory_system_alloc(memory_system, sizeof(Particle_System), MEMORY_SUBSYSTEM_PARTICLE);

    u64 heap_alloc_mem_size = MB(1);
    ps->heap_allocator = memory_system_heap_allocator_create(memory_system, heap_alloc_mem_size,
                                                             MEMORY_SUBSYSTEM_PARTICLE);


    ps->particles_count = PARTICLE_COUNT;
    ps->dead_particles_count = PARTICLE_COUNT;
    ps->particles.pos_x = memory_system_alloc(memory_system, sizeof(f32) * ps->particles_count,
                                              MEMORY_SUBSYSTEM_PARTICLE);
    ps->particles.pos_y = memory_system_alloc(memory_system, sizeof(f32) * ps->particles_count,
                                              MEMORY_SUBSYSTEM_PARTICLE);
    ps->particles.pos_z = memory_system_alloc(memory_system, sizeof(f32) * ps->particles_count,
                                              MEMORY_SUBSYSTEM_PARTICLE);

    ps->particles.rot_x = memory_system_alloc(memory_system, sizeof(f32) * ps->particles_count,
                                                 MEMORY_SUBSYSTEM_PARTICLE);
    ps->particles.rot_y = memory_system_alloc(memory_system, sizeof(f32) * ps->particles_count,
                                             MEMORY_SUBSYSTEM_PARTICLE);


    ps->particles.scale_x = memory_system_alloc(memory_system, sizeof(f32) * ps->particles_count,
                                              MEMORY_SUBSYSTEM_PARTICLE);
    ps->particles.scale_y = memory_system_alloc(memory_system, sizeof(f32) * ps->particles_count,
                                              MEMORY_SUBSYSTEM_PARTICLE);

    ps->particles.color = memory_system_alloc(memory_system, sizeof(vec4s) * ps->particles_count,
                                              MEMORY_SUBSYSTEM_PARTICLE);

    ps->particles.texture_handle = memory_system_alloc(memory_system, sizeof(Texture_Handle) * ps->particles_count,
                                                       MEMORY_SUBSYSTEM_PARTICLE);
    ps->particles.tex_offset = memory_system_alloc(memory_system, sizeof(vec2s) * ps->particles_count,
                                                   MEMORY_SUBSYSTEM_PARTICLE);
    ps->particles.tex_size = memory_system_alloc(memory_system, sizeof(vec2s) * ps->particles_count,
                                                 MEMORY_SUBSYSTEM_PARTICLE);
    ps->particles.life_left = memory_system_alloc(memory_system, sizeof(f32) * ps->particles_count,
                                                  MEMORY_SUBSYSTEM_PARTICLE);
    ps->particles.vel_x = memory_system_alloc(memory_system, sizeof(f32) * ps->particles_count,
                                              MEMORY_SUBSYSTEM_PARTICLE);
    ps->particles.vel_y = memory_system_alloc(memory_system, sizeof(f32) * ps->particles_count,
                                              MEMORY_SUBSYSTEM_PARTICLE);
    ps->particles.vel_z = memory_system_alloc(memory_system, sizeof(f32) * ps->particles_count,
                                              MEMORY_SUBSYSTEM_PARTICLE);

    asset_converter_texture(resource_system,
                            "../z_assets/textures/kenney_particle-pack/PNG (Transparent)/circle_05.png", NULL);
    Texture_Handle handle =
        asset_load_texture_path(resource_system, "kenney_particle-pack/PNG (Transparent)/circle_05");
    for (u32 i = 0; i < ps->particles_count; i++)
    {
        ps->particles.texture_handle[i] = handle;
        ps->particles.scale_x[i] =  10.0f;
        ps->particles.scale_y[i] = 10.0f;
        ps->particles.vel_x[i] = rand_range_f(-10, 10);
        ps->particles.vel_y[i] = rand_range_f(-10, 10);
        ps->particles.vel_z[i] = rand_range_f(-10, 10);
        ps->particles.color[i].a = 1.0f;
        ps->particles.color[i].x = 1.0f;
        // ps->particles[i].life_left = 1.f;
    }

    //instantiate our dead particle lists with all the particles
    ps->dead_particles = memory_system_alloc(memory_system, sizeof(u32) * ps->particles_count,
                                             MEMORY_SUBSYSTEM_PARTICLE);
    ps->dead_particles_available = ps->particles_count;
    for (u32 i = 0; i < ps->particles_count; i++)
    {
        //its a one to one when we start out
        ps->dead_particles[i] = i;
    }


    //emitters
    ps->emitter_count_max = EMITTER_COUNT;
    ps->emitters = memory_system_alloc(memory_system, sizeof(Particle_Emitter) * ps->emitter_count_max,
                                       MEMORY_SUBSYSTEM_PARTICLE);
    ps->emitter_count = 1;


    particle_system_emitter_spawn(ps, &ps->emitters[0]);


    return ps;
}

void particle_system_shutdown(Particle_System* ps, Memory_System* memory_system)
{
    //TODO:
}


/**
 * @note: check for false because that means no particles were available
 */
bool particle_system_request_particle(Particle_System* ps, u32* out_particle_index)
{
    if (ps->dead_particles_available > 0)
    {
        ps->dead_particles_available--;
        *out_particle_index = ps->dead_particles[ps->dead_particles_available];
        return true;
    }
    return false;
}

bool particle_system_free_particle(Particle_System* ps, u32 particle_index)
{
    INFO("particle is dead id: %d", particle_index);

    ps->dead_particles[ps->dead_particles_available] = particle_index;
    ps->dead_particles_available++;


    return true;
}

bool particle_system_is_alive(Particle_System* ps, u32 particle_index)
{
    return ps->particles.life_left[particle_index] > 0;
}

bool particle_system_is_dead(Particle_System* ps, u32 particle_index)
{
    return ps->particles.life_left[particle_index] <= 0;
}


void particle_system_particle_update(Particle_System* ps, float dt)
{
    for (u32 i = 0; i < ps->particles_count; i++)
    {
        if (ps->particles.life_left[i] <= 0)
        {
            continue;
        }

        // ps->particles[i].position.y += sinf(1.f) * dt;
        ps->particles.pos_x[i] += ps->particles.vel_x[i] * dt;
        ps->particles.pos_y[i] += ps->particles.vel_y[i] * dt;
        ps->particles.pos_z[i] += ps->particles.vel_z[i] * dt;
        ps->particles.life_left[i] -= 1.0f * dt;
    }
}


void particle_system_update(Particle_System* ps, float dt)
{
    for (u32 i = 0; i < ps->emitter_count; i++)
    {
        particle_system_emitter_basic_update(ps, &ps->emitters[i], dt);
    }


    particle_system_particle_update(ps, dt);
}


Render_Packet_Particle particle_system_generate_render_packet(Particle_System* ps)
{
    return (Render_Packet_Particle){
        .particles = &ps->particles,
        .particle_count = ps->particles_count,
        .emitters = ps->emitters,
        .emitter_count = ps->emitter_count,
    };
}


/////////////// EMITTERS ///////////////


void particle_system_emitter_spawn(Particle_System* ps, Particle_Emitter* emitter)
{
    emitter->emission_rate = 1;
    emitter->particle_lifetime = 1.0f;
    emitter->particle = dynamic_array_create(u32, 256, ps->heap_allocator);
}


void particle_system_emitter_basic_update(Particle_System* ps, Particle_Emitter* emitter, float dt)
{
    //remove any dead particles
    for (u32 emitter_particle_index = 0; emitter_particle_index < emitter->particle->num_items; emitter_particle_index
         ++)
    {
        u32 particle_index = dynamic_array_get(emitter->particle, u32, emitter_particle_index);
        if (particle_system_is_dead(ps, particle_index))
        {
            particle_system_free_particle(ps, particle_index);
            dynamic_array_remove_swap(emitter->particle, emitter_particle_index);
            emitter_particle_index--;
        }
    }


    emitter->spawn_trigger += emitter->emission_rate * dt;
    if (emitter->spawn_trigger >= 1.0f)
    {
        emitter->particle_velocity.x = rand_range_f(-10, 10);
        emitter->particle_velocity.y = rand_range_f(-10, 10);
        emitter->particle_velocity.z = rand_range_f(-10, 10);

        emitter->particle_color.x = rand_range_f(0, 1);
        emitter->particle_color.y = rand_range_f(0, 1);
        emitter->particle_color.z = rand_range_f(0, 1);
        emitter->particle_color.a = 1.0f;

        u32 spawn_amount = (u32)floorf(emitter->spawn_trigger / 1.0f);
        emitter->spawn_trigger -= spawn_amount;


        for (u32 i = 0; i < spawn_amount; i++)
        {
            //grab a new particle and initialize it
            u32 particle_index = 0;
            if (!particle_system_request_particle(ps, &particle_index))
            {
                WARN("no free particle available")
                break;
            }

            dynamic_array_push(emitter->particle, &particle_index);

            ps->particles.life_left[particle_index] = emitter->particle_lifetime;

            ps->particles.vel_x[particle_index] = emitter->particle_velocity.x;
            ps->particles.vel_y[particle_index] = emitter->particle_velocity.y;
            ps->particles.vel_z[particle_index] = emitter->particle_velocity.z;


            ps->particles.pos_x[particle_index] = emitter->particle_position.x;
            ps->particles.pos_y[particle_index] = emitter->particle_position.y;
            ps->particles.pos_z[particle_index] = emitter->particle_position.z;

            ps->particles.color[particle_index] = emitter->particle_color;
        }
    }


    //TODO: any updates to the properties of the particles, based on emitter behavior
    /*for (u32 emitter_particle_index = 0; emitter_particle_index < emitter->particle->num_items; emitter_particle_index
         ++)
    {
        u32 particle_index = dynamic_array_get(emitter->particle, u32, emitter_particle_index);
    }*/
}


#endif //PARTICLE_SYSTEM_H
