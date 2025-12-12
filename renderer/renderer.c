#include "renderer.h"

#include "camera.h"
#include "darray.h"
#include "logger.h"
#include "vk_device.h"
#include "vk_renderpass.h"
#include "vk_command_buffer.h"
#include "vk_descriptors.h"
#include "vk_image.h"
#include "vk_shader.h"
#include "vk_vertex_buffer.h"


//NOTE: static/global for now, most likely gonna move it into the renderer struct
static vulkan_context vk_context;
static camera main_camera;


bool renderer_init(struct renderer* renderer_inst)
{


    //TODO: remove when done
    spriv_reflect_get_input_variable(NULL, "../renderer/shaders/shader_mesh.vert.spv");

    camera_init(&main_camera);
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
    descriptor_pool_allocator_init(&vk_context, &vk_context.global_descriptor_pool);


    //TODO: move out into its own function
    // Create sync objects.
    //NOTE: semaphores must be per swapchain image

    init_per_frame_sync(&vk_context);
    vk_context.current_frame = 0;

    // Create default shader
    createUniformBuffers(&vk_context);
    // uniform_buffers_create(&vk_context, &vk_context.default_shader_info.global_uniform_buffers);
    createDescriptors(&vk_context);
    vulkan_default_shader_create(&vk_context, &vk_context.default_shader_info);






    //TODO: temporary
    // memcpy(vk_context.default_vertex_info.vertices, test_vertices, sizeof(test_vertices));
    // vk_context.default_vertex_info.vertices_size = ARRAY_SIZE(test_vertices);
    // memcpy(vk_context.default_vertex_info.indices, test_indices, sizeof(test_indices));
    // vk_context.default_vertex_info.indices_size = ARRAY_SIZE(test_indices);
    createVertexBuffer(&vk_context, &vk_context.vertex_buffer, &vk_context.index_buffer,
                       &vk_context.default_vertex_info);

    //TEXTURE TRIANGLE

    create_texture_image(&vk_context, vk_context.graphics_command_buffer, "../renderer/texture/test_texture.jpg",
                          &vk_context.shader_texture.texture_test_object);


    //TODO: temp while testing it
    createDescriptorsTexture_reflect_test(&vk_context, &vk_context.global_descriptor_pool, &vk_context.shader_texture);
    // createDescriptorsTexture(&vk_context, &vk_context.shader_texture);

    vulkan_textured_shader_create(&vk_context, &vk_context.shader_texture);
    createVertexBufferTexture(&vk_context, &vk_context.shader_texture);


    mesh_load_gltf("../z_assets/models/cube_gltf/Cube.gltf");
    // mesh_load_gltf("../z_assets/models/damaged_helmet_gltf/DamagedHelmet.gltf");


    // for (u32 i = 0; i < vk_context.swapchain.image_count; i++)
    // {
    //     create_vertex_and_indices_buffer(&vk_context, &vk_context.graphics_command_buffer[i],
    //                                      &vk_context.default_vertex_buffer,
    //                                      &vk_context.default_vertex_info);
    // }


    // uniform_buffers_create(&vk_context, &vk_context.default_shader_info.global_uniform_buffers);
    // descriptor_pool_create(&vk_context, &vk_context.default_shader_info);
    // descriptor_set_create(&vk_context, &vk_context.default_shader_info);


    INFO("VULKAN RENDERER INITIALIZED");

    return TRUE;
}



static bool texture_flip = false;
void renderer_update(struct renderer* renderer_inst, Clock* clock)
{

    //TODO: test code, can remove later
    if (input_key_released_unique(KEY_M))
    {
        if (texture_flip)
        {
            create_texture_image(&vk_context, vk_context.graphics_command_buffer, "../renderer/texture/test_texture.jpg",
                      &vk_context.shader_texture.texture_test_object);
            updateDescriptorsTexture_reflect_test(&vk_context, &vk_context.global_descriptor_pool, &vk_context.shader_texture);

            texture_flip = !texture_flip;

        }
        else
        {
            create_texture_image(&vk_context, vk_context.graphics_command_buffer, "../renderer/texture/error_texture.png",
                               &vk_context.shader_texture.texture_test_object);
            updateDescriptorsTexture_reflect_test(&vk_context, &vk_context.global_descriptor_pool, &vk_context.shader_texture);
            texture_flip = !texture_flip;

        }
    }

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

    // Wait for the execution of the current frame to complete. The fence being free will allow this one to move on.
    if (!vulkan_fence_wait(
        &vk_context,
        &vk_context.queue_submit_fence[vk_context.current_frame],
        UINT64_MAX))
    {
        WARN("In-flight fence wait failure!");
        return;
    }

    /* Acquire an image from the swap chain */
    // Pass along the semaphore that should signaled when this completes.
    // This same semaphore will later be waited on by the queue submission to ensure this image is available.
    u32 image_index = 0;
    if (!vulkan_swapchain_acquire_next_image_index(
        &vk_context,
        &vk_context.swapchain,
        UINT64_MAX,
        vk_context.swapchain_acquire_semaphore[vk_context.current_frame],
        0,
        &image_index))
    {
        //if it fails it could mean that the swapchain is recreating itself
        return;
    }


    //World Update
    // vertex_buffer_update(&vk_context, &vk_context.graphics_command_buffer[vk_context.current_frame],
    //                      &vk_context.default_vertex_buffer, &vk_context.default_vertex_info);

    //global uniform / projection matrix
    // uniform_buffer_update(&vk_context, &vk_context.default_shader_info.global_uniform_buffers, vk_context.current_frame,
    //                       1.0f, &camera_to_remove);
    // Update the uniform buffer for the next frame

    camera_update(&main_camera, clock->delta_time);

    uniform_buffer_object ubo = {0};
    // quat q = quat_from_axis_angle(vec3_up(), deg_to_rad(90.0f) * clock->time_elapsed, true);
    // ubo.model = quat_to_rotation_matrix(quat_identity(), (vec3){0.0f, 0.0f, 0.0f});
    ubo.model = mat4_identity();
    // glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    // ubo.view = camera_get_view_matrix(&main_camera);
    ubo.view = camera_get_fps_view_matrix(&main_camera);
    // Perspective
    ubo.proj = camera_get_projection(&main_camera, vk_context.framebuffer_width, vk_context.framebuffer_height);

    // Copy the current matrices to the current frame's uniform buffer. As we requested a host coherent memory type for the uniform buffer, the write is instantly visible to the GPU.
    memcpy(vk_context.default_shader_info.global_uniform_buffers.uniform_buffers_mapped[vk_context.current_frame], &ubo,
           sizeof(uniform_buffer_object));


    // Begin recording commands.
    //TODO: might have to change to primary command buffer
    vulkan_command_buffer* command_buffer_current_frame = &vk_context.graphics_command_buffer[vk_context.current_frame];
    vkResetCommandBuffer(command_buffer_current_frame->handle, 0);
    vulkan_command_buffer_begin(command_buffer_current_frame, false, false, false);

    // With dynamic rendering we need to explicitly add layout transitions by using barriers, this set of barriers prepares the color and depth images for output
    insertImageMemoryBarrier(command_buffer_current_frame->handle,
                             vk_context.swapchain.images[image_index], 0, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                             VK_IMAGE_LAYOUT_UNDEFINED,
                             VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
                             VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                             VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                             (VkImageSubresourceRange){VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}
    );
    insertImageMemoryBarrier(command_buffer_current_frame->handle,
                             vk_context.swapchain.depth_attachment.handle, 0,
                             VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                             VK_IMAGE_LAYOUT_UNDEFINED,
                             VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
                             VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
                             VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
                             (VkImageSubresourceRange){
                                 VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, 0, 1, 0, 1
                             }
    );

    //
    // // Begin the render pass.
    // vulkan_renderpass_begin(
    //     command_buffer_current_frame,
    //     &vk_context.main_renderpass,
    //     vk_context.swapchain.framebuffers[image_index].framebuffer_handle);

    //with dynamic rendering we need to add a layout transition, using barriers


    // Set up the rendering attachment info
    VkRenderingAttachmentInfo color_attachment = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .imageView = vk_context.swapchain.image_views[image_index],
        .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .clearValue.color = {0.0f, 0.0f, 0.2f, 0.0f},
    };

    VkRenderingInfo rendering_info = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
        .renderArea = {
            .offset = {0, 0},
            .extent = {vk_context.framebuffer_width, vk_context.framebuffer_height}
        },
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments = &color_attachment,
    };
    vkCmdBeginRendering(command_buffer_current_frame->handle, &rendering_info);

    // Dynamic state
    VkViewport viewport = {
        0.0f, 0.0f, (f32) vk_context.framebuffer_width, (f32) vk_context.framebuffer_height, 0.0f, 1.0f
    };


    // Scissor
    VkRect2D scissor = {
        .offset = {.x = 0, .y = 0},
        .extent = {.width = vk_context.framebuffer_width, .height = vk_context.framebuffer_height},
    };


    vkCmdSetViewport(command_buffer_current_frame->handle, 0, 1, &viewport);
    vkCmdSetScissor(command_buffer_current_frame->handle, 0, 1, &scissor);


    //Do Bindings and Draw

    //Draw Triangle
    // vulkan_default_shader_pipeline_bind(command_buffer_current_frame, &vk_context.default_shader_info.default_shader_pipeline);
    vkCmdBindPipeline(command_buffer_current_frame->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      vk_context.default_shader_info.default_shader_pipeline.handle);

    // Bind descriptor set for the current frame's uniform buffer, so the shader uses the data from that buffer for this draw
    vkCmdBindDescriptorSets(command_buffer_current_frame->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            vk_context.default_shader_info.default_shader_pipeline.pipeline_layout, 0, 1,
                            &vk_context.default_shader_info.descriptor_sets[vk_context.current_frame], 0, 0);

    VkDeviceSize offsets[1] = {0};
    vkCmdBindVertexBuffers(command_buffer_current_frame->handle, 0, 1,
                           &vk_context.vertex_buffer.handle, offsets);

    vkCmdBindIndexBuffer(command_buffer_current_frame->handle,
                         vk_context.index_buffer.handle, 0,
                         VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(command_buffer_current_frame->handle,
                      (u32) vk_context.default_vertex_info.indices_size,
                      1, 0, 0, 0);

    //Draw Textured Triangle
    vkCmdBindPipeline(command_buffer_current_frame->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      vk_context.shader_texture.shader_texture_pipeline.handle);

    // Bind descriptor set for the current frame's uniform buffer, so the shader uses the data from that buffer for this draw
    vkCmdBindDescriptorSets(command_buffer_current_frame->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            vk_context.shader_texture.shader_texture_pipeline.pipeline_layout, 0, 1,
                            &vk_context.shader_texture.descriptor_sets[vk_context.current_frame],
                            0, 0);

    VkDeviceSize offsets2[1] = {0};
    vkCmdBindVertexBuffers(command_buffer_current_frame->handle, 0, 1,
                           &vk_context.shader_texture.vertex_buffer.handle, offsets2);

    vkCmdBindIndexBuffer(command_buffer_current_frame->handle,
                         vk_context.shader_texture.index_buffer.handle, 0,
                         VK_INDEX_TYPE_UINT32);

    vkCmdDrawIndexed(command_buffer_current_frame->handle,
                     (u32) vk_context.shader_texture.vertex_info.indices_size,
                     1, 0, 0, 0);
    //END FRAME//

    // Finish the current dynamic rendering section
    vkCmdEndRendering(command_buffer_current_frame->handle);

    // End renderpass
    // vulkan_renderpass_end(command_buffer_current_frame, &vk_context.main_renderpass);

    // This barrier prepares the color image for presentation, we don't need to care for the depth image
    insertImageMemoryBarrier(command_buffer_current_frame->handle,
                             vk_context.swapchain.images[image_index], VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, 0,
                             VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                             VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_2_NONE,
                             (VkImageSubresourceRange){VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});


    //End DRAW COMMAND
    vulkan_command_buffer_end(command_buffer_current_frame);

    // Submit the queue and wait for the operation to complete.
    // Begin queue submission
    VkSubmitInfo submit_info = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
    // Command buffer(s) to be executed.
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer_current_frame->handle;
    // Semaphore to wait upon before the submitted command buffer starts executing
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = &vk_context.swapchain_release_semaphore[image_index];
    // Semaphore to be signaled when command buffers have completed
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = &vk_context.swapchain_acquire_semaphore[vk_context.current_frame];
    // Each semaphore waits on the corresponding pipeline stage to complete. 1:1 ratio.
    // VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT prevents subsequent colour attachment
    // writes from executing until the semaphore signals (i.e. one frame is presented at a time)
    VkPipelineStageFlags flags[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submit_info.pWaitDstStageMask = flags;

    // Submit to the graphics queue passing a wait fence
    VkResult result = vkQueueSubmit(
        vk_context.device.graphics_queue,
        1,
        &submit_info,
        vk_context.queue_submit_fence[vk_context.current_frame]);
    VK_CHECK(result);

    // End queue submission

    // Give the image back to the swapchain.
    vulkan_swapchain_present_image(
        &vk_context,
        &vk_context.swapchain,
        vk_context.device.graphics_queue,
        vk_context.device.present_queue,
        vk_context.swapchain_release_semaphore[image_index],
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


    // Sync objects
    for (u8 i = 0; i < vk_context.swapchain.image_count; ++i)
    {
        VkCommandPool* primary_command_pool = VK_NULL_HANDLE;
        VkCommandBuffer* primary_command_buffer = VK_NULL_HANDLE;

        vkDestroySemaphore(
            vk_context.device.logical_device,
            vk_context.swapchain_acquire_semaphore[i],
            vk_context.allocator);

        vkDestroySemaphore(
            vk_context.device.logical_device,
            vk_context.swapchain_release_semaphore[i],
            vk_context.allocator);

        vkDestroyFence(vk_context.device.logical_device, vk_context.queue_submit_fence[i],
                       VK_NULL_HANDLE);


        //per frame command buffers
        vkFreeCommandBuffers(vk_context.device.logical_device, vk_context.primary_command_pool[i], 1,
                             &vk_context.primary_command_buffer[i]);
    }

    for (u8 i = 0; i < vk_context.swapchain.image_count; ++i)
    {
        vkDestroyCommandPool(vk_context.device.logical_device, vk_context.primary_command_pool[i], VK_NULL_HANDLE);
    }

    darray_free(vk_context.swapchain_acquire_semaphore);
    darray_free(vk_context.swapchain_release_semaphore);
    darray_free(vk_context.queue_submit_fence);
    darray_free(vk_context.primary_command_buffer);
    darray_free(vk_context.primary_command_pool);


    // Command buffers
    for (u32 i = 0; i < vk_context.swapchain.image_count; ++i)
    {
        if (vk_context.graphics_command_buffer[i].handle)
        {
            vulkan_command_buffer_free(
                &vk_context,
                vk_context.graphics_command_pool,
                &vk_context.graphics_command_buffer[i]);
            vk_context.graphics_command_buffer[i].handle = 0;
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
