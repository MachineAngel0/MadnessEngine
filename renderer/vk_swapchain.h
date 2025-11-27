#ifndef VK_SWAPCHAIN_H
#define VK_SWAPCHAIN_H


#include <vulkan/vulkan.h>
#include "defines.h"
#include "vulkan_types.h"


void vulkan_swapchain_create(vulkan_context* context, u32 width, u32 height, vulkan_swapchain* swapchain_out);

void vulkan_swapchain_destroy(vulkan_context* context, vulkan_swapchain* swapchain);

void vulkan_swapchain_recreate(vulkan_context* context, u32 width, u32 height, vulkan_swapchain* swapchain);

bool vulkan_swapchain_acquire_next_image_index(vulkan_context* context, vulkan_swapchain* swapchain, u64 timeout_ns,
                                               VkSemaphore image_available_semaphore, VkFence fence_out,
                                               u32* out_image_index);

void vulkan_swapchain_present_image(vulkan_context* context, vulkan_swapchain* swapchain,
                              VkQueue graphics_queue, VkQueue present_queue,
                              VkSemaphore render_complete_semaphore,
                              u32 present_image_index);

bool recreate_swapchain(vulkan_context* backend);

#endif
