#ifndef MESH_H
#define MESH_H

#include "vulkan_types.h"
#include "shader_system.h"


typedef struct vertex_mesh
{
    //all darrays, NOTE: They might not need to be darrays, just normal arrays, if they dont change in size
    vec3* pos;
    vec3* normal;
    vec4* tangent;
    vec2* tex_coord;

    // vec4* color; //might not support
} vertex_mesh;

//TODO: so every model can have multiple meshes
typedef struct mesh
{
    vertex_mesh vertices;
    u32* indices;
    u64 indices_size;
    VkIndexType index_type;
    //wether it has index's or not // TODO: can probably move out into its own type of mesh, struct mesh_indexless
    shader_handle* material_handles;
} mesh;


typedef struct static_mesh
{
    mesh* mesh;
    // the number of meshes in the model
    u32 mesh_size;
} static_mesh;


mesh* mesh_init()
{
    mesh* m = malloc(sizeof(m));

    m->vertices.pos = darray_create(vec3);
    m->vertices.normal = darray_create(vec3);
    m->vertices.tex_coord = darray_create(vec2);
    m->vertices.tangent = darray_create(vec4);
    // m->vertices.color = darray_create(vec4);



    return m;
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


//TODO: this needs a rewrite from scratch, i have no idea why its crashing my application
mesh* mesh_load_gltf(renderer* renderer, const char* gltf_path)
{
    //gltf files can technically come with on indices

    //TODO: they're might be multiple meshes here by technicality
    mesh* out_mesh = mesh_init();

    //TODO: since there can be multiple meshes, we would need multiple of these buffers
    u8* position_buffer = NULL;
    u8* normal_buffer = NULL;
    u8* tangent_buffer = NULL;
    u8* tex_coord_buffer = NULL;
    // float* color_buffer;

    u8* index_buffer = NULL;

    cgltf_options options = {0};
    cgltf_data* data = NULL;
    cgltf_result result = cgltf_parse_file(&options, gltf_path, &data);
    if (result != cgltf_result_success)
    {
        MASSERT_MSG(false, "Failed to load GLTF")
        return NULL;
    }


    String* path = STRING_CREATE_FROM_BUFFER(gltf_path);
    string_print(path);
    String* removed_path = string_strip_from_end(path, '/');
    string_print(removed_path);



    for (int i = 0; i < data->meshes_count; i++)
    {
        for (int mesh_index = 0; mesh_index < data->meshes[i].primitives_count; mesh_index++)
        {
            //one set of indices per primitive
            //points to the accessor field data type
            u64 index_count = data->meshes[i].primitives[mesh_index].indices->count;
            u64 index_offset = data->meshes[i].primitives[mesh_index].indices->offset;
            u64 index_stride = data->meshes[i].primitives[mesh_index].indices->stride;
            u64 index_read_size = data->meshes[i].primitives[mesh_index].indices->buffer_view->size;
            const char* index_uri_file_name = data->meshes[i].primitives[mesh_index].indices->buffer_view->buffer->uri;

            out_mesh->indices_size = index_count;

            String* index_bin = STRING_CREATE_FROM_BUFFER(index_uri_file_name);
            String* index_bin_path = string_concat(removed_path, index_bin);
            string_print(index_bin_path);
            const char* new_bin_path = string_convert_to_c_string(index_bin_path);

            FILE* index_bin_fptr = fopen(new_bin_path, "rb");
            if (!index_bin_fptr)
            {
                printf("%s", new_bin_path);
                return NULL;
            }
            fseek(index_bin_fptr, index_offset, SEEK_CUR);

            out_mesh->indices = malloc(index_read_size); // TODO: replace with allocator
            // index_buffer = malloc(index_read_size);
            size_t index_bytes_read = fread(out_mesh->indices, 1, index_read_size, index_bin_fptr);
            if (index_bytes_read != index_read_size)
            {
                WARN("NOT YET HANDLED BUT GLTF BIN NOT READ ENTIRE BUFFER FOR INDEX")
            }
            if (index_stride == 2)
            {
                out_mesh->index_type = VK_INDEX_TYPE_UINT16;
            }
            else if (index_stride == 4)
            {
                out_mesh->index_type = VK_INDEX_TYPE_UINT32;
            }
            else
            {
                WARN("MESH LOADING: UNKNOWN INDEX BUFFER STRIDE");
            }

            fclose(index_bin_fptr);


            for (int atr_index = 0; atr_index < data->meshes[i].primitives[mesh_index].attributes_count; atr_index
                 ++)
            {
                //points to the accessor field data type
                size_t file_offset = data->meshes[i].primitives[mesh_index].attributes[atr_index].data->buffer_view->
                    offset;
                size_t read_size = data->meshes[i].primitives[mesh_index].attributes[atr_index].data->buffer_view->
                    size;



                // how many of the type we have = 36 vec3's
                size_t data_count = data->meshes[i].primitives[mesh_index].attributes[atr_index].data->count;
                // how large is the type: vec3 = 12 stride (36*12) = 432, which is the size in bytes = read_size
                size_t stride = data->meshes[i].primitives[mesh_index].attributes[atr_index].data->stride;
                //gives us a custom type, if i do use, will need have a switch statement
                cgltf_type data_type = data->meshes[i].primitives[mesh_index].attributes[atr_index].data->type;

                data->meshes[i].primitives[mesh_index].attributes[atr_index].data->buffer_view->buffer;

                const char* gltf_bin = data->meshes[i].primitives[mesh_index].attributes[atr_index].data->
                    buffer_view->buffer->uri;
                String* bin = STRING_CREATE_FROM_BUFFER(gltf_bin);
                String* bin_path = string_concat(removed_path, bin);
                string_print(bin_path);
                const char* mesh_bin_path = string_convert_to_c_string(bin_path);

                //NOTE: they can technically be different bins, it's not likely but here just in case
                FILE* bin_fptr = fopen(mesh_bin_path, "rb");
                if (!bin_fptr)
                {
                    printf("%s", mesh_bin_path);
                    return NULL;
                }

                //not needed if we constantly reopen the file
                // go to the file destination from the start
                fseek(bin_fptr, file_offset, SEEK_SET);

                //move the fileptr into the proper location, then read the data later on
                // fseek(bin_fptr, file_offset, SEEK_CUR);


                if (strcmp(data->meshes[i].primitives[mesh_index].attributes[atr_index].name, "POSITION") == 0)
                {
                    INFO("PROCESSING POSITION");

                    //get vertex buffer data
                    position_buffer = malloc(read_size);
                    if (!position_buffer)
                    {
                        FATAL("POSITION BUFFER MALLOC FAILED")
                        return NULL;
                    }

                    size_t bytes_read = fread(position_buffer, 1, read_size, bin_fptr);
                    if (bytes_read != read_size)
                    {
                        WARN("NOT YET HANDLED BUT GLTF BIN NOT READ ENTIRE BUFFER")
                    }
                    for (u64 vert_index = 0; vert_index < read_size; vert_index += stride)
                    {
                        vec3 temp_vert;
                        memcpy(&temp_vert.x, (position_buffer + vert_index), sizeof(float));
                        memcpy(&temp_vert.y, (position_buffer + vert_index + 4), sizeof(float));
                        memcpy(&temp_vert.z, (position_buffer + vert_index + 8), sizeof(float));
                        darray_push(out_mesh->vertices.pos, temp_vert);
                    }

                    free(position_buffer);
                    position_buffer = NULL;

                }
                /*
                else if (strcmp(data->meshes[i].primitives[mesh_index].attributes[atr_index].name, "NORMAL") == 0)
                {
                    INFO("PROCESSING NORMAL");

                    //get normal buffer data
                    normal_buffer = malloc(read_size);
                    size_t bytes_read = fread(normal_buffer, 1, read_size, bin_fptr);
                    if (bytes_read != read_size)
                    {
                        WARN("NOT YET HANDLED BUT GLTF BIN NOT READ ENTIRE BUFFER")
                    }
                    for (u64 normal_index = 0; normal_index < read_size; normal_index += (4 * 3))
                    {
                        vec3 temp_vert;
                        memcpy(&temp_vert.x, (normal_buffer + normal_index), sizeof(float));
                        memcpy(&temp_vert.y, (normal_buffer + normal_index + 4), sizeof(float));
                        memcpy(&temp_vert.z, (normal_buffer + normal_index + 8), sizeof(float));
                        darray_push(out_mesh->vertices.normal, temp_vert);
                    }
                }
                else if (strcmp(data->meshes[i].primitives[mesh_index].attributes[atr_index].name, "TANGENT") == 0)
                {
                    INFO("PROCESSING TANGENT");

                    tangent_buffer = malloc(read_size);
                    size_t bytes_read = fread(tangent_buffer, 1, read_size, bin_fptr);
                    if (bytes_read != read_size)
                    {
                        WARN("NOT YET HANDLED BUT GLTF BIN NOT READ ENTIRE BUFFER")
                    }
                    for (u64 tangent_index = 0; tangent_index < read_size; tangent_index += stride)
                    {
                        vec4 temp_vert;
                        memcpy(&temp_vert.x, (tangent_buffer + tangent_index), sizeof(float));
                        memcpy(&temp_vert.y, (tangent_buffer + tangent_index + 4), sizeof(float));
                        memcpy(&temp_vert.z, (tangent_buffer + tangent_index + 8), sizeof(float));
                        memcpy(&temp_vert.w, (tangent_buffer + tangent_index + 12), sizeof(float));
                        darray_push(out_mesh->vertices.tangent, temp_vert);
                    }
                }

                else if (strcmp(data->meshes[i].primitives[mesh_index].attributes[atr_index].name, "TEXCOORD_0") == 0)
                {
                    INFO("PROCESSING TEXCOORD_0");

                    tex_coord_buffer = malloc(read_size);
                    size_t bytes_read = fread(tex_coord_buffer, 1, read_size, bin_fptr);
                    if (bytes_read != read_size)
                    {
                        WARN("NOT YET HANDLED BUT GLTF BIN NOT READ ENTIRE BUFFER")
                    }
                    for (u64 tex_index = 0; tex_index < read_size; tex_index += stride)
                    {
                        vec2 temp_vert;
                        memcpy(&temp_vert.x, (tex_coord_buffer + tex_index), sizeof(float));
                        memcpy(&temp_vert.y, (tex_coord_buffer + tex_index + 4), sizeof(float));
                        darray_push(out_mesh->vertices.tex_coord, temp_vert);
                    }
                }
                /*
                else if (strcmp(data->meshes[i].primitives[mesh_index].attributes[atr_index].name, "COLOR_n") == 0)
                {
                    WARN("PROCESSING COLOR_n, NOT SUPPORTED");
                    tex_coord_buffer = malloc(read_size);
                    size_t bytes_read = fread(tangent_buffer, 1, read_size, bin_fptr);
                    if (bytes_read != read_size)
                    {
                        WARN("NOT YET HANDLED BUT GLTF BIN NOT READ ENTIRE BUFFER")
                    }
                    switch (data_type)
                    {
                        case cgltf_type_vec3:
                            for (u64 normal_index = 0; normal_index < read_size; normal_index += stride)
                            {
                                vec3 temp_vert;
                                temp_vert.x = tex_coord_buffer[normal_index];
                                temp_vert.y = tex_coord_buffer[normal_index + 1];
                                darray_push(out_mesh->vertices.color, temp_vert);
                            }
                            break;
                        case cgltf_type_vec4:
                            break;
                        default:
                            break;
                    }
                }
                //for animations
                else if (strcmp(data->meshes[i].primitives[mesh_index].attributes[atr_index].name, "JOINTS_n") == 0)
                {
                    DEBUG("TODO: GLTF JOINTS");
                }
                else if (strcmp(data->meshes[i].primitives[mesh_index].attributes[atr_index].name, "WEIGHTS_n") == 0)
                {
                    DEBUG("TODO: GLTF WIEGHTS");
                }
                */
                fclose(bin_fptr);
            }
        }
    }

    INFO("PROCESSING MATERIAL LOCATIONS");
    out_mesh->material_handles = malloc(sizeof(shader_handle) * data->textures_count);
    memset(out_mesh->material_handles, 0, sizeof(shader_handle) * data->textures_count);
    for (size_t i = 0; i < data->textures_count; i++)
    {
        String* index_bin = STRING_CREATE_FROM_BUFFER(data->textures[i].image->uri);
        String* index_bin_path = string_concat(removed_path, index_bin);
        string_print(index_bin_path);
        const char* new_texture_path = string_convert_to_c_string(index_bin_path);
        out_mesh->material_handles[i] = shader_system_add_texture(&renderer->context, renderer->shader_system,
                                                                  new_texture_path);
    }


    /*
    for (int i = 0; i < data->materials_count; i++)
    {

        if (data->materials->has_pbr_metallic_roughness)
        {
            // data->materials->pbr_metallic_roughness.base_color_texture
        };



        if (data->materials->has_pbr_specular_glossiness){}
        if (data->materials->has_clearcoat){}
        if (data->materials->has_sheen){}
        if (data->materials->has_volume){}
        if (data->materials->has_ior){}
        if (data->materials->has_specular){}
        if (data->materials->has_emissive_strength){}
        if (data->materials->has_iridescence){}
        if (data->materials->has_diffuse_transmission){}
        if (data->materials->has_anisotropy){}
        if (data->materials->has_dispersion){}


    }*/


    cgltf_free(data);

    // darray_debug_print(out_mesh->vertices.pos, )
    return out_mesh;
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
