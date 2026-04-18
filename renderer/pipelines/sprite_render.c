#include "sprite_render.h"




Sprite_Renderer* sprite_render_init(Renderer* renderer, Resource_System* resource_system)
{
    Sprite_Renderer* sprite_backend = arena_alloc(&renderer->arena, sizeof(Sprite_Renderer));
    //TODO: move out the memory capacity to the function params or get from sprite system/resource system
    u64 buffer_memory_size = MB(1);

    sprite_backend->index_type = VK_INDEX_TYPE_UINT16;

    sprite_backend->sprite_vertex_buffer = vulkan_buffer_create(renderer, renderer->buffer_system, BUFFER_TYPE_VERTEX,
                                                                buffer_memory_size);

    sprite_backend->sprite_index_buffer = vulkan_buffer_create(renderer, renderer->buffer_system, BUFFER_TYPE_INDEX,
                                                               buffer_memory_size);
    sprite_backend->sprite_instance_ssbo_buffer = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                       BUFFER_TYPE_CPU_STORAGE,
                                                                       buffer_memory_size);

    sprite_backend->sprite_indirect_buffer = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                  BUFFER_TYPE_INDIRECT, buffer_memory_size);

    //staging
    sprite_backend->sprite_vertex_staging_buffer = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                        BUFFER_TYPE_STAGING, buffer_memory_size);
    sprite_backend->sprite_index_staging_buffer = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                       BUFFER_TYPE_STAGING, buffer_memory_size);
    sprite_backend->sprite_instance_staging_buffer = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                          BUFFER_TYPE_STAGING, buffer_memory_size);
    sprite_backend->sprite_indirect_staging_buffer = vulkan_buffer_create(
        renderer, renderer->buffer_system, BUFFER_TYPE_STAGING, buffer_memory_size);

    return sprite_backend;
}


void sprite_upload_draw_data(Renderer* renderer, Sprite_Renderer* sprite_backend,
                             Render_Packet_Sprite* sprite_render_packet, vulkan_command_buffer* command_buffer)
{
    MASSERT(renderer)
    MASSERT(sprite_backend)
    MASSERT(sprite_render_packet)

    vulkan_buffer_reset_offset(renderer, sprite_backend->sprite_vertex_staging_buffer);
    vulkan_buffer_reset_offset(renderer, sprite_backend->sprite_index_staging_buffer);
    vulkan_buffer_reset_offset(renderer, sprite_backend->sprite_instance_staging_buffer);
    vulkan_buffer_reset_offset(renderer, sprite_backend->sprite_indirect_staging_buffer);


    sprite_backend->draw_count = sprite_render_packet->sprite_data->num_items + sprite_render_packet->
        sprite_data_transient->num_items;

    vulkan_buffer_cpu_to_gpu_copy_and_upload_batch(renderer, sprite_backend->sprite_vertex_buffer,
                                                   sprite_backend->sprite_vertex_staging_buffer, command_buffer,
                                                   &sprite_render_packet->sprite_data->data, sizeof(Sprite) * 4);

    vulkan_buffer_cpu_to_gpu_copy_and_upload_batch(renderer, sprite_backend->sprite_index_buffer,
                                                   sprite_backend->sprite_index_staging_buffer, command_buffer,
                                                   sprite_render_packet->sprite_indices,
                                                   sizeof(u16) * 6);

    //one for normal sprite data and one for transient data
    vulkan_buffer_cpu_to_gpu_copy_and_upload_batch(renderer, sprite_backend->sprite_instance_ssbo_buffer,
                                                   sprite_backend->sprite_instance_staging_buffer, command_buffer,
                                                   sprite_render_packet->sprite_data->data,
                                                   Sprite_Data_array_get_bytes_used(sprite_render_packet->sprite_data));
    vulkan_buffer_cpu_to_gpu_copy_and_upload_batch(renderer, sprite_backend->sprite_instance_ssbo_buffer,
                                                   sprite_backend->sprite_instance_staging_buffer, command_buffer,
                                                   sprite_render_packet->sprite_data_transient->data,
                                                   Sprite_Data_array_get_bytes_used(
                                                       sprite_render_packet->sprite_data_transient));

    //generate indirect draw data
    //basically just a bunch of instances with indexes into the instance data buffer

    //literally only need one
    VkDrawIndexedIndirectCommand sprite_indirect_draw;
    u64 sprite_count = sprite_backend->draw_count;

    sprite_indirect_draw.firstIndex = 0;
    sprite_indirect_draw.firstInstance = 0;
    sprite_indirect_draw.vertexOffset = 0; // one quad is 2 triangles / 6 vertex's
    sprite_indirect_draw.indexCount = ARRAY_SIZE(default_sprite_indices);
    sprite_indirect_draw.instanceCount = sprite_count;


    vulkan_buffer_cpu_to_gpu_copy_and_upload_batch(renderer, sprite_backend->sprite_indirect_buffer,
                                                   sprite_backend->sprite_indirect_staging_buffer, command_buffer,
                                                   &sprite_indirect_draw,
                                                   sizeof(VkDrawIndexedIndirectCommand));
}


void sprite_renderer_draw(Renderer* renderer, Sprite_Renderer* sprite_backend, vulkan_command_buffer* command_buffer)
{
    MASSERT(sprite_backend)

    INFO("Sprite DRAW CALLS")

    u64 sprite_draw_count = sprite_backend->draw_count;

    Vulkan_Buffer* vert_buffer = vulkan_buffer_get(renderer, sprite_backend->sprite_vertex_buffer);
    Vulkan_Buffer* index_buffer = vulkan_buffer_get(renderer, sprite_backend->sprite_index_buffer);
    Vulkan_Buffer* sprite_indirect_buffer = vulkan_buffer_get(renderer, sprite_backend->sprite_indirect_buffer);

    vkCmdBindPipeline(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      renderer->sprite_pipeline.handle);

    //uniform
    vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            renderer->sprite_pipeline.pipeline_layout, 0, 1,
                            &renderer->descriptor_system->uniform_descriptors.descriptor_sets[renderer->context.
                                current_frame], 0, 0);

    //textures
    vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            renderer->sprite_pipeline.pipeline_layout, 1, 1,
                            &renderer->descriptor_system->texture_descriptors.descriptor_sets[0], 0, 0);

    //storage buffers
    vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            renderer->sprite_pipeline.pipeline_layout, 2, 1,
                            &renderer->descriptor_system->storage_descriptors.descriptor_sets[renderer->context.
                                current_frame], 0, 0);


    //grab material_handle
    PC_2D pc_2d_ui = {
        renderer->buffer_system->global_ubo_handle.handle,
        sprite_backend->sprite_instance_ssbo_buffer.handle,
    };

    VkPushConstantsInfo push_constant_info_ui = {0};
    push_constant_info_ui.sType = VK_STRUCTURE_TYPE_PUSH_CONSTANTS_INFO;
    push_constant_info_ui.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    push_constant_info_ui.layout = renderer->sprite_pipeline.pipeline_layout;
    push_constant_info_ui.offset = 0;
    push_constant_info_ui.size = sizeof(PC_2D);
    push_constant_info_ui.pValues = &pc_2d_ui;
    push_constant_info_ui.pNext = NULL;

    vkCmdPushConstants2(command_buffer->handle, &push_constant_info_ui);


    VkDeviceSize offsets_bindless[1] = {0};
    vkCmdBindVertexBuffers(command_buffer->handle, 0, 1,
                           &vert_buffer->handle, offsets_bindless);

    vkCmdBindIndexBuffer(command_buffer->handle,
                         index_buffer->handle, 0,
                         sprite_backend->index_type
    );

    //we use one for the draw count since we are instancing the sprites
    if (renderer->context.device.features.multiDrawIndirect)
    {
        vkCmdDrawIndexedIndirect(command_buffer->handle,
                                 sprite_indirect_buffer->handle, 0,
                                 1,
                                 sizeof(VkDrawIndexedIndirectCommand));
    }
    else
    {
        // If multi draw is not available, we must issue separate draw commands
        for (u64 j = 0; j < 1; j++)
        {
            vkCmdDrawIndexedIndirect(command_buffer->handle,
                                     sprite_indirect_buffer->handle,
                                     j * sizeof(VkDrawIndexedIndirectCommand), 1,
                                     sizeof(VkDrawIndexedIndirectCommand));
        }
    }
}


