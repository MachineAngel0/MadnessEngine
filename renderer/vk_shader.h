#ifndef VULKAN_SHADER_OBJECT_H
#define VULKAN_SHADER_OBJECT_H

#include <stdbool.h>
#include "defines.h"
#include "vulkan_types.h"



VkShaderModule create_shader_module(const vulkan_context* context,   const char* shader_bytes, u64 shader_size);

bool vulkan_default_shader_create(vulkan_context* context, vulkan_shader_pipeline* pipeline, VkDescriptorSetLayout* descriptor_set_layout);
void vulkan_default_shader_destroy(vulkan_context* context, vulkan_shader_pipeline* pipeline, VkDescriptorSetLayout* descriptor_set_layout);
void vulkan_default_shader_pipeline_bind(const vulkan_context* context, vulkan_shader_pipeline* pipeline);



#endif
