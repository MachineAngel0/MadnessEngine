#include "material_system.h"
#include "asset_system.h"

bool material_system_init(Material_System* material_system, Asset_System* asset_system, Memory_System* memory_system)
{
    memset(material_system->material_batch, 0, MAX_MATERIAL_COUNT * sizeof(Material_Batch));
    memset(material_system->shader_asset, 0, MAX_MATERIAL_COUNT * sizeof(Shader_Asset));
    memset(material_system->material_definition, 0, MAX_MATERIAL_COUNT * sizeof(Material_Definition));
    memset(material_system->shader_asset_generation, 0, MAX_MATERIAL_COUNT * sizeof(u32));
    memset(material_system->free_list, 0, MAX_MATERIAL_COUNT * sizeof(u32));
    material_system->material_count = 0;


    material_system->free_count = MAX_MATERIAL_COUNT;
    s64 temp_i = MAX_MATERIAL_COUNT - 1;
    while (temp_i >= 0)
    {
        material_system->free_list[temp_i] = temp_i;
        temp_i--;
    }


    material_system_add_shader_material_mapping(asset_system, material_system,
                                                "mesh", TYPE_STRING(Material_Default),
                                                Shader_Mesh_Type_Mesh);
    material_system_add_shader_material_mapping(asset_system, material_system,
                                                "skinned_mesh", TYPE_STRING(Material_Default),
                                                Shader_Mesh_Type_Skinned);
    material_system_add_shader_material_mapping(asset_system, material_system,
                                                "billboard_spherical", TYPE_STRING(Material_Spherical_Billboard_CPU),
                                                Shader_Mesh_Type_Particle);


    // if (app_is_debug_build())
    // {
    Asset_List_Scan* list_scan =
        asset_lists_generate(memory_system, MAX_ASSETS_STRINGS, ENGINE_MATERIAL_PATH_NO_SLASH);

    Shader_Handle handle;
    for (u32 i = 0; i < list_scan->count; i++)
    {
        Scratch_Allocator scratch = scratch_allocator_begin(asset_system->frame_allocator);

        asset_load_shader_asset_path(asset_system,
                                     string_to_c_string_allocator(list_scan->strings, scratch.allocator), &handle);

        scratch_allocator_end(scratch);
    }
    // }


    return material_system;
}

bool material_system_shutdown(Material_System* material_system, Memory_System* memory_system)
{
    MASSERT(material_system);

    memory_system_memory_free(memory_system, material_system, MEMORY_SUBSYSTEM_RESOURCE);

    return true;
}

bool material_system_generate_render_packet(Material_System* material_system,
                                            Render_Packet_3D* render_packet_3d)
{
    render_packet_3d->material_batch = material_system->material_batch;
    render_packet_3d->material_assets = material_system->shader_asset;
    render_packet_3d->material_definition = material_system->material_definition;
    render_packet_3d->material_count = material_system->material_count;


    return true;
}

bool material_system_add_shader_material_mapping(Asset_System* asset_system, Material_System* material_system,
                                                 const char* shader_name, const char* material_name,
                                                 Shader_Mesh_Type shader_type)
{
    //TODO: CHECK IF VALID PATH AND VALID MATERIAL NAME

    material_system->shader_to_material_mapping.shader_name[material_system->shader_to_material_count] =
        STRING_CREATE_FROM_BUFFER_ALLOCATOR(shader_name, asset_system->allocator);

    material_system->shader_to_material_mapping.material_name[material_system->shader_to_material_count] =
        STRING_CREATE_FROM_BUFFER_ALLOCATOR(material_name, asset_system->allocator);

    material_system->shader_to_material_count++;
}

bool material_system_get_shader_material_mapping(Asset_System* asset_system, const char* shader_name,
    Path_String* out_string)
{
    Scratch_Allocator scratch = scratch_allocator_begin(asset_system->allocator);

    String* shader_string = STRING_CREATE_FROM_BUFFER_ALLOCATOR(shader_name, scratch.allocator);

    bool found = false;
    for (u32 i = 0; i < asset_system->material_system->shader_to_material_count; i++)
    {
        if (string_compare(shader_string,
                           asset_system->material_system->shader_to_material_mapping.shader_name[i]))
        {
            out_string = asset_system->material_system->shader_to_material_mapping.shader_name[i];
            found = true;
            break;
        }
    }


    scratch_allocator_end(scratch);

    return found;
}

Shader_Asset* shader_asset_acquire(Material_System* material_system, Shader_Handle* out_shader_handle)
{
    MASSERT(out_shader_handle);

    //check the free index's
    if (material_system->free_count <= 0)
    {
        MASSERT(false);
        return NULL;
    }

    //get a free slot
    u32 index = material_system->free_list[material_system->free_count--];

    *out_shader_handle = (Shader_Handle){
        .handle = index,
        .generation = material_system->shader_asset_generation[index],
    };

    return &material_system->shader_asset[index];
}

bool shader_asset_release(Material_System* material_system, Shader_Handle shader_handle)
{
    material_system->free_list[material_system->free_count] = shader_handle.handle;
    material_system->shader_asset_generation[material_system->free_count]++;
    material_system->free_count++;
    return true;
}

Shader_Asset* shader_asset_get(Material_System* material_system, Shader_Handle out_shader_handle)
{
    if (material_system->shader_asset_generation[out_shader_handle.generation] != out_shader_handle.generation)
    {
        WARN("material_asset_get: OLD GENERATION, handing back 0 index")
        return &material_system->shader_asset[0];
    }

    return &material_system->shader_asset[out_shader_handle.handle];
}


bool material_system_material_exist_by_uuid(Asset_System* asset_system, MADNESS_UUID uuid)
{
    Material_System* material_system = asset_system->material_system;

    for (u32 i = 0; i < material_system->material_count; i++)
    {
        if (madness_uuid_compare(material_system->shader_asset[i].uuid, uuid))
        {
            return true;
        }
    }
    return false;
}

bool material_system_shader_exists_by_material_key(Asset_System* asset_system, Material_Key material_id,
                                                   Shader_Asset* out_asset, Shader_Handle* out_handle)
{
    Material_System* material_system = asset_system->material_system;


    for (u32 i = 0; i < material_system->material_count; i++)
    {
        if (material_system->shader_asset[i].material_info.material_key == material_id)
        {
            *out_asset = material_system->shader_asset[i];

            *out_handle = (Shader_Handle){
                .handle = i,
                .generation = material_system->shader_asset_generation[i]
            };

            return true;
        }
    }

    return false;
}


void material_definition_create(Asset_System* asset_system,
                                Material_Definition* material_definition, const char* material_name)
{
    //get the data from the reflection system and hash it
    material_definition->reflection_material_data = reflection_registry_get_struct(
        asset_system->material_reflection_registry, material_name);

    material_definition->reflection_hash = reflection_registry_struct_hash_u32(
        &material_definition->reflection_material_data);

    //create the gpu definition
    Reflection_Runtime_Struct* reflection_material = &material_definition->reflection_material_data;
    Material_GPU_Definition* material_gpu_definition = &material_definition->material_gpu_definition;
    material_gpu_definition->field_count = reflection_material->field_count;
    material_gpu_definition->name_hashes = allocator_alloc(asset_system->frame_allocator,
                                                           sizeof(u64) *
                                                           reflection_material->field_count);
    material_gpu_definition->field_offsets = allocator_alloc(asset_system->frame_allocator,
                                                             sizeof(u32) *
                                                             reflection_material->field_count);
    material_gpu_definition->types = allocator_alloc(asset_system->frame_allocator,
                                                     sizeof(Reflection_Type) *
                                                     reflection_material->field_count);
    material_gpu_definition->struct_size = 0;
    u32 offset = 0;
    for (u32 i = 0; i < reflection_material->field_count; i++)
    {
        material_gpu_definition->name_hashes[i] =
            c_string_hash_u64(reflection_material->fields[i].name);

        if (reflection_material->fields[i].type == REFLECTION_TYPE_UUID)
        {
            material_gpu_definition->types[i] = REFLECTION_TYPE_U32;
        }
        else
        {
            material_gpu_definition->types[i] = reflection_material->fields[i].type;
        }
        material_gpu_definition->field_offsets[i] = offset;
        material_gpu_definition->struct_size += reflection_type_get_size(material_gpu_definition->types[i]);
        offset += reflection_type_get_size(material_gpu_definition->types[i]);
    }
}


bool shader_asset_load_definitions(Asset_System* asset_system, Shader_Asset* shader_asset)
{
    //NOTE: we assume that at this point the asset is not loaded

    Material_System* material_system = asset_system->material_system;


    Scratch_Allocator scratch = scratch_allocator_begin(asset_system->allocator);

    //we already have our reflection data, we load it, we compare and do any updates, create the gpu definition,
    //serialize it back out, then load it into the material system

    Material_Batch* batch = &material_system->material_batch[material_system->material_count];
    Shader_Asset* cur_material_asset = &material_system->shader_asset[material_system->material_count];
    *cur_material_asset = *shader_asset;
    Material_Definition* material_definition = &material_system->material_definition[material_system->material_count];

    //load in the mat definition
    material_definition_create(asset_system,
                               material_definition,
                               string_to_c_string_allocator(shader_asset->material_info.material_name,
                                                            scratch.allocator));

    //compare hashes, if its different we simply just overwrite the hash and the mat inst will check on its load
    if (cur_material_asset->reflection_hash != material_definition->reflection_hash)
    {
        cur_material_asset->reflection_hash = material_definition->reflection_hash;
        asset_converter_shader_asset(asset_system, cur_material_asset);
    }


    material_system->material_count++;

    //create the material array
    batch->material_data = _dynamic_array_create(material_definition->material_gpu_definition.struct_size, 10,
                                                 asset_system->heap_allocator);


    scratch_allocator_end(scratch);


    return true;
}


bool material_system_change_material_param(Asset_System* asset_system, Material_Handle material_handle,
                                           const char* param_name, const void* new_data)
{
    Material_System* material_system = asset_system->material_system;

    Material_Batch* batch = NULL;
    u32 material_index = 0;
    for (u32 i = 0; i < material_system->material_count; i++)
    {
        if (material_system->shader_asset[i].material_info.material_key == material_handle.material_batch_index)
        {
            batch = &material_system->material_batch[i];
            material_index = i;
        }
    }

    MASSERT(batch);

    Shader_Asset* shader_asset = &material_system->shader_asset[material_index];
    Material_Definition* material_definition = &material_system->material_definition[material_index];

    const u64 hash_name = c_string_hash_u64(param_name);

    for (int i = 0; i < material_definition->material_gpu_definition.field_count; ++i)
    {
        if (material_definition->material_gpu_definition.name_hashes[i] == hash_name)
        {
            void* mat_data = _dynamic_array_get(batch->material_data, material_handle.material_index);
            memcpy((u8*)mat_data + material_definition->material_gpu_definition.field_offsets[i], new_data,
                   reflection_type_get_size(*material_definition->material_gpu_definition.types));
            return true;
        }
    }

    MASSERT(false);
    return false;
}

void material_system_change_material_texture(Asset_System* asset_system, Material_Handle material_handle,
                                             const char* param_name, const char* texture_name)
{
    MASSERT(false);
}

void material_system_swap_material(Asset_System* asset_system, Material_Handle material_handle,
                                   const char* material_name)
{
    MASSERT(false);
}


Material_Key material_generate_id(Material_Info* material_info)
{
    u64 hash = hash_64_continous_start();
    hash = hash_64_continous(hash, (u8*)&material_info->mesh_type, sizeof(material_info->mesh_type));
    hash = hash_64_continous(hash, (u8*)&material_info->transluency, sizeof(material_info->transluency));
    hash = hash_64_continous(hash, (u8*)&material_info->blend_mode, sizeof(material_info->blend_mode));
    hash = hash_64_continous(hash, (u8*)&material_info->renderpass, sizeof(material_info->renderpass));
    hash = hash_64_continous(hash, (u8*)material_info->material_name->chars, material_info->material_name->length);
    hash = hash_64_continous(hash, (u8*)material_info->shader_name->chars, material_info->shader_name->length);

    return hash;
}


void material_instance_set_default_textures(Asset_System* asset_system,
                                            Material* material,
                                            Material_Definition* material_definition)
{
    MASSERT(asset_system);
    MASSERT(material);
    MASSERT(material->meta_data.material_name);
    MASSERT(material->meta_data.name);
    MASSERT(material->cpu_data.material_data);
    MASSERT(material_definition->reflection_material_data.name);
    MASSERT(material_definition->material_gpu_definition.types);


    for (u32 i = 0; i < material_definition->reflection_material_data.field_count; i++)
    {
        if (material_definition->reflection_material_data.fields[i].type == REFLECTION_TYPE_UUID)
        {
            MADNESS_UUID* uuid_data = (MADNESS_UUID*)((u8*)material->cpu_data.material_data + material_definition->
                reflection_material_data.fields[i].offset);
            *uuid_data = asset_system->texture_system->default_texture_uuid;
        }
    }
}

void shader_asset_create(Asset_System* asset_system,
                         Material_Info* material_info,
                         Shader_Asset* shader_asset)
{
    MASSERT(asset_system);
    MASSERT(material_info);
    MASSERT(material_info->material_name);
    MASSERT(shader_asset);

    // we want to check if the registry to see if the shader asset has already been created,
    // if so we get the uuid, otherwise, we have to create it, we also make sure the hash is up to date


    Scratch_Allocator scratch = scratch_allocator_begin(asset_system->allocator);

    String_Builder* str_builder = string_builder_create(256, scratch.allocator);
    string_builder_append_c_string(str_builder, ENGINE_MATERIAL_PATH);
    string_builder_append_string(str_builder, material_info->material_name);
    string_builder_append_c_string(str_builder, "_");
    string_builder_append_string(str_builder, material_info->shader_name);
    string_builder_append_c_string(str_builder, "_");
    string_builder_append_u64(str_builder, material_info->material_key, scratch.allocator);
    string_builder_append_c_string(str_builder, ENGINE_MATERIAL_EXTENSION);


    Reflection_Runtime_Struct reflection_material = reflection_registry_get_struct(
        asset_system->material_reflection_registry, MATERIAL_DEFAULT_NAME);


    //this exist and we have to create it, then serialize it out
    Asset_MetaData* out_meta_data = allocator_alloc(asset_system->frame_allocator, sizeof(Asset_MetaData));
    if (asset_registry_exists_by_engine_path(asset_system->asset_registry,
                                             string_builder_to_string_allocator(str_builder, scratch.allocator),
                                             out_meta_data))
    {
        *shader_asset = (Shader_Asset){
            .version = 1.0f,
            .reflection_hash = reflection_registry_struct_hash_u32(&reflection_material),
            .material_info = *material_info,
            .uuid = out_meta_data->uuid,
        };
    }
    else
    {
        *shader_asset = (Shader_Asset){
            .version = 1.0f,
            .reflection_hash = reflection_registry_struct_hash_u32(&reflection_material),
            .material_info = *material_info,
            .uuid = madness_uuid_generate_return(),
        };
    }

    scratch_allocator_end(scratch);

    asset_converter_shader_asset(asset_system, shader_asset);
}


/**
 * @note: we have the data for the material but we want everything else filled out, does not own the data
 */
void material_create_from_data(Asset_System* asset_system,
                               Shader_Asset* shader_asset,
                               Material_Definition* material_definition,
                               Material* out_material,
                               const char* material_name,
                               void* data)
{
    MASSERT(asset_system);
    MASSERT(out_material);
    MASSERT(shader_asset);
    MASSERT(shader_asset->material_info.material_name);
    MASSERT(material_name);
    MASSERT(shader_asset->uuid.high != 0);
    MASSERT(shader_asset->uuid.low != 0);
    madness_uuid_validate(shader_asset->uuid);


    out_material->cpu_data.material_data = data;
    out_material->cpu_data.data_size = material_definition->reflection_material_data.struct_size;
    out_material->meta_data.shader_uuid = shader_asset->uuid;
    out_material->meta_data.material_uuid = madness_uuid_generate_return();
    out_material->meta_data.name = STRING_CREATE_FROM_BUFFER_HEAP_ALLOCATOR(
        material_name, asset_system->heap_allocator);

    out_material->meta_data.material_name = string_duplicate_heap(shader_asset->material_info.material_name,
                                                                  asset_system->heap_allocator);

    material_instance_set_default_textures(asset_system,
                                           out_material,
                                           material_definition);

    asset_converter_material(asset_system, out_material);
}


void material_create(Asset_System* asset_system, Shader_Asset* shader_asset,
                     Material_Definition* material_definition,
                     Material* out_material,
                     const char* material_name)
{
    void* material_data = allocator_heap_alloc(asset_system->heap_allocator,
                                               material_definition->reflection_material_data.
                                                                    struct_size);
    memset(out_material->cpu_data.material_data, 0, material_definition->reflection_material_data.struct_size);

    material_create_from_data(asset_system,
                              shader_asset,
                              material_definition,
                              out_material,
                              material_name,
                              material_data);
}
