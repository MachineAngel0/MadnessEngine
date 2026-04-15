#ifndef VK_FENCES_H
#define VK_FENCES_H


void sync_object_per_frame_init(Renderer* renderer, vulkan_context* context);

bool vulkan_fence_wait(vulkan_context* context, VkFence* fence, u64 timeout_ns);

void create_semaphore(Renderer* renderer);

void destroy_sempahore(Renderer* renderer, VkSemaphore* semaphore);


void insert_memory_barrier2_test_function_do_not_use_yet(VkCommandBuffer cmdbuffer, VkImage image,
                                                         VkAccessFlags srcAccessMask,
                                                         VkAccessFlags dstAccessMask, VkImageLayout oldImageLayout,
                                                         VkImageLayout newImageLayout,
                                                         VkPipelineStageFlags srcStageMask,
                                                         VkPipelineStageFlags dstStageMask,
                                                         VkImageSubresourceRange subresourceRange)
{
    //FUTURE:
    //NOTE: an example
    VkMemoryBarrier2KHR memoryBarrier = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2_KHR,
        .srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        .srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
        .dstAccessMask = VK_ACCESS_2_INPUT_ATTACHMENT_READ_BIT
    };
    VkDependencyInfoKHR dependencyInfo = {
        .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO_KHR,
        .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
        .memoryBarrierCount = 1,
        .pMemoryBarriers = &memoryBarrier
    };
    vkCmdPipelineBarrier2(cmdbuffer, &dependencyInfo);
}

typedef enum Queue_Type
{
    QUEUE_TYPE_GRAPHICS,
    QUEUE_TYPE_COMPUTE,
    QUEUE_TYPE_TRANSFER,
    QUEUE_TYPE_PRESEMT,
} Queue_Type;


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


void buffer_memory_barrier(Renderer* renderer,  vulkan_command_buffer* command_buffer)
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
        .dstStageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT |VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
        .dstAccessMask = VK_ACCESS_2_MEMORY_READ_BIT
    };

    VkDependencyInfo dependencyInfo = {
        .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        // .pNext = ,
        // .dependencyFlags = ,
        // .memoryBarrierCount = ,
        // .pMemoryBarriers = ,
        .bufferMemoryBarrierCount = 1,
        .pBufferMemoryBarriers = &memory_barrier,
        // .imageMemoryBarrierCount = ,
        // .pImageMemoryBarriers =
    };

    vkCmdPipelineBarrier2(command_buffer->handle, &dependencyInfo);

}


#endif
