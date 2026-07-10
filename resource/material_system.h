#ifndef MATERIAL_SYSTEM_H
#define MATERIAL_SYSTEM_H

#include "resource_types.h"


Material_System* material_system_init(Memory_System* memory_system);

bool material_system_shutdown(Material_System* material_system, Memory_System* memory_system);

bool material_system_generate_render_packet(Material_System* material_system,
                                            Render_Packet_3D* render_packet_3d);

//you have to create a material before requesting an add material type to it
Material_Handle material_system_create_material(Material_System* material_system);


Material_Default* material_system_create_default_pbr(Material_System* material_system, Material_Handle* material_handle);
Material_Default* material_system_pbr_get(Material_System* material_system, Material_Handle material_handle);

void material_system_pbr_init(Material_Default* out_data);

// void* material_create(mat_sys, mat_name)
// void* generate material(mat_sys, mat_name)

//lets make some assumptions, rn lets get this working for meshes only everything is a mesh

//shaders i want in the game:
// black hole, decals, screen space color gradient, multiple blend modes
// wireframe version for all shaders

void material_system_add_mesh_instance_to_default_material_batch(Resource_System* resource_system,
                                                                 Mesh_Parent_Instance* parent_instance)
{
    Material_System* material_system = resource_system->material_system;

    // Mesh_Asset* mesh_asset = &mesh_system->mesh_asset_data[parent_instance->mesh_asset.handle];

    for (u32 batch_idx = 0; batch_idx < material_system->oqaque_batch_count; batch_idx++)
    {
        Material_Batch* batch = &material_system->oqaque_batch[batch_idx];

        if (strcmp(batch->shader_name, "mesh") != 0) { continue; }
        if (strcmp(batch->material_struct->name, "Material_Default") != 0) { continue; }

        batch->material_struct;
        batch->material_data;
        for (u32 mesh_inst = 0; mesh_inst < parent_instance->mesh_count; ++mesh_inst)
        {
            Mesh_Instance* mesh_instance = &parent_instance->mesh_instances_array[mesh_inst];
            dynamic_array_push(batch->mesh_instances, mesh_instance);
            dynamic_array_push(batch->material_data,
                               &material_system->prb[mesh_instance->material_handle.handle]);
        }
        return;
    }
}

void material_system_instantiate_material(Material_System* material_system, const char* shader_name,
                                          const char* material_name, Shader_Mesh_Type mesh_type)
{
    //we are assuming everything is a graphics pipeline
    //TODO: keep a hash of already loaded file paths/material combinations

    Material_Batch* batch = &material_system->oqaque_batch[material_system->oqaque_batch_count++];
    batch->shader_name = shader_name;
    batch->mesh_type = mesh_type;

    for (u32 i = 0; i < material_system->reflection_registry->struct_list->num_items; i++)
    {
        Reflection_Runtime_Struct* struct_info = (Reflection_Runtime_Struct*)_dynamic_array_get(
            material_system->reflection_registry->struct_list, i);

        if (strcmp(struct_info->name, material_name) == 0)
        {
            batch->material_struct = struct_info;
            batch->material_data = _dynamic_array_create(struct_info->struct_size, 1, material_system->heap_allocator);
            switch (mesh_type)
            {
            case Shader_Mesh_Type_Mesh:
                batch->mesh_instances = dynamic_array_create(Mesh_Instance, 1, material_system->heap_allocator);
                break;
            case Shader_Mesh_Type_Skinned:
                batch->mesh_instances = dynamic_array_create(Sk_Mesh_Instance, 1, material_system->heap_allocator);
                break;
            }
            break;
        }
    }




}

void material_system_change_material_param(Material_System* material_system, const char* material_name,
                                           const char* member_name, void* data)
{
}


#endif //MATERIAL_SYSTEM_H
