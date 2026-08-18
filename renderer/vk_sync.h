#ifndef VK_SYNC_H
#define VK_SYNC_H


//fence
void vulkan_fence_create(Renderer* renderer, VkFence* fence);
void vulkan_fence_destroy(Renderer* renderer, VkFence* fence, VkFenceCreateFlags fenceCreateFlags);
bool vulkan_fence_wait(Vulkan_Context* context, VkFence* fence, u64 timeout_ns);
bool vulkan_fence_reset(Renderer* renderer, VkFence* fence);

//semaphores
//binary semaphore
void binary_semaphore_create(Renderer* renderer, VkSemaphore* semaphore);
void binary_semaphore_destroy(Renderer* renderer, VkSemaphore* semaphore);

//timeline semaphore
void timeline_semaphore_create(Renderer* renderer, VkSemaphore* timeline_semaphore);
void timeline_semaphore_destroy(Renderer* renderer, VkSemaphore* timeline_semaphore);
void timeline_semaphore_query(Renderer* renderer, VkSemaphore* timeline_semaphore, u64* out_counter_value);
bool timeline_semaphore_query_and_compare(const Renderer* renderer, const VkSemaphore* timeline_semaphore,
                                          const u64 compare_value);

//
void sync_object_per_frame_init(Renderer* renderer, Vulkan_Context* context);

//sketching out queue transfer stuff

/*void queue_ownership_transfer_texture(Renderer* renderer, Vulkan_Queue_Type from_queue, Vulkan_Command_Buffer* from_buffer,
                              Vulkan_Queue_Type to, Vulkan_Command_Buffer* to_buffer);
void queue_ownership_transfer_mesh(Renderer* renderer, Vulkan_Queue_Type from_queue, Vulkan_Command_Buffer* from_buffer,
                              Vulkan_Queue_Type to, Vulkan_Command_Buffer* to_buffer);*/




/*
void buffer_memory_barrier_ownership_transfer_create(Renderer* renderer, Queue_Type from, Queue_Type to,
                                                     Buffer_Handle buffer_handle)
{
    //batch the commands up and then eventally pass to VkDependencyInfo
    u32 from_queue_index = -1;
    u32 to_queue_index = -1;
    Vulkan_Buffer* buffer = vulkan_buffer_get(renderer, buffer_handle);

    switch (from)
    {
    case QUEUE_TYPE_GRAPHICS:
        from_queue_index = renderer->context.device.graphics_queue_index;
        break;
    case QUEUE_TYPE_COMPUTE:
        from_queue_index = renderer->context.device.compute_queue_index;
        break;
    case QUEUE_TYPE_TRANSFER:
        from_queue_index = renderer->context.device.transfer_queue_index;
        break;
    case QUEUE_TYPE_PRESEMT:
        MASSERT(false);
        break;
    }

    switch (to)
    {
    case QUEUE_TYPE_GRAPHICS:
        to_queue_index = renderer->context.device.graphics_queue_index;
        break;
    case QUEUE_TYPE_COMPUTE:
        to_queue_index = renderer->context.device.compute_queue_index;
        break;
    case QUEUE_TYPE_TRANSFER:
        to_queue_index = renderer->context.device.transfer_queue_index;
        break;
    case QUEUE_TYPE_PRESEMT:
        MASSERT(false);
        break;
    }

    MASSERT(from_queue_index != -1);
    MASSERT(to_queue_index != -1);

    // Pipeline barrier to start a queue ownership transfer after the copy
    //NOTE: a tranfer operation has a release and an acquire memory operation
    VkBufferMemoryBarrier2 release_buffer_memory_barrier = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
        .srcStageMask = VK_PIPELINE_STAGE_2_ALL_TRANSFER_BIT,
        .srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT,
        .srcQueueFamilyIndex = from_queue_index,
        .dstQueueFamilyIndex = to_queue_index,
        .buffer = buffer->handle,
        .offset = 0,
        .size = VK_WHOLE_SIZE,
        //NOTE: spec says these do not need to be used here for a transfer operation
        .dstAccessMask = 0,
        .dstStageMask = 0,
    };


    renderer->buffer_memory_barrier_batch_release[renderer->buffer_memory_barrier_batch_release_count++] =
        release_buffer_memory_barrier;

    //this barrier
    // Pipeline barrier before using the vertex buffer, after finalizing the ownership transfer
    VkBufferMemoryBarrier2 acquire_buffer_memory_barrier = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
        .dstStageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT | VK_PIPELINE_STAGE_2_ALL_TRANSFER_BIT,
        .dstAccessMask = VK_ACCESS_2_MEMORY_READ_BIT,
        .srcQueueFamilyIndex = from_queue_index,
        .dstQueueFamilyIndex = to_queue_index,
        .buffer = buffer->handle,
        .offset = 0,
        .size = VK_WHOLE_SIZE,
        //NOTE: spec says these are not neededvb
        .srcStageMask = 0,
        .srcAccessMask = 0,
        // .pNext = ,
    };


    renderer->buffer_memory_barrier_batch_acquire[renderer->buffer_memory_barrier_batch_acquire_count++] =
        acquire_buffer_memory_barrier;
}


void tranfer_graphics_memory_barrier_release(Renderer* renderer, vulkan_command_buffer* transfer_cmd_buffer)
{
    VkDependencyInfo dependencyInfo = {
        .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO_KHR,
        .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
        .bufferMemoryBarrierCount = renderer->buffer_memory_barrier_batch_release_count,
        .pBufferMemoryBarriers = renderer->buffer_memory_barrier_batch_release,
        //TODO:
        // .memoryBarrierCount = 0,
        // .pMemoryBarriers = 0,
        // .imageMemoryBarrierCount = ,
        // .pImageMemoryBarriers =
        // .pNext = 0,

    };

    vkCmdPipelineBarrier2(transfer_cmd_buffer->handle, &dependencyInfo);

    //reset the count
    renderer->buffer_memory_barrier_batch_release_count = 0;
}


void tranfer_graphics_memory_barrier_acquire(Renderer* renderer, vulkan_command_buffer* command_buffer)
{
    VkDependencyInfoKHR dependencyInfo = {
        .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO_KHR,
        .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
        .bufferMemoryBarrierCount = renderer->buffer_memory_barrier_batch_acquire_count,
        .pBufferMemoryBarriers = renderer->buffer_memory_barrier_batch_acquire,
        // .pNext = ,
        // .memoryBarrierCount = ,
        // .pMemoryBarriers = ,
        // .imageMemoryBarrierCount = ,
        // .pImageMemoryBarriers =
    };

    vkCmdPipelineBarrier2(command_buffer->handle, &dependencyInfo);

    //reset the count
    renderer->buffer_memory_barrier_batch_acquire_count = 0;
}

*/


void transfer_barrier_catch_all(Renderer* renderer, Vulkan_Command_Buffer* command_buffer)
{
    // If there is a semaphore signal + wait between this being submitted and
    // the vertex buffer being used, then skip this pipeline barrier.

    // Pipeline barrier before using the vertex data
    // Note that this can apply to all buffers uploaded in the same way, so
    // ideally batch all copies before this.
    VkMemoryBarrier2 memory_barrier = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2,
        .srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
        .srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
        .dstAccessMask = VK_ACCESS_2_MEMORY_READ_BIT,
    };

    VkDependencyInfo dependencyInfo = {
        .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        // .pNext = ,
        .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
        .memoryBarrierCount = 1,
        .pMemoryBarriers = &memory_barrier,
        // .bufferMemoryBarrierCount = 1,
        // .pBufferMemoryBarriers = &memory_barrier,
        // .imageMemoryBarrierCount = ,
        // .pImageMemoryBarriers =
    };

    vkCmdPipelineBarrier2(command_buffer->handle, &dependencyInfo);
}


#endif
