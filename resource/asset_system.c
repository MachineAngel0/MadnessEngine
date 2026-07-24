#include "asset_system.h"

#include "asset_serialization.h"
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

    //Asset Registry
    asset_system->asset_registry = memory_system_alloc(memory_system, sizeof(Asset_Registry),
                                                       MEMORY_SUBSYSTEM_RESOURCE);
    asset_registry_init(asset_system->asset_registry, asset_system->heap_allocator);


    //Texture
    asset_system->texture_system = memory_system_alloc(memory_system, sizeof(Texture_System),
                                                       MEMORY_SUBSYSTEM_TEXTURE);
    texture_system_init(asset_system, asset_system->texture_system, memory_system);


    asset_system->scene = scene_init(memory_system);

    asset_system->material_system = memory_system_alloc(memory_system, sizeof(Material_System),
                                                       MEMORY_SUBSYSTEM_RESOURCE);
    material_system_init(asset_system->material_system, asset_system, memory_system);

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

    material_system_generate_render_packet(resource_system->material_system,
                                           &resource_system->render_packet->draw_3d_data_packet);

    scene_update(resource_system->scene, resource_system);
    resource_system->render_packet->draw_3d_data_packet.world_space_matrix_array = resource_system->scene->
        world_transforms;
    resource_system->render_packet->draw_3d_data_packet.world_space_matrix_count = resource_system->scene->
        transform_count;

    resource_system->render_packet->draw_3d_data_packet.skinned_matrix = resource_system->mesh_system->
        skinned_matrix_array;

    resource_system->render_packet->particle_packet = particle_system_generate_render_packet(
        resource_system->particle_system);

    return true;
}

void render_packet_clear(Render_Packet* renderer_packets)
{
    memset(renderer_packets, 0, sizeof(Render_Packet));
}

Texture_Handle asset_load_texture_path(Asset_System* asset_system, const char* asset_path)
{
    //TODO: this should load honestly from either engine or import path

    //either load from metadata -> binary or binary blob
    //then send into the texture system
    //in general we just want to deserialize the data quickly,
    //the deserialization is the same, it just depends which file data we end up giving it

    String* load_asset_path = STRING_CREATE_FROM_BUFFER_ALLOCATOR(asset_path, asset_system->frame_allocator);

    MADNESS_UUID uuid = {0, 0};
    u64 hash = 0;
    for (u64 i = 0; i < asset_system->asset_registry->asset_meta_data->num_items; i++)
    {
        Asset_MetaData* meta_data = _dynamic_array_get(asset_system->asset_registry->asset_meta_data, i);
        if (string_compare(meta_data->binary_file, load_asset_path))
        {
            //found
            uuid = meta_data->uuid;
            hash = meta_data->hash;
            break;
        }
    }
    if (hash == 0)
    {
        MASSERT("PLZ CONVERT ASSET")
        return (Texture_Handle){0};
    }

    //has asset already been loaded
    Texture_Handle texture_handle = {0};
    if (texture_system_exists(asset_system, &texture_handle, hash))
    {
        return texture_handle;
    }


    FILE* fptr = NULL;

    //load from individal binary
    bool debug = true;
    if (debug)
    {
        fptr = fopen(asset_path, "rb");
        if (!fptr)
        {
            //TODO: since were in the editor, we should at least try to find the asset in our asset folder
            return asset_system->texture_system->default_texture_handle;
        }

        Madness_Texture_Runtime engine_texture = {0};

        asset_texture_deserialize_heap(&engine_texture, fptr, asset_system->heap_allocator);

        texture_system_upload_new_texture(asset_system, uuid, hash, engine_texture.texture, engine_texture.pixel_data,
                                          &texture_handle);
    }
    /*else
    {
        //TODO:
        MASSERT(false);
        // search for asset by its hash name and its offset, then load it in with our format
        // u64 asset_offset = asset_system_find_asset(asset_system, scene_id, hash_id);
        // Madness_Texture_Runtime runtime_texture = {0};
        // texture_system_upload_new_texture(asset_system, hash_id, editor_texture.texture, editor_texture.pixel_data, &texture_handle);
    }*/
    fclose(fptr);

    return texture_handle;
}



bool asset_load_font(Asset_System* asset_system, const char* engine_asset_path, Texture_Handle* out_handle)
{
    MADNESS_UUID uuid = {0, 0};
    u64 hash = 0;

    String* asset_path_string = STRING_CREATE_FROM_BUFFER_ALLOCATOR(engine_asset_path, asset_system->frame_allocator);

    for (u64 i = 0; i < asset_system->asset_registry->asset_meta_data->num_items; i++)
    {
        Asset_MetaData* meta_data = _dynamic_array_get(asset_system->asset_registry->asset_meta_data, i);
        if (string_compare(meta_data->binary_file, asset_path_string))
        {
            //found
            uuid = meta_data->uuid;
            hash = meta_data->hash;
            break;
        }
    }
    if (hash == 0)
    {
        MASSERT_MSG(false, "PLZ CONVERT ASSET")
        *out_handle = (Texture_Handle){0};
        return out_handle;
    }

    //has asset already been loaded
    if (texture_system_exists(asset_system, out_handle, hash))
    {
        return true;
    }


    FILE* fptr = NULL;

    //load from individal binary
    bool debug = true;
    if (debug)
    {
        Madness_Font_Runtime editor_texture = {0};

        fptr = fopen(engine_asset_path, "rb");

        asset_font_deserialize_heap(&editor_texture, fptr, asset_system->heap_allocator);

        texture_system_upload_new_font(asset_system, uuid, hash, editor_texture.texture, editor_texture.font_texture,
                                       editor_texture.pixel_data, out_handle);
    }
    else
    {
        //TODO:
        MASSERT(false);
        // search for asset by its hash name and its offset, then load it in with our format
        // u64 asset_offset = asset_system_find_asset(asset_system, scene_id, hash_id);
        // Madness_Texture_Runtime runtime_texture = {0};
        // texture_system_upload_new_texture(asset_system, hash_id, editor_texture.texture, editor_texture.pixel_data, &texture_handle);



    }
    fclose(fptr);

    return true;
}


bool asset_system_unload_texture(Asset_System* asset_system, Texture_Handle texture_handle)
{
    MASSERT(false);
    //TODO:
    // texture_system_texture_free(asset_system, texture_handle);
    return false;
}

Texture_Handle asset_unload_font(Asset_System* asset_system, const char* asset_name)
{
    MASSERT(false);
}
