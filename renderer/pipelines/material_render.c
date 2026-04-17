#include "material_render.h"


Material_Renderer* material_renderer_init(Renderer* renderer, Resource_System* resource_system)
{
    Material_Renderer* material_renderer = arena_alloc(&renderer->arena, sizeof(Mesh_Renderer));


    material_renderer->instance_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                     BUFFER_TYPE_CPU_STORAGE,
                                                                     sizeof(Material_Instance) * MAX_MATERIAL);
    material_renderer->instance_staging_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                             BUFFER_TYPE_STAGING,
                                                                             sizeof(Material_Instance) *
                                                                             MAX_MATERIAL);


    material_renderer->pbr_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                BUFFER_TYPE_CPU_STORAGE,
                                                                sizeof(Material_PBR) * MAX_MATERIAL);
    material_renderer->pbr_staging_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                        BUFFER_TYPE_STAGING,
                                                                        sizeof(Material_PBR) * MAX_MATERIAL);

    return material_renderer;
}

void material_renderer_upload_data(Renderer* renderer, Material_Renderer* material_renderer,
                                        Render_Packet_Material* render_packet_material,
                                        vulkan_command_buffer* command_buffer)
{

    vulkan_buffer_reset_offset(renderer, material_renderer->instance_staging_buffer_handle);
    vulkan_buffer_reset_offset(renderer, material_renderer->pbr_staging_buffer_handle);


    //upload every frame
    vulkan_buffer_cpu_to_gpu_copy_and_upload_batch(renderer, material_renderer->instance_buffer_handle,
                                               material_renderer->instance_staging_buffer_handle, command_buffer,
                                               render_packet_material->material_instance,
                                               render_packet_material->material_instance_bytes);

    vulkan_buffer_cpu_to_gpu_copy_and_upload_batch(renderer, material_renderer->pbr_buffer_handle,
                                                   material_renderer->pbr_staging_buffer_handle, command_buffer,
                                                   render_packet_material->prb,
                                                   render_packet_material->prb_bytes);
}
