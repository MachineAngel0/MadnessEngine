#ifndef SHADER_SYSTEM_H
#define SHADER_SYSTEM_H


//TODO: increase later when it becomes relevant
#define SHADER_SYSTEM_CAPACITY 100lu
#include "vulkan_struct_types.h"


Vulkan_Shader_System* vulkan_shader_system_init(Renderer* renderer);


void vulkan_shader_system_shutdown(Vulkan_Shader_System* system);


void vulkan_shader_system_upload_frame_data(Renderer* renderer, Vulkan_Shader_System* shader_system,
                                            Render_Packet* render_packet)
{
}

void vulkan_shader_system_update(Renderer* renderer, Vulkan_Shader_System* shader_system, Render_Packet* render_packet);


//Shader Batch system

void vulkan_shader_system_shader_batch_create(Renderer* renderer, Vulkan_Shader_System* shader_system,
                                              Shader_Asset* shader_asset);

void vulkan_shader_system_shader_batch_free(Renderer* renderer, Vulkan_Shader_System* shader_system,
                                            const char* shader_name);

Vulkan_Shader_Batch* vulkan_shader_system_shader_get_by_key(Renderer* renderer,
                                                            u64 sort_key);

#endif //SHADER_SYSTEM_H
