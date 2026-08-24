#include "ui_render.h"

#include "vk_buffer.h"
#include "vk_command_buffer.h"


UI_Renderer_Backend* ui_render_init(Renderer* renderer)
{
    UI_Renderer_Backend* ui_renderer = allocator_alloc(&renderer->allocator, sizeof(UI_Renderer_Backend));

    //TODO: i should replace this with object counts // like max 1000 UI's on screen until i need to otherwise
    u32 ui_buffer_sizes = MB(1);

    ui_renderer->ui_vertex_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                BUFFER_TYPE_VERTEX, ui_buffer_sizes);
    ui_renderer->ui_index_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                               BUFFER_TYPE_INDEX, ui_buffer_sizes);

    ui_renderer->ui_material_ssbo_handle = vulkan_buffer_create_frame(renderer, renderer->buffer_system,
                                                                      BUFFER_TYPE_STORAGE_GPU,
                                                                      ui_buffer_sizes);
    /*ui_renderer->ui_material_ssbo_handle = vulkan_buffer_create_frame(renderer, renderer->buffer_system,
                                                                      BUFFER_TYPE_STORAGE_GPU, ui_buffer_sizes);*/


    //UI Render
    vulkan_buffer_startup_uploads(renderer,
                                  ui_renderer->ui_vertex_buffer_handle,
                                  default_sprite,
                                  sizeof(Sprite) * 4);

    vulkan_buffer_startup_uploads(renderer,
                                  ui_renderer->ui_index_buffer_handle,
                                  default_sprite_indices,
                                  sizeof(u16) * 6);


    //insanity ui
    ui_renderer->insanity_ui_material_ssbo_handle = vulkan_buffer_create_frame(renderer, renderer->buffer_system,
                                                                               BUFFER_TYPE_STORAGE_GPU,
                                                                               ui_buffer_sizes);

    return ui_renderer;
}


void ui_renderer_upload_draw_data(UI_Renderer_Backend* ui_renderer, Renderer* renderer,
                                  Render_Packet* render_packet,
                                  Vulkan_Command_Buffer* command_buffer)
{
    ui_renderer->madness_ui_render_packet = &render_packet->ui_data_packet.madness_ui_render_packet;
    ui_renderer->insanity_ui_render_packet = &render_packet->ui_data_packet.insanity_ui_render_packet;

    // reset material buffer
    vulkan_buffer_frame_reset(renderer, ui_renderer->ui_material_ssbo_handle);
    vulkan_buffer_frame_reset(renderer, ui_renderer->insanity_ui_material_ssbo_handle);

    vulkan_command_buffer_debug_label_color_begin(renderer, command_buffer, "Material SSBO UPLOAD",
                                                  (float[4]){1.0, 0.0, 1.0, 1.0});


    // ui material data
    vulkan_buffer_frame_staging_upload(renderer,
                                       ui_renderer->ui_material_ssbo_handle,
                                       command_buffer,
                                       ui_renderer->madness_ui_render_packet->ui_material_data,
                                       ui_renderer->madness_ui_render_packet->ui_material_bytes);

    vulkan_buffer_frame_staging_upload(renderer,
                                       ui_renderer->insanity_ui_material_ssbo_handle,
                                       command_buffer,
                                       ui_renderer->insanity_ui_render_packet->ui_material_data,
                                       ui_renderer->insanity_ui_render_packet->ui_material_bytes);

    Vulkan_Buffer* madness_material_buffer = vulkan_buffer_get_frame(renderer, ui_renderer->ui_material_ssbo_handle);
    Vulkan_Buffer* insanity_material_buffer = vulkan_buffer_get_frame(
        renderer, ui_renderer->insanity_ui_material_ssbo_handle);

    VkBufferMemoryBarrier2 madness_barrier = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
        .srcStageMask = VK_PIPELINE_STAGE_2_COPY_BIT,
        .srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,

        .dstStageMask = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT |
        VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
        .dstAccessMask = VK_ACCESS_2_SHADER_STORAGE_READ_BIT,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,

        .buffer = madness_material_buffer->handle,
        .offset = 0,
        .size = VK_WHOLE_SIZE,
    };
    VkBufferMemoryBarrier2 insanity_barrier = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,

        .srcStageMask = VK_PIPELINE_STAGE_2_COPY_BIT,
        .srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,

        .dstStageMask = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT |
        VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
        .dstAccessMask = VK_ACCESS_2_SHADER_STORAGE_READ_BIT,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,

        .buffer = insanity_material_buffer->handle,
        .offset = 0,
        .size = VK_WHOLE_SIZE,
    };

    vulkan_command_add_buffer_barrier(command_buffer, madness_barrier);
    vulkan_command_add_buffer_barrier(command_buffer, insanity_barrier);
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
}

void ui_renderer_madness_draw(UI_Renderer_Backend* ui_renderer, Renderer* renderer,
                              Vulkan_Command_Buffer* command_buffer)
{
    // Vulkan_Buffer* vert_buffer = vulkan_buffer_get(renderer, ui_renderer->ui_vertex_buffer_handle);
    // Vulkan_Buffer* index_buffer = vulkan_buffer_get(renderer, ui_renderer->ui_index_buffer_handle);


    //uniform
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

    PC_UI pc_ui = {
        .material_bda =
        get_buffer_device_address(renderer->logical_device,
                                  vulkan_buffer_get_frame(renderer, ui_renderer->ui_material_ssbo_handle)->handle),
        .padding1 = 0,
        .padding2 = 0,

    };


    VkPushConstantsInfo push_constant_info_ui = {0};
    push_constant_info_ui.sType = VK_STRUCTURE_TYPE_PUSH_CONSTANTS_INFO;
    push_constant_info_ui.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    push_constant_info_ui.layout = renderer->ui_pipeline.pipeline_layout;
    push_constant_info_ui.offset = 0;
    push_constant_info_ui.size = sizeof(PC_UI);
    push_constant_info_ui.pValues = &pc_ui;
    push_constant_info_ui.pNext = NULL;

    vkCmdBindPipeline(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      renderer->ui_pipeline.handle);
    vkCmdPushConstants2(command_buffer->handle, &push_constant_info_ui);

    //two pipelines are slower than one, and much much faster if I use instancing for them

    //draw
    for (u32 i = 0; i < ui_renderer->madness_ui_render_packet->draw_command_count; i++)
    {
        switch (ui_renderer->madness_ui_render_packet->draw_command[i].type)
        {
        case UI_DRAW_TYPE_DRAW:
            //bind pipeline
            //push constant, issue draw at material index
            // firstInstance -> gl_InstanceIndex
            // vkCmdDraw(command_buffer->handle, 6, 1, 0, i);
            vkCmdDraw(command_buffer->handle, 6,
                      ui_renderer->madness_ui_render_packet->draw_command[i].count, 0,
                      ui_renderer->madness_ui_render_packet->draw_command[i].offset);

            break;
        case UI_DRAW_TYPE_SCISSOR_START:
            // vec2 scissor_pos = ui_renderer->madness_ui_render_packet->draw_command[i].scissor_pos;
            // vec2 size = ui_renderer->madness_ui_render_packet->draw_command[i].scissor_size;
            // VkRect2D scissor = {
            // .offset = {.x = scissor_pos.x, .y = scissor_pos.y},
            // .extent = {.width = size.x, .height = size.y},
            // };
            // vkCmdSetScissor(command_buffer->handle, 0, 1, &scissor);
            break;
        case UI_DRAW_TYPE_SCISSOR_END:
            // VkRect2D default_scissor = {
            // .offset = {.x = 0, .y = 0},
            // .extent = {.width = renderer->context.framebuffer_width, .height = renderer->context.framebuffer_height},
            // };
            // vkCmdSetScissor(command_buffer->handle, 0, 1, &default_scissor);
            break;
        }
    }


    //insanity_ui


    PC_UI pc_insanity_ui = {
        .material_bda = get_buffer_device_address(renderer->logical_device,
                                                  vulkan_buffer_get_frame(
                                                      renderer, ui_renderer->insanity_ui_material_ssbo_handle)->
                                                  handle),
        .padding1 = 0,
        .padding2 = 0,

    };


    VkPushConstantsInfo insanity_pc_info = {0};
    insanity_pc_info.sType = VK_STRUCTURE_TYPE_PUSH_CONSTANTS_INFO;
    insanity_pc_info.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    insanity_pc_info.layout = renderer->ui_pipeline.pipeline_layout;
    insanity_pc_info.offset = 0;
    insanity_pc_info.size = sizeof(PC_UI);
    insanity_pc_info.pValues = &pc_insanity_ui;
    insanity_pc_info.pNext = NULL;

    vkCmdBindPipeline(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      renderer->ui_pipeline.handle);
    vkCmdPushConstants2(command_buffer->handle, &insanity_pc_info);

    //two pipelines are slower than one, and much faster if I use instancing for them

    //draw
    for (u32 i = 0; i < ui_renderer->insanity_ui_render_packet->draw_command_count; i++)
    {
        switch (ui_renderer->insanity_ui_render_packet->draw_command[i].type)
        {
        case UI_DRAW_TYPE_DRAW:
            //bind pipeline
            //push constant, issue draw at material index
            // firstInstance -> gl_InstanceIndex
            // vkCmdDraw(command_buffer->handle, 6, 1, 0, i);
            vkCmdDraw(command_buffer->handle, 6,
                      ui_renderer->insanity_ui_render_packet->draw_command[i].count, 0,
                      ui_renderer->insanity_ui_render_packet->draw_command[i].offset);

            break;
        case UI_DRAW_TYPE_SCISSOR_START:
            // vec2 scissor_pos = ui_renderer->madness_ui_render_packet->draw_command[i].scissor_pos;
            // vec2 size = ui_renderer->madness_ui_render_packet->draw_command[i].scissor_size;
            // VkRect2D scissor = {
            // .offset = {.x = scissor_pos.x, .y = scissor_pos.y},
            // .extent = {.width = size.x, .height = size.y},
            // };
            // vkCmdSetScissor(command_buffer->handle, 0, 1, &scissor);
            break;
        case UI_DRAW_TYPE_SCISSOR_END:
            // VkRect2D default_scissor = {
            // .offset = {.x = 0, .y = 0},
            // .extent = {.width = renderer->context.framebuffer_width, .height = renderer->context.framebuffer_height},
            // };
            // vkCmdSetScissor(command_buffer->handle, 0, 1, &default_scissor);
            break;
        }
    }
}
