#include "vk_buffer.h"


Buffer_System* buffer_system_init(renderer* renderer)
{
    Buffer_System* buffer_system = arena_alloc(&renderer->arena, sizeof(Buffer_System));

    buffer_system->global_uniform_buffer = arena_alloc(&renderer->arena,
                                                       sizeof(vulkan_buffer_cpu) * renderer->context.swapchain.
                                                       max_frames_in_flight);
    buffer_system->global_uniform_buffer_size = renderer->context.swapchain.max_frames_in_flight;
    for (int i = 0; i < buffer_system->global_uniform_buffer_size; i++)
    {
        vulkan_buffer_gpu_create(renderer, &buffer_system->global_uniform_buffer[i], GPU_UNIFORM,
                                 sizeof(uniform_buffer_object));
    }

    buffer_system->vertex_buffer_count = 1;
    buffer_system->index_buffer_count = 1;
    buffer_system->indirect_buffer_count = 1;
    buffer_system->storage_buffer_count = 1;
    buffer_system->uniform_buffer_count = 1;
    buffer_system->tangent_buffer_count = 1;
    buffer_system->normal_buffer_count = 1;

    buffer_system->vertex_buffers = arena_alloc(&renderer->arena, sizeof(vulkan_buffer_cpu));
    buffer_system->index_buffers = arena_alloc(&renderer->arena, sizeof(vulkan_buffer_cpu));
    buffer_system->indirect_buffer = arena_alloc(&renderer->arena, sizeof(vulkan_buffer_cpu));
    buffer_system->uv_buffers = arena_alloc(&renderer->arena, sizeof(vulkan_buffer_cpu));
    buffer_system->normal_buffers = arena_alloc(&renderer->arena, sizeof(vulkan_buffer_cpu));
    buffer_system->tangent_buffers = arena_alloc(&renderer->arena, sizeof(vulkan_buffer_cpu));
    buffer_system->storage_buffers = arena_alloc(&renderer->arena, sizeof(vulkan_buffer_cpu));
    buffer_system->uniform_buffers = arena_alloc(&renderer->arena, sizeof(vulkan_buffer_cpu));


    //TODO: pass in the buffer_system
    vulkan_buffer_cpu_create(renderer, buffer_system->vertex_buffers, CPU_VERTEX, MB(32));
    vulkan_buffer_cpu_create(renderer, buffer_system->uv_buffers, CPU_STORAGE, MB(32));
    vulkan_buffer_cpu_create(renderer, buffer_system->storage_buffers, CPU_STORAGE, MB(32));
    vulkan_buffer_cpu_create(renderer, buffer_system->index_buffers, CPU_INDEX, MB(32));
    vulkan_buffer_cpu_create(renderer, buffer_system->normal_buffers, CPU_STORAGE, MB(32));
    vulkan_buffer_cpu_create(renderer, buffer_system->tangent_buffers, CPU_STORAGE, MB(32));

    //NOTE: uniform buffer, can simply be the size of the uniform buffer struct with a count of how many of them we want
    vulkan_buffer_gpu_create(renderer, buffer_system->uniform_buffers, GPU_UNIFORM, MB(32));

    //STAGING BUFFERS
    buffer_system->staging_buffer_ring = arena_alloc(&renderer->arena, sizeof(vulkan_buffer_gpu));
    vulkan_buffer_gpu_create(renderer, buffer_system->staging_buffer_ring, GPU_STAGING, MB(32));
    buffer_system->staging_buffer_count = 1;


    return buffer_system;
}

uint32_t find_memory_type(vulkan_context* context, uint32_t type_filter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(context->device.physical_device, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((type_filter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    FATAL("failed to find suitable memory type");
    return -1;
}


bool buffer_create(vulkan_context* vulkan_context, VkDeviceSize size, VkBufferUsageFlags usage,
                   VkMemoryPropertyFlags properties, VkBuffer* buffer, VkDeviceMemory* bufferMemory)
{
    //create buffer
    VkBufferCreateInfo buffer_create_info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = size,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE
    };
    //The flags parameter is used to configure sparse buffer memory, which is not relevant right now. We'll leave it at the default value of 0.
    //bufferInfo.flags;

    VK_CHECK(
        vkCreateBuffer(vulkan_context->device.logical_device, &buffer_create_info, vulkan_context->allocator, buffer));

    //finding memory size needed for the buffer
    VkMemoryRequirements memory_requirements;
    vkGetBufferMemoryRequirements(vulkan_context->device.logical_device, *buffer, &memory_requirements);

    //  NOTE:
    //  It should be noted that in a real world application,
    //  you're not supposed to actually call vkAllocateMemory for every individual buffer.
    //  The maximum number of simultaneous memory allocations is limited by the maxMemoryAllocationCount physical device limit,
    //  which may be as low as 4096 even on high end hardware like an NVIDIA GTX 1080.
    //  The right way to allocate memory for a large number of objects at the same time
    //  is to create a custom allocator that splits up a single allocation among many different objects by using the offset parameters that we've seen in many functions.
    // You can either implement such an allocator yourself,
    // or use the VulkanMemoryAllocator library provided by the GPUOpen initiative.
    // However, for this tutorial it's okay to use a separate allocation for every resource,
    // because we won't come close to hitting any of these limits for now.


    //allocate buffer and bind to memory
    VkMemoryAllocateInfo memory_allocate_info = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memory_requirements.size,
        .memoryTypeIndex = find_memory_type(vulkan_context, memory_requirements.memoryTypeBits,
                                            properties),
    };
    VK_CHECK(vkAllocateMemory(vulkan_context->device.logical_device, &memory_allocate_info, NULL, bufferMemory));

    VK_CHECK(vkBindBufferMemory(vulkan_context->device.logical_device, *buffer, *bufferMemory, 0));

    return true;
}

bool vulkan_buffer_cpu_free(renderer* renderer, vulkan_buffer_cpu* vk_buffer)
{
    vkFreeMemory(renderer->context.device.logical_device, vk_buffer->memory, NULL);
    vkDestroyBuffer(renderer->context.device.logical_device, vk_buffer->handle, NULL);

    return true;
}


void buffer_copy(vulkan_context* vulkan_context,
                 VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
    // Create a temporary command buffer for the copy operation
    VkCommandBufferAllocateInfo command_buffer_allocation_info = {0};
    command_buffer_allocation_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    command_buffer_allocation_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    command_buffer_allocation_info.commandPool = vulkan_context->graphics_command_pool; //TODO: likely to change
    command_buffer_allocation_info.commandBufferCount = 1;

    VkCommandBuffer temp_command_buffer;
    VK_CHECK(vkAllocateCommandBuffers(vulkan_context->device.logical_device, &command_buffer_allocation_info,
        &temp_command_buffer));

    // Begin recording the command buffer
    VkCommandBufferBeginInfo command_buffer_begin_info = {0};
    command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    VK_CHECK(vkBeginCommandBuffer(temp_command_buffer, &command_buffer_begin_info));

    // Set up the copy region with specified offsets and size
    VkBufferCopy copy_region = {0};
    copy_region.srcOffset = 0;
    copy_region.dstOffset = 0;
    copy_region.size = size;

    // Record the copy command
    vkCmdCopyBuffer(temp_command_buffer, srcBuffer, dstBuffer, 1, &copy_region);

    VK_CHECK(vkEndCommandBuffer(temp_command_buffer));

    // Submit the command buffer and wait for completion
    VkSubmitInfo submit_info = {0};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &temp_command_buffer;

    VK_CHECK(vkQueueSubmit(vulkan_context->device.graphics_queue, 1, &submit_info, VK_NULL_HANDLE));
    VK_CHECK(vkQueueWaitIdle(vulkan_context->device.graphics_queue));

    // Clean up the temporary command buffer
    vkFreeCommandBuffers(vulkan_context->device.logical_device, vulkan_context->graphics_command_pool, 1,
                         &temp_command_buffer);
}


void buffer_copy_region(vulkan_context* vulkan_context, vulkan_command_buffer* command_buffer_context,
                        VkBuffer srcBuffer,
                        VkBuffer dstBuffer, VkDeviceSize size, VkDeviceSize srcOffset, VkDeviceSize dstOffset)
{
    // Create a temporary command buffer for the copy operation
    VkCommandBufferAllocateInfo command_buffer_allocation_info = {0};
    command_buffer_allocation_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    command_buffer_allocation_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    command_buffer_allocation_info.commandPool = vulkan_context->graphics_command_pool; //TODO: likely to change
    command_buffer_allocation_info.commandBufferCount = 1;

    VkCommandBuffer temp_command_buffer;
    vkAllocateCommandBuffers(vulkan_context->device.logical_device, &command_buffer_allocation_info,
                             &temp_command_buffer);

    // Begin recording the command buffer
    VkCommandBufferBeginInfo command_buffer_begin_info = {0};
    command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(temp_command_buffer, &command_buffer_begin_info);

    // Set up the copy region with specified offsets and size
    VkBufferCopy copy_region = {0};
    copy_region.srcOffset = srcOffset;
    copy_region.dstOffset = dstOffset;
    copy_region.size = size;

    // Record the copy command
    vkCmdCopyBuffer(temp_command_buffer, srcBuffer, dstBuffer, 1, &copy_region);

    vkEndCommandBuffer(temp_command_buffer);

    // Submit the command buffer and wait for completion
    VkSubmitInfo submit_info = {0};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &temp_command_buffer;

    vkQueueSubmit(vulkan_context->device.graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(vulkan_context->device.graphics_queue);

    // Clean up the temporary command buffer
    vkFreeCommandBuffers(vulkan_context->device.logical_device, vulkan_context->graphics_command_pool, 1,
                         &temp_command_buffer);
}


void vulkan_buffer_cpu_create(renderer* renderer, vulkan_buffer_cpu* out_buffer,
                              vulkan_cpu_buffer_type buffer_type,
                              u64 data_size)
{

    MASSERT_MSG(out_buffer, "VULKAN BUFFER CPU CREATE: INVALID PASSED IN BUFFER");


    //NOTE: the vulkan buffer must be allocated before bieng passed in

    //do a large allocation upfront
    out_buffer->capacity = data_size;
    out_buffer->current_offset = 0;

    VkDevice device = renderer->context.device.logical_device;


    //for Buffer device addressing
    VkMemoryAllocateFlagsInfoKHR allocFlagsInfo = {0};
    allocFlagsInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO_KHR;
    allocFlagsInfo.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT_KHR;

    VkMemoryAllocateInfo memAlloc = {0};
    memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memAlloc.pNext = &allocFlagsInfo;

    VkMemoryRequirements memReqs;


    VkBufferCreateInfo out_buffer_create_info = {0};
    out_buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    out_buffer_create_info.size = out_buffer->capacity;
    out_buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // look into later

    // intended to be used as the destination of a copy from a staging buffer
    switch (buffer_type)
    {
    case CPU_VERTEX:
        out_buffer_create_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT |
            VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT_KHR;
        break;
    case CPU_INDEX:
        out_buffer_create_info.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        break;
    case CPU_STORAGE:
        out_buffer_create_info.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT |
            VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT_KHR;
        break;
    case CPU_INDIRECT:
        out_buffer_create_info.usage = VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT |
            VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT_KHR;
        break;
    }

    // Create a host-visible buffer to copy the vertex data to (staging buffer)
    VK_CHECK(vkCreateBuffer(device, &out_buffer_create_info, renderer->context.allocator, &out_buffer->handle));
    vkGetBufferMemoryRequirements(device, out_buffer->handle, &memReqs);
    memAlloc.allocationSize = memReqs.size;
    memAlloc.memoryTypeIndex = find_memory_type(&renderer->context, memReqs.memoryTypeBits,
                                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VK_CHECK(vkAllocateMemory(device, &memAlloc, renderer->context.allocator, &out_buffer->memory));
    VK_CHECK(vkBindBufferMemory(device, out_buffer->handle, out_buffer->memory, 0));

}

Buffer_Handle vulkan_buffer_cpu_create_handle(renderer* renderer, vulkan_cpu_buffer_type buffer_type, u64 data_size)
{
    //NOTE: the vulkan buffer must be allocated before bieng passed in

    vulkan_buffer_cpu* buffer_ref = NULL;

    switch (buffer_type)
    {
    case CPU_VERTEX:
        buffer_ref = &renderer->buffer_system->vertex_buffers[0];
        break;
    case CPU_INDEX:
        buffer_ref = &renderer->buffer_system->indirect_buffer[0];
        break;
    case CPU_STORAGE:
        buffer_ref = &renderer->buffer_system->storage_buffers[0];
        break;
    case CPU_INDIRECT:
        buffer_ref = &renderer->buffer_system->indirect_buffer[0];
        break;
    default:
        FATAL("INVALID BUFFER TYPE");
        break;
    }

    MASSERT(buffer_ref);

              //do a large allocation upfront
    buffer_ref->capacity = data_size;
    buffer_ref->current_offset = 0;

    VkDevice device = renderer->context.device.logical_device;


    //for Buffer device addressing
    VkMemoryAllocateFlagsInfoKHR allocFlagsInfo = {0};
    allocFlagsInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO_KHR;
    allocFlagsInfo.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT_KHR;

    VkMemoryAllocateInfo memAlloc = {0};
    memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memAlloc.pNext = &allocFlagsInfo;

    VkMemoryRequirements memReqs;


    VkBufferCreateInfo out_buffer_create_info = {0};
    out_buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    out_buffer_create_info.size = buffer_ref->capacity;
    out_buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // look into later

    // intended to be used as the destination of a copy from a staging buffer
    switch (buffer_type)
    {
    case CPU_VERTEX:
        out_buffer_create_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT |
            VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT_KHR;
        break;
    case CPU_INDEX:
        out_buffer_create_info.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        break;
    case CPU_STORAGE:
        out_buffer_create_info.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT |
            VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT_KHR;
        break;
    case CPU_INDIRECT:
        out_buffer_create_info.usage = VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT |
            VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT_KHR;
        break;
    }

    // Create a host-visible buffer to copy the vertex data to (staging buffer)
    VK_CHECK(vkCreateBuffer(device, &out_buffer_create_info, renderer->context.allocator, &buffer_ref->handle));
    vkGetBufferMemoryRequirements(device, buffer_ref->handle, &memReqs);
    memAlloc.allocationSize = memReqs.size;
    memAlloc.memoryTypeIndex = find_memory_type(&renderer->context, memReqs.memoryTypeBits,
                                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VK_CHECK(vkAllocateMemory(device, &memAlloc, renderer->context.allocator, &buffer_ref->memory));
    VK_CHECK(vkBindBufferMemory(device, buffer_ref->handle, buffer_ref->memory, 0));


    return (Buffer_Handle){-1};
}


void vulkan_buffer_cpu_data_copy_from_offset(renderer* renderer, vulkan_buffer_cpu* buffer,
                                             void* data, u64 data_size)
{
    VkDevice device = renderer->context.device.logical_device;
    vulkan_buffer_gpu* staging_buffer = renderer->buffer_system->staging_buffer_ring;

    // Debug 1: Check source data
    float* src = (float*)data;
    printf("Source data before memcpy: %f, %f\n", src[0], src[1]);

    //copy data into the staging buffer
    memcpy(staging_buffer->mapped_data, data, data_size);

    // Debug 2: Check staging buffer after memcpy
    float* staging = (float*)staging_buffer->mapped_data;
    printf("Staging buffer after memcpy: %f, %f\n", staging[0], staging[1]);

    // Flush the mapped memory to make it visible to the GPU
    VkMappedMemoryRange range = {0};
    range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    range.memory = staging_buffer->memory;
    range.offset = 0;
    range.size = VK_WHOLE_SIZE;
    vkFlushMappedMemoryRanges(device, 1, &range);


    //copy staging buffer data (host visible) into the buffer that for the GPU (device local)

    // Buffer copies have to be submitted to a queue, so we need a command buffer for them
    VkCommandBuffer copyCmd;

    VkCommandBufferAllocateInfo cmdBufAllocateInfo = {0};
    cmdBufAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdBufAllocateInfo.commandPool = renderer->context.graphics_command_pool;
    cmdBufAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmdBufAllocateInfo.commandBufferCount = 1;
    VK_CHECK(vkAllocateCommandBuffers(device, &cmdBufAllocateInfo, &copyCmd));

    VkCommandBufferBeginInfo cmdBufInfo = {0};
    cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    VK_CHECK(vkBeginCommandBuffer(copyCmd, &cmdBufInfo));
    // Copy vertex and index buffers to the device
    VkBufferCopy copyRegion = {0};
    copyRegion.size = data_size;
    copyRegion.srcOffset = 0; // this will always be 0 for now
    copyRegion.dstOffset = buffer->current_offset; // start at the offset of the buffer
    vkCmdCopyBuffer(copyCmd, staging_buffer->handle, buffer->handle, 1, &copyRegion);
    VK_CHECK(vkEndCommandBuffer(copyCmd));

    // Submit the command buffer to the queue to finish the copy
    VkSubmitInfo submitInfo = {0};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &copyCmd;

    // Create fence to ensure that the command buffer has finished executing
    VkFenceCreateInfo fenceCI = {0};
    fenceCI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    VkFence fence;
    VK_CHECK(vkCreateFence(device, &fenceCI, renderer->context.allocator, &fence));
    // Submit copies to the queue
    VK_CHECK(vkQueueSubmit(renderer->context.device.graphics_queue, 1, &submitInfo, fence));
    // Wait for the fence to signal that command buffer has finished executing
    VK_CHECK(vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX));
    vkDestroyFence(device, fence, 0);
    vkFreeCommandBuffers(device, renderer->context.graphics_command_pool, 1, &copyCmd);


    //update the offset when the copy is done
    buffer->current_offset += data_size;
}


void vulkan_buffer_gpu_create(renderer* renderer, vulkan_buffer_gpu* out_buffer,
                              vulkan_gpu_buffer_type buffer_type,
                              u64 data_size)
{
    //NOTE: the vulkan buffer must be allocated before being passed in

    out_buffer->capacity = data_size;
    VkDevice device = renderer->context.device.logical_device;


    // Setup indices
    // We do this for demonstration purposes, a triangle doesn't require indices to be rendered (because of the 1:1 mapping), but more complex shapes usually make use of indices

    //for Buffer devide addressing
    //for Buffer device addressing

    VkMemoryAllocateFlagsInfoKHR allocFlagsInfo = {0};
    allocFlagsInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO_KHR;
    allocFlagsInfo.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT_KHR;

    VkMemoryAllocateInfo memAlloc = {0};
    memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    switch (buffer_type)
    {
    case GPU_UNIFORM:
        memAlloc.pNext = &allocFlagsInfo;
        break;
    case GPU_STORAGE:
        memAlloc.pNext = &allocFlagsInfo;
        break;
    case GPU_STAGING:
        memAlloc.pNext = NULL;
        break;
    }

    VkMemoryRequirements memReqs;

    // Create the host visible staging buffer that we copy vertices and indices too, and from which we copy to the device
    VkBufferCreateInfo out_buffer_create_info = {0};
    out_buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    out_buffer_create_info.size = data_size;
    // Buffer is used as the copy source
    switch (buffer_type)
    {
    case GPU_UNIFORM:
        out_buffer_create_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT |
            VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT_KHR;
        break;
    case GPU_STORAGE:
        out_buffer_create_info.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
            VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT_KHR;
        break;
    case GPU_STAGING:
        out_buffer_create_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        break;
    }
    // Create a host-visible buffer to copy the vertex data to (staging buffer)
    VK_CHECK(vkCreateBuffer(device, &out_buffer_create_info, 0, &out_buffer->handle));
    vkGetBufferMemoryRequirements(device, out_buffer->handle, &memReqs);
    memAlloc.allocationSize = memReqs.size;
    // Request a host visible memory type that can be used to copy our data to
    // Also request it to be coherent, so that writes are visible to the GPU right after unmapping the buffer
    // VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT for buffer device addressing
    memAlloc.memoryTypeIndex = find_memory_type(&renderer->context, memReqs.memoryTypeBits,
                                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    VK_CHECK(vkAllocateMemory(device, &memAlloc, renderer->context.allocator, & out_buffer->memory));
    VK_CHECK(vkBindBufferMemory(device, out_buffer->handle, out_buffer->memory, 0));

    // Map the buffer and copy vertices
    VK_CHECK(vkMapMemory(device, out_buffer->memory, 0, memAlloc.allocationSize, 0, (void**)& out_buffer->mapped_data));
}


void vulkan_buffer_gpu_free(renderer* renderer, vulkan_buffer_gpu* staging_buffer)
{
    // The fence made sure copies are finished, so we can safely delete the staging buffer
    vkDestroyBuffer(renderer->context.device.logical_device, staging_buffer->handle, renderer->context.allocator);
    vkFreeMemory(renderer->context.device.logical_device, staging_buffer->memory, renderer->context.allocator);
}


vulkan_buffer_cpu* vulkan_buffer_cpu_get(renderer* renderer, Buffer_Handle buffer_handle)
{
    return &renderer->buffer_system->vertex_buffers[buffer_handle.handle];
}

vulkan_buffer_cpu* vulkan_buffer_cpu_reset_offset(renderer* renderer, vulkan_buffer_cpu* buffer)
{
    buffer->current_offset = 0;
}
