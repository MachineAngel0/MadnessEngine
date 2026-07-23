#include "mesh_system.h"

#include "cgltf.h"
#include "c_string.h"
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

    out_mesh_system->skinned_matrix_array = dynamic_array_create(mat4s, 100, resource_system->heap_allocator);

    out_mesh_system->mesh_asset_count = 0;
    out_mesh_system->sk_mesh_asset_count = 0;
    out_mesh_system->mesh_parent_instance_count = 0;
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

void mesh_system_load_mesh(Asset_System* asset_system, Madness_Mesh_Runtime* mesh_asset)
{
    Mesh_System* mesh_system = asset_system->mesh_system;
    Heap_Allocator* allocator = asset_system->heap_allocator;
    Frame_Allocator* frame_allocator = asset_system->frame_allocator;

    for (size_t mesh_idx = 0; mesh_idx < mesh_asset->mesh_count; mesh_idx++)
    {
        ring_enqueue(mesh_system->mesh_ring_queue, &mesh_asset->mesh_gpu_upload[mesh_idx]);
    }

    //take a reference to the og asset
    Madness_Mesh* madness_mesh = &mesh_system->madness_mesh[mesh_system->mesh_asset_count++];
    madness_mesh->mesh_count = mesh_asset->mesh_count;
    madness_mesh->mesh_data = mesh_asset->submeshes;


    //create the instance
    Madness_Mesh_Instance* mesh_inst = &mesh_system->mesh_parent_instance[mesh_system->
        mesh_parent_instance_count++];
    mesh_inst->mesh_asset = (Madness_Mesh_Handle){.handle = mesh_system->mesh_asset_count - 1};
    mesh_inst->transform_handle = scene_get_new_mesh_transform(asset_system->scene);
    mesh_inst->mesh_count = mesh_asset->mesh_count;
    mesh_inst->submesh_instances = allocator_heap_alloc(
        allocator, sizeof(Madness_SubMesh_Instance) * mesh_asset->mesh_count);

    for (size_t mesh_idx = 0; mesh_idx < mesh_asset->mesh_count; mesh_idx++)
    {
        Madness_SubMesh_Instance* submesh_inst = &mesh_inst->submesh_instances[mesh_idx];

        //handles
        submesh_inst->material_handle = (Material_Handle){0};
        submesh_inst->parent_transform_handle = mesh_inst->transform_handle;

        //gpu friendly format
        submesh_inst->mesh_gpu_draw.material_instance_handle = 0; // TODO/TEMP:
        submesh_inst->mesh_gpu_draw.transform_idx = mesh_inst->transform_handle.handle;

        //indirect draw, gpu friendly format
        mesh_inst->submesh_instances[mesh_idx].mesh_indirect_draw.vertex_offset
            = madness_mesh->mesh_data[mesh_idx].vertex_offset;
        mesh_inst->submesh_instances[mesh_idx].mesh_indirect_draw.index_count
            = madness_mesh->mesh_data[mesh_idx].index_count;
        mesh_inst->submesh_instances[mesh_idx].mesh_indirect_draw.index_offset
            = madness_mesh->mesh_data[mesh_idx].index_offset;
    }
    material_system_add_mesh_instance_and_material(asset_system, mesh_inst);
}

void mesh_system_load_skinned_mesh(Asset_System* resource_system, Madness_SkMesh_Runtime* skmesh_asset)
{
    /*{
        //create the parent instance
        Sk_Mesh_Parent_Instance* sk_mesh_parent_inst = &mesh_system->skinned_mesh_instance[mesh_system->
            skinned_mesh_instance_count++];
        sk_mesh_parent_inst->sk_mesh_handle = (Sk_Mesh_Asset_Handle){.handle = mesh_system->sk_mesh_asset_count - 1};
        sk_mesh_parent_inst->transform_handle = scene_get_new_mesh_transform(resource_system->scene);
        sk_mesh_parent_inst->mesh_count = sk_mesh_asset_data->skinned_mesh_count;
        sk_mesh_parent_inst->sk_mesh_instance_array = allocator_heap_alloc(
            allocator, sizeof(Sk_Mesh_Instance) * sk_mesh_parent_inst->mesh_count);

        for (int i = 0; i < sk_mesh_asset_data->mesh_count; ++i)
        {
            Mesh_Data* mesh_data = &sk_mesh_asset_data->mesh_data[i];
            Sk_Mesh_Data* sk_mesh_data = &sk_mesh_asset_data->skinned_mesh_data[i];
            Sk_Mesh_Instance* sk_mesh_instance = &sk_mesh_parent_inst->sk_mesh_instance_array[i];
            sk_mesh_instance->material_handle = mesh_data->default_material_handle;
            sk_mesh_instance->parent_transform_handle = sk_mesh_parent_inst->transform_handle;

            //gpu friendly format
            sk_mesh_instance->sk_mesh_gpu_draw.material_instance_handle = sk_mesh_instance->material_handle.handle;
            sk_mesh_instance->sk_mesh_gpu_draw.transform_idx = sk_mesh_parent_inst->transform_handle.handle;

            sk_mesh_instance->sk_mesh_gpu_draw.joint_idx = sk_mesh_data->joint_offset_vec4;
            sk_mesh_instance->sk_mesh_gpu_draw.weight_idx = sk_mesh_data->weight_offset_vec4;

            //indirect draw, gpu friendly format
            sk_mesh_instance->mesh_indirect_draw.vertex_offset = mesh_data->vertex_offset;

            sk_mesh_instance->mesh_indirect_draw.index_count = mesh_data->index_count;
            sk_mesh_instance->mesh_indirect_draw.index_offset = mesh_data->index_offset;
        }


        GLTF_Animation_Data* animation_data = sk_mesh_asset_data->animation_data;

        sk_mesh_parent_inst->joint_count = sk_mesh_asset_data->animation_data->joint_count;

        sk_mesh_parent_inst->local_translation = allocator_heap_alloc(
            allocator, sizeof(vec3s) * sk_mesh_parent_inst->joint_count);
        sk_mesh_parent_inst->local_rotation =
            allocator_heap_alloc(allocator, sizeof(versors) * sk_mesh_parent_inst->joint_count);
        sk_mesh_parent_inst->local_scale = allocator_heap_alloc(
            allocator, sizeof(vec3s) * sk_mesh_parent_inst->joint_count);
        sk_mesh_parent_inst->gpu_matrix = allocator_heap_alloc(
            allocator, sizeof(mat4s) * sk_mesh_parent_inst->joint_count);

        for (u32 j = 0; j < sk_mesh_parent_inst->joint_count; j++)
        {
            vec4s translation = {0};
            mat4s rotation = {0};
            vec3s scale = {0};

            glms_decompose(
                animation_data->resting_pose_local_matrix[j],
                &translation,
                &rotation,
                &scale
            );


            glm_vec3_copy(translation.raw, sk_mesh_parent_inst->local_translation[j].raw);
            // glm_quat_copy(rotation, sk_mesh_inst->local_rotation[j].raw);
            sk_mesh_parent_inst->local_rotation[j] = glms_mat4_quat(rotation);
            glm_vec3_copy(scale.raw, sk_mesh_parent_inst->local_scale[j].raw);
        }


        sk_mesh_parent_inst->current_animation_index = 0;
        sk_mesh_parent_inst->current_time = 0;
        sk_mesh_parent_inst->looping = true;

        material_system_add_skmesh_instance_to_default_material_batch(resource_system, sk_mesh_parent_inst);
    }*/
}

bool mesh_system_exists_mesh(Asset_System* resource_system, Madness_Mesh_Handle* out_handle, u64 hash)
{
    DEBUG("TODO: mesh_system_exists_mesh")
    return false;
}

bool mesh_system_exists_skmesh(Asset_System* resource_system, Madness_SkMesh_Handle* out_handle, u64 hash)
{
    DEBUG("TODO: mesh_system_exists_mesh")
    return false;
}

GLTF_Animation_Data* sk_mesh_parent_instance_get_animation_data(Mesh_System* mesh_system,
                                                                Madness_SkMesh_Instance* sk_mesh_inst)
{
    return mesh_system->skinned_mesh_asset_data[sk_mesh_inst->sk_mesh_handle.handle].animation_data;
}

void animation_update(Mesh_System* mesh_system, float delta_time, Frame_Allocator* frame_allocator)
{
    //update and interpolate the local transformations for the playing animations
    //create the local matrix
    // joint1 =  local parent * local joint
    // joint2 =  joint1 * inverse_bind_matrix
    //send to the gpu for the shader to work


    //assume the mesh is loaded

    dynamic_array_clear(mesh_system->skinned_matrix_array);


    for (u32 i = 0; i < mesh_system->skinned_mesh_instance_count; ++i)
    {
        Madness_SkMesh_Instance* sk_mesh_inst = &mesh_system->skinned_mesh_instance[i];


        GLTF_Animation_Data* animation_data = mesh_system->skinned_mesh_asset_data[sk_mesh_inst->sk_mesh_handle.handle].
            animation_data;
        Animation* anim_data = &animation_data->animations[sk_mesh_inst->current_animation_index];

        /*
        for (u32 j = 0; j < sk_mesh_inst->joint_count; j++)
        {
            vec4s translation = {0};
            mat4s rotation= {0};
            vec3s scale= {0};

            glms_decompose(
                animation_data->resting_pose_local_matrix[j],
                &translation,
                &rotation,
                &scale
            );



            glm_vec3_copy(translation.raw, sk_mesh_inst->local_translation[j].raw);
            // glm_quat_copy(rotation, sk_mesh_inst->local_rotation[j].raw);
            sk_mesh_inst->local_rotation[j] = glms_mat4_quat(rotation);
            glm_vec3_copy(scale.raw, sk_mesh_inst->local_scale[j].raw);
        }
        */


        sk_mesh_inst->current_time += delta_time;

        if (sk_mesh_inst->current_time >= anim_data->anim_end)
        {
            //TODO: at some point this should be its own array of inactive animations
            if (sk_mesh_inst->looping)
            {
                while (sk_mesh_inst->current_time >= anim_data->anim_end)
                {
                    sk_mesh_inst->current_time -= (anim_data->anim_end - anim_data->anim_start);
                }
            }
        }


        for (u32 channel_idx = 0; channel_idx < anim_data->channel_count; channel_idx++)
        {
            const Animation_Channel* channel = &anim_data->channels[channel_idx];
            const Animation_Sampler* sampler = &anim_data->samplers[channel->sampler_idx];

            u32 j_idx = channel->joint_index;

            for (size_t timestamp_idx = 0; timestamp_idx < sampler->timestamps_count - 1; timestamp_idx++)
            {
                if (sampler->interpolation_type != Animation_Interpolation_Type_Linear)
                {
                    // DEBUG("ANIMATION UPDATE: Only using linear interpolations, which this is not");
                    continue;
                }

                // Get the input keyframe values for the current time stamp
                if ((sk_mesh_inst->current_time >= sampler->timestamps[timestamp_idx]) && (
                    sk_mesh_inst->current_time <= sampler->timestamps[timestamp_idx + 1]))
                {
                    float interp_val = (sk_mesh_inst->current_time - sampler->timestamps[timestamp_idx]) / (sampler->
                        timestamps[timestamp_idx + 1] - sampler->timestamps[timestamp_idx]);
                    switch (channel->animation_path_type)
                    {
                    case Animation_Path_Type_Invalid:
                        MASSERT(false);
                        break;
                    case Animation_Path_Type_Translation:
                        sk_mesh_inst->local_translation[j_idx] = glms_vec3_lerp(
                            sampler->interperlation_data.trs_vec3[timestamp_idx],
                            sampler->interperlation_data.trs_vec3[timestamp_idx +
                                1], interp_val);
                        break;
                    case Animation_Path_Type_Rotation:
                        versors q1;
                        q1.x = sampler->interperlation_data.trs_vec4[timestamp_idx].x;
                        q1.y = sampler->interperlation_data.trs_vec4[timestamp_idx].y;
                        q1.z = sampler->interperlation_data.trs_vec4[timestamp_idx].z;
                        q1.w = sampler->interperlation_data.trs_vec4[timestamp_idx].w;

                        versors q2;
                        q2.x = sampler->interperlation_data.trs_vec4[timestamp_idx + 1].x;
                        q2.y = sampler->interperlation_data.trs_vec4[timestamp_idx + 1].y;
                        q2.z = sampler->interperlation_data.trs_vec4[timestamp_idx + 1].z;
                        q2.w = sampler->interperlation_data.trs_vec4[timestamp_idx + 1].w;

                        sk_mesh_inst->local_rotation[j_idx] = glms_quat_normalize(
                            glms_quat_slerp(q1, q2, interp_val));

                        break;
                    case Animation_Path_Type_Scale:
                        sk_mesh_inst->local_scale[j_idx] = glms_vec3_lerp(
                            sampler->interperlation_data.trs_vec3[timestamp_idx],
                            sampler->interperlation_data.trs_vec3[timestamp_idx + 1],
                            interp_val);
                        break;
                    case Animation_Path_Type_Weights:
                        M_ERROR("Animation_Path_Type_Weights: unhandled type");
                        break;
                    case Animation_Path_Type_Max:
                        MASSERT(false);
                        break;
                    }
                }
            }
        }

        mat4s* local_matrix = allocator_alloc(frame_allocator, sizeof(mat4s) * sk_mesh_inst->joint_count);
        mat4s* model_matrix = allocator_alloc(frame_allocator, sizeof(mat4s) * sk_mesh_inst->joint_count);

        for (u32 local_idx = 0; local_idx < sk_mesh_inst->joint_count; local_idx++)
        {
            // Joint(N, t) = Translation(N, t) * Rotation(N, t) * Scale(N, t)

            local_matrix[local_idx] = glms_build_local_matrix(sk_mesh_inst->local_translation[local_idx],
                                                              sk_mesh_inst->local_rotation[local_idx],
                                                              sk_mesh_inst->local_scale[local_idx]);
        }

        model_matrix[0] = local_matrix[0];
        for (u32 model_idx = 1; model_idx < sk_mesh_inst->joint_count; model_idx++)
        {
            //Joint(N, t) = Parent(N, t) * Joint(N, t)
            u32 parent_index = animation_data->joints[model_idx].parent_idx;
            model_matrix[model_idx] = glms_mat4_mul(model_matrix[parent_index], local_matrix[model_idx]);
        }

        for (u32 final_idx = 0; final_idx < sk_mesh_inst->joint_count; final_idx++)
        {
            //Joint(N, t) = Joint(N, t) * InverseBindMatrix(N)
            //TODO: we have two copies of the data technically
            sk_mesh_inst->gpu_matrix[final_idx] = glms_mat4_mul(model_matrix[final_idx],
                                                                animation_data->inverse_bind_matrix[final_idx]
            );


            dynamic_array_push(mesh_system->skinned_matrix_array, &sk_mesh_inst->gpu_matrix[final_idx]);
        }
    }
}
