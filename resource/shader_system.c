#include "shader_system.h"

#include "asset_converter.h"
#include "asset_system.h"
#include "dsa_utility.h"
#include "resource_types.h"


bool shader_system_init(Shader_System* shader_system, Asset_System* asset_system, Memory_System* memory_system)
{
    shader_system->shader_to_material_mapping.count = 0;
    shader_system->material_reflection_registry = asset_system->material_reflection_registry;

    shader_system->shader_mapping_allocator = memory_system_allocator_create(
        memory_system, KB(256), MEMORY_SUBSYSTEM_SHADER, "shader mapping");

    shader_system->shader_asset_allocator = memory_system_allocator_create(
        memory_system, KB(256), MEMORY_SUBSYSTEM_SHADER, "shader asset");

    shader_mapping_add(shader_system,
                       "mesh",
                       TYPE_STRING(Material_Default));
    shader_mapping_add(shader_system,
                       "skinned_mesh",
                       TYPE_STRING(Material_Default));
    shader_mapping_add(shader_system,
                       "billboard_spherical",
                       TYPE_STRING(Material_Spherical_Billboard_CPU));


    //load all shaders up front
    Asset_List_Scan* list_scan =
        asset_lists_generate(memory_system, MAX_ASSETS_STRINGS, ENGINE_SHADER_PATH_NO_SLASH);

    Shader_Handle handle;
    for (u32 i = 0; i < list_scan->count; i++)
    {
        Scratch_Allocator scratch = scratch_allocator_begin(asset_system->frame_allocator);

        asset_load_shader_asset_path(asset_system,
                                     string_to_c_string_allocator(&list_scan->strings[i], scratch.allocator), &handle);

        scratch_allocator_end(scratch);
    }

    return shader_system;
}

bool shader_system_deinit(Shader_System* shader_system)
{
    //TODO:
    return true;
}

void shader_system_update(Shader_System* shader_system)
{
}

void shader_system_render_packet(Shader_System* shader_system, Render_Packet* render_packet)
{
    render_packet->shaders_packet.shader_assets = shader_system->shader_asset;
    render_packet->shaders_packet.shader_asset_count = shader_system->shader_asset_count;
}

void shader_create_material_definition_struct(Reflection_Registry* reflection_registry,
                                              Material_Definition* material_definition,
                                              const char* material_name, Allocator* allocator)
{
    //get the data from the reflection system and hash it
    material_definition->reflection_material_data = reflection_registry_get_struct(
        reflection_registry, material_name);

    material_definition->runtime_reflection_hash = reflection_registry_struct_hash_u32(
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
        //hash name for faster lookups
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


void shader_mapping_add(Shader_System* shader_system, const char* shader_name, const char* material_name)
{
    Shader_Material_Mapping* shader_mat_mapping = &shader_system->shader_to_material_mapping;


    //TODO: check if the shader actually exists
    //TODO: check if the shader name and material mapping is already in the list,
    // or if there is some sort of a conflict

    shader_mat_mapping->shader_name[shader_mat_mapping->count] = STRING_CREATE_FROM_BUFFER_ALLOCATOR(
        shader_name, shader_system->shader_mapping_allocator);
    shader_mat_mapping->material_name[shader_mat_mapping->count] = STRING_CREATE_FROM_BUFFER_ALLOCATOR(
        material_name, shader_system->shader_mapping_allocator);

    Material_Definition* definition = &shader_mat_mapping->material_definition[shader_mat_mapping->count];
    shader_create_material_definition_struct(shader_system->material_reflection_registry,
                                             definition,
                                             material_name,
                                             shader_system->shader_mapping_allocator);


    shader_mat_mapping->count++;
}


bool shader_mapping_get_definition(Shader_System* shader_system, const char* shader_name,
                                   Material_Definition* out_defintion)
{
    size_t size = strlen(shader_name);
    for (u32 i = 0; i < shader_system->shader_to_material_mapping.count; i++)
    {
        if (string_compare_c_string_length(shader_system->shader_to_material_mapping.shader_name[i], shader_name, size))
        {
            *out_defintion = shader_system->shader_to_material_mapping.material_definition[i];
            return true;
        }
    }

    MASSERT_FALSE();
    return false;
}

bool shader_mapping_get_definition_from_shader_asset(Shader_System* shader_system, Shader_Asset* shader_asset,
                                                     Material_Definition* out_defintion)
{
    Scratch_Allocator scratch_allocator = scratch_allocator_begin(shader_system->shader_asset_allocator);
    shader_mapping_get_definition(shader_system,
                                  string_to_c_string_allocator(shader_asset->shader_pipeline_state.shader_name,
                                                               scratch_allocator.allocator),
                                  out_defintion);
    scratch_allocator_end(scratch_allocator);
}


bool shader_mapping_get_definition_from_handle(Shader_System* shader_system, Shader_Handle handle,
                                               Material_Definition* out_defintion)
{
    Shader_Asset* shader_asset = shader_asset_get(shader_system, handle);
    shader_mapping_get_definition(shader_system,
                                  string_to_c_string_allocator(shader_asset->shader_pipeline_state.shader_name,
                                                               shader_system->shader_asset_allocator),
                                  out_defintion);

    return true;
}

bool shader_is_loaded(Shader_System* shader_system, Asset_Record* asset_record, Shader_Handle* out_handle)
{
    for (u32 i = 0; i < shader_system->runtime_asset_count; i++)
    {
        if (shader_system->runtime_asset[i].hash == asset_record->hash)
        {
            *out_handle = shader_system->runtime_asset[i].asset_handles.shader_handle;
            shader_system->runtime_asset[i].reference_count++;
        }
        return true;
    }

    return false;
}


bool shader_asset_serialize(Shader_Asset* shader_asset, FILE* fptr)
{
    //information about the material structure, think of it like the definition of a material/shader
    fwrite(&shader_asset->version, sizeof(shader_asset->version), 1, fptr);

    madness_uuid_serialize(shader_asset->uuid, fptr);

    string_serialize(shader_asset->shader_pipeline_state.shader_name, fptr);
    fwrite(&shader_asset->shader_pipeline_state.blend_mode, sizeof(shader_asset->shader_pipeline_state.blend_mode), 1,
           fptr);
    fwrite(&shader_asset->shader_pipeline_state.two_sided, sizeof(shader_asset->shader_pipeline_state.two_sided), 1,
           fptr);

    fwrite(&shader_asset->shader_pipeline_hash, sizeof(shader_asset->shader_pipeline_hash), 1, fptr);


    return true;
}

bool shader_asset_deserialize(Shader_Asset* shader_asset, FILE* fptr,
                              Allocator* allocator)
{
    fread(&shader_asset->version, sizeof(shader_asset->version), 1, fptr);

    madness_uuid_deserialize(&shader_asset->uuid, fptr);

    shader_asset->shader_pipeline_state.shader_name = allocator_alloc(allocator, sizeof(String));

    string_deserialize(shader_asset->shader_pipeline_state.shader_name, fptr, allocator);
    fread(&shader_asset->shader_pipeline_state.blend_mode, sizeof(shader_asset->shader_pipeline_state.blend_mode), 1,
          fptr);
    fread(&shader_asset->shader_pipeline_state.two_sided, sizeof(shader_asset->shader_pipeline_state.two_sided), 1,
          fptr);

    fread(&shader_asset->shader_pipeline_hash, sizeof(shader_asset->shader_pipeline_hash), 1, fptr);


    return true;
}


void shader_acquire(Shader_System* shader_system, Asset_Record* record, FILE* file, Shader_Handle* out_handle)
{
    //check for a mapping
    //resolve any out of date reflection hashes, i guess it only matters for the material

    Shader_Asset* shader_asset = &shader_system->shader_asset[shader_system->shader_asset_count];
    shader_asset_deserialize(shader_asset, file, shader_system->shader_mapping_allocator);


    *out_handle = (Shader_Handle){shader_system->shader_asset_count};

    //TODO: make sure only one unique shader is loaded at a time
    Asset_Runtime* runtime = &shader_system->runtime_asset[shader_system->runtime_asset_count++];
    asset_runtime_set_info_from_record(runtime, record, ASSET_TEXTURE);
    runtime->asset_handles.shader_handle = (Shader_Handle){shader_system->shader_asset_count};


    shader_system->shader_asset_count++;
}

void shader_release(Shader_System* shader_system, Shader_Handle handle)
{
    //TODO:
    Asset_Runtime* runtime = &shader_system->runtime_asset[handle.handle];
    // if (runtime->generation != handle.generation)
    // {
    // MASSERT_MSG_FALSE("texture_system_release: INVALID TEXTURE GENERATION SHOULD NEVER IDEALLY HAPPEN");
    // return false;
    // }


    // if (runtime_asset_release(runtime))
    // {
    // ring_enqueue(texture_system->available_texture_queue, &handle.handle);
    // }
}


Shader_Pipeline_Hash shader_pipeline_hash_create(Shader_PSO* shader_pso)
{
    u64 hash = hash_64_continous_start();
    hash = hash_64_continous(hash, (u8*)shader_pso->shader_name->chars, shader_pso->shader_name->length);
    hash = hash_64_continous(hash, (u8*)&shader_pso->blend_mode, sizeof(shader_pso->blend_mode));
    hash = hash_64_continous(hash, (u8*)&shader_pso->two_sided, sizeof(shader_pso->two_sided));

    return hash;
}


Shader_Asset* shader_asset_get(Shader_System* shader_system,
                               Shader_Handle shader_handle)
{
    return &shader_system->shader_asset[shader_handle.handle];
}


void shader_create_or_get(Asset_System* asset_system, Shader_PSO* shader_pso,
                          Shader_Handle* out_handle)
{
    MASSERT(asset_system);
    MASSERT(shader_pso);
    MASSERT(shader_pso->shader_name);
    MASSERT(out_handle);

    Shader_System* shader_system = asset_system->shader_system;

    //hash the shader pso  and check if it exists yet
    Shader_Pipeline_Hash pipeline_hash = shader_pipeline_hash_create(shader_pso);


    for (u32 shader_idx = 0; shader_idx < shader_system->shader_asset_count; shader_idx++)
    {
        if (shader_system->shader_asset[shader_idx].shader_pipeline_hash == pipeline_hash)
        {
            *out_handle = (Shader_Handle){
                .handle = shader_idx,
            };
            return;
        }
    }

    //create a new shader asset, if it doesn't exist, and serialize it out

    //find a free shader asset slot
    Shader_Asset* shader_asset = &shader_system->shader_asset[shader_system->shader_asset_count];
    *shader_asset = (Shader_Asset){
        .version = SHADER_VERSION,
        .uuid = madness_uuid_generate_return(),
        .shader_pipeline_state = *shader_pso,
        .shader_pipeline_hash = pipeline_hash,
    };

    asset_converter_shader_asset(asset_system, shader_asset);

    *out_handle = (Shader_Handle){
        .handle = shader_system->shader_asset_count,
    };

    shader_system->shader_asset_count++;
}


String_Builder* shader_asset_construct_path(Shader_Asset* asset, Allocator* allocator)
{
    String_Builder* str_builder = string_builder_create(256, allocator);
    string_builder_append_c_string(str_builder, ENGINE_SHADER_PATH);
    string_builder_append_string(str_builder, asset->shader_pipeline_state.shader_name);
    string_builder_append_c_string(str_builder, "_");
    string_builder_append_u64(str_builder, asset->shader_pipeline_hash, allocator);
    string_builder_append_c_string(str_builder, ENGINE_SHADER_EXTENSION);

    return str_builder;
}
