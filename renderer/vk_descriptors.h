#ifndef VK_DESCRIPTOR_H
#define VK_DESCRIPTOR_H

#include "vulkan_types.h"
#include "SPIRV-Reflect-main/spirv_reflect.h"


//TODO:move out when done


void descriptor_pool_allocator_init(vulkan_context* context, descriptor_pool_allocator* descriptor_pools);

void descriptor_pool_allocator_destroy(vulkan_context* context, descriptor_pool_allocator* descriptor_pools);

void descriptor_pool_allocator_clear(vulkan_context* context, descriptor_pool_allocator* descriptor_pools);

void descriptor_pool_alloc(vulkan_context* context, descriptor_pool_allocator* descriptor_pools,
                           VkDescriptorSetLayout* set_layout, const u32* descriptor_set_count,
                           VkDescriptorSet* out_descriptors);


//Todo: create an out struct for all the data i would want
typedef struct spirv_refect_info
{
    int descriptor_set_count;
    int descriptor_binding_count;
    int descriptor_set_types;
    //etc...
} spirv_refect_info;

typedef struct spirv_reflect_descriptor_set_info
{
    SpvReflectShaderStageFlagBits* stage_flags;
    u32 descriptor_set_count;
    u32* binding_number;
    SpvReflectDescriptorType* descriptor_set_types;

} spirv_reflect_descriptor_set_info;

typedef struct spirv_reflect_input_variable_info
{
    u32 input_count;
    u32* locations;
    SpvReflectFormat* formats;
    u32* offsets;

} spirv_reflect_input_variable_info;


spirv_refect_info* spriv_reflection_testing(const char* shader_path);


spirv_reflect_descriptor_set_info* spriv_reflect_get_descriptor_set(Frame_Arena* arena,
    const char* vertex_shader_path, const char* fragment_shader_path);

spirv_reflect_input_variable_info* spriv_reflect_get_input_variable(
    Frame_Arena* arena, const char* shader_path);

void createDescriptorsTexture_reflect_test(vulkan_context* context,
                                           descriptor_pool_allocator* descriptor_pool_allocator,
                                           vulkan_shader_texture* shader_texture);

void update_descriptors_texture_reflect_test(vulkan_context* context,
                                           descriptor_pool_allocator* descriptor_pool_allocator,
                                           vulkan_shader_texture* shader_texture);

void createDescriptorsMesh(vulkan_context* context, descriptor_pool_allocator* descriptor_pool_allocator,
                           vulkan_mesh_default* default_mesh);

#endif
