#include "renderer.h"
#include "camera.h"
#include "lights.h"
#include "shader_system.h"
#include "vk_command_buffer.h"
#include "vk_descriptors.h"
#include "vk_framebuffer.h"
#include "vk_image.h"
#include "vk_pipeline.h"
#include "vk_renderpass.h"
#include "vk_shader.h"
#include "vk_sync.h"
#include "maths/math_lib.h"
#include "render_resource_backend.h"

//finally works, TODO: whatever the fuck the renderer needs from other applications
bool renderer_on_key(const event_type code, u32 sender, u32 listener_inst, event_context context)
{
    /*
    if (code == EVENT_KEY_RELEASED)
    {
        uint16_t key_code = context.data.u16[0];
        FATAL("released key %hu", key_code);
    }*/
    return false;
}


bool renderer_init(Renderer* renderer, Platform_State* platform_state, Platform_Config platform_config, Memory_System* memory_system, Input_System* input_system,
                   Event_System* event_system, Resource_System* resource_system)
{
    memset(renderer, 0, sizeof(Renderer));
    vulkan_context* vk_context = &renderer->context;

    MASSERT(input_system);
    MASSERT(event_system);
    MASSERT(resource_system);

    //grab the input system if its valid
    if (input_system)
    {
        renderer->input_system = input_system;
    }

    renderer->resource_system = resource_system; //reference

    event_register(event_system, EVENT_KEY_RELEASED, 0, renderer_on_key);


    //set up memory for the renderer
    u64 renderer_system_mem_requirement = GB(0.5);
    u64 frame_arena_mem_size = GB(0.5);

    renderer->mem_tracker = memory_system_get_memory_tracker(memory_system->memory_tracker_system,
                                                             STRING("RENDERER"),
                                                             (renderer_system_mem_requirement + frame_arena_mem_size));


    void* renderer_system_mem = memory_system_alloc(memory_system, renderer_system_mem_requirement);
    arena_init(&renderer->arena, renderer_system_mem, renderer_system_mem_requirement,
               renderer->mem_tracker);

    void* frame_arena_mem = memory_system_alloc(memory_system, renderer_system_mem_requirement);
    arena_init(&renderer->frame_arena, frame_arena_mem, frame_arena_mem_size, renderer->mem_tracker);

    // vulkan_context vk_context = renderer_internal.vulkan_context;


    camera_init(&renderer->main_camera);
    vk_context->is_init = false;
    // vulkan_context vulkan_context;

    //get the size for the default window from the app config
    //if these aren't set we use 800/600 for default
    vk_context->framebuffer_width = (platform_config.start_width != 0)
                                        ? platform_config.start_width
                                        : 600;
    vk_context->framebuffer_height = (platform_config.start_height != 0)
                                         ? platform_config.start_height
                                         : 600;
    //set this as well
    vk_context->framebuffer_width_new = vk_context->framebuffer_width;
    vk_context->framebuffer_height_new = vk_context->framebuffer_height;

    renderer->mode = RENDER_MODE_NONE;

    //create the instance
    vulkan_instance_create(vk_context);

    // get surface from the platform layer, needed before device creation
    if (!platform_create_vulkan_surface(platform_state, vk_context))
    {
        return false;
    }

    //allow the window to resize at this point. NOTE: might want to move this to the end of init
    vk_context->is_init = true;

    // Device creation
    if (!vulkan_device_create(vk_context))
    {
        M_ERROR("Failed to create device!");
        return false;
    }

    // Swapchain
    vulkan_swapchain_create(
        vk_context,
        vk_context->framebuffer_width,
        vk_context->framebuffer_height,
        &vk_context->swapchain);
    // Main Renderpass
    vulkan_renderpass_create(
        vk_context,
        &vk_context->main_renderpass,
        (vec4){.x = 0.f, .y = 0.f, .z = vk_context->framebuffer_width, .w = vk_context->framebuffer_height},
        (vec4){.x = 0.f, .y = 0.f, .z = 0.2f, .w = 1.0f}, 1.0f, 0);

    // Swapchain framebuffers.
    vk_context->swapchain.framebuffers = darray_create_reserve(vulkan_framebuffer, vk_context->swapchain.image_count);
    regenerate_framebuffer(vk_context, &vk_context->swapchain, &vk_context->main_renderpass);


    // Create command buffers.
    vulkan_renderer_command_buffers_create(vk_context);

    //NOTE: semaphores must be per swapchain image
    init_per_frame_sync(vk_context);
    vk_context->current_frame = 0;


    // Create Descriptor Pool
    renderer->descriptor_system = descriptor_pool_allocator_init(renderer);


    //BUFFER SYSTEM
    renderer->buffer_system = buffer_system_init(renderer,
                                                 renderer->context.swapchain.max_frames_in_flight);
    /*for (u32 i = 0; i < renderer_internal.buffer_system->frames_in_flight; i++)
    {
        Buffer_Handle temp_buffer_handle = renderer_internal.buffer_system->global_ubo_handle;
        temp_buffer_handle.handle += i;
        update_uniform_buffer_bindless_descriptor_set(
            &renderer_internal, renderer_internal.descriptor_system, temp_buffer_handle, 0);
    }*/

    //Shader System
    renderer->shader_system = shader_system_init(renderer);
    // Light System
    renderer->light_system = light_system_init(renderer);

    //System specific draws
    // Mesh System
    renderer->mesh_renderer = mesh_renderer_init(renderer, renderer->resource_system);
    // Sprite Backend
    renderer->sprite_renderer = sprite_render_init(renderer, renderer->resource_system);
    // UI Backend
    renderer->ui_renderer = ui_render_init(renderer);


    renderer->pipeline_cache = vulkan_pipeline_cache_initialize(renderer);

    //Pipelines
    ui_shader_create(renderer, &renderer->ui_pipeline, renderer->pipeline_cache);
    text_shader_create(renderer, &renderer->text_pipeline, renderer->pipeline_cache);
    sprite_shader_create(renderer, &renderer->sprite_pipeline, renderer->pipeline_cache);
    mesh_indirect_shader_create(renderer, &renderer->indirect_mesh_pipeline,
                                renderer->pipeline_cache);
    //Pipeline Cache
    vulkan_pipeline_cache_write_to_file(renderer, renderer->pipeline_cache);


    INFO("VULKAN RENDERER INITIALIZED");


    return true;
}


static bool texture_flip = false;

void renderer_update(Renderer* renderer, float delta_time)
{
    Render_Packet* render_packets = renderer->resource_system->render_packet;

    vulkan_context* vk_context = &renderer->context;

    arena_clear(&renderer->frame_arena);

    //TODO: have the render queues be in the render packet
    shader_system_load_textures_into_gpu(renderer, renderer->shader_system, renderer->descriptor_system,
                                         render_packets);


    //TODO: test code, can remove later
    if (input_key_released_unique(renderer->input_system, KEY_U))
    {
        //TODO: MOVE OUT LATER
        renderer->mode = (renderer->mode + 1) % RENDER_MODE_MAX;
        FATAL("RENDER_MODE: %d", renderer->mode)
        if (texture_flip)
        {
            /* TODO:
            create_texture_image(&vk_context, vk_context.graphics_command_buffer,
                                 "../renderer/texture/test_texture.jpg",
                                 &vk_context.shader_texture.texture_test_object);
            update_descriptors_texture_reflect_test(&vk_context, &vk_context.global_descriptor_pool,
                                                    &vk_context.shader_texture);
            */

            texture_flip = !texture_flip;
        }
        else
        {
            /*
            create_texture_image(&vk_context, vk_context.graphics_command_buffer,
                                 "../renderer/texture/error_texture.png",
                                 &vk_context.shader_texture.texture_test_object);
            update_descriptors_texture_reflect_test(&vk_context, &vk_context.global_descriptor_pool,
                                                    &vk_context.shader_texture);
            */
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
        vk_context,
        &vk_context->queue_submit_fence[vk_context->current_frame],
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
        vk_context,
        &vk_context->swapchain,
        UINT64_MAX,
        vk_context->swapchain_acquire_semaphore[vk_context->current_frame],
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

    camera_update(renderer->input_system, &renderer->main_camera, delta_time);

    uniform_buffer_object ubo = {0};
    // quat q = quat_from_axis_angle(vec3_up(), deg_to_rad(90.0f) * clock->time_elapsed, true);
    // ubo.model = quat_to_rotation_matrix(quat_identity(), (vec3){0.0f, 0.0f, 0.0f});
    ubo.model = mat4_identity();
    // glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    // ubo.view = camera_get_view_matrix(&main_camera);
    ubo.view = camera_get_fps_view_matrix(&renderer->main_camera);
    // Perspective
    ubo.proj = camera_get_projection(&renderer->main_camera, vk_context->framebuffer_width,
                                     vk_context->framebuffer_height);


    VkDeviceAddress directional_light_buffer_address = get_buffer_device_address(vk_context->device.logical_device,
        vulkan_buffer_get(renderer,
                          renderer->light_system->directional_light_storage_buffer_handle)->handle);

    VkDeviceAddress point_light_buffer_address = get_buffer_device_address(vk_context->device.logical_device,
                                                                           vulkan_buffer_get(renderer,
                                                                               renderer->light_system->
                                                                               point_light_storage_buffer_handle)->
                                                                           handle);

    ubo.directional_lights_address = directional_light_buffer_address;
    ubo.point_lights_address = point_light_buffer_address;
    ubo.point_lights_count = renderer->light_system->point_light_count;
    ubo.camera_position = renderer->main_camera.viewPos;
    ubo.render_mode = renderer->mode;

    // Copy the current matrices to the current frame's uniform buffer. As we requested a host coherent memory type for the uniform buffer, the write is instantly visible to the GPU.
    vulkan_buffer* ubo_buffer = vulkan_buffer_get(renderer,
                                                  renderer->buffer_system->global_ubo_handle);
    memcpy(ubo_buffer->mapped_data, &ubo,
           sizeof(uniform_buffer_object));

    mesh_system_upload_draw_data(renderer, renderer->mesh_renderer, render_packets);
    sprite_upload_draw_data(renderer, renderer->sprite_renderer, &render_packets->sprite_data_packet);
    ui_renderer_upload_draw_data(renderer->ui_renderer, renderer, render_packets);


    // Begin recording commands.
    //TODO: might have to change to primary command buffer
    vulkan_command_buffer* command_buffer_current_frame = &vk_context->graphics_command_buffer[vk_context->
        current_frame];
    vkResetCommandBuffer(command_buffer_current_frame->handle, 0);
    vulkan_command_buffer_begin(command_buffer_current_frame, false, false, false);

    // With dynamic rendering we need to explicitly add layout transitions by using barriers, this set of barriers prepares the color and depth images for output
    image_insert_memory_barrier(command_buffer_current_frame->handle,
                                vk_context->swapchain.images[image_index], 0, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                                VK_IMAGE_LAYOUT_UNDEFINED,
                                VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
                                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                (VkImageSubresourceRange){VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}
    );
    image_insert_memory_barrier(command_buffer_current_frame->handle,
                                vk_context->swapchain.depth_attachment.texture_image, 0,
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
        .imageView = vk_context->swapchain.image_views[image_index],
        .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .clearValue.color = {0.0f, 0.0f, 0.2f, 0.0f},
    };

    VkRenderingInfo rendering_info = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
        .renderArea = {
            .offset = {0, 0},
            .extent = {vk_context->framebuffer_width, vk_context->framebuffer_height}
        },
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments = &color_attachment,
    };
    vkCmdBeginRendering(command_buffer_current_frame->handle, &rendering_info);

    // Dynamic state
    VkViewport viewport = {
        0.0f, 0.0f, (f32)vk_context->framebuffer_width, (f32)vk_context->framebuffer_height, 0.0f, 1.0f
    };


    // Scissor
    VkRect2D scissor = {
        .offset = {.x = 0, .y = 0},
        .extent = {.width = vk_context->framebuffer_width, .height = vk_context->framebuffer_height},
    };


    vkCmdSetViewport(command_buffer_current_frame->handle, 0, 1, &viewport);
    vkCmdSetScissor(command_buffer_current_frame->handle, 0, 1, &scissor);


    //Do Bindings and Draw

    //TODO: WE CAN BIND THE DESCRIPTOR SETS BEFORE EVERYTHING AS LONG AS THE SET VALUES UP TO N ARE THE SAME
    // SET 0 GLOBAL UNIFORMS: CAMERA LIGHTS ETC
    // SET 1 GLOBAL TEXTURES: Textures
    // Everything after this is based on the pipeline and needs to be binded individually
    // SET 2 GLOBAL BUFFERS: Mesh/Material values (just as an example)
    // SET 2 GLOBAL BUFFERS: FOG/BLOOM/IDK (just as an example)

    //TODO: MESH LAYOUT
    // SET 0: UNIFORM
    // SET 1: TEXTURE
    // THE ABOVE IS FOR EVERYTHING
    // DRAW LOOP:
    //      bind pipeline
    //      bind vertex and index buffer
    //      for(pipeline_mask):
    //            bind and update ubo with pipeline mask
    //            SET 2 : MESH SHADER DATA (push constants / ubo)
    //            DRAW(INDIRECT) (immediate mode, that batches them by type per frame)

    mesh_system_draw(renderer, renderer->mesh_renderer, command_buffer_current_frame,
                     &renderer->indirect_mesh_pipeline);

    sprite_draw(renderer, renderer->sprite_renderer, command_buffer_current_frame);

    ui_renderer_draw(renderer->ui_renderer, renderer, command_buffer_current_frame, render_packets);


    // vkCmdDrawIndexedIndirect()
    //END FRAME//

    // Finish the current dynamic rendering section
    vkCmdEndRendering(command_buffer_current_frame->handle);


    // This barrier prepares the color image for presentation, we don't need to care for the depth image
    image_insert_memory_barrier(command_buffer_current_frame->handle,
                                vk_context->swapchain.images[image_index], VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, 0,
                                VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_2_NONE,
                                (VkImageSubresourceRange){VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});

    // End renderpass


    // vulkan_renderpass_UI_begin(&renderer_internal, command_buffer_current_frame, image_index);
    // ui_system_draw(&renderer_internal, UI_System_internal, command_buffer_current_frame);
    // vulkan_renderpass_UI_end(&renderer_internal, command_buffer_current_frame, image_index);


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
    submit_info.pSignalSemaphores = &vk_context->swapchain_release_semaphore[image_index];
    // Semaphore to be signaled when command buffers have completed
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = &vk_context->swapchain_acquire_semaphore[vk_context->current_frame];
    // Each semaphore waits on the corresponding pipeline stage to complete. 1:1 ratio.
    // VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT prevents subsequent colour attachment
    // writes from executing until the semaphore signals (i.e. one frame is presented at a time)
    VkPipelineStageFlags flags[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submit_info.pWaitDstStageMask = flags;

    // Submit to the graphics queue passing a wait fence
    VkResult result = vkQueueSubmit(
        vk_context->device.graphics_queue,
        1,
        &submit_info,
        vk_context->queue_submit_fence[vk_context->current_frame]);
    VK_CHECK(result);

    // End queue submission

    // Give the image back to the swapchain.
    vulkan_swapchain_present_image(
        vk_context,
        &vk_context->swapchain,
        vk_context->device.graphics_queue,
        vk_context->device.present_queue,
        vk_context->swapchain_release_semaphore[image_index],
        image_index);


    // Increment (and loop) the frame index.
    vk_context->current_frame = (vk_context->current_frame + 1) % vk_context->swapchain.max_frames_in_flight;
}


void renderer_shutdown(Renderer* renderer)
{
    vulkan_context* vk_context = &renderer->context;

    // vulkan_context vk_context = renderer_internal.vulkan_context;


    vkDeviceWaitIdle(vk_context->device.logical_device);

    // Destroy in the opposite order of creation.

    //
    // uniform_buffers_destroy(&vk_context, &vk_context.global_uniform_buffers);
    // vulkan_default_shader_destroy(&vk_context, &vk_context.default_shader_pipelines,
    //                               &vk_context.default_shader_descriptor_set_layout);


    // Sync objects
    for (u8 i = 0; i < vk_context->swapchain.image_count; ++i)
    {
        VkCommandPool* primary_command_pool = VK_NULL_HANDLE;
        VkCommandBuffer* primary_command_buffer = VK_NULL_HANDLE;

        vkDestroySemaphore(
            vk_context->device.logical_device,
            vk_context->swapchain_acquire_semaphore[i],
            vk_context->allocator);

        vkDestroySemaphore(
            vk_context->device.logical_device,
            vk_context->swapchain_release_semaphore[i],
            vk_context->allocator);

        vkDestroyFence(vk_context->device.logical_device, vk_context->queue_submit_fence[i],
                       VK_NULL_HANDLE);


        //per frame command buffers
        vkFreeCommandBuffers(vk_context->device.logical_device, vk_context->primary_command_pool[i], 1,
                             &vk_context->primary_command_buffer[i]);
    }

    for (u8 i = 0; i < vk_context->swapchain.image_count; ++i)
    {
        vkDestroyCommandPool(vk_context->device.logical_device, vk_context->primary_command_pool[i], VK_NULL_HANDLE);
    }

    darray_free(vk_context->swapchain_acquire_semaphore);
    darray_free(vk_context->swapchain_release_semaphore);
    darray_free(vk_context->queue_submit_fence);
    darray_free(vk_context->primary_command_buffer);
    darray_free(vk_context->primary_command_pool);


    // Command buffers
    for (u32 i = 0; i < vk_context->swapchain.image_count; ++i)
    {
        if (vk_context->graphics_command_buffer[i].handle)
        {
            vulkan_command_buffer_free(
                vk_context,
                vk_context->graphics_command_pool,
                &vk_context->graphics_command_buffer[i]);
            vk_context->graphics_command_buffer[i].handle = 0;
        }
    }
    darray_free(vk_context->graphics_command_buffer);
    vk_context->graphics_command_buffer = 0;

    // Destroy framebuffers
    for (u32 i = 0; i < vk_context->swapchain.image_count; ++i)
    {
        vulkan_framebuffer_destroy(vk_context, &vk_context->swapchain.framebuffers[i]);
    }

    // Renderpass
    vulkan_renderpass_destroy(vk_context, &vk_context->main_renderpass);

    // Swapchain
    vulkan_swapchain_destroy(vk_context, &vk_context->swapchain);

    DEBUG("Destroying Vulkan device...");
    vulkan_device_destroy(vk_context);

    DEBUG("Destroying Vulkan surface...");
    if (vk_context->surface)
    {
        vkDestroySurfaceKHR(vk_context->instance, vk_context->surface, vk_context->allocator);
        vk_context->surface = 0;
    }

    DEBUG("Destroying Vulkan debugger...");
    if (vk_context->debug_messenger)
    {
        PFN_vkDestroyDebugUtilsMessengerEXT func =
            (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
                vk_context->instance, "vkDestroyDebugUtilsMessengerEXT");
        func(vk_context->instance, vk_context->debug_messenger, vk_context->allocator);
    }

    DEBUG("Destroying Vulkan instance...");
    vkDestroyInstance(vk_context->instance, vk_context->allocator);


    INFO("RENDERER SHUTDOWN");
}


void renderer_on_resize(Renderer* renderer, u32 width, u32 height)
{
    // vulkan_context vk_context = renderer_internal.vulkan_context;
    vulkan_context* vk_context = &renderer->context;


    if (!vk_context->is_init)
    {
        INFO("cant resize window yet, not initialized");
        return;
    };

    //NOTE: doesn't actually resize anything here, just flags the renderer for a resize
    INFO("VULKAN RENDERER RESIZE HAS BEEN CALLED: new width: %d, height: %d", width, height);
    vk_context->framebuffer_width_new = width;
    vk_context->framebuffer_height_new = height;

    recreate_swapchain(vk_context);
}
