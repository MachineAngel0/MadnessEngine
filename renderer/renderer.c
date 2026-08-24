#include "renderer.h"
#include "camera.h"
#include "lights.h"
#include "shader_system.h"
#include "vk_command_buffer.h"
#include "vk_descriptors.h"
#include "vk_image.h"
#include "vk_pipeline.h"
#include "vk_renderpass.h"
#include "vk_shader.h"
#include "vk_sync.h"
#include "maths/math_lib.h"
#include "pipelines/sprite_render.h"
#include "../renderer/pipelines/ui_render.h"
#include "vulkan_mesh_system.h"
#include "../renderer/pipelines/particle_render.h"

bool renderer_on_key(const Event_Type code, String sender, String listener_inst, Event_Data context)
{
    /*
    if (code == EVENT_KEY_RELEASED)
    {
        uint16_t key_code = context.data.u16[0];
        FATAL("released key %hu", key_code);
    }*/
    return false;
}


Renderer* renderer_init(Platform_State* platform_state, Platform_Config platform_config, Memory_System* memory_system,
                        Input_System* input_system)
{
    MASSERT(platform_state);
    MASSERT(memory_system);
    MASSERT(input_system);
    MASSERT(event_system);


    Renderer* renderer = memory_system_alloc(memory_system, sizeof(Renderer), MEMORY_SUBSYSTEM_RENDERER);
    memset(renderer, 0, sizeof(Renderer));


    //grab the input system if its valid
    if (input_system)
    {
        renderer->input_system = input_system;
    }


    event_register(EVENT_KEY_RELEASED, STRING("RENDERER"), renderer_on_key);


    //set up memory for the renderer
    u64 allocator_renderer_mem_size = MB(256);
    u64 frame_arena_mem_size = MB(256);
    u64 heap_mem_size = MB(16);


    void* renderer_system_mem = memory_system_alloc(memory_system, allocator_renderer_mem_size,
                                                    MEMORY_SUBSYSTEM_RENDERER);
    allocator_init(&renderer->allocator, renderer_system_mem, allocator_renderer_mem_size);

    void* frame_arena_mem = memory_system_alloc(memory_system, allocator_renderer_mem_size,
                                                MEMORY_SUBSYSTEM_RENDERER);
    allocator_init(&renderer->frame_allocator, frame_arena_mem, frame_arena_mem_size);


    renderer->heap_allocator = memory_system_heap_allocator_create(memory_system, heap_mem_size,
                                                                   MEMORY_SUBSYSTEM_RENDERER);


    // vulkan_context vk_context = renderer_internal.vulkan_context;


    camera_init(&renderer->main_camera);
    renderer->is_init = false;
    // vulkan_context vulkan_context;

    //get the size for the default window from the app config
    //if these aren't set we use 800/600 for default
    renderer->framebuffer_width = (platform_config.start_width != 0)
                                      ? platform_config.start_width
                                      : 600;
    renderer->framebuffer_height = (platform_config.start_height != 0)
                                       ? platform_config.start_height
                                       : 600;
    //set this as well
    renderer->framebuffer_width_new = renderer->framebuffer_width;
    renderer->framebuffer_height_new = renderer->framebuffer_height;

    renderer->mode = RENDER_MODE_NONE;

    //TODO: dont really need rn and its only for host (cpu) memory
    // renderer->vk_allocator_callback = allocator_alloc(&renderer->allocator, sizeof(VkAllocationCallbacks));
    // vulkan_allocator_init(renderer);

    //create the instance
    if (!vulkan_instance_create(renderer))
    {
        M_ERROR("Failed to create vulkan instance!");
        return false;
    }
    // get surface from the platform layer, needed before device creation
    if (!platform_create_vulkan_surface(platform_state, renderer))
    {
        M_ERROR("Failed to find vulkan surface from platform!");
        return false;
    }

    //allow the window to resize at this point. NOTE: might want to move this to the end of init
    renderer->is_init = true;


    // Device creation
    if (!vulkan_device_create2(renderer))
    {
        M_ERROR("Failed to create vulkan device!");
        return false;
    }


    if (renderer->framebuffer_width != renderer->swapchain_capabilities.capabilities.currentExtent.width)
    {
        renderer->framebuffer_width = renderer->swapchain_capabilities.capabilities.currentExtent.width;
        renderer->framebuffer_width_new = renderer->swapchain_capabilities.capabilities.currentExtent.width;
    }
    if (renderer->framebuffer_height != renderer->swapchain_capabilities.capabilities.currentExtent.height)
    {
        renderer->framebuffer_height = renderer->swapchain_capabilities.capabilities.currentExtent.height;
        renderer->framebuffer_height_new = renderer->swapchain_capabilities.capabilities.currentExtent.
                                                     height;
    }


    //NOTE: semaphores must be per swapchain image, fence is per frame in flight
    renderer->current_frame = 0;
    renderer->max_frames_in_flight = VULKAN_MAX_FRAMES_IN_FLIGHT;

    // Swapchain
    vulkan_swapchain_create(
        renderer,
        renderer->framebuffer_width,
        renderer->framebuffer_height,
        &renderer->swapchain);

    vulkan_renderpass_create_new(renderer);

    //SHADOW PASS TEXTURE
    vulkan_texture_create_shadowmap(renderer, 1024, 1024, renderer->depth_format,
                                    &renderer->shadowpass_texture);

    // Create command buffers.
    renderer->queue_system = vulkan_queue_system_init(renderer);


    // Create Descriptor Pool
    renderer->descriptor_system = descriptor_pool_allocator_init(renderer);


    //BUFFER SYSTEM
    renderer->buffer_system = buffer_system_init(renderer,
                                                 renderer->max_frames_in_flight);

    renderer->pipeline_cache = vulkan_pipeline_cache_initialize(renderer);

    // Texture System
    renderer->texture_system = vulkan_texture_system_init(renderer);
    //Shader System
    renderer->shader_system = shader_system_init(renderer);
    // Light System
    renderer->light_system = light_system_init(renderer);

    //System specific draws
    // Mesh System
    renderer->mesh_system = mesh_renderer_init(renderer);

    //Particle System
    renderer->particle_render = particle_renderer_init(renderer);

    // Sprite Backend
    renderer->sprite_renderer = sprite_render_init(renderer);
    // UI Backend
    renderer->ui_renderer = ui_render_init(renderer);


    //Pipelines
    ui_shader_create(renderer, &renderer->ui_pipeline, renderer->pipeline_cache);
    text_shader_create(renderer, &renderer->text_pipeline, renderer->pipeline_cache);
    sprite_shader_create(renderer, &renderer->sprite_pipeline, renderer->pipeline_cache);

    //predepth pipeline
    vulkan_pipeline_predepth_create(renderer, "depth_mesh", &renderer->predepth_mesh_pipeline);
    vulkan_pipeline_predepth_create(renderer, "depth_skinned_mesh", &renderer->predepth_skinned_mesh_pipeline);


    //Pipeline Cache
    vulkan_pipeline_cache_write_to_file(renderer, renderer->pipeline_cache);


    INFO("VULKAN RENDERER INITIALIZED");


    return renderer;
}


void renderer_update(Renderer* renderer, float delta_time, Render_Packet* render_packets)
{
    MASSERT(renderer);

    //NOTE: only duplicate resources which are uploaded from the cpu to the gpu, not anything that lives on the gpu like textures
    // this includes, command buffers, sync objects, and cpu side uniform buffers
    // things that should not are, storage buffers, textures, descriptor sets, pipelines, render passes,


    /*
      At a high level, rendering a frame in Vulkan consists of a common set of steps:
      Wait for the frame we are on to finish
      Acquire an image from the swap chain
      Record a command buffer which draws the scene onto that image
      Submit the recorded command buffer
      Present the swap chain image
      */

    //semaphore orders queue operations (waiting happens on the GPU),
    //fences waits until all operations on the GPU are done, meant to sync CPU and GPU


    // Wait for the execution of the current frame to complete on the cpu. The fence being free will allow this one to move on.
    vulkan_queue_system_graphics_fence_wait(renderer, renderer->current_frame);


    /* Acquire an image from the swap chain */
    // Pass along the semaphore that should signal when this completes.
    // This same semaphore will later be waited on by the queue submission to ensure this image is available.
    u32 image_index = 0;
    if (!vulkan_swapchain_acquire_next_image_index(
        renderer,
        &renderer->swapchain,
        UINT64_MAX,
        renderer->queue_system->graphics_render_queue.swapchain_wait_semaphore[renderer->current_frame],
        0,
        &image_index))
    {
        //if it fails it could mean that the swapchain is recreating itself
        return;
    }

    allocator_clear(&renderer->frame_allocator);


    // Begin recording commands.
    Vulkan_Command_Buffer* graphics_command_buffer = &renderer->queue_system->graphics_render_queue.
                                                                graphics_command_buffer[renderer->current_frame];

    vkResetCommandBuffer(graphics_command_buffer->handle, 0);
    vulkan_command_buffer_begin(graphics_command_buffer);
    // vulkan_command_buffer_begin_old(graphics_command_buffer, false, false, false);

    vulkan_queue_frame_begin(renderer, renderer->current_frame);


    //free textures and any other texture/shader updated
    buffer_system_frame_start(renderer->buffer_system, renderer->current_frame);

    vulkan_texture_system_update(renderer, render_packets);

    shader_system_update(renderer, renderer->shader_system, render_packets);


    camera_update(renderer->input_system, &renderer->main_camera, delta_time);


    Global_Ubo ubo = {0};
    // quat q = quat_from_axis_angle(vec3_up(), deg_to_rad(90.0f) * clock->time_elapsed, true);
    // ubo.model = quat_to_rotation_matrix(quat_identity(), (vec3){0.0f, 0.0f, 0.0f});
    // glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    // ubo.view = camera_get_view_matrix(&main_camera);
    ubo.view = camera_get_fps_view_matrix(&renderer->main_camera);
    // Perspective
    ubo.proj = camera_get_projection(&renderer->main_camera, renderer->framebuffer_width,
                                     renderer->framebuffer_height);


    ubo.directional_lights_address = vulkan_buffer_get_frame_device_address(
        renderer, renderer->light_system->directional_light_ssbo_handle);;
    ubo.point_lights_address = vulkan_buffer_get_frame_device_address(
        renderer, renderer->light_system->point_light_ssbo_handle);
    ubo.point_lights_count = renderer->light_system->point_light_count;
    ubo.spot_lights_address = vulkan_buffer_get_frame_device_address(
        renderer, renderer->light_system->spot_light_ssbo_handle);
    ubo.spot_lights_count = renderer->light_system->spot_light_count;
    ubo.camera_position = camera_get_world_position(&renderer->main_camera);
    ubo.screen_dimensions = (vec2s){renderer->framebuffer_width, renderer->framebuffer_height};
    ubo.time = platform_get_absolute_time();
    ubo.render_mode = renderer->mode;

    //global indexes
    //meshes
    ubo.vertex_buffer = vulkan_buffer_get_device_address(renderer, renderer->mesh_system->vertex_buffer_handle);
    ubo.normal_buffer = vulkan_buffer_get_device_address(renderer, renderer->mesh_system->normal_buffer_handle);
    ubo.tangent_buffer = vulkan_buffer_get_device_address(renderer, renderer->mesh_system->tangent_buffer_handle);
    ubo.uv_buffer = vulkan_buffer_get_device_address(renderer, renderer->mesh_system->uv_buffer_handle);
    ubo.joint_buffer = vulkan_buffer_get_device_address(renderer, renderer->mesh_system->joint_buffer_handle);
    ubo.weight_buffer = vulkan_buffer_get_device_address(renderer, renderer->mesh_system->weight_buffer_handle);
    ubo.skinned_matrix_buffer = vulkan_buffer_get_frame_device_address(
        renderer, renderer->mesh_system->skinned_matrix_buffer);
    ubo.transform_buffer = vulkan_buffer_get_frame_device_address(
        renderer, renderer->mesh_system->transform_buffer_handle);


    // Copy the current matrices to the current frame's uniform buffer.
    // As we requested a host coherent memory type for the uniform buffer, the write is instantly visible to the GPU.
    Vulkan_Buffer* ubo_buffer = &renderer->buffer_system->global_ubo[renderer->current_frame];
    memcpy(ubo_buffer->mapped_data, &ubo,
           sizeof(Global_Ubo));


    light_system_update(renderer, renderer->light_system, graphics_command_buffer);

    ui_renderer_upload_draw_data(renderer->ui_renderer, renderer, render_packets, graphics_command_buffer);


    mesh_renderer_upload_draw_data(renderer, renderer->mesh_system, render_packets, graphics_command_buffer);
    mesh_renderer_upload_per_frame_data(renderer, renderer->mesh_system, render_packets, graphics_command_buffer);
    mesh_renderer_construct_batch_draw(renderer, render_packets, graphics_command_buffer);


    particle_renderer_upload_data_draw(renderer, renderer->particle_render,
                                       render_packets, graphics_command_buffer);

    // sprite_upload_draw_data(renderer, renderer->sprite_renderer, &render_packets->sprite_data_packet,graphics_command_buffer);


    //do all our write transfer/cpu->gpu uploads first, then we put a barrier for them
    transfer_barrier_catch_all(renderer, graphics_command_buffer);


    // Dynamic state
    VkViewport default_viewport = {
        0.0f, 0.0f, (f32)renderer->framebuffer_width, (f32)renderer->framebuffer_height, 0.0f, 1.0f
    };


    // Scissor
    VkRect2D default_scissor = {
        .offset = {.x = 0, .y = 0},
        .extent = {.width = renderer->framebuffer_width, .height = renderer->framebuffer_height},
    };

    //Depth Prepass//
    vulkan_command_buffer_debug_label_begin(renderer, graphics_command_buffer, "Depth Prepass");

    image_insert_memory_barrier(graphics_command_buffer->handle,
                                renderer->swapchain.depth_attachment.texture_image,
                                VK_ACCESS_NONE,
                                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                                VK_IMAGE_LAYOUT_UNDEFINED,
                                VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                                VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
                                VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
                                (VkImageSubresourceRange){
                                    VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, 0, 1, 0, 1
                                }
    );

    // Set up the rendering attachment info

    VkRenderingAttachmentInfo predepth_attachment = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .imageView = renderer->swapchain.depth_attachment.texture_image_view,
        .imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR, // clear the depth data
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE, // dont care after rendering
        .clearValue.depthStencil = {1.0f, 0.0f},
    };

    VkRenderingInfo predepth_rendering_info = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
        .renderArea = {
            .offset = {0, 0},
            .extent = {renderer->framebuffer_width, renderer->framebuffer_height}
        },
        .layerCount = 1,
        .colorAttachmentCount = 0,
        .pColorAttachments = NULL,
        .pDepthAttachment = &predepth_attachment,
    };

    vkCmdBeginRendering(graphics_command_buffer->handle, &predepth_rendering_info);
    vkCmdSetViewport(graphics_command_buffer->handle, 0, 1, &default_viewport);
    vkCmdSetScissor(graphics_command_buffer->handle, 0, 1, &default_scissor);


    //draw geometry into depth buffer
    mesh_renderer_batch_draw_custom_pipeline(renderer, renderer->mesh_system,
                                             renderer->shader_system->mesh_batch,
                                             renderer->shader_system->mesh_batch_count,
                                             graphics_command_buffer, &renderer->predepth_mesh_pipeline);

    mesh_renderer_batch_draw_custom_pipeline(renderer, renderer->mesh_system,
                                             renderer->shader_system->skinned_batch,
                                             renderer->shader_system->skinned_batch_count,
                                             graphics_command_buffer, &renderer->predepth_skinned_mesh_pipeline);

    vkCmdEndRendering(graphics_command_buffer->handle);
    vulkan_command_buffer_debug_label_end(renderer, graphics_command_buffer);

    //transition to be read by the later stages
    image_insert_memory_barrier(
        graphics_command_buffer->handle, renderer->swapchain.depth_attachment.texture_image,
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
        VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT,

        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,

        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
        VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT,
        //want to sample in all sorts of shaders

        (VkImageSubresourceRange){
            VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, 0, 1, 0, 1
        }
    );
    //Depth Prepass END //

    //Shadow Pass//

    image_insert_memory_barrier(graphics_command_buffer->handle,
                                renderer->shadowpass_texture.texture_image,
                                0,
                                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                                VK_IMAGE_LAYOUT_UNDEFINED,
                                VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
                                //VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT maybe, this means all pipelines
                                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, // we are not waiting on anything for this write
                                VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
                                (VkImageSubresourceRange){
                                    //no stencil in this pass
                                    VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1
                                }
    );

    // Set up the rendering attachment info
    VkRenderingAttachmentInfo shadow_depth_attachment = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .imageView = renderer->shadowpass_texture.texture_image_view,
        .imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR, // clear the image
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE, //keep for reading
        // .clearValue.color = {0.0f, 0.0f, 0.0f, 0.0f},
        .clearValue.depthStencil = {1.0f, 0}
    };


    VkRenderingInfo shadow_pass_rendering_info = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
        .renderArea = {
            .offset = {0, 0},
            .extent = {renderer->shadowpass_texture.width, renderer->shadowpass_texture.height},
        },
        .layerCount = 1,
        .colorAttachmentCount = 0,
        .pColorAttachments = NULL,
        .pDepthAttachment = &shadow_depth_attachment,
    };

    vulkan_command_buffer_debug_label_begin(renderer, graphics_command_buffer, "Shadow Pass");
    vkCmdBeginRendering(graphics_command_buffer->handle, &shadow_pass_rendering_info);
    // Dynamic state
    VkViewport shadow_map_viewport = {
        0.0f, 0.0f, (f32)renderer->shadowpass_texture.width, (f32)renderer->shadowpass_texture.height, 0.0f, 1.0f
    };
    // Scissor
    VkRect2D shadow_map_scissor = {
        .offset = {.x = 0, .y = 0},
        .extent = {.width = renderer->shadowpass_texture.width, .height = renderer->shadowpass_texture.height},
    };
    vkCmdSetViewport(graphics_command_buffer->handle, 0, 1, &shadow_map_viewport);
    vkCmdSetScissor(graphics_command_buffer->handle, 0, 1, &shadow_map_scissor);


    //draw mesh, draw skinned mesh
    //TODO: just start with normal meshes, and spot lights, then expand it out
    /*
    mesh_renderer_batch_draw_custom_pipeline(renderer, renderer->mesh_renderer,
                       renderer->shader_system->mesh_batch, renderer->shader_system->mesh_batch_count,
                       graphics_command_buffer, &renderer->shadow_mesh_pipeline);

    mesh_renderer_batch_draw_custom_pipeline(renderer, renderer->mesh_renderer,
                       renderer->shader_system->skinned_batch, renderer->shader_system->skinned_batch_count,
                       graphics_command_buffer, &renderer->shadow_skinned_mesh_pipeline);
    */

    vkCmdEndRendering(graphics_command_buffer->handle);
    vulkan_command_buffer_debug_label_end(renderer, graphics_command_buffer);


    //change shadow pass texture from attachment, to read only
    image_insert_memory_barrier(
        graphics_command_buffer->handle, renderer->shadowpass_texture.texture_image,
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
        VK_ACCESS_SHADER_READ_BIT,

        VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,

        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
        VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT, //want to sample in all sorts of shaders

        (VkImageSubresourceRange){
            //no stencil in this pass
            VK_IMAGE_ASPECT_DEPTH_BIT /*| VK_IMAGE_ASPECT_STENCIL_BIT*/, 0, 1, 0, 1

        }
    );


    //Shadow Pass END //


    //Lighting PASS//

    // With dynamic rendering we need to explicitly add layout transitions by using barriers, this set of barriers prepares the color and depth images for output
    image_insert_memory_barrier(graphics_command_buffer->handle,
                                renderer->swapchain.images[image_index], 0,
                                VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                                VK_IMAGE_LAYOUT_UNDEFINED,
                                VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
                                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                (VkImageSubresourceRange){VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}
    );


    // Set up the rendering attachment info
    VkRenderingAttachmentInfo color_attachment = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .imageView = renderer->swapchain.image_views[image_index],
        .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR, // clear the image
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE, //keep for presenting
        .clearValue.color = {0.0f, 0.0f, 0.2f, 0.0f},
    };

    VkRenderingAttachmentInfo depth_attachment = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .imageView = renderer->swapchain.depth_attachment.texture_image_view,
        .imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
        .loadOp = VK_ATTACHMENT_LOAD_OP_LOAD, // load the depth from the predepth apss
        .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE, //TODO: keep if we have later passes
        .clearValue.depthStencil = {1.0f, 0.0f},
    };

    VkRenderingInfo rendering_info = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
        .renderArea = {
            .offset = {0, 0},
            .extent = {renderer->framebuffer_width, renderer->framebuffer_height}
        },
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments = &color_attachment,
        .pDepthAttachment = &depth_attachment,
    };
    vulkan_command_buffer_debug_label_begin(renderer, graphics_command_buffer, "Color Pass");
    vkCmdBeginRendering(graphics_command_buffer->handle, &rendering_info);


    vkCmdSetViewport(graphics_command_buffer->handle, 0, 1, &default_viewport);
    vkCmdSetScissor(graphics_command_buffer->handle, 0, 1, &default_scissor);


    //Do Bindings and Draw

    //TODO: WE CAN BIND THE DESCRIPTOR SETS BEFORE EVERYTHING AS LONG AS THE SET VALUES UP TO N ARE THE SAME
    // SET 0 GLOBAL UNIFORMS/STORAGE BUFFER: CAMERA LIGHTS ETC, VERTEX INDEX SKINNED BUFFERS
    // SET 1 GLOBAL TEXTURES: Textures
    // SET 2 NOTHING RN:

    mesh_renderer_batch_draw(renderer, renderer->mesh_system,
                             renderer->shader_system->mesh_batch, renderer->shader_system->mesh_batch_count,
                             graphics_command_buffer);
    mesh_renderer_batch_draw(renderer, renderer->mesh_system,
                             renderer->shader_system->skinned_batch, renderer->shader_system->skinned_batch_count,
                             graphics_command_buffer);

    particle_renderer_batch_draw(renderer, renderer->particle_render, graphics_command_buffer);

    // sprite_renderer_draw(renderer, renderer->sprite_renderer, graphics_command_buffer);

    // VkRect2D pScissors;
    // pScissors.offset = (VkOffset2D){0, 0};
    // pScissors.extent = (VkExtent2D){200, 150};
    // vkCmdSetScissor(graphics_command_buffer->handle, 0, 1, &pScissors);

    ui_renderer_madness_draw(renderer->ui_renderer, renderer, graphics_command_buffer);


    // Finish the current dynamic rendering section
    vkCmdEndRendering(graphics_command_buffer->handle);
    vulkan_command_buffer_debug_label_end(renderer, graphics_command_buffer);

    //COLOR PASS END//

    // This barrier prepares the color image for presentation, we don't need to care for the depth image
    image_insert_memory_barrier(graphics_command_buffer->handle,
                                renderer->swapchain.images[image_index],
                                VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                                0,
                                VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
                                VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                VK_PIPELINE_STAGE_2_NONE,
                                (VkImageSubresourceRange){VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});


    // Lighint Pass End //

    //Rendering End and Move onto Sumbit

    //TODO: probably gonna need at some point
    // vulkan_renderpass_UI_begin(&renderer_internal, command_buffer_current_frame, image_index);
    // ui_system_draw(&renderer_internal, UI_System_internal, command_buffer_current_frame);
    // vulkan_renderpass_UI_end(&renderer_internal, command_buffer_current_frame, image_index);


    //End DRAW COMMAND
    vulkan_command_buffer_end(graphics_command_buffer);


    vulkan_queue_frame_end(renderer, renderer->current_frame, image_index);

    // Give the image back to the swapchain.
    vulkan_swapchain_present_image(
        renderer,
        &renderer->swapchain,
        renderer->present_queue,
        renderer->queue_system->graphics_render_queue.swapchain_signal_semaphore[renderer->current_frame],
        image_index);


    // Increment (and loop) the frame index.
    renderer->current_frame = (renderer->current_frame + 1) % renderer->max_frames_in_flight;
}


void renderer_shutdown(Renderer* renderer)
{
    // vulkan_context vk_context = renderer_internal.vulkan_context;


    vkDeviceWaitIdle(renderer->logical_device);

    // Destroy in the opposite order of creation.

    //
    // uniform_buffers_destroy(&vk_context, &vk_context.global_uniform_buffers);
    // vulkan_default_shader_destroy(&vk_context, &vk_context.default_shader_pipelines,
    //                               &vk_context.default_shader_descriptor_set_layout);


    // Command buffers


    // Swapchain
    vulkan_swapchain_destroy(renderer, &renderer->swapchain);

    DEBUG("Destroying Vulkan device...");
    vulkan_device_destroy2(renderer);

    DEBUG("Destroying Vulkan surface...");
    if (renderer->surface)
    {
        vkDestroySurfaceKHR(renderer->instance, renderer->surface, renderer->vk_allocator_callback);
        renderer->surface = 0;
    }

    DEBUG("Destroying Vulkan debugger...");
    if (renderer->debug_messenger)
    {
        PFN_vkDestroyDebugUtilsMessengerEXT func =
            (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
                renderer->instance, "vkDestroyDebugUtilsMessengerEXT");
        func(renderer->instance, renderer->debug_messenger, renderer->vk_allocator_callback);
    }

    DEBUG("Destroying Vulkan instance...");
    vkDestroyInstance(renderer->instance, renderer->vk_allocator_callback);


    INFO("RENDERER SHUTDOWN");
}


void renderer_on_resize(Renderer* renderer, u32 width, u32 height)
{
    // vulkan_context vk_context = renderer_internal.vulkan_context;
    if (!renderer) return;


    if (!renderer->is_init)
    {
        INFO("cant resize window yet, not initialized");
        return;
    };

    //NOTE: doesn't actually resize anything here, just flags the renderer for a resize
    INFO("VULKAN RENDERER RESIZE HAS BEEN CALLED: new width: %d, height: %d", width, height);
    renderer->framebuffer_width_new = width;
    renderer->framebuffer_height_new = height;

    recreate_swapchain(renderer);
}
