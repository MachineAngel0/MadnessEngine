#include "mesh_system.h"

#include "animation_system.h"
#include "cgltf.h"
#include "resource_types.h"
#include "ufbx.h"


Mesh_System* mesh_system_init(Asset_System* resource_system, Memory_System* memory_system)
{
    Mesh_System* out_mesh_system = memory_system_alloc(memory_system, sizeof(Mesh_System), MEMORY_SUBSYSTEM_MESH);
    memset(out_mesh_system, 0, sizeof(Mesh_System));


    out_mesh_system->mesh_ring_queue = ring_queue_create(sizeof(Mesh_GPU_Upload), MAX_MESH_COUNT);
    out_mesh_system->skinned_mesh_ring_queue = ring_queue_create(sizeof(Skinned_Mesh_GPU_Upload),
                                                                 MAX_SKINNED_MESH_COUNT);


    out_mesh_system->madness_mesh_count = 0;
    out_mesh_system->mesh_instance_count = 0;
    out_mesh_system->submesh_instance_count = 0;

    out_mesh_system->madness_sk_mesh_count = 0;
    out_mesh_system->skinned_mesh_instance_count = 0;

    INFO("MESH SYSTEM CREATED");

    return out_mesh_system;
}

bool mesh_system_shutdown(Mesh_System* mesh_system, Memory_System* memory_system)
{
    MASSERT(mesh_system);
    memory_system_memory_free(memory_system, mesh_system, MEMORY_SUBSYSTEM_MESH);

    mesh_system = NULL;

    return true;
}


void mesh_system_load_skinned_mesh(Asset_System* asset_system, Madness_SkMesh_Runtime* skmesh_asset,
                                   u64 hash, String* engine_path, MADNESS_UUID uuid)
{
    /*Mesh_System* mesh_system = asset_system->mesh_system;

    for (size_t mesh_idx = 0; mesh_idx < skmesh_asset->mesh_count; mesh_idx++)
    {
        Mesh_GPU_Upload upload = {
            .submesh = &skmesh_asset->submeshes[mesh_idx], .gpu_data = &skmesh_asset->mesh_gpu_upload[mesh_idx]
        };
        ring_enqueue(mesh_system->mesh_ring_queue, &upload);


        Skinned_Mesh_GPU_Upload skinned_upload = {
            .skinned_submesh = &skmesh_asset->skinned_submeshes[mesh_idx],
            .skinned_gpu_data = &skmesh_asset->skmesh_gpu_upload[mesh_idx],

        };
        ring_enqueue(mesh_system->skinned_mesh_ring_queue, &skinned_upload);
    }

    Madness_Skinned_Mesh* madness_mesh = &mesh_system->madness_skinned_mesh[mesh_system->madness_sk_mesh_count++];
    madness_mesh->mesh_count = skmesh_asset->mesh_count;
    madness_mesh->mesh_data = skmesh_asset->submeshes;
    madness_mesh->material_instance = skmesh_asset->material_uuid;
    madness_mesh->skinned_mesh_data = skmesh_asset->skinned_submeshes;
    madness_mesh->animation_data = skmesh_asset->animation_data;


    //create the instance
    //OPTIMIZE: submehses should really be a flat list so that the render can quickly extract data from it
    Madness_Skinned_Mesh_Instance* mesh_inst = &mesh_system->skinned_mesh_instance[mesh_system->
        skinned_mesh_instance_count++];
    mesh_inst->skinned_mesh_asset = (Madness_SkMesh_Handle_Internal){
        .handle = mesh_system->skinned_mesh_instance_count - 1
    };
    scene_get_new_transform(asset_system->scene, &mesh_inst->transform_handle, uuid);
    mesh_inst->mesh_count = skmesh_asset->mesh_count;
    mesh_inst->submesh_instances = allocator_heap_alloc(
        asset_system->heap_allocator, sizeof(Madness_Skinned_Submesh_Instance) * skmesh_asset->mesh_count);


    //load in animation data before we load in mesh instance, so we get an accurate skinned matrix count

    animation_add_data(asset_system, madness_mesh, &mesh_inst->animation_handle,
                       &mesh_inst->skinned_matrix_count_offset);


    for (size_t mesh_idx = 0; mesh_idx < skmesh_asset->mesh_count; mesh_idx++)
    {
        Madness_Skinned_Submesh_Instance* submesh_inst = &mesh_inst->submesh_instances[mesh_idx];

        //handles
        // submesh_inst->skinned_id = mesh_system->skinned_ids++;
        submesh_inst->material_handle = (Material_Handle){0};
        submesh_inst->parent_transform_handle = mesh_inst->transform_handle;
    }*/
}
