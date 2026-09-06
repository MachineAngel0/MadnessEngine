#include "../resource/particle_system.h"
#include "asset_system.h"
#include "tracy/TracyC.h"

Particle_System* particle_system_init(Asset_System* asset_system, Memory_System* memory_system)
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
    ps->particles.gravity_x = memory_system_alloc(memory_system, sizeof(f32) * ps->particles_count,
                                                  MEMORY_SUBSYSTEM_PARTICLE);
    ps->particles.gravity_y = memory_system_alloc(memory_system, sizeof(f32) * ps->particles_count,
                                                  MEMORY_SUBSYSTEM_PARTICLE);
    ps->particles.gravity_z = memory_system_alloc(memory_system, sizeof(f32) * ps->particles_count,
                                                  MEMORY_SUBSYSTEM_PARTICLE);

    asset_converter_texture(asset_system,
                            "../z_assets/textures/kenney_particle-pack/PNG (Transparent)/circle_05.png", NULL);
    Texture_Handle handle =
        asset_load_texture_path(asset_system, "kenney_particle-pack/PNG (Transparent)/circle_05");
    for (u32 i = 0; i < ps->particles_count; i++)
    {
        ps->particles.texture_handle[i] = handle;
        ps->particles.scale_x[i] = 10.0f;
        ps->particles.scale_y[i] = 10.0f;

        ps->particles.color[i].a = 1.0f;
        ps->particles.color[i].x = 1.0f;
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

    //effects
    ps->particle_effects_count = 0;
    ps->particle_effects_count_max = PARTICLE_EFFECTS_COUNT;
    ps->particle_effects = memory_system_alloc(memory_system, sizeof(Particle_Effect) * ps->particle_effects_count_max,
                                               MEMORY_SUBSYSTEM_PARTICLE);;

    ps->available_particle_effects = memory_system_alloc(memory_system,
                                                         sizeof(u32) * ps->particle_effects_count_max,
                                                         MEMORY_SUBSYSTEM_PARTICLE);
    ps->available_particle_effects_count = 0;

    //emitters
    ps->emitter_count_max = PARTICLE_EMITTER_COUNT;
    ps->emitters = memory_system_alloc(memory_system, sizeof(Particle_Emitter) * ps->emitter_count_max,
                                       MEMORY_SUBSYSTEM_PARTICLE);
    ps->emitter_count = 0;
    ps->available_emitters = memory_system_alloc(memory_system, sizeof(Particle_Emitter*) * ps->emitter_count_max,
                                                 MEMORY_SUBSYSTEM_PARTICLE);
    ps->available_emitters_count = 0;


    //create our default system and emitter
    ps->default_emitter = particle_emitter_acquire(ps, &ps->default_emitter_handle);
    ps->default_emitter->data.emission_rate = 1;
    ps->default_emitter->data.particle_lifetime = 8.0f;
    ps->default_emitter->data.gravity = (vec3s){.x = 0.0f, .y = -9.8f, .z = 0.0f};
    ps->default_emitter->data.particle_color = (vec4s){.x = 0.0f, .y = 0.f, .z = 1.0f, .w = 1.0f};
    ps->default_emitter->runtime_data.particle = dynamic_array_create(u32, 256, ps->heap_allocator);
    ps->default_emitter->name = STRING_CREATE_FROM_BUFFER_HEAP_ALLOCATOR(
        "INVALID EMITTER", asset_system->heap_allocator);

    particle_effect_acquire(ps, &ps->default_effect, &ps->default_effect_handle);
    ps->default_effect->name = STRING_CREATE_FROM_BUFFER_HEAP_ALLOCATOR("INVALID EFFECT", asset_system->heap_allocator);
    particle_effect_add_emitter(ps->default_effect, ps->default_emitter);
    ps->default_effect->emitters_start[0] = 0.0f;
    ps->default_effect->emitters_end[1] = 5.0f;
    return ps;
}

void particle_system_shutdown(Particle_System* ps, Memory_System* memory_system)
{
    //TODO:
}

void particle_system_update(Particle_System* ps, float dt)
{
    PROFILE_ZONE(particle_system_update);

    PROFILE_ZONE(particle_update_emitter);
    for (u32 i = 0; i < ps->emitter_count; i++)
    {
        particle_emitter_update(ps, &ps->emitters[i], dt);
    }
    PROFILE_ZONE_END(particle_update_emitter);

    PROFILE_ZONE(particle_update);
    particle_update(ps, dt);
    PROFILE_ZONE_END(particle_update);

    PROFILE_ZONE_END(particle_system_update);
}

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

void particle_update(Particle_System* ps, float dt)
{
    for (u32 i = 0; i < ps->particles_count; i++)
    {
        if (ps->particles.life_left[i] <= 0)
        {
            continue;
        }

        //apply acceleration
        // ps->particles[i].position.y += sinf(1.f) * dt;
        ps->particles.vel_x[i] += ps->particles.gravity_x[i] * dt;
        ps->particles.vel_y[i] += ps->particles.gravity_y[i] * dt;
        ps->particles.vel_z[i] += ps->particles.gravity_z[i] * dt;


        // ps->particles.vel_x[i] *=  1.0f - ps->particles.drag[i] * dt;


        //apply velocity
        ps->particles.pos_x[i] += ps->particles.vel_x[i] * dt;
        ps->particles.pos_y[i] += ps->particles.vel_y[i] * dt;
        ps->particles.pos_z[i] += ps->particles.vel_z[i] * dt;
        ps->particles.life_left[i] -= 1.0f * dt;
    }
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

Particle_Emitter* particle_emitter_acquire(Particle_System* ps, Particle_Emitter_Handle* out_handle)
{
    Particle_Emitter* particle_emitter = NULL;
    if (ps->emitter_count < ps->emitter_count_max)
    {
        *out_handle = (Particle_Emitter_Handle){.handle = ps->emitter_count, .gen = 0};
        particle_emitter = &ps->emitters[ps->emitter_count];
        particle_emitter->runtime_data.particle = dynamic_array_create(u32, 256, ps->heap_allocator);
        ps->emitter_count++;
    }
    else
    {
        if (ps->available_emitters_count > 0)
        {
            //TODO: out handle
            particle_emitter = ps->available_emitters[ps->available_emitters_count--];
        }
    }


    if (!particle_emitter)
    {
        FATAL("NO PARTICLE EFFECTS TO GIVE OUT")
    }

    return particle_emitter;
}

void particle_emitter_release(Particle_System* ps, Particle_Emitter* emitter)
{
    ps->available_emitters[ps->available_emitters_count] = emitter;
    ps->available_emitters_count++;
}


void particle_emitter_update(Particle_System* ps, Particle_Emitter* emitter, float dt)
{
    //remove any dead particles
    for (u32 emitter_particle_index = 0; emitter_particle_index < emitter->runtime_data.particle->num_items;
         emitter_particle_index
         ++)
    {
        u32 particle_index = dynamic_array_get(emitter->runtime_data.particle, u32, emitter_particle_index);
        if (particle_system_is_dead(ps, particle_index))
        {
            particle_system_free_particle(ps, particle_index);
            dynamic_array_remove_swap(emitter->runtime_data.particle, emitter_particle_index);
            emitter_particle_index--;
        }
    }

    //see if we need to spawn any new particles
    emitter->data.spawn_trigger += emitter->data.emission_rate * dt;
    if (emitter->data.spawn_trigger >= 1.0f)
    {
        //see how many we need to spawn
        u32 spawn_amount = (u32)floorf(emitter->data.spawn_trigger / 1.0f);
        emitter->data.spawn_trigger -= spawn_amount;


        //grab a new particle, if available, and initialize it
        for (u32 i = 0; i < spawn_amount; i++)
        {
            u32 particle_index = 0;
            if (!particle_system_request_particle(ps, &particle_index))
            {
                WARN("no free particle available")
                break;
            }

            dynamic_array_push(emitter->runtime_data.particle, &particle_index);

            ps->particles.life_left[particle_index] = emitter->data.particle_lifetime +
                rand_range_f(-emitter->data.particle_lifetime_variance, emitter->data.particle_lifetime_variance);
            ps->particles.vel_x[particle_index] = emitter->data.velocity.x + rand_range_f(
                -emitter->data.velocity_variance.x, emitter->data.velocity_variance.x);
            ps->particles.vel_y[particle_index] = emitter->data.velocity.y + rand_range_f(
                -emitter->data.velocity_variance.y, emitter->data.velocity_variance.y);
            ps->particles.vel_z[particle_index] = emitter->data.velocity.z + rand_range_f(
                -emitter->data.velocity_variance.z, emitter->data.velocity_variance.z);


            ps->particles.pos_x[particle_index] = emitter->runtime_data.position.x;
            ps->particles.pos_y[particle_index] = emitter->runtime_data.position.y;
            ps->particles.pos_z[particle_index] = emitter->runtime_data.position.z;

            ps->particles.color[particle_index] = emitter->data.particle_color;

            ps->particles.gravity_x[particle_index] = emitter->data.gravity.x;
            ps->particles.gravity_y[particle_index] = emitter->data.gravity.y;
            ps->particles.gravity_z[particle_index] = emitter->data.gravity.z;
        }
    }


    //TODO: any updates to the properties of the particles, based on emitter behavior, like color over lifetime
    /*for (u32 emitter_particle_index = 0; emitter_particle_index < emitter->particle->num_items; emitter_particle_index
         ++)
    {
        u32 particle_index = dynamic_array_get(emitter->particle, u32, emitter_particle_index);
    }*/
}

bool particle_system_does_emitter_exist(Particle_System* ps, String* name)
{
    for (u32 i = 0; i < ps->emitter_count; i++)
    {
        if (string_compare(ps->emitters[i].name, name))
        {
            return true;
        }
    }

    return false;
}

bool particle_system_does_effect_exist(Particle_System* ps, String* name)
{
    for (u32 i = 0; i < ps->particle_effects_count; i++)
    {
        if (string_compare(ps->particle_effects[i].name, name))
        {
            return true;
        }
    }

    return false;
}


bool particle_effect_acquire(Particle_System* ps, Particle_Effect** out_effect, Particle_Effect_Handle* out_handle)
{
    MASSERT(out_handle);

    if (ps->particle_effects_count < ps->particle_effects_count_max)
    {
        *out_handle = (Particle_Effect_Handle){ps->particle_effects_count, 0/*TODO*/};
        *out_effect = &ps->particle_effects[ps->particle_effects_count++];
        return true;
    }

    //check our free list
    if (ps->available_particle_effects_count > 0)
    {
        u32 free_particle_effect_index = ps->available_particle_effects[ps->available_particle_effects_count--];
        Particle_Effect* particle_effect = &ps->particle_effects[free_particle_effect_index];
        *out_effect = particle_effect;
        *out_handle = (Particle_Effect_Handle){free_particle_effect_index, particle_effect->generation};
    }
    return true;

    if (!out_effect)
    {
        FATAL("NO PARTICLE EFFECTS TO GIVE OUT")
    }

    return false;
}

void particle_effect_release(Particle_System* ps, Particle_Effect_Handle handle)
{
    ps->particle_effects[handle.handle].generation++; // bump the generation

    ps->available_particle_effects[ps->available_particle_effects_count] = handle.handle;
    ps->available_particle_effects_count++;
}


void particle_effect_add_emitter(Particle_Effect* particle_effect, Particle_Emitter* emitter)
{
    if (particle_effect->emitter_count >= 4)
    {
        FATAL("particle_effect_add_emitter: ran out of space")
        return;
    }

    //check if we are adding a duplicate emitter, since we only allow one of any emitter asset
    for (u32 i = 0; i < particle_effect->emitter_count; i++)
    {
        if (string_compare(particle_effect->emitters[i]->name, emitter->name))
        {
            WARN("particle_effect_add_emitter: trying to add a duplicate emitter [%.*s] to particle [%.*s]",
                 emitter->name->length, emitter->name->chars, particle_effect->name->length,
                 particle_effect->name->chars);
            return;
        }
    }

    particle_effect->emitters[particle_effect->emitter_count] = emitter;
    particle_effect->emitters_start[particle_effect->emitter_count] = 0;
    particle_effect->emitters_end[particle_effect->emitter_count] = 5.0f;

    particle_effect->emitter_count++;
}

void particle_effect_add_emitter_by_handle(Particle_System* particle_system, Particle_Effect* particle_effect,
                                           Particle_Emitter_Handle emitter_handle)
{
    particle_effect_add_emitter(particle_effect, &particle_system->emitters[emitter_handle.handle]);
}

void particle_effect_remove_emitter(Particle_Effect* particle_effect, u32 emitter_index)
{
    if (particle_effect->emitter_count < emitter_index)
    {
        WARN("particle_effect_remove_emitter: invalid removal area")
        return;
    }

    //TODO: unload emitter

    if (particle_effect->emitter_count == emitter_index)
    {
        particle_effect->emitter_count--;
        return;
    }

    //swap and remove
    particle_effect->emitters[emitter_index] = particle_effect->emitters[particle_effect->emitter_count - 1];
    particle_effect->emitters_start[emitter_index] = particle_effect->emitters_start[particle_effect->emitter_count -
        1];
    particle_effect->emitters_end[emitter_index] = particle_effect->emitters_end[particle_effect->emitter_count - 1];

    particle_effect->emitter_count--;
}
