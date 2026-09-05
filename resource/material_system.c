#include "material_system.h"
#include "asset_system.h"

bool material_system_init(Material_System* material_system, Asset_System* asset_system, Memory_System* memory_system)
{
    memset(material_system->material_batch, 0, MAX_MATERIAL_COUNT * sizeof(Material_Batch));
    memset(material_system->material_asset, 0, MAX_MATERIAL_COUNT * sizeof(Material_Asset));
    memset(material_system->material_definition, 0, MAX_MATERIAL_COUNT * sizeof(Material_Definition));
    memset(material_system->material_madness_asset, 0, MAX_MATERIAL_COUNT * sizeof(Madness_Asset));
    material_system->material_count = 0;
    material_system->material_madness_asset_count = 0;


    // if (app_is_debug_build())
    // {
    Asset_List_Scan* list_scan =
        asset_lists_generate(memory_system, MAX_ASSETS_STRINGS, ENGINE_MATERIAL_PATH_NO_SLASH);

    for (u32 i = 0; i < list_scan->count; i++)
    {
        Scratch_Allocator scratch = scratch_allocator_begin(asset_system->frame_allocator);

        asset_load_material_asset_path(asset_system,
                                       string_to_c_string_allocator(list_scan->strings, scratch.allocator), NULL);

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
    render_packet_3d->material_assets = material_system->material_asset;
    render_packet_3d->material_definition = material_system->material_definition;
    render_packet_3d->material_count = material_system->material_count;


    return true;
}

bool material_system_material_exist_by_uuid(Asset_System* asset_system, MADNESS_UUID uuid)
{
    Material_System* material_system = asset_system->material_system;

    for (u32 i = 0; i < material_system->material_count; i++)
    {
        if (madness_uuid_compare(material_system->material_asset[i].uuid, uuid))
        {
            return true;
        }
    }
    return false;
}

bool material_system_material_exists_by_material_id(Asset_System* asset_system, Material_ID material_id,
                                                    Material_Asset* out_asset)
{
    Material_System* material_system = asset_system->material_system;


    for (u32 i = 0; i < material_system->material_count; i++)
    {
        if (material_system->material_asset[i].material_info.material_key == material_id)
        {
            *out_asset = material_system->material_asset[i];

            return true;
        }
    }

    return false;
}

bool material_system_load_material_instance(Asset_System* asset_system, Material_Instance* material_instance,
                                            Material_Handle* out_handle)
{
    Material_System* material_system = asset_system->material_system;

    Material_Batch* material_batch = NULL;
    u32 material_index = 0;

    //find the material batch associated with the material data
    for (u32 i = 0; i < material_system->material_count; i++)
    {
        if (madness_uuid_compare(material_system->material_asset[i].uuid,
                                 material_instance->material_asset_uuid))
        {
            material_batch = &material_system->material_batch[i];
            material_index = i;
            break;
        }
    }

    //if we didn't find it load it in
    if (material_batch == NULL)
    {
        asset_load_material_asset_uuid(asset_system, material_instance->material_asset_uuid);
        //find it
        for (u32 i = 0; i < material_system->material_count; i++)
        {
            if (madness_uuid_compare(material_system->material_asset[i].uuid,
                                     material_instance->material_asset_uuid))
            {
                material_batch = &material_system->material_batch[i];
                material_index = i;

                break;
            }
        }
    }
    MASSERT(material_batch);

    Material_Asset* material_asset = &material_system->material_asset[material_index];
    Material_Definition* material_definition = &material_system->material_definition[material_index];


    void* material_data = allocator_alloc(asset_system->frame_allocator,
                                          material_definition->material_gpu_definition.struct_size);

    //resolve any uuid for texture loading
    for (u32 i = 0; i < material_definition->reflection_material_data.field_count; i++)
    {
        Reflection_Runtime_Struct_Field* field = &material_definition->reflection_material_data.fields[i];

        if (field->type == REFLECTION_TYPE_UUID)
        {
            MASSERT(material_definition->material_gpu_definition.types[i] == REFLECTION_TYPE_U32);

            //load in texture and use the bindless index
            MADNESS_UUID uuid_data = *(MADNESS_UUID*)((u8*)material_instance->material_data + field->offset);
            //sometimes we dont have a texture so we just set the value to zero
            if (uuid_data.high == 0 && uuid_data.low == 0)
            {
                u32 default_texture = 0;
                //copy into the gpu struct
                memcpy(((u8*)material_data + material_definition->material_gpu_definition.field_offsets[i]),
                       &default_texture,
                       sizeof(u32));
            }
            else
            {
                Texture_Handle texture_handle = asset_load_texture_uuid(asset_system, uuid_data);

                //copy into the gpu struct
                memcpy(((u8*)material_data + material_definition->material_gpu_definition.field_offsets[i]),
                       &texture_handle.handle,
                       sizeof(u32));
            }
        }
        else if (material_definition->material_gpu_definition.types[i] == field->type)
        {
            //copy into the gpu struct
            memcpy((u8*)material_data + material_definition->material_gpu_definition.field_offsets[i],
                   (u8*)material_instance->material_data + field->offset,
                   reflection_type_get_size(material_definition->material_gpu_definition.types[i]));
        }
        else
        {
            MASSERT(false); //this should never happen
        }
    }

    //give out the handle and add to our batch
    out_handle->material_id = material_asset->material_info.material_key;
    out_handle->material_index = material_batch->material_data->num_items;
    dynamic_array_push(material_batch->material_data, material_data);
    return true;
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


bool material_system_load_material_asset(Asset_System* asset_system, MADNESS_UUID uuid, u64 uuid_hash,
                                         Material_Asset* material_asset)
{
    //NOTE: we assume that at this point the asset is not loaded


    Material_System* material_system = asset_system->material_system;


    Scratch_Allocator scratch = scratch_allocator_begin(asset_system->allocator);

    //we already have our reflection data, we load it, we compare and do any updates, create the gpu definition,
    //serialize it back out, then load it into the material system

    Material_Batch* batch = &material_system->material_batch[material_system->material_count];
    Material_Asset* cur_material_asset = &material_system->material_asset[material_system->material_count];
    *cur_material_asset = *material_asset;
    Material_Definition* material_definition = &material_system->material_definition[material_system->material_count];

    //load in the mat definition
    material_definition_create(asset_system,
                               material_definition,
                               string_to_c_string_allocator(material_asset->material_info.material_name,
                                                            scratch.allocator));

    //compare hashes, if its different we simply just overwrite the hash and the mat inst will check on its load
    if (cur_material_asset->reflection_hash != material_definition->reflection_hash)
    {
        cur_material_asset->reflection_hash = material_definition->reflection_hash;
        asset_converter_material_asset(asset_system, cur_material_asset);
    }


    //create the material definition
    material_definition_create(asset_system, material_definition,
                               string_to_c_string_allocator(cur_material_asset->material_info.material_name,
                                                            scratch.allocator));


    material_system->material_count++;

    //create the material array
    batch->material_data = _dynamic_array_create(material_definition->material_gpu_definition.struct_size, 10,
                                                 asset_system->heap_allocator);


    Madness_Asset* madness_asset = &material_system->material_madness_asset[material_system->
        material_madness_asset_count++];
    // madness_asset->engine_path = uuid;
    madness_asset->path_hash = uuid_hash;
    madness_asset->type = ASSET_MATERIAL;
    madness_asset->reference_count = 1;


    scratch_allocator_end(scratch);


    return true;
}


void material_system_add_mesh_instance_and_material(Asset_System* asset_system, Madness_Mesh* madness_mesh,
                                                    Madness_Mesh_Instance* parent_instance)
{
    for (u32 mesh_idx = 0; mesh_idx < madness_mesh->mesh_count; mesh_idx++)
    {
        Madness_SubMesh_Instance* submesh_instance = &parent_instance->submesh_instances[mesh_idx];
        Material_Instance* mat_inst = &madness_mesh->material_instance[mesh_idx];

        //load in the instance data and resolve any texture ids, to then add them into the material batch
        // asset_load_material_asset_uuid(asset_system, mat_inst->uuid_material_asset);
        material_system_load_material_instance(asset_system, mat_inst, &submesh_instance->material_handle);
    }
}

void material_system_add_skinned_instance_and_material(Asset_System* asset_system, Madness_Skinned_Mesh* madness_mesh,
                                                       Madness_Skinned_Mesh_Instance* parent_instance)
{
    for (u32 mesh_idx = 0; mesh_idx < madness_mesh->mesh_count; mesh_idx++)
    {
        Madness_Skinned_Submesh_Instance* submesh_instance = &parent_instance->submesh_instances[mesh_idx];
        Material_Instance* mat_inst = &madness_mesh->material_instance[mesh_idx];

        //load in the instance data and resolve any texture ids, to then add them into the material batch
        // asset_load_material_asset_uuid(asset_system, mat_inst->uuid_material_asset);
        material_system_load_material_instance(asset_system, mat_inst, &submesh_instance->material_handle);
    }
}


bool material_system_change_material_param(Asset_System* asset_system, Material_Handle material_handle,
                                           const char* param_name, const void* new_data)
{
    Material_System* material_system = asset_system->material_system;

    Material_Batch* batch = NULL;
    u32 material_index = 0;
    for (u32 i = 0; i < material_system->material_count; i++)
    {
        if (material_system->material_asset[i].material_info.material_key == material_handle.material_id)
        {
            batch = &material_system->material_batch[i];
            material_index = i;
        }
    }

    MASSERT(batch);

    Material_Asset* material_asset = &material_system->material_asset[material_index];
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


Material_ID material_generate_id(Material_Info* material_info)
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
                                            Material_Instance* material_instance,
                                            Material_Definition* material_definition)
{
    MASSERT(asset_system);
    MASSERT(material_instance);
    MASSERT(material_instance->material_data);
    MASSERT(material_definition->reflection_material_data.name);
    MASSERT(material_definition->material_gpu_definition.types);


    for (u32 i = 0; i < material_definition->reflection_material_data.field_count; i++)
    {
        if (material_definition->reflection_material_data.fields[i].type == REFLECTION_TYPE_UUID)
        {
            MADNESS_UUID* uuid_data = *(MADNESS_UUID**)((u8*)material_instance->material_data + material_definition->
                reflection_material_data.fields[i].offset);
            *uuid_data = asset_system->texture_system->default_texture_uuid;
        }
    }
}

void material_asset_create(Asset_System* asset_system, Material_Info* material_info, Material_Asset* out_material_asset)
{
    MASSERT(asset_system);
    MASSERT(material_info);
    MASSERT(material_info->material_name);
    MASSERT(out_material_asset);

    //look for it by key
    material_info->material_key = material_generate_id(material_info);

    if (material_system_material_exists_by_material_id(asset_system, material_info->material_key, out_material_asset))
    {
        return;
    }

    //we want to check if the mat asset has already been created in a file and then just pass it back
    // the load function makes sure the definition is up to date
    Scratch_Allocator scratch = scratch_allocator_begin(asset_system->allocator);

    String_Builder* str_builder = string_builder_create(256, scratch.allocator);
    string_builder_append_c_string(str_builder, ENGINE_MATERIAL_PATH);
    string_builder_append_string(str_builder, material_info->material_name);
    string_builder_append_c_string(str_builder, "_");
    string_builder_append_string(str_builder, material_info->shader_name);
    string_builder_append_c_string(str_builder, "_");
    string_builder_append_u64(str_builder, material_info->material_key, scratch.allocator);
    string_builder_append_c_string(str_builder, ENGINE_MATERIAL_EXTENSION);

    const char* mat_asset_path = string_builder_to_c_string(str_builder);

    if (filesystem_does_file_exists(mat_asset_path))
    {
        asset_load_material_asset_path(asset_system, mat_asset_path, out_material_asset);
        scratch_allocator_end(scratch);

        return;
    }
    scratch_allocator_end(scratch);

    //this doesn't exist and we have to create it

    Reflection_Runtime_Struct reflection_material = reflection_registry_get_struct(
        asset_system->material_reflection_registry, MATERIAL_DEFAULT_NAME);

    //TODO: check to see if we already have this particular material asset already created

    *out_material_asset = (Material_Asset){
        .version = 1.0f,
        .reflection_hash = reflection_registry_struct_hash_u32(&reflection_material),
        .material_info = *material_info,
        .uuid = madness_uuid_generate_return(),
    };
}

void material_asset_destroy(Asset_System* asset_system, Material_Info* material_info, Material_Asset* material_asset)
{
}

void material_instance_create(Asset_System* asset_system, Material_Asset* material_asset,
                              Material_Definition* material_definition,
                              Material_Instance* out_material_instance, const char* mat_inst_name)
{
    MASSERT(asset_system);
    MASSERT(out_material_instance);
    MASSERT(material_asset);
    MASSERT(mat_inst_name);
    madness_uuid_validate(material_asset->uuid);


    out_material_instance->material_data = allocator_heap_alloc(asset_system->heap_allocator,
                                                                material_definition->reflection_material_data.
                                                                struct_size);
    //just for insurance
    memset(out_material_instance->material_data, 0, material_definition->reflection_material_data.struct_size);


    out_material_instance->data_size = material_definition->reflection_material_data.struct_size;
    out_material_instance->material_asset_uuid = material_asset->uuid;
    out_material_instance->material_instance_uuid = madness_uuid_generate_return();
    out_material_instance->material_name = string_duplicate_heap(material_asset->material_info.material_name,
                                                                 asset_system->heap_allocator);
    out_material_instance->name = STRING_CREATE_FROM_BUFFER_HEAP_ALLOCATOR(mat_inst_name, asset_system->heap_allocator);

    material_instance_set_default_textures(asset_system,
                                           out_material_instance,
                                           material_definition);
}

/**
 * @note: we have the data for the material but we want everything else filled out, does not own the data
 */
void material_instance_create_from_data(Asset_System* asset_system,
                                        Material_Asset* material_asset,
                                        Material_Definition* material_definition,
                                        Material_Instance* out_material_instance,
                                        const char* mat_instance_name,
                                        void* data)
{
    MASSERT(asset_system);
    MASSERT(out_material_instance);
    MASSERT(material_asset);
    MASSERT(material_asset->material_info.material_name);
    MASSERT(mat_instance_name);
    MASSERT(material_asset->uuid.high != 0);
    MASSERT(material_asset->uuid.low != 0);


    out_material_instance->material_data = data;
    out_material_instance->data_size = material_definition->reflection_material_data.struct_size;
    out_material_instance->material_asset_uuid = material_asset->uuid;
    out_material_instance->material_instance_uuid = madness_uuid_generate_return();
    out_material_instance->name = STRING_CREATE_FROM_BUFFER_HEAP_ALLOCATOR(
        mat_instance_name, asset_system->heap_allocator);

    out_material_instance->material_name = string_duplicate_heap(material_asset->material_info.material_name,
                                                                 asset_system->heap_allocator);
}
