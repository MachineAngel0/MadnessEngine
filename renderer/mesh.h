#ifndef MESH_H
#define MESH_H

#include "shader_system.h"
#include "str_builder.h"
#include "vulkan_types.h"

mesh* mesh_init(Arena* arena)
{
    mesh* m = arena_alloc(arena, sizeof(mesh));

    // m->vertices.color = darray_create(vec4);
    m->index_type = VK_INDEX_TYPE_UINT32;

    return m;
}

static_mesh* static_mesh_init(Arena* arena, u32 mesh_size)
{
    static_mesh* out_static_mesh = arena_alloc(arena, sizeof(mesh));
    out_static_mesh->mesh_size = mesh_size;
    // for (u32 i = 0; i < mesh_size; i++)
    // {
    // out_static_mesh[i].mesh = arena_alloc(arena, sizeof(mesh));
    // out_static_mesh[i].indirect_draw_array = arena_alloc(arena, sizeof(VkDrawIndexedIndirectCommand));
    // }
    out_static_mesh->mesh = arena_alloc(arena, sizeof(mesh) * mesh_size);
    out_static_mesh->indirect_draw_array = arena_alloc(arena, sizeof(VkDrawIndexedIndirectCommand) * mesh_size);


    return out_static_mesh;
}


void mesh_free(mesh* m)
{
    darray_free(m->vertices.pos);
    // darray_free(m->vertices.normal);
    // darray_free(m->vertices.tex_coord);
    darray_free(m->indices);
    // darray_free(m->textures);
    free(m);
}


static_mesh* mesh_load_gltf(renderer* renderer, const char* gltf_path)
{
    renderer->context.device.properties.limits.maxPerStageDescriptorUniformBuffers;

    cgltf_options options = {0};
    cgltf_data* data = NULL;
    cgltf_result result = cgltf_parse_file(&options, gltf_path, &data);

    if (result != cgltf_result_success)
    {
        fprintf(stderr, "Failed to parse glTF file: %s\n", gltf_path);
        return NULL;
    }

    result = cgltf_load_buffers(&options, data, gltf_path);
    MASSERT(result == cgltf_result_success)

    static_mesh* out_static_mesh = static_mesh_init(&renderer->arena, data->meshes_count);


    // GET BASE PATH

    char* base_path = NULL;
    int i = strlen(gltf_path);
    for (; i > 0; i--)
    {
        if (gltf_path[i] == '/')
        {
            base_path = arena_alloc(&renderer->frame_arena, i + 2);
            memcpy(base_path, gltf_path, i + 1);
            base_path[i + 1] = '\0';

            break;
        }
    }

    /*

    hash_map_string* path_to_handle = HASH_MAP_STRING_CREATE_DEFAULT_SIZE(shader_handle);

    for (size_t texture_index = 0; texture_index < data->textures_count; texture_index++)
    {
        const char* texture_uri = data->textures[texture_index].image->uri;

        char* texture_path = arena_alloc(&renderer->frame_arena, strlen(base_path) + strlen(texture_uri));
        // takes a buffer, message format, then the remaining strings
        sprintf(texture_path, "%s%s", base_path, texture_uri);
        printf("Texture Path:  %s\n", texture_path);


        out_static_mesh->material_handles[texture_index] = shader_system_add_texture(
            &renderer->context, renderer->shader_system,
            texture_path);

        hash_map_string_insert(path_to_handle, texture_path, &out_static_mesh->material_handles[texture_index]);

    }

    hash_map_string_print(path_to_handle, print_int);
    */


    //LOAD VERTEX/INDEX DATA

    for (size_t mesh_idx = 0; mesh_idx < data->meshes_count; mesh_idx++)
    {
        /* Find position accessor */
        const cgltf_accessor* pos_accessor = cgltf_find_accessor(data->meshes[mesh_idx].primitives,
                                                                 cgltf_attribute_type_position,
                                                                 0);
        //this makes no sense if there isn't one
        if (pos_accessor)
        {
            //get size information
            cgltf_size num_floats = cgltf_accessor_unpack_floats(pos_accessor, NULL, 0);
            cgltf_size float_bytes = num_floats * sizeof(float);
            out_static_mesh->mesh[mesh_idx].vertex_bytes = float_bytes;

            //alloc and copy data
            float* pos_data = arena_alloc(&renderer->frame_arena, float_bytes);
            out_static_mesh->mesh[mesh_idx].vertices.pos = arena_alloc(&renderer->arena, float_bytes);
            cgltf_accessor_unpack_floats(pos_accessor, pos_data, num_floats);
            memcpy(out_static_mesh->mesh[mesh_idx].vertices.pos, pos_data, float_bytes);
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
            out_static_mesh->mesh[mesh_idx].normal_bytes = norm_bytes;

            //alloc and copy data
            float* normal_data = arena_alloc(&renderer->arena, norm_bytes);
            out_static_mesh->mesh[mesh_idx].vertices.normal = arena_alloc(&renderer->arena, norm_bytes);
            cgltf_accessor_unpack_floats(norm_accessor, normal_data, norm_floats);
            memcpy(out_static_mesh->mesh[mesh_idx].vertices.normal, normal_data, norm_bytes);
        }

        //  Find tangent accessor
        const cgltf_accessor* tangent_accessor = cgltf_find_accessor(data->meshes[mesh_idx].primitives,
                                                                     cgltf_attribute_type_tangent, 0);

        if (tangent_accessor)
        {
            //get size information
            cgltf_size tangent_floats = cgltf_accessor_unpack_floats(tangent_accessor, NULL, 0);
            cgltf_size tangent_bytes = tangent_floats * sizeof(float);
            out_static_mesh->mesh[mesh_idx].tangent_bytes = tangent_bytes;


            //alloc and copy data
            float* tangent_data = arena_alloc(&renderer->arena, tangent_bytes);
            out_static_mesh->mesh[mesh_idx].vertices.tangent = arena_alloc(&renderer->arena, tangent_bytes);
            cgltf_accessor_unpack_floats(tangent_accessor, tangent_data, tangent_floats);
            memcpy(out_static_mesh->mesh[mesh_idx].vertices.tangent, tangent_data, tangent_bytes);
        }

        //  Find texcoord accessor
        const cgltf_accessor* texcoord_accessor = cgltf_find_accessor(data->meshes[mesh_idx].primitives,
                                                                      cgltf_attribute_type_texcoord, 0);
        if (texcoord_accessor)
        {
            //get size information
            cgltf_size uv_floats_size = cgltf_accessor_unpack_floats(texcoord_accessor, NULL, 0);
            cgltf_size uv_byte_size = uv_floats_size * sizeof(float);
            out_static_mesh->mesh[mesh_idx].uv_bytes = uv_byte_size;


            //alloc and copy data
            float* uv_data = arena_alloc(&renderer->arena, uv_byte_size);
            cgltf_accessor_unpack_floats(texcoord_accessor, uv_data, uv_floats_size);
            out_static_mesh->mesh[mesh_idx].vertices.uv = arena_alloc(&renderer->arena, uv_byte_size);
            memcpy(out_static_mesh->mesh[mesh_idx].vertices.uv, uv_data, uv_byte_size);
        }

        // Load indices
        // SEE componentType in the specs for more detail 3.6.2
        u8 index_stride = data->meshes[mesh_idx].primitives[0].indices->stride;
        if (index_stride == 2)
        {
            out_static_mesh->mesh[mesh_idx].index_type = VK_INDEX_TYPE_UINT16;
        }
        else if (index_stride == 4)
        {
            out_static_mesh->mesh[mesh_idx].index_type = VK_INDEX_TYPE_UINT32;
        }
        else
        {
            WARN("GLTF MESH LOADING: UNKNOWN INDEX TYPE STRIDE");
            out_static_mesh->mesh[mesh_idx].index_type = VK_INDEX_TYPE_UINT32;
        }
        out_static_mesh->mesh[mesh_idx].indices_count = data->meshes[mesh_idx].primitives->indices->count;
        out_static_mesh->mesh[mesh_idx].indices_bytes = data->meshes[mesh_idx].primitives->indices->count *
            index_stride;
        //TODO: there can be multiple primitices/indices, will come back to
        out_static_mesh->mesh[mesh_idx].indices = arena_alloc(&renderer->arena,
                                                              out_static_mesh->mesh[mesh_idx].indices_bytes);

        cgltf_accessor_unpack_indices(data->meshes[mesh_idx].primitives->indices,
                                      out_static_mesh->mesh[mesh_idx].indices,
                                      index_stride,
                                      out_static_mesh->mesh[mesh_idx].indices_count);

        //LOAD TEXTURES/MATERIALS

        //BASE COLOR
        const char* texture_uri = data->meshes[mesh_idx].primitives->material->pbr_metallic_roughness.base_color_texture
                                                        .texture->image->uri;
        if (texture_uri)
        {
            char* texture_path = arena_alloc(&renderer->frame_arena, strlen(base_path) + strlen(texture_uri));
            // takes a buffer, message format, then the remaining strings
            sprintf(texture_path, "%s%s", base_path, texture_uri);
            TRACE("Texture Path:  %s", texture_path);


            out_static_mesh->mesh[mesh_idx].color_texture = shader_system_add_texture(
                &renderer->context, renderer->shader_system,
                texture_path);
        }
    }

    //load materials param data
    // if (data->meshes[0].primitives->material) {
    //     int mat_idx = cgltf_material_index(data, data->meshes[0].primitives->material);
    //     int hi = 0;
    // }
    // cgltf_texture_index();


    cgltf_free(data);
    return out_static_mesh;
}


static_mesh* mesh_load_gltf_indirect(renderer* renderer, const char* gltf_path)
{
    cgltf_options options = {0};
    cgltf_data* data = NULL;
    cgltf_result result = cgltf_parse_file(&options, gltf_path, &data);

    if (result != cgltf_result_success)
    {
        fprintf(stderr, "Failed to parse glTF file: %s\n", gltf_path);
        return NULL;
    }

    result = cgltf_load_buffers(&options, data, gltf_path);
    MASSERT(result == cgltf_result_success)

    static_mesh* out_static_mesh = static_mesh_init(&renderer->arena, data->meshes_count);


    // GET BASE PATH
    char* base_path = c_string_path_strip(gltf_path, &renderer->frame_arena);


    //LOAD VERTEX/INDEX DATA
    u64 index_cumulative_offset = 0;
    u64 vertex_cumulative_offset = 0;

    for (size_t mesh_idx = 0; mesh_idx < data->meshes_count; mesh_idx++)
    {
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
            out_static_mesh->mesh[mesh_idx].vertex_bytes = float_bytes;


            //alloc and copy data
            float* pos_data = arena_alloc(&renderer->frame_arena, float_bytes);
            out_static_mesh->mesh[mesh_idx].vertices.pos = arena_alloc(&renderer->arena, float_bytes);
            cgltf_accessor_unpack_floats(pos_accessor, pos_data, num_floats);
            memcpy(out_static_mesh->mesh[mesh_idx].vertices.pos, pos_data, float_bytes);
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
            out_static_mesh->mesh[mesh_idx].normal_bytes = norm_bytes;

            //alloc and copy data
            float* normal_data = arena_alloc(&renderer->arena, norm_bytes);
            out_static_mesh->mesh[mesh_idx].vertices.normal = arena_alloc(&renderer->arena, norm_bytes);
            cgltf_accessor_unpack_floats(norm_accessor, normal_data, norm_floats);
            memcpy(out_static_mesh->mesh[mesh_idx].vertices.normal, normal_data, norm_bytes);
        }

        //  Find tangent accessor
        const cgltf_accessor* tangent_accessor = cgltf_find_accessor(data->meshes[mesh_idx].primitives,
                                                                     cgltf_attribute_type_tangent, 0);

        if (tangent_accessor)
        {
            //get size information
            cgltf_size tangent_floats = cgltf_accessor_unpack_floats(tangent_accessor, NULL, 0);
            cgltf_size tangent_bytes = tangent_floats * sizeof(float);
            out_static_mesh->mesh[mesh_idx].tangent_bytes = tangent_bytes;


            //alloc and copy data
            float* tangent_data = arena_alloc(&renderer->arena, tangent_bytes);
            out_static_mesh->mesh[mesh_idx].vertices.tangent = arena_alloc(&renderer->arena, tangent_bytes);
            cgltf_accessor_unpack_floats(tangent_accessor, tangent_data, tangent_floats);
            memcpy(out_static_mesh->mesh[mesh_idx].vertices.tangent, tangent_data, tangent_bytes);
        }

        //  Find texcoord accessor
        const cgltf_accessor* texcoord_accessor = cgltf_find_accessor(data->meshes[mesh_idx].primitives,
                                                                      cgltf_attribute_type_texcoord, 0);
        if (texcoord_accessor)
        {
            //get size information
            cgltf_size uv_floats_count = cgltf_accessor_unpack_floats(texcoord_accessor, NULL, 0);
            cgltf_size uv_byte_size = uv_floats_count * sizeof(float);
            out_static_mesh->mesh[mesh_idx].uv_bytes = uv_byte_size;


            //alloc and copy data
            float* uv_data = arena_alloc(&renderer->frame_arena, uv_byte_size);
            cgltf_accessor_unpack_floats(texcoord_accessor, uv_data, uv_floats_count);

            out_static_mesh->mesh[mesh_idx].vertices.uv = arena_alloc(&renderer->arena, uv_byte_size);
            memcpy(out_static_mesh->mesh[mesh_idx].vertices.uv, uv_data, uv_byte_size);
        }

        // Load indices
        // SEE componentType in the specs for more detail 3.6.2
        u8 index_stride = data->meshes[mesh_idx].primitives[0].indices->stride;
        if (index_stride == 2)
        {
            out_static_mesh->mesh[mesh_idx].index_type = VK_INDEX_TYPE_UINT16;
        }
        else if (index_stride == 4)
        {
            out_static_mesh->mesh[mesh_idx].index_type = VK_INDEX_TYPE_UINT32;
        }
        else
        {
            WARN("GLTF MESH LOADING: UNKNOWN INDEX TYPE STRIDE");
        }


        out_static_mesh->mesh[mesh_idx].indices_count = data->meshes[mesh_idx].primitives->indices->count;
        out_static_mesh->mesh[mesh_idx].indices_bytes = data->meshes[mesh_idx].primitives->indices->count *
            index_stride;
        //TODO: there can be multiple primitices/indices, will come back to
        out_static_mesh->mesh[mesh_idx].indices = arena_alloc(&renderer->arena,
                                                              out_static_mesh->mesh[mesh_idx].indices_bytes);

        const uint8_t* index_buffer_data = cgltf_buffer_view_data(
            data->meshes[mesh_idx].primitives->indices->buffer_view);
        memcpy(out_static_mesh->mesh[mesh_idx].indices, index_buffer_data,
               out_static_mesh->mesh[mesh_idx].indices_bytes);
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
        if (color_texture)
        {
            out_static_mesh->mesh[mesh_idx].mesh_pipeline_mask |= MESH_PIPELINE_COLOR;

            char* texture_path = arena_alloc(&renderer->frame_arena,
                                             strlen(base_path) + strlen(color_texture->image->uri));
            // takes a buffer, message format, then the remaining strings
            sprintf(texture_path, "%s%s", base_path, color_texture->image->uri);
            TRACE("COLOR Texture Path:  %s", texture_path);

            out_static_mesh->mesh[mesh_idx].color_texture = shader_system_add_texture(
                &renderer->context, renderer->shader_system,
                texture_path);
        }

        //METAL-ROUGHNESS
        cgltf_texture* metal_roughness_texture = data->meshes[mesh_idx].primitives->material->pbr_metallic_roughness.
                                                                        metallic_roughness_texture.texture;
        if (metal_roughness_texture)
        {
            out_static_mesh->mesh[mesh_idx].mesh_pipeline_mask |= MESH_PIPELINE_ROUGHNESS;
            out_static_mesh->mesh[mesh_idx].mesh_pipeline_mask |= MESH_PIPELINE_METALLIC;
            char* texture_path = arena_alloc(&renderer->frame_arena,
                                             strlen(base_path) + strlen(metal_roughness_texture->image->uri));
            // takes a buffer, message format, then the remaining strings
            sprintf(texture_path, "%s%s", base_path, metal_roughness_texture->image->uri);
            TRACE("METAL/ROUGHNESS Texture Path:  %s", texture_path);
        }

        // AO
        //NOTE: this material in theory can be included in the pbr-metal-roughness texture, in which case, just return a handle
        cgltf_texture* AO_texture = data->meshes[mesh_idx].primitives->material->occlusion_texture.texture;
        if (AO_texture)
        {
            out_static_mesh->mesh[mesh_idx].mesh_pipeline_mask |= MESH_PIPELINE_ROUGHNESS;
            out_static_mesh->mesh[mesh_idx].mesh_pipeline_mask |= MESH_PIPELINE_METALLIC;
            char* texture_path = arena_alloc(&renderer->frame_arena,
                                             strlen(base_path) + strlen(AO_texture->image->uri));
            // takes a buffer, message format, then the remaining strings
            sprintf(texture_path, "%s%s", base_path, AO_texture->image->uri);
            TRACE("AO Texture Path:  %s", texture_path);
        }


        //NORMAL TEXTURE
        // data->meshes[mesh_idx].primitives->material->has_pbr_metallic_roughness
        cgltf_texture* normal_texture = data->meshes[mesh_idx].primitives->material->normal_texture.texture;
        if (normal_texture)
        {
            out_static_mesh->mesh[mesh_idx].mesh_pipeline_mask |= MESH_PIPELINE_NORMAL;
            char* texture_path = arena_alloc(&renderer->frame_arena,
                                             strlen(base_path) + strlen(normal_texture->image->uri));
            // takes a buffer, message format, then the remaining strings
            sprintf(texture_path, "%s%s", base_path, normal_texture->image->uri);
            TRACE("NORMAL Texture Path:  %s", texture_path);
        }

        //EMISSIVE TEXTURE
        cgltf_texture* emissive_texture = data->meshes[mesh_idx].primitives->material->emissive_texture.texture;
        if (emissive_texture)
        {
            out_static_mesh->mesh[mesh_idx].mesh_pipeline_mask |= MESH_PIPELINE_EMISSIVE;
            char* texture_path = arena_alloc(&renderer->frame_arena,
                                             strlen(base_path) + strlen(emissive_texture->image->uri));
            // takes a buffer, message format, then the remaining strings
            sprintf(texture_path, "%s%s", base_path, emissive_texture->image->uri);
            TRACE("EMISSIVE Texture Path:  %s", texture_path);
        }


        out_static_mesh->indirect_draw_array[mesh_idx].firstIndex = index_cumulative_offset;
        out_static_mesh->indirect_draw_array[mesh_idx].firstInstance = 0;
        out_static_mesh->indirect_draw_array[mesh_idx].indexCount = data->meshes[mesh_idx].primitives->indices->count;
        out_static_mesh->indirect_draw_array[mesh_idx].instanceCount = 1;
        out_static_mesh->indirect_draw_array[mesh_idx].vertexOffset = vertex_cumulative_offset / sizeof(vec3);

        index_cumulative_offset += data->meshes[mesh_idx].primitives->indices->count;
        vertex_cumulative_offset += out_static_mesh->mesh[mesh_idx].vertex_bytes;

        //TODO: find out the objects pipeline type
    }

    //load materials
    // if (data->meshes[0].primitives->material) {
    //     int mat_idx = cgltf_material_index(data, data->meshes[0].primitives->material);
    //     int hi = 0;
    // }
    // cgltf_texture_index();


    cgltf_free(data);
    return out_static_mesh;
}

/*
static_mesh* mesh_load_fbx(renderer* renderer, const char* gltf_path)
{

    //NOTE: use this when ready to test
    mesh_load_fbx(&renderer_internal, "../z_assets/models/mug_fbx/teamugfbx.fbx");

    // https://github.com/ufbx/ufbx?tab=readme-ov-file - github
    // https://ufbx.github.io/ - online docs

    ufbx_load_opts opts = { 0 }; // Optional, pass NULL for defaults
    ufbx_error error; // Optional, pass NULL if you don't care about errors
    ufbx_scene *scene = ufbx_load_file("thing.fbx", &opts, &error);
    if (!scene) {
        fprintf(stderr, "Failed to load: %s\n", error.description.data);
        exit(1);
    }

    // Use and inspect `scene`, it's just plain data!
    // Let's just list all objects within the scene for example:
    for (size_t i = 0; i < scene->nodes.count; i++) {
        ufbx_node *node = scene->nodes.data[i];
        if (node->is_root) continue;

        printf("Object: %s\n", node->name.data);
        if (node->mesh) {
            printf("-> mesh with %zu faces\n", node->mesh->faces.count);
        }
    }



    static_mesh* out_static_mesh = static_mesh_init(&renderer->arena, scene->nodes.count);

    // GET BASE PATH

    char* base_path = NULL;
    int i = strlen(gltf_path);
    for (; i > 0; i--)
    {
        if (gltf_path[i] == '/')
        {
            base_path = arena_alloc(&renderer->frame_arena, i + 2);
            memcpy(base_path, gltf_path, i + 1);
            base_path[i + 1] = '\0';

            break;
        }
    }

    ufbx_free_scene(scene);
    return out_static_mesh;

}
*/
/*
mesh* mesh_load_obj(const char* obj_path)
{
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


    mesh* out_mesh = mesh_init();

    size_t file_size;
    FILE* mesh_obj_file = fopen("../z_assets/models/car_obj/falcon.obj", "rb");
    if (!mesh_obj_file)
    {
        FATAL("Failed to open obj file");
        return NULL;
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
            darray_push(out_mesh->vertices.pos, v);
        }
        if (strncmp(buffer, "vt ", 3) == 0)
        {
            vec2 v;
            sscanf(buffer, "v %f %f", &v.x, &v.y);
            DEBUG("v %f %f %f", v.x, v.y);
            darray_push(out_mesh->vertices.tex_coord, v);
        }
        if (strncmp(buffer, "vn ", 3) == 0)
        {
            vec3 v;
            sscanf(buffer, "v %f %f %f", &v.x, &v.y, &v.z);
            DEBUG("v %f %f %f", v.x, v.y, v.z);
            darray_push(out_mesh->vertices.normal, v);
        }
    }

    fclose(mesh_obj_file);

    return out_mesh;
}
*/


Mesh_Pipeline_Permutations* generate_mesh_permutations(renderer* renderer)
{
    int permutation_count = MESH_PIPELINE_ENUM_MAX - 1;

    Mesh_Pipeline_Permutations* mesh_perms =
        arena_alloc(&renderer->arena, sizeof(Mesh_Pipeline_Permutations));
    mesh_perms->permutation_keys = arena_alloc(&renderer->arena, permutation_count * sizeof(u32));
    mesh_perms->debug_shader_name = arena_alloc(&renderer->arena, permutation_count * sizeof(String*));
    mesh_perms->permutations_count = permutation_count;


    int mesh_perm_index = 0;
    for (uint32_t mask = 0; mask < permutation_count; mask++)
    {
        String_Builder* str_builder = string_builder_create(256);
        string_builder_append_char(str_builder, "Mesh Shader", sizeof("Mesh Shader") - 1);

        for (int i = 0; i < MESH_PIPELINE_ENUM_COUNT; i++)
        {
            if (mask & (1 << i))
            {
                // string_builder_append_char(str_builder, "_", strlen("_"));
                // string_builder_append_char(str_builder,  (char*)mesh_pipeline_flag_names[i], strlen(mesh_pipeline_flag_names[i]));

                string_builder_append_char(str_builder, "_", strlen("_"));
                string_builder_append_string(str_builder, &mesh_pipeline_flag_names_string[i]);
            }
        }

        mesh_perms->permutation_keys[mesh_perm_index] = mask;
        mesh_perms->debug_shader_name[mesh_perm_index] = *string_builder_to_string(str_builder);

        mesh_perm_index++;
    }

    return mesh_perms;

    /*
    //TODO: replace with my own custom string format

    int permutation_count = MESH_PIPELINE_ENUM_MAX -1;

    Mesh_Pipeline_Permutations* mesh_perms =
        arena_alloc(&renderer->arena, sizeof(Mesh_Pipeline_Permutations));
    mesh_perms->permutation_keys = arena_alloc(&renderer->arena, permutation_count * sizeof(u32));
    mesh_perms->debug_shader_name = arena_alloc(&renderer->arena, permutation_count * sizeof(const char*));
    mesh_perms->permutations_count = permutation_count;


    int mesh_perm_index = 0;
    for (uint32_t mask = 0; mask < permutation_count; mask++)
    {
        char name[256] = "shader";

        for (int i = 0; i < MESH_PIPELINE_ENUM_COUNT; i++)
        {
            if (mask & (1 << i))
            {
                strncat(name, "_", sizeof(name) - strlen(name) - 1);
                strncat(name, mesh_pipeline_flag_names[i],
                        sizeof(name) - strlen(name) - 1);
            }
        }

        mesh_perms->permutation_keys[mesh_perm_index] = mask;
        mesh_perms->debug_shader_name[mesh_perm_index] = strdup(name);

        mesh_perm_index++;
    }

    return mesh_perms;
    */
}

Mesh_System* mesh_system_init(renderer* renderer)
{
    Mesh_System* out_mesh_system = arena_alloc(&renderer->arena, sizeof(Mesh_System));
    out_mesh_system->mesh_permutations = generate_mesh_permutations(renderer);

    //TODO: create buffers and get a pool arena for meshes

    return out_mesh_system;
}


void mesh_system_generate_draw_data(renderer* renderer, Mesh_System* mesh_system)
{
    // for pipeline_permutation (each permutation is an index)
    // modify mesh data UBO for uniform branching on the permutation state
    // send data in specified permutation -> mesh_draw_data[permutation_index]


    // global static meshes (one large pool) -> meshes (multiple of them) -> permutation type
    // add to draw data, at the premutation index

    //global mesh data -> mesh1 -> mesh 2 -> mesh2
    // go from global data -> buffers data
    //                 permutation 1     |permutation 2
    //vertex buffer -> mesh3 -> mesh 2 -> mesh1
    //uv storage buffer -> mesh3 -> mesh 2 -> mesh1
    //material storage buffer -> mesh3 -> mesh 2 -> mesh1

    mesh_system->draw_data = arena_alloc(&renderer->frame_arena,
                                         mesh_system->mesh_permutations->permutations_count * sizeof(
                                             mesh_permutation_draw_data));
    for (int i = 0; i < mesh_system->mesh_permutations->permutations_count; i++)
    {
        mesh_system->draw_data[i].indirect_draw_permutation = darray_create(VkDrawIndexedIndirectCommand);
    }

    //NOTE: in theory, this could just get tracked whenever any mesh is added or removed
    size_t vertex_size = 0;
    size_t indices_size = 0;
    size_t normals_size = 0;
    size_t tangent_size = 0;
    size_t uv_size = 0;

    for (size_t s_mesh_idx = 0; s_mesh_idx < mesh_system->static_mesh_size; s_mesh_idx++)
    {
        for (size_t submesh_idx = 0; submesh_idx < mesh_system->static_meshes[s_mesh_idx].mesh_size; submesh_idx++)
        {
            mesh* current_mesh = &mesh_system->static_meshes[s_mesh_idx].mesh[submesh_idx];

            vertex_size += current_mesh->vertex_bytes;
            indices_size += current_mesh->indices_bytes;
            normals_size += current_mesh->normal_bytes;
            tangent_size += current_mesh->tangent_bytes;
            uv_size += current_mesh->uv_bytes;
        }
    }
    //TODO: clear any old data

    vec3* vertex_data = arena_alloc(&renderer->arena, vertex_size);
    u32* index_data = arena_alloc(&renderer->arena, indices_size);
    vec3* normals_data = arena_alloc(&renderer->arena, normals_size);
    vec4* tangent_data = arena_alloc(&renderer->arena, tangent_size);
    vec2* uv_data = arena_alloc(&renderer->arena, uv_size);

    size_t vertex_offset = 0;
    size_t index_offset = 0;
    size_t normals_offset = 0;
    size_t tangent_offset = 0;
    size_t uv_offset = 0;


    VkDrawIndexedIndirectCommand current_indirect_draw_info;
    for (size_t s_mesh_idx = 0; s_mesh_idx < mesh_system->static_mesh_size; s_mesh_idx++)
    {
        for (size_t submesh_idx = 0; submesh_idx < mesh_system->static_meshes[s_mesh_idx].mesh_size; submesh_idx++)
        {
            mesh* current_mesh = &mesh_system->static_meshes[s_mesh_idx].mesh[submesh_idx];
            u32 idx = current_mesh->mesh_pipeline_mask;

            //
            current_mesh->vertices.pos;
            current_mesh->vertices.normal;
            current_mesh->vertices.tangent;
            current_mesh->vertices.uv;

            //build the indirect draw info
            current_indirect_draw_info.firstIndex = index_offset;
            current_indirect_draw_info.firstInstance = 0;
            current_indirect_draw_info.indexCount = current_mesh->indices_count;
            current_indirect_draw_info.instanceCount = 1;
            current_indirect_draw_info.vertexOffset = vertex_offset / sizeof(vec3);

            darray_push(mesh_system->draw_data[idx].indirect_draw_permutation, current_indirect_draw_info);

            memcpy(vertex_data + vertex_offset, current_mesh->vertices.pos, current_mesh->vertex_bytes);
            memcpy(index_data + index_offset, current_mesh->indices, current_mesh->indices_bytes);
            memcpy(normals_data + normals_offset, current_mesh->vertices.normal, current_mesh->normal_bytes);
            memcpy(tangent_data + tangent_offset, current_mesh->vertices.tangent, current_mesh->tangent_bytes);
            memcpy(uv_data + uv_offset, current_mesh->vertices.uv, current_mesh->uv_bytes);


            index_offset += current_mesh->indices_bytes;
            vertex_offset += current_mesh->vertex_bytes;
            normals_offset += current_mesh->normal_bytes;
            tangent_offset += current_mesh->tangent_bytes;
            uv_offset += current_mesh->uv_bytes;
        }
    }


    //TODO: upload into the buffers
    //get buffers
    vulkan_buffer_cpu* indirect_buffer = vulkan_buffer_cpu_get(renderer, mesh_system->indirect_buffer_handle);
    vulkan_buffer_cpu* vertex_buffer = vulkan_buffer_cpu_get(renderer, mesh_system->vertex_buffer_handle);
    vulkan_buffer_cpu* index_buffer_handle = vulkan_buffer_cpu_get(renderer, mesh_system->index_buffer_handle);
    vulkan_buffer_cpu* normal_buffer_handle = vulkan_buffer_cpu_get(renderer, mesh_system->normal_buffer_handle);
    vulkan_buffer_cpu* tangent_buffer_handle = vulkan_buffer_cpu_get(renderer, mesh_system->tangent_buffer_handle);
    vulkan_buffer_cpu* uv_buffer_handle = vulkan_buffer_cpu_get(renderer, mesh_system->uv_buffer_handle);


    for (int i = 0; i < mesh_system->mesh_permutations->permutations_count; i++)
    {
        vulkan_buffer_cpu_data_copy_from_offset(renderer, indirect_buffer,
                                                mesh_system->draw_data[i].indirect_draw_permutation,
                                                darray_get_size(mesh_system->draw_data[i].indirect_draw_permutation) *
                                                sizeof(
                                                    VkDrawIndexedIndirectCommand));
    }


    vulkan_buffer_cpu_data_copy_from_offset(renderer, vertex_buffer, vertex_data, vertex_offset);
    vulkan_buffer_cpu_data_copy_from_offset(renderer, index_buffer_handle, index_data, index_offset);
    vulkan_buffer_cpu_data_copy_from_offset(renderer, normal_buffer_handle, normals_data, normals_offset);
    vulkan_buffer_cpu_data_copy_from_offset(renderer, tangent_buffer_handle, tangent_data, tangent_offset);
    vulkan_buffer_cpu_data_copy_from_offset(renderer, uv_buffer_handle, uv_data, uv_offset);
}

void mesh_system_draw(renderer* renderer, Mesh_System* mesh_system, vulkan_command_buffer* command_buffer)
{
    //only bind the vertex and index, the storage buffers are in bindless
    vulkan_buffer_cpu* indirect_buffer = vulkan_buffer_cpu_get(renderer, mesh_system->indirect_buffer_handle);
    vulkan_buffer_cpu* vertex_buffer = vulkan_buffer_cpu_get(renderer, mesh_system->vertex_buffer_handle);
    vulkan_buffer_cpu* index_buffer_handle = vulkan_buffer_cpu_get(renderer, mesh_system->index_buffer_handle);


    VkDeviceSize pOffsets[] = {0};

    // vkCmdBindPipeline(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
    //                   renderer->indirect_pipeline.handle);
    //
    // vkCmdBindVertexBuffers(command_buffer->handle, 0, 1, &vertex_buffer->handle,
    //                    pOffsets);
    //
    // vkCmdBindIndexBuffer(command_buffer->handle, index_buffer_handle->handle, 0,
    //                      VK_INDEX_TYPE_UINT16);


    for (int i = 0; i < mesh_system->mesh_permutations->permutations_count; i++)
    {
        mesh_uniform_constants uniform_constants;
        uniform_constants.mask = i;

        memcpy(&mesh_system->uniform_buffer_permutation, &uniform_constants, sizeof(mesh_uniform_constants));

        if (renderer->context.device.features.multiDrawIndirect)
        {
            vkCmdDrawIndexedIndirect(command_buffer->handle,
                                     indirect_buffer->handle, 0,
                                     darray_get_size(mesh_system->draw_data[i].indirect_draw_permutation),
                                     sizeof(VkDrawIndexedIndirectCommand));
        }
        else
        {
            // If multi draw is not available, we must issue separate draw commands
            for (auto j = 0; j < darray_get_size(mesh_system->draw_data[i].indirect_draw_permutation; j++)
            {
                vkCmdDrawIndexedIndirect(command_buffer->handle,
                                         indirect_buffer->handle,
                                         j * sizeof(VkDrawIndexedIndirectCommand), 1,
                                         sizeof(VkDrawIndexedIndirectCommand));
            }
        }
    }
}


#endif
