#include "renderer.h"

#include "camera.h"
#include "logger.h"
#include "vk_device.h"
#include "vk_renderpass.h"
#include "vk_command_buffer.h"
#include "vk_shader.h"
#include "vk_vertex_buffer.h"

//NOTE: static/global for now, most likely gonna move it into the renderer struct
static vulkan_context vk_context;

static camera camera_to_remove; // TODO: remove


bool renderer_init(struct renderer* renderer_inst)
{
    // camera_init(&camera_to_remove);
    camera_bad_init();

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
    if (!platform_create_vulkan_surface(renderer_inst->plat_state, &vk_context))
    {
        return FALSE;
    }

    //allow the window to resize at this point. NOTE: might want to move this to the end of init
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


    // Main Renderpass
    vulkan_renderpass_create(
        &vk_context,
        &vk_context.main_renderpass,
        (vec4){.x = 0.f, .y = 0.f, .z = vk_context.framebuffer_width, .w = vk_context.framebuffer_height},
        (vec4){.x = 0.f, .y = 0.f, .z = 0.2f, .w = 1.0f}, 1.0f, 0);


    // Swapchain framebuffers.
    vk_context.swapchain.framebuffers = darray_create_reserve(vulkan_framebuffer, vk_context.swapchain.image_count);
    regenerate_framebuffer(&vk_context, &vk_context.swapchain, &vk_context.main_renderpass);


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


    // Create default shader
    vulkan_default_shader_create(&vk_context, &vk_context.default_shader_info);


    for (u32 i = 0; i < vk_context.swapchain.image_count; i++)
    {
        create_vertex_and_indices_buffer(&vk_context, &vk_context.graphics_command_buffer[i],
                                         &vk_context.default_vertex_buffer,
                                         &vk_context.default_vertex_info);
    }

    uniform_buffers_create(&vk_context, &vk_context.default_shader_info.global_uniform_buffers);
    descriptor_pool_create(&vk_context, &vk_context.default_shader_info);
    descriptor_set_create(&vk_context, &vk_context.default_shader_info);

    //TODO: temporary
    memcpy(vk_context.default_vertex_info.vertices, test_vertices, sizeof(test_vertices));
    vk_context.default_vertex_info.vertices_size = sizeof(test_vertices) / sizeof(test_vertices[0]);
    memcpy(vk_context.default_vertex_info.indices, test_indices, sizeof(test_indices));
    vk_context.default_vertex_info.indices_size = sizeof(test_indices) / sizeof(test_indices[0]);
    INFO("VULKAN RENDERER INITIALIZED");

    return TRUE;
}

void renderer_update(struct renderer* renderer_inst)
{
    /*
      At a high level, rendering a frame in Vulkan consists of a common set of steps:
      Wait for the previous frame to finish
      Acquire an image from the swap chain
      Record a command buffer which draws the scene onto that image
      Submit the recorded command buffer
      Present the swap chain image
      */
    //semaphore orders queue operations (waiting happens on the GPU),
    //fences waits until all operations on the GPU are done, meant to sync CPU and GPU

    //NOTE: image index is for the swapchain/framebuffers, current frame is for the command buffers

    // Wait for the execution of the current frame to complete. The fence being free will allow this one to move on.
    if (!vulkan_fence_wait(
        &vk_context,
        &vk_context.in_flight_fences[vk_context.current_frame],
        UINT64_MAX))
    {
        WARN("In-flight fence wait failure!");
        return;
    }

    /* Acquire an image from the swap chain */
    // Pass along the semaphore that should signaled when this completes.
    // This same semaphore will later be waited on by the queue submission to ensure this image is available.
    u32 image_index;
    if (!vulkan_swapchain_acquire_next_image_index(
        &vk_context,
        &vk_context.swapchain,
        UINT64_MAX,
        vk_context.image_available_semaphores[vk_context.current_frame],
        0,
        &image_index))
    {
        //if it fails it could mean that the swapchain is recreating itself
        return;
    }

    //reset fence only when we successfully acquire the image/resize
    vulkan_fence_reset(&vk_context, &vk_context.in_flight_fences[vk_context.current_frame]);


    //World Update
    vertex_buffer_update(&vk_context, &vk_context.graphics_command_buffer[vk_context.current_frame],
                         &vk_context.default_vertex_buffer, &vk_context.default_vertex_info);

    //global uniform / projection matrix
    uniform_buffer_update(&vk_context, &vk_context.default_shader_info.global_uniform_buffers, vk_context.current_frame,
                          1.0f, &camera_to_remove);

    // Begin recording commands.
    vulkan_command_buffer* command_buffer_current_frame = &vk_context.graphics_command_buffer[vk_context.current_frame];
    vkResetCommandBuffer(command_buffer_current_frame->command_buffer_handle, 0);

    vulkan_command_buffer_begin(command_buffer_current_frame, false, false, false);


    // Begin the render pass.
    vulkan_renderpass_begin(
        command_buffer_current_frame,
        &vk_context.main_renderpass,
        vk_context.swapchain.framebuffers[image_index].framebuffer_handle);

    // Dynamic state
    VkViewport viewport;
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (f32) vk_context.framebuffer_width;
    viewport.height = -(f32) vk_context.framebuffer_height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    // Scissor
    VkRect2D scissor;
    scissor.offset.x = scissor.offset.y = 0;
    scissor.extent.width = vk_context.framebuffer_width;
    scissor.extent.height = vk_context.framebuffer_height;

    vkCmdSetViewport(command_buffer_current_frame->command_buffer_handle, 0, 1, &viewport);
    vkCmdSetScissor(command_buffer_current_frame->command_buffer_handle, 0, 1, &scissor);

    //Do Bindings and Draw
    vulkan_default_shader_pipeline_bind(&vk_context, &vk_context.default_shader_info.default_shader_pipeline);

    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(command_buffer_current_frame->command_buffer_handle, 0, 1,
                           &vk_context.default_vertex_buffer.vertex_buffer, offsets);

    vkCmdBindIndexBuffer(command_buffer_current_frame->command_buffer_handle,
                         vk_context.default_vertex_buffer.index_buffer, 0,
                         VK_INDEX_TYPE_UINT16);

    vkCmdBindDescriptorSets(command_buffer_current_frame->command_buffer_handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            vk_context.default_shader_info.default_shader_pipeline.pipeline_layout, 0, 1,
                            &vk_context.default_shader_info.descriptor_sets[vk_context.current_frame],
                            0, NULL);

    vkCmdDrawIndexed(command_buffer_current_frame->command_buffer_handle,
                     (u32)vk_context.default_vertex_info.indices_size,
                     1, 0, 0, 0);

    //END FRAME//


    // End renderpass
    vulkan_renderpass_end(command_buffer_current_frame, &vk_context.main_renderpass);

    //End DRAW COMMAND
    vulkan_command_buffer_end(command_buffer_current_frame);


    // Mark the image fence as in-use by this frame.
    // vk_context.images_in_flight[vk_context.image_index] = &vk_context.in_flight_fences[vk_context.current_frame];

    // Submit the queue and wait for the operation to complete.
    // Begin queue submission
    VkSubmitInfo submit_info = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
    // Command buffer(s) to be executed.
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer_current_frame->command_buffer_handle;
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
        FATAL("RENDER UPDATE: QUEUE SUBMISSION FAILED");
        return;
    }

    // End queue submission

    // Give the image back to the swapchain.
    vulkan_swapchain_present(
        &vk_context,
        &vk_context.swapchain,
        vk_context.device.graphics_queue,
        vk_context.device.present_queue,
        vk_context.queue_complete_semaphores[vk_context.current_frame],
        image_index);


    // Increment (and loop) the frame index.
    vk_context.current_frame = (vk_context.current_frame + 1) % vk_context.swapchain.max_frames_in_flight;
}


void renderer_shutdown(struct renderer* renderer_inst)
{
    vkDeviceWaitIdle(vk_context.device.logical_device);

    // Destroy in the opposite order of creation.

    //
    // uniform_buffers_destroy(&vk_context, &vk_context.global_uniform_buffers);
    // vulkan_default_shader_destroy(&vk_context, &vk_context.default_shader_pipelines,
    //                               &vk_context.default_shader_descriptor_set_layout);


    // vulkan_renderer_command_buffer_destroy(&vk_context); //bugged rn

    create_vertex_and_indices_destroy(&vk_context, &vk_context.default_vertex_buffer);


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


    // Command buffers
    for (u32 i = 0; i < vk_context.swapchain.image_count; ++i)
    {
        if (vk_context.graphics_command_buffer[i].command_buffer_handle)
        {
            vulkan_command_buffer_free(
                &vk_context,
                vk_context.graphics_command_pool,
                &vk_context.graphics_command_buffer[i]);
            vk_context.graphics_command_buffer[i].command_buffer_handle = 0;
        }
    }
    darray_free(vk_context.graphics_command_buffer);
    vk_context.graphics_command_buffer = 0;

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
    vk_context.framebuffer_width_new = width;
    vk_context.framebuffer_height_new = height;

    recreate_swapchain(&vk_context);
}
