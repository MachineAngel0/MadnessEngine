#include "mesh_system.h"

#include "cgltf.h"
#include "c_string.h"
#include "resource_types.h"
#include "ufbx.h"


Mesh_System* mesh_system_init(Memory_System* memory_system)
{
    Mesh_System* out_mesh_system = memory_system_alloc(memory_system, sizeof(Mesh_System), MEMORY_SUBSYSTEM_MESH);
    memset(out_mesh_system, 0, sizeof(Mesh_System));

    memset(out_mesh_system->static_mesh_array, 0, sizeof(static_mesh) * 1000);
    out_mesh_system->static_mesh_array;

    out_mesh_system->vertex_byte_size = 0;
    out_mesh_system->index_byte_size = 0;
    out_mesh_system->normals_byte_size = 0;
    out_mesh_system->tangent_byte_size = 0;
    out_mesh_system->uv_byte_size = 0;


    INFO("MESH SYSTEM CREATED")

    return out_mesh_system;
}


submesh* submesh_init(Arena* arena)
{
    submesh* m = arena_alloc(arena, sizeof(submesh));

    // m->vertices.color = darray_create(vec4);
    m->index_type = VK_INDEX_TYPE_UINT32;

    return m;
}

void sub_mesh_free(submesh* m)
{
    darray_free(m->pos);
    darray_free(m->normal);
    darray_free(m->uv);
    darray_free(m->indices);
    darray_free(m->tangent);
    // darray_free(m->textures);
    free(m);
}

static_mesh* static_mesh_init(Arena* arena, u32 mesh_size)
{
    static_mesh* out_static_mesh = arena_alloc(arena, sizeof(submesh));
    out_static_mesh->mesh_size = mesh_size;
    out_static_mesh->mesh = arena_alloc(arena, sizeof(submesh) * mesh_size);

    return out_static_mesh;
}

skinned_mesh* skinned_mesh_init(Arena* arena, u32 mesh_size)
{
    skinned_mesh* out_static_mesh = arena_alloc(arena, sizeof(skinned_mesh));
    out_static_mesh->mesh_size = mesh_size;
    out_static_mesh->mesh = arena_alloc(arena, sizeof(submesh) * mesh_size);

    return out_static_mesh;
}

void static_mesh_free(static_mesh* static_mesh)
{
    UNIMPLEMENTED();
    // return to a free list of some sort
    /*
    for (u64 i = 0; i < static_mesh->mesh_size; i++)
    {
        sub_mesh_free(&static_mesh->mesh[i]);
    }
    static_mesh* out_static_mesh = arena_alloc(arena, sizeof(submesh));
    out_static_mesh->mesh_size = mesh_size;
    out_static_mesh->indirect_draw_array = arena_alloc(arena, mesh_size * sizeof(VkDrawIndexedIndirectCommand));
    out_static_mesh->mesh = arena_alloc(arena, sizeof(submesh) * mesh_size);

    return out_static_mesh;
    */
}


void mesh_load_gltf(Mesh_System* mesh_system, const char* gltf_path, Arena* arena, Frame_Arena* frame_arena,
                    Resource_System*
                    resource_system)
{
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

    static_mesh* out_static_mesh = static_mesh_init(arena, data->meshes_count);


    // GET BASE PATH
    char* base_path = c_string_path_strip(gltf_path, frame_arena);


    //LOAD VERTEX/INDEX DATA
    //Load Textures


    for (size_t mesh_idx = 0; mesh_idx < data->meshes_count; mesh_idx++)
    {
        submesh* current_submesh = &out_static_mesh->mesh[mesh_idx];

        /* Find position accessor */
        const cgltf_accessor* pos_accessor = cgltf_find_accessor(data->meshes[mesh_idx].primitives,
                                                                 cgltf_attribute_type_position,
                                                                 0);
        MASSERT(pos_accessor);
        if (pos_accessor)
        {
            //get size information

            cgltf_size num_floats = cgltf_accessor_unpack_floats(pos_accessor, NULL, 0);
            cgltf_size float_bytes = num_floats * sizeof(float);
            current_submesh->vertex_bytes = float_bytes;


            //alloc and copy data
            float* pos_data = arena_alloc(frame_arena, float_bytes);
            current_submesh->pos = arena_alloc(arena, float_bytes);
            cgltf_accessor_unpack_floats(pos_accessor, pos_data, num_floats);
            memcpy(current_submesh->pos, pos_data, float_bytes);
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
            current_submesh->normal_bytes = norm_bytes;

            //alloc and copy data
            float* normal_data = arena_alloc(arena, norm_bytes);
            current_submesh->normal = arena_alloc(arena, norm_bytes);
            cgltf_accessor_unpack_floats(norm_accessor, normal_data, norm_floats);
            memcpy(current_submesh->normal, normal_data, norm_bytes);
        }

        //  Find tangent accessor
        const cgltf_accessor* tangent_accessor = cgltf_find_accessor(data->meshes[mesh_idx].primitives,
                                                                     cgltf_attribute_type_tangent, 0);

        if (tangent_accessor)
        {
            //get size information
            cgltf_size tangent_floats = cgltf_accessor_unpack_floats(tangent_accessor, NULL, 0);
            cgltf_size tangent_bytes = tangent_floats * sizeof(float);
            current_submesh->tangent_bytes = tangent_bytes;


            //alloc and copy data
            float* tangent_data = arena_alloc(arena, tangent_bytes);
            current_submesh->tangent = arena_alloc(arena, tangent_bytes);
            cgltf_accessor_unpack_floats(tangent_accessor, tangent_data, tangent_floats);
            memcpy(current_submesh->tangent, tangent_data, tangent_bytes);
        }

        //  Find texcoord accessor
        const cgltf_accessor* texcoord_accessor = cgltf_find_accessor(data->meshes[mesh_idx].primitives,
                                                                      cgltf_attribute_type_texcoord, 0);
        if (texcoord_accessor)
        {
            //get size information
            cgltf_size uv_floats_count = cgltf_accessor_unpack_floats(texcoord_accessor, NULL, 0);
            cgltf_size uv_byte_size = uv_floats_count * sizeof(float);
            current_submesh->uv_bytes = uv_byte_size;


            //alloc and copy data
            float* uv_data = arena_alloc(frame_arena, uv_byte_size);
            cgltf_accessor_unpack_floats(texcoord_accessor, uv_data, uv_floats_count);

            current_submesh->uv = arena_alloc(arena, uv_byte_size);
            memcpy(current_submesh->uv, uv_data, uv_byte_size);
        }

        // Load indices
        // SEE componentType in the specs for more detail 3.6.2
        u8 index_stride = data->meshes[mesh_idx].primitives[0].indices->stride;
        if (index_stride == 2)
        {
            current_submesh->index_type = VK_INDEX_TYPE_UINT16;
        }
        else if (index_stride == 4)
        {
            current_submesh->index_type = VK_INDEX_TYPE_UINT32;
        }
        else
        {
            WARN("GLTF MESH LOADING: UNKNOWN INDEX TYPE STRIDE");
        }


        current_submesh->indices_count = data->meshes[mesh_idx].primitives->indices->count;
        current_submesh->indices_bytes = data->meshes[mesh_idx].primitives->indices->count *
            index_stride;
        //TODO: there can be multiple primitices/indices, will come back to
        current_submesh->indices = arena_alloc(arena,
                                               current_submesh->indices_bytes);

        const uint8_t* index_buffer_data = cgltf_buffer_view_data(
            data->meshes[mesh_idx].primitives->indices->buffer_view);
        memcpy(current_submesh->indices, index_buffer_data,
               current_submesh->indices_bytes);
        /*
        cgltf_accessor_unpack_indices(data->meshes[mesh_idx].primitives->indices,
                                      out_static_mesh->mesh[mesh_idx].indices,
                                      index_stride,
                                      out_static_mesh->mesh[mesh_idx].indices_count);
        */

        //LOAD TEXTURES/MATERIALS

        //COLOR TEXTURE
        cgltf_texture* color_texture = data->meshes[mesh_idx].primitives->material->pbr_metallic_roughness.
                                                              base_color_texture.texture;
        if (color_texture && color_texture->image->uri)
        {
            current_submesh->mesh_pipeline_mask |= MESH_PIPELINE_COLOR;

            char* texture_path = arena_alloc(frame_arena,
                                             strlen(base_path) + strlen(color_texture->image->uri));
            // takes a buffer, message format, then the remaining strings
            sprintf(texture_path, "%s%s", base_path, color_texture->image->uri);
            TRACE("COLOR Texture Path:  %s", texture_path);

            texture_system_load_texture(resource_system->texture_system, texture_path, &current_submesh->color_texture);
            current_submesh->material_params.color_index = current_submesh->color_texture.handle;
        }

        //METAL-ROUGHNESS
        cgltf_texture* metal_roughness_texture = data->meshes[mesh_idx].primitives->material->pbr_metallic_roughness.
                                                                        metallic_roughness_texture.texture;
        if (metal_roughness_texture)
        {
            if (metal_roughness_texture->image->uri)
            {
                current_submesh->mesh_pipeline_mask |= MESH_PIPELINE_ROUGHNESS;
                current_submesh->mesh_pipeline_mask |= MESH_PIPELINE_METALLIC;
                char* texture_path = arena_alloc(frame_arena,
                                                 strlen(base_path) + strlen(metal_roughness_texture->image->uri));
                // takes a buffer, message format, then the remaining strings
                sprintf(texture_path, "%s%s", base_path, metal_roughness_texture->image->uri);
                TRACE("METAL/ROUGHNESS Texture Path:  %s", texture_path);

                texture_system_load_texture(resource_system->texture_system, texture_path,
                                            &current_submesh->metallic_texture);
                current_submesh->roughness_texture = current_submesh->metallic_texture;

                current_submesh->material_params.metallic_index = current_submesh->metallic_texture.handle;
                current_submesh->material_params.roughness_index = current_submesh->roughness_texture.handle;
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
            current_submesh->mesh_pipeline_mask |= MESH_PIPELINE_AO;
            char* texture_path = arena_alloc(frame_arena,
                                             strlen(base_path) + strlen(AO_texture->image->uri));
            // takes a buffer, message format, then the remaining strings
            sprintf(texture_path, "%s%s", base_path, AO_texture->image->uri);
            TRACE("AO Texture Path:  %s", texture_path);

            texture_system_load_texture(resource_system->texture_system, texture_path,
                                        &current_submesh->ambient_occlusion_texture);
            current_submesh->material_params.ambient_occlusion_index = current_submesh->ambient_occlusion_texture.
                handle;
        }


        //NORMAL TEXTURE
        // data->meshes[mesh_idx].primitives->material->has_pbr_metallic_roughness
        cgltf_texture* normal_texture = data->meshes[mesh_idx].primitives->material->normal_texture.texture;
        if (normal_texture && normal_texture->image->uri)
        {
            current_submesh->mesh_pipeline_mask |= MESH_PIPELINE_NORMAL;
            char* texture_path = arena_alloc(frame_arena,
                                             strlen(base_path) + strlen(normal_texture->image->uri));
            // takes a buffer, message format, then the remaining strings
            sprintf(texture_path, "%s%s", base_path, normal_texture->image->uri);
            TRACE("NORMAL Texture Path:  %s", texture_path);

            texture_system_load_texture(resource_system->texture_system, texture_path,
                                        &current_submesh->normal_texture);
            current_submesh->material_params.normal_index = current_submesh->normal_texture.
                                                                             handle;
        }

        //EMISSIVE TEXTURE
        cgltf_texture* emissive_texture = data->meshes[mesh_idx].primitives->material->emissive_texture.texture;
        if (emissive_texture && emissive_texture->image->uri)
        {
            current_submesh->mesh_pipeline_mask |= MESH_PIPELINE_EMISSIVE;
            char* texture_path = arena_alloc(frame_arena,
                                             strlen(base_path) + strlen(emissive_texture->image->uri));
            // takes a buffer, message format, then the remaining strings
            sprintf(texture_path, "%s%s", base_path, emissive_texture->image->uri);
            TRACE("EMISSIVE Texture Path:  %s", texture_path);

            texture_system_load_texture(resource_system->texture_system, texture_path,
                                        &current_submesh->emissive_texture);
            current_submesh->material_params.emissive_index = current_submesh->emissive_texture.
                                                                               handle;
        }


        //TODO: i am technically copy the data twice which is completely not needed
        current_submesh->material_params.feature_mask = current_submesh->mesh_pipeline_mask;

        mesh_system->vertex_byte_size += current_submesh->vertex_bytes;
        mesh_system->index_byte_size += current_submesh->indices_bytes;
        mesh_system->index_count_size += out_static_mesh->mesh[mesh_idx].indices_count;
        mesh_system->normals_byte_size += current_submesh->normal_bytes;
        mesh_system->tangent_byte_size += current_submesh->tangent_bytes;
        mesh_system->uv_byte_size += current_submesh->uv_bytes;


        mesh_system->static_mesh_submesh_size++;
    }

    //load materials
    // if (data->meshes[0].primitives->material) {
    //     int mat_idx = cgltf_material_index(data, data->meshes[0].primitives->material);
    //     int hi = 0;
    // }
    // cgltf_texture_index();

    //TODO: push static mesh into mesh system
    // upload textures and descriptors to shader_system
    //
    // for (u32 i = 0; i < out_static_mesh->mesh_size; i++)
    // {
    //     update_texture_bindless_descriptor_set(renderer,
    //                                            renderer->descriptor_system,
    //                                            out_static_mesh->mesh[i].color_texture);
    // }


    mesh_system->static_mesh_array[mesh_system->static_mesh_array_size] = *out_static_mesh;
    mesh_system->static_mesh_array_size++;

    cgltf_free(data);
}

void mesh_load_anim_gltf(Mesh_System* mesh_system, const char* gltf_path, Arena* arena, Frame_Arena* frame_arena,
                         Resource_System* resource_system)
{
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
    MASSERT(result == cgltf_result_success);

    skinned_mesh* out_static_mesh = skinned_mesh_init(arena, data->meshes_count);


    // GET BASE PATH
    char* base_path = c_string_path_strip(gltf_path, frame_arena);


    for (size_t mesh_idx = 0; mesh_idx < data->meshes_count; mesh_idx++)
    {
        skinned_submesh* current_submesh = &out_static_mesh->mesh[mesh_idx];


        const cgltf_accessor* joint_accessor = cgltf_find_accessor(data->meshes[mesh_idx].primitives,
                                                                   cgltf_attribute_type_joints,
                                                                   0);
        if (joint_accessor)
        {
            //get size information
            cgltf_size num_floats = cgltf_accessor_unpack_floats(joint_accessor, NULL, 0);
            cgltf_size float_bytes = num_floats * sizeof(float);

            current_submesh->joint_bytes = float_bytes;

            //alloc and copy data
            float* joint_data = arena_alloc(frame_arena, float_bytes);
            current_submesh->joints = arena_alloc(arena, float_bytes);
            cgltf_accessor_unpack_floats(joint_accessor, joint_data, num_floats);
            memcpy(current_submesh->joints, joint_data, float_bytes);
        }

        const cgltf_accessor* weight_accessor = cgltf_find_accessor(data->meshes[mesh_idx].primitives,
                                                                    cgltf_attribute_type_weights,
                                                                    0);

        if (weight_accessor)
        {
            //get size information

            cgltf_size num_floats = cgltf_accessor_unpack_floats(weight_accessor, NULL, 0);
            cgltf_size float_bytes = num_floats * sizeof(float);
            current_submesh->weight_bytes = float_bytes;


            //alloc and copy data
            float* weight_data = arena_alloc(frame_arena, float_bytes);
            current_submesh->weights = arena_alloc(arena, float_bytes);
            cgltf_accessor_unpack_floats(weight_accessor, weight_data, num_floats);
            memcpy(current_submesh->weights, weight_data, float_bytes);
        }

        mesh_system->weight_byte_size += current_submesh->weight_bytes;
        mesh_system->joints_byte_size += current_submesh->joint_bytes;
    }

    hash_table* joint_names = HASH_TABLE_CREATE(Joint, 200);
    for (size_t skin_idx = 0; skin_idx < data->skins_count; skin_idx++)
    {
        out_static_mesh->joint_count = data->skins[skin_idx].joints_count;
        out_static_mesh->joints = arena_alloc(arena, out_static_mesh->joint_count * sizeof(Joint));
        // cur_joint->inverse_bind_matrix = data->skins[skin_idx].inverse_bind_matrices[joint_idx].;
        for (size_t joint_idx = 0; joint_idx < data->skins[skin_idx].joints_count; joint_idx++)
        {
            Joint* cur_joint = &out_static_mesh->joints[joint_idx];
            cgltf_node* cgltf_joint = data->skins[skin_idx].joints[joint_idx];

            cur_joint->joint_name = cgltf_joint->name;
            cur_joint->id = joint_idx;
            cur_joint->children_count = cgltf_joint->children_count;
            cur_joint->children = arena_alloc(arena, cur_joint->children_count * sizeof(Joint*));
            hash_table_insert(joint_names, cur_joint->joint_name, cur_joint);
        }
        //this pass is to get the children and parent nodes
        for (size_t joint_idx = 0; joint_idx < data->skins[skin_idx].joints_count; joint_idx++)
        {
            Joint* cur_joint = &out_static_mesh->joints[joint_idx];
            cgltf_node* cgltf_joint = data->skins[skin_idx].joints[joint_idx];

            Joint* parent_joint;
            hash_table_get(joint_names, cgltf_joint->parent->name, &parent_joint);
            cur_joint->parent = parent_joint;

            for (u32 joint_child_idx = 0; joint_child_idx < cur_joint->children_count; joint_child_idx++)
            {
                Joint* child_joint;
                hash_table_get(joint_names, cgltf_joint->children[joint_child_idx]->name, &child_joint);
                if (child_joint)
                {
                    cur_joint->children[joint_child_idx] = child_joint;
                }
                else
                {
                    FATAL("MESH LOAD ANIM GLTF: INVALID JOINT NODE IN HASH TABLE");
                }
            }
        }
    }

    hash_table_destroy(joint_names);


    Animation* out_animation_data = arena_alloc(arena, sizeof(Animation) * data->animations_count);
    for (size_t animation_idx = 0; animation_idx < data->animations_count; animation_idx++)
    {
        cgltf_animation* anim_data = &data->animations[animation_idx];

        out_animation_data->animation_name = anim_data->name;
        cgltf_size channel_count = anim_data->channels_count;
        cgltf_size sampler_count = anim_data->samplers_count;
        out_animation_data->channel_count = channel_count;
        out_animation_data->sampler_count = sampler_count;

        out_animation_data->channels = arena_alloc(arena, sizeof(Animation_Channel) * channel_count);
        out_animation_data->samplers = arena_alloc(arena, sizeof(Animation_Sampler) * sampler_count);


        for (size_t channel_idx = 0; channel_idx < channel_count; channel_idx++)
        {
            Animation_Channel* cur_channel = &out_animation_data->channels[channel_idx];
            cgltf_animation_channel* anim_channel = &anim_data->channels[channel_idx];

            cur_channel->animation_path_type = anim_channel->target_path;
            cur_channel->child_joint_name = anim_channel->target_node->name;
            // Apparently I have to find it myself, TODO: just do a hashmap look up by name, not a big deal
            // cur_channel->child_joint_reference_count = anim_channel->target_node.
            cur_channel->child_joint_reference_count = cgltf_node_index(data, anim_channel->target_node); // TODO: TEST
            cur_channel->sampler_idx = cgltf_animation_sampler_index(anim_data, anim_channel->sampler); // TODO: TEST
        }

        for (size_t sampler_idx = 0; sampler_idx < sampler_count; sampler_idx++)
        {
            Animation_Sampler* cur_sampler = &out_animation_data->samplers[sampler_idx];
            cgltf_animation_sampler* anim_sampler = &anim_data->samplers[sampler_idx];

            cur_sampler->interpolation_type = anim_sampler->interpolation;

            //read gltf input data
            cur_sampler->timestamps_count = anim_sampler->input->count;
            cur_sampler->timestamps = arena_alloc(arena, sizeof(float) * cur_sampler->timestamps_count);
            //get the timestamp data from the view_data and copy it into the timestamps
            const uint8_t* timestamp_buffer_data = cgltf_buffer_view_data(anim_sampler->input->buffer_view);
            memcpy(cur_sampler->timestamps, timestamp_buffer_data, cur_sampler->timestamps_count * sizeof(float));

            //read gltf output data
            cur_sampler->trs_interpolation_count = anim_sampler->output->count;
            const uint8_t* trs_buffer_data = cgltf_buffer_view_data(anim_sampler->output->buffer_view);
            switch (anim_sampler->output->type)
            {
            //these are the only supported formats in the gltf spec for samplers
            case cgltf_type_scalar: // weights
                cur_sampler->trs_interpolation_values_v3 = arena_alloc(arena, sizeof(float) * cur_sampler->trs_interpolation_count);
                memcpy(cur_sampler->trs_interpolation_values_f, trs_buffer_data,
                       cur_sampler->trs_interpolation_count * sizeof(float));
                break;
            case cgltf_type_vec3: // translation, scale
                cur_sampler->trs_interpolation_values_v3 = arena_alloc(arena, sizeof(vec3) * cur_sampler->trs_interpolation_count);
                memcpy(cur_sampler->trs_interpolation_values_v3, trs_buffer_data,
                       cur_sampler->trs_interpolation_count * sizeof(float));
                break;
            case cgltf_type_vec4: // rotation
                cur_sampler->trs_interpolation_values_v3 = arena_alloc(arena, sizeof(vec4) * cur_sampler->trs_interpolation_count);
                memcpy(cur_sampler->trs_interpolation_values_v4, trs_buffer_data,
                       cur_sampler->trs_interpolation_count * sizeof(float));
                break;
            default:
                FATAL("UNSUPPORTED CGLTF SAMPLER TYPE");
                break;
            }
        }
    }


    cgltf_free(data);
}


void mesh_load_fbx(Mesh_System* mesh_system, const char* fbx_path, Arena* arena, Frame_Arena* frame_arena)
{
    if (!c_string_path_is_extension(fbx_path, ".fbx"))
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


    ufbx_free_scene(scene);

    mesh_system->static_mesh_array[mesh_system->static_mesh_array_size] = *out_static_mesh;
    mesh_system->static_mesh_array_size++;
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
    out_mesh_packet->system_name = "Mesh System";

    //mesh_data
    out_mesh_packet->static_mesh_array = mesh_system->static_mesh_array;
    out_mesh_packet->static_mesh_array_size = mesh_system->static_mesh_array_size;
    out_mesh_packet->static_mesh_submesh_size = mesh_system->static_mesh_submesh_size;
    return true;
}


void static_mesh_to_madness_mesh(static_mesh* s_mesh, const char* file_name, Frame_Arena* frame_arena)
{
    //TODO: in general this is for another time, when i need the performance and i know what my engine should look like

    //TODO: so the problem is how i want to structure the data, if we even want to allow submeshes, probably
    const char* testing_asset_format_file_path = "../renderer/asset_format/";
    const char* final_path = c_string_concat(testing_asset_format_file_path, file_name, NULL);
    FILE* fptr = fopen(final_path, "wb");

    fseek(fptr, 0, SEEK_END);
    u64 file_size = ftell(fptr);
    fseek(fptr, 0, SEEK_SET);


    for (u64 i = 0; i < s_mesh->mesh_size; i++)
    {
        s_mesh->mesh[i].vertex_bytes;
        s_mesh->mesh[i].indices_bytes;
        s_mesh->mesh[i].normal_bytes;
        s_mesh->mesh[i].tangent_bytes;
        s_mesh->mesh[i].uv_bytes;

        ////////////////
    }

    // fwrite(); for writing binary data to a file


    fclose(fptr);
}
