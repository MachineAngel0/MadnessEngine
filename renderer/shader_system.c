#include "shader_system.h"


Vulkan_Shader_System* vulkan_shader_system_init(Renderer* renderer)
{
    Vulkan_Shader_System* shader_system = allocator_alloc(&renderer->allocator, sizeof(Vulkan_Shader_System));
    renderer->shader_system = shader_system;

    // create_texture_image(&renderer->context, renderer->context.graphics_command_buffer,
    //                      "../renderer/texture/error_texture.png",
    //                      &(*out_shader_system)->error_texture);

    shader_system->pipeline_indexes = 0;


    //we store the pointer, we dont want a copy
    shader_system->shader_batch_hash_set = hash_set_init(sizeof(Shader_Key), 100);

    shader_system->material_buffers = vulkan_buffer_create_frame(renderer, renderer->buffer_system,
                                                                 BUFFER_TYPE_STORAGE_GPU, MATERIAL_BUFFER_SIZE);


    INFO("SHADER SYSTEM CREATED")


    return shader_system;
}


void vulkan_shader_system_shutdown(Vulkan_Shader_System* system)
{
    //TODO: create and hookup to the renderer shutdown
}


void vulkan_shader_system_update(Renderer* renderer, Vulkan_Shader_System* shader_system, Render_Packet* render_packet)
{
    //TODO: ideally load all our pipelines (at least for the level) at the start but keep the material data empty
    //TODO: we should call this at start up once
    for (int i = 0; i < render_packet->draw_3d_data_packet.shader_asset_count; ++i)
    {
        if (hash_set_contains(shader_system->shader_batch_hash_set,
                              &render_packet->draw_3d_data_packet.shader_assets[i].shader_key))
        {
            continue;
        }
        vulkan_shader_system_shader_batch_create(renderer, shader_system,
                                                 &render_packet->draw_3d_data_packet.shader_assets[i]);
    }


    Vulkan_Command_Buffer command_buffer;
    vulkan_queue_system_get_graphics_command_buffer(renderer, &command_buffer);

    vulkan_buffer_frame_staging_upload(renderer, shader_system->material_buffers, &command_buffer,
                                       render_packet->draw_3d_data_packet.material_buffer,
                                       render_packet->draw_3d_data_packet.material_buffer_byte_size);



    for (int i = 0; i < shader_system->material_batch_count; ++i)
    {
        Vulkan_Shader_Batch* current_batch = &renderer->shader_system->material_batch[i];
        current_batch->draw_count = 0;
    }


}


void vulkan_shader_system_shader_batch_create(Renderer* renderer, Vulkan_Shader_System* shader_system,
                                              Shader_Asset* shader_asset)
{
    Vulkan_Shader_Batch* shader_batch = &shader_system->material_batch[shader_system->material_batch_count++];

    MASSERT(shader_batch)


    shader_batch->material_key = shader_asset->shader_key;
    shader_batch->shader_name = string_to_c_string_alloc_heap(shader_asset->shader_info.shader_name,
                                                              renderer->heap_allocator);

    shader_batch->blend_mode = shader_asset->shader_info.blend_mode;
    shader_batch->draw_count = 0;


    //load pipeline from our configs
    //TODO: add more configs when neccessary
    vulkan_pipeline_graphics_create(renderer, shader_batch->shader_name, shader_batch->blend_mode, false,
                                    &shader_batch->pipeline,
                                    &shader_batch->wireframe_pipeline);





    //TODO: change the sizing or allocate the draw amount per frame
    // or even better, use per frame, but source from a much larger per frame buffer, sized to the max draw count in general
    shader_batch->indirect_draw_buffer_handle = vulkan_buffer_create_frame(renderer, renderer->buffer_system,
                                                                           BUFFER_TYPE_INDIRECT,
                                                                           sizeof(VkDrawIndexedIndirectCommand) *
                                                                           100);


    hash_set_insert(shader_system->shader_batch_hash_set, &shader_batch->material_key);
}

void vulkan_shader_system_shader_batch_free(Renderer* renderer, Vulkan_Shader_System* shader_system,
                                            const char* shader_name)
{
    //TODO:
}

Vulkan_Shader_Batch* vulkan_shader_system_shader_get_by_key(Renderer* renderer, u64 sort_key)
{
    //TODO: make a loop for now, but use a hash map later
    MASSERT(false);
}

