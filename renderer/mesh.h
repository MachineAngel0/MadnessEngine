#ifndef MESH_H
#define MESH_H

#include "vulkan_types.h"
#include "vk_buffer.h"
#include "vk_descriptors.h"


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
    Mesh_System* mesh_system = renderer->mesh_system;


    // GET BASE PATH
    char* base_path = c_string_path_strip(gltf_path, &renderer->frame_arena);


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
            float* pos_data = arena_alloc(&renderer->frame_arena, float_bytes);
            current_submesh->pos = arena_alloc(&renderer->arena, float_bytes);
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
            float* normal_data = arena_alloc(&renderer->arena, norm_bytes);
            current_submesh->normal = arena_alloc(&renderer->arena, norm_bytes);
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
            float* tangent_data = arena_alloc(&renderer->arena, tangent_bytes);
            current_submesh->tangent = arena_alloc(&renderer->arena, tangent_bytes);
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
            float* uv_data = arena_alloc(&renderer->frame_arena, uv_byte_size);
            cgltf_accessor_unpack_floats(texcoord_accessor, uv_data, uv_floats_count);

            current_submesh->uv = arena_alloc(&renderer->arena, uv_byte_size);
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
        current_submesh->indices = arena_alloc(&renderer->arena,
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
        if (color_texture)
        {
            current_submesh->mesh_pipeline_mask |= MESH_PIPELINE_COLOR;

            char* texture_path = arena_alloc(&renderer->frame_arena,
                                             strlen(base_path) + strlen(color_texture->image->uri));
            // takes a buffer, message format, then the remaining strings
            sprintf(texture_path, "%s%s", base_path, color_texture->image->uri);
            TRACE("COLOR Texture Path:  %s", texture_path);

            current_submesh->color_texture = shader_system_add_texture(
                &renderer->context, renderer->shader_system,
                texture_path);
            current_submesh->material_params.color_index = current_submesh->color_texture.handle;
        }

        //METAL-ROUGHNESS
        cgltf_texture* metal_roughness_texture = data->meshes[mesh_idx].primitives->material->pbr_metallic_roughness.
                                                                        metallic_roughness_texture.texture;
        if (metal_roughness_texture)
        {
            current_submesh->mesh_pipeline_mask |= MESH_PIPELINE_ROUGHNESS;
            current_submesh->mesh_pipeline_mask |= MESH_PIPELINE_METALLIC;
            char* texture_path = arena_alloc(&renderer->frame_arena,
                                             strlen(base_path) + strlen(metal_roughness_texture->image->uri));
            // takes a buffer, message format, then the remaining strings
            sprintf(texture_path, "%s%s", base_path, metal_roughness_texture->image->uri);
            TRACE("METAL/ROUGHNESS Texture Path:  %s", texture_path);


            current_submesh->metallic_texture = shader_system_add_texture(
                &renderer->context, renderer->shader_system,
                texture_path);
            current_submesh->roughness_texture = current_submesh->metallic_texture;

            current_submesh->material_params.metallic_index = current_submesh->metallic_texture.handle;
            current_submesh->material_params.roughness_index = current_submesh->roughness_texture.handle;
        }

        // AO
        //NOTE: this material in theory can be included in the pbr-metal-roughness texture, in which case, just return a handle
        cgltf_texture* AO_texture = data->meshes[mesh_idx].primitives->material->occlusion_texture.texture;
        if (AO_texture)
        {
            current_submesh->mesh_pipeline_mask |= MESH_PIPELINE_AO;
            char* texture_path = arena_alloc(&renderer->frame_arena,
                                             strlen(base_path) + strlen(AO_texture->image->uri));
            // takes a buffer, message format, then the remaining strings
            sprintf(texture_path, "%s%s", base_path, AO_texture->image->uri);
            TRACE("AO Texture Path:  %s", texture_path);

            current_submesh->ambient_occlusion_texture = shader_system_add_texture(
                &renderer->context, renderer->shader_system,
                texture_path);
            current_submesh->material_params.ambient_occlusion_index = current_submesh->ambient_occlusion_texture.
                handle;
        }


        //NORMAL TEXTURE
        // data->meshes[mesh_idx].primitives->material->has_pbr_metallic_roughness
        cgltf_texture* normal_texture = data->meshes[mesh_idx].primitives->material->normal_texture.texture;
        if (normal_texture)
        {
            current_submesh->mesh_pipeline_mask |= MESH_PIPELINE_NORMAL;
            char* texture_path = arena_alloc(&renderer->frame_arena,
                                             strlen(base_path) + strlen(normal_texture->image->uri));
            // takes a buffer, message format, then the remaining strings
            sprintf(texture_path, "%s%s", base_path, normal_texture->image->uri);
            TRACE("NORMAL Texture Path:  %s", texture_path);

            current_submesh->normal_texture = shader_system_add_texture(
                &renderer->context, renderer->shader_system,
                texture_path);
            current_submesh->material_params.normal_index = current_submesh->normal_texture.
                                                                             handle;
        }

        //EMISSIVE TEXTURE
        cgltf_texture* emissive_texture = data->meshes[mesh_idx].primitives->material->emissive_texture.texture;
        if (emissive_texture)
        {
            current_submesh->mesh_pipeline_mask |= MESH_PIPELINE_EMISSIVE;
            char* texture_path = arena_alloc(&renderer->frame_arena,
                                             strlen(base_path) + strlen(emissive_texture->image->uri));
            // takes a buffer, message format, then the remaining strings
            sprintf(texture_path, "%s%s", base_path, emissive_texture->image->uri);
            TRACE("EMISSIVE Texture Path:  %s", texture_path);

            current_submesh->emissive_texture = shader_system_add_texture(
                &renderer->context, renderer->shader_system,
                texture_path);
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

    for (u32 i = 0; i < out_static_mesh->mesh_size; i++)
    {
        update_texture_bindless_descriptor_set(renderer,
                                               renderer->descriptor_system,
                                               out_static_mesh->mesh[i].color_texture,
                                               out_static_mesh->mesh[i].color_texture.handle);
    }


    mesh_system->static_mesh_array[mesh_system->static_mesh_array_size] = *out_static_mesh;
    mesh_system->static_mesh_array_size++;

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
    Mesh_System* out_mesh_system = arena_alloc(&renderer->arena, sizeof(Mesh_System));
    out_mesh_system->mesh_shader_permutations = arena_alloc(&renderer->arena, sizeof(Mesh_Pipeline_Permutations));
    out_mesh_system->mesh_shader_permutations->permutation_keys = darray_create_reserve(u32, 100);


    memset(out_mesh_system->static_mesh_array, 0, sizeof(static_mesh) * 1000);
    out_mesh_system->static_mesh_array;

    out_mesh_system->vertex_byte_size = 0;
    out_mesh_system->index_byte_size = 0;
    out_mesh_system->normals_byte_size = 0;
    out_mesh_system->tangent_byte_size = 0;
    out_mesh_system->uv_byte_size = 0;

    out_mesh_system->vertex_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system, BUFFER_TYPE_VERTEX,
                                                                 MB(32));
    out_mesh_system->index_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system, BUFFER_TYPE_INDEX,
                                                                MB(32));
    out_mesh_system->indirect_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                   BUFFER_TYPE_INDIRECT,MB(32));
    out_mesh_system->normal_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                 BUFFER_TYPE_CPU_STORAGE,MB(32));
    out_mesh_system->tangent_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                  BUFFER_TYPE_CPU_STORAGE,MB(32));
    out_mesh_system->uv_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system, BUFFER_TYPE_CPU_STORAGE,
                                                             MB(32));
    out_mesh_system->material_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                   BUFFER_TYPE_CPU_STORAGE,
                                                                   MB(32));


    return out_mesh_system;
}


void mesh_system_generate_draw_data(renderer* renderer, Mesh_System* mesh_system)
{
    mesh_system->indirect_draw_count = 0;
    //were assuming that the data is already in the buffer,
    // we are just generating the draw calls for the specific pipeline
    vulkan_buffer* vertex_buffer = vulkan_buffer_get_clear(renderer, mesh_system->vertex_buffer_handle);
    vulkan_buffer* index_buffer = vulkan_buffer_get_clear(renderer, mesh_system->index_buffer_handle);
    vulkan_buffer* indirect_buffer = vulkan_buffer_get_clear(renderer, mesh_system->indirect_buffer_handle);
    vulkan_buffer* uv_buffer = vulkan_buffer_get_clear(renderer, mesh_system->uv_buffer_handle);
    vulkan_buffer* tangent_buffer = vulkan_buffer_get_clear(renderer, mesh_system->tangent_buffer_handle);
    vulkan_buffer* normal_buffer = vulkan_buffer_get_clear(renderer, mesh_system->normal_buffer_handle);
    vulkan_buffer* material_buffer = vulkan_buffer_get_clear(renderer, mesh_system->material_buffer_handle);

    size_t vertex_byte_size = 0;
    size_t index_count_size = 0;


    VkDrawIndexedIndirectCommand indirect_draw = {0};
    for (size_t s_mesh_idx = 0; s_mesh_idx < mesh_system->static_mesh_array_size; s_mesh_idx++)
    {
        for (size_t submesh_idx = 0; submesh_idx < mesh_system->static_mesh_array[s_mesh_idx].mesh_size; submesh_idx++)
        {
            submesh* current_submesh = &mesh_system->static_mesh_array[s_mesh_idx].mesh[submesh_idx];

            indirect_draw.firstIndex = index_count_size;
            indirect_draw.firstInstance = 0;
            indirect_draw.indexCount = current_submesh->indices_count;
            indirect_draw.instanceCount = 1;
            indirect_draw.vertexOffset = vertex_byte_size / sizeof(vec3);

            vertex_byte_size += current_submesh->vertex_bytes;
            index_count_size += current_submesh->indices_count;

            mesh_system->indirect_draw_count++;

            //this could be optimized later, by using flat arrays for all the submeshes and just doing a memcpy
            vulkan_buffer_cpu_data_copy_from_offset(renderer, vertex_buffer,
                                                    current_submesh->pos,
                                                    current_submesh->vertex_bytes);
            vulkan_buffer_cpu_data_copy_from_offset(renderer, index_buffer,
                                                    current_submesh->indices,
                                                    current_submesh->indices_bytes);
            vulkan_buffer_cpu_data_copy_from_offset(renderer, normal_buffer,
                                                    current_submesh->normal,
                                                    current_submesh->normal_bytes);
            vulkan_buffer_cpu_data_copy_from_offset(renderer, uv_buffer,
                                                    current_submesh->uv,
                                                    current_submesh->uv_bytes);
            //add the draw data
            //push the indirect draw into the draw data
            vulkan_buffer_cpu_data_copy_from_offset(renderer, indirect_buffer,
                                                    &indirect_draw,
                                                    sizeof(VkDrawIndexedIndirectCommand));
            vulkan_buffer_cpu_data_copy_from_offset(renderer, material_buffer,
                                                    &current_submesh->material_params,
                                                    sizeof(Material_Param_Data));
        }
    }


    //it wouldn't be a bad idea to update this once per frame
    //
    // update_storage_buffer_bindless_descriptor_set(renderer, renderer->descriptor_system,
    //                                               mesh_system->uv_buffer_handle, 0);
    // update_storage_buffer_bindless_descriptor_set(renderer, renderer->descriptor_system,
    //                                               mesh_system->normal_buffer_handle, 0);
    // update_storage_buffer_bindless_descriptor_set(renderer, renderer->descriptor_system,
    //                                               mesh_system->material_buffer_handle, 0);
    // update_storage_buffer_bindless_descriptor_set(renderer, renderer->descriptor_system,
    //                                               mesh_system->tangent_buffer_handle, 0);

    /* TODO:
    update_storage_buffer_bindless_descriptor_set(
    &renderer->context, &renderer->global_descriptors.storage_descriptors,
    vulkan_buffer_get(renderer, mesh_system->tangent_buffer_handle), ???);*/
}


void mesh_system_draw(renderer* renderer, Mesh_System* mesh_system, vulkan_command_buffer* command_buffer,
                      vulkan_shader_pipeline* pipeline)
{
    INFO("MESH SYSTEM DRAW CALLS")

    //only bind the vertex and index, the storage buffers are in bindless
    vulkan_buffer* indirect_buffer = vulkan_buffer_get(renderer, mesh_system->indirect_buffer_handle);
    vulkan_buffer* vertex_buffer = vulkan_buffer_get(renderer, mesh_system->vertex_buffer_handle);
    vulkan_buffer* index_buffer = vulkan_buffer_get(renderer, mesh_system->index_buffer_handle);


    VkDeviceSize pOffsets[] = {0};

    vkCmdBindVertexBuffers(command_buffer->handle, 0, 1, &vertex_buffer->handle,
                           pOffsets);

    vkCmdBindIndexBuffer(command_buffer->handle, index_buffer->handle, 0,
                         VK_INDEX_TYPE_UINT16);

    mesh_system->pc_mesh.ubo_buffer_idx = renderer->buffer_system->global_ubo_handle.handle;
    mesh_system->pc_mesh.normal_buffer_idx = mesh_system->normal_buffer_handle.handle;
    mesh_system->pc_mesh.tangent_buffer_idx = mesh_system->tangent_buffer_handle.handle;
    mesh_system->pc_mesh.uv_buffer_idx = mesh_system->uv_buffer_handle.handle;
    mesh_system->pc_mesh.transform_buffer_idx = mesh_system->transform_buffer_handle.handle;
    mesh_system->pc_mesh.material_buffer_idx = mesh_system->material_buffer_handle.handle;

    VkPushConstantsInfo push_constant_info = {0};
    push_constant_info.sType = VK_STRUCTURE_TYPE_PUSH_CONSTANTS_INFO;
    push_constant_info.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    push_constant_info.layout = pipeline->pipeline_layout;
    push_constant_info.offset = 0;
    push_constant_info.size = sizeof(PC_Mesh);
    // push_constant_info.size = sizeof(mesh_system->pc_mesh); // make sure its not a pointer if i use this
    push_constant_info.pValues = &mesh_system->pc_mesh;
    push_constant_info.pNext = NULL;
    vkCmdPushConstants2(command_buffer->handle, &push_constant_info);


    //no need to sort by permutation rn, it gets handled in the shader, we can always sort out this later
    if (renderer->context.device.features.multiDrawIndirect)
    {
        vkCmdDrawIndexedIndirect(command_buffer->handle,
                                 indirect_buffer->handle, 0,
                                 mesh_system->indirect_draw_count,
                                 sizeof(VkDrawIndexedIndirectCommand));
    }
    else
    {
        // If multi draw is not available, we must issue separate draw commands
        for (u64 j = 0; j < mesh_system->indirect_draw_count; j++)
        {
            vkCmdDrawIndexedIndirect(command_buffer->handle,
                                     indirect_buffer->handle,
                                     j * sizeof(VkDrawIndexedIndirectCommand), 1,
                                     sizeof(VkDrawIndexedIndirectCommand));
        }
    }
}


/*
void static_mesh_to_madness_mesh(static_mesh* s_mesh, const char* file_name, Frame_Arena* frame_arena)
{
    //TODO: in general this is for another time, when i need the performance and i know what my engine should look like

    //TODO: so the problem is how i want to structure the data, if we even want to allow submeshes, probably
    const char* testing_asset_format_file_path = "../renderer/asset_format/";
    const char* final_path = c_string_concat(testing_asset_format_file_path, file_name);
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

}*/


#endif
