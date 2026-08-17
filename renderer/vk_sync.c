#include "vk_sync.h"


void vulkan_fence_create(Renderer* renderer, VkFence* fence)
{
    VkFenceCreateInfo fence_create_info = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT, // if needed we can pass this in as a param
    };
    VkResult fence_create_result = vkCreateFence(renderer->context.logical_device, &fence_create_info, NULL,
                                                 fence);

    VK_CHECK(fence_create_result)
}

void vulkan_fence_destroy(Renderer* renderer, VkFence* fence, VkFenceCreateFlags fenceCreateFlags)
{
    vkDestroyFence(renderer->context.logical_device, *fence, NULL);
}

void sync_object_per_frame_init(Renderer* renderer, Vulkan_Context* context)
{
    //memory
    context->queue_submit_fence = darray_create_reserve(VkFence, context->swapchain.image_count);
    context->primary_command_pool = darray_create_reserve(VkCommandPool, context->swapchain.image_count);
    context->primary_command_buffer = darray_create_reserve(VkCommandBuffer, context->swapchain.image_count);

    //NOTE: in the vulkan example, they are creating the semaphores on the fly during the frame if its null for that frame
    context->swapchain_acquire_semaphore = darray_create_reserve(VkSemaphore, context->swapchain.image_count);
    context->swapchain_release_semaphore = darray_create_reserve(VkSemaphore, context->swapchain.image_count);


    renderer->transfer_signal_sempahores = darray_create_reserve(VkSemaphore, context->swapchain.image_count);

    //create
    for (size_t i = 0; i < context->swapchain.image_count; i++)
    {
        VkFenceCreateInfo info = {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .flags = VK_FENCE_CREATE_SIGNALED_BIT,
        };
        VK_CHECK(vkCreateFence(context->logical_device, &info, NULL, &context->queue_submit_fence[i]));

        VkCommandPoolCreateInfo cmd_pool_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
            .queueFamilyIndex = (uint32_t)context->graphics_queue_index
        };
        VK_CHECK(
            vkCreateCommandPool(context->logical_device, &cmd_pool_info, NULL, &context->primary_command_pool[i]
            ));

        VkCommandBufferAllocateInfo cmd_buf_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = *context->primary_command_pool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1
        };
        VK_CHECK(
            vkAllocateCommandBuffers(context->logical_device, &cmd_buf_info, &context->primary_command_buffer[i]
            ));

        VkSemaphoreCreateInfo semaphoreInfo = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
        };
        VK_CHECK(
            vkCreateSemaphore(context->logical_device, &semaphoreInfo, NULL, &context->
                swapchain_acquire_semaphore[i]));
        VK_CHECK(
            vkCreateSemaphore(context->logical_device, &semaphoreInfo, NULL, &context->
                swapchain_release_semaphore[i]));
        VK_CHECK(
            vkCreateSemaphore(context->logical_device, &semaphoreInfo, NULL, &renderer->
                transfer_signal_sempahores[i]));
    }
}



bool vulkan_fence_wait(Vulkan_Context* context, VkFence* fence, u64 timeout_ns)
{
    // if (!fence->is_signaled)
    // {
    VkResult result = vkWaitForFences(
        context->logical_device,
        1,
        fence,
        VK_TRUE,
        timeout_ns);

    switch (result)
    {
    case VK_SUCCESS:
        VkResult fence_reset_result = vkResetFences(context->logical_device, 1, fence);
        VK_CHECK(fence_reset_result);

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


    return false;
}

void create_semaphore(Renderer* renderer)
{
    //NOTE: not likely rn that I would need to deallocate the semaphores
    VkSemaphore* semaphore = allocator_alloc(&renderer->allocator, sizeof(VkSemaphore) * 3);
    VkSemaphoreCreateInfo semaphoreInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
    };

    for (int i = 0; i < renderer->context.swapchain.max_frames_in_flight; ++i)
    {
        VkResult semaphore_result = vkCreateSemaphore(renderer->context.logical_device, &semaphoreInfo, NULL,
                                                      &semaphore[i]);
        VK_CHECK(semaphore_result);
    }
}

void destroy_sempahore(Renderer* renderer, VkSemaphore* semaphore)
{
    for (int i = 0; i < renderer->context.swapchain.max_frames_in_flight; ++i)
    {
        vkDestroySemaphore(renderer->context.logical_device, semaphore[i], renderer->context.allocator);
    }
}

void timeline_semaphore_create(Renderer* renderer, VkSemaphore* timeline_semaphore)
{
    // SPEC: A timeline semaphore is still a semaphore, but it is of TIMELINE type rather than BINARY.
    VkSemaphoreTypeCreateInfo type_create_info = {0};
    type_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
    type_create_info.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
    type_create_info.initialValue = 0;

    VkSemaphoreCreateInfo create_info = {0};
    create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    create_info.flags = 0;
    create_info.pNext = &type_create_info; // setting as timeline


    VK_CHECK(vkCreateSemaphore(renderer->context.logical_device, &create_info, NULL, timeline_semaphore));
}

void timeline_semaphore_destroy(Renderer* renderer, VkSemaphore* timeline_semaphore)
{
    vkDestroySemaphore(renderer->context.logical_device, *timeline_semaphore, NULL);
}

void timeline_semaphore_query(Renderer* renderer, VkSemaphore* timeline_semaphore, u64* out_counter_value)
{
    const VkResult result = vkGetSemaphoreCounterValue(renderer->context.logical_device, *timeline_semaphore, out_counter_value);
    VK_CHECK(result);

}

bool timeline_semaphore_query_and_compare(const Renderer* renderer, const VkSemaphore* timeline_semaphore,
    const u64 compare_value)
{
    u64 out_counter_value =0;
    const VkResult result = vkGetSemaphoreCounterValue(renderer->context.logical_device, *timeline_semaphore, &out_counter_value);
    VK_CHECK(result);

    return compare_value <= out_counter_value;

}
