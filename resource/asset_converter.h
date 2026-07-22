#ifndef ASSET_CONVERTER_H
#define ASSET_CONVERTER_H

#include "asset_registry.h"
#include "resource_types.h"
#include "resource_import_types.h"


//takes in any file and checks its extension type, and calls the appropriate function
MAPI bool asset_convert_file(Asset_System* asset_system, const char* file_path);


MAPI bool asset_converter_texture(Asset_System* asset_system, const char* file_path);

MAPI bool asset_converter_font(Asset_System* asset_system, const char* file_path);

MAPI bool asset_converter_msdf_font(Asset_System* asset_system, const char* file_path);


// gltf format if we want, take what we need -> create a default version of that asset on disk,
// load it up and assign any texture or material data from the file, material data might have to be dynamic/reflection
//

//the source asset




MAPI bool asset_converter_load_gltf_mesh(Asset_System* asset_system, const char* gltf_path)
{
    Mesh_System* mesh_system = asset_system->mesh_system;
    Frame_Allocator* frame_allocator = asset_system->frame_allocator;

    if (!c_string_path_is_extension(gltf_path, ".gltf") && !c_string_path_is_extension(gltf_path, ".glb"))
    {
        FATAL("DID NOT PASS IN A GLTF FILE");
        return false;
    }


    cgltf_options options = {0};
    cgltf_data* data = NULL;
    cgltf_result result = cgltf_parse_file(&options, gltf_path, &data);

    if (result != cgltf_result_success)
    {
        fprintf(stderr, "Failed to parse glTF file: %s\n", gltf_path);
        return false;
    }
    result = cgltf_load_buffers(&options, data, gltf_path);
    MASSERT(result == cgltf_result_success)


    GLTF_Mesh* gltf_mesh = allocator_alloc(asset_system->frame_allocator,
                                           sizeof(GLTF_Mesh));
    gltf_mesh->mesh_count = data->meshes_count;
    gltf_mesh->submesh = allocator_alloc(asset_system->frame_allocator,
                                         sizeof(GLTF_SubMesh) * data->meshes_count);


    for (size_t mesh_idx = 0; mesh_idx < data->meshes_count; mesh_idx++)
    {
        GLTF_SubMesh* submesh = &gltf_mesh->submesh[mesh_idx];


        /* Find position accessor */
        const cgltf_accessor* pos_accessor = cgltf_find_accessor(data->meshes[mesh_idx].primitives,
                                                                 cgltf_attribute_type_position,
                                                                 0);
        //only neccesary attribute which is basically manadatory for us to have
        MASSERT(pos_accessor);
        if (pos_accessor)
        {
            //get size information
            cgltf_size num_floats = cgltf_accessor_unpack_floats(pos_accessor, NULL, 0);
            cgltf_size float_bytes = num_floats * sizeof(float);
            submesh->vertex_bytes = float_bytes;

            //alloc and copy data
            float* pos_data = allocator_alloc(frame_allocator, float_bytes);
            submesh->vertex = allocator_alloc(frame_allocator, float_bytes);
            cgltf_accessor_unpack_floats(pos_accessor, pos_data, num_floats);
            memcpy(submesh->vertex, pos_data, float_bytes);
        }

        // Find normal accessor
        const cgltf_accessor* norm_accessor = cgltf_find_accessor(data->meshes[mesh_idx].primitives,
                                                                  cgltf_attribute_type_normal,
                                                                  0);
        if (norm_accessor)
        {
            //get size information
            cgltf_size norm_floats = cgltf_accessor_unpack_floats(norm_accessor, NULL, 0);
            cgltf_size norm_bytes = norm_floats * sizeof(float);
            submesh->normal_bytes = norm_bytes;

            //alloc and copy data
            float* normal_data = allocator_alloc(frame_allocator, norm_bytes);
            submesh->normal = allocator_alloc(frame_allocator, norm_bytes);
            cgltf_accessor_unpack_floats(norm_accessor, normal_data, norm_floats);
            memcpy(submesh->normal, normal_data, norm_bytes);
        }

        //  Find tangent accessor
        const cgltf_accessor* tangent_accessor = cgltf_find_accessor(data->meshes[mesh_idx].primitives,
                                                                     cgltf_attribute_type_tangent, 0);

        if (tangent_accessor)
        {
            //get size information
            cgltf_size tangent_floats = cgltf_accessor_unpack_floats(tangent_accessor, NULL, 0);
            cgltf_size tangent_bytes = tangent_floats * sizeof(float);
            submesh->tangent_bytes = tangent_bytes;


            //alloc and copy data
            float* tangent_data = allocator_alloc(frame_allocator, tangent_bytes);
            submesh->tangent = allocator_alloc(frame_allocator, tangent_bytes);
            cgltf_accessor_unpack_floats(tangent_accessor, tangent_data, tangent_floats);
            memcpy(submesh->tangent, tangent_data, tangent_bytes);
        }

        //  Find texcoord accessor
        const cgltf_accessor* texcoord_accessor = cgltf_find_accessor(data->meshes[mesh_idx].primitives,
                                                                      cgltf_attribute_type_texcoord, 0);
        if (texcoord_accessor)
        {
            //get size information
            cgltf_size uv_floats_count = cgltf_accessor_unpack_floats(texcoord_accessor, NULL, 0);
            cgltf_size uv_byte_size = uv_floats_count * sizeof(float);
            submesh->uv_bytes = uv_byte_size;


            //alloc and copy data
            float* uv_data = allocator_alloc(frame_allocator, uv_byte_size);
            cgltf_accessor_unpack_floats(texcoord_accessor, uv_data, uv_floats_count);
            submesh->uv = allocator_alloc(frame_allocator, uv_byte_size);
            memcpy(submesh->uv, uv_data, uv_byte_size);
        }

        const cgltf_accessor* color_accessor = cgltf_find_accessor(data->meshes[mesh_idx].primitives,
                                                                   cgltf_attribute_type_color, 0);
        if (color_accessor)
        {
            //get size information
            cgltf_size color_floats_count = cgltf_accessor_unpack_floats(texcoord_accessor, NULL, 0);
            cgltf_size color_byte_size = color_floats_count * sizeof(float);
            submesh->vertex_color_bytes = color_byte_size;

            //alloc and copy data
            float* vertex_color_data = allocator_alloc(frame_allocator, color_byte_size);
            cgltf_accessor_unpack_floats(texcoord_accessor, vertex_color_data, color_floats_count);
            submesh->vertex_color = allocator_alloc(frame_allocator, color_byte_size);
            memcpy(submesh->vertex_color, vertex_color_data, color_byte_size);
        }
        else
        {
            //TODO: generate white colors for everything i guess
        }
        // Load indices
        // SEE componentType in the specs for more detail 3.6.2
        u8 index_stride = data->meshes[mesh_idx].primitives[0].indices->stride;
        if (index_stride == 2)
        {
            submesh->index_type = INDEX_TYPE_U16;
        }
        else if (index_stride == 4)
        {
            submesh->index_type = INDEX_TYPE_U16;
        }
        else
        {
            WARN("GLTF MESH LOADING: UNKNOWN INDEX TYPE STRIDE");
        }

        //TODO: there can be multiple primitices/indices, will come back to
        submesh->indices_bytes = data->meshes[mesh_idx].primitives->indices->count *
            index_stride;
        submesh->indices = allocator_alloc(frame_allocator,
                                           submesh->indices_bytes);

        const uint8_t* index_buffer_data = cgltf_buffer_view_data(
            data->meshes[mesh_idx].primitives->indices->buffer_view);
        memcpy(submesh->indices, index_buffer_data,
               submesh->indices_bytes);


        //LOAD TEXTURES/MATERIALS
        // GET BASE PATH
        char* base_path = c_string_path_strip(gltf_path, frame_allocator);

        //every mesh just gets loaded in with a default pbr, well convert the material later into a custom format
        GLTF_Material* gltf = &submesh->material_default;

        if (!data->meshes[mesh_idx].primitives->material)
        {
            MASSERT(false)
            WARN("NO MATERIAL DATA FOUND FOR GLTF MESH");
            return false;
        }

        //COLOR TEXTURE
        cgltf_texture* color_texture = data->meshes[mesh_idx].primitives->material->pbr_metallic_roughness.
                                                              base_color_texture.texture;
        if (color_texture && color_texture->image->uri)
        {
            gltf->flags |= MESH_PIPELINE_COLOR;
            size_t allocation_size = strlen(base_path) + strlen(color_texture->image->uri) + 1;

            char* texture_path = allocator_alloc(frame_allocator,
                                                 allocation_size);
            // takes a buffer, message format, then the remaining strings
            snprintf(texture_path, allocation_size, "%s%s", base_path, color_texture->image->uri);
            TRACE("COLOR Texture Path:  %s", texture_path);
            asset_converter_texture(asset_system, texture_path);
            memcpy(gltf->color.raw,
                   data->meshes[mesh_idx].primitives->material->pbr_metallic_roughness.base_color_factor,
                   sizeof(vec4s));
        }
        else
        {
            // default_material->flags |= MESH_PIPELINE_COLOR;
            TRACE("No Color Texture using fall back color");
            memcpy(gltf->color.raw,
                   data->meshes[mesh_idx].primitives->material->pbr_metallic_roughness.base_color_factor,
                   sizeof(vec4s));
        }

        //METAL-ROUGHNESS
        cgltf_texture* metal_roughness_texture = data->meshes[mesh_idx].
                                                 primitives->material->pbr_metallic_roughness.
                                                 metallic_roughness_texture.texture;
        if (metal_roughness_texture)
        {
            if (metal_roughness_texture->image->uri)
            {
                gltf->flags |= MESH_PIPELINE_ROUGHNESS;
                gltf->flags |= MESH_PIPELINE_METALLIC;
                size_t allocation_size = strlen(base_path) +
                    strlen(metal_roughness_texture->image->uri) + 1;
                char* texture_path = allocator_alloc(frame_allocator, allocation_size);
                // takes a buffer, message format, then the remaining strings
                snprintf(texture_path, allocation_size, "%s%s", base_path, metal_roughness_texture->image->uri);
                TRACE("METAL/ROUGHNESS Texture Path:  %s", texture_path);

                asset_converter_texture(asset_system, texture_path);
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
        cgltf_texture* AO_texture = data->meshes[mesh_idx].primitives->material->occlusion_texture.texture;
        if (AO_texture && AO_texture->image->uri)
        {
            gltf->flags |= MESH_PIPELINE_AO;
            size_t allocation_size = strlen(base_path) + strlen(AO_texture->image->uri) + 1;
            char* AO_texture_path = allocator_alloc(frame_allocator, allocation_size);
            // takes a buffer, message format, then the remaining strings
            snprintf(AO_texture_path, allocation_size, "%s%s", base_path, AO_texture->image->uri);

            TRACE("AO Texture Path:  %s", AO_texture_path);

            asset_converter_texture(asset_system, AO_texture_path);
        }


        //NORMAL TEXTURE
        // data->meshes[mesh_idx].primitives->material->has_pbr_metallic_roughness
        cgltf_texture* normal_texture = data->meshes[mesh_idx].primitives->material->normal_texture.texture;
        if (normal_texture && normal_texture->image->uri)
        {
            gltf->flags |= MESH_PIPELINE_NORMAL;
            size_t allocation_size = strlen(base_path) + strlen(normal_texture->image->uri) + 1;
            char* texture_path = allocator_alloc(frame_allocator, allocation_size);
            // takes a buffer, message format, then the remaining strings
            snprintf(texture_path, allocation_size, "%s%s", base_path, normal_texture->image->uri);
            TRACE("NORMAL Texture Path:  %s", texture_path);
            asset_converter_texture(asset_system, texture_path);
        }

        //EMISSIVE TEXTURE
        cgltf_texture* emissive_texture = data->meshes[mesh_idx].primitives->material->emissive_texture.
                                                                 texture;
        if (emissive_texture && emissive_texture->image->uri)
        {
            gltf->flags |= MESH_PIPELINE_EMISSIVE;
            size_t allocation_size = strlen(base_path) + strlen(emissive_texture->image->uri) + 1;

            char* texture_path = allocator_alloc(frame_allocator, allocation_size);
            // takes a buffer, message format, then the remaining strings
            snprintf(texture_path, allocation_size, "%s%s", base_path, emissive_texture->image->uri);
            TRACE("EMISSIVE Texture Path:  %s", texture_path);

            asset_converter_texture(asset_system, texture_path);
        }
    }


    //check if we are loading a skinned or normal mesh
    if (data->skins_count > 0)
    {
        //TODO: this is an unhandled case, and honestly why would a mesh have more than 1 skin???
        MASSERT(data->skins_count <= 1);
        gltf_mesh->has_skeleton = true;

        gltf_mesh->skinned_submesh =
            allocator_alloc(asset_system->frame_allocator, sizeof(GLTF_Skinned_SubMesh) * data->meshes_count);

        for (u32 skmesh_idx = 0; skmesh_idx < data->meshes_count; skmesh_idx++)
        {
            GLTF_Skinned_SubMesh* skinned_submesh = &gltf_mesh->skinned_submesh[skmesh_idx];

            const cgltf_accessor* joint_accessor = cgltf_find_accessor(data->meshes[skmesh_idx].primitives,
                                                                       cgltf_attribute_type_joints,
                                                                       0);
            if (joint_accessor)
            {
                //get size information
                cgltf_size num_floats = cgltf_accessor_unpack_floats(joint_accessor, NULL, 0);
                cgltf_size float_bytes = num_floats * sizeof(float);
                skinned_submesh->joint_bytes = float_bytes;

                //alloc and copy data
                float* joint_data = allocator_alloc(frame_allocator, float_bytes);
                skinned_submesh->joints = allocator_alloc(frame_allocator, float_bytes);
                cgltf_accessor_unpack_floats(joint_accessor, joint_data, num_floats);
                memcpy(skinned_submesh->joints, joint_data, float_bytes);
            }

            const cgltf_accessor* weight_accessor = cgltf_find_accessor(data->meshes[skmesh_idx].primitives,
                                                                        cgltf_attribute_type_weights,
                                                                        0);
            if (weight_accessor)
            {
                //get size information

                cgltf_size num_floats = cgltf_accessor_unpack_floats(weight_accessor, NULL, 0);
                cgltf_size float_bytes = num_floats * sizeof(float);
                skinned_submesh->weight_bytes = float_bytes;

                //alloc and copy data
                float* weight_data = allocator_alloc(frame_allocator, float_bytes);
                skinned_submesh->weights = allocator_alloc(frame_allocator, float_bytes);
                cgltf_accessor_unpack_floats(weight_accessor, weight_data, num_floats);
                memcpy(skinned_submesh->weights, weight_data, float_bytes);
            }
        }


        hash_table* joint_name_to_index = HASH_TABLE_CREATE(size_t, 200);

        gltf_mesh->animation_data = allocator_alloc(frame_allocator, sizeof(Animation) * data->animations_count);
        GLTF_Animation_Data* animation_data = gltf_mesh->animation_data;
        animation_data->animations = allocator_alloc(frame_allocator, sizeof(Animation) * data->animations_count);
        animation_data->animations_count = data->animations_count;


        for (size_t skin_idx = 0; skin_idx < data->skins_count; skin_idx++)
        {
            cgltf_skin* skin_data = &data->skins[skin_idx];
            animation_data->joint_count = skin_data->joints_count;
            animation_data->joints = allocator_alloc(frame_allocator, skin_data->joints_count * sizeof(Joint));
            animation_data->resting_pose_local_matrix = allocator_alloc(
                frame_allocator, skin_data->joints_count * sizeof(mat4s));

            // Inverse bind matrices — one 4x4 float matrix per joint
            if (skin_data->inverse_bind_matrices)
            {
                //get size information
                cgltf_size ibm_floats = cgltf_accessor_unpack_floats(skin_data->inverse_bind_matrices, NULL, 0);
                cgltf_size ibm_bytes = ibm_floats * sizeof(float);

                //alloc and copy data
                float* ibm_date = allocator_alloc(frame_allocator, ibm_bytes);
                animation_data->inverse_bind_matrix = allocator_alloc(frame_allocator, ibm_bytes);

                cgltf_accessor_unpack_floats(skin_data->inverse_bind_matrices, ibm_date, ibm_floats);
                memcpy(animation_data->inverse_bind_matrix, ibm_date, ibm_bytes);
            }
            else
            {
                // glTF spec: absent inverse_bind_matrices means identity per joint
                animation_data->inverse_bind_matrix = allocator_alloc(frame_allocator,
                                                                      sizeof(mat4s) * skin_data->joints_count);

                for (cgltf_size j = 0; j < skin_data->joints_count; j++)
                {
                    animation_data->inverse_bind_matrix[j] = glms_mat4_identity();
                }
            }

            // cur_joint->inverse_bind_matrix = data->skins[skin_idx].inverse_bind_matrices[joint_idx].;
            for (size_t joint_idx = 0; joint_idx < skin_data->joints_count; joint_idx++)
            {
                Joint* cur_joint = &animation_data->joints[joint_idx];
                cgltf_node* cgltf_joint = skin_data->joints[joint_idx];

                cur_joint->joint_name = c_string_duplicate_allocator(cgltf_joint->name, frame_allocator);
                cur_joint->id = joint_idx;

                hash_table_insert(joint_name_to_index, cur_joint->joint_name, &joint_idx);
            }
            //this pass is to get the parent id's and parent nodes
            for (size_t joint_idx = 0; joint_idx < data->skins[skin_idx].joints_count; joint_idx++)
            {
                Joint* cur_joint = &animation_data->joints[joint_idx];
                cgltf_node* cgltf_joint = data->skins[skin_idx].joints[joint_idx];

                mat4s* local_mat = &animation_data->resting_pose_local_matrix[joint_idx];
                cgltf_node_transform_local(cgltf_joint, local_mat->raw); // 16 floats, column-major

                size_t parent_idx = 0;
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
                    cur_animation->animation_name =
                        STRING_CREATE_FROM_BUFFER_ALLOCATOR(anim_data->name, frame_allocator);
                }
                else
                {
                    cur_animation->animation_name = STRING_CREATE_FROM_BUFFER_ALLOCATOR(
                        data->meshes[0].name, frame_allocator);
                    cur_animation->animation_name = string_concat(cur_animation->animation_name,
                                                                  string_from_int(
                                                                      animation_idx, frame_allocator),
                                                                  frame_allocator);
                }
                cgltf_size channel_count = anim_data->channels_count;
                cgltf_size sampler_count = anim_data->samplers_count;
                cur_animation->channel_count = channel_count;
                cur_animation->sampler_count = sampler_count;

                cur_animation->channels = allocator_alloc(frame_allocator, sizeof(Animation_Channel) * channel_count);
                cur_animation->samplers = allocator_alloc(frame_allocator, sizeof(Animation_Sampler) * sampler_count);


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
                    cur_sampler->sampler_start = FLT_MAX;
                    cur_sampler->sampler_end = -FLT_MAX;
                    cgltf_animation_sampler* anim_sampler = &anim_data->samplers[sampler_idx];


                    //read gltf input data
                    cur_sampler->timestamps_count = anim_sampler->input->count;
                    cur_sampler->timestamps =
                        allocator_alloc(frame_allocator, sizeof(float) * cur_sampler->timestamps_count);
                    //get size information
                    cgltf_size num_floats = cgltf_accessor_unpack_floats(anim_sampler->input, NULL, 0);
                    cgltf_size float_bytes = num_floats * sizeof(float);
                    //alloc and copy data
                    float* timestamp_data = allocator_alloc(frame_allocator, float_bytes);
                    cgltf_accessor_unpack_floats(anim_sampler->input, timestamp_data, num_floats);
                    memcpy(cur_sampler->timestamps, timestamp_data, float_bytes);


                    for (u32 timestamp_idx = 0; timestamp_idx < cur_sampler->timestamps_count; timestamp_idx++)
                    {
                        cur_sampler->sampler_start = min_f(cur_sampler->sampler_start,
                                                           cur_sampler->timestamps[timestamp_idx]);
                        cur_sampler->sampler_end = max_f(cur_sampler->sampler_end,
                                                         cur_sampler->timestamps[timestamp_idx]);
                    }

                    cur_animation->anim_start = min_f(cur_animation->anim_start, cur_sampler->sampler_start);
                    cur_animation->anim_end = max_f(cur_animation->anim_end, cur_sampler->sampler_end);

                    //read gltf output data
                    cur_sampler->trs_interpolation_count = anim_sampler->output->count;
                    // const uint8_t* trs_buffer_data = cgltf_buffer_view_data(anim_sampler->output->buffer_view);
                    cgltf_size output_num_floats = cgltf_accessor_unpack_floats(anim_sampler->output, NULL, 0);
                    cgltf_size output_float_bytes = output_num_floats * sizeof(float);
                    float* trs_buffer_data = allocator_alloc(frame_allocator, output_float_bytes);
                    cgltf_accessor_unpack_floats(anim_sampler->output, trs_buffer_data, output_num_floats);

                    cur_sampler->interpolation_type = Animation_Interpolation_Type_gltf_to_engine[anim_sampler->
                        interpolation];
                    switch (anim_sampler->output->type)
                    {
                    //these are the only supported formats in the gltf spec for samplers
                    case cgltf_type_scalar: // weights
                        cur_sampler->interperlation_data.trs_float = allocator_alloc(frame_allocator,
                            sizeof(float) * output_num_floats);
                        memcpy(cur_sampler->interperlation_data.trs_float, trs_buffer_data, output_float_bytes);
                        break;
                    case cgltf_type_vec3: // translation, scale
                        cur_sampler->interperlation_data.trs_vec3 = allocator_alloc(
                            frame_allocator, sizeof(float) * output_num_floats);
                        memcpy(cur_sampler->interperlation_data.trs_vec3, trs_buffer_data, output_float_bytes);
                        break;
                    case cgltf_type_vec4: // rotation
                        cur_sampler->interperlation_data.trs_vec4 = allocator_alloc(
                            frame_allocator, sizeof(float) * output_num_floats);
                        memcpy(cur_sampler->interperlation_data.trs_vec4, trs_buffer_data, output_float_bytes);
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


    //build our engine format
    Madness_Mesh_Editor engine_format = {0};
    engine_format.version = 1.0;
    engine_format.mesh_count = gltf_mesh->mesh_count;

    engine_format.sub_mesh = allocator_alloc(frame_allocator, sizeof(Madness_Mesh) * engine_format.mesh_count);
    engine_format.mesh_gpu_upload = allocator_alloc(frame_allocator, sizeof(Madness_Mesh_GPU_Data) * engine_format.mesh_count);
    // engine_format.material_info = allocator_alloc(frame_allocator, sizeof(Mesh_Material_Info) * engine_format.mesh_count);

    for (u32 i = 0; i < gltf_mesh->mesh_count; ++i)
    {
        GLTF_SubMesh* gltf_submesh = &gltf_mesh->submesh[i];
        Madness_Mesh_GPU_Data* mesh_gpu_data = &engine_format.mesh_gpu_upload[i];
        Madness_SubMesh* engine_submesh = &engine_format.sub_mesh[i];


        engine_submesh->index_count = gltf_submesh->index_count;
        engine_submesh->index_type = gltf_submesh->index_type;
        engine_submesh->indices_bytes = gltf_submesh->indices_bytes;
        engine_submesh->normal_bytes = gltf_submesh->normal_bytes;
        engine_submesh->tangent_bytes = gltf_submesh->tangent_bytes;
        engine_submesh->uv_bytes = gltf_submesh->uv_bytes;
        engine_submesh->vertex_bytes = gltf_submesh->vertex_bytes;
        engine_submesh->vertex_color_bytes = gltf_submesh->vertex_color_bytes;
        engine_submesh->vertex_count = gltf_submesh->vertex_count;

        mesh_gpu_data->vertex = gltf_submesh->vertex;
        mesh_gpu_data->vertex_color = gltf_submesh->vertex_color;
        mesh_gpu_data->indices = gltf_submesh->indices;
        mesh_gpu_data->normal = gltf_submesh->normal;
        mesh_gpu_data->tangent = gltf_submesh->tangent;
        mesh_gpu_data->uv = gltf_submesh->uv;

        if (gltf_mesh->has_skeleton)
        {
            //TODO:
        }
    }


    //write it out to the file
    Asset_MetaData meta_data = {0};

    if (gltf_mesh->has_skeleton)
    {
        String_Builder* file_path_strip = string_builder_create(256, asset_system->frame_allocator);
        string_builder_append_c_string(file_path_strip, gltf_path);
        string_builder_strip_extension(file_path_strip);
        string_builder_strip_path(file_path_strip);

        String_Builder* str_builder = string_builder_create(256, asset_system->frame_allocator);
        string_builder_append_c_string(str_builder, ENGINE_SK_MESH_PATH);
        string_builder_append_builder(str_builder, file_path_strip);
        string_builder_append_c_string(str_builder, ENGINE_SK_MESH_PATH);

        const char* output_path = string_builder_to_c_string(str_builder);

        //write out the engine format data
        FILE* fptr = fopen(output_path, "wb");

        if (!fptr)
        {
            MASSERT(false);
        }

        fwrite(&engine_format.version, sizeof(engine_format.version), 1, fptr);
        fwrite(&engine_format.mesh_count, sizeof(engine_format.mesh_count), 1, fptr);
        fwrite(engine_format.sub_mesh, sizeof(Madness_SubMesh) * engine_format.mesh_count, 1, fptr);

        //mesh data
        for (u32 i = 0; i < engine_format.mesh_count; ++i)
        {
            Madness_SubMesh* sub_mesh = &engine_format.sub_mesh[i];
            fwrite(engine_format.mesh_gpu_upload[i].tangent, sub_mesh->tangent_bytes, 1, fptr);
            fwrite(engine_format.mesh_gpu_upload[i].vertex_color, sub_mesh->vertex_color_bytes, 1, fptr);
            fwrite(engine_format.mesh_gpu_upload[i].vertex, sub_mesh->vertex_bytes, 1, fptr);
            fwrite(engine_format.mesh_gpu_upload[i].normal, sub_mesh->normal_bytes, 1, fptr);
            fwrite(engine_format.mesh_gpu_upload[i].uv, sub_mesh->uv_bytes, 1, fptr);
            fwrite(engine_format.mesh_gpu_upload[i].indices, sub_mesh->indices_bytes, 1, fptr);
        }
        //material data
        for (u32 i = 0; i < engine_format.mesh_count; ++i)
        {
            //TODO: figure it out tomorrow
            // Madness_SubMesh* sub_mesh = &engine_format.sub_mesh[i];
            // string_serialize();
            // string_serialize();
            // fwrite(engine_format.mesh_gpu_upload[i].tangent, sub_mesh->tangent_bytes, 1, fptr);

        }


        // write out metadata
        meta_data.source_file = STRING_CREATE_FROM_BUFFER_HEAP_ALLOCATOR(gltf_path, asset_system->heap_allocator);
        meta_data.binary_file = string_builder_to_string(str_builder);
        meta_data.type = ASSET_STATIC_MESH;
        meta_data.uuid = madness_uuid_generate_return();



    }
    else
    {
        String_Builder* file_path_strip = string_builder_create(256, asset_system->frame_allocator);
        string_builder_append_c_string(file_path_strip, gltf_path);
        string_builder_strip_extension(file_path_strip);
        string_builder_strip_path(file_path_strip);

        String_Builder* str_builder = string_builder_create(256, asset_system->frame_allocator);
        string_builder_append_c_string(str_builder, ENGINE_MESH_PATH);
        string_builder_append_builder(str_builder, file_path_strip);
        string_builder_append_c_string(str_builder, ENGINE_MESH_EXTENSION);

        const char* output_path = string_builder_to_c_string(str_builder);

        //write out the engine format data
        //write out the engine format data
        FILE* fptr = fopen(output_path, "wb");

        if (!fptr)
        {
            MASSERT(false);
        }


        // write out metadata

        meta_data.source_file = STRING_CREATE_FROM_BUFFER_HEAP_ALLOCATOR(gltf_path, asset_system->heap_allocator);
        meta_data.binary_file = string_builder_to_string(str_builder);
        meta_data.type = ASSET_SKINNED_MESH;
        meta_data.uuid = madness_uuid_generate_return();
    }
    asset_registry_add_asset(asset_system->asset_registry, &meta_data);


    cgltf_free(data);
    return true;
}


#endif
