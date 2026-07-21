#include "asset_system.h"

#include "material_system.h"
#include "mesh_system.h"
#include "sprite_system.h"


Asset_System* asset_system_init(Memory_System* memory_system)
{
    Asset_System* asset_system = memory_system_alloc(memory_system, sizeof(Asset_System),
                                                           MEMORY_SUBSYSTEM_RESOURCE);
    asset_system->render_packet = memory_system_alloc(memory_system, sizeof(Render_Packet),
                                                         MEMORY_SUBSYSTEM_RESOURCE);


    asset_system->heap_allocator = memory_system_heap_allocator_create(memory_system, MB(64),
                                                     MEMORY_SUBSYSTEM_RESOURCE);
    asset_system->frame_allocator = memory_system_allocator_create(memory_system, MB(64),
                                                     MEMORY_SUBSYSTEM_RESOURCE);

    //Texture
    asset_system->texture_system = memory_system_alloc(memory_system, sizeof(Texture_System),
                                                     MEMORY_SUBSYSTEM_TEXTURE);
    texture_system_init(asset_system, asset_system->texture_system, memory_system);



    asset_system->scene = scene_init(memory_system);
    asset_system->material_system = material_system_init(memory_system);

    asset_system->sprite_system = sprite_system_init(memory_system);
    asset_system->mesh_system = mesh_system_init(asset_system, memory_system);
    asset_system->particle_system = particle_system_init(asset_system, memory_system);


    //ifdef out for debug builds

    //load the asset metadata




    return asset_system;
}


bool asset_system_shutdown(Asset_System* resource_system, Memory_System* memory_system)
{
    //shutdown the resource systems
    particle_system_shutdown(resource_system->particle_system, memory_system);
    mesh_system_shutdown(resource_system->mesh_system, memory_system);
    sprite_system_shutdown(resource_system->sprite_system, memory_system);
    texture_system_shutdown(resource_system->texture_system, memory_system);
    scene_shutdown(resource_system->scene, memory_system);
    material_system_shutdown(resource_system->material_system, memory_system);
    return true;
}


bool asset_system_update_and_create_render_packet(Asset_System* resource_system)
{
    allocator_clear(resource_system->frame_allocator);
    render_packet_clear(resource_system->render_packet);
    resource_system->render_packet->texture_queue = resource_system->texture_system->texture_upload_queue;
    resource_system->render_packet->mesh_queue = resource_system->mesh_system->mesh_ring_queue;
    resource_system->render_packet->skinned_mesh_queue = resource_system->mesh_system->skinned_mesh_ring_queue;
    return asset_system_generate_render_packet(resource_system);
}

bool asset_system_generate_render_packet(Asset_System* resource_system)
{
    sprite_system_generate_render_packet(resource_system->sprite_system,
                                         &resource_system->render_packet->sprite_data_packet);

    material_system_generate_render_packet(resource_system->material_system, &resource_system->render_packet->draw_3d_data_packet);

    scene_update(resource_system->scene, resource_system);
    resource_system->render_packet->draw_3d_data_packet.world_space_matrix_array = resource_system->scene->world_transforms;
    resource_system->render_packet->draw_3d_data_packet.world_space_matrix_count = resource_system->scene->transform_count;

    resource_system->render_packet->draw_3d_data_packet.skinned_matrix = resource_system->mesh_system->skinned_matrix_array;

    resource_system->render_packet->particle_packet = particle_system_generate_render_packet(resource_system->particle_system);

    return true;
}

void render_packet_clear(Render_Packet* renderer_packets)
{
    memset(renderer_packets, 0, sizeof(Render_Packet));
}
