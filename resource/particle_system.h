#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include "asset_converter.h"
#include "material_system.h"
#include "resource_types.h"

#define PARTICLE_COUNT 1000
#define PARTICLE_EMITTER_COUNT 100
#define PARTICLE_EFFECTS_COUNT 10


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


Particle_System* particle_system_init(Asset_System* asset_system, Memory_System* memory_system);

void particle_system_shutdown(Particle_System* ps, Memory_System* memory_system);

void particle_system_update(Particle_System* ps, float dt);

void particle_update(Particle_System* ps, float dt);

void particle_emitter_update(Particle_System* ps, Particle_Emitter* emitter, float dt);


//RENDER
Render_Packet_Particle particle_system_generate_render_packet(Particle_System* ps);


/////////////// Particles ///////////////

/**
 * @note: check for false because that means no particles were available
 */
bool particle_system_request_particle(Particle_System* ps, u32* out_particle_index);


bool particle_system_free_particle(Particle_System* ps, u32 particle_index);


bool particle_system_is_alive(Particle_System* ps, u32 particle_index);

bool particle_system_is_dead(Particle_System* ps, u32 particle_index);


/////////////// EMITTERS ///////////////

Particle_Emitter* particle_emitter_acquire(Particle_System* ps, String material_name);
void particle_emitter_release(Particle_System* ps, Particle_Emitter* emitter);

//should be moved to the asset system, and we should load by effect
Particle_Emitter* particle_emitter_acquire_from_file(Particle_System* ps, const char* file_path);


void particle_system_emitter_spawn(Particle_System* ps, Particle_Emitter* emitter);


/////////////// Particle Effects ///////////////

//TODO: for this it probably needs a file format saving configurations and an api to make creating one simple

Particle_Effect* particle_effect_acquire(Particle_System* ps, String name, Transform transform);
void particle_effect_release(Particle_System* ps, Particle_Effect* particle_effect);


void particle_effect_add_emitter(Particle_Effect* particle_effect, Particle_Emitter* emitter, u32 emitter_time_start,
                                 u32 emitter_time_end);




typedef struct Asset_Particle_Effect
{
    String name;
    u32 emitter_count;
} Asset_Particle_Effect;

typedef struct Asset_Particle_Emitter
{
    bool a;
} Asset_Particle_Emitter;




void particle_emitter_create_default(Asset_System* asset_system, Particle_System* particle_system,
                                               const char* emitter_name)
{

    MASSERT(asset_system);
    MASSERT(emitter_name);
    MASSERT(strlen(emitter_name) > 0);

    Material_Info default_particle_material_info = {
        .shader_name = &STRING("billboard_spherical"),
        .material_name = &STRING("Material_Spherical_Billboard_GPU"),
        .renderpass = Renderpass_Type_Color,
        .transluency = Shader_Transluency_Type_Opaque,
        .mesh_type = Shader_Mesh_Type_Mesh,
        .blend_mode = Shader_Blend_Mode_Additive,
        .material_key = 0,
    };

    Material_Asset material_asset = {0};
    Material_Instance mat_inst = {0};

    asset_converter_material(asset_system,
                                          &default_particle_material_info,
                                          &material_asset,
                                          &mat_inst,
                                          emitter_name);



    Particle_Emitter emitter = {
        .data = {
            .emission_rate = 1.0f,
            .spawn_trigger = 1.0f,
            .particle_lifetime = 1.0f,
            .particle_lifetime_variance = 0.f,
            .particle_color = (vec4s){1.0f, 0.0f, 0.0f, 1.0f},
            .position_variance = {0},
            .scale = {0},
            .scale_variance = {0},
            .rotation = {0},
            .velocity = {0},
            .velocity_variance = {0},
            .gravity = {0},
        },
        .name = STRING_CREATE_FROM_BUFFER_HEAP_ALLOCATOR("no name", asset_system->heap_allocator),
        .material_instance = mat_inst,
        .runtime_data = {0}
    };

    MADNESS_UUID uuid;
    asset_converter_particle_emitter(asset_system, &emitter, &uuid);
}


#endif //PARTICLE_SYSTEM_H
