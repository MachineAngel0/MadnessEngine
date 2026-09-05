#include "asset_system.h"

#include "animation_system.h"
#include "material_system.h"
#include "mesh_system.h"
#include "sprite_system.h"


Asset_System* asset_system_init(Memory_System* memory_system, Reflection_Registry* global_reflection_registry)
{
    Asset_System* asset_system = memory_system_alloc(memory_system, sizeof(Asset_System),
                                                     MEMORY_SUBSYSTEM_RESOURCE);

    asset_system->global_reflection_registry = global_reflection_registry;

    asset_system->render_packet = memory_system_alloc(memory_system, sizeof(Render_Packet),
                                                      MEMORY_SUBSYSTEM_RESOURCE);

    asset_system->heap_allocator = memory_system_heap_allocator_create(memory_system, MB(256),
                                                                       MEMORY_SUBSYSTEM_RESOURCE);

    asset_system->frame_allocator = memory_system_allocator_create(memory_system, MB(64),
                                                                   MEMORY_SUBSYSTEM_RESOURCE);

    //really not using this for much rn
    asset_system->allocator = memory_system_allocator_create(memory_system, KB(16),
                                                               MEMORY_SUBSYSTEM_RESOURCE);

    //texture memory
    asset_system->texture_allocator = memory_system_heap_allocator_create(
        memory_system, MAX_TEXTURE_MEMORY_CPU, MEMORY_SUBSYSTEM_TEXTURE);
    //mesh memory
    asset_system->mesh_allocator = memory_system_heap_allocator_create(
        memory_system, MAX_MESH_MEMORY_CPU, MEMORY_SUBSYSTEM_MESH);


    //Asset Registry
    asset_system->asset_registry = memory_system_alloc(memory_system, sizeof(Asset_Registry),
                                                       MEMORY_SUBSYSTEM_RESOURCE);
    asset_registry_init(asset_system, asset_system->asset_registry, asset_system->heap_allocator, memory_system);


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


    //scan for new assets
    if (app_is_debug_build())
    {
        asset_registry_scan_for_new_assets(asset_system, asset_system->asset_registry,
                                           memory_system, ASSET_TEXTURE);
        asset_registry_scan_for_new_assets(asset_system, asset_system->asset_registry,
                                           memory_system, ASSET_FONT);

        //ASSET_STATIC_MESH: used in this case as a catch all for both normal and skeletal meshes
        asset_registry_scan_for_new_assets(asset_system, asset_system->asset_registry,
                                           memory_system, ASSET_STATIC_MESH);
    }

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
    asset_system->render_packet->mesh_queue = asset_system->mesh_system->mesh_ring_queue;
    asset_system->render_packet->skinned_mesh_queue = asset_system->mesh_system->skinned_mesh_ring_queue;
    asset_system->render_packet->texture_upload_queue = asset_system->texture_system->texture_gpu_upload_queue;

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
    Texture_Handle texture_handle = {0};

    //either load from metadata -> binary or binary blob
    //then send into the texture system
    //in general we just want to deserialize the data quickly,
    //the deserialization is the same, it just depends which file data we end up giving it

    String_Builder* string_builder = string_builder_create(512, asset_system->frame_allocator);
    string_builder_append_c_string(string_builder, ENGINE_TEXTURE_PATH);
    string_builder_append_c_string(string_builder, asset_path);
    string_builder_append_c_string(string_builder, ENGINE_TEXTURE_EXTENSION);


    String* load_asset_path = string_builder_to_string(string_builder);

    Asset_MetaData* meta_data = allocator_alloc(asset_system->frame_allocator, sizeof(Asset_MetaData));
    if (!asset_registry_exists_by_engine_path(asset_system->asset_registry, load_asset_path, meta_data))
    {
        //TODO: try to load in the asset from the import path
        MASSERT_FALSE();
        FATAL("ASSET NOT FOUND: %s", asset_path);
        return texture_handle;
    }

    //has asset already been loaded
    if (texture_system_exists(asset_system, &texture_handle, meta_data->hash))
    {
        return texture_handle;
    }


    FILE* fptr = NULL;

    //load from individal binary
    bool debug = true;
    if (debug)
    {
        fptr = fopen(string_to_c_string_allocator(load_asset_path, asset_system->frame_allocator), "rb");
        if (!fptr)
        {
            //TODO: since were in the editor, we should at least try to find the asset in our asset folder
            MASSERT(false)
            return asset_system->texture_system->default_texture_handle;
        }

        Madness_Texture_Runtime engine_texture = {0};

        asset_texture_deserialize(&engine_texture, fptr, asset_system->texture_allocator);

        texture_system_upload_new_texture(asset_system, meta_data->hash, engine_texture.texture,
                                          engine_texture.pixel_data, &texture_handle, meta_data->engine_path);
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

Texture_Handle asset_load_texture_uuid(Asset_System* asset_system, MADNESS_UUID uuid)
{
    Texture_Handle texture_handle = {0};

    //either load from metadata -> binary or binary blob
    //then send into the texture system
    //in general we just want to deserialize the data quickly,
    //the deserialization is the same, it just depends which file data we end up giving it

    Asset_MetaData* meta_data = allocator_alloc(asset_system->frame_allocator, sizeof(Asset_MetaData));
    if (!asset_registry_exists_by_uuid(asset_system->asset_registry, uuid, meta_data))
    {
        //TODO: try to load in the asset from the import path
        MASSERT_FALSE();
        FATAL("ASSET NOT FOUND: %d %d", uuid.high, uuid.low);
        return texture_handle;
    }

    //has asset already been loaded
    if (texture_system_exists(asset_system, &texture_handle, meta_data->hash))
    {
        return texture_handle;
    }


    FILE* fptr = NULL;

    //load from individal binary
    bool debug = true;
    if (debug)
    {
        fptr = fopen(string_to_c_string_allocator(meta_data->engine_path, asset_system->frame_allocator), "rb");
        if (!fptr)
        {
            //TODO: since were in the editor, we should at least try to find the asset in our asset folder
            MASSERT(false)
            return asset_system->texture_system->default_texture_handle;
        }

        Madness_Texture_Runtime engine_texture = {0};

        asset_texture_deserialize(&engine_texture, fptr, asset_system->texture_allocator);

        texture_system_upload_new_texture(asset_system, meta_data->hash, engine_texture.texture,
                                          engine_texture.pixel_data, &texture_handle, meta_data->engine_path);
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


Texture_Handle asset_load_font_path(Asset_System* asset_system, const char* engine_asset_path)
{
    Texture_Handle out_handle = (Texture_Handle){0};


    String_Builder* string_builder = string_builder_create(512, asset_system->frame_allocator);
    string_builder_append_c_string(string_builder, ENGINE_FONTS_PATH);
    string_builder_append_c_string(string_builder, engine_asset_path);
    string_builder_append_c_string(string_builder, ENGINE_FONTS_EXTENSION);

    String* load_asset_path = string_builder_to_string(string_builder);

    Asset_MetaData* meta_data = allocator_alloc(asset_system->frame_allocator, sizeof(Asset_MetaData));
    if (!asset_registry_exists_by_engine_path(asset_system->asset_registry, load_asset_path, meta_data))
    {
        //TODO: try to load in the asset from the import path
        FATAL("ASSET NOT FOUND: %s", engine_asset_path);
        return out_handle;
    }

    if (meta_data->hash == 0)
    {
        MASSERT_MSG(false, "PLZ CONVERT ASSET")
        return out_handle;
    }

    //has asset already been loaded
    if (texture_system_exists(asset_system, &out_handle, meta_data->hash))
    {
        return out_handle;
    }


    FILE* fptr = NULL;

    //load from individal binary
    bool debug = true;
    if (debug)
    {
        Madness_Font_Runtime editor_texture = {0};

        fptr = fopen(string_to_c_string_allocator(load_asset_path, asset_system->frame_allocator), "rb");
        if (!fptr)
        {
            MASSERT(false);
        }

        asset_font_deserialize(&editor_texture, fptr, asset_system->texture_allocator);

        texture_system_upload_new_font(asset_system, meta_data->uuid, meta_data->hash, editor_texture.texture,
                                       editor_texture.font_texture,
                                       editor_texture.pixel_data, &out_handle, meta_data->engine_path);
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

    return out_handle;
}

Texture_Handle asset_load_font_uuid(Asset_System* asset_system, MADNESS_UUID uuid)
{
    Texture_Handle out_handle = (Texture_Handle){0};
    Asset_MetaData* meta_data = NULL;

    if (uuid.high == 0 && uuid.low == 0)
    {
        WARN("UUID OF 0,0 passed in ")
        return out_handle;
    }

    if (!asset_registry_exists_by_uuid(asset_system->asset_registry, uuid, meta_data))
    {
        MASSERT_MSG(false, "PLZ CONVERT ASSET")
        return out_handle;
    }


    //has asset already been loaded
    if (texture_system_exists(asset_system, &out_handle, meta_data->hash))
    {
        return out_handle;
    }


    FILE* fptr = fopen(string_to_c_string_allocator(meta_data->engine_path, asset_system->frame_allocator), "rb");
    if (!fptr)
    {
        MASSERT(false);
        return out_handle;
    }

    bool editor = true;
    if (editor)
    {
        Madness_Texture_Runtime runtime = {0};
        asset_texture_deserialize(&runtime, fptr, asset_system->texture_allocator);
        texture_system_upload_new_texture(asset_system, meta_data->hash, runtime.texture, runtime.pixel_data,
                                          &out_handle, meta_data->engine_path);
    }
    else
    {
        MASSERT(false);
    }

    fclose(fptr);

    return out_handle;
}


bool asset_system_unload_texture(Asset_System* asset_system, Texture_Handle texture_handle)
{
    MASSERT(false);
    //TODO:
    // texture_system_texture_free(asset_system, texture_handle);
    return false;
}


bool asset_unload_font(Asset_System* asset_system, Texture_Handle texture_handle)
{
    MASSERT(false);
    return false;
}

Madness_Mesh_Handle asset_load_mesh_uuid(Asset_System* asset_system, MADNESS_UUID uuid)
{
    Madness_Mesh_Handle handle = (Madness_Mesh_Handle){0};
    MASSERT(false);
    return handle;
}

Madness_Mesh_Handle asset_load_mesh_path(Asset_System* asset_system, const char* engine_asset_path)
{
    Scratch_Allocator scratch = scratch_allocator_begin(asset_system->allocator);
    String* asset_path_string = STRING_CREATE_FROM_BUFFER_ALLOCATOR(engine_asset_path, scratch.allocator);

    Madness_Mesh_Handle mesh_handle = {0};
    Asset_MetaData* out_meta_data = allocator_alloc(scratch.allocator, sizeof(Asset_MetaData));
    if (!asset_registry_exists_by_engine_path(asset_system->asset_registry, asset_path_string, out_meta_data))
    {
        MASSERT_MSG(false, "PLZ CONVERT ASSET")
        return mesh_handle;
    }

    //has asset already been loaded
    if (mesh_system_exists_mesh(asset_system, &mesh_handle, out_meta_data->hash, out_meta_data->uuid))
    {
        return mesh_handle;
    }


    FILE* fptr = NULL;

    //load from individal binary
    bool debug = true;
    if (debug)
    {
        Madness_Mesh_Runtime runtime_mesh = {0};

        fptr = fopen(engine_asset_path, "rb");


        asset_mesh_deserialize(&runtime_mesh, fptr, asset_system->heap_allocator);

        mesh_system_load_mesh(asset_system, &runtime_mesh, out_meta_data->hash, out_meta_data->engine_path,
                              out_meta_data->uuid, &mesh_handle);
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

    scratch_allocator_end(scratch);

    return mesh_handle;
}

Madness_SkMesh_Handle asset_load_skmesh(Asset_System* asset_system, const char* engine_asset_path)
{
    Madness_SkMesh_Handle out_handle = (Madness_SkMesh_Handle){0};

    String* asset_path_string = STRING_CREATE_FROM_BUFFER_ALLOCATOR(engine_asset_path, asset_system->frame_allocator);

    Asset_MetaData* out_meta_data = allocator_alloc(asset_system->frame_allocator, sizeof(Asset_MetaData));;
    if (!asset_registry_exists_by_engine_path(asset_system->asset_registry, asset_path_string, out_meta_data))
    {
        MASSERT_MSG(false, "PLZ CONVERT ASSET")
        return out_handle;
    }

    //has asset already been loaded
    if (mesh_system_exists_skmesh(asset_system, &out_handle, out_meta_data->hash))
    {
        return out_handle;
    }


    FILE* fptr = NULL;

    //load from individal binary
    bool debug = true;
    if (debug)
    {
        Madness_SkMesh_Runtime runtime_mesh = {0};

        fptr = fopen(engine_asset_path, "rb");

        asset_skmesh_deserialize(&runtime_mesh, fptr, asset_system->heap_allocator);

        mesh_system_load_skinned_mesh(asset_system, &runtime_mesh, out_meta_data->hash, out_meta_data->engine_path,
                                      out_meta_data->uuid);
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

    return out_handle;
}


bool asset_load_material_asset_path(Asset_System* asset_system, const char* asset_path)
{
    String* asset_path_string = STRING_CREATE_FROM_BUFFER_ALLOCATOR(asset_path, asset_system->frame_allocator);

    Asset_MetaData* out_meta_data = allocator_alloc(asset_system->frame_allocator, sizeof(Asset_MetaData));
    if (!asset_registry_exists_by_engine_path(asset_system->asset_registry, asset_path_string, out_meta_data))
    {
        MASSERT_MSG(false, "PLZ CONVERT ASSET")
        return false;
    }


    //material system does exists function
    //has asset already been loaded
    if (material_system_material_batch_exists(asset_system, out_meta_data->uuid))
    {
        return true;
    }

    FILE* fptr = NULL;
    bool debug = true;
    if (debug)
    {
        fptr = fopen(string_to_c_string_allocator(out_meta_data->engine_path, asset_system->frame_allocator), "rb");

        if (!fptr)
        {
            MASSERT(false);
            return false;
        }


        Material_Asset_Runtime runtime_material = {0};
        runtime_material.asset = allocator_heap_alloc(asset_system->heap_allocator, sizeof(Madness_Mesh));
        asset_material_asset_deserialize(&runtime_material, fptr, asset_system->heap_allocator);
        material_system_load_material_asset(asset_system, out_meta_data->uuid, out_meta_data->hash, &runtime_material);
    }
    else
    {
        MASSERT(false);
        //TODO:
    }

    fclose(fptr);


    return true;
}

bool asset_load_material_asset_uuid(Asset_System* asset_system, MADNESS_UUID uuid)
{
    Asset_MetaData* out_meta_data = allocator_alloc(asset_system->frame_allocator, sizeof(Asset_MetaData));;
    if (!asset_registry_exists_by_uuid(asset_system->asset_registry, uuid, out_meta_data))
    {
        MASSERT_MSG(false, "PLZ CONVERT ASSET")
        return false;
    }


    //material system does exists function
    //has asset already been loaded
    if (material_system_material_batch_exists(asset_system, out_meta_data->uuid))
    {
        return true;
    }

    FILE* fptr = NULL;
    bool debug = true;
    if (debug)
    {
        fptr = fopen(string_to_c_string_allocator(out_meta_data->engine_path, asset_system->frame_allocator), "rb");

        if (!fptr)
        {
            MASSERT(false);
            return false;
        }


        Material_Asset_Runtime runtime_material = {0};
        runtime_material.asset = allocator_heap_alloc(asset_system->heap_allocator, sizeof(Madness_Mesh));
        asset_material_asset_deserialize(&runtime_material, fptr, asset_system->heap_allocator);
        material_system_load_material_asset(asset_system, out_meta_data->uuid, out_meta_data->hash, &runtime_material);
    }
    else
    {
        MASSERT(false);
        //TODO:
    }

    fclose(fptr);


    return true;
}

bool asset_load_material_instance(Asset_System* asset_system, const char* asset_path)
{
    MASSERT(false);
    return false;
}
