#ifndef MESH_RENDER_H
#define MESH_RENDER_H


MAPI Mesh_Renderer* mesh_renderer_init(Renderer* renderer, Resource_System* resource_system);

MAPI void mesh_renderer_upload_draw_data(Renderer* renderer, Mesh_Renderer* mesh_renderer, Render_Packet* render_packet,
                                         vulkan_command_buffer* command_buffer);
MAPI void mesh_renderer_upload_per_frame_data(Renderer* renderer, Mesh_Renderer* mesh_renderer,
                                              Render_Packet* render_packet, vulkan_command_buffer* command_buffer);

void mesh_renderer_upload_per_frame_data(Renderer* renderer, Mesh_Renderer* mesh_renderer,
                                         Render_Packet* render_packet, vulkan_command_buffer* command_buffer);

void mesh_renderer_construct_batch_draw(Renderer* renderer, Mesh_Renderer* mesh_renderer,
                                        Shader_System* shader_system,
                                        Render_Packet* render_packet,
                                        vulkan_command_buffer* command_buffer);

void mesh_renderer_batch_draw(Renderer* renderer, Mesh_Renderer* mesh_renderer,
                              Vulkan_Shader_Batch* batch_draw_data, u32 batch_draw_count,
                              vulkan_command_buffer* command_buffer);

#endif //MESH_RENDER_H
