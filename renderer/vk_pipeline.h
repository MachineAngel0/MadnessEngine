#ifndef VK_PIPELINE_H
#define VK_PIPELINE_H


#include <stdbool.h>
#include "vulkan_types.h"


bool vulkan_graphics_pipeline_create(
    vulkan_context* context,
    vulkan_renderpass* renderpass,
    u32 attribute_count,
    VkVertexInputAttributeDescription* attributes,
    u32 descriptor_set_layout_count,
    VkDescriptorSetLayout* descriptor_set_layouts,
    u32 stage_count,
    VkPipelineShaderStageCreateInfo* stages,
    VkViewport viewport,
    VkRect2D scissor,
    b8 is_wireframe,
    vulkan_shader_pipeline* out_pipeline);

void vulkan_pipeline_destroy(vulkan_context* context, vulkan_shader_pipeline* pipeline);

void vulkan_pipeline_bind(command_buffer* command_buffer, VkPipelineBindPoint bind_point, vulkan_shader_pipeline* pipeline);

#endif
