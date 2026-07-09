#include "mesh_system.h"

#include "cgltf.h"
#include "c_string.h"
#include "material_system.h"
#include "resource_types.h"
#include "ufbx.h"

Animation_Path_Type Animation_Path_Type_gltf_to_engine[cgltf_animation_path_type_max_enum + 1] =
{
    [cgltf_animation_path_type_invalid] = Animation_Path_Type_Invalid,
    [cgltf_animation_path_type_translation] = Animation_Path_Type_Translation,
    [cgltf_animation_path_type_rotation] = Animation_Path_Type_Rotation,
    [cgltf_animation_path_type_scale] = Animation_Path_Type_Scale,
    [cgltf_animation_path_type_weights] = Animation_Path_Type_Weights,
    [cgltf_animation_path_type_max_enum] = Animation_Path_Type_Max
};


Animation_Interpolation_Type Animation_Interpolation_Type_gltf_to_engine[cgltf_interpolation_type_max_enum + 1] =
{
    [cgltf_interpolation_type_linear] = Animation_Interpolation_Type_Linear,
    [cgltf_interpolation_type_step] = Animation_Interpolation_Type_Step,
    [cgltf_interpolation_type_cubic_spline] = Animation_Interpolation_Type_Cubic_Spline,
    [cgltf_interpolation_type_max_enum] = Animation_Interpolation_Type_Max_enum,

};


Mesh_System* mesh_system_init(Memory_System* memory_system)
{
    Mesh_System* out_mesh_system = memory_system_alloc(memory_system, sizeof(Mesh_System), MEMORY_SUBSYSTEM_MESH);
    memset(out_mesh_system, 0, sizeof(Mesh_System));


    out_mesh_system->vertex_byte_size = 0;
    out_mesh_system->index_byte_size = 0;
    out_mesh_system->normals_byte_size = 0;
    out_mesh_system->tangent_byte_size = 0;
    out_mesh_system->uv_byte_size = 0;

    out_mesh_system->mesh_ring_queue = ring_queue_create(sizeof(Mesh_Upload_Data), MAX_MESH_COUNT);
    out_mesh_system->skinned_mesh_ring_queue = ring_queue_create(sizeof(Sk_Mesh_Upload_Data),
                                                                 MAX_SKINNED_MESH_COUNT);
    out_mesh_system->mesh_data_count = 0;


    INFO("MESH SYSTEM CREATED")

    return out_mesh_system;
}

bool mesh_system_shutdown(Mesh_System* mesh_system, Memory_System* memory_system)
{
    MASSERT(mesh_system);
    memory_system_memory_free(memory_system, mesh_system, MEMORY_SUBSYSTEM_MESH);

    mesh_system = NULL;

    return true;
}

bool skinned_mesh_instance_fill_out(Mesh_System* mesh_system, Sk_Mesh_Instance* skinned_mesh_inst,
                                    const Animation_Handle animation_handle,
                                    Heap_Allocator* allocator)
{
    skinned_mesh_inst->anim_handle = animation_handle;

    Animation_Data* animation_data = &mesh_system->animation_data[animation_handle.handle];

    u32 j_count = animation_data->joint_count;
    skinned_mesh_inst->joint_count = j_count;


    skinned_mesh_inst->local_translation = allocator_heap_alloc(allocator, sizeof(vec3) * j_count);
    skinned_mesh_inst->local_rotation = allocator_heap_alloc(allocator, sizeof(vec3) * j_count);
    skinned_mesh_inst->local_scale = allocator_heap_alloc(allocator, sizeof(vec3) * j_count);
    skinned_mesh_inst->gpu_matrix = allocator_heap_alloc(allocator, sizeof(mat4) * j_count);

    for (u32 i = 0; i < j_count; i++)
    {
        mat4_decompose(*animation_data->resting_pose_local_matrix, &skinned_mesh_inst->local_translation[i],
                       &skinned_mesh_inst->local_rotation[i], &skinned_mesh_inst->local_scale[i]);
    }

    skinned_mesh_inst->current_animation_index = 0;
    skinned_mesh_inst->current_time = 0;
    skinned_mesh_inst->looping = true;

    return true;
}

Sk_Mesh_Instance* skinned_mesh_instance_init(Mesh_System* mesh_system, const Animation_Handle animation_handle,
                                                  Heap_Allocator* allocator)
{
    Sk_Mesh_Instance* skinned_mesh_inst = allocator_heap_alloc(allocator, sizeof(Sk_Mesh_Instance));
    skinned_mesh_instance_fill_out(mesh_system, skinned_mesh_inst, animation_handle, allocator);
    return skinned_mesh_inst;
}


void _gltf_load_mesh_data(Resource_System* resource_system, const char* gltf_path, cgltf_data* data,
                          u32 gltf_data_mesh_idx, Mesh_Data* mesh_draw_data,
                          Mesh_Upload_Data* upload_data)
{
    Heap_Allocator* allocator = resource_system->heap_allocator;
    Frame_Allocator* frame_allocator = resource_system->frame_allocator;
    Mesh_System* mesh_system = resource_system->mesh_system;


    /* Find position accessor */
    const cgltf_accessor* pos_accessor = cgltf_find_accessor(data->meshes[gltf_data_mesh_idx].primitives,
                                                             cgltf_attribute_type_position,
                                                             0);
    MASSERT(pos_accessor);
    if (pos_accessor)
    {
        //get size information

        cgltf_size num_floats = cgltf_accessor_unpack_floats(pos_accessor, NULL, 0);
        cgltf_size float_bytes = num_floats * sizeof(float);
        upload_data->vertex_bytes = float_bytes;


        //alloc and copy data
        float* pos_data = allocator_alloc(frame_allocator, float_bytes);
        upload_data->pos = allocator_heap_alloc(allocator, float_bytes);
        cgltf_accessor_unpack_floats(pos_accessor, pos_data, num_floats);
        memcpy(upload_data->pos, pos_data, float_bytes);
    }

    // Find normal accessor
    const cgltf_accessor* norm_accessor = cgltf_find_accessor(data->meshes[gltf_data_mesh_idx].primitives,
                                                              cgltf_attribute_type_normal,
                                                              0);
    if (norm_accessor)
    {
        //get size information
        cgltf_size norm_floats = cgltf_accessor_unpack_floats(norm_accessor, NULL, 0);
        cgltf_size norm_bytes = norm_floats * sizeof(float);
        upload_data->normal_bytes = norm_bytes;

        //alloc and copy data
        float* normal_data = allocator_heap_alloc(allocator, norm_bytes);
        upload_data->normal = allocator_heap_alloc(allocator, norm_bytes);
        cgltf_accessor_unpack_floats(norm_accessor, normal_data, norm_floats);
        memcpy(upload_data->normal, normal_data, norm_bytes);
    }

    //  Find tangent accessor
    const cgltf_accessor* tangent_accessor = cgltf_find_accessor(data->meshes[gltf_data_mesh_idx].primitives,
                                                                 cgltf_attribute_type_tangent, 0);

    if (tangent_accessor)
    {
        //get size information
        cgltf_size tangent_floats = cgltf_accessor_unpack_floats(tangent_accessor, NULL, 0);
        cgltf_size tangent_bytes = tangent_floats * sizeof(float);
        upload_data->tangent_bytes = tangent_bytes;


        //alloc and copy data
        float* tangent_data = allocator_heap_alloc(allocator, tangent_bytes);
        upload_data->tangent = allocator_heap_alloc(allocator, tangent_bytes);
        cgltf_accessor_unpack_floats(tangent_accessor, tangent_data, tangent_floats);
        memcpy(upload_data->tangent, tangent_data, tangent_bytes);
    }

    //  Find texcoord accessor
    const cgltf_accessor* texcoord_accessor = cgltf_find_accessor(data->meshes[gltf_data_mesh_idx].primitives,
                                                                  cgltf_attribute_type_texcoord, 0);
    if (texcoord_accessor)
    {
        //get size information
        cgltf_size uv_floats_count = cgltf_accessor_unpack_floats(texcoord_accessor, NULL, 0);
        cgltf_size uv_byte_size = uv_floats_count * sizeof(float);
        upload_data->uv_bytes = uv_byte_size;


        //alloc and copy data
        float* uv_data = allocator_alloc(frame_allocator, uv_byte_size);
        cgltf_accessor_unpack_floats(texcoord_accessor, uv_data, uv_floats_count);

        upload_data->uv = allocator_heap_alloc(allocator, uv_byte_size);
        memcpy(upload_data->uv, uv_data, uv_byte_size);
    }

    // Load indices
    // SEE componentType in the specs for more detail 3.6.2
    u8 index_stride = data->meshes[gltf_data_mesh_idx].primitives[0].indices->stride;
    if (index_stride == 2)
    {
        upload_data->index_type = VK_INDEX_TYPE_UINT16;
    }
    else if (index_stride == 4)
    {
        upload_data->index_type = VK_INDEX_TYPE_UINT32;
    }
    else
    {
        WARN("GLTF MESH LOADING: UNKNOWN INDEX TYPE STRIDE");
    }

    //TODO: there can be multiple primitices/indices, will come back to
    upload_data->indices_bytes = data->meshes[gltf_data_mesh_idx].primitives->indices->count *
        index_stride;
    upload_data->indices = allocator_heap_alloc(allocator,
                                                upload_data->indices_bytes);

    const uint8_t* index_buffer_data = cgltf_buffer_view_data(
        data->meshes[gltf_data_mesh_idx].primitives->indices->buffer_view);
    memcpy(upload_data->indices, index_buffer_data,
           upload_data->indices_bytes);

    //Mesh Data
    mesh_draw_data->index_count = data->meshes[gltf_data_mesh_idx].primitives->indices->count;
    mesh_draw_data->index_offset = mesh_system->index_count_size;
    mesh_draw_data->vertex_offset = mesh_system->vertex_count_size;
    mesh_draw_data->uv_offset = mesh_system->uv_byte_size;
    mesh_draw_data->normal_offset = mesh_system->normals_byte_size;
    mesh_draw_data->tangent_offset = mesh_system->tangent_byte_size;

    //update offsets
    mesh_system->vertex_byte_size += upload_data->vertex_bytes;
    mesh_system->vertex_count_size += upload_data->vertex_bytes / sizeof(vec3);
    mesh_system->index_byte_size += upload_data->indices_bytes;
    mesh_system->index_count_size += mesh_draw_data->index_count;
    mesh_system->normals_byte_size += upload_data->normal_bytes;
    mesh_system->tangent_byte_size += upload_data->tangent_bytes;
    mesh_system->uv_byte_size += upload_data->uv_bytes;

    //LOAD TEXTURES/MATERIALS
    // GET BASE PATH
    char* base_path = c_string_path_strip(gltf_path, frame_allocator);

    //every mesh just gets loaded in with a default pbr, well convert the material later into a custom format
    Material_Default* default_material = material_system_create_default_pbr(resource_system->material_system,
                                                                         &mesh_draw_data->material_handle);

    if (!data->meshes[gltf_data_mesh_idx].primitives->material)
    {
        WARN("NO MATERIAL DATA FOUND FOR GLTF MESH");
        return;
    }

    //COLOR TEXTURE
    cgltf_texture* color_texture = data->meshes[gltf_data_mesh_idx].primitives->material->pbr_metallic_roughness.
                                                                    base_color_texture.texture;
    if (color_texture && color_texture->image->uri)
    {
        default_material->flags |= MESH_PIPELINE_COLOR;

        char* texture_path = allocator_alloc(frame_allocator,
                                             strlen(base_path) + strlen(color_texture->image->uri));
        // takes a buffer, message format, then the remaining strings
        sprintf(texture_path, "%s%s", base_path, color_texture->image->uri);
        TRACE("COLOR Texture Path:  %s", texture_path);
        Texture_Handle color_handle = {0};
        texture_system_load_texture(resource_system->texture_system, texture_path, &color_handle);
        default_material->color_index = color_handle.handle;
        memcpy(default_material->color.elements,
               data->meshes[gltf_data_mesh_idx].primitives->material->pbr_metallic_roughness.base_color_factor,
               sizeof(vec4));
    }
    else
    {
        // default_material->flags |= MESH_PIPELINE_COLOR;
        TRACE("No Color Texture using fall back color");
        default_material->color_index = resource_system->texture_system->default_texture_handle.handle;
        memcpy(default_material->color.elements,
               data->meshes[gltf_data_mesh_idx].primitives->material->pbr_metallic_roughness.base_color_factor,
               sizeof(vec4));
    }

    //METAL-ROUGHNESS
    cgltf_texture* metal_roughness_texture = data->meshes[gltf_data_mesh_idx].
                                             primitives->material->pbr_metallic_roughness.
                                             metallic_roughness_texture.texture;
    if (metal_roughness_texture)
    {
        if (metal_roughness_texture->image->uri)
        {
            default_material->flags |= MESH_PIPELINE_ROUGHNESS;
            default_material->flags |= MESH_PIPELINE_METALLIC;
            char* texture_path = allocator_alloc(frame_allocator,
                                                 strlen(base_path) +
                                                 strlen(metal_roughness_texture->image->uri));
            // takes a buffer, message format, then the remaining strings
            sprintf(texture_path, "%s%s", base_path, metal_roughness_texture->image->uri);
            TRACE("METAL/ROUGHNESS Texture Path:  %s", texture_path);

            Texture_Handle metallic_handle;

            texture_system_load_texture(resource_system->texture_system, texture_path,
                                        &metallic_handle);

            default_material->metallic_index = metallic_handle.handle;
            default_material->roughness_index = metallic_handle.handle;
        }
        if (metal_roughness_texture->image->buffer_view)
        {
            //TODO: load texture data from the shader system
            INFO("WHOA A BUFFER VIEW ");
            const u8* metal_roughness_texture_image_data = cgltf_buffer_view_data(
                metal_roughness_texture->image->buffer_view);
            INFO("WHOA A BUFFER VIEW ");
        }
    }

    // AO
    //NOTE: this material in theory can be included in the pbr-metal-roughness texture, in which case, just return a handle
    cgltf_texture* AO_texture = data->meshes[gltf_data_mesh_idx].primitives->material->occlusion_texture.texture;
    if (AO_texture && AO_texture->image->uri)
    {
        default_material->flags |= MESH_PIPELINE_AO;
        char* texture_path = allocator_alloc(frame_allocator,
                                             strlen(base_path) + strlen(AO_texture->image->uri));
        // takes a buffer, message format, then the remaining strings
        sprintf(texture_path, "%s%s", base_path, AO_texture->image->uri);
        TRACE("AO Texture Path:  %s", texture_path);

        Texture_Handle ao_handle;

        texture_system_load_texture(resource_system->texture_system, texture_path,
                                    &ao_handle);
        default_material->ambient_occlusion_index = ao_handle.
            handle;
    }


    //NORMAL TEXTURE
    // data->meshes[mesh_idx].primitives->material->has_pbr_metallic_roughness
    cgltf_texture* normal_texture = data->meshes[gltf_data_mesh_idx].primitives->material->normal_texture.texture;
    if (normal_texture && normal_texture->image->uri)
    {
        default_material->flags |= MESH_PIPELINE_NORMAL;
        char* texture_path = allocator_alloc(frame_allocator,
                                             strlen(base_path) + strlen(normal_texture->image->uri));
        // takes a buffer, message format, then the remaining strings
        sprintf(texture_path, "%s%s", base_path, normal_texture->image->uri);
        TRACE("NORMAL Texture Path:  %s", texture_path);
        Texture_Handle normal_handle;

        texture_system_load_texture(resource_system->texture_system, texture_path,
                                    &normal_handle);
        default_material->normal_index = normal_handle.
            handle;
    }

    //EMISSIVE TEXTURE
    cgltf_texture* emissive_texture = data->meshes[gltf_data_mesh_idx].primitives->material->emissive_texture.
                                                                       texture;
    if (emissive_texture && emissive_texture->image->uri)
    {
        default_material->flags |= MESH_PIPELINE_EMISSIVE;
        char* texture_path = allocator_alloc(frame_allocator,
                                             strlen(base_path) + strlen(emissive_texture->image->uri));
        // takes a buffer, message format, then the remaining strings
        sprintf(texture_path, "%s%s", base_path, emissive_texture->image->uri);
        TRACE("EMISSIVE Texture Path:  %s", texture_path);

        Texture_Handle emissive_handle;
        texture_system_load_texture(resource_system->texture_system, texture_path,
                                    &emissive_handle);
        default_material->emissive_index = emissive_handle.
            handle;
    }
}

void _gltf_load_skinned_mesh_data(Resource_System* resource_system, cgltf_data* data,
                                  u32 mesh_idx,
                                  Sk_Mesh_Data* skinned_mesh_data,
                                  Sk_Mesh_Upload_Data* skinned_mesh_upload_data)
{
    Heap_Allocator* allocator = resource_system->heap_allocator;
    Frame_Allocator* frame_allocator = resource_system->frame_allocator;
    Mesh_System* mesh_system = resource_system->mesh_system;


    const cgltf_accessor* joint_accessor = cgltf_find_accessor(data->meshes[mesh_idx].primitives,
                                                               cgltf_attribute_type_joints,
                                                               0);
    if (joint_accessor)
    {
        //get size information
        cgltf_size num_floats = cgltf_accessor_unpack_floats(joint_accessor, NULL, 0);
        cgltf_size float_bytes = num_floats * sizeof(float);

        skinned_mesh_data->joint_bytes = float_bytes;
        skinned_mesh_upload_data->joint_bytes = float_bytes;

        //alloc and copy data
        float* joint_data = allocator_alloc(frame_allocator, float_bytes);
        skinned_mesh_upload_data->joints = allocator_heap_alloc(allocator, float_bytes);
        cgltf_accessor_unpack_floats(joint_accessor, joint_data, num_floats);
        memcpy(skinned_mesh_upload_data->joints, joint_data, float_bytes);
    }

    const cgltf_accessor* weight_accessor = cgltf_find_accessor(data->meshes[mesh_idx].primitives,
                                                                cgltf_attribute_type_weights,
                                                                0);
    if (weight_accessor)
    {
        //get size information

        cgltf_size num_floats = cgltf_accessor_unpack_floats(weight_accessor, NULL, 0);
        cgltf_size float_bytes = num_floats * sizeof(float);

        skinned_mesh_upload_data->weight_bytes = float_bytes;
        skinned_mesh_data->weight_bytes = float_bytes;


        //alloc and copy data
        float* weight_data = allocator_alloc(frame_allocator, float_bytes);
        skinned_mesh_upload_data->weights = allocator_heap_alloc(allocator, float_bytes);
        cgltf_accessor_unpack_floats(weight_accessor, weight_data, num_floats);
        memcpy(skinned_mesh_upload_data->weights, weight_data, float_bytes);
    }

    skinned_mesh_data->joint_offset_bytes = mesh_system->joints_byte_size;
    skinned_mesh_data->joint_offset_vec4 = mesh_system->joints_byte_size / sizeof(vec4);
    skinned_mesh_data->weight_offset_bytes = mesh_system->weight_byte_size;
    skinned_mesh_data->weight_offset_vec4 = mesh_system->weight_byte_size / sizeof(vec4);

    skinned_mesh_upload_data->joint_offset = mesh_system->joints_byte_size;
    skinned_mesh_upload_data->weight_offset = mesh_system->weight_byte_size;

    mesh_system->weight_byte_size += skinned_mesh_data->weight_bytes;
    mesh_system->joints_byte_size += skinned_mesh_data->joint_bytes;
}


void _gltf_load_skin_and_animation_data(Resource_System* resource_system, cgltf_data* data,
                                        Sk_Mesh_Asset* skinned_mesh_meta_data)
{
    Heap_Allocator* allocator = resource_system->heap_allocator;
    Frame_Allocator* frame_allocator = resource_system->frame_allocator;
    Mesh_System* mesh_system = resource_system->mesh_system;


    hash_table* joint_name_to_index = HASH_TABLE_CREATE(size_t, 200);

    //TODO: this is an unhandled case, and honestly why would a mesh have more than 1 skin???
    MASSERT(data->skins_count <= 1);

    skinned_mesh_meta_data->anim_handle = (Animation_Handle){.handle = mesh_system->animation_data_count, .gen = 0};

    Animation_Data* animation_data = &mesh_system->animation_data[mesh_system->animation_data_count++];
    animation_data->animations = allocator_heap_alloc(allocator, sizeof(Animation) * data->animations_count);
    animation_data->animations_count = data->animations_count;


    for (size_t skin_idx = 0; skin_idx < data->skins_count; skin_idx++)
    {
        cgltf_skin* skin_data = &data->skins[skin_idx];
        animation_data->joint_count = skin_data->joints_count;
        animation_data->joints = allocator_heap_alloc(allocator, skin_data->joints_count * sizeof(Joint));
        animation_data->resting_pose_local_matrix = allocator_heap_alloc(
            allocator, skin_data->joints_count * sizeof(mat4));

        // Inverse bind matrices — one 4x4 float matrix per joint
        if (skin_data->inverse_bind_matrices)
        {
            //get size information
            cgltf_size ibm_floats = cgltf_accessor_unpack_floats(skin_data->inverse_bind_matrices, NULL, 0);
            cgltf_size ibm_bytes = ibm_floats * sizeof(float);

            //alloc and copy data
            float* ibm_date = allocator_alloc(frame_allocator, ibm_bytes);
            animation_data->inverse_bind_matrix = allocator_heap_alloc(allocator, ibm_bytes);

            cgltf_accessor_unpack_floats(skin_data->inverse_bind_matrices, ibm_date, ibm_floats);
            memcpy(animation_data->inverse_bind_matrix, ibm_date, ibm_bytes);
        }
        else
        {
            // glTF spec: absent inverse_bind_matrices means identity per joint
            animation_data->inverse_bind_matrix = allocator_heap_alloc(
                allocator, sizeof(mat4) * skin_data->joints_count);

            for (cgltf_size j = 0; j < skin_data->joints_count; j++)
            {
                animation_data->inverse_bind_matrix[j] = mat4_identity();
            }
        }

        // cur_joint->inverse_bind_matrix = data->skins[skin_idx].inverse_bind_matrices[joint_idx].;
        for (size_t joint_idx = 0; joint_idx < skin_data->joints_count; joint_idx++)
        {
            Joint* cur_joint = &animation_data->joints[joint_idx];
            cgltf_node* cgltf_joint = skin_data->joints[joint_idx];

            cur_joint->joint_name = c_string_duplicate_heap_alloc(cgltf_joint->name, allocator);
            cur_joint->id = joint_idx;

            hash_table_insert(joint_name_to_index, cur_joint->joint_name, &joint_idx);
        }
        //this pass is to get the parent id's and parent nodes
        for (size_t joint_idx = 0; joint_idx < data->skins[skin_idx].joints_count; joint_idx++)
        {
            Joint* cur_joint = &animation_data->joints[joint_idx];
            cgltf_node* cgltf_joint = data->skins[skin_idx].joints[joint_idx];

            mat4* local_mat = &animation_data->resting_pose_local_matrix[joint_idx];
            cgltf_node_transform_local(cgltf_joint, local_mat->data); // 16 floats, column-major

            size_t parent_idx;
            if (hash_table_get(joint_name_to_index, cgltf_joint->parent->name, &parent_idx))
            {
                cur_joint->parent_idx = parent_idx;
            }
        }

        //sampler inputs - keyframes
        //sampler output - keyframe values
        // sampler interpolation

        for (size_t animation_idx = 0; animation_idx < data->animations_count; animation_idx++)
        {
            Animation* cur_animation = &animation_data->animations[animation_idx];
            cgltf_animation* anim_data = &data->animations[animation_idx];

            if (anim_data->name)
            {
                cur_animation->animation_name = STRING_CREATE_FROM_BUFFER_HEAP_ALLOCATOR(anim_data->name, allocator);
            }
            else
            {
                cur_animation->animation_name = STRING_CREATE_FROM_BUFFER_ALLOCATOR(
                    data->meshes[0].name, frame_allocator);
                cur_animation->animation_name = string_concat_heap(cur_animation->animation_name,
                                                                   string_from_int_heap_allocator(
                                                                       animation_idx, allocator),
                                                                   allocator);
            }
            cgltf_size channel_count = anim_data->channels_count;
            cgltf_size sampler_count = anim_data->samplers_count;
            cur_animation->channel_count = channel_count;
            cur_animation->sampler_count = sampler_count;

            cur_animation->channels = allocator_heap_alloc(allocator, sizeof(Animation_Channel) * channel_count);
            cur_animation->samplers = allocator_heap_alloc(allocator, sizeof(Animation_Sampler) * sampler_count);


            for (size_t channel_idx = 0; channel_idx < channel_count; channel_idx++)
            {
                Animation_Channel* cur_channel = &cur_animation->channels[channel_idx];
                cgltf_animation_channel* anim_channel = &anim_data->channels[channel_idx];

                cur_channel->animation_path_type = Animation_Path_Type_gltf_to_engine[anim_channel->target_path];
                cur_channel->sampler_idx = cgltf_animation_sampler_index(anim_data, anim_channel->sampler);
                // TODO: TEST

                //get a reference to find where the target joint is in the array
                size_t joint_idx;
                if (hash_table_get(joint_name_to_index, anim_channel->target_node->name, &joint_idx))
                {
                    cur_channel->joint_index = joint_idx;
                }
            }


            for (size_t sampler_idx = 0; sampler_idx < sampler_count; sampler_idx++)
            {
                Animation_Sampler* cur_sampler = &cur_animation->samplers[sampler_idx];
                cgltf_animation_sampler* anim_sampler = &anim_data->samplers[sampler_idx];


                //read gltf input data
                cur_sampler->timestamps_count = anim_sampler->input->count;
                cur_sampler->timestamps =
                    allocator_heap_alloc(allocator, sizeof(float) * cur_sampler->timestamps_count);
                //get the timestamp data from the view_data and copy it into the timestamps
                //it has to be memcpied, since the view_data returns the buffer pointer
                const uint8_t* timestamp_buffer_data = cgltf_buffer_view_data(anim_sampler->input->buffer_view);
                memcpy(cur_sampler->timestamps, timestamp_buffer_data, cur_sampler->timestamps_count * sizeof(float));

                for (u32 timestamp_idx = 0; timestamp_idx < cur_sampler->timestamps_count; timestamp_idx++)
                {
                    cur_sampler->sampler_start = min_f(cur_sampler->sampler_start,
                                                       cur_sampler->timestamps[timestamp_idx]);
                    cur_sampler->sampler_end = max_f(cur_sampler->sampler_start,
                                                     cur_sampler->timestamps[timestamp_idx]);
                }

                cur_animation->anim_start = min_f(cur_animation->anim_start, cur_sampler->sampler_start);
                cur_animation->anim_end = max_f(cur_animation->anim_end, cur_sampler->sampler_end);

                //read gltf output data
                cur_sampler->trs_interpolation_count = anim_sampler->output->count;
                const uint8_t* trs_buffer_data = cgltf_buffer_view_data(anim_sampler->output->buffer_view);

                cur_sampler->interpolation_type = Animation_Interpolation_Type_gltf_to_engine[anim_sampler->
                    interpolation];
                switch (anim_sampler->output->type)
                {
                //these are the only supported formats in the gltf spec for samplers
                case cgltf_type_scalar: // weights
                    cur_sampler->interperlation_data.trs_float = allocator_heap_alloc(
                        allocator, sizeof(float) * cur_sampler->trs_interpolation_count);
                    memcpy(cur_sampler->interperlation_data.trs_float, trs_buffer_data,
                           cur_sampler->trs_interpolation_count * sizeof(float));
                    break;
                case cgltf_type_vec3: // translation, scale
                    cur_sampler->interperlation_data.trs_vec3 = allocator_heap_alloc(
                        allocator, sizeof(vec3) * cur_sampler->trs_interpolation_count);
                    memcpy(cur_sampler->interperlation_data.trs_vec3, trs_buffer_data,
                           cur_sampler->trs_interpolation_count * sizeof(float));
                    break;
                case cgltf_type_vec4: // rotation
                    cur_sampler->interperlation_data.trs_vec4 = allocator_heap_alloc(
                        allocator, sizeof(vec4) * cur_sampler->trs_interpolation_count);
                    memcpy(cur_sampler->interperlation_data.trs_vec4, trs_buffer_data,
                           cur_sampler->trs_interpolation_count * sizeof(float));
                    break;
                default:
                    FATAL("UNSUPPORTED CGLTF SAMPLER TYPE");
                    break;
                }
            }
        }

        hash_table_destroy(joint_name_to_index);
    }
}

void mesh_load_fbx(Mesh_System* mesh_system, const char* fbx_path, Allocator* arena, Frame_Allocator* frame_arena)
{
    UNIMPLEMENTED();
    /*if (!c_string_path_is_extension(fbx_path, ".fbx"))
    {
        FATAL("DID NOT PASS IN A FBX FILE");
        return;
    }

    // https://github.com/ufbx/ufbx?tab=readme-ov-file - github
    // https://ufbx.github.io/ - online docs

    //NOTE: uses malloc under the hood which we can override

    ufbx_load_opts opts = {0}; // Optional, pass NULL for defaults
    ufbx_error error; // Optional, pass NULL if you don't care about errors
    ufbx_scene* scene = ufbx_load_file(fbx_path, &opts, &error);
    if (!scene)
    {
        fprintf(stderr, "Failed to load: %s\n", error.description.data);
        MASSERT(scene);
    }


    static_mesh* out_static_mesh = static_mesh_init(arena, scene->meshes.count);


    // Use and inspect `scene`, it's just plain data!
    // Let's just list all objects within the scene for example:
    for (size_t i = 0; i < scene->nodes.count; i++)
    {
        ufbx_node* node = scene->nodes.data[i];
        if (node->is_root) continue;

        printf("Object: %s\n", node->name.data);
        if (node->mesh)
        {
            printf("-> mesh with %zu faces\n", node->mesh->faces.count);
        }
    }


    for (size_t mesh_idx = 0; mesh_idx < scene->meshes.count; mesh_idx++)
    {
        ufbx_mesh* mesh = scene->meshes.data[mesh_idx];
        for (size_t i = 0; i < mesh->faces.count; i++)
        {
            ufbx_face face = mesh->faces.data[i];

            // Loop through the corners of the polygon.
            for (uint32_t corner = 0; corner < face.num_indices; corner++)
            {
                // Faces are defined by consecutive indices, one for each corner.
                uint32_t index = face.index_begin + corner;

                // Retrieve the position, normal and uv for the vertex.
                ufbx_vec3 position = ufbx_get_vertex_vec3(&mesh->vertex_position, index);
                ufbx_vec3 normal = ufbx_get_vertex_vec3(&mesh->vertex_normal, index);
                ufbx_vec2 uv = ufbx_get_vertex_vec2(&mesh->vertex_uv, index);
            }
        }
    }


    // GET BASE PATH
    char* base_path = c_string_path_strip(fbx_path, frame_arena);


    ufbx_free_scene(scene);*/
}


void mesh_load_obj(const char* obj_path, Renderer* renderer)
{
    UNIMPLEMENTED();
    /*
    //NOTE: material data is stored in a seperate .mtl file
    //NOTE: this file format does not support indicis and technically also supports quads, just why, that's awful, so yeah abandoning it

    // https://en.wikipedia.org/wiki/Wavefront_.obj_file
    // # comments
    // v - vertex
    // vt - texture coordinate
    // vn - Vertex Normal
    // vp - parameter space vertices
    // f - indicis/ polygonal face element
    // l - line elements
    // o - object name
    // s - smooth shading

    //TODO:
    // FILE* mesh_mtl = fopen("../z_assets/models/falcon.mtl", "rb");


    submesh* out_mesh = submesh_init(&renderer->arena);

    size_t file_size;
    FILE* mesh_obj_file = fopen("../z_assets/models/car_obj/falcon.obj", "rb");
    if (!mesh_obj_file)
    {
        FATAL("Failed to open obj file");
        return;
    }

    // go to the end of the file and get the size
    fseek(mesh_obj_file, 0, SEEK_END);
    file_size = ftell(mesh_obj_file);
    rewind(mesh_obj_file); // go back to the start of the file

    char buffer[512];

    //this function is a godsend
    //sscanf: This function reads formatted input from a specified character string (buffer). It allows you to parse data that is already present in a string variable.

    while (fgets(buffer, sizeof(buffer), mesh_obj_file))
    {
        if (buffer[0] == '#') { continue; }
        if (strncmp(buffer, "v ", 2) == 0)
        {
            vec3 v;
            sscanf(buffer, "v %f %f %f", &v.x, &v.y, &v.z);
            DEBUG("v %f %f %f", v.x, v.y, v.z);
            darray_push(out_mesh->pos, v);
        }
        if (strncmp(buffer, "vt ", 3) == 0)
        {
            vec2 v;
            sscanf(buffer, "v %f %f", &v.x, &v.y);
            DEBUG("v %f %f %f", v.x, v.y);
            darray_push(out_mesh->uv, v);
        }
        if (strncmp(buffer, "vn ", 3) == 0)
        {
            vec3 v;
            sscanf(buffer, "v %f %f %f", &v.x, &v.y, &v.z);
            DEBUG("v %f %f %f", v.x, v.y, v.z);
            darray_push(out_mesh->normal, v);
        }
    }

    fclose(mesh_obj_file);
    */
}


bool mesh_system_generate_render_packet(Mesh_System* mesh_system, Render_Packet_Mesh* out_mesh_packet)
{
    out_mesh_packet->draw_data = mesh_system->mesh_data;
    out_mesh_packet->draw_data_size = mesh_system->mesh_data_count;

    out_mesh_packet->skinned_draw_data = mesh_system->skinned_mesh_data;
    out_mesh_packet->skinned_draw_data_size = mesh_system->skinned_mesh_data_count;


    return true;
}


void mesh_load_gltf(Resource_System* resource_system, const char* gltf_path)
{
    Mesh_System* mesh_system = resource_system->mesh_system;
    Heap_Allocator* allocator = resource_system->heap_allocator;
    Frame_Allocator* frame_allocator = resource_system->frame_allocator;

    if (!c_string_path_is_extension(gltf_path, ".gltf") && !c_string_path_is_extension(gltf_path, ".glb"))
    {
        FATAL("DID NOT PASS IN A GLTF FILE");
        return;
    }


    cgltf_options options = {0};
    cgltf_data* data = NULL;
    cgltf_result result = cgltf_parse_file(&options, gltf_path, &data);

    if (result != cgltf_result_success)
    {
        fprintf(stderr, "Failed to parse glTF file: %s\n", gltf_path);
        return;
    }
    result = cgltf_load_buffers(&options, data, gltf_path);
    MASSERT(result == cgltf_result_success)


    if (data->skins_count > 0)
    {
        Sk_Mesh_Asset* skinned_mesh_meta_data = &mesh_system->skinned_mesh_meta_data[mesh_system->
            skinned_mesh_meta_data_count++];
        skinned_mesh_meta_data->file_path = c_string_duplicate_heap_alloc(gltf_path, allocator);

        skinned_mesh_meta_data->mesh_index = allocator_heap_alloc(
            allocator, sizeof(u32) * data->meshes_count);
        skinned_mesh_meta_data->mesh_count = data->meshes_count;

        skinned_mesh_meta_data->skinned_mesh_indexs = allocator_heap_alloc(
            allocator, sizeof(u32) * data->meshes_count);
        skinned_mesh_meta_data->skinned_mesh_count = data->meshes_count;


        Mesh_Upload_Data* mesh_upload_data_array = allocator_alloc(frame_allocator,
                                                                   sizeof(Mesh_Upload_Data) * data->meshes_count);

        Sk_Mesh_Upload_Data* skinned_mesh_upload_data_array = allocator_heap_alloc(
            allocator, sizeof(Sk_Mesh_Upload_Data) * data->meshes_count);


        for (size_t mesh_idx = 0; mesh_idx < data->meshes_count; mesh_idx++)
        {
            skinned_mesh_meta_data->skinned_mesh_indexs[mesh_idx] = mesh_system->skinned_mesh_data_count;
            Sk_Mesh_Data* mesh_draw_data = &mesh_system->skinned_mesh_data[mesh_system->skinned_mesh_data_count];
            mesh_system->skinned_mesh_data_count++;

            mesh_draw_data->mesh_data.transform_handle = scene_get_new_mesh_transform(resource_system->scene);
            mesh_draw_data->mesh_data.material_handle = material_system_create_material(
                resource_system->material_system);

            Mesh_Upload_Data* mesh_upload_data = &mesh_upload_data_array[mesh_idx];
            Sk_Mesh_Upload_Data* skinned_mesh_upload_data = &skinned_mesh_upload_data_array[mesh_idx];

            _gltf_load_mesh_data(resource_system, gltf_path, data, mesh_idx, &mesh_draw_data->mesh_data,
                                 mesh_upload_data);

            _gltf_load_skinned_mesh_data(resource_system, data, mesh_idx,
                                         mesh_draw_data, skinned_mesh_upload_data);

            //add to mesh upload queue
            ring_enqueue(mesh_system->mesh_ring_queue, mesh_upload_data);
            ring_enqueue(mesh_system->skinned_mesh_ring_queue, skinned_mesh_upload_data);
        }


        _gltf_load_skin_and_animation_data(resource_system, data, skinned_mesh_meta_data);

        //create the instance
        Sk_Mesh_Instance* new_skinned_mesh_instance = &mesh_system->skinned_mesh_instance[mesh_system->
            skinned_mesh_instance_count++];
        skinned_mesh_instance_fill_out(mesh_system, new_skinned_mesh_instance, skinned_mesh_meta_data->anim_handle,
                                       allocator);
    }
    else
    {
        Mesh_Asset* mesh_meta_data = allocator_heap_alloc(allocator, sizeof(Mesh_Asset));
        mesh_meta_data->file_path = c_string_duplicate_heap_alloc(gltf_path, allocator);
        mesh_meta_data->mesh_index = allocator_heap_alloc(allocator, sizeof(u32) * data->meshes_count);
        mesh_meta_data->mesh_count = data->meshes_count;


        Mesh_Upload_Data* mesh_upload_data_array = allocator_alloc(frame_allocator,
                                                                   sizeof(Mesh_Upload_Data) * data->meshes_count);

        u32* mesh_index_array = mesh_meta_data->mesh_index;

        for (size_t mesh_idx = 0; mesh_idx < data->meshes_count; mesh_idx++)
        {
            mesh_index_array[mesh_idx] = mesh_system->mesh_data_count;
            Mesh_Data* mesh_draw_data = &mesh_system->mesh_data[mesh_system->mesh_data_count];
            mesh_system->mesh_data_count++;

            mesh_draw_data->transform_handle = scene_get_new_mesh_transform(resource_system->scene);
            mesh_draw_data->material_handle = material_system_create_material(resource_system->material_system);


            Mesh_Upload_Data* current_mesh_upload_data = &mesh_upload_data_array[mesh_idx];

            _gltf_load_mesh_data(resource_system,
                                 gltf_path, data, mesh_idx,
                                 mesh_draw_data, current_mesh_upload_data);

            //add to mesh upload queue
            ring_enqueue(mesh_system->mesh_ring_queue, current_mesh_upload_data);
        }
    }


    //load materials
    // if (data->meshes[0].primitives->material) {
    //     int mat_idx = cgltf_material_index(data, data->meshes[0].primitives->material);
    //     int hi = 0;
    // }
    // cgltf_texture_index();

    //TODO: remove
    // mesh_system->static_mesh_array[mesh_system->static_mesh_array_size] = *out_static_mesh;
    // mesh_system->static_mesh_array_size++;


    cgltf_free(data);
}


void animation_update_single_test(Mesh_System* mesh_system, float delta_time, Frame_Allocator* frame_allocator)
{
    //update and interpolate the local transformations for the playing animations
    //create the local matrix
    // joint1 =  local parent * local joint
    // joint2 =  joint1 * inverse_bind_matrix
    //send to the gpu for the shader to work


    //assume the mesh is loaded
    Sk_Mesh_Instance* skinned_mesh_inst = mesh_system->test_skinned_mesh_instance;

    Animation_Data* animation_data = &mesh_system->animation_data[skinned_mesh_inst->anim_handle.handle];
    Animation* anim_data = &animation_data->animations[skinned_mesh_inst->current_animation_index];


    skinned_mesh_inst->current_time += delta_time;

    if (skinned_mesh_inst->current_time > anim_data->anim_end)
    {
        skinned_mesh_inst->current_time -= anim_data->anim_end;
    }

    for (u32 i = 0; i < anim_data->channel_count; i++)
    {
        const Animation_Channel* channel = &anim_data->channels[i];
        const Animation_Sampler* sampler = &anim_data->samplers[channel->sampler_idx];

        u32 j_idx = channel->joint_index;

        for (size_t timestamp_idx = 0; timestamp_idx < sampler->timestamps_count - 1; timestamp_idx++)
        {
            if (sampler->interpolation_type != Animation_Interpolation_Type_Linear)
            {
                DEBUG("This sample only supports linear interpolations");
                continue;
            }

            // Get the input keyframe values for the current time stamp
            if ((skinned_mesh_inst->current_time >= sampler->timestamps[timestamp_idx]) && (skinned_mesh_inst->
                current_time <=
                sampler->timestamps[timestamp_idx + 1]))
            {
                float interp_val = (skinned_mesh_inst->current_time - sampler->timestamps[timestamp_idx]) / (sampler
                    ->
                    timestamps[timestamp_idx + 1] - sampler->timestamps[timestamp_idx]);
                switch (channel->animation_path_type)
                {
                case Animation_Path_Type_Invalid:
                    MASSERT(false);
                    break;
                case Animation_Path_Type_Translation:
                    skinned_mesh_inst->local_translation[j_idx] = vec3_lerp(
                        sampler->interperlation_data.trs_vec3[timestamp_idx],
                        sampler->interperlation_data.trs_vec3[timestamp_idx +
                            1], interp_val);
                    break;
                case Animation_Path_Type_Rotation:
                    quat q1;
                    q1.x = sampler->interperlation_data.trs_vec4[timestamp_idx].x;
                    q1.y = sampler->interperlation_data.trs_vec4[timestamp_idx].y;
                    q1.z = sampler->interperlation_data.trs_vec4[timestamp_idx].z;
                    q1.w = sampler->interperlation_data.trs_vec4[timestamp_idx].w;

                    quat q2;
                    q2.x = sampler->interperlation_data.trs_vec4[timestamp_idx + 1].x;
                    q2.y = sampler->interperlation_data.trs_vec4[timestamp_idx + 1].y;
                    q2.z = sampler->interperlation_data.trs_vec4[timestamp_idx + 1].z;
                    q2.w = sampler->interperlation_data.trs_vec4[timestamp_idx + 1].w;


                    skinned_mesh_inst->local_rotation[j_idx] = quat_normalize(quat_slerp(q1, q2, interp_val));;
                    break;
                case Animation_Path_Type_Scale:
                    skinned_mesh_inst->local_scale[j_idx] = vec3_lerp(
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

    mat4* local_matrix = allocator_alloc(frame_allocator, sizeof(mat4) * skinned_mesh_inst->joint_count);
    mat4* model_matrix = allocator_alloc(frame_allocator, sizeof(mat4) * skinned_mesh_inst->joint_count);

    for (u32 i = 0; i < skinned_mesh_inst->joint_count; i++)
    {
        // Joint(N, t) = Translation(N, t) * Rotation(N, t) * Scale(N, t)

        const mat4 t = mat4_translation(skinned_mesh_inst->local_translation[i]);
        const mat4 r = quat_to_mat4(skinned_mesh_inst->local_rotation[i]);
        const mat4 s = mat4_scale(skinned_mesh_inst->local_scale[i]);

        mat4 tr = mat4_mul(t, r);
        local_matrix[i] = mat4_mul(tr, s);
    }

    model_matrix[0] = local_matrix[0];
    for (u32 i = 0; i < skinned_mesh_inst->joint_count; i++)
    {
        //Joint(N, t) = Parent(N, t) * Joint(N, t)
        u32 parent_index = animation_data->joints[i].parent_idx;
        model_matrix[i] = mat4_mul(local_matrix[parent_index], local_matrix[i]);
    }

    for (u32 i = 0; i < skinned_mesh_inst->joint_count; i++)
    {
        //Joint(N, t) = Joint(N, t) * InverseBindMatrix(N)
        skinned_mesh_inst->gpu_matrix[i] = mat4_mul(local_matrix[i],
                                                    animation_data->inverse_bind_matrix[i]);
    }
}

void animation_update(Mesh_System* mesh_system, float delta_time, Frame_Allocator* frame_allocator)
{
    //update and interpolate the local transformations for the playing animations
    //create the local matrix
    // joint1 =  local parent * local joint
    // joint2 =  joint1 * inverse_bind_matrix
    //send to the gpu for the shader to work


    //update and interpolate the local transformations for the playing animations
    //create the local matrix
    // joint1 =  local parent * local joint
    // joint2 =  joint1 * inverse_bind_matrix
    //send to the gpu for the shader to work


    //assume the mesh is loaded

    for (u32 i = 0; i < mesh_system->skinned_mesh_instance_count; ++i)
    {
        Sk_Mesh_Instance* cur_skinned_mesh_inst = &mesh_system->skinned_mesh_instance[i];

        Animation_Data* animation_data = &mesh_system->animation_data[cur_skinned_mesh_inst->anim_handle.handle];
        Animation* anim_data = &animation_data->animations[cur_skinned_mesh_inst->current_animation_index];


        cur_skinned_mesh_inst->current_time += delta_time;

        if (cur_skinned_mesh_inst->current_time > anim_data->anim_end)
        {
            cur_skinned_mesh_inst->current_time -= anim_data->anim_end;
        }

        for (u32 i = 0; i < anim_data->channel_count; i++)
        {
            const Animation_Channel* channel = &anim_data->channels[i];
            const Animation_Sampler* sampler = &anim_data->samplers[channel->sampler_idx];

            u32 j_idx = channel->joint_index;

            for (size_t timestamp_idx = 0; timestamp_idx < sampler->timestamps_count - 1; timestamp_idx++)
            {
                if (sampler->interpolation_type != Animation_Interpolation_Type_Linear)
                {
                    DEBUG("This sample only supports linear interpolations");
                    continue;
                }

                // Get the input keyframe values for the current time stamp
                if ((cur_skinned_mesh_inst->current_time >= sampler->timestamps[timestamp_idx]) && (
                    cur_skinned_mesh_inst->
                    current_time <=
                    sampler->timestamps[timestamp_idx + 1]))
                {
                    float interp_val = (cur_skinned_mesh_inst->current_time - sampler->timestamps[timestamp_idx]) /
                    (
                        sampler
                        ->
                        timestamps[timestamp_idx + 1] - sampler->timestamps[timestamp_idx]);
                    switch (channel->animation_path_type)
                    {
                    case Animation_Path_Type_Invalid:
                        MASSERT(false);
                        break;
                    case Animation_Path_Type_Translation:
                        cur_skinned_mesh_inst->local_translation[j_idx] = vec3_lerp(
                            sampler->interperlation_data.trs_vec3[timestamp_idx],
                            sampler->interperlation_data.trs_vec3[timestamp_idx +
                                1], interp_val);
                        break;
                    case Animation_Path_Type_Rotation:
                        quat q1;
                        q1.x = sampler->interperlation_data.trs_vec4[timestamp_idx].x;
                        q1.y = sampler->interperlation_data.trs_vec4[timestamp_idx].y;
                        q1.z = sampler->interperlation_data.trs_vec4[timestamp_idx].z;
                        q1.w = sampler->interperlation_data.trs_vec4[timestamp_idx].w;

                        quat q2;
                        q2.x = sampler->interperlation_data.trs_vec4[timestamp_idx + 1].x;
                        q2.y = sampler->interperlation_data.trs_vec4[timestamp_idx + 1].y;
                        q2.z = sampler->interperlation_data.trs_vec4[timestamp_idx + 1].z;
                        q2.w = sampler->interperlation_data.trs_vec4[timestamp_idx + 1].w;


                        cur_skinned_mesh_inst->local_rotation[j_idx] = quat_normalize(
                            quat_slerp(q1, q2, interp_val));;
                        break;
                    case Animation_Path_Type_Scale:
                        cur_skinned_mesh_inst->local_scale[j_idx] = vec3_lerp(
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

        mat4* local_matrix = allocator_alloc(frame_allocator, sizeof(mat4) * cur_skinned_mesh_inst->joint_count);
        mat4* model_matrix = allocator_alloc(frame_allocator, sizeof(mat4) * cur_skinned_mesh_inst->joint_count);

        for (u32 i = 0; i < cur_skinned_mesh_inst->joint_count; i++)
        {
            // Joint(N, t) = Translation(N, t) * Rotation(N, t) * Scale(N, t)

            const mat4 t = mat4_translation(cur_skinned_mesh_inst->local_translation[i]);
            const mat4 r = quat_to_mat4(cur_skinned_mesh_inst->local_rotation[i]);
            const mat4 s = mat4_scale(cur_skinned_mesh_inst->local_scale[i]);

            mat4 tr = mat4_mul(t, r);
            local_matrix[i] = mat4_mul(tr, s);
        }

        model_matrix[0] = local_matrix[0];
        for (u32 i = 0; i < cur_skinned_mesh_inst->joint_count; i++)
        {
            //Joint(N, t) = Parent(N, t) * Joint(N, t)
            u32 parent_index = animation_data->joints[i].parent_idx;
            model_matrix[i] = mat4_mul(local_matrix[parent_index], local_matrix[i]);
        }

        for (u32 i = 0; i < cur_skinned_mesh_inst->joint_count; i++)
        {
            //Joint(N, t) = Joint(N, t) * InverseBindMatrix(N)
            cur_skinned_mesh_inst->gpu_matrix[i] = mat4_mul(local_matrix[i],
                                                            animation_data->inverse_bind_matrix[i]);
        }
    }
}
