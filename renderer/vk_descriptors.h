#ifndef VK_DESCRIPTOR_H
#define VK_DESCRIPTOR_H

#include "vulkan_types.h"


typedef enum Descriptor_Type
{
    Descriptor_Type_UNIFORM,
    Descriptor_Type_TEXTURE,
    Descriptor_Type_STORAGE,
    Descriptor_Type_ATTACHMENT,
    Descriptor_Type_MAX,
} Descriptor_Type;

#define max_bindless_texture_resource 1024u //4096u //might want to bump this up at some point
#define max_uniform_buffer_resources 100u
#define max_storage_buffer_resources 100u
#define max_attachment_resources 100u

u32 descriptor_type_to_size[Descriptor_Type_MAX] = {
    [Descriptor_Type_UNIFORM] = max_uniform_buffer_resources,
    [Descriptor_Type_TEXTURE] = max_bindless_texture_resource,
    [Descriptor_Type_STORAGE] = max_storage_buffer_resources,
    [Descriptor_Type_ATTACHMENT] = max_attachment_resources
};

VkDescriptorType descriptor_type_lookup[Descriptor_Type_MAX] = {
    [Descriptor_Type_UNIFORM] = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    [Descriptor_Type_TEXTURE] = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
    [Descriptor_Type_STORAGE] = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
    [Descriptor_Type_ATTACHMENT] = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT
};

Descriptor_System* descriptor_pool_allocator_init(Renderer* renderer);

void descriptor_pool_allocator_destroy(Renderer* renderer, Descriptor_System* descriptor_system);

void descriptor_pool_allocator_clear(Renderer* renderer, Descriptor_System* descriptor_system);

void descriptor_pool_alloc(Renderer* renderer, Descriptor_System* descriptor_system,
                           VkDescriptorSetLayout* set_layout, const u32* descriptor_set_count,
                           VkDescriptorSet* out_descriptors);

void descriptor_pool_alloc_bindless(Renderer* renderer, Descriptor_System* descriptor_system,
                                    Descriptor_Type descriptor_type,
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
void create_bindless_uniform_buffer_descriptor_set(Renderer* renderer,
                                                   Descriptor_System* descriptor_pool_allocator,
                                                   vulkan_bindless_descriptors* uniform_descriptors);

void create_texture_bindless_descriptor_set(Renderer* renderer,
                                            Descriptor_System* descriptor_pool_allocator,
                                            vulkan_bindless_descriptors* texture_descriptors);

void create_bindless_storage_buffer_descriptor_set(Renderer* renderer,
                                                   Descriptor_System* descriptor_pool_allocator,
                                                   vulkan_bindless_descriptors* storage_descriptors);

// UPDATE
//NOTE: BINDING INDEX IS NOT IN USE RN
void update_uniform_buffer_bindless_descriptor_set(Renderer* renderer,
                                                   Descriptor_System* descriptor_system,
                                                   Buffer_Handle buffer_handle,
                                                   u32 binding_index);


void update_texture_bindless_descriptor_set(Renderer* renderer,
                                            Descriptor_System* descriptor_system,
                                            Texture_Handle texture_handle);

void update_storage_buffer_bindless_descriptor_set(Renderer* renderer,
                                                   Descriptor_System* descriptor_system,
                                                   Buffer_Handle buffer_handle,
                                                   u32 binding_index);


#endif
