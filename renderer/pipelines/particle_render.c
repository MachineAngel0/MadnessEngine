#include "vulkan_struct_types.h"
#include "particle_render.h"


#include "vk_buffer.h"

Particle_Render* particle_renderer_init(Renderer* renderer)
{
    Particle_Render* particle_renderer = allocator_alloc(&renderer->allocator, sizeof(Particle_Render));

    u64 particle_material_buffer_data_size = sizeof(Material_Spherical_Billboard) * MAX_PARTICLE_COUNT;


    particle_renderer->spherical_billboard_material_buffer_handle = vulkan_buffer_create_frame(
        renderer, renderer->buffer_system,
        BUFFER_TYPE_STORAGE_GPU,
        particle_material_buffer_data_size);

    particle_renderer->spherical_billboard_material_buffer_handle_new = vulkan_buffer_create_frame(
        renderer, renderer->buffer_system,
        BUFFER_TYPE_STORAGE_GPU,
        particle_material_buffer_data_size);


    particle_renderer->particle_pos_x_buffer_handle = vulkan_buffer_create_frame(
        renderer, renderer->buffer_system,
        BUFFER_TYPE_STORAGE_GPU,
        sizeof(f32) * MAX_PARTICLE_COUNT);
    particle_renderer->particle_pos_y_buffer_handle = vulkan_buffer_create_frame(
        renderer, renderer->buffer_system,
        BUFFER_TYPE_STORAGE_GPU,
        sizeof(f32) * MAX_PARTICLE_COUNT);
    particle_renderer->particle_pos_z_buffer_handle = vulkan_buffer_create_frame(
        renderer, renderer->buffer_system,
        BUFFER_TYPE_STORAGE_GPU,
        sizeof(f32) * MAX_PARTICLE_COUNT);


    particle_renderer->particle_rot_x_buffer_handle = vulkan_buffer_create_frame(
        renderer, renderer->buffer_system,
        BUFFER_TYPE_STORAGE_GPU,
        sizeof(f32) * MAX_PARTICLE_COUNT);
    particle_renderer->particle_rot_y_buffer_handle = vulkan_buffer_create_frame(
        renderer, renderer->buffer_system,
        BUFFER_TYPE_STORAGE_GPU,
        sizeof(f32) * MAX_PARTICLE_COUNT);

    particle_renderer->particle_scale_x_buffer_handle = vulkan_buffer_create_frame(
        renderer, renderer->buffer_system,
        BUFFER_TYPE_STORAGE_GPU,
        sizeof(f32) * MAX_PARTICLE_COUNT);
    particle_renderer->particle_scale_y_buffer_handle = vulkan_buffer_create_frame(
        renderer, renderer->buffer_system,
        BUFFER_TYPE_STORAGE_GPU,
        sizeof(f32) * MAX_PARTICLE_COUNT);


    particle_renderer->particle_color_buffer_handle = vulkan_buffer_create_frame(
        renderer, renderer->buffer_system,
        BUFFER_TYPE_STORAGE_GPU,
        sizeof(vec4) * MAX_PARTICLE_COUNT);

    particle_renderer->particle_draw_buffer_handle = vulkan_buffer_create_frame(
        renderer, renderer->buffer_system,
        BUFFER_TYPE_STORAGE_GPU,
        sizeof(Vulkan_Particle_Draw) * MAX_PARTICLE_COUNT);


    //default blend for now
    vulkan_pipeline_graphics_create(renderer, "billboard_spherical", Shader_Blend_Mode_Soft_Additive,
                                    Shader_Transluency_Type_Opaque,
                                    &particle_renderer->spherical_billboard_pipeline,
                                    &particle_renderer->wireframe_spherical_billboard_pipeline);


    // TODO: should pass in particle count from the particle system
    /*_shader_system_shader_batch_create_internal(renderer, renderer->shader_system,
                                                     "billboard_spherical",
                                                     Shader_Stage_Type_Graphics,
                                                     Shader_Pass_Type_Opaque,
                                                     Shader_Pass_Type_Particle,
                                                     Shader_Blend_Mode_Alpha,
                                                     sizeof(Material_Spherical_Billboard),
                                                     1000); */

    return particle_renderer;
}


int vulkan_particle_sort_key(void* draw1, void* draw2)
{
    Vulkan_Particle_Draw p1 = *(Vulkan_Particle_Draw*)draw1;
    Vulkan_Particle_Draw p2 = *(Vulkan_Particle_Draw*)draw2;

    return cmp_u64(&p1.material_key, &p2.material_key);
}

void particle_renderer_upload_data_draw(Renderer* renderer, Particle_Render* particle_render,
                                        Render_Packet* render_packet, Vulkan_Command_Buffer* command_buffer)
{
    PROFILE_ZONE(particle_renderer_upload_data_draw)


    Scratch_Allocator scratch = scratch_allocator_begin(&renderer->allocator);


    vulkan_command_buffer_debug_label_begin(renderer, command_buffer, "PARTICLE SSBO UPDATE");

    //update the particle buffer every frame
    //OPTIMIZE: make particles material shader friendly, but obv it can wait
    particle_render->draw_count = render_packet->particle_packet.particle_count;


    Material_Spherical_Billboard* mat_array = allocator_alloc(scratch.allocator,
                                                              sizeof(Material_Spherical_Billboard) * render_packet->
                                                              particle_packet.particle_count);

    Render_Packet_Particle* render_packet_particle = &render_packet->particle_packet;
    Particle* particle = render_packet->particle_packet.particles;

    for (u32 i = 0; i < render_packet_particle->particle_count; i++)
    {
        mat_array[i].point = (vec3s){
            .x = particle->pos_x[i],
            .y = particle->pos_y[i],
            .z = particle->pos_z[i]
        };
        mat_array[i].rotation = (vec2s){.x = particle->rot_x[i], .y = particle->rot_y[i]};
        mat_array[i].size = (vec2s){.x = particle->scale_x[i], .y = particle->scale_y[i]};
        mat_array[i].texture_idx = particle->texture_handle[i].handle;
        mat_array[i].color = particle->color[i];
        // TODO:
        // billboard_spherical_material.color
        // render_packet->particle_packet.particles[i].tex_size;
        // render_packet->particle_packet.particles[i].tex_offset;
    }


    //
    vulkan_buffer_frame_staging_upload(
        renderer, particle_render->spherical_billboard_material_buffer_handle,
        command_buffer,
        mat_array,
        sizeof(Material_Spherical_Billboard) * render_packet->particle_packet.particle_count);

    vulkan_buffer_frame_staging_upload(
        renderer, particle_render->particle_pos_x_buffer_handle,
        command_buffer,
        particle->pos_x,
        sizeof(f32) * render_packet->particle_packet.particle_count);
    vulkan_buffer_frame_staging_upload(
        renderer, particle_render->particle_pos_y_buffer_handle,
        command_buffer,
        particle->pos_y,
        sizeof(f32) * render_packet->particle_packet.particle_count);
    vulkan_buffer_frame_staging_upload(
        renderer, particle_render->particle_pos_z_buffer_handle,
        command_buffer,
        particle->pos_z,
        sizeof(f32) * render_packet->particle_packet.particle_count);

    vulkan_buffer_frame_staging_upload(
        renderer, particle_render->particle_rot_x_buffer_handle,
        command_buffer,
        particle->rot_x,
        sizeof(f32) * render_packet->particle_packet.particle_count);
    vulkan_buffer_frame_staging_upload(
        renderer, particle_render->particle_rot_y_buffer_handle,
        command_buffer,
        particle->rot_y,
        sizeof(f32) * render_packet->particle_packet.particle_count);

    vulkan_buffer_frame_staging_upload(
        renderer, particle_render->particle_scale_x_buffer_handle,
        command_buffer,
        particle->scale_x,
        sizeof(f32) * render_packet->particle_packet.particle_count);
    vulkan_buffer_frame_staging_upload(
        renderer, particle_render->particle_scale_y_buffer_handle,
        command_buffer,
        particle->scale_y,
        sizeof(f32) * render_packet->particle_packet.particle_count);

    vulkan_buffer_frame_staging_upload(
        renderer, particle_render->particle_color_buffer_handle,
        command_buffer,
        particle->color,
        sizeof(f32) * render_packet->particle_packet.particle_count);


    Vulkan_Buffer* spherical_material_buffer = vulkan_buffer_get_frame(
        renderer, particle_render->spherical_billboard_material_buffer_handle);

    Vulkan_Buffer* pos_x_buffer = vulkan_buffer_get_frame(
        renderer, particle_render->particle_pos_x_buffer_handle);
    Vulkan_Buffer* pos_y_buffer = vulkan_buffer_get_frame(
        renderer, particle_render->particle_pos_y_buffer_handle);
    Vulkan_Buffer* pos_z_buffer = vulkan_buffer_get_frame(
        renderer, particle_render->particle_pos_z_buffer_handle);

    Vulkan_Buffer* rot_x_buffer = vulkan_buffer_get_frame(
        renderer, particle_render->particle_rot_x_buffer_handle);
    Vulkan_Buffer* rot_y_buffer = vulkan_buffer_get_frame(
        renderer, particle_render->particle_rot_y_buffer_handle);

    Vulkan_Buffer* scale_x_buffer = vulkan_buffer_get_frame(
        renderer, particle_render->particle_scale_x_buffer_handle);
    Vulkan_Buffer* scale_y_buffer = vulkan_buffer_get_frame(
        renderer, particle_render->particle_scale_y_buffer_handle);

    Vulkan_Buffer* color_buffer = vulkan_buffer_get_frame(
        renderer, particle_render->particle_color_buffer_handle);

    VkBufferMemoryBarrier2 particle_barrier = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
        .srcStageMask = VK_PIPELINE_STAGE_2_COPY_BIT,
        .srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,

        .dstStageMask = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
        .dstAccessMask = VK_ACCESS_2_SHADER_STORAGE_READ_BIT,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,

        .buffer = spherical_material_buffer->handle,
        .offset = 0,
        .size = VK_WHOLE_SIZE,
    };

    vulkan_command_add_buffer_barrier(command_buffer, particle_barrier);


    particle_barrier.buffer = pos_x_buffer->handle;
    vulkan_command_add_buffer_barrier(command_buffer, particle_barrier);
    particle_barrier.buffer = pos_y_buffer->handle;
    vulkan_command_add_buffer_barrier(command_buffer, particle_barrier);
    particle_barrier.buffer = pos_z_buffer->handle;
    vulkan_command_add_buffer_barrier(command_buffer, particle_barrier);

    particle_barrier.buffer = rot_x_buffer->handle;
    vulkan_command_add_buffer_barrier(command_buffer, particle_barrier);
    particle_barrier.buffer = rot_y_buffer->handle;
    vulkan_command_add_buffer_barrier(command_buffer, particle_barrier);

    particle_barrier.buffer = scale_x_buffer->handle;
    vulkan_command_add_buffer_barrier(command_buffer, particle_barrier);
    particle_barrier.buffer = scale_y_buffer->handle;
    vulkan_command_add_buffer_barrier(command_buffer, particle_barrier);

    particle_barrier.buffer = color_buffer->handle;
    vulkan_command_add_buffer_barrier(command_buffer, particle_barrier);


    //OPTIMIZE: technically since we have a dead list with a count we know the size of the particles alive
    //  and we can also keep an alive list during emission if we really wnt
    //construct the alive particle list
    particle_render->particle_draw = allocator_alloc(&renderer->frame_allocator,
                                                     sizeof(Vulkan_Particle_Draw) * render_packet->
                                                     particle_packet.particle_count);
    particle_render->particle_draw_count = 0;
    for (u32 i = 0; i < render_packet_particle->particle_count; i++)
    {
        if (render_packet->particle_packet.particles->life_left[i] > 0)
        {
            // particle_draw[particle_draw_count].material_key = render_packet->particle_packet.particles->material_key[i];
            particle_render->particle_draw[particle_render->particle_draw_count].material_key = rand_range_i(0, 789);
            particle_render->particle_draw[particle_render->particle_draw_count].material_index = render_packet->
                particle_packet.particles->material_id[i];
            particle_render->particle_draw[particle_render->particle_draw_count].particle_index = i;
            particle_render->particle_draw_count++;
        }
    }
    //sort the particles by material key
    qsort(particle_render->particle_draw, particle_render->particle_draw_count, sizeof(Vulkan_Particle_Draw),
          vulkan_particle_sort_key);

    vulkan_buffer_frame_staging_upload(
        renderer, particle_render->particle_draw_buffer_handle,
        command_buffer,
        particle_render->particle_draw,
        sizeof(Vulkan_Particle_Draw) * particle_render->particle_draw_count);

    particle_barrier.buffer = vulkan_buffer_get_frame(renderer, particle_render->particle_draw_buffer_handle)->handle;
    vulkan_command_add_buffer_barrier(command_buffer, particle_barrier);


    vulkan_command_flush_barriers(command_buffer);

    vulkan_command_buffer_debug_label_end(renderer, command_buffer);


    scratch_allocator_end(scratch);


    PROFILE_ZONE_END(particle_renderer_upload_data_draw)
}

void particle_renderer_batch_draw(Renderer* renderer, Particle_Render* particle_render,
                                  Vulkan_Command_Buffer* command_buffer)
{
    // particle_ssbo[]
    // material_ssbo[] - material2_ssbo[]
    // e1|e2 (mat instance) - e1 (mat instance 2)

    //TODO: issue a draw for material range in the particle draw

    if (particle_render->particle_draw_count == 0) { return; }

    //TODO: remove when we finally want to test it
    bool do_it = false;
    if (do_it)
    {
        u64 current_key = particle_render->particle_draw[0].material_key;
        u64 last_idx = 0;
        u64 current_idx = 0;
        while (last_idx <= particle_render->particle_draw_count)
        {
            if (current_key != particle_render->particle_draw[current_idx].material_key)
            {
                //find the batch and do a draw over the range
                Vulkan_Shader_Batch* batch = vulkan_shader_system_shader_batch_get_by_mat_key_for_particles(
                    renderer, current_key);

                //uniform
                vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                        batch->pipeline.pipeline_layout, 0, 1,
                                        &renderer->descriptor_system->uniform_descriptors.descriptor_sets[renderer->
                                            current_frame],
                                        0, 0);

                //textures
                vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                        batch->pipeline.pipeline_layout, 1, 1,
                                        &renderer->descriptor_system->texture_descriptors.descriptor_sets[0], 0, 0);

                //storage buffers
                vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                        batch->pipeline.pipeline_layout, 2, 1,
                                        &renderer->descriptor_system->storage_descriptors.descriptor_sets[renderer->
                                            current_frame],
                                        0, 0);


                PC_Particle pc_particle = {
                    //TODO: set the proper material buffer
                    .material_buffer = get_buffer_device_address(renderer->logical_device,
                                                                 vulkan_buffer_get_frame(
                                                                     renderer,
                                                                     particle_render->
                                                                     spherical_billboard_material_buffer_handle)->
                                                                 handle),
                    .particle_draw_buffer = get_buffer_device_address(renderer->logical_device,
                                                                      vulkan_buffer_get_frame(
                                                                          renderer,
                                                                          particle_render->particle_draw_buffer_handle)
                                                                      ->
                                                                      handle),
                };

                VkPushConstantsInfo push_constant_info_ui = {0};
                push_constant_info_ui.sType = VK_STRUCTURE_TYPE_PUSH_CONSTANTS_INFO;
                push_constant_info_ui.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
                push_constant_info_ui.layout = particle_render->spherical_billboard_pipeline.pipeline_layout;
                push_constant_info_ui.offset = 0;
                push_constant_info_ui.size = sizeof(PC_Particle);
                push_constant_info_ui.pValues = &pc_particle;
                push_constant_info_ui.pNext = NULL;


                vkCmdBindPipeline(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                  batch->pipeline.handle);
                vkCmdPushConstants2(command_buffer->handle, &push_constant_info_ui);


                vkCmdDraw(command_buffer->handle, 6,
                          current_idx - last_idx, 0,
                          0);
            }
            current_idx++;
        }
    }
    //TODO: we want to issue a draw for the last bit of data left


    //uniform
    vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            particle_render->spherical_billboard_pipeline.pipeline_layout, 0, 1,
                            &renderer->descriptor_system->uniform_descriptors.descriptor_sets[renderer->current_frame],
                            0, 0);

    //textures
    vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            particle_render->spherical_billboard_pipeline.pipeline_layout, 1, 1,
                            &renderer->descriptor_system->texture_descriptors.descriptor_sets[0], 0, 0);

    //storage buffers
    vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            particle_render->spherical_billboard_pipeline.pipeline_layout, 2, 1,
                            &renderer->descriptor_system->storage_descriptors.descriptor_sets[renderer->current_frame],
                            0, 0);

    PC_Particle pc_particle = {
        .material_buffer = get_buffer_device_address(renderer->logical_device,
                                                     vulkan_buffer_get_frame(
                                                         renderer,
                                                         particle_render->
                                                         spherical_billboard_material_buffer_handle)->
                                                     handle),
        .particle_draw_buffer = get_buffer_device_address(renderer->logical_device,
                                                          vulkan_buffer_get_frame(
                                                              renderer,
                                                              particle_render->
                                                              spherical_billboard_material_buffer_handle)->
                                                          handle),
    };


    VkPushConstantsInfo push_constant_info_ui = {0};
    push_constant_info_ui.sType = VK_STRUCTURE_TYPE_PUSH_CONSTANTS_INFO;
    push_constant_info_ui.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    push_constant_info_ui.layout = particle_render->spherical_billboard_pipeline.pipeline_layout;
    push_constant_info_ui.offset = 0;
    push_constant_info_ui.size = sizeof(PC_Particle);
    push_constant_info_ui.pValues = &pc_particle;
    push_constant_info_ui.pNext = NULL;

    vkCmdBindPipeline(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      particle_render->spherical_billboard_pipeline.handle);
    vkCmdPushConstants2(command_buffer->handle, &push_constant_info_ui);


    vkCmdDraw(command_buffer->handle, 6,
              particle_render->draw_count, 0,
              0);
}
