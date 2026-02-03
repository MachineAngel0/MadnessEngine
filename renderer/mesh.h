#ifndef MESH_H
#define MESH_H

#include "shader_system.h"
#include "vk_buffer.h"



typedef struct
{
    //vertex_data
    bool vertex;
    bool index;

    //texture
    bool color;
    bool albedo_texture;
    bool normal_texture;

    /* TODO:
    //effects
    bool burn_effect;
    bool wave_effect;*/
} pipeline_mesh_config;

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


Mesh_System* mesh_system_init(renderer* renderer)
{
    Mesh_System* out_system = arena_alloc(&renderer->arena, sizeof(Mesh_System));


    return out_system;
}


void mesh_system_begin_frame(renderer* renderer, Mesh_System* mesh_system)
{
    //clear all the data
}

void mesh_system_end_frame(renderer* renderer, Mesh_System* mesh_system)
{
}

void mesh_system_generate_draw_data(renderer* renderer, Mesh_System* mesh_system)
{
    //global_mesh_data, stores all our mesh data,
    //check the mesh pipeline type, then add to the corresponding pipeline type

    //for the mesh data descriptor set, we will have a few buffers
    // vertex, index, uv, normal, tangent, material, transforms
    // these need to be constructed every frame, so that all the data matches up with the index it is retrieved from

    u64 index_cumulative_offset = 0;
    u64 vertex_cumulative_offset = 0;

    //get the number of meshes we have and then allocate that amount for the indirect draw data
    size_t indirect_draw_array_size = 0;
    size_t vertex_size = 0;
    size_t indices_size = 0;
    size_t normals_size = 0;
    size_t tangent_size = 0;
    size_t uv_size = 0;
    for (size_t s_mesh_idx = 0; s_mesh_idx < mesh_system->static_mesh_size; s_mesh_idx++)
    {
        indirect_draw_array_size += mesh_system->static_meshes[s_mesh_idx].mesh_size;
        for (size_t submesh_idx = 0; submesh_idx < mesh_system->static_meshes[s_mesh_idx].mesh_size; submesh_idx++)
        {
            vertex_size += mesh_system->static_meshes[s_mesh_idx].mesh[submesh_idx].vertex_bytes;
            indices_size += mesh_system->static_meshes[s_mesh_idx].mesh[submesh_idx].indices_bytes;
            normals_size += mesh_system->static_meshes[s_mesh_idx].mesh[submesh_idx].normal_bytes;
            tangent_size += mesh_system->static_meshes[s_mesh_idx].mesh[submesh_idx].tangent_bytes;
            uv_size += mesh_system->static_meshes[s_mesh_idx].mesh[submesh_idx].uv_bytes;
        }
    }

    VkDrawIndexedIndirectCommand* indirect_draw_array = arena_alloc(&renderer->frame_arena,
                                                                    indirect_draw_array_size * sizeof(
                                                                        VkDrawIndexedIndirectCommand));


    size_t current_indirect_index = 0;
    for (size_t s_mesh_idx = 0; s_mesh_idx < mesh_system->static_mesh_size; s_mesh_idx++)
    {
        for (size_t submesh_idx = 0; submesh_idx < mesh_system->static_meshes[s_mesh_idx].mesh_size; submesh_idx++)
        {
            mesh* current_mesh = &mesh_system->static_meshes[s_mesh_idx].mesh[submesh_idx];

            //
            current_mesh->vertices.pos;
            current_mesh->vertices.normal;
            current_mesh->vertices.tangent;
            current_mesh->vertices.uv;

            //build the indirect draw info
            indirect_draw_array[current_indirect_index].firstIndex = index_cumulative_offset;
            indirect_draw_array[current_indirect_index].firstInstance = 0;
            indirect_draw_array[current_indirect_index].indexCount = current_mesh->indices_count;
            indirect_draw_array[current_indirect_index].instanceCount = 1;
            indirect_draw_array[current_indirect_index].vertexOffset = vertex_cumulative_offset / sizeof(vec3);

            index_cumulative_offset += current_mesh->indices_count;
            vertex_cumulative_offset += current_mesh->vertex_bytes;

            current_indirect_index += 1;
        }
    }


    //TODO: upload into the buffers
    //get buffers
    vulkan_buffer_cpu* indirect_buffer = vulkan_buffer_cpu_get(renderer, mesh_system->indirect_buffer_handle);
    vulkan_buffer_cpu_data_copy_from_offset(renderer, indirect_buffer, indirect_draw_array,
                                            indirect_draw_array_size * sizeof(VkDrawIndexedIndirectCommand));




}


void mesh_system_generate_permutations(renderer* renderer, Mesh_System* mesh_system,
                                       pipeline_mesh_config* pipeline_config)
{
    //global_mesh_data, stores all our mesh data,
    //check the mesh pipeline type, then add to the corresponding pipeline type
    //add

    //points to a pipeline
    pipeline_mesh_config* global_configs = NULL;
    u32 config_size = 0;


    for (int i = 0; i < config_size; i++)
    {
        if (global_configs->color != pipeline_config->color)
        {
            continue;
        }
        if (global_configs->albedo_texture != pipeline_config->albedo_texture)
        {
            continue;
        }

        //if we found a matching one
        // return []->pipeline_handle;
        return;
    }
}


#endif
