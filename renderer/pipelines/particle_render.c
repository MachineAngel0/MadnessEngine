#include "vulkan_struct_types.h"
#include "particle_render.h"


#include "vk_buffer.h"

Particle_Render* particle_renderer_init(Renderer* renderer)
{
    Particle_Render* particle_renderer = allocator_alloc(&renderer->allocator, sizeof(Particle_Render));

    u64 mesh_buffer_data_size = MB(16);


    particle_renderer->spherical_billboard_material_buffer_handle = vulkan_buffer_create_frame(
        renderer, renderer->buffer_system,
        BUFFER_TYPE_STORAGE_GPU,
        mesh_buffer_data_size);

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


void particle_renderer_upload_data_draw(Renderer* renderer, Particle_Render* particle_render,
                                        Render_Packet* render_packet, Vulkan_Command_Buffer* command_buffer)
{
    vulkan_buffer_frame_reset(renderer, particle_render->spherical_billboard_material_buffer_handle);

    vulkan_command_buffer_debug_label_begin(renderer, command_buffer, "PARTICLE SSBO UPDATE");

    //update the particle buffer every frame
    //OPTIMIZE: make particles material shader friendly, but obv it can wait
    Material_Spherical_Billboard billboard_spherical_material;
    particle_render->draw_count = render_packet->particle_packet.particle_count;
    for (u32 i = 0; i < render_packet->particle_packet.particle_count; i++)
    {
        billboard_spherical_material.point = render_packet->particle_packet.particles[i].position;
        billboard_spherical_material.rotation = render_packet->particle_packet.particles[i].rotation;
        billboard_spherical_material.size = render_packet->particle_packet.particles[i].scale;
        billboard_spherical_material.texture_idx = render_packet->particle_packet.particles[i].texture_handle.handle;

        // TODO:
        // billboard_spherical_material.color
        // render_packet->particle_packet.particles[i].tex_size;
        // render_packet->particle_packet.particles[i].tex_offset;

        vulkan_buffer_frame_staging_upload(
            renderer, particle_render->spherical_billboard_material_buffer_handle,
            command_buffer,
            &billboard_spherical_material,
            sizeof(Material_Spherical_Billboard));
    }

    vulkan_command_buffer_debug_label_end(renderer, command_buffer);

    Vulkan_Buffer* particle_material_buffer = vulkan_buffer_get_frame(
        renderer, particle_render->spherical_billboard_material_buffer_handle);

    VkBufferMemoryBarrier2 particle_barrier = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
        .srcStageMask = VK_PIPELINE_STAGE_2_COPY_BIT,
        .srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,

        .dstStageMask = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT |
        VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
        .dstAccessMask = VK_ACCESS_2_SHADER_STORAGE_READ_BIT,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,

        .buffer = particle_material_buffer->handle,
        .offset = 0,
        .size = VK_WHOLE_SIZE,
    };

    vulkan_command_add_buffer_barrier(command_buffer, particle_barrier);
}

void particle_renderer_batch_draw(Renderer* renderer, Particle_Render* particle_render,
                                  Vulkan_Command_Buffer* command_buffer)
{
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
        .draw_material_buffer = get_buffer_device_address(renderer->logical_device,
                                                          vulkan_buffer_get_frame(
                                                              renderer,
                                                              particle_render->
                                                              spherical_billboard_material_buffer_handle)->
                                                          handle),
        .unused = get_buffer_device_address(renderer->logical_device,
                                            vulkan_buffer_get_frame(
                                                renderer, particle_render->spherical_billboard_material_buffer_handle)->
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
