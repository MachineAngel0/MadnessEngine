#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

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



Particle_System* particle_system_init(Asset_System* resource_system, Memory_System* memory_system);

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

Particle_Emitter* particle_emitter_acquire(Particle_System* ps);
void particle_emitter_release(Particle_System* ps, Particle_Emitter* emitter);

void particle_system_emitter_spawn(Particle_System* ps, Particle_Emitter* emitter);


/////////////// Particle Effects ///////////////

//TODO: for this it probably needs a file format saving configurations and an api to make creating one simple

Particle_Effect* particle_effect_acquire(Particle_System* ps, String name, Transform transform);
void particle_effect_release(Particle_System* ps, Particle_Effect* particle_effect);


void particle_effect_add_emitter(Particle_Effect* particle_effect, Particle_Emitter* emitter, u32 emitter_time_start,
                                 u32 emitter_time_end);




//TODO: it can wait until more of this is fleshed out
#define ENGINE_PARTICLE_EXTENSION ".mparticle"
#define ENGINE_PARTICLE_PATH "../z_assets_engine/particle/"
void particle_effect_serialize(Particle_Effect* particle_effect)
{
    for (u32 i = 0; i < particle_effect->emitter_count; i++)
    {
        //serialize emitter
    }

    // u32 emitters_start[4];
    // u32 emitters_end[4];

    // String name;



}
void particle_effect_deserialize(Particle_Effect* particle_effect){}


#endif //PARTICLE_SYSTEM_H
