#ifndef VK_DESCRIPTOR_H
#define VK_DESCRIPTOR_H


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


void createDescriptorsMesh(renderer* renderer, descriptor_pool_allocator* descriptor_pool_allocator,
                           vulkan_bindless_descriptors* uniform_descriptors, const mesh* in_mesh);

void create_texture_bindless_descriptor_set(vulkan_context* context,
                                            descriptor_pool_allocator* descriptor_pool_allocator,
                                            vulkan_bindless_descriptors* texture_descriptors);

void update_global_texture_bindless_descriptor_set(vulkan_context* context,
                                                   vulkan_bindless_descriptors* texture_descriptors,
                                                   vulkan_shader_texture* test_texture, u32 array_index);


void create_bindless_uniform_buffer_descriptor_set(vulkan_context* context,
                                                   descriptor_pool_allocator* descriptor_pool_allocator,
                                                   vulkan_bindless_descriptors* uniform_descriptors);

void update_global_uniform_buffer_bindless_descriptor_set(vulkan_context* context,
                                                          vulkan_bindless_descriptors* uniform_descriptors,
                                                          vulkan_uniform_buffer* buffer, u32 array_index);


#endif
