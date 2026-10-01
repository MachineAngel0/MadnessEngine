#ifndef CHAOS_UI_RENDERER_H
#define CHAOS_UI_RENDERER_H

#include "vulkan_struct_types.h"
#include "ui_chaos_structs_enums.h"


typedef struct PC_Chaos_UI
{
    VkDeviceAddress quad_ssbo_handle;
    u64 padding1;
    u64 padding2;
} PC_Chaos_UI;


Chaos_UI_Renderer_Backend* chaos_ui_render_init(Renderer* renderer)
{
    Chaos_UI_Renderer_Backend* cui_renderer = allocator_alloc(&renderer->allocator, sizeof(Chaos_UI_Renderer_Backend));

    //TODO: i should replace this with object counts // like max 1000 UI's on screen until i need to otherwise
    u32 ui_quad_vertex_size = sizeof(Sprite) * 4;
    u32 ui_quad_index_size = sizeof(u16) * 6;

    cui_renderer->ui_vertex_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                 BUFFER_TYPE_VERTEX, ui_quad_vertex_size);
    cui_renderer->ui_index_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                BUFFER_TYPE_INDEX, ui_quad_index_size);


    /*ui_renderer->ui_material_ssbo_handle = vulkan_buffer_create_frame(renderer, renderer->buffer_system,
                                                                      BUFFER_TYPE_STORAGE_GPU, ui_buffer_sizes);*/


    //single vertex's
    vulkan_buffer_startup_uploads(renderer,
                                  cui_renderer->ui_vertex_buffer_handle,
                                  default_sprite,
                                  ui_quad_vertex_size);

    vulkan_buffer_startup_uploads(renderer,
                                  cui_renderer->ui_index_buffer_handle,
                                  default_sprite_indices,
                                  ui_quad_index_size);


    //all the material ssbo's
    u32 ui_buffer_sizes = sizeof(CUI_Render_Item) * CHAOS_UI_MAX_NODE_COUNT;

    cui_renderer->cui_ssbo_handle = vulkan_buffer_create_frame(renderer, renderer->buffer_system,
                                                               BUFFER_TYPE_STORAGE_GPU,
                                                               ui_buffer_sizes);


    vulkan_pipeline_graphics_create(renderer, "CUI", Shader_Blend_Mode_Alpha,false,
                                    &cui_renderer->cui_pipeline,
                                    &cui_renderer->cui_wireframe_pipeline);


    return cui_renderer;
}

void chaos_ui_renderer_upload_draw_data(Chaos_UI_Renderer_Backend* cui_renderer, Renderer* renderer,
                                        Chaos_UI_Render_Packet* cui_render_packet,
                                        Vulkan_Command_Buffer* command_buffer)
{
    PROFILE_ZONE(ui_renderer_upload_draw_data)

    cui_renderer->cui_render_packet = cui_render_packet;


    // reset material buffer
    vulkan_buffer_frame_reset(renderer, cui_renderer->cui_ssbo_handle);


    vulkan_command_buffer_debug_label_begin_color(renderer, command_buffer, "CHAOS UI SSBO UPLOAD",
                                                  (float[4]){1.0, 0.0, 1.0, 1.0});


    // ui material data
    vulkan_buffer_frame_staging_upload(renderer,
                                       cui_renderer->cui_ssbo_handle,
                                       command_buffer,
                                       cui_render_packet->render_items,
                                       cui_render_packet->render_items_byte_size);


    Vulkan_Buffer* cui_ssbo = vulkan_buffer_get_frame(renderer, cui_renderer->cui_ssbo_handle);


    VkBufferMemoryBarrier2 cui_barrier = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
        .srcStageMask = VK_PIPELINE_STAGE_2_COPY_BIT,
        .srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,

        .dstStageMask = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT |
        VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
        .dstAccessMask = VK_ACCESS_2_SHADER_STORAGE_READ_BIT,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,

        .buffer = cui_ssbo->handle,
        .offset = 0,
        .size = VK_WHOLE_SIZE,
    };


    vulkan_command_add_buffer_barrier(command_buffer, cui_barrier);

    vulkan_command_flush_barriers(command_buffer);

    vulkan_command_buffer_debug_label_end(renderer, command_buffer);


    //need one per draw call
    /*VkDrawIndexedIndirectCommand indirect_draw_ui = {0};
    for (u32 i = 0; i < ui_renderer->madness_ui_render_packet->draw_command_count; i++)
    {
        UI_Draw_Command* draw_command = &ui_renderer->madness_ui_render_packet->draw_command[i];
        if (draw_command->type != UI_DRAW_TYPE_DRAW) {continue;}

        indirect_draw_ui.firstIndex = 0;
        indirect_draw_ui.firstInstance = draw_command->offset;
        indirect_draw_ui.vertexOffset = 0; // one quad is 2 triangles / 6 vertex's
        indirect_draw_ui.indexCount = ARRAY_SIZE(default_sprite_indices);
        indirect_draw_ui.instanceCount = draw_command->count;

        //buffer upload, TODO: redo buffers from the ground up again.
    }*/

    PROFILE_ZONE_END(ui_renderer_upload_draw_data)
}

void chaos_ui_renderer_draw(Chaos_UI_Renderer_Backend* ui_renderer, Renderer* renderer,
                            Vulkan_Command_Buffer* command_buffer)
{
    vulkan_command_buffer_debug_label_begin_color(renderer, command_buffer, "CHAOS UI DRAW",
                                                  (float[4]){1.0, 0.0, 1.0, 1.0});

    //global uniform
    vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            renderer->ui_pipeline.pipeline_layout, 0, 1,
                            &renderer->descriptor_system->uniform_descriptors.descriptor_sets[renderer->current_frame],
                            0, 0);

    //textures
    vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            renderer->ui_pipeline.pipeline_layout, 1, 1,
                            &renderer->descriptor_system->texture_descriptors.descriptor_sets[0], 0, 0);

    //storage buffers
    vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            renderer->ui_pipeline.pipeline_layout, 2, 1,
                            &renderer->descriptor_system->storage_descriptors.descriptor_sets[renderer->current_frame],
                            0, 0);

    PC_Chaos_UI pc_cui = {
        .quad_ssbo_handle =
        get_buffer_device_address(renderer->logical_device,
                                  vulkan_buffer_get_frame(renderer, ui_renderer->cui_ssbo_handle)->handle),
        .padding1 = 0,
        .padding2 = 0,

    };


    VkPushConstantsInfo push_constant_info_ui = {0};
    push_constant_info_ui.sType = VK_STRUCTURE_TYPE_PUSH_CONSTANTS_INFO;
    push_constant_info_ui.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    push_constant_info_ui.layout = renderer->ui_pipeline.pipeline_layout;
    push_constant_info_ui.offset = 0;
    push_constant_info_ui.size = sizeof(PC_Chaos_UI);
    push_constant_info_ui.pValues = &pc_cui;
    push_constant_info_ui.pNext = NULL;


    vkCmdBindPipeline(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      ui_renderer->cui_pipeline.handle);

    vkCmdPushConstants2(command_buffer->handle, &push_constant_info_ui);


    if (renderer->wireframe_mode)
    {
        vkCmdBindPipeline(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          ui_renderer->cui_wireframe_pipeline.handle);
    }

    for (u32 i = 0; i < ui_renderer->cui_render_packet->render_command_count; i++)
    {
        Chaos_UI_Render_Command* command = &ui_renderer->cui_render_packet->render_command_array[i];

        switch (command->type)
        {
        case Chaos_UI_Render_Command_Type_Scissor:
            vec2s pos = ui_renderer->cui_render_packet->render_items[command->buffer_offset].pos;
            vec2s size = ui_renderer->cui_render_packet->render_items[command->buffer_offset].size;
            VkRect2D default_scissor = {
                .offset = {.x = pos.x, .y = pos.y},
                .extent = {.width = size.x, .height = size.y},
            };
            vkCmdSetScissor(command_buffer->handle, 0, 1, &default_scissor);
            break;
        case Chaos_UI_Render_Command_Type_Draw:
            //bind pipeline
            //push constant, has render item ssbo
            //issue draw, use instancing and offset into buffer


            vkCmdDraw(command_buffer->handle, 6,
                      command->draw_count, 0,
                      command->buffer_offset);

            //TODO: draw indexed
            /*vkCmdDrawIndexed(command_buffer->handle, 6,
                      command->draw_count, 0,
                      command->buffer_offset);*/

            break;
        }
    }


    vulkan_command_buffer_debug_label_end(renderer, command_buffer);
}


#endif //CHAOS_UI_RENDERER_H
