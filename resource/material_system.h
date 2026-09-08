#ifndef MATERIAL_SYSTEM_H
#define MATERIAL_SYSTEM_H


#include "resource_types.h"


bool material_system_init(Material_System* material_system, Asset_System* asset_system, Memory_System* memory_system);

bool material_system_shutdown(Material_System* material_system, Memory_System* memory_system);

bool material_system_generate_render_packet(Material_System* material_system,
                                            Render_Packet_3D* render_packet_3d);

bool material_system_add_shader_material_mapping(Asset_System* asset_system, Material_System* material_system,
                                                 const char* shader_name, const char* material_name, Shader_Mesh_Type shader_type);

Material_Asset* material_asset_acquire(Material_System* material_system,
                                       Shader_Handle* out_shader_handle)
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
        .generation = material_system->material_asset_generation[index],
    };

    return &material_system->material_asset[index];
}

bool material_asset_release(Material_System* material_system, Shader_Handle shader_handle)
{
    material_system->free_list[material_system->free_count] = shader_handle.handle;
    material_system->material_asset_generation[material_system->free_count]++;
    material_system->free_count++;
    return true;
}

Material_Asset* material_asset_get(Material_System* material_system,
                                   Shader_Handle out_shader_handle)
{
    if (material_system->material_asset_generation[out_shader_handle.generation] != out_shader_handle.generation)
    {
        WARN("material_asset_get: OLD GENERATION, handing back 0 index")
        return &material_system->material_asset[0];
    }

    return &material_system->material_asset[out_shader_handle.handle];
}


//you have to create a material before requesting an add material type to it
// Material_Handle material_system_create_material(Material_System* material_system);

//shaders I want in the game:
// black hole, decals, screen space color gradient, multiple blend modes
// wireframe version for all shaders


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

bool material_system_material_exist_by_uuid(Asset_System* asset_system, MADNESS_UUID uuid);

bool material_system_material_exists_by_material_id(Asset_System* asset_system,
                                                    Material_Key material_id,
                                                    Material_Asset* out_asset);


bool material_system_load_material_instance(Asset_System* asset_system, Material_Instance* material_instance,
                                            Material_Handle* out_handle);


bool material_system_load_material_asset_definition(Asset_System* asset_system, MADNESS_UUID uuid, u64 uuid_hash,
                                                    Material_Asset* material_asset);

//NOTE: changing textures requires more elaborate steps
bool material_system_change_material_param(Asset_System* asset_system, Material_Handle material_handle,
                                           const char* param_name, const void* new_data);
void material_system_change_material_texture(Asset_System* asset_system, Material_Handle material_handle,
                                             const char* param_name, const char* texture_name);

void material_system_get_material_data(Asset_System* asset_system, Material_Handle handle);


void material_system_swap_material(Asset_System* asset_system, Material_Handle material_handle,
                                   const char* material_name);


void material_system_add_mesh_instance_and_material(Asset_System* asset_system, Madness_Mesh* madness_mesh,
                                                    Madness_Mesh_Instance* parent_instance);
void material_system_add_skinned_instance_and_material(Asset_System* asset_system, Madness_Skinned_Mesh* madness_mesh,
                                                       Madness_Skinned_Mesh_Instance* parent_instance);


//

Material_Key material_generate_id(Material_Info* material_info);


void material_create_gpu_definition(Asset_System* asset_system, Reflection_Runtime_Struct* reflection_material,
                                    Material_GPU_Definition* out_material_gpu_definition);
/**
 * @brief: sets any uuids in the materials to the default texture uuid
 */
void material_instance_set_default_textures(Asset_System* asset_system,
                                            Material_Instance* material_instance,
                                            Material_Definition* material_definition);

void material_asset_create(Asset_System* asset_system,
                           Material_Info* material_info,
                           Material_Asset* out_material_asset);

void material_definition_create(Asset_System* asset_system,
                                Material_Definition* material_definition,
                                const char* material_name);
/**
 * @note: allocates for a material instance with all values to 0, user has to fill it out
 * @note: its required that there be a material asset for creating an instance
 */
void material_instance_create(Asset_System* asset_system,
                              Material_Asset* material_asset,
                              Material_Definition* material_definition,
                              Material_Instance* out_material_instance,
                              const char* mat_inst_name);

void material_instance_create_from_data(Asset_System* asset_system,
                                        Material_Asset* material_asset,
                                        Material_Definition* material_definition,
                                        Material_Instance* out_material_instance,
                                        const char* mat_instance_name,
                                        void* data);
#endif //MATERIAL_SYSTEM_H
