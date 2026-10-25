#ifndef MESH_H
#define MESH_H
#include "animation_system.h"
#include "material_system.h"
#include "scene.h"


Mesh_System* mesh_system_init(Asset_System* resource_system, Memory_System* memory_system);

bool mesh_system_shutdown(Mesh_System* mesh_system, Memory_System* memory_system);


bool mesh_is_loaded(Asset_System* asset_system, Asset_Record* asset_record, Mesh_Handle* out_handle)
{
    /*Mesh_System* mesh_system = asset_system->mesh_system;
    //check the runtime assets
    for (u32 i = 0; i < MAX_MESH_COUNT; i++)
    {
        Asset_Runtime* runtime = &mesh_system->mesh_asset[i];

        if (runtime->hash == asset_record->hash)
        {
            // TODO: fix this garbage later
            // TODO: create a new mesh instance
            runtime->reference_count++;
            runtime->asset_handles.mesh_handle;


            for (u32 madness_mesh_idx = 0; madness_mesh_idx < MAX_MESH_COUNT; madness_mesh_idx++)
            {
                mesh_system->madness_mesh[madness_mesh_idx].mesh_uuid ==
            }


            Madness_Mesh_Instance mesh_instance = {
                .madness_mesh_index = ,
                .transform_handle = ,
                .submesh_count = ,
                .material_handles = ,
                .pipeline_hash = ,
            };


            return true;
        }
    }
    */
    return false;
}



bool skinned_mesh_is_loaded(Asset_System* asset_system, Asset_Record* asset_record, Skinned_Mesh_Handle* out_handle)
{
    //check the runtime assets
    for (u32 i = 0; i < MAX_SKINNED_MESH_COUNT; i++)
    {
        Asset_Runtime* runtime = &asset_system->mesh_system->skinned_asset[i];

        if (runtime->hash == asset_record->hash)
        {
            runtime->reference_count++;
            *out_handle = runtime->asset_handles.skinned_handle;

            //TODO: create a new mesh instance


            return true;
        }
    }

    return false;
}



bool mesh_acquire(Asset_System* asset_system, Asset_Record* asset_record, Madness_Mesh_Runtime* mesh_asset,
                  Mesh_Handle* out_handle, Material_Handle* in_material_handles)
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
    madness_mesh->mesh_uuid = mesh_asset->mesh_uuid;

    //TODO: find a slot for the mesh instance, (this will be the handle passed back)
    const u32 mesh_instance_idx = mesh_system->mesh_instance_count++;
    *out_handle = (Mesh_Handle){.handle = mesh_instance_idx, 0};

    Madness_Mesh_Instance* mesh_inst = &mesh_system->mesh_instance[mesh_instance_idx];
    mesh_inst->submesh_count = mesh_asset->mesh_count;
    // mesh_inst->first_submesh = mesh_asset->mesh_count;
    mesh_inst->madness_mesh_index = mesh_asset_index;
    scene_get_new_transform(asset_system->scene, &mesh_inst->transform_handle, mesh_asset->mesh_uuid);


    // the submesh index is different from the upload data, since that allocated data starting at 0 and eventually needs to be freed

    //TODO: figure out a better way to allocate this
    mesh_inst->material_handles = allocator_alloc(asset_system->allocator,
                                                  mesh_asset->mesh_count * sizeof(Material_Handle));

    mesh_inst->pipeline_hash = allocator_alloc(asset_system->allocator,
                                               mesh_asset->mesh_count * sizeof(Shader_Pipeline_Hash));



    //create the submesh instance
    for (size_t submesh_idx = 0; submesh_idx < mesh_asset->mesh_count; submesh_idx++)
    {
        // TODO: create unique material overrides for each submesh, for now its shared
        mesh_inst->material_handles[submesh_idx] = in_material_handles[submesh_idx];
        mesh_inst->pipeline_hash[submesh_idx] = material_system_get_shader_asset(
            asset_system, mesh_inst->material_handles[submesh_idx])->shader_pipeline_hash;

    }

    //send to the gpu
    Mesh_GPU_Upload upload = {
        .mesh_id = mesh_inst->madness_mesh_index,
        .submesh_count = mesh_inst->submesh_count,
        .submesh = mesh_asset->submeshes,
        .gpu_data = mesh_asset->mesh_gpu_upload,
        .mesh_memory_allocator = asset_system->mesh_allocator,
    };
    ring_enqueue(mesh_system->mesh_ring_queue, &upload);


    //add to the loaded assets list
    Asset_Runtime* runtime = &asset_system->mesh_system->mesh_asset[out_handle->handle];
    asset_runtime_set_info_from_record(runtime, asset_record, ASSET_TEXTURE);
    runtime->asset_handles.mesh_handle = (Mesh_Handle){out_handle->handle, 0};





    return true;
}


void mesh_release(Asset_System* asset_system, Mesh_Handle handle);
void mesh_get(Asset_System* asset_system, Mesh_Handle handle);

bool skinned_mesh_acquire(Asset_System* asset_system, Asset_Record* record, Madness_Skinned_Mesh_Runtime* skinned_mesh_asset,
                          Skinned_Mesh_Handle* out_handle,
                          Material_Handle* in_material_handles)
{

    Mesh_System* mesh_system = asset_system->mesh_system;


    //TODO: find a slot for the mesh asset/metadata, also lazy load this asset, if it has already been loaded
    const u32 mesh_asset_index = mesh_system->madness_sk_mesh_count;
    Madness_Skinned_Mesh* madness_mesh = &mesh_system->madness_skinned_mesh[mesh_system->madness_sk_mesh_count++];
    madness_mesh->mesh_count = skinned_mesh_asset->mesh_count;
    madness_mesh->skinned_mesh_uuid = skinned_mesh_asset->skinned_mesh_uuid;
    madness_mesh->material_handles = in_material_handles;
    madness_mesh->material_uuids = skinned_mesh_asset->material_uuid;
    madness_mesh->mesh_data = skinned_mesh_asset->submeshes;
    madness_mesh->skinned_mesh_data = skinned_mesh_asset->skinned_submeshes;
    madness_mesh->animation_data = skinned_mesh_asset->animation_data;

    //TODO: find a slot for the mesh instance, (this will be the handle passed back)
    const u32 skinned_mesh_instance_idx = mesh_system->skinned_mesh_instance_count++;
    *out_handle = (Skinned_Mesh_Handle){.handle = skinned_mesh_instance_idx};

    Skinned_Mesh_Instance* mesh_inst = &mesh_system->skinned_mesh_instance[skinned_mesh_instance_idx];
    mesh_inst->madness_mesh_index = mesh_asset_index;
    mesh_inst->submesh_count = madness_mesh->mesh_count;
    scene_get_new_transform(asset_system->scene, &mesh_inst->transform_handle, madness_mesh->skinned_mesh_uuid);
    // the submesh index is different from the upload data, since that allocated data starting at 0 and eventually needs to be freed

    //TODO: figure out a better way to allocate this
    mesh_inst->material_handles = allocator_alloc(asset_system->allocator,
                                                  madness_mesh->mesh_count * sizeof(Material_Handle));

    mesh_inst->pipeline_hash = allocator_alloc(asset_system->allocator,
                                               madness_mesh->mesh_count * sizeof(Shader_Pipeline_Hash));
    mesh_inst->permutation = allocator_alloc(asset_system->allocator,
                                             madness_mesh->mesh_count * sizeof(Permutation_Mask));

    //NOTE: idk what this comment is about
    //load in animation data before we load in mesh instance, so we get an accurate skinned matrix count
    animation_add_data(asset_system, madness_mesh, &mesh_inst->animation_handle,
                       &mesh_inst->skinned_matrix_count_offset);

    //create the submesh instances
    for (size_t submesh_idx = 0; submesh_idx < madness_mesh->mesh_count; submesh_idx++)
    {
        // TODO: create unique material overrides for each submesh, for now its shared
        mesh_inst->material_handles[submesh_idx] = in_material_handles[submesh_idx];
        mesh_inst->pipeline_hash[submesh_idx] = material_system_get_shader_asset(
            asset_system, mesh_inst->material_handles[submesh_idx])->shader_pipeline_hash;;
        mesh_inst->permutation[submesh_idx] = 0;

    }


    //send to the gpu
    Skinned_Mesh_GPU_Upload upload = {
        .mesh_id = mesh_inst->madness_mesh_index,
        .submesh_count = skinned_mesh_asset->mesh_count,
        .submesh = skinned_mesh_asset->submeshes,
        .gpu_data = skinned_mesh_asset->mesh_gpu_upload,
        .skinned_submesh_data = madness_mesh->skinned_mesh_data,
        .skinned_gpu_data =  skinned_mesh_asset->skmesh_gpu_upload,
        .mesh_memory_allocator = asset_system->mesh_allocator,
    };
    ring_enqueue(mesh_system->skinned_mesh_ring_queue, &upload);

    //add to the loaded assets list
    Asset_Runtime* runtime = &asset_system->mesh_system->skinned_asset[out_handle->handle];
    asset_runtime_set_info_from_record(runtime, record, ASSET_TEXTURE);
    runtime->asset_handles.skinned_handle = (Skinned_Mesh_Handle){out_handle->handle};

    return true;
}

void skinned_mesh_release(Asset_System* asset_system, Skinned_Mesh_Handle handle);




//animation system
GLTF_Animation_Data* sk_mesh_parent_instance_get_animation_data(Mesh_System* mesh_system,
                                                                Skinned_Mesh_Instance* sk_mesh_inst);


#endif
