#ifndef MATERIAL_SYSTEM_H
#define MATERIAL_SYSTEM_H

#include "asset_system.h"
#include "resource_types.h"


Material_System* material_system_init(Memory_System* memory_system);

bool material_system_shutdown(Material_System* material_system, Memory_System* memory_system);

bool material_system_generate_render_packet(Material_System* material_system,
                                            Render_Packet_3D* render_packet_3d);

//you have to create a material before requesting an add material type to it
// Material_Handle material_system_create_material(Material_System* material_system);

//shaders i want in the game:
// black hole, decals, screen space color gradient, multiple blend modes
// wireframe version for all shaders




void material_system_add_mesh_instance_and_material(Asset_System* assset_system, Madness_Mesh* madness_mesh,
                                                                 Madness_Mesh_Instance* parent_instance)
{
    Material_System* material_system = assset_system->material_system;

    // Mesh_Asset* mesh_asset = &mesh_system->mesh_asset_data[parent_instance->mesh_asset.handle];


    for (u32 mesh_inst = 0; mesh_inst < madness_mesh->mesh_count; ++mesh_inst)
    {
        Madness_SubMesh_Instance* submesh_instance = &parent_instance->submesh_instances[mesh_inst];
        Madness_SubMesh* submesh = &madness_mesh->mesh_data[mesh_inst];

        asset_load_material(assset_system, submesh->material_uuid, &submesh_instance->material_handle);


        //TODO: add to batch
        Material_Batch* batch = &material_system->mesh_batch[batch_idx];

        dynamic_array_push(batch->mesh_instances, mesh_instance);
        dynamic_array_push(batch->material_data,
                           &material_system->prb[mesh_instance->material_handle.handle]);
    }

}

/*

void material_system_add_skmesh_instance_to_default_material_batch(Asset_System* resource_system,
                                                                   Madness_SkMesh_Instance* parent_instance)
{
    Material_System* material_system = resource_system->material_system;

    // Mesh_Asset* mesh_asset = &mesh_system->mesh_asset_data[parent_instance->mesh_asset.handle];

    for (u32 batch_idx = 0; batch_idx < material_system->skinned_batch_count; batch_idx++)
    {
        Material_Batch* batch = &material_system->skinned_batch[batch_idx];

        if (string_compare_c_string(batch->material_info.shader_name, "skinned_mesh") != 0) { continue; }
        if (strcmp(batch->material_struct->name, "Material_Default") != 0) { continue; }

        for (u32 mesh_inst = 0; mesh_inst < parent_instance->mesh_count; ++mesh_inst)
        {
            Madness_Skinned_SubMesh_Instance* mesh_instance = &parent_instance->sk_mesh_instance_array[mesh_inst];
            dynamic_array_push(batch->mesh_instances, mesh_instance);
            dynamic_array_push(batch->material_data,
                               &material_system->prb[mesh_instance->material_handle.handle]);
        }
        return;
    }
}
*/


void material_system_instantiate_material(Material_System* material_system, const char* shader_name,
                                          const char* material_name, Shader_Mesh_Type mesh_type,
                                          Shader_Blend_Mode blend_mode, Shader_Pass_Type pass_type)
{
    //we are assuming everything is a graphics pipeline
    //TODO: keep a hash of already loaded file paths/material combinations
    Material_Batch* batch = NULL;
    switch (mesh_type)
    {
    case Shader_Mesh_Type_Mesh:
        batch = &material_system->mesh_batch[material_system->mesh_batch_count++];
        break;
    case Shader_Mesh_Type_Skinned:
        batch = &material_system->skinned_batch[material_system->skinned_batch_count++];
        break;
    }
    if (!batch)
    {
        MASSERT(false);
    }

    batch->material_info.shader_name = STRING_CREATE_FROM_BUFFER(shader_name); // TODO: pass in an allocator
    batch->material_info.mesh_type = mesh_type;
    batch->material_info.blend_mode = blend_mode;
    batch->material_info.shader_pass = pass_type;

    if ((pass_type & Shader_Pass_Type_Opaque) && (pass_type & Shader_Pass_Type_Transparent))
    {
        MASSERT_MSG_FALSE("material_system_instantiate_material: PASSED IN OPAQUE AND TRANSPARENT INTO THE PASS TYPE");
    }


    for (u32 i = 0; i < material_system->reflection_registry->struct_list->num_items; i++)
    {
        Reflection_Runtime_Struct* struct_info = (Reflection_Runtime_Struct*)_dynamic_array_get(
            material_system->reflection_registry->struct_list, i);

        if (strcmp(struct_info->name, material_name) == 0)
        {
            batch->material_struct = struct_info;
            batch->material_data = _dynamic_array_create(struct_info->struct_size, 1,
                                                         material_system->heap_allocator);
            switch (mesh_type)
            {
            case Shader_Mesh_Type_Mesh:
                batch->mesh_instances = dynamic_array_create(Madness_SubMesh_Instance, 1, material_system->heap_allocator);
                break;
            case Shader_Mesh_Type_Skinned:
                batch->mesh_instances = dynamic_array_create(Madness_Skinned_SubMesh_Instance, 1, material_system->heap_allocator);
                break;
            }
            break;
        }
    }
}

void material_system_change_material_param(Material_System* material_system, const char* material_name,
                                           const char* member_name, Reflection_Type intended_type, void* data)
{
}


void material_system_load_material(Asset_System* asset_system, Material_Asset_Runtime* material_asset)
{

    Material_System* material_system = asset_system->material_system;

    //TODO: check a hash to not duplicate material info

    //we are assuming everything is a graphics pipeline
    //TODO: keep a hash of already loaded file paths/material combinations
    Material_Batch* batch = NULL;
    switch (material_asset->asset->material_info.mesh_type)
    {
    case Shader_Mesh_Type_Mesh:
        batch = &material_system->mesh_batch[material_system->mesh_batch_count++];
        break;
    case Shader_Mesh_Type_Skinned:
        batch = &material_system->skinned_batch[material_system->skinned_batch_count++];
        break;
    }
    if (!batch)
    {
        MASSERT(false);
    }

    batch->material_info.shader_name = string_duplicate_heap(material_asset->asset->material_info.shader_name, material_system->heap_allocator);
    batch->material_info.material_name = string_duplicate_heap(material_asset->asset->material_info.material_name, material_system->heap_allocator);
    batch->material_info.mesh_type = material_asset->asset->material_info.mesh_type;
    batch->material_info.blend_mode = material_asset->asset->material_info.blend_mode;
    batch->material_info.shader_pass = material_asset->asset->material_info.shader_pass;

    if ((material_asset->asset->material_info.shader_pass & Shader_Pass_Type_Opaque) && (material_asset->asset->material_info.shader_pass & Shader_Pass_Type_Transparent))
    {
        MASSERT_MSG_FALSE("material_system_instantiate_material: PASSED IN OPAQUE AND TRANSPARENT INTO THE PASS TYPE");
    }


    for (u32 i = 0; i < material_system->reflection_registry->struct_list->num_items; i++)
    {
        Reflection_Runtime_Struct* struct_info = (Reflection_Runtime_Struct*)_dynamic_array_get(
            material_system->reflection_registry->struct_list, i);

        if (string_compare_c_string(material_asset->asset->material_info.material_name, struct_info->name) == 0)
        {
            batch->material_struct = struct_info;
            batch->material_data = _dynamic_array_create(struct_info->struct_size, 1,
                                                         material_system->heap_allocator);
            switch (material_asset->asset->material_info.mesh_type)
            {
            case Shader_Mesh_Type_Mesh:
                batch->mesh_instances = dynamic_array_create(Madness_SubMesh_Instance, 1, material_system->heap_allocator);
                break;
            case Shader_Mesh_Type_Skinned:
                batch->mesh_instances = dynamic_array_create(Madness_Skinned_SubMesh_Instance, 1, material_system->heap_allocator);
                break;
            }
            break;
        }
    }

}





#endif //MATERIAL_SYSTEM_H
