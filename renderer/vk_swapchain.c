#include "vk_swapchain.h"

#include "logger.h"
#include "vk_command_buffer.h"
#include "vk_device.h"
#include "vk_image.h"

void vulkan_swapchain_create(Renderer* renderer, u32 width, u32 height,
                             Vulkan_Swapchain* swapchain_out)
{
    VkExtent2D swapchain_extent = {width, height};


    //choose a swap surface format, that suits our needs
    bool found = false;

    for (u32 i = 0; i < renderer->swapchain_capabilities.format_count; i++)
    {
        VkSurfaceFormatKHR current_surface_format = renderer->swapchain_capabilities.formats[i];
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
        swapchain_out->surface_format = renderer->swapchain_capabilities.formats[0];
    }

    //best to use mailbox mode (best for games) or fifo(basically vsync) (is always supported)
    VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR;
    for (u32 i = 0; i < renderer->swapchain_capabilities.present_mode_count; i++)
    {
        VkPresentModeKHR current_present_mode = renderer->swapchain_capabilities.present_modes[i];
        if (current_present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            present_mode = current_present_mode;
            break;
        }
    }

    //requery swapchain support, needed if a device was changed or resolution was changed
    vulkan_device_query_swapchain_support(renderer->physical_device, renderer->surface,
                                          &renderer->swapchain_capabilities);

    //set the swapchain extend, in the event anything happens
    if (renderer->swapchain_capabilities.capabilities.currentExtent.width != UINT32_MAX)
    {
        swapchain_extent = renderer->swapchain_capabilities.capabilities.currentExtent;
    }
    VkExtent2D min = renderer->swapchain_capabilities.capabilities.minImageExtent;
    VkExtent2D max = renderer->swapchain_capabilities.capabilities.maxImageExtent;

    swapchain_extent.width = clamp_int(swapchain_extent.width, min.width, max.width);
    swapchain_extent.height = clamp_int(swapchain_extent.height, min.height, max.height);

    //TODO: we should have a clamp here to choose the smallest frame counts possible
    u32 image_count = renderer->swapchain_capabilities.capabilities.minImageCount;
    //NOTE: specs says if max image count is = 0, that means unlimited amount of images
    // we cant do the sceond check or we will get get 0-1 which wraps around to 255
    if (renderer->swapchain_capabilities.capabilities.maxImageCount > 0)
    {
        // in the event our image count is for some reason greater than the max allowed
        if (renderer->swapchain_capabilities.capabilities.minImageCount > 0 && image_count >
            renderer->swapchain_capabilities.capabilities.maxImageCount)
        {
            image_count = renderer->swapchain_capabilities.capabilities.maxImageCount;
        }
    }


    VkSwapchainCreateInfoKHR swapchain_create_info = {0};
    swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_create_info.surface = renderer->surface;
    // swapchain_create_info.preTransform = ; // queried from surface capabiltiies
    swapchain_create_info.minImageCount = image_count;
    swapchain_create_info.imageFormat = swapchain_out->surface_format.format;
    swapchain_create_info.imageColorSpace = swapchain_out->surface_format.colorSpace;
    swapchain_create_info.imageExtent = swapchain_extent;
    swapchain_create_info.imageArrayLayers = 1;
    swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // render to color buffer

    // Setup the queue family indices
    if (renderer->graphics_queue_index != renderer->present_queue_index)
    {
        // want images to be accessible by both queue families
        u32 queueFamilyIndices[] = {
            (u32)renderer->graphics_queue_index,
            (u32)renderer->present_queue_index
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
    swapchain_create_info.preTransform = renderer->swapchain_capabilities.capabilities.currentTransform;
    //compositing with the operating system, wont ever need this
    swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_create_info.presentMode = present_mode;
    swapchain_create_info.clipped = VK_TRUE; // dont render anything off the screen
    swapchain_create_info.oldSwapchain = 0; // TODO: pass in the old swapchin

    VkResult swapchain_create_result = vkCreateSwapchainKHR(renderer->logical_device, &swapchain_create_info,
                                                            renderer->vulkan_allocator,
                                                            &swapchain_out->swapchain_handle);
    VK_CHECK(swapchain_create_result)

    //create the swapchain image and image view

    // swapchain_out->image_count = 0;
    VkResult get_result =
        vkGetSwapchainImagesKHR(renderer->logical_device, swapchain_out->swapchain_handle, &swapchain_out->
                                image_count,
                                0);
    VK_CHECK(get_result);
    //NOTE: these might fail, idk why
    if (!swapchain_out->images)
    {
        swapchain_out->images = (VkImage*)malloc(sizeof(VkImage) * swapchain_out->image_count);
    }
    if (!swapchain_out->image_views)
    {
        swapchain_out->image_views = (VkImageView*)malloc(sizeof(VkImageView) * swapchain_out->image_count);
    }
    VK_CHECK(
        vkGetSwapchainImagesKHR(renderer->logical_device, swapchain_out->swapchain_handle, &swapchain_out->
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
        view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        VK_CHECK(
            vkCreateImageView(renderer->logical_device, &view_info, renderer->vulkan_allocator, &swapchain_out->
                image_views[i]));
    }

    // Get our depth resources
    if (!vulkan_device_detect_depth_stencil_format(renderer))
    {
        renderer->depth_format = VK_FORMAT_UNDEFINED;
        FATAL("Failed to find a supported depth format!");
    }

    // Create depth image and its view.
    vulkan_image_create(
        renderer,
        swapchain_extent.width,
        swapchain_extent.height,
        renderer->depth_format,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        true,
        VK_IMAGE_ASPECT_DEPTH_BIT,
        &swapchain_out->depth_attachment);

    INFO("SWAPCHAIN CREATED");
}

void vulkan_swapchain_destroy(Renderer* renderer, Vulkan_Swapchain* swapchain)
{
    vkDeviceWaitIdle(renderer->logical_device);
    vulkan_texture_free(renderer, &swapchain->depth_attachment);
    // Only destroy the views, not the images, since those are owned by the swapchain and are thus
    // destroyed when it is.
    for (u32 i = 0; i < swapchain->image_count; ++i)
    {
        vkDestroyImageView(renderer->logical_device, swapchain->image_views[i], renderer->vulkan_allocator);
    }
    vkDestroySwapchainKHR(renderer->logical_device, swapchain->swapchain_handle, renderer->vulkan_allocator);
    INFO("SWAPCHAIN DESTROYED");
}

void vulkan_swapchain_recreate(Renderer* renderer, u32 width, u32 height,
                               Vulkan_Swapchain* swapchain)
{
    //destroy the old and create the new
    vulkan_swapchain_destroy(renderer, swapchain);
    vulkan_swapchain_create(renderer, width, height, swapchain);
}

bool vulkan_swapchain_acquire_next_image_index(Renderer* renderer, Vulkan_Swapchain* swapchain,
                                               u64 timeout_ns, VkSemaphore image_available_semaphore,
                                               VkFence fence_out, u32* out_image_index)
{
    VkResult result = vkAcquireNextImageKHR(renderer->logical_device, swapchain->swapchain_handle, timeout_ns,
                                            image_available_semaphore, fence_out, out_image_index);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        vulkan_swapchain_recreate(renderer, renderer->framebuffer_width, renderer->framebuffer_height, swapchain);
        return false;
    }
    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        FATAL("FAILED TO ACQUIRE SWAPCHAIN IMAGE!")
        return false;
    }

    return true;
}

void vulkan_swapchain_present_image(Renderer* renderer,
                                    Vulkan_Swapchain* swapchain, VkQueue present_queue,
                                    VkSemaphore render_complete_semaphore, u32 present_image_index)
{
    VkPresentInfoKHR present_info = {0};
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
        vulkan_swapchain_recreate(renderer, renderer->framebuffer_width, renderer->framebuffer_height, swapchain);
    }
    else if (result != VK_SUCCESS)
    {
        FATAL("FAILED TO PRESENT SWAPCHAIN IMAGE!")
    }
}

bool recreate_swapchain(Renderer* renderer)
{
    // If already being recreated, do not try again.
    if (renderer->recreating_swapchain)
    {
        DEBUG("recreate_swapchain called when already recreating. Booting.");
        return false;
    }

    // Mark as recreating if the dimensions are valid.
    renderer->recreating_swapchain = true;

    // Wait for any operations to complete.
    vkDeviceWaitIdle(renderer->logical_device);


    // Requery support
    vulkan_device_query_swapchain_support(
        renderer->physical_device,
        renderer->surface,
        &renderer->swapchain_capabilities);
    vulkan_device_detect_depth_stencil_format(renderer);

    vulkan_swapchain_recreate(renderer,
                              renderer->framebuffer_width_new, renderer->framebuffer_height_new,
                              &renderer->swapchain);

    // Sync the framebuffer size with the new sizes.
    renderer->framebuffer_width = renderer->framebuffer_width_new;
    renderer->framebuffer_height = renderer->framebuffer_height_new;



    // cleanup swapchain

    /*NOTE:  idk why this is here in the old code?
    for (u32 i = 0; i < renderer->context.swapchain.image_count; ++i)
    {
        vulkan_command_buffer_free(&renderer->context, &renderer->context.graphics_command_buffer[i],
                                   renderer->context.graphics_command_pool);
    }
    vulkan_renderer_command_buffers_create(&renderer->context);
    */




    // Clear the recreating flag.
    renderer->recreating_swapchain = false;

    return true;
}
