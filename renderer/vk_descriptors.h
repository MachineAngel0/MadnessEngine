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

void descriptor_pool_alloc_bindless(vulkan_context* context, descriptor_pool_allocator* descriptor_pools,
                                    VkDescriptorSetLayout* set_layout, u32* descriptor_set_count,
                                    VkDescriptorSet* out_descriptors);



void createDescriptorsTexture_reflect_test(vulkan_context* context,
                                           descriptor_pool_allocator* descriptor_pool_allocator,
                                           vulkan_shader_texture* shader_texture);

void update_descriptors_texture_reflect_test(vulkan_context* context,
                                           descriptor_pool_allocator* descriptor_pool_allocator,
                                           vulkan_shader_texture* shader_texture);




void createDescriptorsMesh(vulkan_context* context, descriptor_pool_allocator* descriptor_pool_allocator,
                           vulkan_mesh_default* default_mesh);


void create_global_texture_bindless_descriptor_set(vulkan_context* context,
                                           descriptor_pool_allocator* descriptor_pool_allocator,
                                           vulkan_bindless_texture_descriptors* texture_descriptors,
                                           vulkan_shader_texture* test_texture);

void update_global_texture_bindless_descriptor_set(vulkan_context* context,
                                           vulkan_bindless_texture_descriptors* texture_descriptors,
                                           vulkan_shader_texture* test_texture);
#endif
