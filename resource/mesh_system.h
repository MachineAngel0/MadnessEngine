#ifndef MESH_H
#define MESH_H
#include "scene.h"


Mesh_System* mesh_system_init(Asset_System* resource_system, Memory_System* memory_system);

bool mesh_system_shutdown(Mesh_System* mesh_system, Memory_System* memory_system);

// bool mesh_acquire_instance(Asset_System* asset_system, Madness_Mesh_Runtime* mesh_asset, Mesh_Handle* out_handle)
// { }

bool mesh_acquire(Asset_System* asset_system, Madness_Mesh_Runtime* mesh_asset, Mesh_Handle* out_handle,
                  Material_Handle* in_material_handles)
{
    madness_uuid_validate(mesh_asset->mesh_uuid);

    Mesh_System* mesh_system = asset_system->mesh_system;

    //TODO: find a slot for the mesh asset/metadata, also lazy load this asset, if it has already been loaded
    const u32 mesh_asset_index = mesh_system->madness_mesh_count;
    Madness_Mesh* madness_mesh = &mesh_system->madness_mesh[mesh_system->madness_mesh_count++];
    madness_mesh->mesh_count = mesh_asset->mesh_count;
    madness_mesh->mesh_data = mesh_asset->submeshes;
    madness_mesh->material_uuid = mesh_asset->material_uuid;
    madness_mesh->material_handles = in_material_handles;

    //TODO: find a slot for the mesh instance, (this will be the handle passed back)
    const u32 mesh_instance_idx = mesh_system->mesh_instance_count;
    *out_handle = (Mesh_Handle){.handle = mesh_instance_idx, 0};
    Madness_Mesh_Instance* mesh_inst = &mesh_system->mesh_instance[mesh_instance_idx];
    mesh_system->mesh_instance_count++;
    mesh_inst->mesh_count = mesh_asset->mesh_count;
    // mesh_inst->first_submesh = mesh_asset->mesh_count;
    mesh_inst->mesh_asset_index = mesh_asset_index;
    scene_get_new_transform(asset_system->scene, &mesh_inst->transform_handle, mesh_asset->mesh_uuid);


    //TODO: find a free range for the submeshes needed
    const u32 start_submesh_index = mesh_system->submesh_instance_count;
    mesh_system->submesh_instance_count += mesh_asset->mesh_count;
    mesh_inst->start_submesh_index = start_submesh_index;


    // the submesh index is different from the upload data, since that allocated data starting at 0 and eventually needs to be freed
    size_t upload_index = 0;

    //create the submesh instance
    for (size_t submesh_idx = start_submesh_index; submesh_idx < start_submesh_index + mesh_asset->mesh_count;
         submesh_idx++)
    {
        Madness_SubMesh_Instance* submesh_inst = &mesh_system->submesh_instances[submesh_idx];
        //handles
        submesh_inst->mesh_asset_index = mesh_asset_index;
        submesh_inst->parent_instance_index = mesh_instance_idx;
        submesh_inst->material_handle = (Material_Handle){0};
        submesh_inst->parent_transform_handle = mesh_inst->transform_handle;

        //send to the gpu
        Mesh_GPU_Upload upload = {
            .submesh_id = submesh_idx,
            .submesh = &mesh_asset->submeshes[upload_index],
            .gpu_data = &mesh_asset->mesh_gpu_upload[upload_index],
            .mesh_memory_allocator = asset_system->mesh_allocator,

        };
        ring_enqueue(mesh_system->mesh_ring_queue, &upload);
        upload_index++;
    }

    return true;
}


void mesh_release(Asset_System* asset_system, Mesh_Handle handle);
void mesh_get(Asset_System* asset_system, Mesh_Handle handle);

bool skinned_mesh_acquire(Asset_System* asset_system, Madness_SkMesh_Runtime* skinned_mesh_asset, Skinned_Mesh_Handle* out_handle,
                          Material_Handle* in_material_handles)
{
    /*
    madness_uuid_validate(skinned_mesh_asset->skinned_mesh_uuid);

    Mesh_System* mesh_system = asset_system->mesh_system;

    const u32 mesh_asset_index = mesh_system->madness_mesh_count;
    Madness_Mesh* madness_mesh = &mesh_system->madness_mesh[mesh_system->madness_mesh_count++];
    madness_mesh->mesh_count = mesh_asset->mesh_count;
    madness_mesh->mesh_data = mesh_asset->submeshes;
    madness_mesh->material_uuid = mesh_asset->material_uuid;
    madness_mesh->material_handles = in_material_handles;

    //TODO: find a slot for the mesh instance, (this will be the handle passed back)
    const u32 mesh_instance_idx = mesh_system->mesh_instance_count;
    *out_handle = (Mesh_Handle){.handle = mesh_instance_idx, 0};
    Madness_Mesh_Instance* mesh_inst = &mesh_system->mesh_instance[mesh_instance_idx];
    mesh_system->mesh_instance_count++;
    mesh_inst->mesh_count = mesh_asset->mesh_count;
    // mesh_inst->first_submesh = mesh_asset->mesh_count;
    mesh_inst->mesh_asset_index = mesh_asset_index;
    scene_get_new_transform(asset_system->scene, &mesh_inst->transform_handle, mesh_asset->mesh_uuid);


    //TODO: find a free range for the submeshes needed
    const u32 start_submesh_index = mesh_system->submesh_instance_count;
    mesh_system->submesh_instance_count += mesh_asset->mesh_count;
    mesh_inst->start_submesh_index = start_submesh_index;


    // the submesh index is different from the upload data, since that allocated data starting at 0 and eventually needs to be freed
    size_t upload_index = 0;

    //create the submesh instance
    for (size_t submesh_idx = start_submesh_index; submesh_idx < start_submesh_index + mesh_asset->mesh_count;
         submesh_idx++)
    {
        Madness_SubMesh_Instance* submesh_inst = &mesh_system->submesh_instance[submesh_idx];
        //handles
        submesh_inst->mesh_asset_index = mesh_asset_index;
        submesh_inst->parent_instance_index = mesh_instance_idx;
        submesh_inst->material_handle = (Material_Handle){0};
        submesh_inst->parent_transform_handle = mesh_inst->transform_handle;

        //send to the gpu
        Mesh_GPU_Upload upload = {
            .submesh_id = submesh_idx,
            .submesh = &mesh_asset->submeshes[upload_index],
            .gpu_data = &mesh_asset->mesh_gpu_upload[upload_index],
            .mesh_memory_allocator = asset_system->mesh_allocator,

        };
        ring_enqueue(mesh_system->mesh_ring_queue, &upload);
        upload_index++;
    }


*/
    return true;


}
void skinned_mesh_release(Asset_System* asset_system, Skinned_Mesh_Handle handle);
void skinned_mesh_get(Asset_System* asset_system, Skinned_Mesh_Handle handle);


void mesh_system_load_skinned_mesh(Asset_System* asset_system, Madness_SkMesh_Runtime* skmesh_asset, u64 hash,
                                   String* engine_path, MADNESS_UUID uuid);

//animation system
GLTF_Animation_Data* sk_mesh_parent_instance_get_animation_data(Mesh_System* mesh_system,
                                                                Madness_Skinned_Mesh_Instance* sk_mesh_inst);


#endif
