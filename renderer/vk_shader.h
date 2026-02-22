#ifndef VULKAN_SHADER_OBJECT_H
#define VULKAN_SHADER_OBJECT_H

VkShaderModule create_shader_module(const vulkan_context* context, const char* shader_bytes, u64 shader_size);


void vulkan_default_shader_destroy(vulkan_context* context, vulkan_shader_pipeline* pipeline,
                                   VkDescriptorSetLayout* descriptor_set_layout);

void vulkan_default_shader_pipeline_bind(vulkan_command_buffer* command_buffer, vulkan_shader_pipeline* pipeline);


bool vulkan_mesh_indirect_shader_create(renderer* renderer, Mesh_System* mesh_system, vulkan_shader_pipeline* mesh_indirect_pipeline);

//NOTE: Might be able to change these into more generic sprites
bool ui_shader_create(renderer* renderer, vulkan_shader_pipeline* ui_pipeline);
bool text_shader_create(renderer* renderer, vulkan_shader_pipeline* text_pipeline);





#endif
