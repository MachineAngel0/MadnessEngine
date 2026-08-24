#include "vulkan_mesh_system.h"

#include "vk_buffer.h"
#include "vulkan_struct_types.h"


Vulkan_Mesh_System* mesh_renderer_init(Renderer* renderer)
{
    Vulkan_Mesh_System* mesh_system = allocator_alloc(&renderer->allocator, sizeof(Vulkan_Mesh_System));

    u64 mesh_buffer_data_size = MB(64);


    mesh_system->vertex_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                             BUFFER_TYPE_STORAGE_GPU,
                                                             mesh_buffer_data_size);
    mesh_system->index_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system, BUFFER_TYPE_INDEX,
                                                            mesh_buffer_data_size);


    mesh_system->normal_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                             BUFFER_TYPE_STORAGE_GPU, mesh_buffer_data_size);
    mesh_system->tangent_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                              BUFFER_TYPE_STORAGE_GPU, mesh_buffer_data_size);
    mesh_system->uv_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                         BUFFER_TYPE_STORAGE_GPU,
                                                         mesh_buffer_data_size);
    mesh_system->joint_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                            BUFFER_TYPE_STORAGE_GPU,
                                                            mesh_buffer_data_size);
    mesh_system->weight_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                             BUFFER_TYPE_STORAGE_GPU,
                                                             mesh_buffer_data_size);


    //DRAW DATA
    mesh_system->skinned_matrix_buffer = vulkan_buffer_create_frame(renderer, renderer->buffer_system,
                                                                    BUFFER_TYPE_STORAGE_GPU,
                                                                    mesh_buffer_data_size);

    u64 transform_buffer_memory_size = MAX_TRANSFORM_COUNT * sizeof(mat4s);

    mesh_system->transform_buffer_handle = vulkan_buffer_create_frame(renderer, renderer->buffer_system,
                                                                      BUFFER_TYPE_STORAGE_GPU,
                                                                      transform_buffer_memory_size);


    //
    mesh_system->mesh_submitted_upload_array = array_create(Mesh_Gpu_Upload_Pending, 1024, &renderer->allocator);
    mesh_system->mesh_pending_upload_array = array_create(Mesh_Unfinished_Upload, 1024, &renderer->allocator);
    timeline_semaphore_create(renderer, &mesh_system->mesh_upload_timeline_semaphore);
    mesh_system->mesh_upload_semaphore_value = 0;


    return mesh_system;
}


bool mesh_system_upload_data(Renderer* renderer, Vulkan_Mesh_System* mesh_system,
                             Vulkan_Command_Buffer* transfer_command_buffer,
                             Buffer_Handle handle,
                             Vulkan_Mesh_Data_Type type,
                             void* data,
                             u64 data_byte_size,
                             u32 mesh_id,
                             u64 mesh_upload_semaphore_value,
                             Mesh_Render_Record* record, bool is_initial_submit)
{

    if (data_byte_size == 0)
    {
        WARN("mesh_system_upload_data: passed in 0 data size, for type %d", type);
        return true;
    }

    if (is_initial_submit)
    {
        record->pending_uploads += 1;
    }

    if (vulkan_buffer_transfer_upload(renderer,
                                      transfer_command_buffer,
                                      handle,
                                      data,
                                      data_byte_size, mesh_upload_semaphore_value))
    {
        //successful upload
        Mesh_Gpu_Upload_Pending mesh_pending =
        {
            .mesh_id = mesh_id,
            .type = type,
            .timeline_semaphore_value = mesh_upload_semaphore_value,
        };
        array_push(mesh_system->mesh_submitted_upload_array, &mesh_pending);
        return true;
    }
    else
    {
        //we dont want to add this again, if its already an unfinished upload
        if (is_initial_submit)
        {
            //not enough memory, will try again next frame
            Mesh_Unfinished_Upload unfinished_upload = {
                .mesh_id = mesh_id,
                .type = type,
                .buffer_handle = handle,
                .bytes = data_byte_size,
                .data = data,
            };

            array_push(mesh_system->mesh_pending_upload_array, &unfinished_upload);
        }
        return false;
    }
}

void mesh_renderer_upload_draw_data(Renderer* renderer, Vulkan_Mesh_System* mesh_system, Render_Packet* render_packet,
                                    Vulkan_Command_Buffer* graphics_command_buffer)
{
    // return;
    //
    //mesh gpu uploads we are waiting on, checked once a frame
    u64 i = 0;

    while (i < mesh_system->mesh_submitted_upload_array->num_items)
    {
        Mesh_Gpu_Upload_Pending pending_upload =
            array_get(
                mesh_system->mesh_submitted_upload_array,
                Mesh_Gpu_Upload_Pending,
                i);

        if (timeline_semaphore_query_and_compare(
            renderer,
            vulkan_queue_system_get_transfer_semaphore(renderer),
            pending_upload.timeline_semaphore_value))
        {
            Mesh_Render_Record* mesh_record = &mesh_system->mesh_render_record[pending_upload.mesh_id];
            mesh_record->pending_uploads--;
            if (mesh_record->pending_uploads == 0)
            {
                mesh_record->is_uploaded = true;
            }

            array_remove_swap(
                mesh_system->mesh_submitted_upload_array,
                i);

            // Don't increment i.
            // The swapped-in element now occupies index i.
            continue;
        }

        i++;
    }


    if (mesh_system->mesh_pending_upload_array->num_items == 0 && ring_queue_is_empty(render_packet->mesh_queue))
    {
        return;
    }


    //TODO: we still want to flush our queues
    Vulkan_Command_Buffer* transfer_command_buffer = NULL;
    if (!vulkan_queue_system_get_transfer_command_buffer(renderer, &transfer_command_buffer))
    {
        return;
    }


    u64 mesh_upload_semaphore_value = ++mesh_system->mesh_upload_semaphore_value;

    //things we werent able to upload yet
    u64 unfinished_idx = 0;
    while (unfinished_idx < mesh_system->mesh_pending_upload_array->num_items)
    {
        Mesh_Unfinished_Upload unfinished_upload =
            array_get(
                mesh_system->mesh_pending_upload_array,
                Mesh_Unfinished_Upload,
                unfinished_idx);


        Mesh_Render_Record* record = &mesh_system->mesh_render_record[unfinished_upload.mesh_id];

        //if upload successful remove it from the array
        if (mesh_system_upload_data(renderer,
                                    mesh_system,
                                    transfer_command_buffer,
                                    unfinished_upload.buffer_handle,
                                    unfinished_upload.type,
                                    unfinished_upload.data,
                                    unfinished_upload.bytes,
                                    unfinished_upload.mesh_id,
                                    mesh_upload_semaphore_value,
                                    record, false))
        {
            // Don't increment i.
            // The swapped-in element now occupies index i.
            array_remove_swap(
                mesh_system->mesh_pending_upload_array,
                unfinished_idx);
            continue;
        }
        //upload could not be done
        unfinished_idx++;
    }


    ring_queue* mesh_render_queue = render_packet->mesh_queue;
    ring_queue* skinned_mesh_render_queue = render_packet->skinned_mesh_queue;


    //NOTE: rn it copies from an offset, which is fine for now,
    // but when the system needs to be more dynamic, its going to need a rewrite, especially the buffer system function calls

    //TODO: add memory barriers
    Mesh_GPU_Upload submesh_upload_data = {0};


    while (!ring_queue_is_empty(mesh_render_queue))
    {
        ring_dequeue(mesh_render_queue, &submesh_upload_data);

        Mesh_Render_Record* record = &mesh_system->mesh_render_record[submesh_upload_data.mesh_id];
        record->mesh_id = submesh_upload_data.mesh_id; // TODO: change this for now we are just testing, should be false
        record->is_uploaded = false; // TODO: change this for now we are just testing, should be false
        record->is_in_use = true;
        record->tangent_bytes = submesh_upload_data.submesh->tangent_bytes;
        record->vertex_color_bytes = submesh_upload_data.submesh->vertex_color_bytes;
        record->vertex_bytes = submesh_upload_data.submesh->vertex_bytes;
        record->normal_bytes = submesh_upload_data.submesh->normal_bytes;
        record->uv_bytes = submesh_upload_data.submesh->uv_bytes;
        record->indices_bytes = submesh_upload_data.submesh->indices_bytes;
        record->index_count = submesh_upload_data.submesh->index_count;
        record->index_type = submesh_upload_data.submesh->index_type;


        //TODO: TEMP CODE, need a free list for this
        record->vertex_count_offset = mesh_system->vertex_offset_count;
        record->index_offset_count = mesh_system->index_offset_count;
        mesh_system->vertex_offset_count += submesh_upload_data.submesh->vertex_count;
        mesh_system->index_offset_count += submesh_upload_data.submesh->index_count;

        mesh_system_upload_data(renderer,
                                mesh_system,
                                transfer_command_buffer,
                                mesh_system->vertex_buffer_handle,
                                VERTEX,
                                submesh_upload_data.gpu_data->vertex,
                                submesh_upload_data.submesh->vertex_bytes,
                                submesh_upload_data.mesh_id,
                                mesh_upload_semaphore_value,
                                record, true);

        mesh_system_upload_data(renderer,
                                mesh_system,
                                transfer_command_buffer,
                                mesh_system->index_buffer_handle,
                                INDEX,
                                submesh_upload_data.gpu_data->indices,
                                submesh_upload_data.submesh->indices_bytes,
                                submesh_upload_data.mesh_id,
                                mesh_upload_semaphore_value,
                                record, true);

        mesh_system_upload_data(renderer,
                                mesh_system,
                                transfer_command_buffer,
                                mesh_system->normal_buffer_handle,
                                NORMAL,
                                submesh_upload_data.gpu_data->normal,
                                submesh_upload_data.submesh->normal_bytes,
                                submesh_upload_data.mesh_id,
                                mesh_upload_semaphore_value,
                                record, true);

        mesh_system_upload_data(renderer,
                                mesh_system,
                                transfer_command_buffer,
                                mesh_system->uv_buffer_handle,
                                UV,
                                submesh_upload_data.gpu_data->uv,
                                submesh_upload_data.submesh->uv_bytes,
                                submesh_upload_data.mesh_id,
                                mesh_upload_semaphore_value,
                                record, true);

        mesh_system_upload_data(renderer,
                                mesh_system,
                                transfer_command_buffer,
                                mesh_system->tangent_buffer_handle,
                                TANGENT,
                                submesh_upload_data.gpu_data->tangent,
                                submesh_upload_data.submesh->tangent_bytes,
                                submesh_upload_data.mesh_id,
                                mesh_upload_semaphore_value,
                                record, true);
    }


    //skinned data
    /*u64 skinned_semaphore_value = ++mesh_system->skinned_upload_semaphore_value;
    Skinned_Mesh_GPU_Upload skinned_mesh_upload_data = {0};
    while (!ring_queue_is_empty(skinned_mesh_render_queue))
    {
        MASSERT(false); // TODO:
        ring_queue_peek(skinned_mesh_render_queue, &skinned_mesh_upload_data);

        u64 skinned_memory_request = skinned_mesh_upload_data.skinned_submesh->joint_bytes + skinned_mesh_upload_data.
            skinned_submesh->weight_bytes;
        u64 staging_offset = 0;
        if (!vulkan_buffer_upload_data_request(renderer, skinned_memory_request, &staging_offset))
        {
            //we cant service the request, move on
            break;
        }


        vulkan_buffer_staging_transfer_queue_upload(
            renderer, mesh_system->joint_buffer_handle,
            transfer_command_buffer,
            skinned_mesh_upload_data.skinned_gpu_data->joints,
            skinned_mesh_upload_data.skinned_submesh->joint_bytes);
        vulkan_buffer_staging_transfer_queue_upload(
            renderer, mesh_system->weight_buffer_handle,
            transfer_command_buffer,
            skinned_mesh_upload_data.skinned_gpu_data->weights,
            skinned_mesh_upload_data.skinned_submesh->weight_bytes);


        Skinned_Gpu_Upload_Pending skinned_pending =
        {
            .skinned_id = skinned_mesh_upload_data.skinned_id,
            .timeline_semaphore_value = skinned_semaphore_value,
        };
        array_push(mesh_system->skinned_pending_array, &skinned_pending);


        //remove from our queue since we have already proccessed it
        ring_dequeue_discard(skinned_mesh_render_queue);
    }*/
}

void mesh_renderer_upload_per_frame_data(Renderer* renderer, Vulkan_Mesh_System* mesh_renderer,
                                         Render_Packet* render_packet, Vulkan_Command_Buffer* command_buffer)
{
    //transform data
    vulkan_buffer_frame_reset(renderer, mesh_renderer->transform_buffer_handle);
    if (vulkan_buffer_frame_staging_upload(
        renderer, mesh_renderer->transform_buffer_handle,
        command_buffer, render_packet->draw_3d_data_packet.world_space_matrix_array,
        sizeof(mat4s) * render_packet->draw_3d_data_packet.
                                       world_space_matrix_count))
    {
        Vulkan_Buffer* transform_buffer = vulkan_buffer_get_frame(renderer, mesh_renderer->transform_buffer_handle);
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


    //skinned matrix
    vulkan_buffer_frame_reset(renderer, mesh_renderer->skinned_matrix_buffer);
    if (vulkan_buffer_frame_staging_upload(renderer, mesh_renderer->skinned_matrix_buffer,
                                           command_buffer,
                                           render_packet->draw_3d_data_packet.skinned_matrix->data,
                                           array_get_byte_size(render_packet->draw_3d_data_packet.skinned_matrix)))
    {
        //successful data uploads need a barrier
        Vulkan_Buffer* skinned_buffer = vulkan_buffer_get_frame(renderer, mesh_renderer->skinned_matrix_buffer);
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

}

void mesh_renderer_construct_batch_draw(Renderer* renderer,
                                        Render_Packet* render_packet,
                                        Vulkan_Command_Buffer* command_buffer)
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


    /*for (u32 record_idx = 0; record_idx < renderer->mesh_system->mesh_render_count; record_idx++)
    {
        Mesh_Render_Record* render_record = &renderer->mesh_system->mesh_render_record[record_idx];

        if (render_record->is_uploaded && render_record->is_in_use)
        {
            //add it to the draw list
        }
    }*/


    // Vulkan_Mesh_Draw draw_data = {
    // .transform_idx = sub_mesh_instance->parent_transform_handle.handle,
    // .material_instance_handle = sub_mesh_instance->material_handle.buffer_handle,
    // };

    //TODO: realizing now that visibility work load doenst need to happen in the renderer,
    // it can be an intermediate step that generates the info needed for the render packet,
    // unless you do gpu compute culling then that shit happens in the renderer
    for (u32 i = 0; i < render_packet->draw_3d_data_packet.mesh_instances_count; i++)
    {
        Madness_Mesh_Instance* mesh_instance = &render_packet->draw_3d_data_packet.mesh_instances[i];
        for (u32 submesh_idx = 0; submesh_idx < mesh_instance->mesh_count; submesh_idx++)
        {
            Madness_SubMesh_Instance* sub_mesh_instance = &mesh_instance->submesh_instances[submesh_idx];
            Mesh_Render_Record* render_record = &renderer->mesh_system->mesh_render_record[sub_mesh_instance->mesh_id];

            //check if its valid for uploading
            if (!render_record->is_uploaded || !render_record->is_in_use)
            {
                continue;
            }

            //TODO: check if material upload is valid yet, when we get around to that
            Mesh_Render_Item* render_inst = &render_items[render_item_count++];
            *render_inst = (Mesh_Render_Item){
                .material_key = sub_mesh_instance->material_handle.material_id,
                .mesh_id = sub_mesh_instance->mesh_id,
                .mesh_handle = mesh_instance->mesh_asset.handle,
                .submesh_handle = submesh_idx,
                .material_handle = sub_mesh_instance->material_handle.buffer_handle,
                .transform_handle = mesh_instance->transform_handle.handle,
                .index_count = render_record->index_count,
                .index_offset = render_record->index_offset_count,
                .vertex_offset = render_record->vertex_count_offset,
            };
        }
    }
    // }

    /*
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
                .transform_handle = mesh_instance->transform_handle.handle, // TODO: query the bindless index
                .index_count = sub_mesh_instance->mesh_indirect_draw.index_count,
                .index_offset = sub_mesh_instance->mesh_indirect_draw.index_offset,
                .vertex_offset = sub_mesh_instance->mesh_indirect_draw.vertex_count_offset,
                .joint_idx = sub_mesh_instance->skinned_draw_data.joint_idx,
                .weight_idx = sub_mesh_instance->skinned_draw_data.weight_idx,
                .skinned_matrix_idx = sub_mesh_instance->skinned_draw_data.skinned_matrix_idx,


            };
        }
    }*/


    //TODO/ OPTIMIZE : sort render items by material id,
    // when a render item doesn't match, that means we move onto the next batch

    VkDrawIndexedIndirectCommand indirect_draw = {0};
    Vulkan_Mesh_Draw mesh_draw = {0};
    for (u32 batch_idx = 0; batch_idx < renderer->shader_system->mesh_batch_count; batch_idx++)
    {
        Vulkan_Shader_Batch* current_batch = &renderer->shader_system->mesh_batch[batch_idx];
        current_batch->draw_count = 0;

        vulkan_buffer_frame_reset(renderer, current_batch->material_data_buffer_handle);
        vulkan_buffer_frame_reset(renderer, current_batch->draw_data_buffer_handle);
        vulkan_buffer_frame_reset(renderer, current_batch->indirect_draw_buffer_handle);


        //TODO/ OPTIMIZE: we can upload only whats needed per frame, instead of the whole thing,
        // but who knows if individual uploads would be slower?
        // honestly, just depends on the performance, should profile
        vulkan_buffer_frame_staging_upload(
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


                vulkan_buffer_frame_staging_upload(
                    renderer, current_batch->draw_data_buffer_handle,
                    command_buffer,
                    &mesh_draw,
                    sizeof(Vulkan_Mesh_Draw));


                vulkan_buffer_frame_staging_upload(
                    renderer, current_batch->indirect_draw_buffer_handle,
                    command_buffer,
                    &indirect_draw,
                    sizeof(VkDrawIndexedIndirectCommand));


                current_batch->draw_count++;
            }
        }
    }


    /*Vulkan_Skinned_Draw skinned_draw = {0};
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
    }*/
}


void mesh_renderer_batch_draw(Renderer* renderer, Vulkan_Mesh_System* mesh_renderer,
                              Vulkan_Shader_Batch* batch_draw_data, u32 batch_draw_count,
                              Vulkan_Command_Buffer* command_buffer)
{
    //only bind the vertex and index, the storage buffers are bda, so either in the ubo or the push constant

    for (u32 batch_idx = 0; batch_idx < batch_draw_count; ++batch_idx)
    {
        Vulkan_Shader_Batch* draw_data = &batch_draw_data[batch_idx];

        Vulkan_Buffer* indirect_buffer = vulkan_buffer_get_frame(renderer, draw_data->indirect_draw_buffer_handle);
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
                                &renderer->descriptor_system->uniform_descriptors.descriptor_sets[renderer->
                                    current_frame], 0, 0);
        //texturess
        vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                layout, 1, 1,
                                &renderer->descriptor_system->texture_descriptors.descriptor_sets[0], 0, 0);

        //storage buffers
        vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                layout, 2, 1,
                                &renderer->descriptor_system->storage_descriptors.descriptor_sets[renderer->
                                    current_frame], 0, 0);

        // vkCmdBindVertexBuffers(command_buffer->handle, 0, 1, &vertex_buffer->handle,
        // pOffsets);

        vkCmdBindIndexBuffer(command_buffer->handle, index_buffer->handle, 0,
                             VK_INDEX_TYPE_UINT16);


        draw_data->pc_data.draw_data_buffer = vulkan_buffer_get_frame_device_address(
            renderer, draw_data->draw_data_buffer_handle);
        draw_data->pc_data.material_buffer = vulkan_buffer_get_frame_device_address(
            renderer, draw_data->material_data_buffer_handle);

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


        if (renderer->features2.features.multiDrawIndirect)
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


void mesh_renderer_batch_draw_custom_pipeline(Renderer* renderer, Vulkan_Mesh_System* mesh_renderer,
                                              Vulkan_Shader_Batch* batch_draw_data, u32 batch_draw_count,
                                              Vulkan_Command_Buffer* command_buffer,
                                              Vulkan_Shader_Pipeline* shader_pipeline)
{
    MASSERT(shader_pipeline);
    //only bind the vertex and index, the storage buffers are bda, so either in the ubo or the push constant
    VkPipeline pipeline = shader_pipeline->handle;
    VkPipelineLayout layout = shader_pipeline->pipeline_layout;

    for (u32 batch_idx = 0; batch_idx < batch_draw_count; ++batch_idx)
    {
        Vulkan_Shader_Batch* draw_data = &batch_draw_data[batch_idx];


        Vulkan_Buffer* indirect_buffer = vulkan_buffer_get_frame(renderer, draw_data->indirect_draw_buffer_handle);
        Vulkan_Buffer* index_buffer = vulkan_buffer_get(renderer, mesh_renderer->index_buffer_handle);

        //figure out the shadow pass type we are using


        //UBER SHADER MESH INDIRECT DRAW
        vkCmdBindPipeline(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          pipeline);

        //uniform
        vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                layout, 0, 1,
                                &renderer->descriptor_system->uniform_descriptors.descriptor_sets[renderer->
                                    current_frame], 0, 0);
        //texturess
        vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                layout, 1, 1,
                                &renderer->descriptor_system->texture_descriptors.descriptor_sets[0], 0, 0);

        //storage buffers
        vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                layout, 2, 1,
                                &renderer->descriptor_system->storage_descriptors.descriptor_sets[renderer->
                                    current_frame], 0, 0);

        // vkCmdBindVertexBuffers(command_buffer->handle, 0, 1, &vertex_buffer->handle,
        // pOffsets);

        vkCmdBindIndexBuffer(command_buffer->handle, index_buffer->handle, 0,
                             VK_INDEX_TYPE_UINT16);

        draw_data->pc_data.draw_data_buffer = vulkan_buffer_get_frame_device_address(
            renderer, draw_data->draw_data_buffer_handle);
        draw_data->pc_data.material_buffer = vulkan_buffer_get_frame_device_address(
            renderer, draw_data->material_data_buffer_handle);


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


        if (renderer->features2.features.multiDrawIndirect)
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
