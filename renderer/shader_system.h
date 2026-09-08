#ifndef SHADER_SYSTEM_H
#define SHADER_SYSTEM_H


//TODO: increase later when it becomes relevant
#define SHADER_SYSTEM_CAPACITY 100lu
#include "vulkan_struct_types.h"


Vulkan_Shader_System* vulkan_shader_system_init(Renderer* renderer);


void vulkan_shader_system_shutdown(Vulkan_Shader_System* system);


void vulkan_shader_system_update(Renderer* renderer, Vulkan_Shader_System* shader_system, Render_Packet* render_packet);


//Shader Batch system

void vulkan_shader_system_shader_batch_create(Renderer* renderer, Vulkan_Shader_System* shader_system,
                                              Material_Batch* material_batch, Material_Asset* material_asset,
                                              Material_Definition* material_definition);

void vulkan_shader_system_shader_batch_free(Renderer* renderer, Vulkan_Shader_System* shader_system,
                                            const char* shader_name);

Vulkan_Shader_Batch* vulkan_shader_system_shader_batch_get_by_mat_key_for_particles(Renderer* renderer, u64 material_key);


#endif //SHADER_SYSTEM_H
