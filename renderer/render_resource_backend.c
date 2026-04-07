#include "render_resource_backend.h"

bool renderer_resource_backend_init(Renderer* renderer, Resource_System* resource_system)
{

    renderer->sprite_renderer = sprite_render_init(renderer, resource_system);
    renderer->ui_renderer = ui_render_init(renderer);
    return true;

}

bool renderer_resource_backend_shutdown(Renderer* renderer, Resource_System* resource_system)
{
    return true;
}


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
    sprite_backend->sprite_instance_ssbo_buffer = vulkan_buffer_create(renderer, renderer->buffer_system, BUFFER_TYPE_CPU_STORAGE,
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


void sprite_upload_draw_data(Renderer* renderer, Sprite_Renderer* sprite_backend, Render_Packet_Sprite* sprite_render_packet)
{
    MASSERT(renderer)
    MASSERT(sprite_backend)
    MASSERT(sprite_render_packet)

    vulkan_buffer_reset_offset(renderer, sprite_backend->sprite_vertex_staging_buffer);
    vulkan_buffer_reset_offset(renderer, sprite_backend->sprite_index_staging_buffer);
    vulkan_buffer_reset_offset(renderer, sprite_backend->sprite_instance_staging_buffer);
    vulkan_buffer_reset_offset(renderer, sprite_backend->sprite_indirect_staging_buffer);


    sprite_backend->draw_count = sprite_render_packet->sprite_data->num_items + sprite_render_packet->sprite_data_transient->num_items;

    vulkan_buffer_data_copy_and_upload(renderer, sprite_backend->sprite_vertex_buffer,
                                       sprite_backend->sprite_vertex_staging_buffer,
                                       &sprite_render_packet->sprite_data->data, sizeof(Sprite) * 4);

    vulkan_buffer_data_copy_and_upload(renderer, sprite_backend->sprite_index_buffer,
                                       sprite_backend->sprite_index_staging_buffer,
                                       sprite_render_packet->sprite_indices,
                                       sizeof(u16) * 6);

    //one for normal sprite data and one for transient data
    vulkan_buffer_data_copy_and_upload(renderer, sprite_backend->sprite_instance_ssbo_buffer,
                                       sprite_backend->sprite_instance_staging_buffer,
                                       sprite_render_packet->sprite_data->data,
                                       Sprite_Data_array_get_bytes_used(sprite_render_packet->sprite_data));
    vulkan_buffer_data_copy_and_upload(renderer, sprite_backend->sprite_instance_ssbo_buffer,
                                         sprite_backend->sprite_instance_staging_buffer,
                                         sprite_render_packet->sprite_data_transient->data,
                                         Sprite_Data_array_get_bytes_used(sprite_render_packet->sprite_data_transient));

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


    vulkan_buffer_data_copy_and_upload(renderer, sprite_backend->sprite_indirect_buffer,
                                       sprite_backend->sprite_indirect_staging_buffer,
                                       &sprite_indirect_draw,
                                       sizeof(VkDrawIndexedIndirectCommand));
}


void sprite_draw(Renderer* renderer, Sprite_Renderer* sprite_backend, vulkan_command_buffer* command_buffer)
{
    MASSERT(sprite_backend)

    INFO("Sprite DRAW CALLS")

    u64 sprite_draw_count = sprite_backend->draw_count;

    vulkan_buffer* vert_buffer = vulkan_buffer_get(renderer, sprite_backend->sprite_vertex_buffer);
    vulkan_buffer* index_buffer = vulkan_buffer_get(renderer, sprite_backend->sprite_index_buffer);
    vulkan_buffer* sprite_indirect_buffer = vulkan_buffer_get(renderer, sprite_backend->sprite_indirect_buffer);

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
                            &renderer->descriptor_system->texture_descriptors.descriptor_sets[renderer->context.
                                current_frame], 0, 0);

    //storage buffers
    vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            renderer->sprite_pipeline.pipeline_layout, 2, 1,
                            &renderer->descriptor_system->storage_descriptors.descriptor_sets[renderer->context.
                                current_frame], 0, 0);


    //grab material_handle
    PC_2D pc_2d_ui = {
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
    INFO("UI DRAW CALLS")

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

Mesh_Renderer* mesh_renderer_init(Renderer* renderer, Resource_System* resource_system)
{
    Mesh_Renderer* out_mesh_renderer = arena_alloc(&renderer->arena, sizeof(Mesh_Renderer));

    u64 mesh_buffer_data_size = MB(16);


    out_mesh_renderer->mesh_shader_permutations = arena_alloc(&renderer->arena, sizeof(Mesh_Pipeline_Permutations));
    out_mesh_renderer->mesh_shader_permutations->permutation_keys = darray_create_reserve(u32, 100);

    out_mesh_renderer->vertex_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system, BUFFER_TYPE_VERTEX,
                                                                 mesh_buffer_data_size);
    out_mesh_renderer->index_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system, BUFFER_TYPE_INDEX,
                                                                mesh_buffer_data_size);
    out_mesh_renderer->indirect_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                   BUFFER_TYPE_INDIRECT,mesh_buffer_data_size);
    out_mesh_renderer->normal_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                 BUFFER_TYPE_CPU_STORAGE,mesh_buffer_data_size);
    out_mesh_renderer->tangent_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                  BUFFER_TYPE_CPU_STORAGE,mesh_buffer_data_size);
    out_mesh_renderer->uv_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system, BUFFER_TYPE_CPU_STORAGE,
                                                             mesh_buffer_data_size);
    out_mesh_renderer->material_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                   BUFFER_TYPE_CPU_STORAGE,
                                                                   mesh_buffer_data_size);

    out_mesh_renderer->vertex_staging_buffer_handle = vulkan_buffer_create(
        renderer, renderer->buffer_system, BUFFER_TYPE_STAGING,
        mesh_buffer_data_size);
    out_mesh_renderer->index_staging_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                        BUFFER_TYPE_STAGING,
                                                                        mesh_buffer_data_size);
    out_mesh_renderer->indirect_staging_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                           BUFFER_TYPE_STAGING,mesh_buffer_data_size);
    out_mesh_renderer->normal_staging_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                         BUFFER_TYPE_STAGING,mesh_buffer_data_size);
    out_mesh_renderer->tangent_staging_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                          BUFFER_TYPE_STAGING,mesh_buffer_data_size);
    out_mesh_renderer->uv_staging_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                     BUFFER_TYPE_STAGING,
                                                                     mesh_buffer_data_size);
    out_mesh_renderer->material_staging_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                           BUFFER_TYPE_STAGING,
                                                                           mesh_buffer_data_size);


    out_mesh_renderer->pc_mesh.ubo_buffer_idx = renderer->buffer_system->global_ubo_handle.handle;
    out_mesh_renderer->pc_mesh.normal_buffer_idx = out_mesh_renderer->normal_buffer_handle.handle;
    out_mesh_renderer->pc_mesh.tangent_buffer_idx = out_mesh_renderer->tangent_buffer_handle.handle;
    out_mesh_renderer->pc_mesh.uv_buffer_idx = out_mesh_renderer->uv_buffer_handle.handle;
    out_mesh_renderer->pc_mesh.transform_buffer_idx = out_mesh_renderer->transform_buffer_handle.handle;
    out_mesh_renderer->pc_mesh.material_buffer_idx = out_mesh_renderer->material_buffer_handle.handle;

    return out_mesh_renderer;

}

void mesh_system_upload_draw_data(Renderer* renderer, Mesh_Renderer* mesh_renderer, Render_Packet* render_packet)
{

    Render_Packet_Mesh* mesh_render_packet = &render_packet->mesh_data_packet;

    vulkan_buffer_reset_offset(renderer, mesh_renderer->vertex_staging_buffer_handle);
    vulkan_buffer_reset_offset(renderer, mesh_renderer->index_staging_buffer_handle);
    vulkan_buffer_reset_offset(renderer, mesh_renderer->indirect_staging_buffer_handle);
    vulkan_buffer_reset_offset(renderer, mesh_renderer->normal_staging_buffer_handle);
    vulkan_buffer_reset_offset(renderer, mesh_renderer->uv_staging_buffer_handle);
    vulkan_buffer_reset_offset(renderer, mesh_renderer->material_staging_buffer_handle);
    vulkan_buffer_reset_offset(renderer, mesh_renderer->tangent_staging_buffer_handle);

    mesh_renderer->indirect_draw_count = 0;
    //were assuming that the data is already in the buffer,
    // we are just generating the draw calls for the specific pipeline

    size_t vertex_byte_size = 0;
    size_t index_count_size = 0;


    VkDrawIndexedIndirectCommand indirect_draw = {0};
    for (size_t s_mesh_idx = 0; s_mesh_idx < mesh_render_packet->static_mesh_array_size; s_mesh_idx++)
    {
        for (size_t submesh_idx = 0; submesh_idx < mesh_render_packet->static_mesh_array[s_mesh_idx].mesh_size; submesh_idx++)
        {
            submesh* current_submesh = &mesh_render_packet->static_mesh_array[s_mesh_idx].mesh[submesh_idx];

            indirect_draw.firstIndex = index_count_size;
            indirect_draw.firstInstance = 0;
            indirect_draw.indexCount = current_submesh->indices_count;
            indirect_draw.instanceCount = 1;
            indirect_draw.vertexOffset = vertex_byte_size / sizeof(vec3); // in counts

            vertex_byte_size += current_submesh->vertex_bytes;
            index_count_size += current_submesh->indices_count;

            mesh_renderer->indirect_draw_count++;

            //this could be optimized later, by using flat arrays for all the submeshes and just doing a memcpy
            vulkan_buffer_data_copy_from_offset(renderer, mesh_renderer->vertex_staging_buffer_handle,
                                                current_submesh->pos,
                                                current_submesh->vertex_bytes);
            vulkan_buffer_data_copy_from_offset(renderer, mesh_renderer->index_staging_buffer_handle,
                                                current_submesh->indices,
                                                current_submesh->indices_bytes);

            vulkan_buffer_data_copy_from_offset(renderer, mesh_renderer->normal_staging_buffer_handle,
                                                current_submesh->normal,
                                                current_submesh->normal_bytes);

            vulkan_buffer_data_copy_from_offset(renderer, mesh_renderer->uv_staging_buffer_handle,
                                                current_submesh->uv,
                                                current_submesh->uv_bytes);
            //add the draw data
            //push the indirect draw into the draw data
            vulkan_buffer_data_copy_from_offset(renderer, mesh_renderer->indirect_staging_buffer_handle,
                                                &indirect_draw,
                                                sizeof(VkDrawIndexedIndirectCommand));
            vulkan_buffer_data_copy_from_offset(renderer, mesh_renderer->material_staging_buffer_handle,
                                                &current_submesh->material_params,
                                                sizeof(Material_Param_Data));
        }
    }


    vulkan_buffer_copy(renderer, mesh_renderer->vertex_buffer_handle, mesh_renderer->vertex_staging_buffer_handle);
    vulkan_buffer_copy(renderer, mesh_renderer->index_buffer_handle, mesh_renderer->index_staging_buffer_handle);
    vulkan_buffer_copy(renderer, mesh_renderer->indirect_buffer_handle, mesh_renderer->indirect_staging_buffer_handle);
    vulkan_buffer_copy(renderer, mesh_renderer->normal_buffer_handle, mesh_renderer->normal_staging_buffer_handle);
    vulkan_buffer_copy(renderer, mesh_renderer->uv_buffer_handle, mesh_renderer->uv_staging_buffer_handle);
    vulkan_buffer_copy(renderer, mesh_renderer->material_buffer_handle, mesh_renderer->material_staging_buffer_handle);
    vulkan_buffer_copy(renderer, mesh_renderer->tangent_buffer_handle, mesh_renderer->tangent_staging_buffer_handle);

}


void mesh_system_draw(Renderer* renderer, Mesh_Renderer* mesh_renderer, vulkan_command_buffer* command_buffer,
                      vulkan_shader_pipeline* pipeline)
{
    INFO("MESH SYSTEM DRAW CALLS")

    //only bind the vertex and index, the storage buffers are in bindless
    vulkan_buffer* indirect_buffer = vulkan_buffer_get(renderer, mesh_renderer->indirect_buffer_handle);
    vulkan_buffer* vertex_buffer = vulkan_buffer_get(renderer, mesh_renderer->vertex_buffer_handle);
    vulkan_buffer* index_buffer = vulkan_buffer_get(renderer, mesh_renderer->index_buffer_handle);


    VkDeviceSize pOffsets[] = {0};


    //UBER SHADER MESH INDIRECT DRAW
    vkCmdBindPipeline(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      renderer->indirect_mesh_pipeline.handle);

    //uniform
    vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            renderer->indirect_mesh_pipeline.pipeline_layout, 0, 1,
                            &renderer->descriptor_system->uniform_descriptors.descriptor_sets[renderer->context.
                                current_frame], 0, 0);

    //textures
    vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            renderer->indirect_mesh_pipeline.pipeline_layout, 1, 1,
                            &renderer->descriptor_system->texture_descriptors.descriptor_sets[renderer->context.
                                current_frame], 0, 0);

    //storage buffers
    vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            renderer->indirect_mesh_pipeline.pipeline_layout, 2, 1,
                            &renderer->descriptor_system->storage_descriptors.descriptor_sets[renderer->context.
                                current_frame], 0, 0);

    vkCmdBindVertexBuffers(command_buffer->handle, 0, 1, &vertex_buffer->handle,
                           pOffsets);

    vkCmdBindIndexBuffer(command_buffer->handle, index_buffer->handle, 0,
                         VK_INDEX_TYPE_UINT16);



    VkPushConstantsInfo push_constant_info = {0};
    push_constant_info.sType = VK_STRUCTURE_TYPE_PUSH_CONSTANTS_INFO;
    push_constant_info.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    push_constant_info.layout = pipeline->pipeline_layout;
    push_constant_info.offset = 0;
    push_constant_info.size = sizeof(PC_Mesh);
    // push_constant_info.size = sizeof(mesh_system->pc_mesh); // make sure its not a pointer if i use this
    push_constant_info.pValues = &mesh_renderer->pc_mesh;
    push_constant_info.pNext = NULL;
    vkCmdPushConstants2(command_buffer->handle, &push_constant_info);


    //no need to sort by permutation rn, it gets handled in the shader, we can always sort out this later
    if (renderer->context.device.features.multiDrawIndirect)
    {
        vkCmdDrawIndexedIndirect(command_buffer->handle,
                                 indirect_buffer->handle, 0,
                                 mesh_renderer->indirect_draw_count,
                                 sizeof(VkDrawIndexedIndirectCommand));
    }
    else
    {
        // If multi draw is not available, we must issue separate draw commands
        for (u64 j = 0; j < mesh_renderer->indirect_draw_count; j++)
        {
            vkCmdDrawIndexedIndirect(command_buffer->handle,
                                     indirect_buffer->handle,
                                     j * sizeof(VkDrawIndexedIndirectCommand), 1,
                                     sizeof(VkDrawIndexedIndirectCommand));
        }
    }
}