#include "animation_system.h"

Animation_System* animation_init(Memory_System* memory_system)
{
    Animation_System* animation_system = memory_system_alloc(memory_system, sizeof(Animation_System),
                                                             MEMORY_SUBSYSTEM_ANIMATION);

    //frame allocator

    //NOTE: should be more than skinned mesh count in theory
    u64 animation_gpu_matrix_byte_size = MAX_SKINNED_MESH_COUNT * sizeof(mat4s);
    animation_system->frame_allocator = memory_system_allocator_create(memory_system, animation_gpu_matrix_byte_size,
                                                                       MEMORY_SUBSYSTEM_ANIMATION);

    return animation_system;
}

bool animation_deinit(Animation_System* animation_system, Memory_System* memory_system)
{
    return false;
}

void animation_add_data(Asset_System* asset_system, Madness_Skinned_Mesh* madness_mesh, Animation_Handle* out_handle,
                        u32* out_skinned_idx)
{
    Animation_System* animation_system = asset_system->animation_system;

    out_handle->handle = animation_system->animation_count;
    out_handle->gen = 0;

    Madness_Animation* animation_data = &animation_system->animation_data[animation_system->animation_count++];
    animation_data->animation_data = madness_mesh->animation_data;
    animation_data->joint_count = madness_mesh->animation_data->joint_count;

    animation_data->local_translation = allocator_heap_alloc(
        asset_system->heap_allocator, sizeof(vec3s) * animation_data->joint_count);
    animation_data->local_rotation =
        allocator_heap_alloc(asset_system->heap_allocator, sizeof(versors) * animation_data->joint_count);
    animation_data->local_scale = allocator_heap_alloc(
        asset_system->heap_allocator, sizeof(vec3s) * animation_data->joint_count);
    animation_data->gpu_matrix = allocator_heap_alloc(
        asset_system->heap_allocator, sizeof(mat4s) * animation_data->joint_count);

    animation_data->skinned_matrix_offset = animation_system->skinned_matrix_offset_count;
    *out_skinned_idx = animation_system->skinned_matrix_offset_count;
    animation_system->skinned_matrix_offset_count += animation_data->joint_count;


    for (u32 j = 0; j < madness_mesh->animation_data->joint_count; j++)
    {
        vec4s translation = {0};
        mat4s rotation = {0};
        vec3s scale = {0};

        glms_decompose(
            animation_data->animation_data->resting_pose_local_matrix[j],
            &translation,
            &rotation,
            &scale
        );

        glm_vec3_copy(translation.raw, animation_data->local_translation[j].raw);
        // glm_quat_copy(rotation, sk_mesh_inst->local_rotation[j].raw);
        animation_data->local_rotation[j] = glms_mat4_quat(rotation);
        glm_vec3_copy(scale.raw, animation_data->local_scale[j].raw);
    }


    animation_data->current_animation_index = 0;
    animation_data->current_time = 0;
    animation_data->looping = true;
}


void animation_system_update(Animation_System* animation_system, float delta_time)
{

    PROFILE_ZONE(animation_system_update)


    //update and interpolate the local transformations for the playing animations
    //create the local matrix
    // joint1 =  local parent * local joint
    // joint2 =  joint1 * inverse_bind_matrix
    //send to the gpu for the shader to work


    allocator_clear(animation_system->frame_allocator);


    //TODO: we can multithread the animation updates, but the upload into the array has to be ordered
    // or we use the count and offset, which we will know somehow, to upload into that part of the array
    /*Job_Counter* job_counter = job_counter_create("animation update", 100);
    job_create_with_counter();*/

    u32 joint_count = 0;
    for (u32 anim_idx = 0; anim_idx < animation_system->animation_count; ++anim_idx)
    {
        joint_count += animation_system->animation_data[anim_idx].joint_count;
    }

    //TODO: probably something better than a dynamic array for this honestly (normal array?)
    animation_system->skinned_matrix_array = array_create(mat4s, joint_count, animation_system->frame_allocator);

    //can be jobified
    for (u32 anim_idx = 0; anim_idx < animation_system->animation_count; ++anim_idx)
    {
        Madness_Animation* madness_anim = &animation_system->animation_data[anim_idx];


        GLTF_Animation_Data* animation_data = madness_anim->animation_data;
        Animation* anim_data = &animation_data->animations[madness_anim->current_animation_index];

        /*for (u32 j = 0; j < animation_data->joint_count; j++)
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



            glm_vec3_copy(translation.raw, madness_anim->local_translation[j].raw);
            // glm_quat_copy(rotation, sk_mesh_inst->local_rotation[j].raw);
            madness_anim->local_rotation[j] = glms_mat4_quat(rotation);
            glm_vec3_copy(scale.raw, madness_anim->local_scale[j].raw);
        }*/


        madness_anim->current_time += delta_time;

        if (madness_anim->current_time >= anim_data->anim_end)
        {
            //TODO: at some point this should be its own array of inactive animations
            if (madness_anim->looping)
            {
                while (madness_anim->current_time >= anim_data->anim_end)
                {
                    madness_anim->current_time -= (anim_data->anim_end - anim_data->anim_start);
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
                if ((madness_anim->current_time >= sampler->timestamps[timestamp_idx]) && (
                    madness_anim->current_time <= sampler->timestamps[timestamp_idx + 1]))
                {
                    float interp_val = (madness_anim->current_time - sampler->timestamps[timestamp_idx]) / (sampler->
                        timestamps[timestamp_idx + 1] - sampler->timestamps[timestamp_idx]);
                    switch (channel->animation_path_type)
                    {
                    case Animation_Path_Type_Invalid:
                        MASSERT(false);
                        break;
                    case Animation_Path_Type_Translation:
                        madness_anim->local_translation[j_idx] = glms_vec3_lerp(
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

                        madness_anim->local_rotation[j_idx] = glms_quat_normalize(
                            glms_quat_slerp(q1, q2, interp_val));

                        break;
                    case Animation_Path_Type_Scale:
                        madness_anim->local_scale[j_idx] = glms_vec3_lerp(
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

        mat4s* local_matrix = allocator_alloc(animation_system->frame_allocator, sizeof(mat4s) * madness_anim->joint_count);
        mat4s* model_matrix = allocator_alloc(animation_system->frame_allocator, sizeof(mat4s) * madness_anim->joint_count);

        for (u32 local_idx = 0; local_idx < madness_anim->joint_count; local_idx++)
        {
            // Joint(N, t) = Translation(N, t) * Rotation(N, t) * Scale(N, t)

            local_matrix[local_idx] = glms_build_local_matrix(madness_anim->local_translation[local_idx],
                                                              madness_anim->local_rotation[local_idx],
                                                              madness_anim->local_scale[local_idx]);
        }

        model_matrix[0] = local_matrix[0];
        for (u32 model_idx = 1; model_idx < madness_anim->joint_count; model_idx++)
        {
            //Joint(N, t) = Parent(N, t) * Joint(N, t)
            u32 parent_index = animation_data->joints[model_idx].parent_idx;
            model_matrix[model_idx] = glms_mat4_mul(model_matrix[parent_index], local_matrix[model_idx]);
        }

        for (u32 final_idx = 0; final_idx < madness_anim->joint_count; final_idx++)
        {
            //Joint(N, t) = Joint(N, t) * InverseBindMatrix(N)
            //TODO: we have two copies of the data technically
            madness_anim->gpu_matrix[final_idx] = glms_mat4_mul(model_matrix[final_idx],
                                                                animation_data->inverse_bind_matrix[final_idx]
            );


            array_push(animation_system->skinned_matrix_array, &madness_anim->gpu_matrix[final_idx]);
        }
    }


    PROFILE_ZONE_END(animation_system_update)

}


Madness_Animation* animation_system_get_animation_data(Animation_System* animation_system,
                                                       Animation_Handle* animation_handle)
{
    return &animation_system->animation_data[animation_handle->handle];
}
