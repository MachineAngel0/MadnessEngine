
#include "transform_render.h"

MAPI Transform_Renderer* transform_renderer_init(Renderer* renderer, Resource_System* resource_system)
{
    Transform_Renderer* transform_renderer = arena_alloc(&renderer->arena, sizeof(Mesh_Renderer));

    u64 transform_buffer_memory_size = MAX_TRANSFORM_COUNT * sizeof(mat4);

    transform_renderer->transform_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                   BUFFER_TYPE_CPU_STORAGE, transform_buffer_memory_size);
    transform_renderer->transform_staging_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                               BUFFER_TYPE_STAGING,
                                                               transform_buffer_memory_size);

    return transform_renderer;

}

MAPI void transform_renderer_upload_data(Renderer* renderer, Transform_Renderer* transform_renderer,
                                         Render_Packet* render_packet, vulkan_command_buffer* command_buffer)
{
    vulkan_buffer_reset_offset(renderer, transform_renderer->transform_staging_buffer_handle);

    //upload every frame
    vulkan_buffer_cpu_to_gpu_copy_and_upload_batch(renderer, transform_renderer->transform_buffer_handle,
                                                   transform_renderer->transform_staging_buffer_handle, command_buffer,
                                                   render_packet->transform_data_packet.world_space_matrix_array,
                                                   sizeof(mat4) * render_packet->transform_data_packet.
                                                   world_space_matrix_count);

}


