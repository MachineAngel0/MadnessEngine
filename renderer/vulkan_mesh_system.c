#include "vulkan_mesh_system.h"

#include "scene.h"
#include "vk_buffer.h"
#include "vulkan_struct_types.h"
#include "logger.h"
#include "sort_key.h"
#include "vulkan_shader_system.h"


Vulkan_Mesh_System* vulkan_mesh_system_init(Renderer* renderer)
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


    //
    mesh_system->mesh_submitted_upload_array = array_create(Mesh_Gpu_Upload_Pending, 1024, &renderer->allocator);
    mesh_system->mesh_pending_upload_array = array_create(Mesh_Unfinished_Upload, 1024, &renderer->allocator);
    timeline_semaphore_create(renderer, &mesh_system->mesh_upload_timeline_semaphore, 0);
    mesh_system->mesh_upload_semaphore_value = 0;


    return mesh_system;
}


bool mesh_system_upload_data_into_buffers(Renderer* renderer, Vulkan_Mesh_System* mesh_system,
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
            .submesh_id = mesh_id,
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
                .submesh_id = mesh_id,
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


void vulkan_mesh_system_update_async_resources(Renderer* renderer, Vulkan_Mesh_System* mesh_system)
{
    //mesh gpu uploads we are waiting on, checked once a frame
    u64 i = 0;

    while (i < mesh_system->mesh_submitted_upload_array->num_items)
    {
        Mesh_Gpu_Upload_Pending pending_upload =
            array_get(
                mesh_system->mesh_submitted_upload_array,
                i,
                Mesh_Gpu_Upload_Pending);

        if (timeline_semaphore_query_and_compare(
            renderer,
            vulkan_queue_system_get_transfer_semaphore(renderer),
            pending_upload.timeline_semaphore_value))
        {
            Mesh_Render_Record* mesh_record = &mesh_system->mesh_render_record[pending_upload.submesh_id];
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
}


void vulkan_mesh_system_upload_new_resources(Renderer* renderer, Vulkan_Mesh_System* mesh_system,
                                             Render_Packet* render_packet)
{
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

    //mesh data we werent able to upload yet
    u64 unfinished_idx = 0;
    while (unfinished_idx < mesh_system->mesh_pending_upload_array->num_items)
    {
        Mesh_Unfinished_Upload unfinished_upload =
            array_get(
                mesh_system->mesh_pending_upload_array,
                unfinished_idx,
                Mesh_Unfinished_Upload);


        Mesh_Render_Record* record = &mesh_system->mesh_render_record[unfinished_upload.submesh_id];

        //if upload successful remove it from the array
        if (mesh_system_upload_data_into_buffers(renderer,
                                                 mesh_system,
                                                 transfer_command_buffer,
                                                 unfinished_upload.buffer_handle,
                                                 unfinished_upload.type,
                                                 unfinished_upload.data,
                                                 unfinished_upload.bytes,
                                                 unfinished_upload.submesh_id,
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


    //TODO: get skinned data out of the upload data as well
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
        record->submesh_count = submesh_upload_data.submesh_count;
        record->mesh_id = submesh_upload_data.mesh_id;
        record->is_uploaded = false; // TODO: change this for now we are just testing, should be false
        record->submesh_data = submesh_upload_data.submesh;


        //allocate space for the record
        record->tangent_bytes_offset = allocator_heap_alloc(renderer->heap_allocator,
                                                            sizeof(u64) * record->submesh_count);
        record->vertex_color_bytes_offset = allocator_heap_alloc(renderer->heap_allocator,
                                                                 sizeof(u64) * record->submesh_count);
        record->vertex_bytes_offset = allocator_heap_alloc(renderer->heap_allocator,
                                                           sizeof(u64) * record->submesh_count);
        record->normal_bytes_offset = allocator_heap_alloc(renderer->heap_allocator,
                                                           sizeof(u64) * record->submesh_count);
        record->uv_bytes_offset = allocator_heap_alloc(renderer->heap_allocator, sizeof(u64) * record->submesh_count);


        for (u32 i = 0; i < submesh_upload_data.submesh_count; i++)
        {
            // TODO: change this for now we are just testing, should be false
            //indirect draw data
            record->index_count[i] = submesh_upload_data.submesh[i].index_count;
            record->index_type[i] = submesh_upload_data.submesh[i].index_type;

            //TODO: TEMP CODE, need a free list for this
            record->vertex_count_offset[i] = mesh_system->vertex_offset_count;
            record->index_offset_count[i] = mesh_system->index_offset_count;

            mesh_system->vertex_offset_count += submesh_upload_data.submesh[i].vertex_count;
            mesh_system->index_offset_count += submesh_upload_data.submesh[i].index_count;

            mesh_system_upload_data_into_buffers(renderer,
                                                 mesh_system,
                                                 transfer_command_buffer,
                                                 mesh_system->vertex_buffer_handle,
                                                 VERTEX,
                                                 submesh_upload_data.gpu_data[i].vertex,
                                                 submesh_upload_data.submesh[i].vertex_bytes,
                                                 submesh_upload_data.mesh_id,
                                                 mesh_upload_semaphore_value,
                                                 record, true);

            mesh_system_upload_data_into_buffers(renderer,
                                                 mesh_system,
                                                 transfer_command_buffer,
                                                 mesh_system->index_buffer_handle,
                                                 INDEX,
                                                 submesh_upload_data.gpu_data[i].indices,
                                                 submesh_upload_data.submesh[i].indices_bytes,
                                                 submesh_upload_data.mesh_id,
                                                 mesh_upload_semaphore_value,
                                                 record, true);

            mesh_system_upload_data_into_buffers(renderer,
                                                 mesh_system,
                                                 transfer_command_buffer,
                                                 mesh_system->normal_buffer_handle,
                                                 NORMAL,
                                                 submesh_upload_data.gpu_data[i].normal,
                                                 submesh_upload_data.submesh[i].normal_bytes,
                                                 submesh_upload_data.mesh_id,
                                                 mesh_upload_semaphore_value,
                                                 record, true);

            mesh_system_upload_data_into_buffers(renderer,
                                                 mesh_system,
                                                 transfer_command_buffer,
                                                 mesh_system->uv_buffer_handle,
                                                 UV,
                                                 submesh_upload_data.gpu_data[i].uv,
                                                 submesh_upload_data.submesh[i].uv_bytes,
                                                 submesh_upload_data.mesh_id,
                                                 mesh_upload_semaphore_value,
                                                 record, true);
            mesh_system_upload_data_into_buffers(renderer,
                                                 mesh_system,
                                                 transfer_command_buffer,
                                                 mesh_system->tangent_buffer_handle,
                                                 TANGENT,
                                                 submesh_upload_data.gpu_data[i].tangent,
                                                 submesh_upload_data.submesh[i].tangent_bytes,
                                                 submesh_upload_data.mesh_id,
                                                 mesh_upload_semaphore_value,
                                                 record, true);
        }
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


void mesh_renderer_construct_draw_lists(Renderer* renderer,
                                        Render_Packet* render_packet,
                                        Vulkan_Command_Buffer* graphics_command_buffer,
                                        Vulkan_Mesh_Render_List* render_list)
{
    //TODO: were going to assume we have already done culling,
    // but we would need to pass in another structure for actually culled data
    // or even cull and create the render format at the same time, instead of two seperate arrays


    //steps:
    // 1. cull // TODO:
    // 2. add items into each appropriate renderpass list
    // 3. sort each renderpass lists and their items
    // 4. batch the instances
    // 5. add the draws into the buffer


    // loop through our meshes and create render items for each

    //TODO: TEMP
    render_list->mesh_renderable = allocator_alloc(&renderer->frame_allocator,
                                                   1000 * sizeof(Vulkan_Mesh_Renderable));
    Vulkan_Mesh_Renderable* render_items = render_list->mesh_renderable;
    u32 render_item_count = 0;
    render_list->draw_count = 0; // TODO: move out to a frame start

    // Vulkan_Mesh_Draw draw_data = {
    // .transform_idx = sub_mesh_instance->parent_transform_handle.handle,
    // .material_instance_handle = sub_mesh_instance->material_handle.buffer_handle,
    // };
    render_list->mesh_renderable_count = render_packet->draw_3d_data_packet.mesh_instances_count;

    for (u32 i = 0; i < render_packet->draw_3d_data_packet.mesh_instances_count; i++)
    {
        Madness_Mesh_Instance* mesh_instance = &render_packet->draw_3d_data_packet.mesh_instances[i];
        Mesh_Render_Record* render_record = &renderer->mesh_system->mesh_render_record[mesh_instance->
            madness_mesh_index];

        //check if its valid for uploading
        if (!render_record->is_uploaded)
        {
            continue;
        }

        //TODO: check if material upload is valid yet, when we get around to that,
        //NOTE: YOU CAN JUST ASSUME MATERIALS and TEXTURES ARE VALID


        for (u32 submesh_idx = 0; submesh_idx < mesh_instance->submesh_count; submesh_idx++)
        {
            Vulkan_Mesh_Renderable* render_inst = &render_items[render_item_count++];

            *render_inst = (Vulkan_Mesh_Renderable){
                //TODO: each sort key should vary based on pass, as they are being added into the list
                // .shader_sort_key = sort_key_create_oqaque(mesh_instance->pipeline_hash[i], mesh_instance->permutation[i]),
                .pipeline_hash = mesh_instance->pipeline_hash[i],
                .permutation = mesh_instance->permutation[i],
                .render_pass_masks = 0,
                .mesh_id = mesh_instance->madness_mesh_index,
                .visible = true,
                .index_count = render_record->index_count[i],
                .index_offset = render_record->index_offset_count[i],
                .vertex_offset = render_record->vertex_count_offset[i],
                .mesh_render_item = (Mesh_GPU_Renderable){
                    .material_idx = mesh_instance->material_handles[i].handle,
                    .transform_idx = mesh_instance->transform_handle.handle,
                    .normal_offset = render_record->normal_bytes_offset[i],
                    .uv_offset = render_record->uv_bytes_offset[i],
                    .tangent_offset = render_record->tangent_bytes_offset[i],
                    .joint_idx = 0,
                    .weight_idx = 0,
                    .skinned_matrix_idx = 0,
                }
            };
        }
    }

    //TODO: append to each render pass type


    //TODO: sort the meshes by sort key
    qsort(render_items, render_item_count, sizeof(Vulkan_Mesh_Renderable), vulkan_mesh_sort_key);


    //TODO/ OPTIMIZE : when a render item doesn't match, that means we move onto the next batch
    // we can also upload based on that range instead of individual uploads

    VkDrawIndexedIndirectCommand indirect_draw = {0};

    for (u32 i = 0; i < render_item_count; i++)
    {
        //TODO: check if material upload is valid yet, when we get around to that
        Vulkan_Mesh_Renderable* mesh_item = &render_items[i];

        // Vulkan_Shader* shader = vulkan_shader_system_get_by_pipeline_hash(renderer, mesh_item->pipeline_hash);

        //TODO: batch instances, basically figure it out
        indirect_draw = (VkDrawIndexedIndirectCommand){
            .indexCount = mesh_item->index_count,
            .firstIndex = mesh_item->index_offset,
            .vertexOffset = mesh_item->vertex_offset,
            .instanceCount = 1,
            .firstInstance = 0
        };

        vulkan_buffer_frame_staging_upload(
            renderer, render_list->mesh_draw_data,
            graphics_command_buffer,
            &mesh_item->mesh_render_item,
            sizeof(Mesh_GPU_Renderable));


        vulkan_buffer_frame_staging_upload(
            renderer, render_list->indirect_buffer,
            graphics_command_buffer,
            &indirect_draw,
            sizeof(VkDrawIndexedIndirectCommand));

        render_list->draw_count++; // TODO: modify it later to count each instance draw
    }


    VkBufferMemoryBarrier2 mesh_draw_data_barrier = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
        .srcStageMask = VK_PIPELINE_STAGE_2_COPY_BIT,
        .srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,

        .dstStageMask = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
        .dstAccessMask = VK_ACCESS_2_SHADER_STORAGE_READ_BIT,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,

        .buffer = vulkan_buffer_get_frame(renderer, render_list->mesh_draw_data)->handle,
        .offset = 0,
        .size = VK_WHOLE_SIZE,
    };


    VkBufferMemoryBarrier2 indirect_buffer_barrier = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
        .srcStageMask = VK_PIPELINE_STAGE_2_COPY_BIT,
        .srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,

        .dstStageMask = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
        .dstAccessMask = VK_ACCESS_2_SHADER_STORAGE_READ_BIT,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,

        .buffer = vulkan_buffer_get_frame(renderer, render_list->indirect_buffer)->handle,
        .offset = 0,
        .size = VK_WHOLE_SIZE,
    };


    vulkan_command_add_buffer_barrier(graphics_command_buffer, mesh_draw_data_barrier);
    vulkan_command_add_buffer_barrier(graphics_command_buffer, indirect_buffer_barrier);


    //SKINNED
    /*
    Vulkan_Skinned_Render_Item* skinned_render_items = allocator_alloc(&renderer->frame_allocator,
                                                                  100 * sizeof(Vulkan_Skinned_Render_Item));
    u32 skinned_render_item_count = 0;

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
                              Vulkan_Command_Buffer* command_buffer, Vulkan_Mesh_Render_List* render_list)
{
    //only bind the vertex and index, the storage buffers are bda, so either in the ubo or the push constant

    Vulkan_Buffer* indirect_buffer = vulkan_buffer_get_frame(renderer, render_list->indirect_buffer);

    for (u32 draw_idx = 0; draw_idx < render_list->draw_count; ++draw_idx)
    {
        Vulkan_Shader_Batch* vulkan_shader = vulkan_shader_system_get_by_pipeline_hash(
            renderer, render_list->mesh_renderable[draw_idx].pipeline_hash);

        if (!vulkan_shader)
        {
            FATAL("mesh_renderer_batch_draw: could not find vulkan shader")
            continue;
        }

        Vulkan_Buffer* index_buffer = vulkan_buffer_get(renderer, mesh_renderer->index_buffer_handle);

        //get pipeline
        VkPipeline pipeline = vulkan_shader->pipeline.handle;
        VkPipelineLayout layout = vulkan_shader->pipeline.pipeline_layout;

        //check if we are using wireframe_mode
        if (renderer->wireframe_mode)
        {
            pipeline = vulkan_shader->wireframe_pipeline.handle;
            layout = vulkan_shader->wireframe_pipeline.pipeline_layout;;
        }


        //UBER SHADER MESH INDIRECT DRAW
        vkCmdBindPipeline(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          pipeline);

        //uniform
        vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                layout, 0, 1,
                                &renderer->descriptor_system->uniform_descriptors.descriptor_sets[renderer->
                                    current_frame_index], 0, 0);
        //texturess
        vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                layout, 1, 1,
                                &renderer->descriptor_system->texture_descriptors.descriptor_sets[0], 0, 0);

        //storage buffers
        vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                layout, 2, 1,
                                &renderer->descriptor_system->storage_descriptors.descriptor_sets[renderer->
                                    current_frame_index], 0, 0);

        // vkCmdBindVertexBuffers(command_buffer->handle, 0, 1, &vertex_buffer->handle,
        // pOffsets);

        vkCmdBindIndexBuffer(command_buffer->handle, index_buffer->handle, 0,
                             VK_INDEX_TYPE_UINT16);


        PC_General pc_data = {
            .draw_data_buffer = vulkan_buffer_get_frame_device_address(renderer, render_list->mesh_draw_data),
            .material_buffer = 0,
        };


        VkPushConstantsInfo push_constant_info = {0};
        push_constant_info.sType = VK_STRUCTURE_TYPE_PUSH_CONSTANTS_INFO;
        push_constant_info.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        push_constant_info.layout = layout;
        push_constant_info.offset = 0;
        push_constant_info.size = sizeof(PC_General);
        // push_constant_info.size = sizeof(draw_data->general); // make sure its not a pointer if i use this
        push_constant_info.pValues = &pc_data;
        push_constant_info.pNext = NULL;
        vkCmdPushConstants2(command_buffer->handle, &push_constant_info);


        if (renderer->features2.features.multiDrawIndirect)
        {
            vkCmdDrawIndexedIndirect(command_buffer->handle,
                                     indirect_buffer->handle, 0,
                                     render_list->draw_count,
                                     sizeof(VkDrawIndexedIndirectCommand));
        }
        else
        {
            // If multi draw is not available, we must issue separate draw commands
            for (u64 j = 0; j < render_list->draw_count; j++)
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
                                              Vulkan_Command_Buffer* command_buffer,
                                              Vulkan_Shader_Pipeline* vulkan_pipeline,
                                              Vulkan_Mesh_Render_List* render_list)
{
    //only bind the vertex and index, the storage buffers are bda, so either in the ubo or the push constant

    Vulkan_Buffer* indirect_buffer = vulkan_buffer_get_frame(renderer, render_list->indirect_buffer);

    for (u32 draw_idx = 0; draw_idx < render_list->draw_count; ++draw_idx)
    {
        Vulkan_Buffer* index_buffer = vulkan_buffer_get(renderer, mesh_renderer->index_buffer_handle);

        //get pipeline
        VkPipeline pipeline = vulkan_pipeline->handle;
        VkPipelineLayout layout = vulkan_pipeline->pipeline_layout;

        /*//check if we are using wireframe_mode
        if (renderer->wireframe_mode)
        {
            pipeline = vulkan_pipeline->handle;
            layout = vulkan_pipeline->pipeline_layout;;
        }*/


        //UBER SHADER MESH INDIRECT DRAW
        vkCmdBindPipeline(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          pipeline);

        //uniform
        vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                layout, 0, 1,
                                &renderer->descriptor_system->uniform_descriptors.descriptor_sets[renderer->
                                    current_frame_index], 0, 0);
        //texturess
        vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                layout, 1, 1,
                                &renderer->descriptor_system->texture_descriptors.descriptor_sets[0], 0, 0);

        //storage buffers
        vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                layout, 2, 1,
                                &renderer->descriptor_system->storage_descriptors.descriptor_sets[renderer->
                                    current_frame_index], 0, 0);

        // vkCmdBindVertexBuffers(command_buffer->handle, 0, 1, &vertex_buffer->handle,
        // pOffsets);

        vkCmdBindIndexBuffer(command_buffer->handle, index_buffer->handle, 0,
                             VK_INDEX_TYPE_UINT16);


        PC_General pc_data = {
            .draw_data_buffer = vulkan_buffer_get_frame_device_address(renderer, render_list->mesh_draw_data),
            .material_buffer = 0,
        };


        VkPushConstantsInfo push_constant_info = {0};
        push_constant_info.sType = VK_STRUCTURE_TYPE_PUSH_CONSTANTS_INFO;
        push_constant_info.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        push_constant_info.layout = layout;
        push_constant_info.offset = 0;
        push_constant_info.size = sizeof(PC_General);
        // push_constant_info.size = sizeof(draw_data->general); // make sure its not a pointer if i use this
        push_constant_info.pValues = &pc_data;
        push_constant_info.pNext = NULL;
        vkCmdPushConstants2(command_buffer->handle, &push_constant_info);


        if (renderer->features2.features.multiDrawIndirect)
        {
            vkCmdDrawIndexedIndirect(command_buffer->handle,
                                     indirect_buffer->handle, 0,
                                     render_list->draw_count,
                                     sizeof(VkDrawIndexedIndirectCommand));
        }
        else
        {
            // If multi draw is not available, we must issue separate draw commands
            for (u64 j = 0; j < render_list->draw_count; j++)
            {
                vkCmdDrawIndexedIndirect(command_buffer->handle,
                                         indirect_buffer->handle,
                                         j * sizeof(VkDrawIndexedIndirectCommand), 1,
                                         sizeof(VkDrawIndexedIndirectCommand));
            }
        }
    }
}
