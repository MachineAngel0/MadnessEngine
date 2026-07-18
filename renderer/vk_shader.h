#ifndef VULKAN_SHADER_OBJECT_H
#define VULKAN_SHADER_OBJECT_H

VkShaderModule create_shader_module(const vulkan_context* context, const u8* shader_bytes, u64 shader_size);


void vulkan_default_shader_destroy(vulkan_context* context, Vulkan_Shader_Pipeline* pipeline,
                                   VkDescriptorSetLayout* descriptor_set_layout);

void vulkan_default_shader_pipeline_bind(vulkan_command_buffer* command_buffer, Vulkan_Shader_Pipeline* pipeline);


bool mesh_pipeline_create(Renderer* renderer, Vulkan_Shader_Pipeline* mesh_indirect_pipeline, vulkan_pipeline_cache*
                                 pipeline_cache);

bool sk_mesh_pipeline_create(Renderer* renderer, Vulkan_Shader_Pipeline* skinned_mesh_pipeline, vulkan_pipeline_cache*
                                 pipeline_cache);


//NOTE: Might be able to change these into more generic sprites
bool ui_shader_create(Renderer* renderer, Vulkan_Shader_Pipeline* ui_pipeline, vulkan_pipeline_cache* pipeline_cache);
bool text_shader_create(Renderer* renderer, Vulkan_Shader_Pipeline* text_pipeline, vulkan_pipeline_cache* pipeline_cache);
bool sprite_shader_create(Renderer* renderer, Vulkan_Shader_Pipeline* sprite_pipeline, vulkan_pipeline_cache* pipeline_cache);



#endif
