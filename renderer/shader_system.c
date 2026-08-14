#include "shader_system.h"
#include "vk_image.h"


Shader_System* shader_system_init(Renderer* renderer)
{
    Shader_System* shader_system = allocator_alloc(&renderer->allocator, sizeof(Shader_System));
    renderer->shader_system = shader_system;

    // create_texture_image(&renderer->context, renderer->context.graphics_command_buffer,
    //                      "../renderer/texture/error_texture.png",
    //                      &(*out_shader_system)->error_texture);

    shader_system->pipeline_indexes = 0;



    //we store the pointer, we dont want a copy
    shader_system->shader_batch_hash_set = hash_set_init(sizeof(Material_ID), 100);


    INFO("SHADER SYSTEM CREATED")


    return shader_system;
}


void shader_system_shutdown(Shader_System* system)
{
    //TODO: create and hookup to the renderer shutdown
}




void shader_system_update(Renderer* renderer, Shader_System* shader_system, Render_Packet* render_packet)
{
    //TODO: we should call this at start up once
    for (int i = 0; i < render_packet->draw_3d_data_packet.material_batch_count; ++i)
    {
        if (hash_set_contains(shader_system->shader_batch_hash_set,
                              &render_packet->draw_3d_data_packet.material_batch[i].material_key))
        {
            continue;
        }
        shader_system_shader_batch_create(renderer, shader_system,
                                          &render_packet->draw_3d_data_packet.material_batch[i]);
    }

}



// Vulkan_Texture* shader_system_new_render_pass_texture(Shader_System* system)
// {
//     return &system->renderpass_textures[system->renderpass_texture_indexes++];
// }





void shader_system_shader_batch_create(Renderer* renderer, Shader_System* shader_system, Material_Batch* material_batch)
{
    Vulkan_Shader_Batch* shader_batch = NULL;
    switch (material_batch->material_asset->material_info.mesh_type)
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
    shader_batch->material_batch_reference = material_batch;

    shader_batch->material_id = material_batch->material_key;
    shader_batch->shader_name = string_to_c_string_alloc_heap(material_batch->material_asset->material_info.shader_name, renderer->heap_allocator);
    shader_batch->transluency = material_batch->material_asset->material_info.transluency;
    shader_batch->renderpass_types = material_batch->material_asset->material_info.renderpass;
    shader_batch->mesh_type = material_batch->material_asset->material_info.mesh_type;
    shader_batch->blend_mode = material_batch->material_asset->material_info.blend_mode;
    shader_batch->material_stride = material_batch->material_asset->material_gpu_definition->struct_size;
    shader_batch->draw_count = 0;


    //load pipeline from our configs
    //TODO: add more configs when neccessary
    vulkan_pipeline_graphics_create(renderer, shader_batch->shader_name, shader_batch->blend_mode,
                                    shader_batch->transluency,
                                    &shader_batch->pipeline, &shader_batch->wireframe_pipeline);


    u32 ssbo_init_amount = 0;
    if (material_batch->material_data->num_items > 0)
    {
        ssbo_init_amount = material_batch->material_data->num_items;
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
                                                                     sizeof(Vulkan_Mesh_Draw) * ssbo_init_amount);
        break;
    case Shader_Mesh_Type_Skinned:

        shader_batch->draw_data_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                     BUFFER_TYPE_CPU_STORAGE,
                                                                     sizeof(Vulkan_Skinned_Draw) * ssbo_init_amount);
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

    hash_set_insert(shader_system->shader_batch_hash_set, &shader_batch->material_id);
}

void shader_system_shader_batch_free(Renderer* renderer, Shader_System* shader_system, const char* shader_name)
{
    //TODO:
}


