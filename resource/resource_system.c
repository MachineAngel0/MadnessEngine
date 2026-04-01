#include "resource_system.h"

#include "sprite_system.h"


bool resource_system_init(Resource_System* resource_system, Memory_System* memory_system)
{
    resource_system->render_packet = memory_system_alloc(memory_system, sizeof(Render_Packet));


    resource_system->sprite_system = sprite_system_init(memory_system);
    resource_system->texture_system = texture_system_init(memory_system);
    resource_system->mesh_system = mesh_system_init(memory_system);

    // resource_system->mesh_system = mesh_system_init();
    // resource_system->shader_system = shader_system_init();

    return true;
}


bool resource_system_shutdown(Resource_System* resource_system)
{
    //shutdown the resource systems
    sprite_system_shutdown(resource_system->sprite_system);
    return true;
}

bool resource_system_update_and_create_render_packet(Resource_System* resource_system)
{
    render_packet_clear(resource_system->render_packet);
    resource_system->render_packet->texture_queue = resource_system->texture_system->textures_ring_queue;
    return resource_system_generate_render_packet(resource_system);
}

bool resource_system_generate_render_packet(Resource_System* resource_system)
{
    sprite_system_generate_render_packet(resource_system->sprite_system,
                                         &resource_system->render_packet->ui_data_packet,
                                         &resource_system->render_packet->sprite_data_packet);

    mesh_system_generate_render_packet(resource_system->mesh_system, &resource_system->render_packet->mesh_data_packet);

    return true;
}

void render_packet_clear(Render_Packet* renderer_packets)
{
    memset(renderer_packets, 0, sizeof(Render_Packet));
}



