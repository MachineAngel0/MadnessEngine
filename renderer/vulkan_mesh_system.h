#ifndef VULKAN_MESH_SYSTEM_H
#define VULKAN_MESH_SYSTEM_H

Vulkan_Mesh_System* vulkan_mesh_system_init(Renderer* renderer);


void vulkan_mesh_system_update_async_resources(Renderer* renderer, Vulkan_Mesh_System* mesh_system);

void vulkan_mesh_system_upload_new_resources(Renderer* renderer, Vulkan_Mesh_System* mesh_system, Render_Packet* render_packet);





void mesh_renderer_construct_batch_draw(Renderer* renderer,
                                        Render_Packet* render_packet,
                                        Vulkan_Command_Buffer* command_buffer);

void mesh_renderer_batch_draw(Renderer* renderer, Vulkan_Mesh_System* mesh_renderer,
                              Vulkan_Shader_Batch* batch_draw_data, u32 batch_draw_count,
                              Vulkan_Command_Buffer* command_buffer);


//TODO: should be pass in a custom push constant??? since we will likely need it
void mesh_renderer_batch_draw_custom_pipeline(Renderer* renderer, Vulkan_Mesh_System* mesh_renderer,
                                              Vulkan_Shader_Batch* batch_draw_data, u32 batch_draw_count,
                                              Vulkan_Command_Buffer* command_buffer,
                                              Vulkan_Shader_Pipeline* shader_pipeline);

bool mesh_system_upload_data_into_buffers(Renderer* renderer, Vulkan_Mesh_System* mesh_system,
                             Vulkan_Command_Buffer* transfer_command_buffer,
                             Buffer_Handle handle,
                             Vulkan_Mesh_Data_Type type,
                             void* data,
                             u64 data_byte_size,
                             u32 submesh_id,
                             u64 mesh_upload_semaphore_value,
                             Mesh_Render_Record* record,
                             bool is_initial_submit);






#endif //VULKAN_MESH_SYSTEM_H
