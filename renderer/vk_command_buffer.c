#include "vk_command_buffer.h"
#include <string.h>


Vulkan_Queue_System* vulkan_queue_system_init(Renderer* renderer)
{
    Vulkan_Queue_System* queue_system = allocator_alloc(&renderer->allocator,
                                                        sizeof(Vulkan_Queue_System));


    //Graphics
    queue_system->graphics_queue = renderer->graphics_queue;
    queue_system->graphics_pool = renderer->graphics_command_pool;
    queue_system->graphics_queue_index = renderer->graphics_queue_index;
    Vulkan_Graphics_Queue* graphics_render_queue = &queue_system->graphics_render_queue;

    u8 frames_in_flight = renderer->max_frames_in_flight;
    // u8 swapchain_image_count = renderer->swapchain.image_count; // TODO: remove when your certain you dont need it


    graphics_render_queue->frame_submit_fence = allocator_alloc(
        &renderer->allocator, sizeof(VkFence) * frames_in_flight);
    graphics_render_queue->graphics_command_buffer = allocator_alloc(
        &renderer->allocator, sizeof(Vulkan_Command_Buffer) * frames_in_flight);
    graphics_render_queue->swapchain_signal_semaphore = allocator_alloc(
        &renderer->allocator, sizeof(VkSemaphore) * frames_in_flight);
    graphics_render_queue->swapchain_wait_semaphore = allocator_alloc(
        &renderer->allocator, sizeof(VkSemaphore) * frames_in_flight);

    //Transfer
    //TODO: temp for testing
    queue_system->transfer_queue = renderer->transfer_queue;
    queue_system->transfer_pool = renderer->transfer_command_pool;
    queue_system->transfer_queue_index = renderer->transfer_queue_index;
    Vulkan_Transfer_Queue* transfer_render_queue = &queue_system->transfer_render_queue;
    timeline_semaphore_create(renderer, &transfer_render_queue->timeline_semaphore);
    transfer_render_queue->semaphore_value = 0;

    transfer_render_queue->transfer_command_buffer_in_flight = array_create(Transfer_Command_Buffer_In_Flight,
                                                                            MAX_VULKAN_COMMAND_BUFFERS,
                                                                            &renderer->allocator);


    //anything needed per frame in flight
    for (size_t i = 0; i < frames_in_flight; i++)
    {
        //fences
        vulkan_fence_create(renderer, &queue_system->graphics_render_queue.frame_submit_fence[i]);

        //command buffers
        vulkan_command_buffer_allocate(&queue_system->graphics_render_queue.graphics_command_buffer[i],
                                       VULKAN_COMMAND_BUFFER_LEVEL_PRIMARY,
                                       queue_system->graphics_pool, renderer);

        binary_semaphore_create(renderer, &graphics_render_queue->swapchain_signal_semaphore[i]);
        binary_semaphore_create(renderer, &graphics_render_queue->swapchain_wait_semaphore[i]);
    }

    //command buffers
    transfer_render_queue->command_buffer_count = MAX_VULKAN_COMMAND_BUFFERS;
    for (u32 i = 0; i < transfer_render_queue->command_buffer_count; i++)
    {
        Vulkan_Command_Buffer* cb = &transfer_render_queue->command_buffer[i];
        if (!vulkan_command_buffer_allocate(cb, VULKAN_COMMAND_BUFFER_LEVEL_PRIMARY,
                                            renderer->transfer_command_pool,
                                            renderer))
        {
            cb->handle = 0;
            transfer_render_queue->command_buffer_count = i - 1;
            break;
        }
        cb->state = VULKAN_COMMAND_BUFFER_STATE_USABLE;
        cb->queue_type = VULKAN_QUEUE_TYPE_TRANSFER;
        cb->id = i;
    }


    return queue_system;
}

bool vulkan_queue_system_deinit(Renderer* renderer, Vulkan_Queue_System* queue_system)
{
    vkDeviceWaitIdle(renderer->logical_device);
    return true;
}

void vulkan_queue_frame_begin(Renderer* renderer, u32 current_frame)
{
    Vulkan_Queue_System* queue_system = renderer->queue_system;
    Vulkan_Transfer_Queue* transfer_queue = &queue_system->transfer_render_queue;

    u32 i = 0;
    while (i < transfer_queue->transfer_command_buffer_in_flight->num_items)
    {
        Transfer_Command_Buffer_In_Flight inflight = array_get(transfer_queue->transfer_command_buffer_in_flight,
                                                               Transfer_Command_Buffer_In_Flight, i);

        if (timeline_semaphore_query_and_compare(
            renderer,
            inflight.timeline_semaphore,
            inflight.semaphore_value))
        {
            inflight.command_buffer->state = VULKAN_COMMAND_BUFFER_STATE_USABLE;
            array_remove_swap(transfer_queue->transfer_command_buffer_in_flight, i);
            continue;
        }
        i++;
    }

    transfer_queue->semaphore_incremented_this_frame = false;
}

void vulkan_queue_frame_end(Renderer* renderer, u32 current_frame, u32 image_index)
{
    //TODO: assume we have a thread sync point here before executing the rest
    Scratch_Allocator scratch = scratch_allocator_begin(&renderer->allocator);
    Vulkan_Queue_System* queue_system = renderer->queue_system;


    Vulkan_Transfer_Queue* transfer_queue = &queue_system->transfer_render_queue;

    VkCommandBufferSubmitInfo* transfer_cb_submit_infos = allocator_alloc(
        scratch.allocator, sizeof(VkCommandBufferSubmitInfo) * transfer_queue->command_buffer_count);
    u32 ready_commands_buffers = 0;


    for (u32 i = 0; i < transfer_queue->command_buffer_count; i++)
    {
        Vulkan_Command_Buffer* current_transfer_buffer = &transfer_queue->command_buffer[i];
        if (current_transfer_buffer->state == VULKAN_COMMAND_BUFFER_STATE_END)
        {
            transfer_cb_submit_infos[ready_commands_buffers++] = vulkan_command_buffer_get_submit_info(
                current_transfer_buffer);

            current_transfer_buffer->state = VULKAN_COMMAND_BUFFER_STATE_SUBMITTED;

            Transfer_Command_Buffer_In_Flight upload = {
                .command_buffer = current_transfer_buffer,
                .timeline_semaphore = transfer_queue->timeline_semaphore,
                .semaphore_value = transfer_queue->semaphore_value,
            };
            array_push(transfer_queue->transfer_command_buffer_in_flight, &upload);
        }
        if (transfer_queue->command_buffer[i].state == VULKAN_COMMAND_BUFFER_STATE_BEGIN)
        {
            vulkan_command_buffer_end(current_transfer_buffer);
            transfer_cb_submit_infos[ready_commands_buffers++] = vulkan_command_buffer_get_submit_info(
                current_transfer_buffer);

            current_transfer_buffer->state = VULKAN_COMMAND_BUFFER_STATE_SUBMITTED;

            Transfer_Command_Buffer_In_Flight upload = {
                .command_buffer = current_transfer_buffer,
                .timeline_semaphore = transfer_queue->timeline_semaphore,
                .semaphore_value = transfer_queue->semaphore_value,
            };
            array_push(transfer_queue->transfer_command_buffer_in_flight, &upload);
        }
    }




    VkSubmitInfo2 transfer_submit_info = {0};
    transfer_submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
    transfer_submit_info.pNext = 0;
    transfer_submit_info.flags = 0;
    transfer_submit_info.commandBufferInfoCount = ready_commands_buffers;
    transfer_submit_info.pCommandBufferInfos = transfer_cb_submit_infos;
    transfer_submit_info.waitSemaphoreInfoCount = transfer_queue->wait_semaphore_info_count;
    transfer_submit_info.pWaitSemaphoreInfos = transfer_queue->wait_semaphore_info;
    transfer_submit_info.signalSemaphoreInfoCount = transfer_queue->signal_semaphore_info_count;
    transfer_submit_info.pSignalSemaphoreInfos = transfer_queue->signal_semaphore_info;

    VkResult transfer_result = vkQueueSubmit2(queue_system->transfer_queue, 1, &transfer_submit_info, NULL);
    VK_CHECK(transfer_result);


    //GRAPHICS

    Vulkan_Graphics_Queue* graphics_queue = &renderer->queue_system->graphics_render_queue;
    Vulkan_Command_Buffer* graphics_cb = &graphics_queue->graphics_command_buffer[current_frame];
    VkCommandBufferSubmitInfo cb_submit_info = vulkan_command_buffer_get_submit_info(graphics_cb);

    //easiest way of adding the swapchain semaphores
    VkSemaphoreSubmitInfo swapchain_wait_semaphore = {0};
    swapchain_wait_semaphore.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    swapchain_wait_semaphore.semaphore = graphics_queue->swapchain_wait_semaphore[current_frame];
    swapchain_wait_semaphore.value = 0; // not needed for binary semaphores
    swapchain_wait_semaphore.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSemaphoreSubmitInfo swapchain_signal_semaphore = {0};
    swapchain_signal_semaphore.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    swapchain_signal_semaphore.semaphore = graphics_queue->swapchain_signal_semaphore[current_frame];
    swapchain_signal_semaphore.value = 0; // not needed for binary semaphores
    swapchain_signal_semaphore.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;

    vulkan_queue_add_wait_semaphore(renderer, VULKAN_QUEUE_TYPE_GRAPHICS, swapchain_wait_semaphore);
    vulkan_queue_add_signal_semaphore(renderer, VULKAN_QUEUE_TYPE_GRAPHICS, swapchain_signal_semaphore);

    VkSubmitInfo2 submit_info = {0};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
    submit_info.pNext = 0;
    submit_info.flags = 0;
    submit_info.commandBufferInfoCount = 1;
    submit_info.pCommandBufferInfos = &cb_submit_info;
    submit_info.waitSemaphoreInfoCount = graphics_queue->wait_semaphore_info_count;
    submit_info.pWaitSemaphoreInfos = graphics_queue->wait_semaphore_info;
    submit_info.signalSemaphoreInfoCount = graphics_queue->signal_semaphore_info_count;
    submit_info.pSignalSemaphoreInfos = graphics_queue->signal_semaphore_info;


    VkResult graphics_result = vkQueueSubmit2(queue_system->graphics_queue, 1, &submit_info,
                                              graphics_queue->frame_submit_fence[current_frame]);

    VK_CHECK(graphics_result);
    scratch_allocator_end(scratch);


    /*
    // Give the image back to the swapchain.
    vulkan_swapchain_present_image(
        renderer,
        &renderer->context,
        &renderer->context.swapchain,
        renderer->context.present_queue,
        renderer->queue_system->graphics_render_queue.swapchain_signal_semaphore[image_index],
        image_index);*/

    //reset semaphore counts

    graphics_queue->wait_semaphore_info_count = 0;
    graphics_queue->signal_semaphore_info_count = 0;

    transfer_queue->signal_semaphore_info_count = 0;
    transfer_queue->wait_semaphore_info_count = 0;

    queue_system->compute_render_queue.signal_semaphore_count = 0;
    queue_system->compute_render_queue.wait_semaphore_count = 0;
}

void vulkan_queue_system_graphics_fence_wait(Renderer* renderer, u32 current_frame)
{
    Vulkan_Queue_System* queue_system = renderer->queue_system;
    if (!vulkan_fence_wait(
        renderer,
        &queue_system->graphics_render_queue.frame_submit_fence[current_frame],
        UINT64_MAX))
    {
        WARN("GRAPHICS START FRAME: In-flight fence wait failure!");
        return;
    }

    queue_system->graphics_render_queue.graphics_command_buffer[current_frame].state =
        VULKAN_COMMAND_BUFFER_STATE_USABLE;
}

bool vulkan_queue_system_get_cb(Renderer* renderer, Vulkan_Queue_Type type,
                                Vulkan_Command_Buffer** out_cb)
{
    Vulkan_Queue_System* system = renderer->queue_system;
    switch (type)
    {
    case VULKAN_QUEUE_TYPE_GRAPHICS:
        return vulkan_queue_system_get_graphics_command_buffer(renderer, out_cb);
        break;
    case VULKAN_QUEUE_TYPE_TRANSFER:
        return vulkan_queue_system_get_transfer_command_buffer(renderer, out_cb);
        break;
    case VULKAN_QUEUE_TYPE_COMPUTE:
        MASSERT_FALSE()
        *out_cb = &system->compute_render_queue.command_buffer[renderer->current_frame];
        break;
    }


    INFO("NO COMMAND BUFFERS AVAILABLE")
    return false;
}

bool vulkan_queue_system_get_graphics_command_buffer(Renderer* renderer, Vulkan_Command_Buffer** out_cb)
{
    Vulkan_Queue_System* system = renderer->queue_system;
    Vulkan_Command_Buffer* command_buffer = &system->graphics_render_queue.graphics_command_buffer[renderer->
        current_frame];

    if (command_buffer->state != VULKAN_COMMAND_BUFFER_STATE_BEGIN)
    {
        WARN("vulkan_queue_get_graphics_command_buffer: trying to get graphics command buffer before frame has started")
        return false;
    }

    // *out_cb = &system->graphics_render_queue.graphics_command_buffer[renderer->current_frame];
    *out_cb = command_buffer;
    return true;
}

bool vulkan_queue_system_get_transfer_command_buffer(Renderer* renderer, Vulkan_Command_Buffer** out_cb)
{
    Vulkan_Queue_System* system = renderer->queue_system;
    for (u32 i = 0; i < system->transfer_render_queue.command_buffer_count; i++)
    {
        //search for any command buffers already in use
        if ((system->transfer_render_queue.command_buffer[i].state == VULKAN_COMMAND_BUFFER_STATE_BEGIN))
        {
            *out_cb = &system->transfer_render_queue.command_buffer[i];
            if (!system->transfer_render_queue.semaphore_incremented_this_frame)
            {
                system->transfer_render_queue.semaphore_incremented_this_frame = true;
                system->transfer_render_queue.semaphore_value++;
            }
            return true;
        }
    }
    for (u32 i = 0; i < system->transfer_render_queue.command_buffer_count; i++)
    {
        //if we dont find one, grab one and begin it
        if ((system->transfer_render_queue.command_buffer[i].state == VULKAN_COMMAND_BUFFER_STATE_USABLE))
        {
            vulkan_command_buffer_begin(&system->transfer_render_queue.command_buffer[i]);
            *out_cb = &system->transfer_render_queue.command_buffer[i];

            if (!system->transfer_render_queue.semaphore_incremented_this_frame)
            {
                system->transfer_render_queue.semaphore_incremented_this_frame = true;
                system->transfer_render_queue.semaphore_value++;
            }

            return true;
        }
    }

    INFO("vulkan_queue_system_get_transfer_command_buffer: no transfer command buffer currently available")
    return false;
}

void vulkan_queue_system_get_transfer_semaphore_value(Renderer* renderer, u64* out_semaphore_value)
{
    if (!renderer->queue_system->transfer_render_queue.semaphore_incremented_this_frame)
    {
        renderer->queue_system->transfer_render_queue.semaphore_incremented_this_frame = true;
        renderer->queue_system->transfer_render_queue.semaphore_value++;
    }
    *out_semaphore_value = renderer->queue_system->transfer_render_queue.semaphore_value;
}

VkSemaphore vulkan_queue_system_get_transfer_semaphore(Renderer* renderer)
{
    return renderer->queue_system->transfer_render_queue.timeline_semaphore;
}

bool vulkan_queue_system_get_primary_command_buffer(Renderer* renderer, Vulkan_Queue_Type type,
                                                    Vulkan_Command_Buffer** out_cb)
{
    *out_cb = &renderer->queue_system->graphics_render_queue.graphics_command_buffer[renderer->current_frame];
    return true;
}


bool vulkan_queue_add_signal_semaphore(Renderer* renderer, Vulkan_Queue_Type queue_type,
                                       VkSemaphoreSubmitInfo submit_info)
{
    Vulkan_Queue_System* queue_system = renderer->queue_system;
    switch (queue_type)
    {
    case VULKAN_QUEUE_TYPE_GRAPHICS:
        queue_system->graphics_render_queue.signal_semaphore_info[queue_system->graphics_render_queue.
            signal_semaphore_info_count++] = submit_info;
        return true;
        break;
    case VULKAN_QUEUE_TYPE_TRANSFER:
        queue_system->transfer_render_queue.signal_semaphore_info[queue_system->transfer_render_queue.
            signal_semaphore_info_count++] = submit_info;
        return true;
        break;
    case VULKAN_QUEUE_TYPE_COMPUTE:
        queue_system->compute_render_queue.signal_semaphore[queue_system->compute_render_queue.
                                                                          signal_semaphore_count++] = submit_info;
        return true;
        break;
    }
    return false;
}

bool vulkan_queue_add_wait_semaphore(Renderer* renderer, Vulkan_Queue_Type queue_type,
                                     VkSemaphoreSubmitInfo submit_info)
{
    Vulkan_Queue_System* queue_system = renderer->queue_system;
    switch (queue_type)
    {
    case VULKAN_QUEUE_TYPE_GRAPHICS:

        queue_system->graphics_render_queue.wait_semaphore_info[queue_system->graphics_render_queue.
                                                                              wait_semaphore_info_count++] =
            submit_info;
        return true;
        break;
    case VULKAN_QUEUE_TYPE_TRANSFER:
        queue_system->transfer_render_queue.wait_semaphore_info[queue_system->transfer_render_queue.
                                                                              wait_semaphore_info_count++] =
            submit_info;
        return true;
        break;
    case VULKAN_QUEUE_TYPE_COMPUTE:
        queue_system->compute_render_queue.wait_semaphore[queue_system->compute_render_queue.
                                                                        wait_semaphore_count++] = submit_info;
        return true;
        break;
    }

    MASSERT_FALSE()
    return false;
}


bool vulkan_command_add_image_barrier(Vulkan_Command_Buffer* command_buffer,
                                      VkImageMemoryBarrier2 image_memory_barrier)
{
    VkDependencyInfo dependency_info = {0};
    dependency_info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    dependency_info.pNext = NULL;
    dependency_info.dependencyFlags = 0;
    dependency_info.memoryBarrierCount = 0;
    dependency_info.pMemoryBarriers = NULL;
    dependency_info.bufferMemoryBarrierCount = 0;
    dependency_info.pBufferMemoryBarriers = 0;
    dependency_info.imageMemoryBarrierCount = 1;
    dependency_info.pImageMemoryBarriers = &image_memory_barrier;

    vkCmdPipelineBarrier2(command_buffer->handle, &dependency_info);

    return true;
}

bool vulkan_command_add_buffer_barrier(Vulkan_Command_Buffer* command_buffer,
                                       VkBufferMemoryBarrier2 buffer_memory_barrier)
{
    VkDependencyInfo dependency_info = {0};
    dependency_info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    dependency_info.pNext = NULL;
    dependency_info.dependencyFlags = 0;
    dependency_info.memoryBarrierCount = 0;
    dependency_info.pMemoryBarriers = NULL;
    dependency_info.bufferMemoryBarrierCount = 1;
    dependency_info.pBufferMemoryBarriers = &buffer_memory_barrier;
    dependency_info.imageMemoryBarrierCount = 0;
    dependency_info.pImageMemoryBarriers = NULL;

    //TODO: remove this and add a flush command and flush at frame end
    vkCmdPipelineBarrier2(command_buffer->handle, &dependency_info);

    return true;
}


bool vulkan_command_buffer_allocate(Vulkan_Command_Buffer* out_command_buffer,
                                    Vulkan_Command_Buffer_Level cb_level,
                                    VkCommandPool pool, Renderer* renderer)
{
    //SPEC:
    //vkAllocateCommandBuffers can be used to allocate multiple command buffers.
    //If the allocation of any of those command buffers fails,
    //the implementation must free all successfully allocated command buffer objects from this command,
    //set all entries of the pCommandBuffers array to NULL and return the error.


    memset(out_command_buffer, 0, sizeof(out_command_buffer));

    VkCommandBufferAllocateInfo allocate_info = {0};
    allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocate_info.commandPool = pool;

    switch (cb_level)
    {
    case VULKAN_COMMAND_BUFFER_LEVEL_PRIMARY:
        allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        break;
    case VULKAN_COMMAND_BUFFER_LEVEL_SECONDARY:
        allocate_info.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
        break;
    }
    allocate_info.commandBufferCount = 1;
    allocate_info.pNext = 0;

    VkResult cmd_buffer_alloc_result = vkAllocateCommandBuffers(renderer->logical_device, &allocate_info,
                                                                &out_command_buffer->handle);
    // VK_CHECK(cmd_buffer_alloc_result)
    if (cmd_buffer_alloc_result != VK_SUCCESS)
    {
        return false;
    }

    return true;
}

void vulkan_command_buffer_free(Renderer* renderer, Vulkan_Command_Buffer* command_buffer,
                                VkCommandPool pool)
{
    vkFreeCommandBuffers(renderer->logical_device, pool, 1, &command_buffer->handle);
    command_buffer->handle = 0;
}

void vulkan_command_buffer_reset(Vulkan_Command_Buffer* command_buffer)
{
    //Flag: VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT -> returns memory back to the command pool, which we probably dont want
    vkResetCommandBuffer(command_buffer->handle, 0);
    command_buffer->state = VULKAN_COMMAND_BUFFER_STATE_USABLE;
}

void vulkan_command_buffer_begin(Vulkan_Command_Buffer* command_buffer)
{
    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = 0;
    begin_info.pInheritanceInfo = NULL; //used if its a secondary command buffer
    /*
    VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT: The command buffer will be rerecorded right after executing it once.
    VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT: This is a secondary command buffer that will be entirely within a single render pass.
    VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT: The command buffer can be resubmitted while it is also already pending execution.
    */
    /*if (is_single_use)
    {
        begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    }
    if (is_renderpass_continue)
    {
        begin_info.flags |= VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
    }
    if (is_simultaneous_use)
    {
        // not likley to ever be used
        begin_info.flags |= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    }*/
    VK_CHECK(vkBeginCommandBuffer(command_buffer->handle, &begin_info));
    command_buffer->state = VULKAN_COMMAND_BUFFER_STATE_BEGIN;
}


void vulkan_command_buffer_end(Vulkan_Command_Buffer* command_buffer)
{
    VK_CHECK(vkEndCommandBuffer(command_buffer->handle));
    command_buffer->state = VULKAN_COMMAND_BUFFER_STATE_END;
}


void vulkan_command_buffer_begin_single_use(Renderer* renderer,
                                            VkCommandPool pool, Vulkan_Command_Buffer* out_command_buffer)
{
    vulkan_command_buffer_allocate(out_command_buffer, VULKAN_COMMAND_BUFFER_LEVEL_PRIMARY, pool, renderer);
    vulkan_command_buffer_begin(out_command_buffer);
}


void vulkan_command_buffer_end_single_use(Renderer* renderer,
                                          VkCommandPool pool, Vulkan_Command_Buffer* command_buffer,
                                          VkQueue queue)
{
    vulkan_command_buffer_end(command_buffer);

    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer->handle;
    //TODO:
    // submit_info.signalSemaphoreCount = 1;
    // submit_info.pSignalSemaphores = NULL;
    // submit_info.waitSemaphoreCount = 0;
    // submit_info.pWaitSemaphores = NULL;
    // submit_info.pWaitDstStageMask = 0;

    VK_CHECK(vkQueueSubmit(queue, 1, &submit_info, 0));
    VK_CHECK(vkQueueWaitIdle(queue));
    vulkan_command_buffer_free(renderer, command_buffer, pool);
}


void vulkan_command_buffer_submit_binary_semaphore(Renderer* renderer,
                                                   Vulkan_Command_Buffer* command_buffer,
                                                   VkQueue queue,
                                                   VkSemaphoreSubmitInfo* wait_semaphore,
                                                   VkSemaphoreSubmitInfo* signal_semaphore)
{
    VkCommandBufferSubmitInfo command_buffer_submit_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
        .pNext = NULL,
        .commandBuffer = command_buffer->handle,
        .deviceMask = 1, // should be one for some reason, because i am not using device groups
    };

    // Submit the command buffer to the queue to finish the copy
    VkSubmitInfo2 submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
        .pNext = 0,
        .flags = 0,
        .commandBufferInfoCount = 1,
        .pCommandBufferInfos = &command_buffer_submit_info,
    };
    if (wait_semaphore)
    {
        submitInfo.waitSemaphoreInfoCount = 1;
        submitInfo.pWaitSemaphoreInfos = wait_semaphore;
    }
    if (signal_semaphore)
    {
        submitInfo.signalSemaphoreInfoCount = 1;
        submitInfo.pSignalSemaphoreInfos = signal_semaphore;
    }


    // Create fence to ensure that the command buffer has finished executing
    VkFenceCreateInfo fenceCI = {0};
    fenceCI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    VkFence fence;
    VK_CHECK(vkCreateFence(renderer->logical_device, &fenceCI, renderer->vk_allocator_callback, &fence));

    // Submit copies to the queue
    vkQueueSubmit2(queue, 1, &submitInfo, fence);

    // Wait for the fence to signal that command buffer has finished executing
    VK_CHECK(vkWaitForFences(renderer->logical_device, 1, &fence, VK_TRUE, UINT64_MAX));
    vkDestroyFence(renderer->logical_device, fence, 0);
}

VkCommandBufferSubmitInfo vulkan_command_buffer_get_submit_info(Vulkan_Command_Buffer* command_buffer)
{
    return (VkCommandBufferSubmitInfo){
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
        .commandBuffer = command_buffer->handle,
        .deviceMask = 0,
        .pNext = NULL,
    };
}


void vulkan_command_buffer_debug_label_begin(Renderer* renderer, Vulkan_Command_Buffer* command_buffer,
                                             const char* name)
{
    VkDebugUtilsLabelEXT debug_label = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT,
        .pNext = NULL,
        .pLabelName = name,
        .color = {1.0, 0.0, 0.0, 1.0}
    };
    renderer->debug_label_start(command_buffer->handle, &debug_label);
}

void vulkan_command_buffer_debug_label_color_begin(Renderer* renderer, Vulkan_Command_Buffer* command_buffer,
                                             const char* name, float color[4])
{
    VkDebugUtilsLabelEXT debug_label = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT,
        .pNext = NULL,
        .pLabelName = name,
    };
    memcpy(debug_label.color, color, sizeof(debug_label.color));
    renderer->debug_label_start(command_buffer->handle, &debug_label);
}

void vulkan_command_buffer_debug_label_end(Renderer* renderer, Vulkan_Command_Buffer* command_buffer)
{
    renderer->debug_label_end(command_buffer->handle);
}


/*
void command_pool_allocate(vulkan_context* vulkan_context, Command_Buffer_Context* command_buffer_context)
{
    QueueFamilyIndices queue_families_indices = find_queue_families(vulkan_context->surface,
                                                                    vulkan_context->physical_device);

    // FLAGS
    // VK_COMMAND_POOL_CREATE_TRANSIENT_BIT: Hint that command buffers
    // are rerecorded with new commands very often (may change memory allocation behavior)
    // VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT: Allow command buffers to be rerecorded invidiually,
    // without this flag they all have to be reset together
    //

    VkCommandPoolCreateInfo pool_create_info{};
    pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    pool_create_info.queueFamilyIndex = queue_families_indices.graphicsFamily.value();

    VK_CHECK(vkCreateCommandPool(vulkan_context->logical_device, &pool_create_info, NULL,
        &command_buffer_context->command_pool))
    INFO("CREATED COMMANDPOOL SUCCESS\n");
}

void command_pool_free(vulkan_context* vulkan_context, Command_Buffer_Context* command_buffer_context)
{
    vkDestroyCommandPool(vulkan_context->logical_device, command_buffer_context->command_pool, NULL);
}

void command_buffer_allocate(vulkan_context* vulkan_context, Command_Buffer_Context* command_buffer_context,
                             uint32_t frames_in_flight)
{
    command_buffer_context->command_buffer.resize(MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo buffer_allocate_info{};
    buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    buffer_allocate_info.commandBufferCount = static_cast<uint32_t>(command_buffer_context->command_buffer.size());
    //VK_COMMAND_BUFFER_LEVEL_SECONDARY - idk what having a secondary level means
    buffer_allocate_info.commandPool = command_buffer_context.command_pool;
    buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;


    VK_CHECK(vkAllocateCommandBuffers(vulkan_context->logical_device, &buffer_allocate_info,
        command_buffer_context->command_buffer.data()));

    INFO("CREATED COMMANDBUFFER SUCCESS\n");
}

void command_buffer_free(vulkan_context* vulkan_context, Command_Buffer_Context* command_buffer_context,
                         uint32_t frames_in_flight)
{
    //TODO: might want to do frames in flight instead
    auto command_buffer_size = command_buffer_context->command_buffer.size();
    for (uint32_t i = 0; i < command_buffer_size; i++)
    {
        vkFreeCommandBuffers(vulkan_context->logical_device, command_buffer_context->command_pool, 1,
                             &command_buffer_context->command_buffer[i]);
    }
}


VkCommandBuffer command_buffer_begin_single_use(vulkan_context* vulkan_context, VkCommandPool* command_pool)
{
    //create and allocate a command buffer
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; //TODO: make param
    allocInfo.commandPool = command_pool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(vulkan_context.logical_device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT; //TODO: make param

    //copy command buffer over
    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void command_buffer_end_single_use(vulkan_context* vulkan_context, VkCommandPool* command_pool,
                                   VkCommandBuffer commandBuffer)
{
    //submit command buffer to queue
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(vulkan_context.graphics_queue, 1, &submitInfo, VK_NULL_HANDLE);
    //wait for process to finish
    vkQueueWaitIdle(vulkan_context.graphics_queue);

    vkFreeCommandBuffers(vulkan_context.logical_device, command_pool, 1, &commandBuffer);
}
*/
