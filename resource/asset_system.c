#include "asset_system.h"

#include "animation_system.h"
#include "material_system.h"
#include "mesh_system.h"
#include "sprite_system.h"


Asset_System* asset_system_init(Memory_System* memory_system, Reflection_Registry* global_reflection_registry,
                                Reflection_Registry* material_reflection_registry)
{
    Asset_System* asset_system = memory_system_alloc(memory_system, sizeof(Asset_System),
                                                     MEMORY_SUBSYSTEM_RESOURCE);

    asset_system->global_reflection_registry = global_reflection_registry;
    asset_system->material_reflection_registry = material_reflection_registry;

    asset_system->render_packet = memory_system_alloc(memory_system, sizeof(Render_Packet),
                                                      MEMORY_SUBSYSTEM_RESOURCE);

    asset_system->heap_allocator = memory_system_heap_allocator_create(memory_system, MB(256),
                                                                       MEMORY_SUBSYSTEM_RESOURCE);

    asset_system->frame_allocator = memory_system_allocator_create(memory_system, MB(4),
                                                                   MEMORY_SUBSYSTEM_RESOURCE);

    //really not using this for much rn
    asset_system->allocator = memory_system_allocator_create(memory_system, MB(16),
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
    PROFILE_ZONE(asset_system_update_and_create_render_packet)

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

    PROFILE_ZONE_END(asset_system_update_and_create_render_packet)

    return true;
}


void render_packet_clear(Render_Packet* renderer_packets)
{
    memset(renderer_packets, 0, sizeof(Render_Packet));
}

Texture_Handle asset_load_texture_path(Asset_System* asset_system, const char* asset_path)
{
    PROFILE_ZONE(asset_load_texture_path)


    Scratch_Allocator scratch = scratch_allocator_begin(asset_system->allocator);


    Texture_Handle texture_handle = {0};

    //either load from metadata -> binary or binary blob
    //then send into the texture system
    //in general we just want to deserialize the data quickly,
    //the deserialization is the same, it just depends which file data we end up giving it

    String_Builder* string_builder = string_builder_create(512, scratch.allocator);
    string_builder_append_c_string(string_builder, ENGINE_TEXTURE_PATH);
    string_builder_append_c_string(string_builder, asset_path);
    string_builder_append_c_string(string_builder, ENGINE_TEXTURE_EXTENSION);


    String* load_asset_path = string_builder_to_string(string_builder);

    Asset_MetaData* meta_data = allocator_alloc(scratch.allocator, sizeof(Asset_MetaData));
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
        fptr = fopen(string_to_c_string_allocator(load_asset_path, scratch.allocator), "rb");
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


    scratch_allocator_end(scratch);

    PROFILE_ZONE_END(asset_load_texture_path)


    return texture_handle;
}

Texture_Handle asset_load_texture_uuid(Asset_System* asset_system, MADNESS_UUID uuid)
{
    PROFILE_ZONE(asset_load_texture_uuid)


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

    PROFILE_ZONE_END(asset_load_texture_uuid)


    return texture_handle;
}


Texture_Handle asset_load_font_path(Asset_System* asset_system, const char* engine_asset_path)
{
    PROFILE_ZONE(asset_load_font_path)


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

    PROFILE_ZONE_END(asset_load_font_path)


    return out_handle;
}

Texture_Handle asset_load_font_uuid(Asset_System* asset_system, MADNESS_UUID uuid)
{
    PROFILE_ZONE(asset_load_font_uuid)


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

    PROFILE_ZONE_END(asset_load_font_uuid)


    return out_handle;
}


bool asset_system_unload_texture(Asset_System* asset_system, Texture_Handle texture_handle)
{
    PROFILE_ZONE(asset_system_unload_texture)


    MASSERT(false);
    //TODO:
    // texture_system_texture_free(asset_system, texture_handle);

    PROFILE_ZONE_END(asset_system_unload_texture)


    return false;
}


bool asset_unload_font(Asset_System* asset_system, Texture_Handle texture_handle)
{
    PROFILE_ZONE(asset_unload_font)

    MASSERT(false);

    PROFILE_ZONE_END(asset_unload_font)

    return false;
}

Mesh_Handle asset_load_mesh_uuid(Asset_System* asset_system, MADNESS_UUID uuid)
{
    Mesh_Handle handle = (Mesh_Handle){0};
    MASSERT(false);
    return handle;
}

bool asset_load_mesh_path(Asset_System* asset_system, const char* engine_asset_path, Mesh_Handle* out_handle)
{
    MASSERT(asset_system);
    MASSERT(engine_asset_path);
    MASSERT(out_handle);

    PROFILE_ZONE(asset_load_mesh_path)


    Scratch_Allocator scratch = scratch_allocator_begin(asset_system->allocator);
    String* asset_path_string = STRING_CREATE_FROM_BUFFER_ALLOCATOR(engine_asset_path, scratch.allocator);

    Asset_MetaData* out_meta_data = allocator_alloc(scratch.allocator, sizeof(Asset_MetaData));
    if (!asset_registry_exists_by_engine_path(asset_system->asset_registry, asset_path_string, out_meta_data))
    {
        MASSERT_MSG(false, "PLZ CONVERT ASSET")
        *out_handle = (Mesh_Handle){0, 0};
        return false;
    }

    //TODO: has asset already been loaded


    //steps:
    // load mesh and all dependencies like materials
    // set the data and get back the handle
    FILE* fptr = fopen(engine_asset_path, "rb");
    Madness_Mesh_Runtime runtime_mesh = {0};
    asset_mesh_deserialize(&runtime_mesh, fptr, asset_system->heap_allocator);
    fclose(fptr);


    //load in the materials and textures
    Material_Handle* material_handles = allocator_alloc(scratch.allocator, sizeof(Material_Handle) * runtime_mesh.mesh_count);
    for (u32 i = 0; i < runtime_mesh.mesh_count; i++)
    {
        asset_load_material_uuid(asset_system, runtime_mesh.material_uuid[i], &material_handles[i]);
    }


    if (!mesh_acquire(asset_system, &runtime_mesh, out_handle))
    {
        MASSERT(false);
    }

    Madness_Asset* mesh_asset = &asset_system->asset_registry->mesh_asset[out_handle->handle];
    madness_asset_set_info(mesh_asset, out_meta_data, ASSET_STATIC_MESH);


    scratch_allocator_end(scratch);

    PROFILE_ZONE_END(asset_load_mesh_path)


    return true;
}

Skinned_Mesh_Handle asset_load_skmesh(Asset_System* asset_system, const char* engine_asset_path)
{
    PROFILE_ZONE(asset_load_skmesh)


    Skinned_Mesh_Handle out_handle = (Skinned_Mesh_Handle){0};

    String* asset_path_string = STRING_CREATE_FROM_BUFFER_ALLOCATOR(engine_asset_path, asset_system->frame_allocator);

    Asset_MetaData* out_meta_data = allocator_alloc(asset_system->frame_allocator, sizeof(Asset_MetaData));;
    if (!asset_registry_exists_by_engine_path(asset_system->asset_registry, asset_path_string, out_meta_data))
    {
        MASSERT_MSG(false, "PLZ CONVERT ASSET")
        return out_handle;
    }

    //TODO: check if has asset already been loaded



    FILE* fptr = fopen(engine_asset_path, "rb");

    if (!fptr)
    {
        MASSERT(false);
        // return false;
    }

    Madness_SkMesh_Runtime runtime_mesh = {0};
    asset_skmesh_deserialize(&runtime_mesh, fptr, asset_system->heap_allocator);

    mesh_system_load_skinned_mesh(asset_system, &runtime_mesh, out_meta_data->hash, out_meta_data->engine_path,
                                  out_meta_data->uuid);

    fclose(fptr);

    PROFILE_ZONE_END(asset_load_skmesh)


    return out_handle;
}

bool _asset_load_shader_asset(Asset_System* asset_system, Asset_MetaData* meta_data,
                              Shader_Handle* out_handle, Scratch_Allocator* scratch_allocator)
{
    //check if the shader asset has already been loaded
    for (u32 i = 0; i < MAX_MATERIAL_COUNT; i++)
    {
        if (string_compare(asset_system->asset_registry->shader_madness_asset[i].engine_path,
                           meta_data->engine_path))
        {
            *out_handle = (Shader_Handle){
                .handle = i,
                .generation = asset_system->material_system->shader_asset_generation,
            };
            return true;
        }
    }


    FILE* fptr = fopen(string_to_c_string_allocator(meta_data->engine_path, scratch_allocator->allocator), "rb");

    if (!fptr)
    {
        MASSERT(false);
        return false;
    }


    Shader_Asset* shader_asset = shader_asset_acquire(asset_system->material_system, out_handle);
    asset_shader_deserialize(shader_asset, fptr, asset_system->heap_allocator);

    shader_asset_load_definitions(asset_system, shader_asset);


    Madness_Asset* madness_asset = &asset_system->asset_registry->shader_madness_asset[out_handle->handle];
    madness_asset->path_hash = meta_data->hash;
    madness_asset->engine_path = meta_data->engine_path;
    madness_asset->type = ASSET_MATERIAL;
    madness_asset->reference_count = 1;

    fclose(fptr);
}


bool asset_load_shader_asset_path(Asset_System* asset_system, const char* asset_path,
                                  Shader_Handle* out_shader_handle)
{
    MASSERT(asset_system)
    MASSERT(asset_path)
    MASSERT(out_shader_handle)

    PROFILE_ZONE(asset_load_material_asset_path)

    Scratch_Allocator scratch = scratch_allocator_begin(asset_system->allocator);

    String* asset_path_string = STRING_CREATE_FROM_BUFFER_ALLOCATOR(asset_path, scratch.allocator);

    Asset_MetaData* out_meta_data = allocator_alloc(asset_system->frame_allocator, sizeof(Asset_MetaData));
    if (!asset_registry_exists_by_engine_path(asset_system->asset_registry, asset_path_string, out_meta_data))
    {
        MASSERT_MSG(false, "PLZ CONVERT ASSET")
        return false;
    }


    //check if the shader asset has already been loaded
    for (u32 i = 0; i < MAX_MATERIAL_COUNT; i++)
    {
        if (asset_system->asset_registry->shader_madness_asset[i].path_hash == 0) { continue; }
        if (string_compare(asset_system->asset_registry->shader_madness_asset[i].engine_path,
                           out_meta_data->engine_path))
        {
            *out_shader_handle = (Shader_Handle){
                .handle = i,
                .generation = asset_system->material_system->shader_asset_generation[i],
            };
            PROFILE_ZONE_END(asset_load_material_asset_path)
            return true;
        }
    }

    bool load_result = _asset_load_shader_asset(asset_system, out_meta_data,
                                                out_shader_handle, &scratch);


    scratch_allocator_end(scratch);
    PROFILE_ZONE_END(asset_load_material_asset_path)

    return load_result;
}

bool asset_load_shader_asset_uuid(Asset_System* asset_system, MADNESS_UUID uuid, Shader_Handle* out_handle)
{
    PROFILE_ZONE(asset_load_material_asset_uuid)

    Scratch_Allocator scratch = scratch_allocator_begin(asset_system->allocator);

    Asset_MetaData* out_meta_data = allocator_alloc(asset_system->frame_allocator, sizeof(Asset_MetaData));;
    if (!asset_registry_exists_by_uuid(asset_system->asset_registry, uuid, out_meta_data))
    {
        MASSERT_MSG(false, "PLZ CONVERT ASSET")
        return false;
    }


    bool load_result = _asset_load_shader_asset(asset_system, out_meta_data,
                                                out_handle, &scratch);

    scratch_allocator_end(scratch);


    PROFILE_ZONE_END(asset_load_material_asset_uuid)


    return load_result;
}

bool asset_unload_shader_asset(Asset_System* asset_system, Shader_Handle shader_handle)
{
    PROFILE_ZONE(asset_unload_material_asset)
    MASSERT(asset_system);


    Madness_Asset* madness_asset = &asset_system->asset_registry->shader_madness_asset[shader_handle.handle];
    if (asset_system->material_system->shader_asset_generation[shader_handle.handle] == shader_handle.generation)
    {
        madness_asset->reference_count--;
        if (madness_asset->reference_count <= 0)
        {
            //unload the asset
            shader_asset_release(asset_system->material_system, shader_handle);
        }
    }
    else
    {
        WARN("asset_unload_material_asset: trying to unload an invalid generation")
    }


    PROFILE_ZONE_END(asset_unload_material_asset)


    return true;
}


bool _asset_load_material(Asset_System* asset_system, Asset_MetaData* meta_data, Material_Handle* out_material_handle)
{
    MASSERT(asset_system);
    MASSERT(meta_data);
    MASSERT(out_material_handle);


    Scratch_Allocator scratch = scratch_allocator_begin(asset_system->allocator);


    FILE* fptr = fopen(string_to_c_string_allocator(meta_data->engine_path, scratch.allocator), "rb");

    if (!fptr)
    {
        MASSERT(false);
        return false;
    }

    Material material = {0};
    asset_material_deserialize(&material, fptr, asset_system->heap_allocator);

    asset_load_shader_asset_uuid(asset_system, material.meta_data.material_uuid, &material.meta_data.shader_handle);

    if (!material_acquire(asset_system, material.meta_data.shader_handle, &material,
                          out_material_handle))
    {
        MASSERT(false);
    }




    scratch_allocator_end(scratch);

    return false;
}

bool asset_load_material_uuid(Asset_System* asset_system, MADNESS_UUID madness_uuid, Material_Handle* out_material)
{
    MASSERT(asset_system)
    MASSERT(out_material)

    PROFILE_ZONE(asset_load_material_uuid)


    Scratch_Allocator scratch = scratch_allocator_begin(asset_system->allocator);


    Asset_MetaData* out_meta_data = allocator_alloc(scratch.allocator, sizeof(Asset_MetaData));;
    if (!asset_registry_exists_by_uuid(asset_system->asset_registry, madness_uuid, out_meta_data))
    {
        MASSERT_MSG(false, "PLZ CONVERT ASSET")
        return false;
    }


    bool load_result = _asset_load_material(asset_system, out_meta_data, out_material);
    scratch_allocator_end(scratch);

    PROFILE_ZONE_END(asset_load_material_uuid)

    return load_result;
}

bool asset_load_material_path(Asset_System* asset_system, const char* asset_path, Material_Handle* out_material)
{
    Scratch_Allocator scratch = scratch_allocator_begin(asset_system->allocator);

    String* asset_path_string = STRING_CREATE_FROM_BUFFER_ALLOCATOR(asset_path, scratch.allocator);

    Asset_MetaData* out_meta_data = allocator_alloc(scratch.allocator, sizeof(Asset_MetaData));;
    if (!asset_registry_exists_by_engine_path(asset_system->asset_registry, asset_path_string, out_meta_data))
    {
        MASSERT_MSG(false, "PLZ CONVERT ASSET")
        return false;
    }


    bool load_result = _asset_load_material(asset_system, out_meta_data, out_material);


    scratch_allocator_end(scratch);

    PROFILE_ZONE_END(asset_load_material_uuid)


    return load_result;
}


bool asset_load_particle_effect_by_path(Asset_System* asset_system, const char* asset_path,
                                        Particle_Effect_Handle* out_handle)

{
    MASSERT(asset_system);
    MASSERT(asset_path);
    MASSERT(out_handle);

    PROFILE_ZONE(asset_load_particle_effect_by_path)


    Scratch_Allocator scratch = scratch_allocator_begin(asset_system->allocator);

    String* path_string = STRING_CREATE_FROM_BUFFER_ALLOCATOR(asset_path, scratch.allocator);

    Asset_MetaData* out_meta_data = allocator_alloc(asset_system->frame_allocator, sizeof(Asset_MetaData));
    if (!asset_registry_exists_by_engine_path(asset_system->asset_registry, path_string, out_meta_data))
    {
        MASSERT_MSG(false, "PLZ CONVERT ASSET")
        *out_handle = asset_system->particle_system->default_effect_handle;
        return false;
    }

    //see if asset is already loaded
    for (u32 i = 0; i < MAX_PARTICLE_EFFECTS_COUNT; i++)
    {
        Madness_Asset* asset = &asset_system->asset_registry->particle_effect_asset[i];
        if (asset->path_hash == 0) { continue; }

        if (string_compare(asset->engine_path, path_string))
        {
            asset->reference_count++;
            *out_handle = (Particle_Effect_Handle){i, asset_system->particle_system->particle_generation[i]};
            return true;
        }
    }

    //the asset isn't loaded, so we load it in asset
    FILE* fptr = fopen(string_to_c_string_allocator(path_string, scratch.allocator), "rb");

    if (!fptr)
    {
        MASSERT(false);
        return false;
    }


    //grab an available particle effect, with its handle
    Particle_Effect* particle_effect = particle_effect_acquire(asset_system->particle_system, out_handle);

    if (!particle_effect)
    {
        MASSERT_MSG_FALSE("COULD NOT FIND A PARTICLE EFFECT NOT LOADING IN PARTICLE");

        *out_handle = (Particle_Effect_Handle){0, 0};
        return false;
    }

    particle_effect_deserialize(particle_effect, fptr, asset_system->heap_allocator);

    fclose(fptr);

    Madness_Asset* particle_asset = &asset_system->asset_registry->particle_effect_asset[out_handle->handle];
    madness_asset_set_info(particle_asset, out_meta_data, ASSET_PARTICLE_EFFECT);


    scratch_allocator_end(scratch);

    PROFILE_ZONE_END(asset_load_particle_effect_by_path)


    return true;
}


bool asset_unload_particle_effect(Asset_System* asset_system, Particle_Effect_Handle out_handle)
{
    PROFILE_ZONE(asset_unload_particle_effect)

    MASSERT(asset_system);

    Madness_Asset* asset = &asset_system->asset_registry->particle_effect_asset[out_handle.handle];

    if (asset_system->particle_system->particle_generation[out_handle.handle] == out_handle.gen)
    {
        if (madness_asset_release(asset))
        {
            particle_effect_release(asset_system->particle_system, out_handle);
        }
    }


    PROFILE_ZONE_END(asset_unload_particle_effect)

    return true;
}

bool asset_unload_particle_emitter(Asset_System* asset_system, Particle_Emitter_Handle handle)
{
    PROFILE_ZONE(asset_unload_particle_effect)

    MASSERT(asset_system);

    Madness_Asset* asset = &asset_system->asset_registry->particle_effect_asset[handle.handle];

    if (asset_system->particle_system->emitter_generation[handle.handle] == handle.gen)
    {
        if (madness_asset_release(asset))
        {
            particle_emitter_release(asset_system->particle_system, handle);
        }
    }


    PROFILE_ZONE_END(asset_unload_particle_effect)

    return true;
}

bool asset_unload_particle_emitter_by_asset_path(Asset_System* asset_system, const char* asset_path)
{
    MASSERT_FALSE();
}
