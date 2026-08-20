#ifndef VK_FRAMEBUFFER_H
#define VK_FRAMEBUFFER_H


#include "vulkan_struct_types.h"



void vulkan_framebuffer_create(
    Vulkan_Context* context,
    Vulkan_Renderpass* renderpass,
    u32 width,
    u32 height,
    u32 attachment_count,
    VkImageView* attachments,
    Vulkan_Framebuffer* out_framebuffer, Renderer* renderer);


void vulkan_framebuffer_destroy(Vulkan_Context* context, Vulkan_Framebuffer* framebuffer, Renderer* renderer);

//used by the renderer
void regenerate_framebuffer(Vulkan_Context* context, Vulkan_Swapchain* swapchain, Vulkan_Renderpass* renderpass, Renderer* renderer);

#endif
