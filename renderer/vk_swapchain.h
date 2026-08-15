#ifndef VK_SWAPCHAIN_H
#define VK_SWAPCHAIN_H


#include <vulkan/vulkan.h>
#include "vulkan_struct_types.h"

void vulkan_swapchain_create(Vulkan_Context* context, u32 width, u32 height, vulkan_swapchain* swapchain_out);

void vulkan_swapchain_destroy(Vulkan_Context* context, vulkan_swapchain* swapchain);

void vulkan_swapchain_recreate(Vulkan_Context* context, u32 width, u32 height, vulkan_swapchain* swapchain);

bool vulkan_swapchain_acquire_next_image_index(Vulkan_Context* context, vulkan_swapchain* swapchain, u64 timeout_ns,
                                               VkSemaphore image_available_semaphore, VkFence fence_out,
                                               u32* out_image_index);

void vulkan_swapchain_present_image(Vulkan_Context* context, vulkan_swapchain* swapchain,
                                    VkQueue present_queue,
                                    VkSemaphore render_complete_semaphore,
                                    u32 present_image_index);

bool recreate_swapchain(Vulkan_Context* backend);

#endif
