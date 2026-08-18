#include "vk_command_buffer.h"
#include <string.h>


Vulkan_Queue_System* vulkan_queue_system_init(Renderer* renderer)
{
    Vulkan_Queue_System* cb_system = allocator_alloc(&renderer->allocator,
                                                              sizeof(Vulkan_Queue_System));

    //create the command buffers and see how many we get available from the pool, from our intended max
    cb_system->graphics_command_buffer_count = MAX_VULKAN_COMMAND_BUFFERS;
    for (u32 i = 0; i < cb_system->graphics_command_buffer_count; i++)
    {
        Vulkan_Command_Buffer* cb = &cb_system->graphics_command_buffers[i];
        if (!vulkan_command_buffer_allocate(&renderer->context, cb,
                                            VULKAN_COMMAND_BUFFER_LEVEL_PRIMARY,
                                            renderer->context.graphics_command_pool))
        {
            cb->handle = 0;
            cb_system->graphics_command_buffer_count = i - 1;
            break;
        }
        cb->state = VULKAN_COMMAND_BUFFER_STATE_USABLE;
        cb->queue_type = VULKAN_QUEUE_TYPE_GRAPHICS;
    }



    return cb_system;
}

bool vulkan_queue_system_deinit(Renderer* renderer, Vulkan_Queue_System* queue_system)
{
    vkDeviceWaitIdle(renderer->context.logical_device);

    for (u32 i = 0; i < queue_system->graphics_command_buffer_count; i++)
    {
        vulkan_command_buffer_free(&renderer->context, &queue_system->graphics_command_buffers[i],
                                   renderer->context.graphics_command_pool);
    }




    return true;
}

bool vulkan_queue_system_get_cb(Vulkan_Queue_System* system, Vulkan_Queue_Type type,
                                         Vulkan_Command_Buffer** out_cb)
{
    switch (type)
    {
    case VULKAN_QUEUE_TYPE_GRAPHICS:

        for (u32 i = 0; i < system->graphics_command_buffer_count; i++)
        {
            if ((system->graphics_command_buffers[i].state == VULKAN_COMMAND_BUFFER_STATE_USABLE))
            {
                *out_cb = &system->graphics_command_buffers[i];
                return true;
            }
        }

        break;
    case VULKAN_QUEUE_TYPE_TRANSFER:
        MASSERT(false)

        break;
    case VULKAN_QUEUE_TYPE_COMPUTE:
        MASSERT(false)
        break;
    }


    INFO("NO COMMAND BUFFERS AVAILABLE")
    return false;
}

bool vulkan_queue_system_get_and_begin_cb(Vulkan_Queue_System* system,
                                                   Vulkan_Queue_Type type,
                                                   Vulkan_Command_Buffer** out_cb)
{
    if (!vulkan_queue_system_get_cb(system, type, out_cb))
    {
        return false;
    }

    vulkan_command_buffer_begin(*out_cb);

    return true;
}

bool vulkan_command_buffer_add_semaphore(Vulkan_Command_Buffer* cb, VkSemaphoreSubmitInfo submit_info,
    Vulkan_Semaphore_Submit_Type submit_type)
{
    //TODO:
    MASSERT_FALSE();
}

void vulkan_renderer_command_buffers_create(Vulkan_Context* vk_context)
{
    //this won't work rn, because it never gets freed or zero'd
    // if (!vk_context->graphics_command_buffers)
    // {
    vk_context->graphics_command_buffer = darray_create_reserve(Vulkan_Command_Buffer,
                                                                vk_context->swapchain.image_count);
    vk_context->transfer_command_buffer = darray_create_reserve(Vulkan_Command_Buffer,
                                                                vk_context->swapchain.image_count);
    vk_context->compute_command_buffer = darray_create_reserve(Vulkan_Command_Buffer,
                                                               vk_context->swapchain.image_count);
    for (u32 i = 0; i < vk_context->swapchain.image_count; i++)
    {
        memset(&vk_context->graphics_command_buffer[i], 0, sizeof(Vulkan_Command_Buffer));
        memset(&vk_context->transfer_command_buffer[i], 0, sizeof(Vulkan_Command_Buffer));
        memset(&vk_context->compute_command_buffer[i], 0, sizeof(Vulkan_Command_Buffer));
    }

    for (u32 i = 0; i < vk_context->swapchain.image_count; i++)
    {
        vulkan_command_buffer_allocate(vk_context, &vk_context->graphics_command_buffer[i],
                                       VULKAN_COMMAND_BUFFER_LEVEL_PRIMARY,
                                       vk_context->graphics_command_pool);
        vulkan_command_buffer_allocate(vk_context, &vk_context->transfer_command_buffer[i],
                                       VULKAN_COMMAND_BUFFER_LEVEL_PRIMARY,
                                       vk_context->transfer_command_pool);
        vulkan_command_buffer_allocate(vk_context, &vk_context->compute_command_buffer[i],
                                       VULKAN_COMMAND_BUFFER_LEVEL_PRIMARY,
                                       vk_context->compute_command_pool);
    }
    // }
    INFO("COMMAND BUFFERS CREATED");
}

void vulkan_renderer_command_buffer_destroy(Vulkan_Context* vk_context)
{
    for (u32 i = 0; i < vk_context->swapchain.image_count; ++i)
    {
        if (vk_context->graphics_command_buffer[i].handle)
        {
            vulkan_command_buffer_free(
                vk_context,
                &vk_context->graphics_command_buffer[i],
                vk_context->graphics_command_pool);
            vk_context->graphics_command_buffer[i].handle = 0;
        }
    }
    darray_free(&vk_context->graphics_command_buffer);
    vk_context->graphics_command_buffer = 0;
    INFO("COMMAND BUFFERS DESTROYED");
}


bool vulkan_command_buffer_allocate(Vulkan_Context* context, Vulkan_Command_Buffer* out_command_buffer,
                                    Vulkan_Command_Buffer_Level cb_level,
                                    VkCommandPool pool)
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

    VkResult cmd_buffer_alloc_result = vkAllocateCommandBuffers(context->logical_device, &allocate_info,
                                                                &out_command_buffer->handle);
    // VK_CHECK(cmd_buffer_alloc_result)
    if (cmd_buffer_alloc_result != VK_SUCCESS)
    {
        return false;
    }

    return true;
}

void vulkan_command_buffer_free(Vulkan_Context* context, Vulkan_Command_Buffer* command_buffer, VkCommandPool pool)
{
    vkFreeCommandBuffers(context->logical_device, pool, 1, &command_buffer->handle);
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

void vulkan_command_buffer_begin_old(Vulkan_Command_Buffer* command_buffer, bool is_single_use,
                                     bool is_renderpass_continue,
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

void vulkan_command_buffer_end(Vulkan_Command_Buffer* command_buffer)
{
    VK_CHECK(vkEndCommandBuffer(command_buffer->handle));
    command_buffer->state = VULKAN_COMMAND_BUFFER_STATE_END;
}


void vulkan_command_buffer_allocate_and_begin_single_use(Vulkan_Context* context, VkCommandPool pool,
                                                         Vulkan_Command_Buffer* out_command_buffer)
{
    vulkan_command_buffer_allocate(context, out_command_buffer, VULKAN_COMMAND_BUFFER_LEVEL_PRIMARY, pool);
    vulkan_command_buffer_begin_old(out_command_buffer, true, false, false);
}



void vulkan_command_buffer_end_and_submit_and_free_single_use(Vulkan_Context* context, VkCommandPool pool,
                                                              Vulkan_Command_Buffer* command_buffer, VkQueue queue)
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
    vulkan_command_buffer_free(context, command_buffer, pool);
}


void vulkan_command_buffer_submit(Vulkan_Context* context, Vulkan_Command_Buffer* command_buffer, VkQueue queue)
{
    // Submit the command buffer to the queue to finish the copy
    VkSubmitInfo submitInfo = {0};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &command_buffer->handle;

    // Create fence to ensure that the command buffer has finished executing
    VkFenceCreateInfo fenceCI = {0};
    fenceCI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    VkFence fence;
    VK_CHECK(vkCreateFence(context->logical_device, &fenceCI, context->allocator, &fence));
    // Submit copies to the queue
    VK_CHECK(vkQueueSubmit(queue, 1, &submitInfo, fence));
    // Wait for the fence to signal that command buffer has finished executing
    VK_CHECK(vkWaitForFences(context->logical_device, 1, &fence, VK_TRUE, UINT64_MAX));
    vkDestroyFence(context->logical_device, fence, 0);
}


void vulkan_command_buffer_submit_binary_semaphore(Vulkan_Context* context,
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
    VK_CHECK(vkCreateFence(context->logical_device, &fenceCI, context->allocator, &fence));

    // Submit copies to the queue
    vkQueueSubmit2(queue, 1, &submitInfo, fence);

    // Wait for the fence to signal that command buffer has finished executing
    VK_CHECK(vkWaitForFences(context->logical_device, 1, &fence, VK_TRUE, UINT64_MAX));
    vkDestroyFence(context->logical_device, fence, 0);
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


void vulkan_command_buffer_begin_debug_label(Renderer* renderer, Vulkan_Command_Buffer* command_buffer,
                                             const char* name)
{
    VkDebugUtilsLabelEXT debug_label = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT,
        .pNext = NULL,
        .pLabelName = name,
        .color = {1.0, 0.0, 0.0, 1.0}
    };
    renderer->context.debug_label_start(command_buffer->handle, &debug_label);
}

void vulkan_command_buffer_end_debug_label(Renderer* renderer, Vulkan_Command_Buffer* command_buffer)
{
    renderer->context.debug_label_end(command_buffer->handle);
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
