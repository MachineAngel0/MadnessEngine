#ifndef VK_DESCRIPTOR_H
#define VK_DESCRIPTOR_H


//TODO: it would probably just be better to pass in a buffer handle and have the update take care of the rest
//TODO: have descripor pool allocator keep track of all its global descripors and remove them from the render structs concerns

Descriptor_System* descriptor_pool_allocator_init(renderer* renderer);

void descriptor_pool_allocator_destroy(renderer* renderer, Descriptor_System* descriptor_system);

void descriptor_pool_allocator_clear(renderer* renderer, Descriptor_System* descriptor_system);

void descriptor_pool_alloc(renderer* renderer, Descriptor_System* descriptor_system,
                           VkDescriptorSetLayout* set_layout, const u32* descriptor_set_count,
                           VkDescriptorSet* out_descriptors);

void descriptor_pool_alloc_bindless(renderer* renderer, Descriptor_System* descriptor_system,
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


// CREATE
void create_bindless_uniform_buffer_descriptor_set(renderer* renderer,
                                                   Descriptor_System* descriptor_pool_allocator,
                                                   vulkan_bindless_descriptors* uniform_descriptors);

void create_texture_bindless_descriptor_set(renderer* renderer,
                                            Descriptor_System* descriptor_pool_allocator,
                                            vulkan_bindless_descriptors* texture_descriptors);

void create_bindless_storage_buffer_descriptor_set(renderer* renderer,
                                                   Descriptor_System* descriptor_pool_allocator,
                                                   vulkan_bindless_descriptors* storage_descriptors);

// UPDATE
//NOTE: BINDING INDEX IS NOT IN USE RN
void update_uniform_buffer_bindless_descriptor_set(renderer* renderer,
                                                   Descriptor_System* descriptor_system,
                                                   Buffer_Handle buffer_handle,
                                                   u32 binding_index);


void update_texture_bindless_descriptor_set(renderer* renderer,
                                            Descriptor_System* descriptor_system,
                                            Texture_Handle texture_handle);

void update_storage_buffer_bindless_descriptor_set(renderer* renderer,
                                                   Descriptor_System* descriptor_system,
                                                   Buffer_Handle buffer_handle,
                                                   u32 binding_index);




#endif
