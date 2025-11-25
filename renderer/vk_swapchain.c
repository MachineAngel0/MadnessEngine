#include "vk_swapchain.h"

#include "logger.h"
#include "vk_command_buffer.h"
#include "vk_device.h"
#include "vk_image.h"
#include "vk_framebuffer.h"

void vulkan_swapchain_create(vulkan_context* context, u32 width, u32 height, vulkan_swapchain* swapchain_out)
{
    VkExtent2D swapchain_extent = {width, height};
    swapchain_out->max_frames_in_flight = 2; // this does mean 3 swapchain images, since we start at 0

    //choose a swap surface format, that suits our needs
    bool found = false;

    for (u32 i = 0; i < context->device.swapchain_capabilities.format_count; i++)
    {
        VkSurfaceFormatKHR current_surface_format = context->device.swapchain_capabilities.formats[i];
        //Preferred formats
        //NOTE: very unlikely to not have these format, but we can always create a second loop to find another ideal format
        if (current_surface_format.format == VK_FORMAT_R8G8B8A8_UNORM &&
            current_surface_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            swapchain_out->surface_format = current_surface_format;
            found = true;
            break;
        }
    }

    //if we don't find an ideal format, we will just pick the first one
    if (!found)
    {
        M_ERROR("NO IDEAL SWAPCHAIN FORMAT FOUND, PICKING FIRST AVAILABLE");
        swapchain_out->surface_format = context->device.swapchain_capabilities.formats[0];
    }

    //best to use mailbox mode (best for games) or fifo(basically vsync) (is always supported)
    VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR;
    for (u32 i = 0; i < context->device.swapchain_capabilities.present_mode_count; i++)
    {
        VkPresentModeKHR current_present_mode = context->device.swapchain_capabilities.present_modes[i];
        if (current_present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            present_mode = current_present_mode;
            break;
        };
    }

    //requery swapchain support, needed if a device was changed or resolution was changed
    vulkan_device_query_swapchain_support(context->device.physical_device, context->surface,
                                          &context->device.swapchain_capabilities);

    //set the swapchain extend, in the event anything happens
    if (context->device.swapchain_capabilities.capabilities.currentExtent.width != UINT32_MAX)
    {
        swapchain_extent = context->device.swapchain_capabilities.capabilities.currentExtent;
    }
    VkExtent2D min = context->device.swapchain_capabilities.capabilities.minImageExtent;
    VkExtent2D max = context->device.swapchain_capabilities.capabilities.maxImageExtent;

    swapchain_extent.width = clamp_int(swapchain_extent.width, min.width, max.width);
    swapchain_extent.height = clamp_int(swapchain_extent.height, min.height, max.height);

    u32 image_count = context->device.swapchain_capabilities.capabilities.minImageCount + 1;
    // in the event our image count is for some reason greater than the max allowed
    if (context->device.swapchain_capabilities.capabilities.minImageCount > 0 && image_count >
        context->device.swapchain_capabilities.capabilities.maxImageCount)
    {
        image_count = context->device.swapchain_capabilities.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR swapchain_create_info = {};
    swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_create_info.surface = context->surface;
    swapchain_create_info.minImageCount = image_count;
    swapchain_create_info.imageFormat = swapchain_out->surface_format.format;
    swapchain_create_info.imageColorSpace = swapchain_out->surface_format.colorSpace;
    swapchain_create_info.imageExtent = swapchain_extent;
    swapchain_create_info.imageArrayLayers = 1;
    swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // render to color buffer

    // Setup the queue family indices
    if (context->device.graphics_queue_index != context->device.present_queue_index)
    {
        // want images to be accessible by both queue families
        u32 queueFamilyIndices[] = {
            (u32) context->device.graphics_queue_index,
            (u32) context->device.present_queue_index
        };
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchain_create_info.queueFamilyIndexCount = 2;
        swapchain_create_info.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        // since they are both the same queue families, they both have single ownership over images
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchain_create_info.queueFamilyIndexCount = 0;
        swapchain_create_info.pQueueFamilyIndices = 0;
    }

    //portrait vs landscape
    swapchain_create_info.preTransform = context->device.swapchain_capabilities.capabilities.currentTransform;
    //compositing with the operating system, wont ever need this
    swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_create_info.presentMode = present_mode;
    swapchain_create_info.clipped = VK_TRUE; // dont render anything off the screen
    swapchain_create_info.oldSwapchain = 0; // TODO: pass in the old swapchin

    VK_CHECK(vkCreateSwapchainKHR(context->device.logical_device, &swapchain_create_info, context->allocator,
        &swapchain_out->swapchain_handle));

    //create the swapchain image and image view
    context->current_frame = 0;

    swapchain_out->image_count = 0;

    VK_CHECK(
        vkGetSwapchainImagesKHR(context->device.logical_device, swapchain_out->swapchain_handle, &swapchain_out->
            image_count,
            0));
    //NOTE: these might fail, idk why
    if (!swapchain_out->images)
    {
        swapchain_out->images = (VkImage *) malloc(sizeof(VkImage) * swapchain_out->image_count);
    }
    if (!swapchain_out->image_views)
    {
        swapchain_out->image_views = (VkImageView *) malloc(sizeof(VkImageView) * swapchain_out->image_count);
    }
    VK_CHECK(
        vkGetSwapchainImagesKHR(context->device.logical_device, swapchain_out->swapchain_handle, &swapchain_out->
            image_count, swapchain_out->images));


    // Image_views
    for (u32 i = 0; i < swapchain_out->image_count; ++i)
    {
        VkImageViewCreateInfo view_info = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
        view_info.image = swapchain_out->images[i];
        view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        view_info.format = swapchain_out->surface_format.format;
        view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        view_info.subresourceRange.baseMipLevel = 0;
        view_info.subresourceRange.levelCount = 1;
        view_info.subresourceRange.baseArrayLayer = 0;
        view_info.subresourceRange.layerCount = 1;

        VK_CHECK(
            vkCreateImageView(context->device.logical_device, &view_info, context->allocator, &swapchain_out->
                image_views[i]));
    }

    // Get our depth resources
    if (!vulkan_device_detect_depth_format(&context->device))
    {
        context->device.depth_format = VK_FORMAT_UNDEFINED;
        FATAL("Failed to find a supported depth format!");
    }

    // Create depth image and its view.
    vulkan_image_create(
        context,
        swapchain_extent.width,
        swapchain_extent.height,
        context->device.depth_format,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        TRUE,
        VK_IMAGE_ASPECT_DEPTH_BIT,
        &swapchain_out->depth_attachment);

    INFO("SWAPCHAIN CREATED");
}

void vulkan_swapchain_destroy(vulkan_context* context, vulkan_swapchain* swapchain)
{
    vkDeviceWaitIdle(context->device.logical_device);
    vulkan_image_destroy(context, &swapchain->depth_attachment);
    // Only destroy the views, not the images, since those are owned by the swapchain and are thus
    // destroyed when it is.
    for (u32 i = 0; i < swapchain->image_count; ++i)
    {
        vkDestroyImageView(context->device.logical_device, swapchain->image_views[i], context->allocator);
    }
    vkDestroySwapchainKHR(context->device.logical_device, swapchain->swapchain_handle, context->allocator);
    INFO("SWAPCHAIN DESTROYED");
}

void vulkan_swapchain_recreate(vulkan_context* context, u32 width, u32 height, vulkan_swapchain* swapchain)
{
    //destroy the old and create the new
    vulkan_swapchain_destroy(context, swapchain);
    vulkan_swapchain_create(context, width, height, swapchain);
}

bool vulkan_swapchain_acquire_next_image_index(vulkan_context* context, vulkan_swapchain* swapchain, u64 timeout_ns,
                                               VkSemaphore image_available_semaphore, VkFence fence_out,
                                               u32* out_image_index)
{
    VkResult result = vkAcquireNextImageKHR(context->device.logical_device, swapchain->swapchain_handle, timeout_ns,
                                            image_available_semaphore, fence_out, out_image_index);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        vulkan_swapchain_recreate(context, context->framebuffer_width, context->framebuffer_height, swapchain);
        return false;
    }
    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        FATAL("FAILED TO ACQUIRE SWAPCHAIN IMAGE!")
        return false;
    }

    return true;
}

void vulkan_swapchain_present(vulkan_context* context, vulkan_swapchain* swapchain, VkQueue graphics_queue,
                              VkQueue present_queue, VkSemaphore render_complete_semaphore, u32 present_image_index)
{
    VkPresentInfoKHR present_info = {};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &render_complete_semaphore;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &swapchain->swapchain_handle;
    present_info.pImageIndices = &present_image_index;
    present_info.pResults = 0;

    VkResult result = vkQueuePresentKHR(present_queue, &present_info);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
    {
        vulkan_swapchain_recreate(context, context->framebuffer_width, context->framebuffer_height, swapchain);
    }
    else if (result != VK_SUCCESS)
    {
        FATAL("FAILED TO PRESENT SWAPCHAIN IMAGE!")
    }


}

bool recreate_swapchain(vulkan_context* context)
{
    // If already being recreated, do not try again.
    if (context->recreating_swapchain)
    {
        DEBUG("recreate_swapchain called when already recreating. Booting.");
        return FALSE;
    }

    // Mark as recreating if the dimensions are valid.
    context->recreating_swapchain = TRUE;

    // Wait for any operations to complete.
    vkDeviceWaitIdle(context->device.logical_device);



    // Requery support
    vulkan_device_query_swapchain_support(
        context->device.physical_device,
        context->surface,
        &context->device.swapchain_capabilities);
    vulkan_device_detect_depth_format(&context->device);

    vulkan_swapchain_recreate(context,
        context->framebuffer_width_new, context->framebuffer_height_new,
        &context->swapchain);

    // Sync the framebuffer size with the new sizes.
    context->framebuffer_width = context->framebuffer_width_new;
    context->framebuffer_height = context->framebuffer_height_new;
    context->main_renderpass.screen_pos.z = context->framebuffer_width;
    context->main_renderpass.screen_pos.w = context->framebuffer_height;


    // cleanup swapchain
    for (u32 i = 0; i < context->swapchain.image_count; ++i)
    {
        vulkan_command_buffer_free(context, context->graphics_command_pool,
                                   &context->graphics_command_buffer[i]);
    }

    // Framebuffers.
    for (u32 i = 0; i < context->swapchain.image_count; ++i)
    {
        vulkan_framebuffer_destroy(context, &context->swapchain.framebuffers[i]);
    }

    context->main_renderpass.screen_pos.x = 0;
    context->main_renderpass.screen_pos.y = 0;
    context->main_renderpass.screen_pos.z = context->framebuffer_width;
    context->main_renderpass.screen_pos.w = context->framebuffer_height;

    regenerate_framebuffer(context, &context->swapchain, &context->main_renderpass);


    vulkan_renderer_command_buffers_create(context);

    // Clear the recreating flag.
    context->recreating_swapchain = FALSE;

    return TRUE;
}
