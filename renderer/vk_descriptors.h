#ifndef VK_DESCRIPTOR_H
#define VK_DESCRIPTOR_H

#include "vulkan_types.h"
#include "SPIRV-Reflect-main/spirv_reflect.h"


//TODO:move out when done
typedef struct descriptor_pool_allocator
{
    //darrays
    VkDescriptorPool* ready_pools;
    VkDescriptorPool* full_pools;
    u32 sets_per_pool;
} descriptor_pool_allocator;

//TODO: this is kinda wrong rn, but close to what it needs to be

void descriptor_pool_allocator_init(vulkan_context* context, descriptor_pool_allocator* descriptor_pools);

void descriptor_pool_allocator_destroy(vulkan_context* context, descriptor_pool_allocator* descriptor_pools);

void descriptor_pool_allocator_clear(vulkan_context* context, descriptor_pool_allocator* descriptor_pools);

VkDescriptorPool* descriptor_pool_create_return(vulkan_context* context, descriptor_pool_allocator* descriptor_pools,
                                                VkDescriptorType* types, u64 types_size);


void vulkan_descriptor_pool_allocate(vulkan_context* context, descriptor_pool_allocator* descriptor_pools,
                                     VkDescriptorSetLayout* set_layout, VkDescriptorType* types,
                                     VkDescriptorSet* out_descriptors);

void spriv_reflection_testing(vulkan_context* context, vulkan_shader_texture* shader_texture);


#endif
