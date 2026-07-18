#ifndef VK_PIPELINE_H
#define VK_PIPELINE_H

#include "vk_shader.h"

#define SHADER_PATH "../renderer/shaders/"

//TODO: mutltisampling
//making the assumption that the shader will have both .vert and .frag, throw an error otherwise

//used in mesh and currently particles types (TODO: might move out the particles)
bool vulkan_pipeline_graphics_create(Renderer* renderer, const char* shader_name,
                                     Shader_Blend_Mode blend_mode, Renderpass_Single_Type renderpass_type,
                                     Vulkan_Shader_Pipeline* out_pipeline,
                                     Vulkan_Shader_Pipeline* out_wire_frame_pipeline);

bool vulkan_pipeline_predepth_create(Renderer* renderer, const char* shader_name, Vulkan_Shader_Pipeline* out_pipeline);


void vulkan_pipeline_destroy(vulkan_context* context, Vulkan_Shader_Pipeline* pipeline);


bool vulkan_pipeline_compute_create();

const char* pipeline_cache_file_path = "../renderer/pipeline_cache.bin";

#define pipeline_cache_magic_number 79846

void vulkan_pipeline_cache_read_from_file(Renderer* renderer, vulkan_pipeline_cache* pipeline_info,
                                          u8** pipeline_cache_data, size_t* pipeline_cache_size);

vulkan_pipeline_cache* vulkan_pipeline_cache_initialize(Renderer* renderer);

void vulkan_pipeline_cache_write_to_file(Renderer* renderer, vulkan_pipeline_cache* pipeline_cache);


#endif
