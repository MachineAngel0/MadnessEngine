#include "asset_system.h"

#include "animation_system.h"
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


    asset_system->heap_allocator = memory_system_heap_allocator_create(memory_system, MB(256),
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

    asset_system->animation_system = animation_init(memory_system);


    return asset_system;
}


bool asset_system_shutdown(Asset_System* resource_system, Memory_System* memory_system)
{
    //shutdown the resource systems
    animation_deinit(resource_system->animation_system, memory_system);
    particle_system_shutdown(resource_system->particle_system, memory_system);
    mesh_system_shutdown(resource_system->mesh_system, memory_system);
    sprite_system_shutdown(resource_system->sprite_system, memory_system);
    texture_system_shutdown(resource_system->texture_system, memory_system);
    scene_shutdown(resource_system->scene, memory_system);
    material_system_shutdown(resource_system->material_system, memory_system);
    return true;
}


bool asset_system_update_and_create_render_packet(Asset_System* asset_system)
{
    allocator_clear(asset_system->frame_allocator);
    render_packet_clear(asset_system->render_packet);
    asset_system->render_packet->texture_queue = asset_system->texture_system->texture_upload_queue;
    asset_system->render_packet->mesh_queue = asset_system->mesh_system->mesh_ring_queue;
    asset_system->render_packet->skinned_mesh_queue = asset_system->mesh_system->skinned_mesh_ring_queue;
    //

    sprite_system_generate_render_packet(asset_system->sprite_system,
                                         &asset_system->render_packet->sprite_data_packet);

    material_system_generate_render_packet(asset_system->material_system,
                                           &asset_system->render_packet->draw_3d_data_packet);

    scene_update(asset_system->scene, asset_system);
    asset_system->render_packet->draw_3d_data_packet.world_space_matrix_array = asset_system->scene->
        world_transforms;
    asset_system->render_packet->draw_3d_data_packet.world_space_matrix_count = asset_system->scene->
        transform_count;

    asset_system->render_packet->draw_3d_data_packet.skinned_matrix = asset_system->animation_system->
        skinned_matrix_array;

    asset_system->render_packet->particle_packet = particle_system_generate_render_packet(
        asset_system->particle_system);

    asset_system->render_packet->draw_3d_data_packet.mesh_instances = asset_system->mesh_system->
        mesh_instance;
    asset_system->render_packet->draw_3d_data_packet.mesh_instances_count = asset_system->mesh_system->
        mesh_instance_count;

    asset_system->render_packet->draw_3d_data_packet.skinned_instances = asset_system->mesh_system->
        skinned_mesh_instance;
    asset_system->render_packet->draw_3d_data_packet.skinned_instances_count = asset_system->mesh_system->
        skinned_mesh_instance_count;

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

bool asset_load_texture_uuid(Asset_System* asset_system, MADNESS_UUID uuid, Texture_Handle* out_handle)
{
    Asset_MetaData meta_data = {0};

    if (uuid.high == 0 && uuid.low == 0)
    {
        WARN("UUID OF 0,0 passed in ")
        *out_handle = (Texture_Handle){0};
        return true;
    }

    if (!asset_registry_get_metadata_from_uuid(asset_system, uuid, &meta_data))
    {
        MASSERT_MSG(false, "PLZ CONVERT ASSET")
        *out_handle = (Texture_Handle){0};
        return out_handle;
    }


    //has asset already been loaded
    if (texture_system_exists(asset_system, out_handle, meta_data.hash))
    {
        return true;
    }


    FILE* fptr = fopen(string_to_c_string_allocator(meta_data.binary_file, asset_system->frame_allocator), "rb");
    if (!fptr)
    {
        MASSERT(false);
        *out_handle = (Texture_Handle){0};
        return false;
    }

    bool editor = true;
    if (editor)
    {
        Madness_Texture_Runtime runtime = {0};
        asset_texture_deserialize_heap(&runtime, fptr, asset_system->heap_allocator);
        texture_system_upload_new_texture(asset_system, meta_data.uuid, meta_data.hash, runtime.texture,
                                          runtime.pixel_data, out_handle);
        return true;
    }
    else
    {
        MASSERT(false);
    }

    fclose(fptr);

    return true;
}

bool asset_unload_font(Asset_System* asset_system, Texture_Handle texture_handle)
{
    MASSERT(false);
    return false;
}

bool asset_load_mesh_uuid(Asset_System* asset_system, MADNESS_UUID* uuid, Madness_Mesh_Handle* out_handle)
{
    MASSERT(false);
    return false;
}

bool asset_load_mesh_path(Asset_System* asset_system, const char* engine_asset_path, Madness_Mesh_Handle* out_handle)
{
    MADNESS_UUID uuid = {0, 0};
    u64 hash = 0;

    String* asset_path_string = STRING_CREATE_FROM_BUFFER_ALLOCATOR(engine_asset_path, asset_system->frame_allocator);

    if (!asset_registry_exists_by_engine_path(asset_system, asset_path_string, &uuid, &hash))
    {
        MASSERT_MSG(false, "PLZ CONVERT ASSET")
        *out_handle = (Madness_Mesh_Handle){0};
        return out_handle;
    }

    //has asset already been loaded
    if (mesh_system_exists_mesh(asset_system, out_handle, uuid, hash))
    {
        return true;
    }


    FILE* fptr = NULL;

    //load from individal binary
    bool debug = true;
    if (debug)
    {
        Madness_Mesh_Runtime runtime_mesh = {0};

        fptr = fopen(engine_asset_path, "rb");

        asset_mesh_deserialize_heap(&runtime_mesh, fptr, asset_system->heap_allocator);

        mesh_system_load_mesh(asset_system, &runtime_mesh, uuid, hash);
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

bool asset_load_skmesh_path(Asset_System* asset_system, const char* engine_asset_path,
                            Madness_SkMesh_Handle* out_handle)
{
    MADNESS_UUID uuid = {0, 0};
    u64 hash = 0;

    String* asset_path_string = STRING_CREATE_FROM_BUFFER_ALLOCATOR(engine_asset_path, asset_system->frame_allocator);

    if (!asset_registry_exists_by_engine_path(asset_system, asset_path_string, &uuid, &hash))
    {
        MASSERT_MSG(false, "PLZ CONVERT ASSET")
        *out_handle = (Madness_SkMesh_Handle){0};
        return out_handle;
    }

    //has asset already been loaded
    if (mesh_system_exists_skmesh(asset_system, out_handle, uuid, hash))
    {
        return true;
    }


    FILE* fptr = NULL;

    //load from individal binary
    bool debug = true;
    if (debug)
    {
        Madness_SkMesh_Runtime runtime_mesh = {0};

        fptr = fopen(engine_asset_path, "rb");

        asset_skmesh_deserialize_heap(&runtime_mesh, fptr, asset_system->heap_allocator);

        mesh_system_load_skinned_mesh(asset_system, &runtime_mesh, uuid, hash);
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

bool asset_load_material_asset_uuid(Asset_System* asset_system, MADNESS_UUID uuid)
{
    Asset_MetaData meta_data = {0};
    if (!asset_registry_get_metadata_from_uuid(asset_system, uuid, &meta_data))
    {
        MASSERT("false");
        return false;
    }


    //material system does exists function
    //has asset already been loaded
    if (material_system_exists(asset_system, meta_data.uuid))
    {
        return true;
    }

    FILE* fptr = NULL;
    bool debug = true;
    if (debug)
    {
        fptr = fopen(string_to_c_string_allocator(meta_data.binary_file, asset_system->frame_allocator), "rb");

        if (!fptr)
        {
            MASSERT(false);
            return false;
        }


        Material_Asset_Runtime runtime_material = {0};
        runtime_material.asset = allocator_heap_alloc(asset_system->heap_allocator, sizeof(Madness_Mesh));
        asset_material_deserialize_heap(&runtime_material, fptr, asset_system->heap_allocator);
        material_system_load_material_asset(asset_system, meta_data.uuid, meta_data.hash, &runtime_material);
    }
    else
    {
        MASSERT(false);
        //TODO:
    }

    fclose(fptr);


    return true;
}
