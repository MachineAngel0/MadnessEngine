#include "mesh_render.h"

#include "vk_buffer.h"
#include "vulkan_struct_types.h"


Mesh_Renderer* mesh_renderer_init(Renderer* renderer, Asset_System* resource_system)
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

    Mesh_GPU_Upload* submesh_upload_data = allocator_alloc(&renderer->frame_allocator, sizeof(Mesh_GPU_Upload));

    //NOTE: rn it copies from an offset, which is fine for now,
    // but when the system needs to be more dynamic, its going to need a rewrite, especially the buffer system function calls
    while (!ring_queue_is_empty(mesh_render_queue))
    {
        ring_dequeue(mesh_render_queue, submesh_upload_data);

        //this could be optimized later, by using flat arrays for all the submeshes and just doing a memcpy
        vulkan_buffer_data_copy_from_offset(renderer, mesh_renderer->vertex_staging_buffer_handle,
                                            submesh_upload_data->gpu_data->vertex,
                                            submesh_upload_data->submesh->vertex_bytes);
        vulkan_buffer_data_copy_from_offset(renderer, mesh_renderer->index_staging_buffer_handle,
                                            submesh_upload_data->gpu_data->indices,
                                            submesh_upload_data->submesh->indices_bytes);

        vulkan_buffer_data_copy_from_offset(renderer, mesh_renderer->normal_staging_buffer_handle,
                                            submesh_upload_data->gpu_data->normal,
                                            submesh_upload_data->submesh->normal_bytes);

        vulkan_buffer_data_copy_from_offset(renderer, mesh_renderer->uv_staging_buffer_handle,
                                            submesh_upload_data->gpu_data->uv,
                                            submesh_upload_data->submesh->uv_bytes);

        vulkan_buffer_data_copy_from_offset(renderer, mesh_renderer->tangent_staging_buffer_handle,
                                            submesh_upload_data->gpu_data->tangent,
                                            submesh_upload_data->submesh->tangent_bytes);

        //TODO: free the data on the cpu
    }

    //skinned data

    Skinned_Mesh_GPU_Upload skinned_mesh_upload_data = {0};

    while (!ring_queue_is_empty(skinned_mesh_render_queue))
    {
        ring_dequeue(skinned_mesh_render_queue, &skinned_mesh_upload_data);


        vulkan_buffer_cpu_to_gpu_copy_and_upload_batch_global_staging_from_offset(
            renderer, mesh_renderer->joint_buffer_handle,
            command_buffer, skinned_mesh_upload_data.skinned_gpu_data->joints,
            skinned_mesh_upload_data.skinned_submesh->joint_bytes);
        vulkan_buffer_cpu_to_gpu_copy_and_upload_batch_global_staging_from_offset(
            renderer, mesh_renderer->weight_buffer_handle,
            command_buffer, skinned_mesh_upload_data.skinned_gpu_data->weights,
            skinned_mesh_upload_data.skinned_submesh->weight_bytes);
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

    //skinned matrix
    vulkan_buffer_reset_offset(renderer, mesh_renderer->skinned_matrix_buffer);
    vulkan_buffer_reset_offset(renderer, mesh_renderer->skinned_matrix_staging_buffer_handle);
    vulkan_buffer_cpu_to_gpu_copy_and_upload_batch(renderer, mesh_renderer->skinned_matrix_buffer,
                                                   mesh_renderer->skinned_matrix_staging_buffer_handle, command_buffer,
                                                   render_packet->draw_3d_data_packet.skinned_matrix->data,
                                                   array_get_byte_size(
                                                       render_packet->draw_3d_data_packet.skinned_matrix));
}

void mesh_renderer_construct_batch_draw(Renderer* renderer,
                                        Render_Packet* render_packet,
                                        vulkan_command_buffer* command_buffer)
{
    //TODO: were going to assume we have already done culling,
    // but we would need to pass in another structure for actually culled data
    // or even cull and create the render format at the same time, instead of two seperate arrays

    // loop through our meshes and create render items for each

    //TODO: TEMP
    Mesh_Render_Item* render_items = allocator_alloc(&renderer->frame_allocator, 100 * sizeof(Mesh_Render_Item));
    u32 render_item_count = 0;

    Skinned_Render_Item* skinned_render_items = allocator_alloc(&renderer->frame_allocator,
                                                                100 * sizeof(Skinned_Render_Item));
    u32 skinned_render_item_count = 0;

    // Vulkan_Mesh_Draw draw_data = {
    // .transform_idx = sub_mesh_instance->parent_transform_handle.handle,
    // .material_instance_handle = sub_mesh_instance->material_handle.buffer_handle,
    // };

    for (u32 i = 0; i < render_packet->draw_3d_data_packet.mesh_instances_count; i++)
    {
        Madness_Mesh_Instance* mesh_instance = &render_packet->draw_3d_data_packet.mesh_instances[i];
        for (u32 submesh_idx = 0; submesh_idx < mesh_instance->mesh_count; submesh_idx++)
        {
            Madness_SubMesh_Instance* sub_mesh_instance = &mesh_instance->submesh_instances[submesh_idx];

            Mesh_Render_Item* render_inst = &render_items[render_item_count++];
            *render_inst = (Mesh_Render_Item){
                .material_key = sub_mesh_instance->material_handle.material_id,
                .mesh_handle = mesh_instance->mesh_asset.handle,
                .submesh_handle = submesh_idx,
                .material_handle = sub_mesh_instance->material_handle.buffer_handle,
                .transform_handle = mesh_instance->transform_handle.handle,
                .index_count = sub_mesh_instance->mesh_indirect_draw.index_count,
                .index_offset = sub_mesh_instance->mesh_indirect_draw.index_offset,
                .vertex_offset = sub_mesh_instance->mesh_indirect_draw.vertex_count_offset,
            };
        }
    }

    for (u32 i = 0; i < render_packet->draw_3d_data_packet.skinned_instances_count; i++)
    {
        Madness_Skinned_Mesh_Instance* mesh_instance = &render_packet->draw_3d_data_packet.skinned_instances[i];
        for (u32 submesh_idx = 0; submesh_idx < mesh_instance->mesh_count; submesh_idx++)
        {
            Madness_Skinned_Submesh_Instance* sub_mesh_instance = &mesh_instance->submesh_instances[submesh_idx];

            Skinned_Render_Item* render_inst = &skinned_render_items[skinned_render_item_count++];
            *render_inst = (Skinned_Render_Item){
                .material_key = sub_mesh_instance->material_handle.material_id,
                .mesh_handle = mesh_instance->skinned_mesh_asset.handle,
                .submesh_handle = submesh_idx,
                .material_handle = sub_mesh_instance->material_handle.buffer_handle,
                .transform_handle = mesh_instance->transform_handle.handle,
                .index_count = sub_mesh_instance->mesh_indirect_draw.index_count,
                .index_offset = sub_mesh_instance->mesh_indirect_draw.index_offset,
                .vertex_offset = sub_mesh_instance->mesh_indirect_draw.vertex_count_offset,
                .joint_idx = sub_mesh_instance->skinned_draw_data.joint_idx,
                .weight_idx = sub_mesh_instance->skinned_draw_data.weight_idx,
                .skinned_matrix_idx = sub_mesh_instance->skinned_draw_data.skinned_matrix_idx,



            };
        }
    }


    //TODO/ OPTIMIZE : sort render items by material id,
    // when a render item doesn't match, that means we move onto the next batch

    VkDrawIndexedIndirectCommand indirect_draw = {0};
    Vulkan_Mesh_Draw mesh_draw = {0};
    for (u32 batch_idx = 0; batch_idx < renderer->shader_system->mesh_batch_count; batch_idx++)
    {
        Vulkan_Shader_Batch* current_batch = &renderer->shader_system->mesh_batch[batch_idx];
        current_batch->draw_count = 0;

        vulkan_buffer_reset_offset(renderer, current_batch->material_data_buffer_handle);
        vulkan_buffer_reset_offset(renderer, current_batch->draw_data_buffer_handle);
        vulkan_buffer_reset_offset(renderer, current_batch->indirect_draw_buffer_handle);


        //TODO/ OPTIMIZE: we can upload only whats needed per frame, instead of the whole thing,
        // but who knows if individual uploads would be slower?
        // honestly, just depends on the performance, should profile
        vulkan_buffer_cpu_to_gpu_copy_and_upload_batch_global_staging_from_offset(
            renderer, current_batch->material_data_buffer_handle,
            command_buffer,
            current_batch->material_batch_reference->material_data->data,
            dynamic_array_get_byte_size(current_batch->material_batch_reference->material_data));

        for (u32 item_idx = 0; item_idx < render_item_count; item_idx++)
        {
            Mesh_Render_Item* cur_render_item = &render_items[item_idx];

            if (cur_render_item->material_key == current_batch->material_id)
            {
                mesh_draw = (Vulkan_Mesh_Draw){
                    .transform_idx = cur_render_item->transform_handle,
                    .material_instance_handle = cur_render_item->material_handle,
                };
                indirect_draw = (VkDrawIndexedIndirectCommand){
                    .indexCount = cur_render_item->index_count,
                    .firstIndex = cur_render_item->index_offset,
                    .vertexOffset = cur_render_item->vertex_offset,
                    .instanceCount = 1,
                    .firstInstance = 0
                };


                vulkan_buffer_cpu_to_gpu_copy_and_upload_batch_global_staging_from_offset(
                    renderer, current_batch->draw_data_buffer_handle,
                    command_buffer,
                    &mesh_draw,
                    sizeof(Vulkan_Mesh_Draw));


                vulkan_buffer_cpu_to_gpu_copy_and_upload_batch_global_staging_from_offset(
                    renderer, current_batch->indirect_draw_buffer_handle,
                    command_buffer,
                    &indirect_draw,
                    sizeof(VkDrawIndexedIndirectCommand));


                current_batch->draw_count++;
            }
        }
    }


    Vulkan_Skinned_Draw skinned_draw = {0};
    for (u32 batch_idx = 0; batch_idx < renderer->shader_system->skinned_batch_count; batch_idx++)
    {
        Vulkan_Shader_Batch* current_batch = &renderer->shader_system->skinned_batch[batch_idx];
        current_batch->draw_count = 0;

        vulkan_buffer_reset_offset(renderer, current_batch->material_data_buffer_handle);
        vulkan_buffer_reset_offset(renderer, current_batch->draw_data_buffer_handle);
        vulkan_buffer_reset_offset(renderer, current_batch->indirect_draw_buffer_handle);


        //TODO/ OPTIMIZE: we can upload only whats needed per frame, instead of the whole thing,
        // but who knows if individual uploads would be slower?
        // honestly, just depends on the performance, should profile
        vulkan_buffer_cpu_to_gpu_copy_and_upload_batch_global_staging_from_offset(
            renderer, current_batch->material_data_buffer_handle,
            command_buffer,
            current_batch->material_batch_reference->material_data->data,
            dynamic_array_get_byte_size(current_batch->material_batch_reference->material_data));

        for (u32 item_idx = 0; item_idx < skinned_render_item_count; item_idx++)
        {
            Skinned_Render_Item* cur_render_item = &skinned_render_items[item_idx];

            if (cur_render_item->material_key == current_batch->material_id)
            {
                skinned_draw = (Vulkan_Skinned_Draw){
                    .transform_idx = cur_render_item->transform_handle,
                    .material_instance_handle = cur_render_item->material_handle,
                    .joint_idx = cur_render_item->joint_idx,
                    .weight_idx = cur_render_item->weight_idx,
                    .skinned_matrix_idx = cur_render_item->skinned_matrix_idx,
                    .vertex_offset_count = cur_render_item->vertex_offset,
                };
                indirect_draw = (VkDrawIndexedIndirectCommand){
                    .indexCount = cur_render_item->index_count,
                    .firstIndex = cur_render_item->index_offset,
                    .vertexOffset = cur_render_item->vertex_offset,
                    .instanceCount = 1,
                    .firstInstance = 0
                };


                vulkan_buffer_cpu_to_gpu_copy_and_upload_batch_global_staging_from_offset(
                    renderer, current_batch->draw_data_buffer_handle,
                    command_buffer,
                    &skinned_draw,
                    sizeof(Vulkan_Skinned_Draw));


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
