#include "../resource/particle_system.h"
#include "asset_system.h"

Particle_System* particle_system_init(Asset_System* asset_system, Memory_System* memory_system)
{
    Particle_System* ps = memory_system_alloc(memory_system, sizeof(Particle_System), MEMORY_SUBSYSTEM_PARTICLE);

    u64 heap_alloc_mem_size = MB(1);
    ps->heap_allocator = memory_system_heap_allocator_create(memory_system, heap_alloc_mem_size,
                                                             MEMORY_SUBSYSTEM_PARTICLE, "particle system dynamic");

    u64 alloc_mem_size = MB(1);
    ps->allocator = memory_system_allocator_create(memory_system, alloc_mem_size, MEMORY_SUBSYSTEM_PARTICLE,
                                                   "particle system");

    ps->particles_count = MAX_PARTICLE_COUNT;
    ps->dead_particles_count = MAX_PARTICLE_COUNT;
    ps->particle_effects_count_max = MAX_PARTICLE_EFFECTS_COUNT;
    ps->emitter_count_max = MAX_PARTICLE_EMITTER_COUNT;


    ps->particles = memory_system_alloc(memory_system, sizeof(Particle) * ps->particles_count, MEMORY_SUBSYSTEM_PARTICLE);


    asset_converter_texture(asset_system,
                            "../z_assets/textures/kenney_particle-pack/PNG (Transparent)/circle_05.png", NULL);
    Texture_Handle handle =
        asset_load_texture_path(asset_system, "kenney_particle-pack/PNG (Transparent)/circle_05");

    for (u32 i = 0; i < ps->particles_count; i++)
    {
        ps->particles[i].scale.x = 10.0f;
        ps->particles[i].scale.y = 10.0f;

        ps->particles[i].color.a = 1.0f;
        ps->particles[i].color.x = 1.0f;
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

    ps->free_effects_queue = ring_queue_create_allocator(sizeof(u32), ps->particle_effects_count_max, ps->allocator);




    //emitters
    ps->emitters = memory_system_alloc(memory_system, sizeof(Particle_Emitter) * ps->emitter_count_max,
                                       MEMORY_SUBSYSTEM_PARTICLE);
    ps->free_emitters_queue = ring_queue_create_allocator(sizeof(u32), ps->emitter_count_max, ps->allocator);


    //queues
    // we start at 1 because our default emitters/effects take the 0 slot
    for (u32 i = 1; i < ps->particle_effects_count_max; i++)
    {
        ring_enqueue(ps->free_effects_queue, &i);
    }
    for (u32 i = 1; i < ps->emitter_count_max; i++)
    {
        ring_enqueue(ps->free_emitters_queue, &i);
    }


    //TODO: calculate the size needed for a large allocator,
    // so we can use that and free when everything is done bieng used
    ps->active_emitters = array_create_heap(u32, ps->emitter_count_max, ps->heap_allocator);
    ps->active_effects = array_create_heap(u32, ps->particle_effects_count_max, ps->heap_allocator);

    //create our default system and emitter
    ps->default_emitter = &ps->emitters[0];
    ps->default_emitter->data.emission_rate = 1;
    ps->default_emitter->data.particle_lifetime = 8.0f;
    ps->default_emitter->data.gravity = (vec3s){.x = 0.0f, .y = -9.8f, .z = 0.0f};
    ps->default_emitter->data.particle_color = (vec4s){.x = 0.0f, .y = 0.f, .z = 1.0f, .w = 1.0f};
    ps->default_emitter->particle = dynamic_array_create_heap(u32, 256, ps->heap_allocator);
    ps->default_emitter->name = STRING_CREATE_FROM_BUFFER_HEAP_ALLOCATOR(
        "INVALID EMITTER", asset_system->heap_allocator);
    ps->default_emitter->in_use = true;

    Shader_Handle shader_handle = {};
    Shader_PSO default_particle_material_info = {
        .shader_name = &STRING("billboard_spherical"),
        .blend_mode = Shader_Blend_Mode_Default,
        .two_sided = false,
    };
    shader_create_or_get(asset_system, &default_particle_material_info, &shader_handle );

    material_create_or_get(asset_system, shader_handle, "default_emitter", &ps->default_emitter->material_handle);


    ps->default_effect = &ps->particle_effects[0];
    ps->default_effect->name = STRING_CREATE_FROM_BUFFER_HEAP_ALLOCATOR("INVALID EFFECT", asset_system->heap_allocator);
    particle_effect_add_emitter(ps->default_effect, ps->default_emitter);
    ps->default_effect->emitters_start[0] = 0.0f;
    ps->default_effect->emitters_end[0] = 5.0f;

    //default handles
    ps->default_effect_handle = (Particle_Effect_Handle){0,0};
    ps->default_emitter_handle = (Particle_Emitter_Handle){0,0};

    u32 default_index = 0;
    array_push(ps->active_effects, &default_index);
    array_push(ps->active_emitters, &default_index);



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
    for (u32 i = 0; i < ps->emitter_count_max; i++) // TODO: replace with the active emitters
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
    return ps->particles[particle_index].life_left > 0;
}

bool particle_system_is_dead(Particle_System* ps, u32 particle_index)
{
    return ps->particles[particle_index].life_left <= 0;
}

void particle_update(Particle_System* ps, float dt)
{
    for (u32 i = 0; i < ps->particles_count; i++)
    {
        if (ps->particles[i].life_left <= 0)
        {
            continue;
        }

        //apply acceleration
        // ps->particles[i].position.y += sinf(1.f) * dt;
        ps->particles[i].velocity.x += ps->particles[i].gravity.x * dt;
        ps->particles[i].velocity.y += ps->particles[i].gravity.y * dt;
        ps->particles[i].velocity.z += ps->particles[i].gravity.z * dt;


        // ps->particles.vel_x[i] *=  1.0f - ps->particles.drag[i] * dt;


        //apply velocity
        ps->particles[i].position.x += ps->particles[i].velocity.x * dt;
        ps->particles[i].position.y += ps->particles[i].velocity.y * dt;
        ps->particles[i].position.z += ps->particles->velocity.z * dt;
        ps->particles[i].life_left -= 1.0f * dt;
    }
}


Render_Packet_Particle particle_system_generate_render_packet(Particle_System* ps)
{
    return (Render_Packet_Particle){
        .particles = ps->particles,
        .particle_count = ps->particles_count,
        .emitters = ps->emitters,
        .emitter_count = ps->emitter_count_max,
    };
}

Particle_Emitter* particle_emitter_acquire(Particle_System* ps, Asset_Record* record,
                                           Particle_Emitter_Handle* out_handle)
{
    u32 free_index;
    if (!ring_dequeue(ps->free_emitters_queue, &free_index))
    {
        WARN("particle_emitter_acquire: ran out of free emitters")
        MASSERT_FALSE()
        return NULL;
    }

    Particle_Emitter* particle_emitter = NULL;
    particle_emitter = &ps->emitters[free_index];
    particle_emitter->in_use = true;

    //list of particles manage by the emitter
    if (!particle_emitter->particle)
    {
        particle_emitter->particle = dynamic_array_create_heap(u32, 256, ps->heap_allocator);
    }


    Asset_Runtime runtime = ps->particle_emitter_asset[free_index];
    asset_runtime_set_info_from_record(&runtime, record, ASSET_PARTICLE_EMITTER);
    runtime.asset_handles.particle_emitter_handle = (Particle_Emitter_Handle){free_index, runtime.generation};

    //set emitter
    *out_handle = runtime.asset_handles.particle_emitter_handle;

    array_push(ps->active_emitters, &free_index);

    return particle_emitter;
}

bool particle_emitter_release(Particle_System* ps, Particle_Emitter_Handle handle)
{
    if (handle.handle == 0)
    {
        WARN("particle_emitter_release: trying to release default emitter");
        return false;
    }


    ps->emitters[handle.handle].in_use = false;
    for (u32 i = 0; i < ps->active_emitters->num_items; i++)
    {
        if (handle.handle == array_get(ps->active_emitters, i, u32))
        {
            array_remove_swap(ps->active_emitters, i);
            break;
        }
    }

    Asset_Runtime* runtime = &ps->particle_emitter_asset[handle.handle];
    if (runtime->generation != handle.gen)
    {
        MASSERT_MSG_FALSE("particle_emitter_release: INVALID TEXTURE GENERATION SHOULD NEVER IDEALLY HAPPEN");
        return false;
    }

    if (runtime_asset_release(runtime))
    {
        ring_enqueue(ps->free_emitters_queue, &handle.handle);
    }

    return true;
}


void particle_emitter_update(Particle_System* ps, Particle_Emitter* emitter, float dt)
{
    //remove any dead particles
    for (u32 emitter_particle_index = 0; emitter_particle_index < emitter->particle->num_items; emitter_particle_index
         ++)
    {
        u32 particle_index = dynamic_array_get(emitter->particle, emitter_particle_index, u32);
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

            ps->particles[particle_index].life_left = emitter->data.particle_lifetime +
                rand_range_f(-emitter->data.particle_lifetime_variance, emitter->data.particle_lifetime_variance);
            ps->particles[particle_index].velocity.x = emitter->data.velocity.x + rand_range_f(
                -emitter->data.velocity_variance.x, emitter->data.velocity_variance.x);
            ps->particles[particle_index].velocity.y = emitter->data.velocity.y + rand_range_f(
                -emitter->data.velocity_variance.y, emitter->data.velocity_variance.y);
            ps->particles[particle_index].velocity.z = emitter->data.velocity.z + rand_range_f(
                -emitter->data.velocity_variance.z, emitter->data.velocity_variance.z);


            ps->particles[particle_index].position.x = emitter->position.x;
            ps->particles[particle_index].position.y = emitter->position.y;
            ps->particles[particle_index].position.z = emitter->position.z;

            ps->particles[particle_index].color = emitter->data.particle_color;

            ps->particles[particle_index].gravity.x = emitter->data.gravity.x;
            ps->particles[particle_index].gravity.y = emitter->data.gravity.y;
            ps->particles[particle_index].gravity.z = emitter->data.gravity.z;



            ps->particles[particle_index].material_idx = emitter->material_handle.handle;
        }
    }


    //TODO: any updates to the properties of the particles, based on emitter behavior, like color over lifetime
    /*for (u32 emitter_particle_index = 0; emitter_particle_index < emitter->particle->num_items; emitter_particle_index
         ++)
    {
        u32 particle_index = dynamic_array_get(emitter->particle, u32, emitter_particle_index);
    }*/
}

bool particle_system_does_emitter_exist(Particle_System* ps, Asset_Record* record,
                                        Particle_Emitter_Handle* out_emitter_handle)
{
    for (u32 i = 0; i < MAX_PARTICLE_EMITTER_COUNT; i++)
    {
        Asset_Runtime* asset_runtime = &ps->particle_emitter_asset[i];
        if (madness_uuid_compare(asset_runtime->uuid, record->uuid))
        {
            *out_emitter_handle = asset_runtime->asset_handles.particle_emitter_handle;
            return true;
        }
    }

    return false;
}

bool particle_system_does_effect_exist(Particle_System* ps, Asset_Record* record,
                                       Particle_Effect_Handle* out_effect_handle)
{
    for (u32 i = 0; i < MAX_PARTICLE_EFFECTS_COUNT; i++)
    {
        Asset_Runtime* asset_runtime = &ps->particle_effect_asset[i];
        if (madness_uuid_compare(asset_runtime->uuid, record->uuid))
        {
            *out_effect_handle = asset_runtime->asset_handles.particle_effect_handle;
            return true;
        }
    }

    return false;
}


Particle_Effect* particle_effect_acquire(Particle_System* ps, Asset_Record* asset_record,
                                         Particle_Effect_Handle* out_handle)
{
    MASSERT(out_handle);

    u32 free_index;
    if (!ring_dequeue(ps->free_effects_queue, &free_index))
    {
        WARN("particle_emitter_acquire: ran out of free emitters")
        MASSERT_FALSE()
        return NULL;
        // return ps->default_effect;
    }

    Particle_Effect* out_effect = &ps->particle_effects[free_index];

    Asset_Runtime runtime = ps->particle_emitter_asset[free_index];
    asset_runtime_set_info_from_record(&runtime, asset_record, ASSET_PARTICLE_EMITTER);
    runtime.asset_handles.particle_effect_handle = (Particle_Effect_Handle){free_index, runtime.generation};


    *out_handle = runtime.asset_handles.particle_effect_handle;

    array_push(ps->active_effects, &free_index);


    return out_effect;
}

bool particle_effect_release(Particle_System* ps, Particle_Effect_Handle handle)
{
    if (handle.handle == 0)
    {
        WARN("particle_effect_release: trying to release default effect");
        return false;
    }


    for (u32 i = 0; i < ps->active_effects->num_items; i++)
    {
        if (handle.handle == array_get(ps->active_effects, i, u32))
        {
            array_remove_swap(ps->active_effects, i);
            break;
        }
    }

    Asset_Runtime* runtime = &ps->particle_emitter_asset[handle.handle];
    if (runtime->generation != handle.gen)
    {
        MASSERT_MSG_FALSE("particle_effect_release: INVALID TEXTURE GENERATION SHOULD NEVER IDEALLY HAPPEN");
        return false;
    }

    if (runtime_asset_release(runtime))
    {
        ring_enqueue(ps->free_effects_queue, &handle.handle);
    }
    return true;

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
