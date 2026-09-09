#ifndef MESH_H
#define MESH_H
#include "scene.h"


Mesh_System* mesh_system_init(Asset_System* resource_system, Memory_System* memory_system);

bool mesh_system_shutdown(Mesh_System* mesh_system, Memory_System* memory_system);


bool mesh_acquire(Asset_System* asset_system, Madness_Mesh_Runtime* mesh_asset, Mesh_Handle* out_handle)
{
    madness_uuid_validate(mesh_asset->mesh_uuid);

    Mesh_System* mesh_system = asset_system->mesh_system;

    //TODO: find a proper free range for meshes
    *out_handle = (Mesh_Handle){.handle = mesh_system->mesh_instance_count, 0};
    Madness_Mesh_Instance* mesh_inst = &mesh_system->mesh_instance[mesh_system->mesh_instance_count];

    mesh_system->mesh_instance_count += mesh_asset->mesh_count;


    //meta data of the mesh
    Madness_Mesh* madness_mesh = &mesh_system->madness_mesh[mesh_system->madness_mesh_count++];
    madness_mesh->mesh_count = mesh_asset->mesh_count;
    madness_mesh->mesh_data = mesh_asset->submeshes;
    madness_mesh->submesh_ids =
        allocator_heap_alloc(asset_system->heap_allocator, sizeof(u32) * mesh_asset->mesh_count);


    //create the instance
    //OPTIMIZE: submesh's should really be a flat list so that the render can quickly extract data from it


    mesh_inst->mesh_reference_index = mesh_system->madness_mesh_count - 1;
    scene_get_new_transform(asset_system->scene, &mesh_inst->transform_handle, mesh_asset->mesh_uuid);
    mesh_inst->mesh_count = mesh_asset->mesh_count;
    mesh_inst->submesh_instances = allocator_heap_alloc(
        asset_system->heap_allocator, sizeof(Madness_SubMesh_Instance) * mesh_asset->mesh_count);

    for (size_t mesh_idx = 0; mesh_idx < mesh_asset->mesh_count; mesh_idx++)
    {
        u32 cur_mesh_id = mesh_system->mesh_ids++;
        madness_mesh->submesh_ids[mesh_idx] = cur_mesh_id;

        Madness_SubMesh_Instance* submesh_inst = &mesh_inst->submesh_instances[mesh_idx];
        //handles
        submesh_inst->mesh_id = cur_mesh_id;
        submesh_inst->material_handle = (Material_Handle){0};
        submesh_inst->parent_transform_handle = mesh_inst->transform_handle;

        //send to the gpu
        Mesh_GPU_Upload upload = {
            .mesh_id = cur_mesh_id,
            .submesh = &mesh_asset->submeshes[mesh_idx],
            .gpu_data = &mesh_asset->mesh_gpu_upload[mesh_idx],
            .mesh_memory_allocator = asset_system->mesh_allocator,

        };
        ring_enqueue(mesh_system->mesh_ring_queue, &upload);
    }


    return true;
}


void mesh_release(Asset_System* asset_system, Mesh_Handle handle);
void mesh_get(Asset_System* asset_system, Mesh_Handle handle);


void skinned_mesh_acquire(Asset_System* asset_system, u32 mesh_count, Skinned_Mesh_Handle* out_handle);
void skinned_mesh_release(Asset_System* asset_system, Skinned_Mesh_Handle handle);
void skinned_mesh_get(Asset_System* asset_system, Skinned_Mesh_Handle handle);


void mesh_system_load_skinned_mesh(Asset_System* asset_system, Madness_SkMesh_Runtime* skmesh_asset, u64 hash,
                                   String* engine_path, MADNESS_UUID uuid);

//animation system
GLTF_Animation_Data* sk_mesh_parent_instance_get_animation_data(Mesh_System* mesh_system,
                                                                Madness_Skinned_Mesh_Instance* sk_mesh_inst);


#endif
