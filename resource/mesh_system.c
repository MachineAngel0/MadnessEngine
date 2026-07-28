#include "mesh_system.h"

#include "cgltf.h"
#include "material_system.h"
#include "resource_types.h"
#include "ufbx.h"


Mesh_System* mesh_system_init(Asset_System* resource_system, Memory_System* memory_system)
{
    Mesh_System* out_mesh_system = memory_system_alloc(memory_system, sizeof(Mesh_System), MEMORY_SUBSYSTEM_MESH);
    memset(out_mesh_system, 0, sizeof(Mesh_System));


    out_mesh_system->vertex_byte_size = 0;
    out_mesh_system->index_byte_size = 0;
    out_mesh_system->normals_byte_size = 0;
    out_mesh_system->tangent_byte_size = 0;
    out_mesh_system->uv_byte_size = 0;

    out_mesh_system->mesh_ring_queue = ring_queue_create(sizeof(Mesh_GPU_Upload), MAX_MESH_COUNT);
    out_mesh_system->skinned_mesh_ring_queue = ring_queue_create(sizeof(Skinned_Mesh_GPU_Upload),
                                                                 MAX_SKINNED_MESH_COUNT);


    out_mesh_system->mesh_asset_count = 0;
    out_mesh_system->sk_mesh_asset_count = 0;
    out_mesh_system->mesh_instance_count = 0;
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

bool mesh_system_exists_mesh(Asset_System* asset_system, Madness_Mesh_Handle* out_handle, MADNESS_UUID uuid, u64 hash)
{
    for (u32 i = 0; i < asset_system->mesh_system->madness_asset_count; i++)
    {
        if (asset_system->mesh_system->madness_asset[i].hash == hash)
        {
            asset_system->mesh_system->madness_asset[i].reference_count++;
            FATAL("mesh_system_exists_mesh: NOT PASSING BACK A MESH HANDLE IF ALREADY LOADED")
            //TODO: this should be creating a new parent mesh instance

            return true;
        }
    }


    return false;
}

bool mesh_system_exists_skmesh(Asset_System* asset_system, Madness_SkMesh_Handle* out_handle, MADNESS_UUID uuid,
                               u64 hash)
{
    for (u32 i = 0; i < asset_system->mesh_system->skinned_madness_asset_count; i++)
    {
        if (asset_system->mesh_system->skinned_madness_asset[i].hash == hash)
        {
            asset_system->mesh_system->skinned_madness_asset[i].reference_count++;
            FATAL("mesh_system_exists_mesh: NOT PASSING BACK A MESH HANDLE IF ALREADY LOADED")
            //TODO: this should be creating a new parent skinned mesh instance

            return true;
        }
    }
    return false;
}



void mesh_system_load_mesh(Asset_System* asset_system, Madness_Mesh_Runtime* mesh_asset, MADNESS_UUID uuid, u64 hash)
{
    Mesh_System* mesh_system = asset_system->mesh_system;

    for (size_t mesh_idx = 0; mesh_idx < mesh_asset->mesh_count; mesh_idx++)
    {
        Mesh_GPU_Upload upload = {
            .submesh = &mesh_asset->submeshes[mesh_idx],
            .gpu_data = &mesh_asset->mesh_gpu_upload[mesh_idx]
        };
        ring_enqueue(mesh_system->mesh_ring_queue, &upload);
    }

    //take a reference to the og asset
    Madness_Mesh* madness_mesh = &mesh_system->madness_mesh[mesh_system->mesh_asset_count++];
    madness_mesh->mesh_count = mesh_asset->mesh_count;
    madness_mesh->mesh_data = mesh_asset->submeshes;
    madness_mesh->material_instance = mesh_asset->material_instance;

    //TODO: out into its own function
    //find free space for the loaded in data
    for (size_t mesh_idx = 0; mesh_idx < mesh_asset->mesh_count; mesh_idx++)
    {
        Madness_SubMesh* submesh = &madness_mesh->mesh_data[mesh_idx];
        submesh->vertex_count_offset = mesh_system->vertex_count_size;
        submesh->vertex_offset = mesh_system->vertex_byte_size;
        submesh->uv_offset = mesh_system->uv_byte_size;
        submesh->vertex_color_offset = mesh_system->vertex_color_byte_size;
        submesh->tangent_offset = mesh_system->tangent_byte_size;
        submesh->normal_offset = mesh_system->normals_byte_size;
        submesh->index_offset = mesh_system->index_count_size;

        mesh_system->vertex_count_size += submesh->vertex_bytes / sizeof(vec3s);
        mesh_system->vertex_byte_size += submesh->vertex_bytes;
        mesh_system->uv_byte_size += submesh->uv_bytes;
        mesh_system->vertex_color_byte_size += submesh->vertex_color_bytes;
        mesh_system->tangent_byte_size += submesh->tangent_bytes;
        mesh_system->normals_byte_size += submesh->normal_bytes;
        mesh_system->index_byte_size += submesh->indices_bytes;
        mesh_system->index_count_size += submesh->index_count;
    }


    //create the instance
    //OPTIMIZE: submehses should really be a flat list so that the render can quickly extract data from it
    Madness_Mesh_Instance* mesh_inst = &mesh_system->mesh_instance[mesh_system->
        mesh_instance_count++];
    mesh_inst->mesh_asset = (Madness_Mesh_Handle){.handle = mesh_system->mesh_asset_count - 1};
    mesh_inst->transform_handle = scene_get_new_mesh_transform(asset_system->scene);
    mesh_inst->mesh_count = mesh_asset->mesh_count;
    mesh_inst->submesh_instances = allocator_heap_alloc(
        asset_system->heap_allocator, sizeof(Madness_SubMesh_Instance) * mesh_asset->mesh_count);

    for (size_t mesh_idx = 0; mesh_idx < mesh_asset->mesh_count; mesh_idx++)
    {
        Madness_SubMesh_Instance* submesh_inst = &mesh_inst->submesh_instances[mesh_idx];

        //handles
        submesh_inst->material_handle = (Material_Handle){0};
        submesh_inst->parent_transform_handle = mesh_inst->transform_handle;

        //indirect draw, gpu friendly format
        submesh_inst->mesh_indirect_draw.vertex_count_offset
            = madness_mesh->mesh_data[mesh_idx].vertex_count_offset;
        submesh_inst->mesh_indirect_draw.index_count
            = madness_mesh->mesh_data[mesh_idx].index_count;
        submesh_inst->mesh_indirect_draw.index_offset
            = madness_mesh->mesh_data[mesh_idx].index_offset;
    }

    //loads in the material asset if needed, and adds material instance data to the material batch
    material_system_add_mesh_instance_and_material(asset_system, madness_mesh, mesh_inst);


    //take a reference to the og asset
    Madness_Asset* asset = &mesh_system->madness_asset[mesh_system->madness_asset_count++];
    asset->hash = hash;
    asset->uuid = uuid;
    asset->reference_count = 1;
    asset->type = ASSET_STATIC_MESH;
}

void mesh_system_load_skinned_mesh(Asset_System* asset_system, Madness_SkMesh_Runtime* skmesh_asset, MADNESS_UUID uuid,
                                   u64 hash)
{
    Mesh_System* mesh_system = asset_system->mesh_system;

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

    Madness_Skinned_Mesh* madness_mesh = &mesh_system->madness_skinned_mesh[mesh_system->sk_mesh_asset_count++];
    madness_mesh->mesh_count = skmesh_asset->mesh_count;
    madness_mesh->mesh_data = skmesh_asset->submeshes;
    madness_mesh->material_instance = skmesh_asset->material_instance;
    madness_mesh->skinned_mesh_data = skmesh_asset->skinned_submeshes;
    madness_mesh->animation_data = skmesh_asset->animation_data;

    //TODO: out into its own function
    //find free space for the loaded in data
    for (size_t mesh_idx = 0; mesh_idx < skmesh_asset->mesh_count; mesh_idx++)
    {
        Madness_SubMesh* submesh = &madness_mesh->mesh_data[mesh_idx];
        submesh->vertex_count_offset = mesh_system->vertex_count_size;
        submesh->vertex_offset = mesh_system->vertex_byte_size;
        submesh->uv_offset = mesh_system->uv_byte_size;
        submesh->vertex_color_offset = mesh_system->vertex_color_byte_size;
        submesh->tangent_offset = mesh_system->tangent_byte_size;
        submesh->normal_offset = mesh_system->normals_byte_size;
        submesh->index_offset = mesh_system->index_count_size;

        mesh_system->vertex_count_size += submesh->vertex_bytes / sizeof(vec3s);
        mesh_system->vertex_byte_size += submesh->vertex_bytes;
        mesh_system->uv_byte_size += submesh->uv_bytes;
        mesh_system->vertex_color_byte_size += submesh->vertex_color_bytes;
        mesh_system->tangent_byte_size += submesh->tangent_bytes;
        mesh_system->normals_byte_size += submesh->normal_bytes;
        mesh_system->index_byte_size += submesh->indices_bytes;
        mesh_system->index_count_size += submesh->index_count;


        Madness_Skinned_SubMesh* skinned_submesh = &madness_mesh->skinned_mesh_data[mesh_idx];

        skinned_submesh->joint_offset_bytes = mesh_system->joints_byte_size;
        skinned_submesh->joint_offset_vec4 = mesh_system->joints_byte_size / sizeof(vec4s);
        skinned_submesh->weight_offset_bytes = mesh_system->weight_byte_size;
        skinned_submesh->weight_offset_vec4 = mesh_system->weight_byte_size / sizeof(vec4s);


        mesh_system->joints_byte_size += skinned_submesh->joint_bytes;
        mesh_system->weight_byte_size += skinned_submesh->weight_bytes;
    }


    //create the instance
    //OPTIMIZE: submehses should really be a flat list so that the render can quickly extract data from it
    Madness_Skinned_Mesh_Instance* mesh_inst = &mesh_system->skinned_mesh_instance[mesh_system->
        skinned_mesh_instance_count++];
    mesh_inst->skinned_mesh_asset = (Madness_SkMesh_Handle){.handle = mesh_system->skinned_mesh_instance_count - 1};
    mesh_inst->transform_handle = scene_get_new_mesh_transform(asset_system->scene);
    mesh_inst->mesh_count = skmesh_asset->mesh_count;
    mesh_inst->submesh_instances = allocator_heap_alloc(
        asset_system->heap_allocator, sizeof(Madness_Skinned_Submesh_Instance) * skmesh_asset->mesh_count);


    //load in animation data before we load in mesh instance, so we get an accurate skinned matrix count

    animation_add_data(asset_system, madness_mesh, &mesh_inst->animation_handle, &mesh_inst->skinned_matrix_count_offset);


    for (size_t mesh_idx = 0; mesh_idx < skmesh_asset->mesh_count; mesh_idx++)
    {
        Madness_Skinned_Submesh_Instance* submesh_inst = &mesh_inst->submesh_instances[mesh_idx];
        Madness_Skinned_SubMesh* skinned_submesh = &madness_mesh->skinned_mesh_data[mesh_idx];

        //handles
        submesh_inst->material_handle = (Material_Handle){0};
        submesh_inst->parent_transform_handle = mesh_inst->transform_handle;

        //indirect draw, gpu friendly format
        submesh_inst->mesh_indirect_draw.vertex_count_offset
            = madness_mesh->mesh_data[mesh_idx].vertex_count_offset;
        submesh_inst->mesh_indirect_draw.index_count
            = madness_mesh->mesh_data[mesh_idx].index_count;
        submesh_inst->mesh_indirect_draw.index_offset
            = madness_mesh->mesh_data[mesh_idx].index_offset;

        //skinned draw data

        submesh_inst->skinned_draw_data.joint_idx = skinned_submesh->joint_offset_vec4;
        submesh_inst->skinned_draw_data.weight_idx = skinned_submesh->weight_offset_vec4;
        submesh_inst->skinned_draw_data.skinned_matrix_idx = mesh_inst->skinned_matrix_count_offset;
        submesh_inst->skinned_draw_data.vertex_byte_offset = madness_mesh->mesh_data[mesh_idx].vertex_offset;
    }


    //loads in the material asset if needed, and adds material instance data to the material batch
    material_system_add_skinned_instance_and_material(asset_system, madness_mesh, mesh_inst);


    //take a reference to the og asset
    Madness_Asset* asset = &mesh_system->skinned_madness_asset[mesh_system->skinned_madness_asset_count++];
    asset->hash = hash;
    asset->uuid = uuid;
    asset->reference_count = 1;
    asset->type = ASSET_SKINNED_MESH;
}

