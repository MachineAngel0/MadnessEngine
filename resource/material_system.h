#ifndef MATERIAL_SYSTEM_H
#define MATERIAL_SYSTEM_H

#include "resource_types.h"


bool material_system_init(Material_System* material_system, Asset_System* asset_system, Memory_System* memory_system);

bool material_system_shutdown(Material_System* material_system, Memory_System* memory_system);

bool material_system_generate_render_packet(Material_System* material_system,
                                            Render_Packet_3D* render_packet_3d);

//you have to create a material before requesting an add material type to it
// Material_Handle material_system_create_material(Material_System* material_system);

//shaders I want in the game:
// black hole, decals, screen space color gradient, multiple blend modes
// wireframe version for all shaders


void material_system_add_mesh_instance_and_material(Asset_System* asset_system, Madness_Mesh* madness_mesh,
                                                    Madness_Mesh_Instance* parent_instance);
void* material_system_resolve_and_load_material_data(Asset_System* asset_system, Material_Asset_Handle asset_handle,
                                                     Material_Instance* material_instance);

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




void material_system_load_material(Asset_System* asset_system, MADNESS_UUID uuid, u64 uuid_hash,
                                   Material_Asset_Runtime* material_asset,
                                   Material_Asset_Handle* material_asset_handle)
{
    //checks if a material has a batch associated with it, if not create it, if so we can do nothing
    Material_System* material_system = asset_system->material_system;

    if ((material_asset->asset->material_info.shader_pass & Shader_Pass_Type_Opaque) && (material_asset->asset->
        material_info.shader_pass & Shader_Pass_Type_Transparent))
    {
        MASSERT_MSG_FALSE("material_system_instantiate_material: PASSED IN OPAQUE AND TRANSPARENT INTO THE PASS TYPE");
    }

    //we are assuming everything is a graphics pipeline
    //TODO: check a hash to not duplicate material info

    material_asset_handle->handle = material_system->material_asset_count;
    material_system->material_assets[material_system->material_asset_count++] = *material_asset->asset;


    //find out the mesh type
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

    batch->material_info = material_asset->asset->material_info;
    batch->material_cpu_definition = material_asset->asset->reflection_material_data;
    batch->material_gpu_definition = material_asset->asset->material_gpu_definition;

    //create the material array and the mesh instance array
    batch->material_data = _dynamic_array_create(batch->material_gpu_definition->struct_size, 1,
                                                 asset_system->heap_allocator);
    switch (material_asset->asset->material_info.mesh_type)
    {
    case Shader_Mesh_Type_Mesh:
        batch->mesh_instances = dynamic_array_create(Madness_SubMesh_Instance, 1,
                                                     asset_system->heap_allocator);
        break;
    case Shader_Mesh_Type_Skinned:
        batch->mesh_instances = dynamic_array_create(Madness_Skinned_SubMesh_Instance, 1,
                                                     asset_system->heap_allocator);
        break;
    }

}

//NOTE: changing textures requires more elaborate steps
bool material_system_change_material_param(Asset_System* asset_system, Material_Handle material_handle,
                                           const char* param_name, const void* new_data);
void material_system_change_material_data(Asset_System* asset_system, Material_Handle material_handle, void* new_data);
void material_system_change_material_texture(Asset_System* asset_system, Material_Handle material_handle,
                                             const char* param_name, const char* texture_name);


void material_system_swap_material(Asset_System* asset_system, Material_Handle material_handle,
                                   const char* material_name);

#endif //MATERIAL_SYSTEM_H
