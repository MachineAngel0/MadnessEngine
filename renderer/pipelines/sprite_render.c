#include "sprite_render.h"
#include "vk_buffer.h"


Sprite_Renderer* sprite_render_init(Renderer* renderer)
{
    Sprite_Renderer* sprite_backend = allocator_alloc(&renderer->allocator, sizeof(Sprite_Renderer));


    sprite_backend->sprite_ssbo = vulkan_buffer_create_frame(renderer, renderer->buffer_system,
                                                             BUFFER_TYPE_STORAGE_GPU,
                                                             sizeof(Sprite_Data) * (MAX_SPRITE_COUNT + MAX_SPRITE_TRANSIENT_COUNT));



    //only needs to happen once
    u16 sprite_indices[6];
    memcpy(sprite_indices, default_sprite_indices, sizeof(default_sprite_indices));

    sprite_backend->index_type = VK_INDEX_TYPE_UINT16;



    sprite_backend->sprite_index_buffer = vulkan_buffer_create(renderer, renderer->buffer_system, BUFFER_TYPE_INDEX,
                                                               sizeof(default_sprite_indices));


    vulkan_buffer_startup_uploads(renderer, sprite_backend->sprite_index_buffer,
                                  sprite_indices,
                                  sizeof(u16) * 6);


    return sprite_backend;
}


void sprite_upload_draw_data(Renderer* renderer, Sprite_Renderer* sprite_backend,
                             Render_Packet_Sprite* sprite_render_packet, Vulkan_Command_Buffer* command_buffer)
{
    MASSERT(renderer)
    MASSERT(sprite_backend)
    MASSERT(sprite_render_packet)

    vulkan_command_buffer_debug_label_begin(renderer, command_buffer, "SPRITE SSBO UPDATE");

    sprite_backend->draw_count = sprite_render_packet->sprite_data->num_items + sprite_render_packet->
        sprite_data_transient->num_items;


    //one for normal sprite data and one for transient data right after
    vulkan_buffer_frame_staging_upload(renderer, sprite_backend->sprite_ssbo,
                                       command_buffer,
                                       sprite_render_packet->sprite_data->data,
                                       array_get_byte_size(sprite_render_packet->sprite_data));
    vulkan_buffer_frame_staging_upload(renderer, sprite_backend->sprite_ssbo,
                                       command_buffer,
                                       sprite_render_packet->sprite_data_transient->data,
                                       array_get_byte_size(
                                           sprite_render_packet->sprite_data_transient));


    VkBufferMemoryBarrier2 sprite_draw_data_barrier = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
        .srcStageMask = VK_PIPELINE_STAGE_2_COPY_BIT,
        .srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,

        .dstStageMask = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
        .dstAccessMask = VK_ACCESS_2_SHADER_STORAGE_READ_BIT,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,

        .buffer = vulkan_buffer_get_frame(renderer, sprite_backend->sprite_ssbo)->handle,
        .offset = 0,
        .size = VK_WHOLE_SIZE,
    };

    vulkan_command_add_buffer_barrier(command_buffer, sprite_draw_data_barrier);
    vulkan_command_flush_barriers(command_buffer);


    vulkan_command_buffer_debug_label_end(renderer, command_buffer);





}


void sprite_renderer_draw(Renderer* renderer, Sprite_Renderer* sprite_backend, Vulkan_Command_Buffer* command_buffer)
{
    MASSERT(sprite_backend)


    vulkan_command_buffer_debug_label_begin(renderer, command_buffer, "SPRITE DRAW");

    Vulkan_Buffer* index_buffer = vulkan_buffer_get(renderer, sprite_backend->sprite_index_buffer);


    vkCmdBindPipeline(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      renderer->sprite_pipeline.handle);

    //uniform
    vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            renderer->sprite_pipeline.pipeline_layout, 0, 1,
                            &renderer->descriptor_system->uniform_descriptors.descriptor_sets[renderer->
                                current_frame_index],
                            0, 0);

    //textures
    vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            renderer->sprite_pipeline.pipeline_layout, 1, 1,
                            &renderer->descriptor_system->texture_descriptors.descriptor_sets[0], 0, 0);

    //storage buffers
    vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            renderer->sprite_pipeline.pipeline_layout, 2, 1,
                            &renderer->descriptor_system->storage_descriptors.descriptor_sets[renderer->
                                current_frame_index],
                            0, 0);



    PC_Sprite pc_sprite = {
        .sprite_data_ssbo = vulkan_buffer_get_frame_device_address(renderer, sprite_backend->sprite_ssbo),
        .padding0 = 0,
        .padding1 = 0,
    };

    VkPushConstantsInfo push_constant_info = {0};
    push_constant_info.sType = VK_STRUCTURE_TYPE_PUSH_CONSTANTS_INFO;
    push_constant_info.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    push_constant_info.layout = renderer->sprite_pipeline.pipeline_layout;
    push_constant_info.offset = 0;
    push_constant_info.size = sizeof(PC_Sprite);
    push_constant_info.pValues = &pc_sprite;
    push_constant_info.pNext = NULL;

    vkCmdPushConstants2(command_buffer->handle, &push_constant_info);



    // vkCmdBindIndexBuffer(command_buffer->handle,
                         // index_buffer->handle, 0,
                         // sprite_backend->index_type);



    vkCmdDraw(command_buffer->handle,
                         6,
                         sprite_backend->draw_count,
                         0,
                         0);


    //we would want this if we wanted unique materials on the sprites
    /*if (renderer->features2.features.multiDrawIndirect)
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
    }*/



    vulkan_command_buffer_debug_label_end(renderer, command_buffer);


}
