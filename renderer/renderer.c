#include "renderer.h"

#include "logger.h"
#include "vk_device.h"
#include "vk_renderpass.h"
#include "vk_command_buffer.h"


//NOTE: static/global for now, most likely gonna move it into the renderer struct
static vulkan_context vk_context;


bool renderer_init(struct renderer* renderer_inst)
{
    vk_context.is_init = false;
    // vulkan_context vulkan_context;

    //get the size for the default window from the app config
    //if these aren't set we use 800/600 for default
    vk_context.framebuffer_width = (renderer_inst->app_config.start_width != 0)
                                       ? renderer_inst->app_config.start_width
                                       : 600;
    vk_context.framebuffer_height = (renderer_inst->app_config.start_height != 0)
                                        ? renderer_inst->app_config.start_height
                                        : 600;
    //set this as well
    vk_context.framebuffer_width_new = vk_context.framebuffer_width;
    vk_context.framebuffer_height_new = vk_context.framebuffer_height;

    //create the instance
    vulkan_instance_create(&vk_context);

    // get surface from the platform layer, needed before device creation
    DEBUG("Creating Vulkan surface...");
    if (!platform_create_vulkan_surface(renderer_inst->plat_state, &vk_context))
    {
        return FALSE;
    }

    //allow the window to resize at this point. NOTE: might want to flip this at the end of init
    vk_context.is_init = true;

    // Device creation
    if (!vulkan_device_create(&vk_context))
    {
        M_ERROR("Failed to create device!");
        return FALSE;
    }

    // Swapchain
    vulkan_swapchain_create(
        &vk_context,
        vk_context.framebuffer_width,
        vk_context.framebuffer_height,
        &vk_context.swapchain);

    vulkan_renderpass_create(
        &vk_context,
        &vk_context.main_renderpass,
        0, 0, vk_context.framebuffer_width, vk_context.framebuffer_height,
        0.0f, 0.0f, 0.2f, 1.0f,
        1.0f,
        0);

    // Swapchain framebuffers.
    vk_context.swapchain.framebuffers = darray_create_reserve(vulkan_framebuffer, vk_context.swapchain.image_count);
    regenerate_framebuffer(&vk_context, &vk_context.swapchain, &vk_context.main_renderpass);


    vulkan_renderer_command_buffers_create(&vk_context);

    // Create command buffers.
    vulkan_renderer_command_buffers_create(&vk_context);


    //TODO: move out into its own function
    // Create sync objects.
    vk_context.image_available_semaphores = darray_create_reserve(VkSemaphore,
                                                                  vk_context.swapchain.max_frames_in_flight);
    vk_context.queue_complete_semaphores =
            darray_create_reserve(VkSemaphore, vk_context.swapchain.max_frames_in_flight);
    vk_context.in_flight_fences = darray_create_reserve(vulkan_fence, vk_context.swapchain.max_frames_in_flight);

    for (u8 i = 0; i < vk_context.swapchain.max_frames_in_flight; ++i)
    {
        VkSemaphoreCreateInfo semaphore_create_info = {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
        vkCreateSemaphore(vk_context.device.logical_device, &semaphore_create_info, vk_context.allocator,
                          &vk_context.image_available_semaphores[i]);
        vkCreateSemaphore(vk_context.device.logical_device, &semaphore_create_info, vk_context.allocator,
                          &vk_context.queue_complete_semaphores[i]);

        // Create the fence in a signaled state, indicating that the first frame has already been "rendered".
        // This will prevent the application from waiting indefinitely for the first frame to render since it
        // cannot be rendered until a frame is "rendered" before it.
        vulkan_fence_create(&vk_context, true, &vk_context.in_flight_fences[i]);
    }

    // In flight fences should not yet exist at this point, so clear the list. These are stored in pointers
    // because the initial state should be 0, and will be 0 when not in use. Acutal fences are not owned
    // by this list.
    vk_context.images_in_flight = darray_create_reserve(vulkan_fence*, vk_context.swapchain.image_count);
    for (u32 i = 0; i < vk_context.swapchain.image_count; ++i)
    {
        vk_context.images_in_flight[i] = 0;
    }

    INFO("VULKAN RENDERER INITIALIZED");

    return TRUE;
}

void renderer_update(struct renderer* renderer_inst)
{
    // TRACE("renderer is running test");
    // Check if recreating swap chain and boot out.
    if (vk_context.recreating_swapchain)
    {
        VkResult result = vkDeviceWaitIdle(vk_context.device.logical_device);

        INFO("Recreating swapchain, booting.");
        return;
    }

    // Check if the framebuffer has been resized. If so, a new swapchain must be created.
    if (vk_context.framebuffer_size_generation != vk_context.framebuffer_last_generation)
    {
        VkResult result = vkDeviceWaitIdle(vk_context.device.logical_device);

        // If the swapchain recreation failed (because, for example, the window was minimized),
        // boot out before unsetting the flag.
        if (!recreate_swapchain(&vk_context))
        {
            INFO("recreate swapchain, failed.");
        }

        INFO("Resized, booting.");
        return;
    }

    // Wait for the execution of the current frame to complete. The fence being free will allow this one to move on.
    if (!vulkan_fence_wait(
        &vk_context,
        &vk_context.in_flight_fences[vk_context.current_frame],
        UINT64_MAX))
    {
        WARN("In-flight fence wait failure!");
        return;
    }

    // Acquire the next image from the swap chain. Pass along the semaphore that should signaled when this completes.
    // This same semaphore will later be waited on by the queue submission to ensure this image is available.
    if (!vulkan_swapchain_acquire_next_image_index(
        &vk_context,
        &vk_context.swapchain,
        UINT64_MAX,
        vk_context.image_available_semaphores[vk_context.current_frame],
        0,
        &vk_context.image_index))
    {
        return;
    }

    // Begin recording commands.
    vulkan_command_buffer* command_buffer = &vk_context.graphics_command_buffers[vk_context.image_index];
    vulkan_command_buffer_reset(command_buffer);
    vulkan_command_buffer_begin(command_buffer, FALSE, FALSE, FALSE);

    // Dynamic state
    VkViewport viewport;
    viewport.x = 0.0f;
    viewport.y = (f32) vk_context.framebuffer_height;
    viewport.width = (f32) vk_context.framebuffer_width;
    viewport.height = -(f32) vk_context.framebuffer_height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    // Scissor
    VkRect2D scissor;
    scissor.offset.x = scissor.offset.y = 0;
    scissor.extent.width = vk_context.framebuffer_width;
    scissor.extent.height = vk_context.framebuffer_height;

    vkCmdSetViewport(command_buffer->command_buffer_handle, 0, 1, &viewport);
    vkCmdSetScissor(command_buffer->command_buffer_handle, 0, 1, &scissor);

    vk_context.main_renderpass.w = vk_context.framebuffer_width;
    vk_context.main_renderpass.h = vk_context.framebuffer_height;

    // Begin the render pass.
    vulkan_renderpass_begin(
        command_buffer,
        &vk_context.main_renderpass,
        vk_context.swapchain.framebuffers[vk_context.image_index].framebuffer_handle);


    //END FRAME//

    // End renderpass
    vulkan_renderpass_end(command_buffer, &vk_context.main_renderpass);

    //begin DRAW COMMAND
    vulkan_command_buffer_end(command_buffer);

    // Make sure the previous frame is not using this image (i.e. its fence is being waited on)
    if (vk_context.images_in_flight[vk_context.image_index] != VK_NULL_HANDLE)
    {
        // was frame
        vulkan_fence_wait(
            &vk_context,
            vk_context.images_in_flight[vk_context.image_index],
            UINT64_MAX);
    }

    // Mark the image fence as in-use by this frame.
    vk_context.images_in_flight[vk_context.image_index] = &vk_context.in_flight_fences[vk_context.current_frame];

    // Reset the fence for use on the next frame
    vulkan_fence_reset(&vk_context, &vk_context.in_flight_fences[vk_context.current_frame]);

    // Submit the queue and wait for the operation to complete.
    // Begin queue submission
    VkSubmitInfo submit_info = {VK_STRUCTURE_TYPE_SUBMIT_INFO};

    // Command buffer(s) to be executed.
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer->command_buffer_handle;
    // The semaphore(s) to be signaled when the queue is complete.
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = &vk_context.queue_complete_semaphores[vk_context.current_frame];
    // Wait semaphore ensures that the operation cannot begin until the image is available.
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = &vk_context.image_available_semaphores[vk_context.current_frame];

    // Each semaphore waits on the corresponding pipeline stage to complete. 1:1 ratio.
    // VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT prevents subsequent colour attachment
    // writes from executing until the semaphore signals (i.e. one frame is presented at a time)
    VkPipelineStageFlags flags[1] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submit_info.pWaitDstStageMask = flags;

    VkResult result = vkQueueSubmit(
        vk_context.device.graphics_queue,
        1,
        &submit_info,
        vk_context.in_flight_fences[vk_context.current_frame].fence_handle);
    if (result != VK_SUCCESS)
    {
        return;
    }

    vulkan_command_buffer_update_submitted(command_buffer);
    // End queue submission

    // Give the image back to the swapchain.
    vulkan_swapchain_present(
        &vk_context,
        &vk_context.swapchain,
        vk_context.device.graphics_queue,
        vk_context.device.present_queue,
        vk_context.queue_complete_semaphores[vk_context.current_frame],
        vk_context.image_index);
}


/*void renderer_shutdown(struct renderer* renderer_inst)
{
    INFO("WAITING ON ANY DEVICES BEFORE SHUTDOWN...");
    vkDeviceWaitIdle(vk_context.device.logical_device);

    INFO("Renderer Shutting Down");
    // Sempaphores and Fences / sync objects
    for (u8 i = 0; i < vk_context.swapchain.max_frames_in_flight; ++i)
    {
        if (vk_context.image_available_semaphores[i])
        {
            vkDestroySemaphore(
                vk_context.device.logical_device,
                vk_context.image_available_semaphores[i],
                vk_context.allocator);
            vk_context.image_available_semaphores[i] = 0;
        }
        if (vk_context.queue_complete_semaphores[i])
        {
            vkDestroySemaphore(
                vk_context.device.logical_device,
                vk_context.queue_complete_semaphores[i],
                vk_context.allocator);
            vk_context.queue_complete_semaphores[i] = 0;
        }
        vulkan_fence_destroy(&vk_context, &vk_context.in_flight_fences[i]);
    }
    darray_free(vk_context.image_available_semaphores);
    vk_context.image_available_semaphores = 0;

    darray_free(vk_context.queue_complete_semaphores);
    vk_context.queue_complete_semaphores = 0;

    darray_free(vk_context.in_flight_fences);
    vk_context.in_flight_fences = 0;

    darray_free(vk_context.images_in_flight);
    vk_context.images_in_flight = 0;


    // Command buffers
    vulkan_renderer_command_buffer_destroy(&vk_context);

    //TODO: free command pools

    // Command buffers
    for (u32 i = 0; vk_context.swapchain.image_count; i++)
    {
        vulkan_framebuffer_destroy(&vk_context, &vk_context.swapchain.framebuffers[i]);
    }

    //renderpass
    vulkan_renderpass_destroy(&vk_context, &vk_context.main_renderpass);

    //swapchain
    vulkan_swapchain_destroy(&vk_context, &vk_context.swapchain);

    //device
    vulkan_device_destroy(&vk_context);

    //instance
    vulkan_instance_destroy(&vk_context);
}*/

void renderer_shutdown(struct renderer* renderer_inst)
{
    vkDeviceWaitIdle(vk_context.device.logical_device);

    // Destroy in the opposite order of creation.

    // Sync objects
    for (u8 i = 0; i < vk_context.swapchain.max_frames_in_flight; ++i)
    {
        if (vk_context.image_available_semaphores[i])
        {
            vkDestroySemaphore(
                vk_context.device.logical_device,
                vk_context.image_available_semaphores[i],
                vk_context.allocator);
            vk_context.image_available_semaphores[i] = 0;
        }
        if (vk_context.queue_complete_semaphores[i])
        {
            vkDestroySemaphore(
                vk_context.device.logical_device,
                vk_context.queue_complete_semaphores[i],
                vk_context.allocator);
            vk_context.queue_complete_semaphores[i] = 0;
        }
        vulkan_fence_destroy(&vk_context, &vk_context.in_flight_fences[i]);
    }
    darray_free(vk_context.image_available_semaphores);
    vk_context.image_available_semaphores = 0;

    darray_free(vk_context.queue_complete_semaphores);
    vk_context.queue_complete_semaphores = 0;

    darray_free(vk_context.in_flight_fences);
    vk_context.in_flight_fences = 0;

    darray_free(vk_context.images_in_flight);
    vk_context.images_in_flight = 0;

    // Command buffers
    for (u32 i = 0; i < vk_context.swapchain.image_count; ++i)
    {
        if (vk_context.graphics_command_buffers[i].command_buffer_handle)
        {
            vulkan_command_buffer_free(
                &vk_context,
                vk_context.device.graphics_command_pool,
                &vk_context.graphics_command_buffers[i]);
            vk_context.graphics_command_buffers[i].command_buffer_handle = 0;
        }
    }
    darray_free(vk_context.graphics_command_buffers);
    vk_context.graphics_command_buffers = 0;

    // Destroy framebuffers
    for (u32 i = 0; i < vk_context.swapchain.image_count; ++i)
    {
        vulkan_framebuffer_destroy(&vk_context, &vk_context.swapchain.framebuffers[i]);
    }

    // Renderpass
    vulkan_renderpass_destroy(&vk_context, &vk_context.main_renderpass);

    // Swapchain
    vulkan_swapchain_destroy(&vk_context, &vk_context.swapchain);

    DEBUG("Destroying Vulkan device...");
    vulkan_device_destroy(&vk_context);

    DEBUG("Destroying Vulkan surface...");
    if (vk_context.surface)
    {
        vkDestroySurfaceKHR(vk_context.instance, vk_context.surface, vk_context.allocator);
        vk_context.surface = 0;
    }

    DEBUG("Destroying Vulkan debugger...");
    if (vk_context.debug_messenger)
    {
        PFN_vkDestroyDebugUtilsMessengerEXT func =
                (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
                    vk_context.instance, "vkDestroyDebugUtilsMessengerEXT");
        func(vk_context.instance, vk_context.debug_messenger, vk_context.allocator);
    }

    DEBUG("Destroying Vulkan instance...");
    vkDestroyInstance(vk_context.instance, vk_context.allocator);
}



void renderer_on_resize(struct renderer* renderer_inst, u32 width, u32 height)
{
    if (!vk_context.is_init)
    {
        INFO("cant resize window yet, not initialized");
        return;
    };

    //NOTE: doesn't actually resize anything here, just flags the renderer for a resize
    INFO("VULKAN RENDERER RESIZE HAS BEEN CALLED: new width: %d, height: %d", width, height);
    vk_context.framebuffer_size_generation++;
    vk_context.framebuffer_width_new = width;
    vk_context.framebuffer_height_new = height;
}
