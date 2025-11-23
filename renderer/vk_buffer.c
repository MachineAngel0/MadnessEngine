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


bool buffer_create(vulkan_context* vulkan_context, VkDeviceSize size, VkBufferUsageFlags usage,
                   VkMemoryPropertyFlags properties, VkBuffer* buffer, VkDeviceMemory* bufferMemory)
{
    //create buffer
    VkBufferCreateInfo buffer_create_info = {0};
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
    VkMemoryAllocateInfo memory_allocate_info = {0};
    memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memory_allocate_info.allocationSize = memory_requirements.size;
    memory_allocate_info.memoryTypeIndex = find_memory_type(vulkan_context, memory_requirements.memoryTypeBits,
                                                          properties);

    if (vkAllocateMemory(vulkan_context->device.logical_device, &memory_allocate_info, NULL, bufferMemory) != VK_SUCCESS)
    {
        MASSERT("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(vulkan_context->device.logical_device, *buffer, *bufferMemory, 0);

    return true;
}

bool buffer_destroy_free(vulkan_context* vulkan_context, vertex_buffer* vertex_buffer)
{
    vkDestroyBuffer(vulkan_context->device.logical_device, vertex_buffer->index_buffer, NULL);
    vkFreeMemory(vulkan_context->device.logical_device, vertex_buffer->index_buffer_memory, NULL);

    vkDestroyBuffer(vulkan_context->device.logical_device, vertex_buffer->vertex_buffer, NULL);
    vkFreeMemory(vulkan_context->device.logical_device, vertex_buffer->vertex_buffer_memory, NULL);

    vkDestroyBuffer(vulkan_context->device.logical_device, vertex_buffer->index_staging_buffer, NULL);
    vkFreeMemory(vulkan_context->device.logical_device, vertex_buffer->index_staging_buffer_memory, NULL);

    vkDestroyBuffer(vulkan_context->device.logical_device, vertex_buffer->vertex_staging_buffer, NULL);
    vkFreeMemory(vulkan_context->device.logical_device, vertex_buffer->vertex_staging_buffer_memory, NULL);

    return true;

}



void buffer_copy(vulkan_context* vulkan_context, command_buffer* command_buffer_context,
                 VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
    // Create a temporary command buffer for the copy operation
    VkCommandBufferAllocateInfo command_buffer_allocation_info = {0};
    command_buffer_allocation_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    command_buffer_allocation_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    command_buffer_allocation_info.commandPool = vulkan_context->graphics_command_pool; //TODO: likely to change
    command_buffer_allocation_info.commandBufferCount = 1;

    VkCommandBuffer temp_command_buffer;
    vkAllocateCommandBuffers(vulkan_context->device.logical_device, &command_buffer_allocation_info, &temp_command_buffer);

    // Begin recording the command buffer
    VkCommandBufferBeginInfo command_buffer_begin_info = {0};
    command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(temp_command_buffer, &command_buffer_begin_info);

    // Set up the copy region with specified offsets and size
    VkBufferCopy copy_region = {0};
    copy_region.srcOffset = 0;
    copy_region.dstOffset = 0;
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
    vkFreeCommandBuffers(vulkan_context->device.logical_device, vulkan_context->graphics_command_pool, 1, &temp_command_buffer);
}


void buffer_copy_region(vulkan_context* vulkan_context, command_buffer* command_buffer_context, VkBuffer srcBuffer,
    VkBuffer dstBuffer, VkDeviceSize size, VkDeviceSize srcOffset, VkDeviceSize dstOffset)
{

    // Create a temporary command buffer for the copy operation
    VkCommandBufferAllocateInfo command_buffer_allocation_info = {0};
    command_buffer_allocation_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    command_buffer_allocation_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    command_buffer_allocation_info.commandPool = vulkan_context->graphics_command_pool; //TODO: likely to change
    command_buffer_allocation_info.commandBufferCount = 1;

    VkCommandBuffer temp_command_buffer;
    vkAllocateCommandBuffers(vulkan_context->device.logical_device, &command_buffer_allocation_info, &temp_command_buffer);

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
    vkFreeCommandBuffers(vulkan_context->device.logical_device, vulkan_context->graphics_command_pool, 1, &temp_command_buffer);
}