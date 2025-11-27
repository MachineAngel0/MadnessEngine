#ifndef VK_FENCES_H
#define VK_FENCES_H

#include "vulkan_types.h"


void init_per_frame_sync(vulkan_context* context);

b8 vulkan_fence_wait(vulkan_context* context, VkFence* fence, u64 timeout_ns);


#endif
