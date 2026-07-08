#include "material_render.h"



Material_Renderer* material_renderer_init(Renderer* renderer, Resource_System* resource_system)
{
    Material_Renderer* material_renderer = allocator_alloc(&renderer->arena, sizeof(Mesh_Renderer));



    material_renderer->pbr_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                BUFFER_TYPE_CPU_STORAGE,
                                                                sizeof(Material_Default) * MAX_MATERIAL);
    material_renderer->pbr_staging_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                        BUFFER_TYPE_STAGING,
                                                                        sizeof(Material_Default) * MAX_MATERIAL);

    u64 transform_buffer_memory_size = MAX_TRANSFORM_COUNT * sizeof(mat4);

    material_renderer->transform_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                   BUFFER_TYPE_CPU_STORAGE, transform_buffer_memory_size);
    material_renderer->transform_staging_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                               BUFFER_TYPE_STAGING,
                                                               transform_buffer_memory_size);



    return material_renderer;
}

void material_renderer_upload_data(Renderer* renderer, Material_Renderer* material_renderer,
                                   Render_Packet* render_packet,
                                   vulkan_command_buffer* command_buffer)
{

    vulkan_buffer_reset_offset(renderer, material_renderer->pbr_staging_buffer_handle);


    //upload every frame

    vulkan_buffer_cpu_to_gpu_copy_and_upload_batch(renderer, material_renderer->pbr_buffer_handle,
                                                   material_renderer->pbr_staging_buffer_handle, command_buffer,
                                                   render_packet->material_data_packet.prb,
                                                   render_packet->material_data_packet.prb_bytes);



    //transform data
    vulkan_buffer_reset_offset(renderer, material_renderer->transform_staging_buffer_handle);

    //upload every frame
    vulkan_buffer_cpu_to_gpu_copy_and_upload_batch(renderer, material_renderer->transform_buffer_handle,
                                                   material_renderer->transform_staging_buffer_handle, command_buffer,
                                                   render_packet->transform_data_packet.world_space_matrix_array,
                                                   sizeof(mat4) * render_packet->transform_data_packet.
                                                   world_space_matrix_count);
}
