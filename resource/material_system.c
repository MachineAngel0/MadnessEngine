#include "material_system.h"
#include "asset_system.h"

bool material_system_init(Material_System* material_system, Asset_System* asset_system, Memory_System* memory_system)
{
    //reflection/registry system
    material_system->reflection_system = reflection_system_init(memory_system);
    reflection_system_parse(material_system->reflection_system, "../resource/material_types.h",
                            REFLECTION_PARSE_CONSTANT);
    reflection_system_parse(material_system->reflection_system, "../resource/material_types.h", REFLECTION_PARSE_ENUM);
    reflection_system_parse(material_system->reflection_system, "../resource/material_types.h",
                            REFLECTION_PARSE_STRUCT);

    reflection_data_to_files(material_system->reflection_system, "material",
                             "../resource/generated/mat_enums.h",
                             "../resource/generated/mat_structs.h");

    material_system->reflection_registry = reflection_registry_init(memory_system);
    generate_runtime_enums_material(material_system->reflection_registry);
    generate_runtime_structs_material(material_system->reflection_registry);

    reflection_registry_debug_print_info(material_system->reflection_registry);


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
    render_packet_3d->mesh_batch = material_system->mesh_batch;
    render_packet_3d->mesh_batch_count = material_system->mesh_batch_count;

    render_packet_3d->skinned_batch = material_system->skinned_batch;
    render_packet_3d->skinned_batch_count = material_system->skinned_batch_count;


    return true;
}

void* material_system_resolve_and_load_material_data(Asset_System* asset_system, Material_Asset_Handle asset_handle,
                                                     Material_Instance* material_instance)
{
    //convert from our serialized material format to our runtime format

    // TODO: properly find the asset based on uuid
    Material_Asset* material_asset = &asset_system->material_system->material_assets[asset_handle.handle];

    void* material_data = allocator_alloc(asset_system->frame_allocator,
                                          material_asset->material_gpu_definition->struct_size);


    for (u32 i = 0; i < material_asset->reflection_material_data->field_count; i++)
    {
        Reflection_Runtime_Struct_Field* field = &material_asset->reflection_material_data->fields[i];

        if (field->type == REFLECTION_TYPE_UUID)
        {
            MASSERT(material_asset->material_gpu_definition->types[i] == REFLECTION_TYPE_U32);

            //load in texture and set the bindless id to the proper spot in the material data

            MADNESS_UUID* uuid_data = (MADNESS_UUID*)((u8*)material_instance->material_data + field->offset);
            Texture_Handle texture_handle = {0};
            asset_load_texture_uuid(asset_system, *uuid_data, &texture_handle);


            memcpy(((u8*)material_data + material_asset->material_gpu_definition->field_offsets[i]), &texture_handle.handle,
                   sizeof(u32));
        }
        else if (material_asset->material_gpu_definition->types[i] == field->type)
        {
            //copy from the location of the
            memcpy((u8*)material_data + material_asset->material_gpu_definition->field_offsets[i],
                   (u8*)material_instance->material_data + field->offset,
                   reflection_type_get_size(field->type));
        }
        else
        {
            MASSERT(false); //this should never happen
        }
    }

    return material_data;
}


void material_system_add_mesh_instance_and_material(Asset_System* asset_system, Madness_Mesh* madness_mesh,
                                                    Madness_Mesh_Instance* parent_instance)
{
    //were assuming the submeshes already have their material handles

    Material_System* material_system = asset_system->material_system;

    for (u32 mesh_idx = 0; mesh_idx < madness_mesh->mesh_count; ++mesh_idx)
    {
        Madness_SubMesh_Instance* submesh_instance = &parent_instance->submesh_instances[mesh_idx];
        Material_Instance* mat_inst = &madness_mesh->material_instance[mesh_idx];

        Material_Asset_Handle handle;
        asset_load_material(asset_system, mat_inst->uuid_material_asset, &handle);
        //resolve texture bindless id's
        void* mat_data = material_system_resolve_and_load_material_data(asset_system, handle, mat_inst);

        //batch wants the render version of the data, not the serialized version
        Material_Batch* batch = &material_system->mesh_batch[submesh_instance->material_handle.batch_handle];

        dynamic_array_push(batch->mesh_instances, submesh_instance);
        dynamic_array_push(batch->material_data, mat_data);
    }
}


bool material_system_change_material_param(Asset_System* asset_system, Material_Handle material_handle,
                                           const char* param_name, const void* new_data)
{
    Material_System* material_system = asset_system->material_system;

    u64 hash_name = c_string_hash_u64(param_name);
    if (material_handle.batch_type == 0)
    {
        Material_Batch* batch = &material_system->mesh_batch[material_handle.batch_handle];
        batch->material_gpu_definition;
        batch->material_cpu_definition;

        for (int i = 0; i < batch->material_gpu_definition->field_count; ++i)
        {
            if (batch->material_gpu_definition->name_hashes[i] == hash_name)
            {
                void* mat_data = _dynamic_array_get(batch->material_data, material_handle.index_handle);
                memcpy((u8*)mat_data + batch->material_gpu_definition->field_offsets[i], new_data,
                       reflection_type_get_size(*batch->material_gpu_definition->types));
                return true;
            }
        }
    }
    if (material_handle.batch_type == 1)
    {
        Material_Batch* batch = &material_system->skinned_batch[material_handle.batch_handle];
    }

    MASSERT(false);
    return false;
}

void material_system_change_material_data(Asset_System* asset_system, Material_Handle material_handle, void* new_data)
{
    MASSERT(false);
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
