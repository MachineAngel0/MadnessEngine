#ifndef MESH_RENDER_H
#define MESH_RENDER_H


MAPI Mesh_Renderer* mesh_renderer_init(Renderer* renderer, Resource_System* resource_system);

MAPI void mesh_renderer_upload_draw_data(Renderer* renderer, Mesh_Renderer* mesh_renderer, Render_Packet* render_packet, vulkan_command_buffer* command_buffer);


void mesh_renderer_construct_indirect_draw(Renderer* renderer, Mesh_Renderer* mesh_renderer, Render_Packet* render_packet, vulkan_command_buffer* command_buffer);


MAPI void mesh_renderer_draw(Renderer* renderer, Mesh_Renderer* mesh_renderer, vulkan_command_buffer* command_buffer,
                      vulkan_shader_pipeline* pipeline);


//

void mesh_renderer_construct_skinned_indirect_draw_new(Renderer* renderer, Mesh_Renderer* mesh_renderer,
                                                      Render_Packet* render_packet,
                                                      vulkan_command_buffer* command_buffer);
void mesh_renderer_skinned_draw(Renderer* renderer, Mesh_Renderer* mesh_renderer, vulkan_command_buffer* command_buffer,
                                vulkan_shader_pipeline* pipeline);

#endif //MESH_RENDER_H
