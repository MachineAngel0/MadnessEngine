#include "asset_system.h"

#include "animation_system.h"
#include "material_system.h"
#include "shader_system.h"
#include "mesh_system.h"
#include "sprite_system.h"
#include "texture_system.h"
#include "asset_registry.h"
#include "asset_serialization.h"

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
                                                                       MEMORY_SUBSYSTEM_RESOURCE, "asset heap");

    asset_system->frame_allocator = memory_system_allocator_create(memory_system, MB(4),
                                                                   MEMORY_SUBSYSTEM_RESOURCE, "asset frame");

    asset_system->allocator = memory_system_allocator_create(memory_system, MB(16),
                                                             MEMORY_SUBSYSTEM_RESOURCE, "asset persistent");
    asset_system->scratch_allocator = memory_system_allocator_create(memory_system, MB(16),
                                                                     MEMORY_SUBSYSTEM_RESOURCE, "asset scratch");


    //texture memory
    asset_system->texture_allocator = memory_system_heap_allocator_create(
        memory_system, MAX_TEXTURE_MEMORY_CPU, MEMORY_SUBSYSTEM_TEXTURE, "texture");
    //mesh memory
    asset_system->mesh_allocator = memory_system_heap_allocator_create(
        memory_system, MAX_MESH_MEMORY_CPU, MEMORY_SUBSYSTEM_MESH, "mesh");


    //Asset Registry
    asset_system->asset_registry = memory_system_alloc(memory_system, sizeof(Asset_Registry),
                                                       MEMORY_SUBSYSTEM_RESOURCE);
    asset_registry_init(asset_system, asset_system->asset_registry, asset_system->heap_allocator, memory_system);


    //Texture
    asset_system->texture_system = memory_system_alloc(memory_system, sizeof(Texture_System),
                                                       MEMORY_SUBSYSTEM_TEXTURE);
    texture_system_init(asset_system, asset_system->texture_system, memory_system);


    asset_system->scene = scene_init(memory_system);

    asset_system->shader_system = memory_system_alloc(memory_system, sizeof(Shader_System),
                                                      MEMORY_SUBSYSTEM_RESOURCE);
    shader_system_init(asset_system->shader_system, asset_system, memory_system);

    asset_system->material_system = memory_system_alloc(memory_system, sizeof(Material_System2),
                                                        MEMORY_SUBSYSTEM_RESOURCE);
    material_system_init(asset_system->material_system, memory_system);

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
    material_system_deinit(resource_system->material_system, memory_system);
    return true;
}


bool asset_system_generate_render_packet(Asset_System* asset_system)
{
    PROFILE_ZONE(asset_system_update_and_create_render_packet)

    allocator_clear(asset_system->frame_allocator);
    render_packet_clear(asset_system->render_packet);
    asset_system->render_packet->mesh_queue = asset_system->mesh_system->mesh_ring_queue;
    asset_system->render_packet->skinned_mesh_queue = asset_system->mesh_system->skinned_mesh_ring_queue;
    asset_system->render_packet->texture_upload_queue = asset_system->texture_system->texture_gpu_upload_queue;

    sprite_system_generate_render_packet(asset_system->sprite_system,
                                         &asset_system->render_packet->sprite_data_packet);

    material_system_render_packet(asset_system->material_system, asset_system->render_packet);
    shader_system_render_packet(asset_system->shader_system, asset_system->render_packet);

    scene_update(asset_system->scene, asset_system);


    //animation
    asset_system->render_packet->draw_3d_data_packet.world_space_matrix_array = asset_system->scene->
        world_transforms;
    asset_system->render_packet->draw_3d_data_packet.world_space_matrix_count = asset_system->scene->
        transform_count;

    asset_system->render_packet->draw_3d_data_packet.skinned_matrix = asset_system->animation_system->
        skinned_matrix_array;

    //particle
    asset_system->render_packet->particle_packet = particle_system_generate_render_packet(
        asset_system->particle_system);

    //mesh
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

bool asset_unload_material(Asset_System* asset_system, const char* asset_path)
{
}


void render_packet_clear(Render_Packet* renderer_packets)
{
    memset(renderer_packets, 0, sizeof(Render_Packet));
}

Texture_Handle asset_load_texture_path(Asset_System* asset_system, const char* asset_path)
{
    PROFILE_ZONE(asset_load_texture_path)


    Scratch_Allocator scratch = scratch_allocator_begin(asset_system->scratch_allocator);


    //either load from metadata -> binary or binary blob
    //then send into the texture system
    //in general we just want to deserialize the data quickly,
    //the deserialization is the same, it just depends which file data we end up giving it

    String_Builder* string_builder = string_builder_create(512, scratch.allocator);
    string_builder_append_c_string(string_builder, ENGINE_TEXTURE_PATH);
    string_builder_append_c_string(string_builder, asset_path);
    string_builder_append_c_string(string_builder, ENGINE_TEXTURE_EXTENSION);


    String* load_asset_path = string_builder_to_string(string_builder, scratch.allocator);

    Asset_Record* asset_record = allocator_alloc(scratch.allocator, sizeof(Asset_Record));
    if (!asset_registry_exists_by_engine_path(asset_system->asset_registry, load_asset_path, asset_record))
    {
        MASSERT_FALSE();
        FATAL("ASSET NOT FOUND: %s", asset_path);
        return asset_system->texture_system->default_texture_handle;
    }


    Texture_Handle texture_handle = {0};

    //has asset already been loaded
    //and increase the ref count
    if (texture_system_does_texture_exist(asset_system->texture_system, asset_record, &texture_handle))
    {
        scratch_allocator_end(scratch);
        PROFILE_ZONE_END(asset_load_texture_path)
        return texture_handle;
    }


    //open file,
    //get texture data,
    //register the texture in the texture system,
    //add to the runtime state


    FILE* fptr = fopen(string_to_c_string_allocator(load_asset_path, scratch.allocator), "rb");
    if (!fptr)
    {
        MASSERT_FALSE();
        return asset_system->texture_system->default_texture_handle;
    }

    Madness_Texture_Runtime engine_texture = {0};

    asset_texture_deserialize(&engine_texture, fptr, asset_system->texture_allocator);

    texture_system_acquire(asset_system, asset_record, engine_texture.texture,
                           engine_texture.pixel_data, &texture_handle);

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

    Asset_Record* asset_record = allocator_alloc(asset_system->frame_allocator, sizeof(Asset_Record));
    if (!asset_registry_exists_by_uuid(asset_system->asset_registry, uuid, asset_record))
    {
        //TODO: try to load in the asset from the import path
        MASSERT_FALSE();
        FATAL("ASSET NOT FOUND: %d %d", uuid.high, uuid.low);
        return texture_handle;
    }

    //has asset already been loaded
    if (texture_system_does_texture_exist(asset_system->texture_system, asset_record, &texture_handle))
    {
        return texture_handle;
    }


    FILE* fptr = NULL;

    //load from individal binary
    fptr = fopen(string_to_c_string_allocator(asset_record->engine_path, asset_system->frame_allocator), "rb");
    if (!fptr)
    {
        //TODO: since were in the editor, we should at least try to find the asset in our asset folder
        MASSERT(false)
        return asset_system->texture_system->default_texture_handle;
    }

    Madness_Texture_Runtime engine_texture = {0};

    asset_texture_deserialize(&engine_texture, fptr, asset_system->texture_allocator);

    texture_system_acquire(asset_system, asset_record, engine_texture.texture,
                           engine_texture.pixel_data, &texture_handle);

    fclose(fptr);

    PROFILE_ZONE_END(asset_load_texture_uuid)


    return texture_handle;
}


bool asset_system_unload_texture(Asset_System* asset_system, Texture_Handle texture_handle)
{
    PROFILE_ZONE(asset_system_unload_texture)

    texture_system_release(asset_system, texture_handle);

    PROFILE_ZONE_END(asset_system_unload_texture)

    return false;
}


Texture_Handle asset_load_font_path(Asset_System* asset_system, const char* engine_asset_path)
{
    PROFILE_ZONE(asset_load_font_path)


    Texture_Handle out_handle = (Texture_Handle){0};
    Scratch_Allocator scratch = scratch_allocator_begin(asset_system->allocator);

    String_Builder* string_builder = string_builder_create(512, asset_system->frame_allocator);
    string_builder_append_c_string(string_builder, ENGINE_FONTS_PATH);
    string_builder_append_c_string(string_builder, engine_asset_path);
    string_builder_append_c_string(string_builder, ENGINE_FONTS_EXTENSION);

    String* load_asset_path = string_builder_to_string(string_builder, scratch.allocator);

    Asset_Record* asset_record = allocator_alloc(asset_system->frame_allocator, sizeof(Asset_Record));
    if (!asset_registry_exists_by_engine_path(asset_system->asset_registry, load_asset_path, asset_record))
    {
        //TODO: try to load in the asset from the import path
        FATAL("ASSET NOT FOUND: %s", engine_asset_path);
        scratch_allocator_end(scratch);
        return out_handle;
    }

    if (asset_record->hash == 0)
    {
        MASSERT_MSG(false, "PLZ CONVERT ASSET")
        scratch_allocator_end(scratch);
        return out_handle;
    }

    //has asset already been loaded
    //has asset already been loaded
    if (texture_system_does_texture_exist(asset_system->texture_system, asset_record, &out_handle))
    {
        scratch_allocator_end(scratch);
        return out_handle;
    }


    FILE* fptr = NULL;

    //load from individal binary

    Madness_Font_Runtime editor_texture = {0};

    fptr = fopen(string_to_c_string_allocator(load_asset_path, asset_system->frame_allocator), "rb");
    if (!fptr)
    {
        MASSERT(false);
    }

    asset_font_deserialize(&editor_texture, fptr, asset_system->texture_allocator);

    texture_system_acquire_font(asset_system, asset_record, editor_texture.texture, editor_texture.font_texture,
                                editor_texture.pixel_data,
                                &out_handle);

    fclose(fptr);

    PROFILE_ZONE_END(asset_load_font_path)


    scratch_allocator_end(scratch);
    return out_handle;
}


Texture_Handle asset_load_font_uuid(Asset_System* asset_system, MADNESS_UUID uuid)
{
    PROFILE_ZONE(asset_load_font_uuid)


    Texture_Handle out_handle = (Texture_Handle){0};
    Asset_Record* asset_record = NULL;

    if (uuid.high == 0 && uuid.low == 0)
    {
        WARN("UUID OF 0,0 passed in ")
        return out_handle;
    }

    if (!asset_registry_exists_by_uuid(asset_system->asset_registry, uuid, asset_record))
    {
        MASSERT_MSG(false, "PLZ CONVERT ASSET")
        return out_handle;
    }


    //has asset already been loaded
    if (texture_system_does_texture_exist(asset_system->texture_system, asset_record, &out_handle))
    {
        return out_handle;
    }


    FILE* fptr = fopen(string_to_c_string_allocator(asset_record->engine_path, asset_system->frame_allocator), "rb");
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
        texture_system_acquire(asset_system, asset_record, runtime.texture, runtime.pixel_data,
                               &out_handle);
    }
    else
    {
        MASSERT(false);
    }

    fclose(fptr);

    PROFILE_ZONE_END(asset_load_font_uuid)


    return out_handle;
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


    Scratch_Allocator scratch = scratch_allocator_begin(asset_system->scratch_allocator);
    String* asset_path_string = STRING_CREATE_FROM_BUFFER_ALLOCATOR(engine_asset_path, scratch.allocator);

    Asset_Record* asset_record = allocator_alloc(scratch.allocator, sizeof(Asset_Record));
    if (!asset_registry_exists_by_engine_path(asset_system->asset_registry, asset_path_string, asset_record))
    {
        MASSERT_MSG(false, "PLZ CONVERT ASSET")
        *out_handle = (Mesh_Handle){0, 0};
        return false;
    }

    //TODO: has asset already been loaded
    // mesh_is_loaded(asset_system, asset_record, out_handle);


    //steps:
    // load mesh and all dependencies like materials
    // set the data and get back the handle
    FILE* fptr = fopen(engine_asset_path, "rb");
    Madness_Mesh_Runtime runtime_mesh = {0};
    asset_mesh_deserialize(&runtime_mesh, fptr, asset_system->heap_allocator);
    fclose(fptr);


    //load in the materials and textures
    Material_Handle* material_handles = allocator_heap_alloc(asset_system->heap_allocator,
                                                             sizeof(Material_Handle) * runtime_mesh.mesh_count);
    for (u32 i = 0; i < runtime_mesh.mesh_count; i++)
    {
        asset_load_material_uuid(asset_system, runtime_mesh.material_uuid[i], &material_handles[i]);
    }


    if (!mesh_acquire(asset_system, &runtime_mesh, out_handle, material_handles))
    {
        MASSERT(false);
    }

    // Asset_Runtime* mesh_asset = &asset_system->asset_registry->mesh_asset[out_handle->handle];
    // asset_runtime_set_info_from_record(mesh_asset, out_meta_data, ASSET_STATIC_MESH);


    scratch_allocator_end(scratch);

    PROFILE_ZONE_END(asset_load_mesh_path)


    return true;
}

bool asset_load_skmesh(Asset_System* asset_system, const char* engine_asset_path, Skinned_Mesh_Handle* out_handle)
{
    PROFILE_ZONE(asset_load_skmesh)
    PROFILE_ZONE(out_handle)


    String* asset_path_string = STRING_CREATE_FROM_BUFFER_ALLOCATOR(engine_asset_path, asset_system->frame_allocator);

    Asset_Record* asset_record = allocator_alloc(asset_system->frame_allocator, sizeof(Asset_Record));;
    if (!asset_registry_exists_by_engine_path(asset_system->asset_registry, asset_path_string, asset_record))
    {
        MASSERT_MSG(false, "PLZ CONVERT ASSET")
        *out_handle = (Skinned_Mesh_Handle){0};
        return false;
    }

    //TODO: check if has asset already been loaded


    FILE* fptr = fopen(engine_asset_path, "rb");

    if (!fptr)
    {
        MASSERT(false);
        // return false;
    }

    Madness_Skinned_Mesh_Runtime runtime_mesh = {0};
    asset_skmesh_deserialize(&runtime_mesh, fptr, asset_system->heap_allocator);

    Material_Handle* material_handles = allocator_heap_alloc(asset_system->heap_allocator,
                                                             sizeof(Material_Handle) * runtime_mesh.mesh_count);
    for (u32 i = 0; i < runtime_mesh.mesh_count; i++)
    {
        asset_load_material_uuid(asset_system, runtime_mesh.material_uuid[i], &material_handles[i]);
    }


    skinned_mesh_acquire(asset_system, asset_record, &runtime_mesh, out_handle, material_handles);

    fclose(fptr);

    PROFILE_ZONE_END(asset_load_skmesh)


    return out_handle;
}

bool _asset_load_shader_asset(Asset_System* asset_system, Asset_Record* record,
                              Shader_Handle* out_handle, Scratch_Allocator* scratch_allocator)
{
    FILE* fptr = fopen(string_to_c_string_allocator(record->engine_path, scratch_allocator->allocator), "rb");

    if (!fptr)
    {
        MASSERT(false);
        return false;
    }

    shader_acquire(asset_system->shader_system, record, fptr, out_handle);


    fclose(fptr);
    return true;
}


bool asset_load_shader_asset_path(Asset_System* asset_system, const char* asset_path,
                                  Shader_Handle* out_shader_handle)
{
    MASSERT(asset_system)
    MASSERT(asset_path)
    MASSERT(out_shader_handle)

    PROFILE_ZONE(asset_load_material_asset_path)

    Scratch_Allocator scratch = scratch_allocator_begin(asset_system->scratch_allocator);

    String* asset_path_string = STRING_CREATE_FROM_BUFFER_ALLOCATOR(asset_path, scratch.allocator);

    Asset_Record* record = allocator_alloc(asset_system->frame_allocator, sizeof(Asset_Record));
    if (!asset_registry_exists_by_engine_path(asset_system->asset_registry, asset_path_string, record))
    {
        MASSERT_MSG(false, "PLZ CONVERT ASSET")
        return false;
    }


    bool load_result = _asset_load_shader_asset(asset_system, record,
                                                out_shader_handle, &scratch);


    scratch_allocator_end(scratch);
    PROFILE_ZONE_END(asset_load_material_asset_path)

    return load_result;
}

bool asset_load_shader_asset_uuid(Asset_System* asset_system, MADNESS_UUID uuid, Shader_Handle* out_handle)
{
    PROFILE_ZONE(asset_load_material_asset_uuid)

    Scratch_Allocator scratch = scratch_allocator_begin(asset_system->scratch_allocator);

    Asset_Record* out_meta_data = allocator_alloc(asset_system->frame_allocator, sizeof(Asset_Record));
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


bool _asset_load_material(Asset_System* asset_system, Asset_Record* meta_data, Material_Handle* out_material_handle)
{
    MASSERT(asset_system);
    MASSERT(meta_data);
    MASSERT(out_material_handle);


    Scratch_Allocator scratch = scratch_allocator_begin(asset_system->scratch_allocator);


    FILE* fptr = fopen(string_to_c_string_allocator(meta_data->engine_path, scratch.allocator), "rb");

    if (!fptr)
    {
        MASSERT(false);
        return false;
    }


    if (!material_acquire(asset_system, asset_system->material_system, fptr, meta_data, out_material_handle))
    {
        MASSERT(false);
        return false;
    }


    scratch_allocator_end(scratch);

    return true;
}

bool asset_load_material_uuid(Asset_System* asset_system, MADNESS_UUID madness_uuid, Material_Handle* out_material)
{
    MASSERT(asset_system)
    MASSERT(out_material)

    PROFILE_ZONE(asset_load_material_uuid)


    Scratch_Allocator scratch = scratch_allocator_begin(asset_system->scratch_allocator);


    Asset_Record* out_meta_data = allocator_alloc(scratch.allocator, sizeof(Asset_Record));;
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
    PROFILE_ZONE(asset_load_material_path)
    Scratch_Allocator scratch = scratch_allocator_begin(asset_system->scratch_allocator);

    String* asset_path_string = STRING_CREATE_FROM_BUFFER_ALLOCATOR(asset_path, scratch.allocator);

    Asset_Record* record = allocator_alloc(scratch.allocator, sizeof(Asset_Record));
    if (!asset_registry_exists_by_engine_path(asset_system->asset_registry, asset_path_string, record))
    {
        MASSERT_MSG(false, "PLZ CONVERT ASSET")
        return false;
    }


    bool load_result = _asset_load_material(asset_system, record, out_material);


    scratch_allocator_end(scratch);

    PROFILE_ZONE_END(asset_load_material_path)


    return load_result;
}


bool asset_load_particle_effect_by_path(Asset_System* asset_system, const char* asset_path,
                                        Particle_Effect_Handle* out_handle)

{
    MASSERT(asset_system);
    MASSERT(asset_path);
    MASSERT(out_handle);

    PROFILE_ZONE(asset_load_particle_effect_by_path)


    Scratch_Allocator scratch = scratch_allocator_begin(asset_system->scratch_allocator);

    String* path_string = STRING_CREATE_FROM_BUFFER_ALLOCATOR(asset_path, scratch.allocator);

    Asset_Record* record = allocator_alloc(asset_system->frame_allocator, sizeof(Asset_Record));
    if (!asset_registry_exists_by_engine_path(asset_system->asset_registry, path_string, record))
    {
        MASSERT_MSG(false, "PLZ CONVERT ASSET")
        // *out_handle = asset_system->particle_system->default_effect_handle;
        return false;
    }

    //see if asset is already loaded
    if (particle_system_does_effect_exist(asset_system->particle_system, record, out_handle))
    {
        return true;
    }


    //the asset isn't loaded, so we load it in asset
    FILE* fptr = fopen(string_to_c_string_allocator(path_string, scratch.allocator), "rb");

    if (!fptr)
    {
        MASSERT(false);
        return false;
    }


    //grab an available particle effect, with its handle
    Particle_Effect* particle_effect = particle_effect_acquire(asset_system->particle_system, record, out_handle);

    particle_effect_deserialize(particle_effect, fptr, asset_system->heap_allocator);


    fclose(fptr);

    scratch_allocator_end(scratch);

    PROFILE_ZONE_END(asset_load_particle_effect_by_path)


    return true;
}


bool asset_unload_particle_effect(Asset_System* asset_system, Particle_Effect_Handle out_handle)
{
    PROFILE_ZONE(asset_unload_particle_effect)

    MASSERT(asset_system);

    particle_effect_release(asset_system->particle_system, out_handle);


    PROFILE_ZONE_END(asset_unload_particle_effect)

    return true;
}

bool asset_load_particle_emitter(Asset_System* asset_system, const char* asset_path,
                                 Particle_Emitter_Handle* out_handle)
{
    MASSERT(asset_system);
    MASSERT(asset_path);
    MASSERT(out_handle);

    PROFILE_ZONE(asset_load_particle_emitter)


    Scratch_Allocator scratch = scratch_allocator_begin(asset_system->scratch_allocator);

    String* path_string = STRING_CREATE_FROM_BUFFER_ALLOCATOR(asset_path, scratch.allocator);

    Asset_Record* record = allocator_alloc(asset_system->frame_allocator, sizeof(Asset_Record));
    if (!asset_registry_exists_by_engine_path(asset_system->asset_registry, path_string, record))
    {
        MASSERT_MSG(false, "PLZ CONVERT ASSET")
        *out_handle = (Particle_Emitter_Handle){0, 0};
        PROFILE_ZONE_END(asset_load_particle_emitter)

        return false;
    }

    //check if the emitter has already been loaded
    if (particle_system_does_emitter_exist(asset_system->particle_system, record, out_handle))
    {
        return true;
    }

    //the asset isn't loaded, so we load it in asset
    // FILE* fptr = fopen(string_to_c_string_allocator(path_string, scratch.allocator), "rb");
    FILE* fptr = fopen(asset_path, "rb");

    if (!fptr)
    {
        MASSERT_FALSE();

        scratch_allocator_end(scratch);

        PROFILE_ZONE_END(asset_load_particle_emitter)

        return false;
    }

    //effect -> emitter[]
    //emitter[] -> asset or part of the effect


    //grab an available particle effect, with its handle
    Particle_Emitter* particle_emitter = particle_emitter_acquire(asset_system->particle_system, record, out_handle);
    if (!particle_emitter)
    {
        MASSERT_MSG_FALSE("COULD NOT FIND A PARTICLE EFFECT NOT LOADING IN PARTICLE");

        // *out_handle = asset_system->particle_system->default_emitter_handle;
        scratch_allocator_end(scratch);
        PROFILE_ZONE_END(asset_load_particle_emitter)

        return false;
    }

    particle_emitter_deserialize(particle_emitter, fptr, asset_system->heap_allocator);
    fclose(fptr);
    scratch_allocator_end(scratch);

    //we also have to load in the material
    if (!asset_load_material_uuid(asset_system, particle_emitter->material_uuid, &particle_emitter->material_handle))
    {
        MASSERT_FALSE();
    }


    PROFILE_ZONE_END(asset_load_particle_emitter)


    return true;
}

bool asset_unload_particle_emitter(Asset_System* asset_system, Particle_Emitter_Handle handle)
{
    PROFILE_ZONE(asset_unload_particle_effect)

    MASSERT(asset_system);


    particle_emitter_release(asset_system->particle_system, handle);


    PROFILE_ZONE_END(asset_unload_particle_effect)

    return true;
}

bool asset_unload_particle_emitter_by_asset_path(Asset_System* asset_system, const char* asset_path)
{
    MASSERT_FALSE();
    return true;
}
