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

bool vulkan_fence_reset(Renderer* renderer, VkFence* fence)
{
    vkResetFences(renderer->context.logical_device, 1, fence);
}

void binary_semaphore_create(Renderer* renderer, VkSemaphore* semaphore)
{
    VkSemaphoreCreateInfo semaphoreInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .flags = 0,
        .pNext = 0,
    };

    VkResult semaphore_result = vkCreateSemaphore(renderer->context.logical_device, &semaphoreInfo, NULL,
                                                  semaphore);
    VK_CHECK(semaphore_result);
}

void binary_semaphore_destroy(Renderer* renderer, VkSemaphore* semaphore)
{
    vkDestroySemaphore(renderer->context.logical_device, *semaphore, NULL);
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
    const VkResult result = vkGetSemaphoreCounterValue(renderer->context.logical_device, *timeline_semaphore,
                                                       out_counter_value);
    VK_CHECK(result);
}

bool timeline_semaphore_query_and_compare(const Renderer* renderer, const VkSemaphore* timeline_semaphore,
                                          const u64 compare_value)
{
    u64 out_counter_value = 0;
    const VkResult result = vkGetSemaphoreCounterValue(renderer->context.logical_device, *timeline_semaphore,
                                                       &out_counter_value);
    VK_CHECK(result);

    return compare_value <= out_counter_value;
}
