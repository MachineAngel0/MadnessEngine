#include "vulkan_struct_types.h"
#include "particle_render.h"


#include "vk_buffer.h"

Particle_Render* particle_renderer_init(Renderer* renderer, Resource_System* resource_system)
{
    Particle_Render* particle_renderer = allocator_alloc(&renderer->allocator, sizeof(Particle_Render));

    u64 mesh_buffer_data_size = MB(16);


    particle_renderer->spherical_billboard_material_buffer_handle = vulkan_buffer_create(
        renderer, renderer->buffer_system,
        BUFFER_TYPE_CPU_STORAGE,
        mesh_buffer_data_size);

    //default blend for now
    vulkan_pipeline_graphics_create(renderer, "billboard_spherical", Shader_Blend_Mode_Soft_Additive,
                                    Renderpass_Type_Opaque,
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
                                        Render_Packet* render_packet, vulkan_command_buffer* command_buffer)
{
    vulkan_buffer_reset_offset(renderer, particle_render->spherical_billboard_material_buffer_handle);


    //update the particle buffer every frame
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

        vulkan_buffer_cpu_to_gpu_copy_and_upload_batch_global_staging_from_offset(
            renderer, particle_render->spherical_billboard_material_buffer_handle,
            command_buffer,
            &billboard_spherical_material,
            sizeof(Material_Spherical_Billboard));
    }
}

void particle_renderer_batch_draw(Renderer* renderer, Particle_Render* particle_render,
                                  vulkan_command_buffer* command_buffer)
{
    //uniform
    vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            particle_render->spherical_billboard_pipeline.pipeline_layout, 0, 1,
                            &renderer->descriptor_system->uniform_descriptors.descriptor_sets[renderer->context.
                                current_frame], 0, 0);

    //textures
    vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            particle_render->spherical_billboard_pipeline.pipeline_layout, 1, 1,
                            &renderer->descriptor_system->texture_descriptors.descriptor_sets[0], 0, 0);

    //storage buffers
    vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            particle_render->spherical_billboard_pipeline.pipeline_layout, 2, 1,
                            &renderer->descriptor_system->storage_descriptors.descriptor_sets[renderer->context.
                                current_frame], 0, 0);

    PC_Particle pc_particle = {
        .draw_material_buffer = get_buffer_device_address(renderer->context.device.logical_device,
                                                          vulkan_buffer_get(
                                                              renderer,
                                                              particle_render->
                                                              spherical_billboard_material_buffer_handle)->
                                                          handle),
        .unused = get_buffer_device_address(renderer->context.device.logical_device,
                                            vulkan_buffer_get(
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
