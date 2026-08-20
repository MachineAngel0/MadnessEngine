#ifndef VK_SWAPCHAIN_H
#define VK_SWAPCHAIN_H


#include <vulkan/vulkan.h>
#include "vulkan_struct_types.h"

void vulkan_swapchain_create(Renderer* renderer, Vulkan_Context* context, u32 width, u32 height, Vulkan_Swapchain* swapchain_out);

void vulkan_swapchain_destroy(Vulkan_Context* context, Vulkan_Swapchain* swapchain, Renderer* renderer);

void vulkan_swapchain_recreate(Renderer* renderer, Vulkan_Context* context, u32 width, u32 height, Vulkan_Swapchain* swapchain);

bool vulkan_swapchain_acquire_next_image_index(Renderer* renderer, Vulkan_Context* context, Vulkan_Swapchain* swapchain,
                                               u64 timeout_ns, VkSemaphore image_available_semaphore,
                                               VkFence fence_out, u32* out_image_index);

void vulkan_swapchain_present_image(Renderer* renderer, Vulkan_Context* context,
                                    Vulkan_Swapchain* swapchain,
                                    VkQueue present_queue,
                                    VkSemaphore render_complete_semaphore, u32 present_image_index);

bool recreate_swapchain(Renderer* renderer);

#endif
