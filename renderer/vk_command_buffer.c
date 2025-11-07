#include "vk_command_buffer.h"


/*
void command_pool_allocate(vulkan_context* vulkan_context, Command_Buffer_Context* command_buffer_context)
{
    QueueFamilyIndices queue_families_indices = find_queue_families(vulkan_context->surface,
                                                                    vulkan_context->device.physical_device);

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

    VK_CHECK(vkCreateCommandPool(vulkan_context->device.logical_device, &pool_create_info, NULL,
        &command_buffer_context->command_pool))
    INFO("CREATED COMMANDPOOL SUCCESS\n");
}

void command_pool_free(vulkan_context* vulkan_context, Command_Buffer_Context* command_buffer_context)
{
    vkDestroyCommandPool(vulkan_context->device.logical_device, command_buffer_context->command_pool, NULL);
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


    VK_CHECK(vkAllocateCommandBuffers(vulkan_context->device.logical_device, &buffer_allocate_info,
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
        vkFreeCommandBuffers(vulkan_context->device.logical_device, command_buffer_context->command_pool, 1,
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