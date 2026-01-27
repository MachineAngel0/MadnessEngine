#include "renderer.h"
#include "camera.h"
#include "lights.h"
#include "mesh.h"
#include "shader_system.h"
#include "vk_command_buffer.h"
#include "vk_framebuffer.h"
#include "vk_image.h"
#include "vk_renderpass.h"
#include "vk_shader.h"
#include "vk_sync.h"
#include "vk_vertex_buffer.h"


//NOTE: static/global for now, most likely gonna move it into the renderer struct
static renderer renderer_internal;


bool renderer_init(struct renderer_app* renderer_inst, Arena* arena)
{
    vulkan_context* vk_context = &renderer_internal.context;

    //set up memory for the renderer
    u64 renderer_system_mem_requirement = GB(1);
    void* renderer_system_mem = arena_alloc(arena, renderer_system_mem_requirement);
    arena_init(&renderer_internal.arena, renderer_system_mem, renderer_system_mem_requirement);

    u64 frame_arena_mem_size = GB(0.5);
    void* frame_arena_mem = arena_alloc(&renderer_internal.arena, frame_arena_mem_size);
    arena_init(&renderer_internal.frame_arena, frame_arena_mem, frame_arena_mem_size);

    // vulkan_context vk_context = renderer_internal.vulkan_context;


    camera_init(&renderer_internal.main_camera);
    vk_context->is_init = false;
    // vulkan_context vulkan_context;

    //get the size for the default window from the app config
    //if these aren't set we use 800/600 for default
    vk_context->framebuffer_width = (renderer_inst->app_config.start_width != 0)
                                        ? renderer_inst->app_config.start_width
                                        : 600;
    vk_context->framebuffer_height = (renderer_inst->app_config.start_height != 0)
                                         ? renderer_inst->app_config.start_height
                                         : 600;
    //set this as well
    vk_context->framebuffer_width_new = vk_context->framebuffer_width;
    vk_context->framebuffer_height_new = vk_context->framebuffer_height;

    //create the instance
    vulkan_instance_create(vk_context);

    // get surface from the platform layer, needed before device creation
    if (!platform_create_vulkan_surface(renderer_inst->plat_state, vk_context))
    {
        return FALSE;
    }

    //allow the window to resize at this point. NOTE: might want to move this to the end of init
    vk_context->is_init = true;

    // Device creation
    if (!vulkan_device_create(vk_context))
    {
        M_ERROR("Failed to create device!");
        return FALSE;
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
    descriptor_pool_allocator_init(vk_context, &vk_context->global_descriptor_pool);
    //TODO: move functions into the init of pool allocator
    create_texture_bindless_descriptor_set(vk_context, &vk_context->global_descriptor_pool,
                                           &renderer_internal.global_descriptors.texture_descriptors);
    create_bindless_uniform_buffer_descriptor_set(vk_context, &vk_context->global_descriptor_pool,
                                                  &renderer_internal.global_descriptors.uniform_descriptors);
    create_bindless_storage_buffer_descriptor_set(vk_context, &vk_context->global_descriptor_pool,
                                                  &renderer_internal.global_descriptors.storage_descriptors);


    //BUFFER SYSTEM
    //Allocate buffers up front
    renderer_internal.buffer_system = buffer_system_init(&renderer_internal);
    for (u32 i = 0; i < renderer_internal.buffer_system->global_uniform_buffer_size; i++)
    {
        update_uniform_buffer_bindless_descriptor_set(
            vk_context, &renderer_internal.global_descriptors.uniform_descriptors,
            &renderer_internal.buffer_system->global_uniform_buffer[i], sizeof(uniform_buffer_object), i);
    }

    //Shader System
    shader_system_init(&renderer_internal, &renderer_internal.shader_system);
    // Light System
    renderer_internal.light_system = light_system_init(&renderer_internal);

    //INDIRECT DRAW
    renderer_internal.indirect_mesh = mesh_load_gltf_indirect(&renderer_internal,
                                                              "../z_assets/models/FlightHelmet_gltf/FlightHelmet.gltf");
    // renderer_internal.indirect_mesh = mesh_load_gltf_indirect(&renderer_internal,
    // "../z_assets/models/damaged_helmet_gltf/DamagedHelmet.gltf");

    vulkan_buffer_cpu_create(&renderer_internal, &renderer_internal.indirect_vertex_buffer, CPU_VERTEX, MB(32));
    vulkan_buffer_cpu_create(&renderer_internal, &renderer_internal.indirect_index_buffer, CPU_INDEX, MB(32));
    vulkan_buffer_cpu_create(&renderer_internal, &renderer_internal.indirect_draw_command_buffer, CPU_INDIRECT, MB(32));
    vulkan_buffer_cpu_create(&renderer_internal, &renderer_internal.uv_storage_buffer, CPU_STORAGE, MB(32));
    vulkan_buffer_cpu_create(&renderer_internal, &renderer_internal.normal_storage_buffer, CPU_STORAGE, MB(32));
    vulkan_buffer_cpu_create(&renderer_internal, &renderer_internal.material_ssbo_buffer, CPU_STORAGE, MB(32));
    vulkan_buffer_cpu_create(&renderer_internal, &renderer_internal.directional_light_storage_buffer, CPU_STORAGE,
                             MB(32));


    for (u32 i = 0; i < renderer_internal.indirect_mesh->mesh_size; i++)
    {
        update_global_texture_bindless_descriptor_set(
            vk_context, &renderer_internal.global_descriptors.texture_descriptors,
            shader_system_get_texture(
                renderer_internal.shader_system,
                renderer_internal.indirect_mesh->mesh[i].color_texture),
            renderer_internal.indirect_mesh->mesh[i].color_texture.handle);


        vulkan_buffer_cpu_data_copy_from_offset(&renderer_internal, &renderer_internal.indirect_vertex_buffer,
                                                renderer_internal.indirect_mesh->mesh[i].vertices.pos,
                                                renderer_internal.indirect_mesh->mesh[i].vertex_bytes);
        vulkan_buffer_cpu_data_copy_from_offset(&renderer_internal, &renderer_internal.indirect_index_buffer,
                                                renderer_internal.indirect_mesh->mesh[i].indices,
                                                renderer_internal.indirect_mesh->mesh[i].indices_bytes);
        vulkan_buffer_cpu_data_copy_from_offset(&renderer_internal, &renderer_internal.indirect_draw_command_buffer,
                                                &renderer_internal.indirect_mesh->indirect_draw_array[i],
                                                sizeof(VkDrawIndexedIndirectCommand));

        vulkan_buffer_cpu_data_copy_from_offset(&renderer_internal, &renderer_internal.material_ssbo_buffer,
                                                &renderer_internal.indirect_mesh->mesh[i].color_texture.handle,
                                                sizeof(u32));

        vulkan_buffer_cpu_data_copy_from_offset(&renderer_internal, &renderer_internal.uv_storage_buffer,
                                                renderer_internal.indirect_mesh->mesh[i].vertices.uv,
                                                renderer_internal.indirect_mesh->mesh[i].uv_bytes);

        vulkan_buffer_cpu_data_copy_from_offset(&renderer_internal, &renderer_internal.normal_storage_buffer,
                                                renderer_internal.indirect_mesh->mesh[i].vertices.normal,
                                                renderer_internal.indirect_mesh->mesh[i].normal_bytes);
    }
    update_storage_buffer_bindless_descriptor_set(
        vk_context, &renderer_internal.global_descriptors.storage_descriptors,
        &renderer_internal.uv_storage_buffer, renderer_internal.uv_storage_buffer.current_offset, 0);
    update_storage_buffer_bindless_descriptor_set(
        vk_context, &renderer_internal.global_descriptors.storage_descriptors,
        &renderer_internal.normal_storage_buffer, renderer_internal.normal_storage_buffer.current_offset, 1);
    update_storage_buffer_bindless_descriptor_set(
        vk_context, &renderer_internal.global_descriptors.storage_descriptors,
        &renderer_internal.material_ssbo_buffer, renderer_internal.material_ssbo_buffer.current_offset, 2);


    vulkan_mesh_indirect_shader_create(&renderer_internal, &renderer_internal.indirect_pipeline);


    //BUFFER ADDRESSING
    /*
    // mesh* test_mesh = mesh_load_gltf(&renderer_internal, "../z_assets/models/damaged_helmet_gltf/DamagedHelmet.gltf");
    static_mesh* test_mesh = mesh_load_gltf(&renderer_internal,
                                            "../z_assets/models/FlightHelmet_gltf/FlightHelmet.gltf");
    // mesh* test_mesh = mesh_load_gltf(&renderer_internal, "../z_assets/models/cube_gltf/Cube.gltf");
    vk_context->mesh_default.static_mesh = test_mesh;



    for (u32 i = 0; i < test_mesh->mesh_size; i++)
    {
        update_global_texture_bindless_descriptor_set(
            vk_context, &renderer_internal.global_descriptors.texture_descriptors,
            shader_system_get_texture(
                renderer_internal.shader_system,
                test_mesh->mesh[i].color_texture),
            test_mesh->mesh[i].color_texture.handle);

        vulkan_buffer_cpu_data_copy_from_offset(&renderer_internal, renderer_internal.buffer_system->vertex_buffers,
                                                test_mesh->mesh[i].vertices.pos,
                                                test_mesh->mesh[i].vertex_bytes);
        vulkan_buffer_cpu_data_copy_from_offset(&renderer_internal, renderer_internal.buffer_system->normal_buffers,
                                                test_mesh->mesh[i].vertices.normal, test_mesh->mesh[i].normal_bytes);


        vulkan_buffer_cpu_data_copy_from_offset(&renderer_internal, renderer_internal.buffer_system->tangent_buffers,
                                                test_mesh->mesh[i].vertices.tangent, test_mesh->mesh[i].tangent_bytes);

        vulkan_buffer_cpu_data_copy_from_offset(&renderer_internal, renderer_internal.buffer_system->uv_buffers,
                                                test_mesh->mesh[i].vertices.uv,
                                                test_mesh->mesh[i].uv_bytes);

        vulkan_buffer_cpu_data_copy_from_offset(&renderer_internal, renderer_internal.buffer_system->index_buffers,
                                                test_mesh->mesh[i].indices,
                                                test_mesh->mesh[i].indices_bytes);
    }

    vulkan_mesh_bda_shader_create(&renderer_internal, &vk_context->mesh_default);
    */

    //BINDLESS TEXTURE
    create_texture_image(vk_context, vk_context->graphics_command_buffer, "../renderer/texture/error_texture.png",
                         &vk_context->shader_texture_bindless.texture_test_object);


    vulkan_bindless_textured_shader_create(&renderer_internal, &vk_context->shader_texture_bindless);
    createVertexBufferTexture(vk_context, &vk_context->shader_texture_bindless);

    update_global_texture_bindless_descriptor_set(vk_context, &renderer_internal.global_descriptors.texture_descriptors,
                                                  &vk_context->shader_texture_bindless.texture_test_object, 0);
    /*
    update_uniform_buffer_bindless_descriptor_set(
        vk_context, &vk_context->global_descriptors.uniform_descriptors,
        &renderer_internal.buffer_system->global_uniform_buffer[0], sizeof(uniform_buffer_object), 0);
        */

    INFO("VULKAN RENDERER INITIALIZED");

    return TRUE;
}


static bool texture_flip = false;

void renderer_update(struct renderer_app* renderer_inst, Clock* clock)
{
    vulkan_context vk_context = renderer_internal.context;


    arena_clear(&renderer_internal.frame_arena);


    // vulkan_context vk_context = renderer_internal.vulkan_context;


    //TODO: test code, can remove later
    if (input_key_released_unique(KEY_M))
    {
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

    camera_update(&renderer_internal.main_camera, clock->delta_time);

    uniform_buffer_object ubo = {0};
    // quat q = quat_from_axis_angle(vec3_up(), deg_to_rad(90.0f) * clock->time_elapsed, true);
    // ubo.model = quat_to_rotation_matrix(quat_identity(), (vec3){0.0f, 0.0f, 0.0f});
    ubo.model = mat4_identity();
    // glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    // ubo.view = camera_get_view_matrix(&main_camera);
    ubo.view = camera_get_fps_view_matrix(&renderer_internal.main_camera);
    // Perspective
    ubo.proj = camera_get_projection(&renderer_internal.main_camera, vk_context.framebuffer_width,
                                     vk_context.framebuffer_height);

    VkDeviceAddress light_buffer_address = get_buffer_device_address(vk_context.device.logical_device,
                                                                     renderer_internal.light_system->
                                                                     point_light_storage_buffer.handle);
    ubo.point_lights_address = light_buffer_address;
    ubo.point_lights_count = renderer_internal.light_system->point_light_count;
    ubo.camera_position = renderer_internal.main_camera.viewPos;
    ubo.debug_mode = DEBUG_MODE_NONE;

    // Copy the current matrices to the current frame's uniform buffer. As we requested a host coherent memory type for the uniform buffer, the write is instantly visible to the GPU.
    memcpy(renderer_internal.buffer_system->global_uniform_buffer[vk_context.current_frame].mapped_data, &ubo,
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
        0.0f, 0.0f, (f32)vk_context.framebuffer_width, (f32)vk_context.framebuffer_height, 0.0f, 1.0f
    };


    // Scissor
    VkRect2D scissor = {
        .offset = {.x = 0, .y = 0},
        .extent = {.width = vk_context.framebuffer_width, .height = vk_context.framebuffer_height},
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

    //INDIRECT DRAW
    vkCmdBindPipeline(command_buffer_current_frame->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      renderer_internal.indirect_pipeline.handle);

    //uniform
    vkCmdBindDescriptorSets(command_buffer_current_frame->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            renderer_internal.indirect_pipeline.pipeline_layout, 0, 1,
                            &renderer_internal.global_descriptors.uniform_descriptors.descriptor_sets[vk_context.
                                current_frame], 0, 0);

    //textures
    vkCmdBindDescriptorSets(command_buffer_current_frame->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            renderer_internal.indirect_pipeline.pipeline_layout, 1, 1,
                            &renderer_internal.global_descriptors.texture_descriptors.descriptor_sets[vk_context.
                                current_frame], 0, 0);

    //storage buffers
    vkCmdBindDescriptorSets(command_buffer_current_frame->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            renderer_internal.indirect_pipeline.pipeline_layout, 2, 1,
                            &renderer_internal.global_descriptors.storage_descriptors.descriptor_sets[vk_context.
                                current_frame], 0, 0);


    VkDeviceSize pOffsets[] = {0};
    vkCmdBindVertexBuffers(command_buffer_current_frame->handle, 0, 1, &renderer_internal.indirect_vertex_buffer.handle,
                           pOffsets);

    vkCmdBindIndexBuffer(command_buffer_current_frame->handle, renderer_internal.indirect_index_buffer.handle, 0,
                         renderer_internal.indirect_mesh->mesh[0].index_type);

    if (renderer_internal.context.device.features.multiDrawIndirect)
    {
        vkCmdDrawIndexedIndirect(command_buffer_current_frame->handle,
                                 renderer_internal.indirect_draw_command_buffer.handle, 0,
                                 renderer_internal.indirect_mesh->mesh_size, sizeof(VkDrawIndexedIndirectCommand));
    }
    else
    {
        // If multi draw is not available, we must issue separate draw commands
        for (auto j = 0; j < renderer_internal.indirect_mesh->mesh_size; j++)
        {
            vkCmdDrawIndexedIndirect(command_buffer_current_frame->handle,
                                     renderer_internal.indirect_draw_command_buffer.handle,
                                     j * sizeof(VkDrawIndexedIndirectCommand), 1, sizeof(VkDrawIndexedIndirectCommand));
        }
    }

    /*
    //BUFFER ADDRESSING//


    vkCmdBindPipeline(command_buffer_current_frame->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      vk_context.mesh_default.mesh_shader_pipeline.handle);

    //uniform
    vkCmdBindDescriptorSets(command_buffer_current_frame->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            vk_context.mesh_default.mesh_shader_pipeline.pipeline_layout, 0, 1,
                            &renderer_internal.global_descriptors.uniform_descriptors.descriptor_sets[vk_context.
                                current_frame], 0, 0);

    VkDeviceAddress position_buffer_address = get_buffer_device_address(vk_context.device.logical_device,
                                                                        renderer_internal.buffer_system->vertex_buffers
                                                                        ->handle);
    VkDeviceAddress uv_buffer_address = get_buffer_device_address(vk_context.device.logical_device,
                                                                  renderer_internal.buffer_system->uv_buffers->handle);
    VkDeviceAddress normal_buffer_address = get_buffer_device_address(vk_context.device.logical_device,
                                                                      renderer_internal.buffer_system->normal_buffers->
                                                                      handle);
    VkDeviceAddress tangent_buffer_address = get_buffer_device_address(vk_context.device.logical_device,
                                                                       renderer_internal.buffer_system->tangent_buffers
                                                                       ->handle);

    //textures
    vkCmdBindDescriptorSets(command_buffer_current_frame->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            vk_context.mesh_default.mesh_shader_pipeline.pipeline_layout, 1, 1,
                            &renderer_internal.global_descriptors.texture_descriptors.descriptor_sets[vk_context.
                                current_frame],
                            0, 0);

    u32 vertex_bytes = 0;
    u32 uv_bytes = 0;
    u32 index_bytes = 0;
    u32 tangent_bytes = 0;
    u32 normal_bytes = 0;
    for (u64 i = 0; i < vk_context.mesh_default.static_mesh->mesh_size; i++)
    {
        // PUSH CONSTANT with DEVICE BUFFER ADDRESS's
        pc_mesh temp_pc_mesh = {
            .pos_address = position_buffer_address + vertex_bytes,
            .normal_index = normal_buffer_address + normal_bytes,
            .tangent_index = tangent_buffer_address + tangent_bytes,
            .uv_index = uv_buffer_address + uv_bytes,
            .albedo_material_index = vk_context.mesh_default.static_mesh->mesh[i].color_texture.handle,
        };
        vkCmdPushConstants(command_buffer_current_frame->handle,
                           vk_context.mesh_default.mesh_shader_pipeline.pipeline_layout,
                           VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                           sizeof(pc_mesh), &temp_pc_mesh);
        // vkCmdPushConstants2() // TODO: look into


        //dont need to bind vertex buffer since we are passing its address instead
        vkCmdBindIndexBuffer(command_buffer_current_frame->handle,
                             renderer_internal.buffer_system->index_buffers->handle, index_bytes,
                             vk_context.mesh_default.static_mesh->mesh[i].index_type);
        // // allows better validation and allows partial binding, such as rendering a certain amount of meshes
        // vkCmdBindIndexBuffer2();
        vkCmdDrawIndexed(command_buffer_current_frame->handle,
                         (u32)vk_context.mesh_default.static_mesh->mesh[i].indices_count,
                         1, 0, 0, 0);

        vertex_bytes += vk_context.mesh_default.static_mesh->mesh[i].vertex_bytes;
        uv_bytes += vk_context.mesh_default.static_mesh->mesh[i].uv_bytes;
        index_bytes += vk_context.mesh_default.static_mesh->mesh[i].indices_bytes;
        tangent_bytes += vk_context.mesh_default.static_mesh->mesh[i].tangent_bytes;
        normal_bytes += vk_context.mesh_default.static_mesh->mesh[i].normal_bytes;
    }
*/
    //DRAW BINDLESS textured triangle
    vkCmdBindPipeline(command_buffer_current_frame->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      vk_context.shader_texture_bindless.shader_texture_pipeline.handle);

    //uniform
    vkCmdBindDescriptorSets(command_buffer_current_frame->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            vk_context.shader_texture_bindless.shader_texture_pipeline.pipeline_layout, 0, 1,
                            &renderer_internal.global_descriptors.uniform_descriptors.descriptor_sets[vk_context.
                                current_frame],
                            0, 0);
    //textures
    vkCmdBindDescriptorSets(command_buffer_current_frame->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            vk_context.shader_texture_bindless.shader_texture_pipeline.pipeline_layout, 1, 1,
                            &renderer_internal.global_descriptors.texture_descriptors.descriptor_sets[vk_context.
                                current_frame],
                            0, 0);


    VkDeviceSize offsets_bindless[1] = {0};
    vkCmdBindVertexBuffers(command_buffer_current_frame->handle, 0, 1,
                           &vk_context.shader_texture_bindless.vertex_buffer.handle, offsets_bindless);

    vkCmdBindIndexBuffer(command_buffer_current_frame->handle,
                         vk_context.shader_texture_bindless.index_buffer.handle, 0,
                         VK_INDEX_TYPE_UINT32);

    vkCmdDrawIndexed(command_buffer_current_frame->handle,
                     (u32)vk_context.shader_texture_bindless.vertex_info.indices_size,
                     1, 0, 0, 0);
    // vkCmdDrawIndexedIndirect()
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


void renderer_shutdown(struct renderer_app* renderer_inst)
{
    vulkan_context vk_context = renderer_internal.context;

    // vulkan_context vk_context = renderer_internal.vulkan_context;


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
            (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
                vk_context.instance, "vkDestroyDebugUtilsMessengerEXT");
        func(vk_context.instance, vk_context.debug_messenger, vk_context.allocator);
    }

    DEBUG("Destroying Vulkan instance...");
    vkDestroyInstance(vk_context.instance, vk_context.allocator);
}


void renderer_on_resize(struct renderer_app* renderer_inst, u32 width, u32 height)
{
    // vulkan_context vk_context = renderer_internal.vulkan_context;
    vulkan_context vk_context = renderer_internal.context;


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
