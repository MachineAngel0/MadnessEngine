//
// Created by Adams Humbert on 9/10/2025.
//

#ifndef VK_DESCRIPTOR_H
#define VK_DESCRIPTOR_H

#include "texture.h"
#include "vk_device.h"


struct Descriptor {
    VkDescriptorSetLayout descriptor_set_layout;
    VkDescriptorPool descriptor_pool;
    std::vector<VkDescriptorSet> descriptor_sets;
};

void create_descriptor_set_layout(Vulkan_Context& vulkan_context, Descriptor& descriptor);
void create_descriptor_pool(Vulkan_Context& vulkan_context, Descriptor& descriptor);
void create_descriptor_sets(Vulkan_Context& vulkan_context, Texture& texture, Descriptor& descriptor);



void create_descriptor_set_layout_text(Vulkan_Context& vulkan_context, Descriptor& descriptor);
void create_descriptor_pool_text(Vulkan_Context& vulkan_context, Descriptor& descriptor);
void create_descriptor_sets_text(Vulkan_Context& vulkan_context, Texture& texture, Descriptor& descriptor);


#endif //VK_DESCRIPTOR_H
