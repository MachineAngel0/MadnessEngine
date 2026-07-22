#ifndef SHADER_SYSTEM_H
#define SHADER_SYSTEM_H


//TODO: increase later when it becomes relevant
#define SHADER_SYSTEM_CAPACITY 100lu
#include "vk_buffer.h"
#include "vk_pipeline.h"

//shader is the pipeline and descriptors ubos/ssbos needed
//material is all the param data
//texture is the physical image


Shader_System* shader_system_init(Renderer* renderer);


void shader_system_shutdown(Shader_System* system);


//GPU Texture System
Vulkan_Texture* shader_system_get_vulkan_texture(Shader_System* system, u32 bindless_location);

void shader_system_update(Renderer* renderer, Shader_System* system);

//TODO: Texture_Handle shader_system_add_texture_data(renderer* renderer, Shader_System* system, void* pixel_data, u32 width, u32 height);

void shader_system_load_textures_into_gpu(Renderer* renderer, Shader_System* shader_system,
                                          Descriptor_System* descriptor_system, Render_Packet* render_packet);


//Shader Batch system

void _shader_system_shader_batch_create_internal(Renderer* renderer, Shader_System* shader_system,
                                                 const char* shader_name,
                                                 Shader_Stage_Type shader_stage,
                                                 Shader_Pass_Type shader_pass,
                                                 Shader_Mesh_Type mesh_type,
                                                 Shader_Blend_Mode blend_mode,
                                                 u32 material_stride,
                                                 u32 initial_material_count)
{
    Vulkan_Shader_Batch* shader_batch = NULL;
    switch (mesh_type)
    {
    case Shader_Mesh_Type_Mesh:
        shader_batch = &shader_system->mesh_batch[shader_system->mesh_batch_count++];
        break;
    case Shader_Mesh_Type_Skinned:
        shader_batch = &shader_system->skinned_batch[shader_system->skinned_batch_count++];
        break;

    }

    if (!shader_batch)
    {
        MASSERT(false);
    }

    shader_batch->shader_name = c_string_duplicate_heap_alloc(shader_name, renderer->heap_allocator);
    shader_batch->shader_stage_type = shader_stage;
    shader_batch->shader_pass_type = shader_pass;
    shader_batch->draw_count = 0;
    shader_batch->material_stride = material_stride;
    shader_batch->mesh_type = mesh_type;
    shader_batch->blend_mode = blend_mode;


    //load pipeline from our configs
    //TODO: add more configs when neccessary
    vulkan_pipeline_graphics_create(renderer, shader_batch->shader_name, shader_batch->blend_mode, Renderpass_Type_Opaque,
                                    &shader_batch->pipeline, &shader_batch->wireframe_pipeline);


    u32 ssbo_init_amount = 0;
    if (initial_material_count > 0)
    {
        ssbo_init_amount = initial_material_count;
    }
    else
    {
        ssbo_init_amount = 10;
    }


    //create the ssbo's
    shader_batch->material_data_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                     BUFFER_TYPE_CPU_STORAGE,
                                                                     shader_batch->material_stride *
                                                                     ssbo_init_amount);
    switch (shader_batch->mesh_type)
    {
    case Shader_Mesh_Type_Mesh:

        shader_batch->draw_data_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                     BUFFER_TYPE_CPU_STORAGE,
                                                                     sizeof(Mesh_GPU_Draw) * ssbo_init_amount);
        break;
    case Shader_Mesh_Type_Skinned:

        shader_batch->draw_data_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                     BUFFER_TYPE_CPU_STORAGE,
                                                                     sizeof(SKMesh_GPU_Draw) * ssbo_init_amount);
        break;

    }


    shader_batch->indirect_draw_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                     BUFFER_TYPE_INDIRECT,
                                                                     sizeof(VkDrawIndexedIndirectCommand) *
                                                                     ssbo_init_amount);

    //create the push constant, this basically will never change
    shader_batch->pc_data.draw_data_buffer = vulkan_buffer_get_device_address(
        renderer, shader_batch->draw_data_buffer_handle);
    shader_batch->pc_data.material_buffer = vulkan_buffer_get_device_address(
        renderer, shader_batch->material_data_buffer_handle);


    hash_table_insert(shader_system->shader_batch_hash_table, shader_batch->shader_name, &shader_batch);
}

void shader_system_shader_batch_create(Renderer* renderer, Shader_System* shader_system, Material_Batch* material_batch)
{
    _shader_system_shader_batch_create_internal(renderer, renderer->shader_system,
                                                material_batch->shader_name,
                                                material_batch->shader_stage,
                                                material_batch->shader_pass,
                                                material_batch->mesh_type,
                                                material_batch->blend_mode,
                                                material_batch->material_struct->struct_size,
                                                material_batch->material_data->num_items);

    /*

    Vulkan_Shader_Batch* shader_batch = &shader_system->shader_batches[shader_system->shader_batches_count++];
    shader_batch->shader_name = c_string_duplicate_heap_alloc(material_batch->shader_name, renderer->heap_allocator);
    shader_batch->shader_stage_type = material_batch->shader_stage;
    shader_batch->shader_pass_type = material_batch->shader_pass;
    shader_batch->draw_count = 0;
    shader_batch->material_stride = material_batch->material_struct->struct_size;
    shader_batch->material_stride = material_batch->material_struct->struct_size;
    shader_batch->mesh_type = material_batch->mesh_type;
    shader_batch->blend_mode = material_batch->blend_mode;


    //load pipeline from our configs
    //TODO: add more configs when neccessary
    vulkan_pipeline_graphics_create(renderer, shader_batch->shader_name, shader_batch->blend_mode,
                                    &shader_batch->pipeline, &shader_batch->wireframe_pipeline);


    u32 ssbo_init_amount = 0;
    if (material_batch->material_data->num_items <= 0)
    {
        ssbo_init_amount = 10;
    }
    else
    {
        ssbo_init_amount = material_batch->material_data->num_items;
    }

    // shader_batch->shader_pass;

    //create the ssbo's
    shader_batch->material_data_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                     BUFFER_TYPE_CPU_STORAGE,
                                                                     shader_batch->material_stride *
                                                                     ssbo_init_amount);
    switch (shader_batch->mesh_type)
    {
    case Shader_Mesh_Type_Mesh:

        shader_batch->draw_data_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                     BUFFER_TYPE_CPU_STORAGE,
                                                                     sizeof(Mesh_GPU_Draw) * ssbo_init_amount);
        break;
    case Shader_Mesh_Type_Skinned:

        shader_batch->draw_data_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                     BUFFER_TYPE_CPU_STORAGE,
                                                                     sizeof(SKMesh_GPU_Draw) * ssbo_init_amount);
        break;
    }


    shader_batch->indirect_draw_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                     BUFFER_TYPE_INDIRECT,
                                                                     sizeof(VkDrawIndexedIndirectCommand) *
                                                                     ssbo_init_amount);

    //create the push constant, this basically will never change
    shader_batch->pc_data.draw_data_buffer = vulkan_buffer_get_device_address(
        renderer, shader_batch->draw_data_buffer_handle);
    shader_batch->pc_data.material_buffer = vulkan_buffer_get_device_address(
        renderer, shader_batch->material_data_buffer_handle);


    hash_table_insert(shader_system->shader_batch_hash_table, shader_batch->shader_name, &shader_batch);
    */
}


void shader_system_shader_batch_free(Renderer* renderer, Shader_System* shader_system, const char* shader_name)
{
    //TODO:
}


void shader_system_check_for_new_shader_batches(Renderer* renderer, Shader_System* shader_system,
                                                Render_Packet* render_packet)
{
    //TODO: we should call this at start up once
    for (int i = 0; i < render_packet->draw_3d_data_packet.mesh_batch_count; ++i)
    {
        if (hash_table_contains(shader_system->shader_batch_hash_table,
                                render_packet->draw_3d_data_packet.mesh_batch[i].shader_name))
        {
            continue;
        }

        shader_system_shader_batch_create(renderer, shader_system, &render_packet->draw_3d_data_packet.mesh_batch[i]);
    }

    for (int i = 0; i < render_packet->draw_3d_data_packet.skinned_batch_count; ++i)
    {
        if (hash_table_contains(shader_system->shader_batch_hash_table,
                                render_packet->draw_3d_data_packet.skinned_batch[i].shader_name))
        {
            continue;
        }

        shader_system_shader_batch_create(renderer, shader_system, &render_packet->draw_3d_data_packet.skinned_batch[i]);
    }
}





#endif //SHADER_SYSTEM_H
