#include "vulkan_struct_types.h"
#include "particle_render.h"


#include "vulkan_shader_system.h"
#include "vk_buffer.h"

Particle_Render* particle_renderer_init(Renderer* renderer)
{
    PROFILE_ZONE(particle_renderer_init);

    Particle_Render* particle_renderer = allocator_alloc(&renderer->allocator, sizeof(Particle_Render));

    u64 particle_renderable_size = sizeof(Particle) * MAX_PARTICLE_COUNT;
    u64 particle_draw_data_size = sizeof(Particle_Renderable) * MAX_PARTICLE_COUNT;


    particle_renderer->particle_render_data_buffer_handle = vulkan_buffer_create_frame(
        renderer, renderer->buffer_system,
        BUFFER_TYPE_STORAGE_GPU,
        particle_renderable_size, "Particle Render Data");

    particle_renderer->particle_draw_buffer_handle = vulkan_buffer_create_frame(
        renderer, renderer->buffer_system,
        BUFFER_TYPE_STORAGE_GPU,
        particle_draw_data_size, "Particle Draw Data");


    //default blend for now
    /*vulkan_pipeline_graphics_create(renderer, "billboard_spherical", Shader_Blend_Mode_Soft_Additive,
                                    Shader_Transluency_Type_Opaque,
                                    &particle_renderer->spherical_billboard_pipeline,
                                    &particle_renderer->wireframe_spherical_billboard_pipeline);*/
    vulkan_pipeline_graphics_create(renderer, "billboard_spherical", Shader_Blend_Mode_Soft_Additive, false,
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

    PROFILE_ZONE_END(particle_renderer_init);
    return particle_renderer;
}


int vulkan_particle_sort_key(const void* draw1, const void* draw2)
{
    Particle_Renderable p1 = *(Particle_Renderable*)draw1;
    Particle_Renderable p2 = *(Particle_Renderable*)draw2;

    return cmp_u64(&p1.shader_key, &p2.shader_key);
}


void particle_renderer_upload_data_draw(Renderer* renderer, Particle_Render* particle_render,
                                        Render_Packet* render_packet, Vulkan_Command_Buffer* command_buffer)
{
    PROFILE_ZONE(particle_renderer_upload_data_draw)

    Render_Packet_Particle* particle_packet = &render_packet->particle_packet;

    Scratch_Allocator scratch = scratch_allocator_begin(&renderer->allocator);


    vulkan_command_buffer_debug_label_begin(renderer, command_buffer, "PARTICLE SSBO UPDATE");

    //update the particle buffer every frame
    //OPTIMIZE: make particles material shader friendly, but obv it can wait


    //NOTE: WE COULD SORT BY EMITTERS FOR TRANSPARNECY AND USE THE DYNAMIC ARRAY TO APPEND TO THE DRAW LIST

    //we could try getting the alive count to properly size this if memory is an issue
    Particle_Renderable* particle_draw_data = allocator_alloc(&renderer->frame_allocator,
                                                                     sizeof(Particle_Renderable) *
                                                                     particle_packet->
                                                                     particle_count);
    particle_render->particle_draw_count = 0;

    for (u32 i = 0; i < particle_packet->particle_count; i++)
    {
        Particle* particle = &particle_packet->particles[i];
        if (particle->life_left <= 0) { continue; }

        particle_draw_data[particle_render->particle_draw_count++] = (Particle_Renderable){
            .shader_key = particle_packet->particles[i].shader_key,
            .material_index = particle_packet->particles[i].material_idx,
            .particle_index = i,
        };
    }

    MASSERT(particle_render->particle_draw_count <= particle_packet->particle_count);


    //TODO: sort the particles by shader key
    // qsort(particle_render->particle_draw, particle_render->particle_draw_count, sizeof(Vulkan_Particle_Draw), vulkan_particle_sort_key);


    //upload render data
    vulkan_buffer_frame_staging_upload(
        renderer, particle_render->particle_render_data_buffer_handle,
        command_buffer,
        particle_packet->particles,
        sizeof(Particle) * particle_packet->particle_count);


    //upload particle draw data
    vulkan_buffer_frame_staging_upload(
        renderer, particle_render->particle_draw_buffer_handle,
        command_buffer,
        particle_draw_data,
        // sizeof(Vulkan_Particle_Draw) * particle_packet->particle_count);
        sizeof(Particle_Renderable) * particle_render->particle_draw_count);


    VkBufferMemoryBarrier2 particle_render_data_barrier = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
        .srcStageMask = VK_PIPELINE_STAGE_2_COPY_BIT,
        .srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,

        .dstStageMask = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
        .dstAccessMask = VK_ACCESS_2_SHADER_STORAGE_READ_BIT,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,

        .buffer = vulkan_buffer_get_frame(renderer, particle_render->particle_render_data_buffer_handle)->handle,
        .offset = 0,
        .size = VK_WHOLE_SIZE,
    };

    VkBufferMemoryBarrier2 particle_draw_data_barrier = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
        .srcStageMask = VK_PIPELINE_STAGE_2_COPY_BIT,
        .srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,

        .dstStageMask = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
        .dstAccessMask = VK_ACCESS_2_SHADER_STORAGE_READ_BIT,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,

        .buffer = vulkan_buffer_get_frame(renderer, particle_render->particle_draw_buffer_handle)->handle,
        .offset = 0,
        .size = VK_WHOLE_SIZE,
    };


    vulkan_command_add_buffer_barrier(command_buffer, particle_render_data_barrier);
    vulkan_command_add_buffer_barrier(command_buffer, particle_draw_data_barrier);


    vulkan_command_flush_barriers(command_buffer);

    vulkan_command_buffer_debug_label_end(renderer, command_buffer);


    scratch_allocator_end(scratch);


    PROFILE_ZONE_END(particle_renderer_upload_data_draw)
}


void particle_renderer_batch_draw(Renderer* renderer, Particle_Render* particle_render,
                                  Vulkan_Command_Buffer* command_buffer)
{
    if (particle_render->particle_draw_count == 0) { return; }

    vulkan_command_buffer_debug_label_begin(renderer, command_buffer, "PARTICLE DRAW");

    /*
     u64 current_key = particle_render->particle_draw[0].shader_key;
    u64 last_idx = 0;
    u64 current_idx = 0;

    while (last_idx <= particle_render->particle_draw_count)
    {
        if (current_key != particle_render->particle_draw[current_idx].shader_key)
        {
            //find the batch and do a draw over the range
            Vulkan_Shader_Batch* batch = vulkan_shader_system_shader_get_by_key(
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
    //TODO: we want to issue a draw for the last bit of data left*/


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
        .particle_render_data_buffer = get_buffer_device_address(renderer->logical_device,
                                                                 vulkan_buffer_get_frame(
                                                                     renderer,
                                                                     particle_render->
                                                                     particle_render_data_buffer_handle)->
                                                                 handle),
        .particle_draw_buffer = get_buffer_device_address(renderer->logical_device,
                                                          vulkan_buffer_get_frame(
                                                              renderer,
                                                              particle_render->particle_draw_buffer_handle)->handle),
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
              particle_render->particle_draw_count, 0,
              0);


    vulkan_command_buffer_debug_label_end(renderer, command_buffer);
}
