#ifndef VK_PIPELINE_H
#define VK_PIPELINE_H

#include "vk_shader.h"

#define SHADER_PATH "../renderer/shaders/"

//TODO: integrate wireframe, blending modes, mutltisampling, (possibly depth, i would have to learn more about it)
//making the assumption that the shader will have both .vert and .frag, throw an error otherwise
bool vulkan_pipeline_graphics_create(Renderer* renderer, const char* shader_name, vulkan_shader_pipeline* out_pipeline, Shader_Blend_Mode blend_mode);




void vulkan_pipeline_destroy(vulkan_context* context, vulkan_shader_pipeline* pipeline);






bool vulkan_pipeline_compute_create();

const char* pipeline_cache_file_path = "../renderer/pipeline_cache.bin";

#define pipeline_cache_magic_number 79846

void vulkan_pipeline_cache_read_from_file(Renderer* renderer, vulkan_pipeline_cache* pipeline_info, u8** pipeline_cache_data, size_t* pipeline_cache_size);

vulkan_pipeline_cache* vulkan_pipeline_cache_initialize(Renderer* renderer);

void vulkan_pipeline_cache_write_to_file(Renderer* renderer, vulkan_pipeline_cache* pipeline_cache);


#endif
