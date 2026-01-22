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
    for (u32 i = 0; i < mesh_size; i++)
    {
        out_static_mesh[i].mesh = arena_alloc(arena, sizeof(mesh));
    }

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


    // for (size_t i = 0; i < data->meshes_count; i++)
    for (size_t i = 0; i < 2; i++) // TODO: TEMP for now, delete later
    {
        /* Find position accessor */
        const cgltf_accessor* pos_accessor = cgltf_find_accessor(data->meshes[i].primitives,
                                                                 cgltf_attribute_type_position,
                                                                 i);
        if (pos_accessor)
        {
            out_static_mesh->mesh[i].vertex_count = pos_accessor->count;
            cgltf_size num_floats = cgltf_accessor_unpack_floats(pos_accessor, NULL, 0);
            float* pos_data = arena_alloc(&renderer->arena, num_floats * sizeof(float));
            out_static_mesh->mesh[i].vertices.pos = arena_alloc(&renderer->arena, num_floats * sizeof(float));
            if (pos_data)
            {
                cgltf_accessor_unpack_floats(pos_accessor, pos_data, num_floats * sizeof(float));
                memcpy(out_static_mesh->mesh[i].vertices.pos, pos_data, num_floats * sizeof(vec3));
            }
        }


        // Find normal accessor
        const cgltf_accessor* norm_accessor = cgltf_find_accessor(data->meshes[i].primitives,
                                                                  cgltf_attribute_type_normal,
                                                                  i);
        if (norm_accessor)
        {
            out_static_mesh->mesh[i].normal_count = norm_accessor->count;
            cgltf_size norm_floats = cgltf_accessor_unpack_floats(norm_accessor, NULL, 0);
            float* normal_data = arena_alloc(&renderer->arena, norm_floats * sizeof(float));
            cgltf_accessor_unpack_floats(norm_accessor, normal_data, norm_floats);
            out_static_mesh->mesh[i].vertices.normal = (vec3*)normal_data;
        }

        //  Find tangent accessor
        const cgltf_accessor* tangent_accessor = cgltf_find_accessor(data->meshes[i].primitives,
                                                                     cgltf_attribute_type_tangent, i);

        if (tangent_accessor)
        {
            out_static_mesh->mesh[i].tangent_count = tangent_accessor->count;
            cgltf_size tangent_floats = cgltf_accessor_unpack_floats(tangent_accessor, NULL, 0);
            float* tangent_data = arena_alloc(&renderer->arena, tangent_floats * sizeof(float));
            cgltf_accessor_unpack_floats(tangent_accessor, tangent_data, tangent_floats);
            out_static_mesh->mesh[i].vertices.tangent = (vec4*)tangent_data;
        }

        //  Find texcoord accessor
        const cgltf_accessor* texcoord_accessor = cgltf_find_accessor(data->meshes[i].primitives,
                                                                      cgltf_attribute_type_texcoord, i);
        if (texcoord_accessor)
        {
            out_static_mesh->mesh[i].uv_count = texcoord_accessor->count;
            cgltf_size texcoord_floats_size = cgltf_accessor_unpack_floats(texcoord_accessor, NULL, 0);
            float* texcoords_data = arena_alloc(&renderer->arena, texcoord_floats_size * sizeof(float));
            cgltf_accessor_unpack_floats(texcoord_accessor, texcoords_data, texcoord_floats_size);
            out_static_mesh->mesh[i].vertices.uv = arena_alloc(&renderer->arena, texcoord_floats_size * sizeof(float));
            // for (size_t i = 0; i < texcoord_accessor->count; i++)
            // {
            //     out_mesh->vertices.uv[i].x = texcoords_data[i * 2 + 0];
            //     out_mesh->vertices.uv[i].y = texcoords_data[i * 2 + 1];
            // }
            memcpy(out_static_mesh->mesh[i].vertices.uv, texcoords_data, texcoord_floats_size * sizeof(float));
        }

        // Load indices
        // SEE componentType in the specs for more detail 3.6.2
        out_static_mesh->mesh[i].indices_count = data->meshes[i].primitives->indices->count;
        //TODO: there can be multiple primitices/indices, will come back to
        out_static_mesh->mesh[i].indices = (size_t*)arena_alloc(&renderer->arena,
                                                 data->meshes[i].primitives[0].indices->buffer_view->size);
        u8 index_stride = data->meshes[i].primitives[0].indices->stride;
        if (index_stride == 2)
        {
            out_static_mesh->mesh[i].index_type = VK_INDEX_TYPE_UINT16;
        }
        else if (index_stride == 4)
        {
            out_static_mesh->mesh[i].index_type = VK_INDEX_TYPE_UINT32;
        }
        else
        {
            WARN("GLTF MESH LOADING: UNKNOWN INDEX TYPE STRIDE");
            out_static_mesh->mesh[i].index_type = VK_INDEX_TYPE_UINT32;
        }
        cgltf_accessor_unpack_indices(data->meshes[0].primitives->indices, out_static_mesh->mesh[i].indices, index_stride,
                                      out_static_mesh->mesh[i].indices_count);
    }

    //load materials
    // if (data->meshes[0].primitives->material) {
    //     int mat_idx = cgltf_material_index(data, data->meshes[0].primitives->material);
    //     int hi = 0;
    // }
    // cgltf_texture_index();

    //load the textures
    out_static_mesh->material_handles = arena_alloc(&renderer->arena, sizeof(shader_handle) * data->textures_count);

    char* base_path = NULL;
    for (int i = strlen(gltf_path); i > 0; i--)
    {
        if (gltf_path[i] == '/')
        {
            base_path = arena_alloc(&renderer->frame_arena, i + 2);
            memcpy(base_path, gltf_path, i + 1);
            base_path[i + 1] = '\0';

            break;
        }
    }

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
    }

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
