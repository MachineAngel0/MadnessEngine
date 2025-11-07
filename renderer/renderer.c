#include "renderer.h"

#include "logger.h"
#include "vk_device.h"


//NOTE: static/global for now, most likely gonna move it into the renderer struct
static vulkan_context vk_context;

bool renderer_init(struct renderer* renderer_inst)
{
    // vulkan_context vulkan_context;
    vulkan_instance_create(&vk_context);
    platform_create_vulkan_surface(renderer_inst->plat_state, &vk_context);
    vulkan_device_create(&vk_context);

    vulkan_swapchain_create(
      &vk_context,
      vk_context.framebuffer_width,
      vk_context.framebuffer_height,
      &vk_context.swapchain);

    return true;
}

void renderer_update(struct renderer* renderer_inst)
{
    TRACE("renderer is running test");
}

void renderer_shutdown(struct renderer* renderer_inst)
{
    INFO("Renderer Shutting Down");

    vulkan_swapchain_destroy(&vk_context, &vk_context.swapchain);

    vulkan_device_destroy(&vk_context);
    vulkan_instance_destroy(&vk_context);
}
