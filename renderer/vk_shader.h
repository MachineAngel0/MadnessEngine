#ifndef VULKAN_SHADER_OBJECT_H
#define VULKAN_SHADER_OBJECT_H

#include "vulkan_struct_types.h"

VkShaderModule create_shader_module(const Vulkan_Context* context, const u8* shader_bytes, u64 shader_size, Renderer* renderer);


void vulkan_default_shader_destroy(Vulkan_Context* context, Vulkan_Shader_Pipeline* pipeline,
                                   VkDescriptorSetLayout* descriptor_set_layout);

void vulkan_default_shader_pipeline_bind(Vulkan_Command_Buffer* command_buffer, Vulkan_Shader_Pipeline* pipeline);


bool mesh_pipeline_create(Renderer* renderer, Vulkan_Shader_Pipeline* mesh_indirect_pipeline, vulkan_pipeline_cache*
                                 pipeline_cache);

bool sk_mesh_pipeline_create(Renderer* renderer, Vulkan_Shader_Pipeline* skinned_mesh_pipeline, vulkan_pipeline_cache*
                                 pipeline_cache);


//NOTE: Might be able to change these into more generic sprites
bool ui_shader_create(Renderer* renderer, Vulkan_Shader_Pipeline* ui_pipeline, vulkan_pipeline_cache* pipeline_cache);
bool text_shader_create(Renderer* renderer, Vulkan_Shader_Pipeline* text_pipeline, vulkan_pipeline_cache* pipeline_cache);
bool sprite_shader_create(Renderer* renderer, Vulkan_Shader_Pipeline* sprite_pipeline, vulkan_pipeline_cache* pipeline_cache);



#endif
