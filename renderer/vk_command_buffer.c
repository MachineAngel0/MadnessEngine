#include "vk_command_buffer.h"
#include <string.h>


void vulkan_renderer_command_buffers_create(vulkan_context* vk_context)
{
    //this won't work rn, because it never gets freed or zero'd
    // if (!vk_context->graphics_command_buffers)
    // {
        vk_context->graphics_command_buffer = darray_create_reserve(vulkan_command_buffer,
                                                                     vk_context->swapchain.image_count);
        for (u32 i = 0; i < vk_context->swapchain.image_count; i++)
        {
            memset(&vk_context->graphics_command_buffer[i], 0, sizeof(vulkan_command_buffer));
        }

        for (u32 i = 0; i < vk_context->swapchain.image_count; i++)
        {
            if (vk_context->graphics_command_buffer[i].handle)
            {
                vulkan_command_buffer_free(vk_context, vk_context->graphics_command_pool,
                                           &vk_context->graphics_command_buffer[i]);
            }
            memset(&vk_context->graphics_command_buffer[i], 0, sizeof(vulkan_command_buffer));
            vulkan_command_buffer_allocate(vk_context, vk_context->graphics_command_pool, true,
                                           &vk_context->graphics_command_buffer[i]);
        }
    // }
    INFO("COMMAND BUFFERS CREATED");
}

void vulkan_renderer_command_buffer_destroy(vulkan_context* vk_context)
{
    for (u32 i = 0; i < vk_context->swapchain.image_count; ++i)
    {
        if (vk_context->graphics_command_buffer[i].handle)
        {
            vulkan_command_buffer_free(
                vk_context,
                vk_context->graphics_command_pool,
                &vk_context->graphics_command_buffer[i]);
            vk_context->graphics_command_buffer[i].handle = 0;
        }
    }
    darray_free(&vk_context->graphics_command_buffer);
    vk_context->graphics_command_buffer = 0;
    INFO("COMMAND BUFFERS DESTROYED");

}


void vulkan_command_buffer_allocate(vulkan_context* context, VkCommandPool pool, bool is_primary,
                                    vulkan_command_buffer* out_command_buffer)
{
    memset(out_command_buffer, 0, sizeof(out_command_buffer));

    VkCommandBufferAllocateInfo allocate_info = {0};
    allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocate_info.commandPool = pool;
    if (is_primary)
    {
        allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    }
    else
    {
        allocate_info.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
    }
    allocate_info.commandBufferCount = 1;
    allocate_info.pNext = 0;

    VK_CHECK(
        vkAllocateCommandBuffers(context->device.logical_device, &allocate_info, &out_command_buffer->
            handle));
}

void vulkan_command_buffer_free(vulkan_context* context, VkCommandPool pool, vulkan_command_buffer* command_buffer)
{
    vkFreeCommandBuffers(context->device.logical_device, pool, 1, &command_buffer->handle);
    command_buffer->handle = 0;
}

void vulkan_command_buffer_begin(vulkan_command_buffer* command_buffer, bool is_single_use, bool is_renderpass_continue,
                                 bool is_simultaneous_use)
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
    if (is_single_use)
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
    }

    VK_CHECK(vkBeginCommandBuffer(command_buffer->handle, &begin_info));
}

void vulkan_command_buffer_end(vulkan_command_buffer* command_buffer)
{
    VK_CHECK(vkEndCommandBuffer(command_buffer->handle));
}


void vulkan_command_buffer_allocate_and_begin_single_use(vulkan_context* context, VkCommandPool pool,
                                                         vulkan_command_buffer* out_command_buffer)
{
    vulkan_command_buffer_allocate(context, pool, true, out_command_buffer);
    vulkan_command_buffer_begin(out_command_buffer, true, false, false);
}

void vulkan_command_buffer_end_single_use(vulkan_context* context, VkCommandPool pool,
                                          vulkan_command_buffer* command_buffer, VkQueue queue)
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

    //TODO: Fence
    VK_CHECK(vkQueueSubmit(queue, 1, &submit_info, 0));

    //wait for command buffer to finish then free
    VK_CHECK(vkQueueWaitIdle(queue));
    vulkan_command_buffer_free(context, pool, command_buffer);
}


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
