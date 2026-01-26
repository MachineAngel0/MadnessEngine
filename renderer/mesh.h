#ifndef MESH_H
#define MESH_H

#include "vulkan_types.h"
#include "shader_system.h"


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
            memcpy( out_static_mesh->mesh[mesh_idx].vertices.tangent, tangent_data, tangent_bytes);

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

        char* texture_path = arena_alloc(&renderer->frame_arena, strlen(base_path) + strlen(texture_uri));
        // takes a buffer, message format, then the remaining strings
        sprintf(texture_path, "%s%s", base_path, texture_uri);
        TRACE("Texture Path:  %s", texture_path);


        out_static_mesh->mesh[mesh_idx].color_texture = shader_system_add_texture(
            &renderer->context, renderer->shader_system,
            texture_path);
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


    //LOAD VERTEX/INDEX DATA
    u64 index_cumulative_offset = 0;
    u64 vertex_cumulative_offset = 0;

    for (size_t mesh_idx = 0; mesh_idx < data->meshes_count; mesh_idx++)
    {

        /* Find position accessor */
        const cgltf_accessor* pos_accessor = cgltf_find_accessor(data->meshes[mesh_idx].primitives,
                                                                 cgltf_attribute_type_position,
                                                                 0);
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
            memcpy( out_static_mesh->mesh[mesh_idx].vertices.tangent, tangent_data, tangent_bytes);

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

        char* texture_path = arena_alloc(&renderer->frame_arena, strlen(base_path) + strlen(texture_uri));
        // takes a buffer, message format, then the remaining strings
        sprintf(texture_path, "%s%s", base_path, texture_uri);
        TRACE("Texture Path:  %s", texture_path);


        out_static_mesh->mesh[mesh_idx].color_texture = shader_system_add_texture(
            &renderer->context, renderer->shader_system,
            texture_path);



         out_static_mesh->indirect_draw_array[mesh_idx].firstIndex = index_cumulative_offset;
         out_static_mesh->indirect_draw_array[mesh_idx].firstInstance = 0;
         out_static_mesh->indirect_draw_array[mesh_idx].indexCount = data->meshes[mesh_idx].primitives->indices->count;
         out_static_mesh->indirect_draw_array[mesh_idx].instanceCount = 1;
         out_static_mesh->indirect_draw_array[mesh_idx].vertexOffset = vertex_cumulative_offset/sizeof(vec3);

        index_cumulative_offset += data->meshes[mesh_idx].primitives->indices->count;
        vertex_cumulative_offset +=  out_static_mesh->mesh[mesh_idx].vertex_bytes;

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


#endif
