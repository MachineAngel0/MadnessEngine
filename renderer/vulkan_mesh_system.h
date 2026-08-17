#ifndef VULKAN_MESH_SYSTEM_H
#define VULKAN_MESH_SYSTEM_H


MAPI Vulkan_Mesh_System* mesh_renderer_init(Renderer* renderer);




MAPI void mesh_renderer_upload_draw_data(Renderer* renderer, Vulkan_Mesh_System* mesh_system, Render_Packet* render_packet,
                                         Vulkan_Command_Buffer* graphics_command_buffer);
MAPI void mesh_renderer_upload_per_frame_data(Renderer* renderer, Vulkan_Mesh_System* mesh_renderer,
                                              Render_Packet* render_packet, Vulkan_Command_Buffer* command_buffer);


void mesh_renderer_construct_batch_draw(Renderer* renderer,
                                         Render_Packet* render_packet,
                                         Vulkan_Command_Buffer* command_buffer);

void mesh_renderer_batch_draw(Renderer* renderer, Vulkan_Mesh_System* mesh_renderer,
                              Vulkan_Shader_Batch* batch_draw_data, u32 batch_draw_count,
                              Vulkan_Command_Buffer* command_buffer);


//TODO: should be pass in a custom push constant??? since we will likely need it
void mesh_renderer_batch_draw_custom_pipeline(Renderer* renderer, Vulkan_Mesh_System* mesh_renderer,
                                            Vulkan_Shader_Batch* batch_draw_data, u32 batch_draw_count,
                                            Vulkan_Command_Buffer* command_buffer, Vulkan_Shader_Pipeline* shader_pipeline);


#endif //MESH_RENDER_H
