#ifndef VK_PIPELINE_H
#define VK_PIPELINE_H


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
    bool is_wireframe,
    vulkan_shader_pipeline* out_pipeline);

void vulkan_pipeline_destroy(vulkan_context* context, vulkan_shader_pipeline* pipeline);

void vulkan_pipeline_graphics_bind(vulkan_command_buffer* command_buffer, vulkan_shader_pipeline* pipeline);

void vulkan_pipeline_compute_bind(vulkan_command_buffer* command_buffer, vulkan_shader_pipeline* pipeline);



const char* pipeline_cache_file_path = "../renderer/pipeline_cache.bin";

u32 pipeline_cache_magic_number = 42069;

void vulkan_pipeline_cache_read_from_file(renderer* renderer, vulkan_pipeline_cache* pipeline_info, u8** pipeline_cache_data, size_t* pipeline_cache_size);

vulkan_pipeline_cache* vulkan_pipeline_cache_initialize(renderer* renderer);

void vulkan_pipeline_cache_write_to_file(renderer* renderer, vulkan_pipeline_cache* pipeline_cache);


#endif
