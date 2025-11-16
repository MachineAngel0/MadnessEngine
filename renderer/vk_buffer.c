#include "vk_buffer.h"
#include "logger.h"


uint32_t find_memory_type(vulkan_context* vulkan_context, uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(vulkan_context->device.physical_device, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    FATAL("failed to find suitable memory type");
    return -1;
}


bool vulkan_buffer_create(vulkan_context* context, u64 size, VkBufferUsageFlagBits usage,
                        u32 memory_property_flags, b8 bind_on_create,
                        vulkan_buffer* out_buffer)
{
    memset(out_buffer, 0, sizeof(vulkan_buffer));
    out_buffer->total_size = size;
    out_buffer->usage = usage;
    out_buffer->memory_property_flags = memory_property_flags;

    VkBufferCreateInfo buffer_info = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    buffer_info.size = size;
    buffer_info.usage = usage;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // NOTE: Only used in one queue.

    VK_CHECK(vkCreateBuffer(context->device.logical_device, &buffer_info, context->allocator, &out_buffer->handle));

    // Gather memory requirements.
    VkMemoryRequirements requirements;
    vkGetBufferMemoryRequirements(context->device.logical_device, out_buffer->handle, &requirements);
    out_buffer->memory_index =
            find_memory_type(context, requirements.memoryTypeBits, out_buffer->memory_property_flags);
    if (out_buffer->memory_index == -1)
    {
        M_ERROR("Unable to create vulkan buffer because the required memory type index was not found.");
        return false;
    }

    // Allocate memory info
    VkMemoryAllocateInfo allocate_info = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    allocate_info.allocationSize = requirements.size;
    allocate_info.memoryTypeIndex = (u32) out_buffer->memory_index;

    // Allocate the memory.
    VkResult result = vkAllocateMemory(
        context->device.logical_device,
        &allocate_info,
        context->allocator,
        &out_buffer->memory);

    if (result != VK_SUCCESS)
    {
        M_ERROR("Unable to create vulkan buffer because the required memory allocation failed. Error: %i", result);
        return false;
    }

    if (bind_on_create)
    {
        vulkan_buffer_bind(context, out_buffer, 0);
    }

    return true;
}

void vulkan_buffer_destroy(vulkan_context* context, vulkan_buffer* buffer)
{
    if (buffer->memory)
    {
        vkFreeMemory(context->device.logical_device, buffer->memory, context->allocator);
        buffer->memory = 0;
    }
    if (buffer->handle)
    {
        vkDestroyBuffer(context->device.logical_device, buffer->handle, context->allocator);
        buffer->handle = 0;
    }
    buffer->total_size = 0;
    buffer->usage = 0;
    buffer->is_locked = false;
}

bool vulkan_buffer_resize(vulkan_context* context, u64 new_size, vulkan_buffer* buffer,
                        VkQueue queue, VkCommandPool pool)
{
    // Create new buffer.
    VkBufferCreateInfo buffer_info = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    buffer_info.size = new_size;
    buffer_info.usage = buffer->usage;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // NOTE: Only used in one queue.

    VkBuffer new_buffer;
    VK_CHECK(vkCreateBuffer(context->device.logical_device, &buffer_info, context->allocator, &new_buffer));

    // Gather memory requirements.
    VkMemoryRequirements requirements;
    vkGetBufferMemoryRequirements(context->device.logical_device, new_buffer, &requirements);

    // Allocate memory info
    VkMemoryAllocateInfo allocate_info = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    allocate_info.allocationSize = requirements.size;
    allocate_info.memoryTypeIndex = (u32) buffer->memory_index;

    // Allocate the memory.
    VkDeviceMemory new_memory;
    VkResult result = vkAllocateMemory(context->device.logical_device, &allocate_info, context->allocator, &new_memory);
    if (result != VK_SUCCESS)
    {
        M_ERROR("Unable to resize vulkan buffer because the required memory allocation failed. Error: %i", result);
        return false;
    }

    // Bind the new buffer's memory
    VK_CHECK(vkBindBufferMemory(context->device.logical_device, new_buffer, new_memory, 0));

    // Copy over the data
    vulkan_buffer_copy_to(context, pool, 0, queue, buffer->handle, 0, new_buffer, 0, buffer->total_size);

    // Make sure anything potentially using these is finished.
    vkDeviceWaitIdle(context->device.logical_device);

    // Destroy the old
    if (buffer->memory)
    {
        vkFreeMemory(context->device.logical_device, buffer->memory, context->allocator);
        buffer->memory = 0;
    }
    if (buffer->handle)
    {
        vkDestroyBuffer(context->device.logical_device, buffer->handle, context->allocator);
        buffer->handle = 0;
    }

    // Set new properties
    buffer->total_size = new_size;
    buffer->memory = new_memory;
    buffer->handle = new_buffer;

    return true;
}

void vulkan_buffer_bind(vulkan_context* context, vulkan_buffer* buffer, u64 offset)
{
    VK_CHECK(vkBindBufferMemory(context->device.logical_device, buffer->handle, buffer->memory, offset));
}

void* vulkan_buffer_lock_memory(vulkan_context* context, vulkan_buffer* buffer, u64 offset, u64 size, u32 flags)
{
    void* data;
    VK_CHECK(vkMapMemory(context->device.logical_device, buffer->memory, offset, size, flags, &data));
    return data;
}

void vulkan_buffer_unlock_memory(vulkan_context* context, vulkan_buffer* buffer)
{
    vkUnmapMemory(context->device.logical_device, buffer->memory);
}

void vulkan_buffer_load_data(vulkan_context* context, vulkan_buffer* buffer, u64 offset, u64 size, u32 flags,
                             const void* data)
{
    void* data_ptr;
    VK_CHECK(vkMapMemory(context->device.logical_device, buffer->memory, offset, size, flags, &data_ptr));
    memcpy(data_ptr, data, size);
    vkUnmapMemory(context->device.logical_device, buffer->memory);
}

void vulkan_buffer_copy_to(
    vulkan_context* context,
    VkCommandPool pool,
    VkFence fence,
    VkQueue queue,
    VkBuffer source,
    u64 source_offset,
    VkBuffer dest,
    u64 dest_offset,
    u64 size)
{
    vkQueueWaitIdle(queue);
    // Create a one-time-use command buffer.
    vulkan_command_buffer temp_command_buffer;
    vulkan_command_buffer_allocate_and_begin_single_use(context, pool, &temp_command_buffer);

    // Prepare the copy command and add it to the command buffer.
    VkBufferCopy copy_region;
    copy_region.srcOffset = source_offset;
    copy_region.dstOffset = dest_offset;
    copy_region.size = size;

    vkCmdCopyBuffer(temp_command_buffer.command_buffer_handle, source, dest, 1, &copy_region);

    // Submit the buffer for execution and wait for it to complete.
    vulkan_command_buffer_end_single_use(context, pool, &temp_command_buffer, queue);
}


/*
void buffer_create(vulkan_context* vulkan_context, VkDeviceSize size, VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties, VkBuffer* buffer, VkDeviceMemory* bufferMemory)
{
    //create buffer
    VkBufferCreateInfo buffer_create_info{};
    buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_create_info.size = size;
    buffer_create_info.usage = usage;
    buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    //The flags parameter is used to configure sparse buffer memory, which is not relevant right now. We'll leave it at the default value of 0.
    //bufferInfo.flags;


    if (vkCreateBuffer(vulkan_context->device.logical_device, &buffer_create_info, vulkan_context->allocator, buffer) != VK_SUCCESS)
    {
        MASSERT("failed to create buffer!");
    }

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
    VkMemoryAllocateInfo memory_allocate_info{};
    memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memory_allocate_info.allocationSize = memory_requirements.size;
    memory_allocate_info.memoryTypeIndex = find_memory_type(vulkan_context, memory_requirements.memoryTypeBits,
                                                          properties);

    if (vkAllocateMemory(vulkan_context->device.logical_device, &memory_allocate_info, NULL, bufferMemory) != VK_SUCCESS)
    {
        MASSERT("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(vulkan_context->device.logical_device, *buffer, *bufferMemory, 0);
}

void buffer_destroy_free(vulkan_context* vulkan_context, Buffer_Context* buffer_context)
{
    vkDestroyBuffer(vulkan_context->device.logical_device, buffer_context->index_buffer, NULL);
    vkFreeMemory(vulkan_context->device.logical_device, buffer_context->index_buffer_memory, NULL);

    vkDestroyBuffer(vulkan_context->device.logical_device, buffer_context->vertex_buffer, NULL);
    vkFreeMemory(vulkan_context->device.logical_device, buffer_context->vertex_buffer_memory, NULL);

    vkDestroyBuffer(vulkan_context->device.logical_device, buffer_context->index_staging_buffer, NULL);
    vkFreeMemory(vulkan_context->device.logical_device, buffer_context->index_staging_buffer_memory, NULL);

    vkDestroyBuffer(vulkan_context->device.logical_device, buffer_context->vertex_staging_buffer, NULL);
    vkFreeMemory(vulkan_context->device.logical_device, buffer_context->vertex_staging_buffer_memory, NULL);
}

void buffer_copy(vulkan_context* vulkan_context, Command_Buffer_Context* command_buffer_index, VkBuffer srcBuffer,
    VkBuffer dstBuffer, VkDeviceSize size)
{
    //Memory transfer operations are executed using command buffers, just like drawing commands

    //NOTE:
    //You may wish to create a separate command pool for these kinds of short-lived buffers,
    //because the implementation may be able to apply memory allocation optimizations.
    //You should use the VK_COMMAND_POOL_CREATE_TRANSIENT_BIT flag during command pool generation in that case.

    //create a command buffer
    VkCommandBufferAllocateInfo command_buffer_allocation_info{};
    command_buffer_allocation_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    command_buffer_allocation_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    command_buffer_allocation_info.commandPool = command_buffer_index->command_pool;
    command_buffer_allocation_info.commandBufferCount = 1;

    VkCommandBuffer temp_command_buffer;
    vkAllocateCommandBuffers(vulkan_context->device.logical_device, &command_buffer_allocation_info, &temp_command_buffer);

    VkCommandBufferBeginInfo command_buffer_begin_info{};
    command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    //copy command buffer over
    vkBeginCommandBuffer(temp_command_buffer, &command_buffer_begin_info);

    VkBufferCopy copy_region{};
    copy_region.srcOffset = 0; // Optional
    copy_region.dstOffset = 0; // Optional
    copy_region.size = size;
    vkCmdCopyBuffer(temp_command_buffer, srcBuffer, dstBuffer, 1, &copy_region);

    vkEndCommandBuffer(temp_command_buffer);

    //submit command buffer
    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &temp_command_buffer;

    vkQueueSubmit(vulkan_context->device.graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
    //wait for process to finish
    vkQueueWaitIdle(vulkan_context->device.graphics_queue);

    vkFreeCommandBuffers(vulkan_context->device.logical_device, command_buffer_index->command_pool, 1, &temp_command_buffer);

}
*/
