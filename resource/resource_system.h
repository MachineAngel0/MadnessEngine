#ifndef RESOURCE_SYSTEM_H
#define RESOURCE_SYSTEM_H

#include <stdbool.h>

#include "resource_types.h"
#include "ring_queue.h"

typedef struct Resource_Render_Packet
{
    ring_queue textures_to_load;
}Resource_Render_Packet;

typedef struct Resource_System
{
    //the resource system is just a container for all the system,
    //each system can just access the system it need to load assets


    // Shader_System* shader_system;
    // Material_System* shader_system; //probably want a material system, but not a shader system here, but in the renderer
    Sprite_System* sprite_system;
    Mesh_System* mesh_system;
    Texture_System* texture_system;


    Render_Packet* render_packet;

} Resource_System;



Resource_System* resource_system_init(Memory_System* memory_system);

bool resource_system_shutdown(Resource_System* resource_system);
bool resource_system_update_and_create_render_packet(Resource_System* resource_system);

bool resource_system_generate_render_packet(Resource_System* resource_system);
MAPI void render_packet_clear(Render_Packet* renderer_packets);






#endif //RESOURCE_SYSTEM_H
