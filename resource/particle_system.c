#include "../resource/particle_system.h"
#include "asset_system.h"
#include "tracy/TracyC.h"

Particle_System* particle_system_init(Asset_System* asset_system, Memory_System* memory_system)
{
    Particle_System* ps = memory_system_alloc(memory_system, sizeof(Particle_System), MEMORY_SUBSYSTEM_PARTICLE);

    u64 heap_alloc_mem_size = MB(1);
    ps->heap_allocator = memory_system_heap_allocator_create(memory_system, heap_alloc_mem_size,
                                                             MEMORY_SUBSYSTEM_PARTICLE);


    ps->particles_count = MAX_PARTICLE_COUNT;
    ps->dead_particles_count = MAX_PARTICLE_COUNT;
    ps->particle_effects_count_max = MAX_PARTICLE_EFFECTS_COUNT;
    ps->emitter_count_max = MAX_PARTICLE_EMITTER_COUNT;


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

    ps->particles.material_key = memory_system_alloc(memory_system, sizeof(u64) * ps->particles_count,
                                                     MEMORY_SUBSYSTEM_PARTICLE);
    ps->particles.material_id = memory_system_alloc(memory_system, sizeof(u32) * ps->particles_count,
                                                    MEMORY_SUBSYSTEM_PARTICLE);


    ps->particle_generation = memory_system_alloc(memory_system, sizeof(u32) * ps->particle_effects_count_max,
                                                  MEMORY_SUBSYSTEM_PARTICLE);
    ps->emitter_generation = memory_system_alloc(memory_system, sizeof(u32) * ps->emitter_count_max,
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
    for (u32 i = 0; i < ps->particles_count; i++) // TODO: reverse the order of the index
    {
        //its a one to one when we start out
        ps->dead_particles[i] = i;
    }

    //effects
    ps->particle_effects = memory_system_alloc(memory_system, sizeof(Particle_Effect) * ps->particle_effects_count_max,
                                               MEMORY_SUBSYSTEM_PARTICLE);;

    ps->free_particle_effects = memory_system_alloc(memory_system,
                                                    sizeof(u32) * ps->particle_effects_count_max,
                                                    MEMORY_SUBSYSTEM_PARTICLE);
    ps->free_particle_effects_count = ps->particle_effects_count_max;

    s64 temp_effect_idx = ps->particle_effects_count_max - 1;
    s64 temp_effect_free = 0;
    while (temp_effect_idx > 0)
    {
        //its a one to one when we start out
        ps->free_particle_effects[temp_effect_idx] = temp_effect_free;
        temp_effect_idx--;
        temp_effect_free++;
    }


    //emitters
    ps->emitters = memory_system_alloc(memory_system, sizeof(Particle_Emitter) * ps->emitter_count_max,
                                       MEMORY_SUBSYSTEM_PARTICLE);
    ps->free_emitters_count = ps->emitter_count_max;
    ps->free_emitters = memory_system_alloc(memory_system, sizeof(u32) * ps->emitter_count_max,
                                            MEMORY_SUBSYSTEM_PARTICLE);

    s64 temp_idx = ps->emitter_count_max - 1;
    s64 temp_free = 0;
    while (temp_idx > 0)
    {
        //its a one to one when we start out
        ps->free_emitters[temp_free] = temp_idx;
        temp_idx--;
        temp_free++;
    }


    //TODO: calculate the size needed for a large allocator,
    // so we can use that and free when everything is done bieng used
    ps->active_emitters = array_create_heap(u32, ps->emitter_count_max, ps->heap_allocator);
    ps->active_effects = array_create_heap(u32, ps->particle_effects_count_max, ps->heap_allocator);


    //create our default system and emitter
    ps->default_emitter = particle_emitter_acquire(ps, &ps->default_emitter_handle);
    ps->default_emitter->data.emission_rate = 1;
    ps->default_emitter->data.particle_lifetime = 8.0f;
    ps->default_emitter->data.gravity = (vec3s){.x = 0.0f, .y = -9.8f, .z = 0.0f};
    ps->default_emitter->data.particle_color = (vec4s){.x = 0.0f, .y = 0.f, .z = 1.0f, .w = 1.0f};
    ps->default_emitter->particle = dynamic_array_create(u32, 256, ps->heap_allocator);
    ps->default_emitter->name = STRING_CREATE_FROM_BUFFER_HEAP_ALLOCATOR(
        "INVALID EMITTER", asset_system->heap_allocator);

    ps->default_effect = particle_effect_acquire(ps, &ps->default_effect_handle);
    ps->default_effect->name = STRING_CREATE_FROM_BUFFER_HEAP_ALLOCATOR("INVALID EFFECT", asset_system->heap_allocator);
    particle_effect_add_emitter(ps->default_effect, ps->default_emitter);
    ps->default_effect->emitters_start[0] = 0.0f;
    ps->default_effect->emitters_end[0] = 5.0f;
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
    for (u32 i = 0; i < ps->emitter_count_max; i++)
    {
        if (!ps->emitters[i].in_use) { continue; }
        particle_emitter_update(ps, &ps->emitters[i], dt);
    }
    PROFILE_ZONE_END(particle_update_emitter);

    /*PROFILE_ZONE(particle_update_active_emitter);
    for (u32 i = 0; i < ps->active_emitters->num_items; i++)
    {
        u32 emitter_index = array_get(ps->active_emitters, u32, i);
        particle_emitter_update(ps, &ps->emitters[emitter_index], dt);
    }
    PROFILE_ZONE_END(particle_update_active_emitter);*/


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

    //TODO: might not be needed since the particle is dead anyway
    // ps->particles.material_id[particle_index] = 0;
    // ps->particles.material_key[particle_index] = 0;

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
        .emitter_count = ps->emitter_count_max,
    };
}

Particle_Emitter* particle_emitter_acquire(Particle_System* ps, Particle_Emitter_Handle* out_handle)
{
    Particle_Emitter* particle_emitter = NULL;

    if (ps->free_emitters_count <= 0)
    {
        WARN("particle_emitter_acquire: ran out of fre emitters")
        return ps->default_emitter;
    }


    u32 free_index = ps->free_emitters[--ps->free_emitters_count];

    particle_emitter = &ps->emitters[free_index];
    particle_emitter->in_use = true;

    if (!particle_emitter->particle)
    {
        particle_emitter->particle = dynamic_array_create(u32, 256, ps->heap_allocator);
    }

    *out_handle = (Particle_Emitter_Handle){
        .handle = free_index,
        .gen = ps->emitter_generation[free_index]
    };

    array_push(ps->active_emitters, &free_index);

    return particle_emitter;
}

void particle_emitter_release(Particle_System* ps, Particle_Emitter_Handle handle)
{
    ps->emitters[handle.handle].in_use = false;
    // dynamic_array_free(ps->emitters[handle.handle].particle); //idk if i want to do this
    // allocator_heap_free(ps->heap_allocator, ps->emitters[handle.handle].name); //idk if i want to do this

    for (u32 i = 0; i < ps->active_emitters->num_items; i++)
    {
        if (handle.handle == array_get(ps->active_emitters, u32, i))
        {
            array_remove_swap(ps->active_emitters, i);
            break;
        }
    }


    ps->emitter_generation[handle.handle]++; // bump the generation
    ps->free_emitters[ps->free_emitters_count] = handle.handle;
    ps->free_emitters_count++;
}


void particle_emitter_update(Particle_System* ps, Particle_Emitter* emitter, float dt)
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

            dynamic_array_push(emitter->particle, &particle_index);

            ps->particles.life_left[particle_index] = emitter->data.particle_lifetime +
                rand_range_f(-emitter->data.particle_lifetime_variance, emitter->data.particle_lifetime_variance);
            ps->particles.vel_x[particle_index] = emitter->data.velocity.x + rand_range_f(
                -emitter->data.velocity_variance.x, emitter->data.velocity_variance.x);
            ps->particles.vel_y[particle_index] = emitter->data.velocity.y + rand_range_f(
                -emitter->data.velocity_variance.y, emitter->data.velocity_variance.y);
            ps->particles.vel_z[particle_index] = emitter->data.velocity.z + rand_range_f(
                -emitter->data.velocity_variance.z, emitter->data.velocity_variance.z);


            ps->particles.pos_x[particle_index] = emitter->position.x;
            ps->particles.pos_y[particle_index] = emitter->position.y;
            ps->particles.pos_z[particle_index] = emitter->position.z;

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
    for (u32 i = 0; i < ps->emitter_count_max; i++)
    {
        if (!ps->emitters[i].name) { continue; }

        if (string_compare(ps->emitters[i].name, name))
        {
            return true;
        }
    }

    return false;
}

bool particle_system_does_effect_exist(Particle_System* ps, String* name)
{
    for (u32 i = 0; i < ps->particle_effects_count_max; i++)
    {
        if (!ps->particle_effects->name) { continue; }
        if (string_compare(ps->particle_effects[i].name, name))
        {
            return true;
        }
    }

    return false;
}


Particle_Effect* particle_effect_acquire(Particle_System* ps, Particle_Effect_Handle* out_handle)
{
    MASSERT(out_handle);

    if (ps->free_particle_effects_count <= 0)
    {
        WARN("particle_effect_acquire: out of effects to give")
        *out_handle = (Particle_Effect_Handle){0, 0};
        return ps->default_effect;
    }

    u32 free_index = ps->free_particle_effects[--ps->free_particle_effects_count];
    Particle_Effect* out_effect = &ps->particle_effects[free_index];
    *out_handle = (Particle_Effect_Handle){
        .handle = free_index,
        .gen = ps->particle_generation[free_index]
    };
    array_push(ps->active_effects, &free_index);


    return out_effect;
}

void particle_effect_release(Particle_System* ps, Particle_Effect_Handle handle)
{
    for (u32 i = 0; i < ps->active_effects->num_items; i++)
    {
        if (handle.handle == array_get(ps->active_effects, u32, i))
        {
            array_remove_swap(ps->active_effects, i);
            break;
        }
    }

    ps->particle_generation[handle.handle]++; // bump the generation
    ps->free_particle_effects[ps->free_particle_effects_count] = handle.handle;
    ps->free_particle_effects_count++;
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
