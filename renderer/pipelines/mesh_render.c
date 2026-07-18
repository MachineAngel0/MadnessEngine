#include "mesh_render.h"

#include "vk_buffer.h"
#include "vulkan_struct_types.h"


Mesh_Renderer* mesh_renderer_init(Renderer* renderer, Resource_System* resource_system)
{
    Mesh_Renderer* out_mesh_renderer = allocator_alloc(&renderer->allocator, sizeof(Mesh_Renderer));

    u64 mesh_buffer_data_size = MB(16);


    out_mesh_renderer->vertex_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                   BUFFER_TYPE_VERTEX,
                                                                   mesh_buffer_data_size);
    out_mesh_renderer->index_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system, BUFFER_TYPE_INDEX,
                                                                  mesh_buffer_data_size);


    out_mesh_renderer->normal_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                   BUFFER_TYPE_CPU_STORAGE, mesh_buffer_data_size);
    out_mesh_renderer->tangent_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                    BUFFER_TYPE_CPU_STORAGE, mesh_buffer_data_size);
    out_mesh_renderer->uv_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                               BUFFER_TYPE_CPU_STORAGE,
                                                               mesh_buffer_data_size);
    out_mesh_renderer->joint_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                  BUFFER_TYPE_CPU_STORAGE,
                                                                  mesh_buffer_data_size);
    out_mesh_renderer->weight_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                   BUFFER_TYPE_CPU_STORAGE,
                                                                   mesh_buffer_data_size);


    out_mesh_renderer->vertex_staging_buffer_handle = vulkan_buffer_create(
        renderer, renderer->buffer_system, BUFFER_TYPE_STAGING,
        mesh_buffer_data_size);
    out_mesh_renderer->index_staging_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                          BUFFER_TYPE_STAGING,
                                                                          mesh_buffer_data_size);
    out_mesh_renderer->indirect_staging_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                             BUFFER_TYPE_STAGING,
                                                                             mesh_buffer_data_size);
    out_mesh_renderer->normal_staging_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                           BUFFER_TYPE_STAGING, mesh_buffer_data_size);
    out_mesh_renderer->tangent_staging_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                            BUFFER_TYPE_STAGING, mesh_buffer_data_size);
    out_mesh_renderer->uv_staging_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                       BUFFER_TYPE_STAGING,
                                                                       mesh_buffer_data_size);


    //DRAW DATA
    out_mesh_renderer->skinned_matrix_buffer = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                    BUFFER_TYPE_CPU_STORAGE,
                                                                    mesh_buffer_data_size);
    out_mesh_renderer->skinned_matrix_staging_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
        BUFFER_TYPE_STAGING,
        mesh_buffer_data_size);

    out_mesh_renderer->pbr_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                BUFFER_TYPE_CPU_STORAGE,
                                                                sizeof(Material_Default) * MAX_DEFAULT_MATERIAL);
    out_mesh_renderer->pbr_staging_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                        BUFFER_TYPE_STAGING,
                                                                        sizeof(Material_Default) *
                                                                        MAX_DEFAULT_MATERIAL);

    u64 transform_buffer_memory_size = MAX_TRANSFORM_COUNT * sizeof(mat4s);

    out_mesh_renderer->transform_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                      BUFFER_TYPE_CPU_STORAGE,
                                                                      transform_buffer_memory_size);
    out_mesh_renderer->transform_staging_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                              BUFFER_TYPE_STAGING,
                                                                              transform_buffer_memory_size);


    //?? i need to get these
    // VkDeviceAddress transform_buffer;
    // VkDeviceAddress material_buffer;

    return out_mesh_renderer;
}


void mesh_renderer_upload_draw_data(Renderer* renderer, Mesh_Renderer* mesh_renderer, Render_Packet* render_packet,
                                    vulkan_command_buffer* command_buffer)
{
    //update the transforms every frame


    ring_queue* mesh_render_queue = render_packet->mesh_queue;
    ring_queue* skinned_mesh_render_queue = render_packet->skinned_mesh_queue;

    Mesh_Upload_Data* submesh_upload_data = allocator_alloc(&renderer->frame_allocator, sizeof(Mesh_Upload_Data));

    //NOTE: rn it copies from an offset, which is fine for now,
    // but when the system needs to be more dynamic, its going to need a rewrite, especially the buffer system function calls
    while (!ring_queue_is_empty(mesh_render_queue))
    {
        ring_dequeue(mesh_render_queue, submesh_upload_data);

        //this could be optimized later, by using flat arrays for all the submeshes and just doing a memcpy
        vulkan_buffer_data_copy_from_offset(renderer, mesh_renderer->vertex_staging_buffer_handle,
                                            submesh_upload_data->pos,
                                            submesh_upload_data->vertex_bytes);
        vulkan_buffer_data_copy_from_offset(renderer, mesh_renderer->index_staging_buffer_handle,
                                            submesh_upload_data->indices,
                                            submesh_upload_data->indices_bytes);

        vulkan_buffer_data_copy_from_offset(renderer, mesh_renderer->normal_staging_buffer_handle,
                                            submesh_upload_data->normal,
                                            submesh_upload_data->normal_bytes);

        vulkan_buffer_data_copy_from_offset(renderer, mesh_renderer->uv_staging_buffer_handle,
                                            submesh_upload_data->uv,
                                            submesh_upload_data->uv_bytes);
    }

    //skinned data

    Sk_Mesh_Upload_Data skinned_mesh_upload_data = {0};

    while (!ring_queue_is_empty(skinned_mesh_render_queue))
    {
        ring_dequeue(skinned_mesh_render_queue, &skinned_mesh_upload_data);


        vulkan_buffer_cpu_to_gpu_copy_and_upload_batch_global_staging_from_offset(
            renderer, mesh_renderer->joint_buffer_handle,
            command_buffer, skinned_mesh_upload_data.joints,
            skinned_mesh_upload_data.joint_bytes);
        vulkan_buffer_cpu_to_gpu_copy_and_upload_batch_global_staging_from_offset(
            renderer, mesh_renderer->weight_buffer_handle,
            command_buffer, skinned_mesh_upload_data.weights,
            skinned_mesh_upload_data.weight_bytes);
    }


    vulkan_buffer_cpu_to_gpu_upload(renderer, mesh_renderer->vertex_buffer_handle,
                                    mesh_renderer->vertex_staging_buffer_handle, command_buffer);
    vulkan_buffer_cpu_to_gpu_upload(renderer, mesh_renderer->index_buffer_handle,
                                    mesh_renderer->index_staging_buffer_handle, command_buffer);
    vulkan_buffer_cpu_to_gpu_upload(renderer, mesh_renderer->normal_buffer_handle,
                                    mesh_renderer->normal_staging_buffer_handle, command_buffer);
    vulkan_buffer_cpu_to_gpu_upload(renderer, mesh_renderer->uv_buffer_handle, mesh_renderer->uv_staging_buffer_handle,
                                    command_buffer);
    vulkan_buffer_cpu_to_gpu_upload(renderer, mesh_renderer->tangent_buffer_handle,
                                    mesh_renderer->tangent_staging_buffer_handle, command_buffer);
}

void mesh_renderer_upload_per_frame_data(Renderer* renderer, Mesh_Renderer* mesh_renderer,
                                         Render_Packet* render_packet, vulkan_command_buffer* command_buffer)
{
    //transform data
    vulkan_buffer_reset_offset(renderer, mesh_renderer->transform_buffer_handle);
    vulkan_buffer_reset_offset(renderer, mesh_renderer->transform_staging_buffer_handle);
    vulkan_buffer_cpu_to_gpu_copy_and_upload_batch(renderer, mesh_renderer->transform_buffer_handle,
                                                   mesh_renderer->transform_staging_buffer_handle, command_buffer,
                                                   render_packet->draw_3d_data_packet.world_space_matrix_array,
                                                   sizeof(mat4s) * render_packet->draw_3d_data_packet.
                                                   world_space_matrix_count);

    //stuff uploaded very frame
    vulkan_buffer_reset_offset(renderer, mesh_renderer->pbr_buffer_handle);
    vulkan_buffer_reset_offset(renderer, mesh_renderer->pbr_staging_buffer_handle);
    vulkan_buffer_cpu_to_gpu_copy_and_upload_batch(renderer, mesh_renderer->pbr_buffer_handle,
                                                   mesh_renderer->pbr_staging_buffer_handle, command_buffer,
                                                   render_packet->draw_3d_data_packet.prb,
                                                   render_packet->draw_3d_data_packet.prb_bytes);


    //skinned matrix
    vulkan_buffer_reset_offset(renderer, mesh_renderer->skinned_matrix_buffer);
    vulkan_buffer_reset_offset(renderer, mesh_renderer->skinned_matrix_staging_buffer_handle);
    vulkan_buffer_cpu_to_gpu_copy_and_upload_batch(renderer, mesh_renderer->skinned_matrix_buffer,
                                                   mesh_renderer->skinned_matrix_staging_buffer_handle, command_buffer,
                                                   render_packet->draw_3d_data_packet.skinned_matrix->data,
                                                   dynamic_array_get_byte_size(
                                                       render_packet->draw_3d_data_packet.skinned_matrix));
}


void mesh_renderer_construct_batch_draw(Renderer* renderer, Mesh_Renderer* mesh_renderer, Shader_System* shader_system,
                                        Render_Packet* render_packet,
                                        vulkan_command_buffer* command_buffer)
{
    //TODO: techincally duplicate code, we can abstract it out, pass in the material batch array and count
    // and switch on the mesh type
    // not using this anywhere so its fine for now, just be careful on modifying this code

    for (u32 batch_idx = 0; batch_idx < render_packet->draw_3d_data_packet.mesh_batch_count; batch_idx++)
    {
        Material_Batch* batch_data = &render_packet->draw_3d_data_packet.mesh_batch[batch_idx];

        Vulkan_Shader_Batch* current_batch = allocator_alloc(&renderer->frame_allocator, sizeof(Vulkan_Shader_Batch));
        if (hash_table_get(shader_system->shader_batch_hash_table, batch_data->shader_name, &current_batch))
        {
            current_batch->draw_count = 0;

            //reset the offset of our per draw data
            vulkan_buffer_reset_offset(renderer, current_batch->draw_data_buffer_handle);
            vulkan_buffer_reset_offset(renderer, current_batch->indirect_draw_buffer_handle);
            vulkan_buffer_reset_offset(renderer, current_batch->material_data_buffer_handle);

            vulkan_buffer_cpu_to_gpu_copy_and_upload_batch_global_staging_from_offset(
                renderer, current_batch->material_data_buffer_handle,
                command_buffer,
                batch_data->material_data->data,
                dynamic_array_get_byte_size(batch_data->material_data));

            VkDrawIndexedIndirectCommand indirect_draw = {0};

            for (u32 mesh_idx = 0; mesh_idx < batch_data->mesh_instances->num_items; mesh_idx++)
            {
                Mesh_Instance* mesh_instance = dynamic_array_get_ptr(batch_data->mesh_instances, Mesh_Instance,
                                                                     mesh_idx);
                mesh_instance->mesh_gpu_draw.material_instance_handle = mesh_idx;
                vulkan_buffer_cpu_to_gpu_copy_and_upload_batch_global_staging_from_offset(
                    renderer, current_batch->draw_data_buffer_handle,
                    command_buffer,
                    &mesh_instance->mesh_gpu_draw,
                    sizeof(Mesh_GPU_Draw));

                indirect_draw.firstIndex = mesh_instance->mesh_indirect_draw.index_offset;
                indirect_draw.indexCount = mesh_instance->mesh_indirect_draw.index_count;
                indirect_draw.vertexOffset = mesh_instance->mesh_indirect_draw.vertex_offset;
                //instance data
                indirect_draw.instanceCount = 1;
                indirect_draw.firstInstance = 0;

                vulkan_buffer_cpu_to_gpu_copy_and_upload_batch_global_staging_from_offset(
                    renderer, current_batch->indirect_draw_buffer_handle,
                    command_buffer,
                    &indirect_draw,
                    sizeof(VkDrawIndexedIndirectCommand));


                current_batch->draw_count++;
            }
        }
    }


    for (u32 batch_idx = 0; batch_idx < render_packet->draw_3d_data_packet.skinned_batch_count; batch_idx++)
    {
        Material_Batch* batch_data = &render_packet->draw_3d_data_packet.skinned_batch[batch_idx];

        Vulkan_Shader_Batch* current_batch = allocator_alloc(&renderer->frame_allocator, sizeof(Vulkan_Shader_Batch));
        if (hash_table_get(shader_system->shader_batch_hash_table, batch_data->shader_name, &current_batch))
        {
            current_batch->draw_count = 0;

            //reset the offset of our per draw data
            vulkan_buffer_reset_offset(renderer, current_batch->draw_data_buffer_handle);
            vulkan_buffer_reset_offset(renderer, current_batch->indirect_draw_buffer_handle);
            vulkan_buffer_reset_offset(renderer, current_batch->material_data_buffer_handle);

            vulkan_buffer_cpu_to_gpu_copy_and_upload_batch_global_staging_from_offset(
                renderer, current_batch->material_data_buffer_handle,
                command_buffer,
                batch_data->material_data->data,
                dynamic_array_get_byte_size(batch_data->material_data));

            VkDrawIndexedIndirectCommand indirect_draw = {0};

            for (u32 mesh_idx = 0; mesh_idx < batch_data->mesh_instances->num_items; mesh_idx++)
            {
                Sk_Mesh_Instance* sk_mesh_instance = dynamic_array_get_ptr(
                    batch_data->mesh_instances, Sk_Mesh_Instance,
                    mesh_idx);
                //we are indexing into the material buffer based on the draw count
                sk_mesh_instance->sk_mesh_gpu_draw.material_instance_handle = mesh_idx;
                sk_mesh_instance->sk_mesh_gpu_draw.vertex_offset = sk_mesh_instance->mesh_indirect_draw.
                    vertex_offset;
                vulkan_buffer_cpu_to_gpu_copy_and_upload_batch_global_staging_from_offset(
                    renderer, current_batch->draw_data_buffer_handle,
                    command_buffer,
                    &sk_mesh_instance->sk_mesh_gpu_draw,
                    sizeof(SKMesh_GPU_Draw));

                indirect_draw.firstIndex = sk_mesh_instance->mesh_indirect_draw.index_offset;
                indirect_draw.indexCount = sk_mesh_instance->mesh_indirect_draw.index_count;
                indirect_draw.vertexOffset = sk_mesh_instance->mesh_indirect_draw.vertex_offset;
                //instance data
                indirect_draw.instanceCount = 1;
                indirect_draw.firstInstance = 0;

                vulkan_buffer_cpu_to_gpu_copy_and_upload_batch_global_staging_from_offset(
                    renderer, current_batch->indirect_draw_buffer_handle,
                    command_buffer,
                    &indirect_draw,
                    sizeof(VkDrawIndexedIndirectCommand));

                current_batch->draw_count++;
            }
        }
    }
}


void mesh_renderer_batch_draw(Renderer* renderer, Mesh_Renderer* mesh_renderer,
                              Vulkan_Shader_Batch* batch_draw_data, u32 batch_draw_count,
                              vulkan_command_buffer* command_buffer)
{
    //only bind the vertex and index, the storage buffers are bda, so either in the ubo or the push constant

    for (u32 batch_idx = 0; batch_idx < batch_draw_count; ++batch_idx)
    {
        Vulkan_Shader_Batch* draw_data = &batch_draw_data[batch_idx];

        Vulkan_Buffer* indirect_buffer = vulkan_buffer_get(renderer, draw_data->indirect_draw_buffer_handle);
        Vulkan_Buffer* index_buffer = vulkan_buffer_get(renderer, mesh_renderer->index_buffer_handle);

        //check if we are using wireframe_mode
        VkPipeline pipeline = draw_data->pipeline.handle;
        VkPipelineLayout layout = draw_data->pipeline.pipeline_layout;
        if (renderer->wireframe_mode)
        {
            pipeline = draw_data->wireframe_pipeline.handle;
            layout = draw_data->wireframe_pipeline.pipeline_layout;;
        }
        else
        {
            pipeline = draw_data->pipeline.handle;
            layout = draw_data->pipeline.pipeline_layout;
        }


        //UBER SHADER MESH INDIRECT DRAW
        vkCmdBindPipeline(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          pipeline);

        //uniform
        vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                layout, 0, 1,
                                &renderer->descriptor_system->uniform_descriptors.descriptor_sets[renderer->context.
                                    current_frame], 0, 0);
        //texturess
        vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                layout, 1, 1,
                                &renderer->descriptor_system->texture_descriptors.descriptor_sets[0], 0, 0);

        //storage buffers
        vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                layout, 2, 1,
                                &renderer->descriptor_system->storage_descriptors.descriptor_sets[renderer->context.
                                    current_frame], 0, 0);

        // vkCmdBindVertexBuffers(command_buffer->handle, 0, 1, &vertex_buffer->handle,
        // pOffsets);

        vkCmdBindIndexBuffer(command_buffer->handle, index_buffer->handle, 0,
                             VK_INDEX_TYPE_UINT16);


        VkPushConstantsInfo push_constant_info = {0};
        push_constant_info.sType = VK_STRUCTURE_TYPE_PUSH_CONSTANTS_INFO;
        push_constant_info.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        push_constant_info.layout = layout;
        push_constant_info.offset = 0;
        push_constant_info.size = sizeof(PC_General);
        // push_constant_info.size = sizeof(draw_data->general); // make sure its not a pointer if i use this
        push_constant_info.pValues = &draw_data->pc_data;
        push_constant_info.pNext = NULL;
        vkCmdPushConstants2(command_buffer->handle, &push_constant_info);


        if (renderer->context.device.features.multiDrawIndirect)
        {
            vkCmdDrawIndexedIndirect(command_buffer->handle,
                                     indirect_buffer->handle, 0,
                                     draw_data->draw_count,
                                     sizeof(VkDrawIndexedIndirectCommand));
        }
        else
        {
            // If multi draw is not available, we must issue separate draw commands
            for (u64 j = 0; j < draw_data->draw_count; j++)
            {
                vkCmdDrawIndexedIndirect(command_buffer->handle,
                                         indirect_buffer->handle,
                                         j * sizeof(VkDrawIndexedIndirectCommand), 1,
                                         sizeof(VkDrawIndexedIndirectCommand));
            }
        }
    }
}




void mesh_renderer_batch_draw_custom_pipeline(Renderer* renderer, Mesh_Renderer* mesh_renderer,
                                              Vulkan_Shader_Batch* batch_draw_data, u32 batch_draw_count,
                                              vulkan_command_buffer* command_buffer,
                                              Vulkan_Shader_Pipeline* shader_pipeline)
{
    MASSERT(shader_pipeline);
    //only bind the vertex and index, the storage buffers are bda, so either in the ubo or the push constant
    VkPipeline pipeline = shader_pipeline->handle;
    VkPipelineLayout layout = shader_pipeline->pipeline_layout;

    for (u32 batch_idx = 0; batch_idx < batch_draw_count; ++batch_idx)
    {
        Vulkan_Shader_Batch* draw_data = &batch_draw_data[batch_idx];

        //check if we have a shadow pass
        if ((draw_data->shader_pass_type & Shader_Pass_Type_Shadow) == 0)
        {
            continue;
        }


        Vulkan_Buffer* indirect_buffer = vulkan_buffer_get(renderer, draw_data->indirect_draw_buffer_handle);
        Vulkan_Buffer* index_buffer = vulkan_buffer_get(renderer, mesh_renderer->index_buffer_handle);

        //figure out the shadow pass type we are using


        //UBER SHADER MESH INDIRECT DRAW
        vkCmdBindPipeline(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          pipeline);

        //uniform
        vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                layout, 0, 1,
                                &renderer->descriptor_system->uniform_descriptors.descriptor_sets[renderer->context.
                                    current_frame], 0, 0);
        //texturess
        vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                layout, 1, 1,
                                &renderer->descriptor_system->texture_descriptors.descriptor_sets[0], 0, 0);

        //storage buffers
        vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                layout, 2, 1,
                                &renderer->descriptor_system->storage_descriptors.descriptor_sets[renderer->context.
                                    current_frame], 0, 0);

        // vkCmdBindVertexBuffers(command_buffer->handle, 0, 1, &vertex_buffer->handle,
        // pOffsets);

        vkCmdBindIndexBuffer(command_buffer->handle, index_buffer->handle, 0,
                             VK_INDEX_TYPE_UINT16);


        VkPushConstantsInfo push_constant_info = {0};
        push_constant_info.sType = VK_STRUCTURE_TYPE_PUSH_CONSTANTS_INFO;
        push_constant_info.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        push_constant_info.layout = layout;
        push_constant_info.offset = 0;
        push_constant_info.size = sizeof(PC_General);
        // push_constant_info.size = sizeof(draw_data->general); // make sure its not a pointer if i use this
        push_constant_info.pValues = &draw_data->pc_data;
        push_constant_info.pNext = NULL;
        vkCmdPushConstants2(command_buffer->handle, &push_constant_info);


        if (renderer->context.device.features.multiDrawIndirect)
        {
            vkCmdDrawIndexedIndirect(command_buffer->handle,
                                     indirect_buffer->handle, 0,
                                     draw_data->draw_count,
                                     sizeof(VkDrawIndexedIndirectCommand));
        }
        else
        {
            // If multi draw is not available, we must issue separate draw commands
            for (u64 j = 0; j < draw_data->draw_count; j++)
            {
                vkCmdDrawIndexedIndirect(command_buffer->handle,
                                         indirect_buffer->handle,
                                         j * sizeof(VkDrawIndexedIndirectCommand), 1,
                                         sizeof(VkDrawIndexedIndirectCommand));
            }
        }
    }
}
