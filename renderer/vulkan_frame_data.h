#ifndef VK_FRAME_DATA_H
#define VK_FRAME_DATA_H

#include "vk_command_buffer.h"
#include "vulkan_struct_types.h"


void vulkan_frame_data_init(Renderer* renderer)
{
    //DRAW DATA
    u64 skinned_buffer_data_size = MB(64); // TODO: this is gonna have to get adjusted
    u64 transform_buffer_memory_size = MAX_TRANSFORM_COUNT * sizeof(mat4s);


    renderer->skinned_matrix_buffer = vulkan_buffer_create_frame(renderer, renderer->buffer_system,
                                                                 BUFFER_TYPE_STORAGE_GPU,
                                                                 skinned_buffer_data_size, "Skinned");


    renderer->transform_buffer_handle = vulkan_buffer_create_frame(renderer, renderer->buffer_system,
                                                                   BUFFER_TYPE_STORAGE_GPU,
                                                                   transform_buffer_memory_size, "Transform");

    renderer->texture_table_buffer = vulkan_buffer_create_frame(renderer, renderer->buffer_system,
                                                                BUFFER_TYPE_STORAGE_GPU,
                                                                sizeof(u32) * MAX_TEXTURE_COUNT, "Texture Table");


    renderer->material_buffer = vulkan_buffer_create_frame(renderer, renderer->buffer_system,
                                                           BUFFER_TYPE_STORAGE_GPU, MATERIAL_BUFFER_SIZE, "Material");

    renderer->material_record_buffer = vulkan_buffer_create_frame(renderer, renderer->buffer_system,
                                                                  BUFFER_TYPE_STORAGE_GPU,
                                                                  sizeof(Material_Record) * MAX_MATERIAL_COUNT, "Material Buffer");


}


void vulkan_upload_frame_data(Renderer* renderer, Render_Packet* render_packet, Vulkan_Command_Buffer* command_buffer)
{
    vulkan_command_buffer_debug_label_begin_color(renderer, command_buffer, "Transform",
                                              (float[4]){0.0, 1.0, 0.0, 1.0}); // green color



    //transform data
    if (vulkan_buffer_frame_staging_upload(
        renderer, renderer->transform_buffer_handle,
        command_buffer, render_packet->draw_3d_data_packet.world_space_matrix_array,
        sizeof(mat4s) * render_packet->draw_3d_data_packet.
                                       world_space_matrix_count))
    {
        Vulkan_Buffer* transform_buffer = vulkan_buffer_get_frame(renderer, renderer->transform_buffer_handle);
        VkBufferMemoryBarrier2 transform_buffer_barrier = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,

            //barrier so that we finish our transfer before our shaders read the data
            .srcStageMask = VK_PIPELINE_STAGE_2_COPY_BIT,
            .srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,

            .dstStageMask = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT |
            VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
            .dstAccessMask = VK_ACCESS_2_SHADER_STORAGE_READ_BIT,

            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,

            .buffer = transform_buffer->handle,
            .offset = 0,
            .size = transform_buffer->current_offset,
        };
        vulkan_command_add_buffer_barrier(command_buffer, transform_buffer_barrier);
    }

    vulkan_command_buffer_debug_label_end(renderer, command_buffer);


    vulkan_command_buffer_debug_label_begin_color(renderer, command_buffer, "Skinned",
                                              (float[4]){0.0, 1.0, 0.0, 1.0}); // green color

    //skinned matrix
    if (vulkan_buffer_frame_staging_upload(renderer, renderer->skinned_matrix_buffer,
                                           command_buffer,
                                           render_packet->draw_3d_data_packet.skinned_matrix->data,
                                           array_get_byte_size(render_packet->draw_3d_data_packet.skinned_matrix)))
    {
        //successful data uploads need a barrier
        Vulkan_Buffer* skinned_buffer = vulkan_buffer_get_frame(renderer, renderer->skinned_matrix_buffer);
        VkBufferMemoryBarrier2 skinned_buffer_barrier = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,

            .srcStageMask = VK_PIPELINE_STAGE_2_COPY_BIT,
            .srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,

            .dstStageMask = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT |
            VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
            .dstAccessMask = VK_ACCESS_2_SHADER_STORAGE_READ_BIT,

            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,

            .buffer = skinned_buffer->handle,
            .offset = 0,
            .size = skinned_buffer->current_offset,
        };
        vulkan_command_add_buffer_barrier(command_buffer, skinned_buffer_barrier);
    }

    vulkan_command_flush_barriers(command_buffer);
    vulkan_command_buffer_debug_label_end(renderer, command_buffer);


    //TODO: material record


    vulkan_command_buffer_debug_label_begin_color(renderer, command_buffer, "Material",
                                              (float[4]){0.0, 1.0, 0.0, 1.0}); // green color
    //Material
    vulkan_buffer_frame_staging_upload(renderer, renderer->material_buffer, command_buffer,
                                   render_packet->material_packet.material_buffer,
                                   render_packet->material_packet.material_buffer_byte_size);

    VkBufferMemoryBarrier2 material_barrier = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
        .srcStageMask = VK_PIPELINE_STAGE_2_COPY_BIT,
        .srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,

        .dstStageMask = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
        .dstAccessMask = VK_ACCESS_2_SHADER_STORAGE_READ_BIT,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,

        .buffer = vulkan_buffer_get_frame(renderer, renderer->material_buffer)->handle,
        .offset = 0,
        .size = VK_WHOLE_SIZE,
    };
    vulkan_command_add_buffer_barrier(command_buffer, material_barrier);
    vulkan_command_flush_barriers(command_buffer);


    vulkan_buffer_frame_staging_upload(renderer, renderer->material_record_buffer, command_buffer,
                               render_packet->material_packet.material_records,
                               render_packet->material_packet.material_records_byte_size);

    VkBufferMemoryBarrier2 material_record_barrier = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
        .srcStageMask = VK_PIPELINE_STAGE_2_COPY_BIT,
        .srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,

        .dstStageMask = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
        .dstAccessMask = VK_ACCESS_2_SHADER_STORAGE_READ_BIT,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,

        .buffer = vulkan_buffer_get_frame(renderer, renderer->material_record_buffer)->handle,
        .offset = 0,
        .size = VK_WHOLE_SIZE,
    };
    vulkan_command_add_buffer_barrier(command_buffer, material_record_barrier);
    vulkan_command_flush_barriers(command_buffer);


    vulkan_command_buffer_debug_label_end(renderer, command_buffer);


}





#endif //VK_FRAME_DATA_H
