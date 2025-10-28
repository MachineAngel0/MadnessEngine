#include "vk_buffer.h"

#include <stdexcept>

#include "vk_command_buffer.h"
#include "vk_device.h"

uint32_t findMemoryType(Vulkan_Context& vulkan_context, uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(vulkan_context.physical_device, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type");
}

void buffer_create(Vulkan_Context& vulkan_context, VkDeviceSize size, VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
    //create buffer
    VkBufferCreateInfo buffer_create_info{};
    buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_create_info.size = size;
    buffer_create_info.usage = usage;
    buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    //The flags parameter is used to configure sparse buffer memory, which is not relevant right now. We'll leave it at the default value of 0.
    //bufferInfo.flags;


    if (vkCreateBuffer(vulkan_context.logical_device, &buffer_create_info, nullptr, &buffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create buffer!");
    }

    //finding memory size needed for the buffer
    VkMemoryRequirements memory_requirements;
    vkGetBufferMemoryRequirements(vulkan_context.logical_device, buffer, &memory_requirements);


    /* NOTE:
    * It should be noted that in a real world application,
    * you're not supposed to actually call vkAllocateMemory for every individual buffer.
    * The maximum number of simultaneous memory allocations is limited by the maxMemoryAllocationCount physical device limit,
    * which may be as low as 4096 even on high end hardware like an NVIDIA GTX 1080.
    * The right way to allocate memory for a large number of objects at the same time
    * is to create a custom allocator that splits up a single allocation among many different objects by using the offset parameters that we've seen in many functions.
    *You can either implement such an allocator yourself,
    *or use the VulkanMemoryAllocator library provided by the GPUOpen initiative.
    *However, for this tutorial it's okay to use a separate allocation for every resource,
    *because we won't come close to hitting any of these limits for now.
     */

    //allocate buffer and bind to memory
    VkMemoryAllocateInfo memory_allocate_info{};
    memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memory_allocate_info.allocationSize = memory_requirements.size;
    memory_allocate_info.memoryTypeIndex = findMemoryType(vulkan_context, memory_requirements.memoryTypeBits,
                                                          properties);

    if (vkAllocateMemory(vulkan_context.logical_device, &memory_allocate_info, nullptr, &bufferMemory) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(vulkan_context.logical_device, buffer, bufferMemory, 0);
}

void buffer_destroy_free(Vulkan_Context& vulkan_context, Buffer_Context& buffer_context)
{
    vkDestroyBuffer(vulkan_context.logical_device, buffer_context.index_buffer, nullptr);
    vkFreeMemory(vulkan_context.logical_device, buffer_context.index_buffer_memory, nullptr);

    vkDestroyBuffer(vulkan_context.logical_device, buffer_context.vertex_buffer, nullptr);
    vkFreeMemory(vulkan_context.logical_device, buffer_context.vertex_buffer_memory, nullptr);

    vkDestroyBuffer(vulkan_context.logical_device, buffer_context.index_staging_buffer, nullptr);
    vkFreeMemory(vulkan_context.logical_device, buffer_context.index_staging_buffer_memory, nullptr);

    vkDestroyBuffer(vulkan_context.logical_device, buffer_context.vertex_staging_buffer, nullptr);
    vkFreeMemory(vulkan_context.logical_device, buffer_context.vertex_staging_buffer_memory, nullptr);
}

void buffer_copy(Vulkan_Context& vulkan_context, Command_Buffer_Context& command_buffer_index, VkBuffer srcBuffer,
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
    command_buffer_allocation_info.commandPool = command_buffer_index.command_pool;
    command_buffer_allocation_info.commandBufferCount = 1;

    VkCommandBuffer temp_command_buffer;
    vkAllocateCommandBuffers(vulkan_context.logical_device, &command_buffer_allocation_info, &temp_command_buffer);

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

    vkQueueSubmit(vulkan_context.graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
    //wait for process to finish
    vkQueueWaitIdle(vulkan_context.graphics_queue);

    vkFreeCommandBuffers(vulkan_context.logical_device, command_buffer_index.command_pool, 1, &temp_command_buffer);

}
