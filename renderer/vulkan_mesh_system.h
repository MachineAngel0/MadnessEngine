#ifndef VULKAN_MESH_SYSTEM_H
#define VULKAN_MESH_SYSTEM_H

Vulkan_Mesh_System* vulkan_mesh_system_init(Renderer* renderer);


void vulkan_mesh_system_update_async_resources(Renderer* renderer, Vulkan_Mesh_System* mesh_system);

void vulkan_mesh_system_upload_new_resources(Renderer* renderer, Vulkan_Mesh_System* mesh_system, Render_Packet* render_packet);

bool mesh_system_upload_data_into_buffers(Renderer* renderer, Vulkan_Mesh_System* mesh_system,
                             Vulkan_Command_Buffer* transfer_command_buffer,
                             Buffer_Handle handle,
                             Vulkan_Mesh_Data_Type type,
                             void* data,
                             u64 data_byte_size,
                             u32 mesh_id,
                             u64 mesh_upload_semaphore_value,
                             Mesh_Render_Record* record,
                             bool is_initial_submit);




void mesh_renderer_construct_draw_lists(Renderer* renderer,
                                        Render_Packet* render_packet,
                                        Vulkan_Command_Buffer* graphics_command_buffer, Vulkan_Mesh_Render_List* render_list);

void mesh_renderer_batch_draw(Renderer* renderer, Vulkan_Mesh_System* mesh_renderer,
                              Vulkan_Command_Buffer* command_buffer, Vulkan_Mesh_Render_List* render_list);



//TODO: should be pass in a custom push constant??? since we will likely need it
void mesh_renderer_batch_draw_custom_pipeline(Renderer* renderer, Vulkan_Mesh_System* mesh_renderer,
                                              Vulkan_Command_Buffer* command_buffer,
                                              Vulkan_Shader_Pipeline* vulkan_pipeline,
                                              Vulkan_Mesh_Render_List* render_list);










#endif //VULKAN_MESH_SYSTEM_H
