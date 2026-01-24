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

/*
void createDescriptorsTexture_reflect_test(vulkan_context* context,
                                           descriptor_pool_allocator* descriptor_pool_allocator,
                                           vulkan_shader_texture* shader_texture);

void update_descriptors_texture_reflect_test(vulkan_context* context,
                                             descriptor_pool_allocator* descriptor_pool_allocator,
                                             vulkan_shader_texture* shader_texture);
*/

void createDescriptorsMesh(renderer* renderer, descriptor_pool_allocator* descriptor_pool_allocator,
                           vulkan_bindless_descriptors* uniform_descriptors, const mesh* in_mesh);

void create_texture_bindless_descriptor_set(vulkan_context* context,
                                            descriptor_pool_allocator* descriptor_pool_allocator,
                                            vulkan_bindless_descriptors* texture_descriptors);


void update_global_texture_bindless_descriptor_set(vulkan_context* context,
                                                   vulkan_bindless_descriptors* texture_descriptors,
                                                   Texture* texture, u32 array_index);

void create_bindless_uniform_buffer_descriptor_set(vulkan_context* context,
                                                   descriptor_pool_allocator* descriptor_pool_allocator,
                                                   vulkan_bindless_descriptors* uniform_descriptors);

void update_uniform_buffer_bindless_descriptor_set(vulkan_context* context,
                                                          vulkan_bindless_descriptors* uniform_descriptors,
                                                          vulkan_buffer_gpu* buffer, u64 data_size, u32 array_index);

//TODO: we need a function of some sort to return a handle so that we can use it for later updates
//NOTE: one isn't needed for textures because the shader_system handles that
// descriptor_uniform_handle descriptors_retrieve_uniform_available_handle(renderer* renderer);

#endif
