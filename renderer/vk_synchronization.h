#ifndef VK_FENCES_H
#define VK_FENCES_H

#include "vulkan_types.h"


void vulkan_fence_create(vulkan_context* context, bool create_signaled, vulkan_fence* out_fence);

void vulkan_fence_destroy(vulkan_context* context, vulkan_fence* fence);

b8 vulkan_fence_wait(vulkan_context* context, vulkan_fence* fence, u64 timeout_ns);

void vulkan_fence_reset(vulkan_context* context, vulkan_fence* fence);

#endif
