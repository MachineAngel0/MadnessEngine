#include "material_system.h"
#include "asset_system.h"

bool material_system_init(Material_System* material_system, Asset_System* asset_system, Memory_System* memory_system)
{
    memset(material_system->shader_asset, 0, MAX_MATERIAL_COUNT * sizeof(Shader_Asset));
    material_system->shader_count = 0;


    memset(material_system->material_record, 0, MAX_MATERIAL_COUNT * sizeof(Material_Record));
    memset(material_system->materials, 0, MAX_MATERIAL_COUNT * sizeof(Material));
    memset(material_system->generation, 0, MAX_MATERIAL_COUNT * sizeof(u32));

    material_system->material_record_count = 0;


    material_system->material_buffer = memory_system_alloc(memory_system, MATERIAL_BUFFER_SIZE,
                                                           MEMORY_SUBSYSTEM_MATERIAL);
    material_system->material_buffer_offset = 0;
    material_system->material_buffer_size = MATERIAL_BUFFER_SIZE;


    material_system_add_shader_material_mapping(asset_system, material_system,
                                                "mesh", TYPE_STRING(Material_Default));
    material_system_add_shader_material_mapping(asset_system, material_system,
                                                "skinned_mesh", TYPE_STRING(Material_Default));
    material_system_add_shader_material_mapping(asset_system, material_system,
                                                "billboard_spherical", TYPE_STRING(Material_Spherical_Billboard_CPU));


    //load all shaders up front
    // if (app_is_debug_build())
    // {
    Asset_List_Scan* list_scan =
        asset_lists_generate(memory_system, MAX_ASSETS_STRINGS, ENGINE_SHADER_PATH_NO_SLASH);

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
    render_packet_3d->shader_assets = material_system->shader_asset;
    render_packet_3d->material_count = material_system->shader_count;


    return true;
}

void material_system_add_shader_material_mapping(Asset_System* asset_system, Material_System* material_system,
                                                 const char* shader_name, const char* material_name)
{
    //TODO: CHECK IF VALID PATH AND VALID MATERIAL NAME, also that we are not adding duplicates

    //check if shader actual exists
    // shader_name;

    //check if the material exists
    Reflection_Runtime_Struct material_reflection_struct =
        reflection_registry_get_struct(asset_system->material_reflection_registry, material_name);

    MASSERT(material_reflection_struct.field_count > 0);

    material_system->shader_to_material_mapping.shader_name[material_system->shader_to_material_count] =
        STRING_CREATE_FROM_BUFFER_ALLOCATOR(shader_name, asset_system->allocator);

    material_system->shader_to_material_mapping.material_name[material_system->shader_to_material_count] =
        STRING_CREATE_FROM_BUFFER_ALLOCATOR(material_name, asset_system->allocator);

    //create the material definition
    Material_Definition* material_definition = &material_system->shader_to_material_mapping.material_definition[
        material_system->shader_to_material_count];
    material_definition_create(asset_system, material_definition, material_name, asset_system->allocator);


    material_system->shader_to_material_count++;
}

bool shader_material_mapping_get_material_name_and_defintion(Asset_System* asset_system,
                                                             String* shader_name,
                                                             String* out_string,
                                                             Material_Definition* out_definition,
                                                             Allocator* allocator)
{
    bool found = false;
    for (u32 i = 0; i < asset_system->material_system->shader_to_material_count; i++)
    {
        if (string_compare(asset_system->material_system->shader_to_material_mapping.shader_name[i],
                           shader_name))
        {
            out_string = string_create_allocator(shader_name->chars, shader_name->length, allocator);
            *out_definition = asset_system->material_system->shader_to_material_mapping.material_definition[i];
            found = true;
            break;
        }
    }

    return found;
}

bool shader_material_mapping_get_index(Asset_System* asset_system, String* shader_name, u32* index)
{
    for (u32 i = 0; i < asset_system->material_system->shader_to_material_count; i++)
    {
        if (string_compare(asset_system->material_system->shader_to_material_mapping.shader_name[i],
                           shader_name))
        {
            *index = i;
            return true;
        }
    }

    return false;
}

bool shader_material_mapping_get_material_defintion(Asset_System* asset_system, String* shader_name,
                                                    Material_Definition* out_definition)
{
    bool found = false;
    for (u32 i = 0; i < asset_system->material_system->shader_to_material_count; i++)
    {
        if (string_compare(asset_system->material_system->shader_to_material_mapping.shader_name[i],
                           shader_name))
        {
            *out_definition = asset_system->material_system->shader_to_material_mapping.material_definition[i];
            found = true;
            break;
        }
    }

    return found;
}


String_Builder* shader_asset_construct_path(Material_System* material_system, Shader_Asset* asset, Allocator* allocator)
{
    String_Builder* str_builder = string_builder_create(256, allocator);
    string_builder_append_c_string(str_builder, ENGINE_SHADER_PATH);
    string_builder_append_string(str_builder, asset->shader_info.shader_name);
    string_builder_append_c_string(str_builder, "_");
    string_builder_append_u64(str_builder, asset->shader_key, allocator);
    string_builder_append_c_string(str_builder, ENGINE_SHADER_EXTENSION);

    return str_builder;
}


Shader_Asset* shader_asset_get(Material_System* material_system, Shader_Handle out_shader_handle)
{
    return &material_system->shader_asset[out_shader_handle.handle];
}


void material_definition_create(Asset_System* asset_system,
                                Material_Definition* material_definition,
                                const char* material_name, Allocator* allocator)
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
    material_gpu_definition->name_hashes = allocator_alloc(allocator,
                                                           sizeof(u64) *
                                                           reflection_material->field_count);
    material_gpu_definition->field_offsets = allocator_alloc(allocator,
                                                             sizeof(u32) *
                                                             reflection_material->field_count);
    material_gpu_definition->types = allocator_alloc(allocator,
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


bool shader_asset_acquire_load_data(Asset_System* asset_system, Shader_Asset* loaded_asset, Shader_Handle* out_handle)
{
    //NOTE: we assume that at this point the asset is loaded

    Material_System* material_system = asset_system->material_system;


    //we already have our reflection data, we load it, we compare and do any updates, create the gpu definition,
    //serialize it back out, then load it into the material system

    u32 shader_index = material_system->shader_count++;
    *out_handle = (Shader_Handle){.handle = shader_index};

    //take a copy of the loaded asset
    Shader_Asset* shader_asset = &material_system->shader_asset[shader_index];
    shader_asset->shader_info.shader_name = string_duplicate_heap(loaded_asset->shader_info.shader_name,
                                                                  asset_system->heap_allocator);
    shader_asset->shader_info.two_sided = loaded_asset->shader_info.two_sided;
    shader_asset->shader_info.blend_mode = loaded_asset->shader_info.blend_mode;
    shader_asset->reflection_hash = loaded_asset->reflection_hash;
    shader_asset->shader_key = loaded_asset->shader_key;
    shader_asset->uuid = loaded_asset->uuid;
    shader_asset->version = loaded_asset->version;

    Material_Definition out_definition = {0};
    shader_material_mapping_get_material_defintion(asset_system,
                                                   loaded_asset->shader_info.shader_name,
                                                   &out_definition);

    //compare hashes, if its different we simply just overwrite the hash and the mat inst will check on its load
    if (shader_asset->reflection_hash != shader_asset->reflection_hash)
    {
        shader_asset->reflection_hash = out_definition.reflection_hash;
        asset_converter_shader_asset(asset_system, shader_asset);
    }

    return true;
}


Shader_Key shader_generate_key(Shader_Info* shader_info)
{
    u64 hash = hash_64_continous_start();
    hash = hash_64_continous(hash, (u8*)&shader_info->blend_mode, sizeof(shader_info->blend_mode));
    hash = hash_64_continous(hash, (u8*)&shader_info->two_sided, sizeof(shader_info->two_sided));
    // hash = hash_64_continous(hash, (u8*)shader_info->material_name->chars, shader_info->material_name->length);
    hash = hash_64_continous(hash, (u8*)shader_info->shader_name->chars, shader_info->shader_name->length);

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

void shader_get_or_create(Asset_System* asset_system, Shader_Info* shader_info, Shader_Handle* out_handle)
{
    MASSERT(asset_system);
    MASSERT(shader_info);
    MASSERT(shader_info->shader_name);


    Material_System* material_system = asset_system->material_system;
    Shader_Key key = shader_generate_key(shader_info);

    //optimize: hash map
    for (u32 shader_idx = 0; shader_idx < material_system->shader_count; shader_idx++)
    {
        if (material_system->shader_asset->shader_key == key)
        {
            *out_handle = (Shader_Handle){
                .handle = shader_idx,
            };
            return;
        }
    }


    //create a new shader
    u32 shader_index = material_system->shader_count++;
    *out_handle = (Shader_Handle){
        .handle = shader_index,
    };
    Shader_Asset* shader_asset = &material_system->shader_asset[shader_index];

    // get shader mapping, for the material name and the material definition
    // if we are loading from a file, we also then need to check if our hash is out of date
    u32 index = 0;
    shader_material_mapping_get_index(asset_system, shader_info->shader_name, &index);
    material_system->shader_asset_to_mapping[shader_index] = index;


    Material_Definition* mat_def = &material_system->shader_to_material_mapping.material_definition[index];
    // material_system->shader_to_material_mapping.material_name[index];


    *shader_asset = (Shader_Asset){
        .version = 1.0f,
        .reflection_hash = mat_def->reflection_hash,
        .shader_info = *shader_info,
        .uuid = madness_uuid_generate_return(),
        .shader_key = key,
    };
    asset_converter_shader_asset(asset_system, shader_asset);

    //TODO:
    // ring_enqueue(asset_system->material_system->new_shaders, );

}


/**
 * @note: we have the data for the material but we want everything else filled out, does not own the data
 */
void material_create_from_data(Asset_System* asset_system,
                               Shader_Handle* shader_handle,
                               Material* out_material,
                               const char* material_asset_name,
                               void* data)
{
    MASSERT(asset_system);
    MASSERT(out_material);
    MASSERT(material_asset_name);


    Shader_Asset* shader_asset = &asset_system->material_system->shader_asset[shader_handle->handle];



    u32 def_index = asset_system->material_system->shader_asset_to_mapping[shader_handle->handle];
    Material_Definition* material_definition = &asset_system->material_system->shader_to_material_mapping.material_definition[def_index];
    String* mat_name = asset_system->material_system->shader_to_material_mapping.material_name[def_index];


    out_material->cpu_data.material_data = data;
    out_material->cpu_data.data_size = material_definition->reflection_material_data.struct_size;
    out_material->meta_data.shader_uuid = shader_asset->uuid;
    out_material->meta_data.material_uuid = madness_uuid_generate_return();
    out_material->meta_data.name = STRING_CREATE_FROM_BUFFER_HEAP_ALLOCATOR(
        material_asset_name, asset_system->heap_allocator);

    out_material->meta_data.material_name = string_duplicate_heap(mat_name,
                                                                  asset_system->heap_allocator);

    material_instance_set_default_textures(asset_system,
                                           out_material,
                                           material_definition);

    asset_converter_material(asset_system, out_material);
}

