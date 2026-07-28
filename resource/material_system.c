#include "material_system.h"
#include "asset_system.h"

bool material_system_init(Material_System* material_system, Asset_System* asset_system, Memory_System* memory_system)
{
    //reflection/registry system
    material_system->reflection_registry = reflection_registry_init(memory_system);
    generate_runtime_enums_material(material_system->reflection_registry);
    generate_runtime_structs_material(material_system->reflection_registry);

    reflection_registry_debug_print_info(material_system->reflection_registry);

    memset(material_system->material_batch, 0, 100 * sizeof(Material_Batch));

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
    render_packet_3d->material_batch_count = material_system->material_batch_count;


    return true;
}

bool material_system_exists(Asset_System* asset_system, MADNESS_UUID uuid)
{
    Material_System* material_system = asset_system->material_system;

    for (u32 i = 0; i < material_system->material_batch_count; i++)
    {
        if (madness_uuid_compare(material_system->material_batch[i].material_asset_uuid,
                                 uuid))
        {
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

    //find the material batch associated with the material data

    for (u32 i = 0; i < material_system->material_batch_count; i++)
    {
        if (madness_uuid_compare(material_system->material_batch[i].material_asset_uuid,
                                 material_instance->uuid_material_asset))
        {
            material_batch = &material_system->material_batch[i];
            break;
        }
    }

    //if we didn't find it load it in
    if (material_batch == NULL)
    {
        asset_load_material_asset_uuid(asset_system, material_instance->uuid_material_asset);
        //find it
        for (u32 i = 0; i < material_system->material_batch_count; i++)
        {
            if (madness_uuid_compare(material_system->material_batch[i].material_asset_uuid,
                                     material_instance->uuid_material_asset))
            {
                material_batch = &material_system->material_batch[i];
                break;
            }
        }
    }
    MASSERT(material_batch);


    void* material_data = allocator_alloc(asset_system->frame_allocator,
                                          material_batch->material_asset->material_gpu_definition->struct_size);

    //resolve any uuid for texture loading
    for (u32 i = 0; i < material_batch->material_asset->reflection_material_data->field_count; i++)
    {
        Reflection_Runtime_Struct_Field* field = &material_batch->material_asset->reflection_material_data->fields[i];

        if (field->type == REFLECTION_TYPE_UUID)
        {
            MASSERT(material_batch->material_asset->material_gpu_definition->types[i] == REFLECTION_TYPE_U32);

            //load in texture and set the bindless id to the proper spot in the material data

            MADNESS_UUID* uuid_data = (MADNESS_UUID*)((u8*)material_instance->material_data + field->offset);
            Texture_Handle texture_handle = {0};
            asset_load_texture_uuid(asset_system, *uuid_data, &texture_handle);


            memcpy(((u8*)material_data + material_batch->material_asset->material_gpu_definition->field_offsets[i]),
                   &texture_handle.handle,
                   sizeof(u32));
        }
        else if (material_batch->material_asset->material_gpu_definition->types[i] == field->type)
        {
            memcpy((u8*)material_data + material_batch->material_asset->material_gpu_definition->field_offsets[i],
                   (u8*)material_instance->material_data + field->offset,
                   reflection_type_get_size(material_batch->material_asset->material_gpu_definition->types[i]));
        }
        else
        {
            MASSERT(false); //this should never happen
        }
    }

    //give out the handle and add to our batch
    out_handle->material_id = material_batch->material_key;
    out_handle->buffer_handle = material_batch->material_data->num_items;
    dynamic_array_push(material_batch->material_data, material_data);

    return true;
}

bool material_system_load_material_asset(Asset_System* asset_system, MADNESS_UUID uuid, u64 uuid_hash,
                                         Material_Asset_Runtime* material_asset)
{
    //checks if a material has a batch associated with it, if not create it, otherwise do nothing and return the handle
    Material_System* material_system = asset_system->material_system;

    if (material_system_exists(asset_system, uuid))
    {
        return true;
    }


    Material_Batch* batch = &material_system->material_batch[material_system->material_batch_count++];
    batch->material_asset = material_asset->asset;
    batch->material_asset_uuid = uuid;
    batch->material_asset = material_asset->asset;
    //create the material array
    batch->material_data = _dynamic_array_create(batch->material_asset->material_gpu_definition->struct_size, 10,
                                                 asset_system->heap_allocator);
    batch->material_key = material_system_generate_id(&batch->material_asset->material_info);


    Madness_Asset* madness_asset = &material_system->material_madness_asset[material_system->
        material_madness_asset_count++];
    madness_asset->uuid = uuid;
    madness_asset->hash = uuid_hash;
    madness_asset->type = ASSET_MATERIAL;
    madness_asset->reference_count = 1;



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
        asset_load_material_asset_uuid(asset_system, mat_inst->uuid_material_asset);
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
        asset_load_material_asset_uuid(asset_system, mat_inst->uuid_material_asset);
        material_system_load_material_instance(asset_system, mat_inst, &submesh_instance->material_handle);
    }
}



bool material_system_change_material_param(Asset_System* asset_system, Material_Handle material_handle,
                                           const char* param_name, const void* new_data)
{
    Material_System* material_system = asset_system->material_system;

    Material_Batch* batch = NULL;
    for (u32 i = 0; i < material_system->material_batch_count; i++)
    {
        if (material_system->material_batch[i].material_key == material_handle.material_id)
        {
            batch = &material_system->material_batch[i];
        }
    }

    MASSERT(batch);

    const u64 hash_name = c_string_hash_u64(param_name);

    for (int i = 0; i < batch->material_asset->material_gpu_definition->field_count; ++i)
    {
        if (batch->material_asset->material_gpu_definition->name_hashes[i] == hash_name)
        {
            void* mat_data = _dynamic_array_get(batch->material_data, material_handle.buffer_handle);
            memcpy((u8*)mat_data + batch->material_asset->material_gpu_definition->field_offsets[i], new_data,
                   reflection_type_get_size(*batch->material_asset->material_gpu_definition->types));
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


Material_ID material_system_generate_id(Material_Info* material_info)
{
    return material_info->mesh_type +
        material_info->transluency +
        material_info->blend_mode +
        material_info->renderpass +
        string_hash_u32(*material_info->material_name) +
        string_hash_u32(*material_info->shader_name);
}
