#include "vk_sync.h"
#include "vulkan_types.h"


void init_per_frame_sync(vulkan_context* context)
{

    //memory
    context->queue_submit_fence = darray_create_reserve(VkFence, context->swapchain.image_count);
    context->primary_command_pool = darray_create_reserve(VkCommandPool, context->swapchain.image_count);
    context->primary_command_buffer = darray_create_reserve(VkCommandBuffer, context->swapchain.image_count);

    //NOTE: in the vulkan example, they are creating the semaphores on the fly during the frame if its null for that frame
    context->swapchain_acquire_semaphore = darray_create_reserve(VkSemaphore, context->swapchain.image_count);
    context->swapchain_release_semaphore = darray_create_reserve(VkSemaphore, context->swapchain.image_count);

    //create
    for (size_t i = 0; i < context->swapchain.image_count; i++)
    {
        VkFenceCreateInfo info{
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .flags = VK_FENCE_CREATE_SIGNALED_BIT
        };
        VK_CHECK(vkCreateFence(context->device.logical_device, &info, NULL, &context->queue_submit_fence[i]));

        VkCommandPoolCreateInfo cmd_pool_info{
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
            .queueFamilyIndex = (uint32_t) (context->device.graphics_queue)
        };
        VK_CHECK(
            vkCreateCommandPool(context->device.logical_device, &cmd_pool_info, NULL, &context->primary_command_pool[i]));

        VkCommandBufferAllocateInfo cmd_buf_info{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = *context->primary_command_pool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1
        };
        VK_CHECK(
            vkAllocateCommandBuffers(context->device.logical_device, &cmd_buf_info, &context->primary_command_buffer[i]));

        VkSemaphoreCreateInfo semaphoreInfo{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
        };
        VK_CHECK(
            vkCreateSemaphore(context->device.logical_device, &semaphoreInfo, NULL, &context->
                swapchain_acquire_semaphore[i]));
        VK_CHECK(
            vkCreateSemaphore(context->device.logical_device, &semaphoreInfo, NULL, &context->
                swapchain_release_semaphore[i]));
    }
}


b8 vulkan_fence_wait(vulkan_context* context, VkFence* fence, u64 timeout_ns)
{
    // if (!fence->is_signaled)
    // {
    VkResult result = vkWaitForFences(
        context->device.logical_device,
        1,
        fence,
        true,
        timeout_ns);
	VK_CHECK(vkResetFences(context->device.logical_device, 1, fence));

    switch (result)
    {
        case VK_SUCCESS:
            return true;
        case VK_TIMEOUT:
            M_ERROR("vk_fence_wait - Timed out");
            break;
        case VK_ERROR_DEVICE_LOST:
            M_ERROR("vk_fence_wait - VK_ERROR_DEVICE_LOST.");
            break;
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            M_ERROR("vk_fence_wait - VK_ERROR_OUT_OF_HOST_MEMORY.");
            break;
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            M_ERROR("vk_fence_wait - VK_ERROR_OUT_OF_DEVICE_MEMORY.");
            break;
        default:
            M_ERROR("vk_fence_wait - An unknown error has occurred.");
            break;
    }
    // }
    // else
    // {
    // If already signaled, do not wait.
    // return TRUE;
    // }

    return FALSE;
}
