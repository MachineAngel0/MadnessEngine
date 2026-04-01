#include "render_resource_backend.h"

bool renderer_resource_backend_init(Renderer* renderer, Resource_System* resource_system)
{

    renderer->sprite_backend = sprite_render_backend_init(renderer, resource_system);
    renderer->ui_renderer = ui_render_init(renderer);
    return true;

}

bool renderer_resource_backend_shutdown(Renderer* renderer, Resource_System* resource_system)
{
    return true;
}


Sprite_Renderer_Backend* sprite_render_backend_init(Renderer* renderer, Resource_System* resource_system)
{
    Sprite_Renderer_Backend* sprite_backend = arena_alloc(&renderer->arena, sizeof(Sprite_Renderer_Backend));
    //TODO: move out the memory capacity to the function params or get from sprite system/resource system
    u64 memory_capacity = MB(1);

    sprite_backend->index_type = VK_INDEX_TYPE_UINT16;

    sprite_backend->sprite_vertex_buffer = vulkan_buffer_create(renderer, renderer->buffer_system, BUFFER_TYPE_VERTEX,
                                                                memory_capacity);

    sprite_backend->sprite_index_buffer = vulkan_buffer_create(renderer, renderer->buffer_system, BUFFER_TYPE_INDEX,
                                                               memory_capacity);
    sprite_backend->sprite_indirect_buffer = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                  BUFFER_TYPE_INDIRECT, memory_capacity);

    sprite_backend->sprite_vertex_staging_buffer = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                        BUFFER_TYPE_STAGING, memory_capacity);
    sprite_backend->sprite_index_staging_buffer = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                       BUFFER_TYPE_STAGING, memory_capacity);
    sprite_backend->sprite_indirect_staging_buffer = vulkan_buffer_create(
        renderer, renderer->buffer_system, BUFFER_TYPE_STAGING, memory_capacity);

    return sprite_backend;
}


void sprite_upload_draw_data(Renderer* renderer, Sprite_System* sprite_system, Sprite_Renderer_Backend* sprite_backend)
{
    MASSERT(sprite_system)

    sprite_backend->draw_count = sprite_system->sprites_data->num_items;

    vulkan_buffer_data_copy_and_upload(renderer, sprite_backend->sprite_vertex_buffer,
                                       sprite_backend->sprite_vertex_staging_buffer,
                                       &sprite_system->sprites, sizeof(Sprite) * 4);

    vulkan_buffer_data_copy_and_upload(renderer, sprite_backend->sprite_index_buffer,
                                       sprite_backend->sprite_index_staging_buffer,
                                       sprite_system->sprite_indices,
                                       sizeof(u16) * 6);

    vulkan_buffer_data_copy_and_upload(renderer, sprite_backend->sprite_instance_buffer,
                                       sprite_backend->sprite_instance_staging_buffer,
                                       sprite_system->sprites_data->data,
                                       Sprite_Data_array_get_bytes_used(sprite_system->sprites_data));


    //generate indirect draw data
    //basically just a bunch of instances with indexes into the instance data buffer

    //literally only need one
    VkDrawIndexedIndirectCommand sprite_indirect_draw;
    u64 sprite_count = sprite_system->sprites_data->num_items;

    sprite_indirect_draw.firstIndex = 0;
    sprite_indirect_draw.firstInstance = 0;
    sprite_indirect_draw.vertexOffset = 0; // one quad is 2 triangles / 6 vertex's
    sprite_indirect_draw.indexCount = ARRAY_SIZE(default_sprite_indices);
    sprite_indirect_draw.instanceCount = sprite_count;


    vulkan_buffer_data_copy_and_upload(renderer, sprite_backend->sprite_indirect_buffer,
                                       sprite_backend->sprite_indirect_staging_buffer,
                                       &sprite_indirect_draw,
                                       sizeof(VkDrawIndexedIndirectCommand));
}


void sprite_draw(Renderer* renderer, Sprite_Renderer_Backend* sprite_backend, vulkan_command_buffer* command_buffer)
{
    MASSERT(sprite_backend)
    vulkan_buffer* vert_buffer = vulkan_buffer_get(renderer, sprite_backend->sprite_vertex_buffer);
    vulkan_buffer* index_buffer = vulkan_buffer_get(renderer, sprite_backend->sprite_index_buffer);
    vulkan_buffer* indirect_buffer = vulkan_buffer_get(renderer, sprite_backend->sprite_indirect_buffer);

    vkCmdBindPipeline(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      renderer->sprite_pipeline.handle);

    //uniform
    vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            renderer->ui_pipeline.pipeline_layout, 0, 1,
                            &renderer->descriptor_system->uniform_descriptors.descriptor_sets[renderer->context.
                                current_frame], 0, 0);

    //textures
    vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            renderer->ui_pipeline.pipeline_layout, 1, 1,
                            &renderer->descriptor_system->texture_descriptors.descriptor_sets[renderer->context.
                                current_frame], 0, 0);

    //storage buffers
    vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            renderer->ui_pipeline.pipeline_layout, 2, 1,
                            &renderer->descriptor_system->storage_descriptors.descriptor_sets[renderer->context.
                                current_frame], 0, 0);


    VkDeviceSize offsets_bindless[1] = {0};
    vkCmdBindVertexBuffers(command_buffer->handle, 0, 1,
                           &vert_buffer->handle, offsets_bindless);

    vkCmdBindIndexBuffer(command_buffer->handle,
                         index_buffer->handle, 0,
                         sprite_backend->index_type
    );

    u64 sprite_count = sprite_backend->draw_count;

    if (renderer->context.device.features.multiDrawIndirect)
    {
        vkCmdDrawIndexedIndirect(command_buffer->handle,
                                 indirect_buffer->handle, 0,
                                 sprite_count,
                                 sizeof(VkDrawIndexedIndirectCommand));
    }
    else
    {
        // If multi draw is not available, we must issue separate draw commands
        for (u64 j = 0; j < sprite_count; j++)
        {
            vkCmdDrawIndexedIndirect(command_buffer->handle,
                                     indirect_buffer->handle,
                                     j * sizeof(VkDrawIndexedIndirectCommand), 1,
                                     sizeof(VkDrawIndexedIndirectCommand));
        }
    }
}





UI_Renderer_Backend* ui_render_init(Renderer* renderer)
{
    UI_Renderer_Backend* UI_Render_Info = arena_alloc(&renderer->arena, sizeof(UI_Renderer_Backend));

    //TODO: i should replace this with object counts // like max 1000 UI's on screen until i need to otherwise
    u32 ui_buffer_sizes = MB(1);

    UI_Render_Info->ui_vertex_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                   BUFFER_TYPE_VERTEX, ui_buffer_sizes);
    UI_Render_Info->ui_index_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                  BUFFER_TYPE_INDEX, ui_buffer_sizes);
    UI_Render_Info->ui_indirect_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                     BUFFER_TYPE_INDIRECT, ui_buffer_sizes);
    UI_Render_Info->ui_instance_ssbo_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                   BUFFER_TYPE_CPU_STORAGE,
                                                                   ui_buffer_sizes);
    UI_Render_Info->text_vertex_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                     BUFFER_TYPE_VERTEX,
                                                                     ui_buffer_sizes);
    UI_Render_Info->text_index_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                    BUFFER_TYPE_INDEX,
                                                                    ui_buffer_sizes);
    UI_Render_Info->text_instance_ssbo_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                     BUFFER_TYPE_CPU_STORAGE,
                                                                     ui_buffer_sizes);
    UI_Render_Info->text_indirect_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                       BUFFER_TYPE_INDIRECT,
                                                                       ui_buffer_sizes);


    UI_Render_Info->ui_vertex_staging_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                           BUFFER_TYPE_STAGING, ui_buffer_sizes);
    UI_Render_Info->ui_index_staging_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                          BUFFER_TYPE_STAGING, ui_buffer_sizes);
    UI_Render_Info->ui_quad_indirect_staging_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
        BUFFER_TYPE_STAGING, ui_buffer_sizes);

    UI_Render_Info->ui_instance_staging_ssbo_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                           BUFFER_TYPE_STAGING,
                                                                           ui_buffer_sizes);


    UI_Render_Info->text_vertex_staging_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                             BUFFER_TYPE_STAGING,
                                                                             ui_buffer_sizes);
    UI_Render_Info->text_index_staging_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                            BUFFER_TYPE_STAGING,
                                                                            ui_buffer_sizes);
    UI_Render_Info->text_instance_staging_ssbo_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                             BUFFER_TYPE_STAGING,
                                                                             ui_buffer_sizes);
    UI_Render_Info->text_indirect_staging_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                               BUFFER_TYPE_STAGING,
                                                                               ui_buffer_sizes);
    return UI_Render_Info;
}


void ui_renderer_upload_draw_data(UI_Renderer_Backend* ui_renderer, Renderer* renderer,
                                          Render_Packet* render_packet)
{
    // UI
    vulkan_buffer_reset_offset(renderer, ui_renderer->ui_vertex_staging_buffer_handle);
    vulkan_buffer_reset_offset(renderer, ui_renderer->ui_index_staging_buffer_handle);
    vulkan_buffer_reset_offset(renderer, ui_renderer->ui_quad_indirect_staging_buffer_handle);
    vulkan_buffer_reset_offset(renderer, ui_renderer->ui_instance_staging_ssbo_handle);
    //text
    vulkan_buffer_reset_offset(renderer, ui_renderer->text_vertex_staging_buffer_handle);
    vulkan_buffer_reset_offset(renderer, ui_renderer->text_index_staging_buffer_handle);
    vulkan_buffer_reset_offset(renderer, ui_renderer->text_indirect_staging_buffer_handle);
    vulkan_buffer_reset_offset(renderer, ui_renderer->text_instance_staging_ssbo_handle);

    //UI Render
    vulkan_buffer_data_copy_and_upload(renderer,
                                       ui_renderer->ui_vertex_buffer_handle,
                                       ui_renderer->ui_vertex_staging_buffer_handle,
                                       default_sprite,
                                       sizeof(Sprite) * 4);

    vulkan_buffer_data_copy_and_upload(renderer,
                                       ui_renderer->ui_index_buffer_handle,
                                       ui_renderer->ui_index_staging_buffer_handle,
                                       default_sprite_indices,
                                       sizeof(u16) * 6);
    //material data
    vulkan_buffer_data_copy_and_upload(renderer,
                                       ui_renderer->ui_instance_ssbo_handle,
                                       ui_renderer->ui_instance_staging_ssbo_handle,
                                       render_packet->ui_data_packet.ui_data_packet->data,
                                       Sprite_Data_array_get_bytes_used(
                                           render_packet->ui_data_packet.ui_data_packet));
    //literally only need one
    VkDrawIndexedIndirectCommand indirect_draw_ui;

    indirect_draw_ui.firstIndex = 0;
    indirect_draw_ui.firstInstance = 0;
    indirect_draw_ui.vertexOffset = 0; // one quad is 2 triangles / 6 vertex's
    indirect_draw_ui.indexCount = ARRAY_SIZE(default_sprite_indices);
    indirect_draw_ui.instanceCount = render_packet->ui_data_packet.ui_data_packet->num_items;

    vulkan_buffer_data_copy_and_upload(renderer,
                                       ui_renderer->ui_indirect_buffer_handle,
                                       ui_renderer->ui_quad_indirect_staging_buffer_handle,
                                       &indirect_draw_ui,
                                       sizeof(VkDrawIndexedIndirectCommand)
    );


    //TEXT Render

    vulkan_buffer_data_copy_and_upload(renderer,
                                       ui_renderer->text_vertex_buffer_handle,
                                       ui_renderer->text_vertex_staging_buffer_handle,
                                       default_sprite,
                                       sizeof(Sprite) * 4);

    vulkan_buffer_data_copy_and_upload(renderer,
                                       ui_renderer->text_index_buffer_handle,
                                       ui_renderer->text_index_staging_buffer_handle,
                                       default_sprite_indices,
                                       sizeof(u16) * 6);
    //material data
    vulkan_buffer_data_copy_and_upload(renderer,
                                       ui_renderer->text_instance_ssbo_handle,
                                       ui_renderer->text_instance_staging_ssbo_handle,
                                       render_packet->ui_data_packet.text_data_packet->data,
                                       Sprite_Data_array_get_bytes_used(
                                           render_packet->ui_data_packet.text_data_packet));

    //generate indirect draws for text
    //literally only need one
    VkDrawIndexedIndirectCommand indirect_draw_text;

    indirect_draw_text.firstIndex = 0;
    indirect_draw_text.firstInstance = 0;
    indirect_draw_text.vertexOffset = 0; // one quad is 2 triangles / 6 vertex's
    indirect_draw_text.indexCount = ARRAY_SIZE(default_sprite_indices);
    indirect_draw_text.instanceCount = render_packet->ui_data_packet.text_data_packet->num_items;


    vulkan_buffer_data_copy_and_upload(renderer,
                                       ui_renderer->text_indirect_buffer_handle,
                                       ui_renderer->text_indirect_staging_buffer_handle,
                                       &indirect_draw_text,
                                       sizeof(VkDrawIndexedIndirectCommand));
}

void ui_renderer_draw(UI_Renderer_Backend* ui_renderer, Renderer* renderer, vulkan_command_buffer* command_buffer,
                      Render_Packet* render_packet)
{
    //does the draw
    u64 ui_draw_count = render_packet->ui_data_packet.ui_data_packet->num_items;
    u64 text_draw_count = render_packet->ui_data_packet.text_data_packet->num_items;

    // UI
    vulkan_buffer* vert_buffer = vulkan_buffer_get(renderer, ui_renderer->ui_vertex_buffer_handle);
    vulkan_buffer* index_buffer = vulkan_buffer_get(renderer, ui_renderer->ui_index_buffer_handle);
    vulkan_buffer* quad_indirect_buffer = vulkan_buffer_get(renderer, ui_renderer->ui_indirect_buffer_handle);


    vkCmdBindPipeline(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      renderer->ui_pipeline.handle);

    //uniform
    vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            renderer->ui_pipeline.pipeline_layout, 0, 1,
                            &renderer->descriptor_system->uniform_descriptors.descriptor_sets[renderer->context.
                                current_frame], 0, 0);

    //textures
    vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            renderer->ui_pipeline.pipeline_layout, 1, 1,
                            &renderer->descriptor_system->texture_descriptors.descriptor_sets[renderer->context.
                                current_frame], 0, 0);

    //storage buffers
    vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            renderer->ui_pipeline.pipeline_layout, 2, 1,
                            &renderer->descriptor_system->storage_descriptors.descriptor_sets[renderer->context.
                                current_frame], 0, 0);


    //grab material_handle
    PC_2D pc_2d_ui = {
        ui_renderer->ui_instance_ssbo_handle.handle,
    };

    VkPushConstantsInfo push_constant_info_ui = {0};
    push_constant_info_ui.sType = VK_STRUCTURE_TYPE_PUSH_CONSTANTS_INFO;
    push_constant_info_ui.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    push_constant_info_ui.layout = renderer->ui_pipeline.pipeline_layout;
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
                         VK_INDEX_TYPE_UINT16
    );

    // vkCmdDrawIndexed(command_buffer->handle,
    // Madness_UI->draw_info.index_count,
    // 1, 0, 0, 0);
    if (renderer->context.device.features.multiDrawIndirect)
    {
        vkCmdDrawIndexedIndirect(command_buffer->handle,
                                 quad_indirect_buffer->handle, 0,
                                 1,
                                 sizeof(VkDrawIndexedIndirectCommand));
    }
    else
    {
        // If multi draw is not available, we must issue separate draw commands
        for (u64 j = 0; j < 1; j++)
        {
            vkCmdDrawIndexedIndirect(command_buffer->handle,
                                     quad_indirect_buffer->handle,
                                     j * sizeof(VkDrawIndexedIndirectCommand), 1,
                                     sizeof(VkDrawIndexedIndirectCommand));
        }
    }

    // vkCmdDrawIndexed(command_buffer->handle,
    // Madness_UI->quad_draw_info.index_count,
    // 1, 0, 0, 0);


    //TEXT
    vulkan_buffer* text_vert_buffer = vulkan_buffer_get(renderer, ui_renderer->text_vertex_buffer_handle);
    vulkan_buffer* text_index_buffer = vulkan_buffer_get(renderer, ui_renderer->text_index_buffer_handle);
    vulkan_buffer* text_indirect = vulkan_buffer_get(renderer, ui_renderer->text_indirect_buffer_handle);


    vkCmdBindPipeline(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      renderer->text_pipeline.handle);

    //uniform
    vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            renderer->text_pipeline.pipeline_layout, 0, 1,
                            &renderer->descriptor_system->uniform_descriptors.descriptor_sets[renderer->context.
                                current_frame], 0, 0);

    //textures
    vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            renderer->text_pipeline.pipeline_layout, 1, 1,
                            &renderer->descriptor_system->texture_descriptors.descriptor_sets[renderer->context.
                                current_frame], 0, 0);

    //storage buffers
    vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            renderer->text_pipeline.pipeline_layout, 2, 1,
                            &renderer->descriptor_system->storage_descriptors.descriptor_sets[renderer->context.
                                current_frame], 0, 0);


    //grab material_handle
    PC_2D pc_2d_text = {
        ui_renderer->text_instance_ssbo_handle.handle,
    };

    VkPushConstantsInfo push_constant_info_text = {0};
    push_constant_info_text.sType = VK_STRUCTURE_TYPE_PUSH_CONSTANTS_INFO;
    push_constant_info_text.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    push_constant_info_text.layout = renderer->text_pipeline.pipeline_layout;
    push_constant_info_text.offset = 0;
    push_constant_info_text.size = sizeof(PC_2D);
    push_constant_info_text.pValues = &pc_2d_text;
    push_constant_info_text.pNext = NULL;

    vkCmdPushConstants2(command_buffer->handle, &push_constant_info_text);

    vkCmdBindVertexBuffers(command_buffer->handle, 0, 1,
                           &text_vert_buffer->handle, offsets_bindless);

    vkCmdBindIndexBuffer(command_buffer->handle,
                         text_index_buffer->handle, 0,
                         VK_INDEX_TYPE_UINT16);

    if (renderer->context.device.features.multiDrawIndirect)
    {
        vkCmdDrawIndexedIndirect(command_buffer->handle,
                                 text_indirect->handle, 0,
                                 1,
                                 sizeof(VkDrawIndexedIndirectCommand));
    }
    else
    {
        // If multi draw is not available, we must issue separate draw commands
        for (u64 j = 0; j < 1; j++)
        {
            vkCmdDrawIndexedIndirect(command_buffer->handle,
                                     text_indirect->handle,
                                     j * sizeof(VkDrawIndexedIndirectCommand), 1,
                                     sizeof(VkDrawIndexedIndirectCommand));
        }
    }
}

