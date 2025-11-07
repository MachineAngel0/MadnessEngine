//
// Created by Adams Humbert on 9/10/2025.
//

#ifndef VK_DESCRIPTOR_H
#define VK_DESCRIPTOR_H

#include "vk_image.h"
#include "vk_device.h"


struct Descriptor {
    VkDescriptorSetLayout descriptor_set_layout;
    VkDescriptorPool descriptor_pool;
    std::vector<VkDescriptorSet> descriptor_sets;
};

void create_descriptor_set_layout(vulkan_context& vulkan_context, Descriptor& descriptor);
void create_descriptor_pool(vulkan_context& vulkan_context, Descriptor& descriptor);
void create_descriptor_sets(vulkan_context& vulkan_context, Texture& texture, Descriptor& descriptor);



void create_descriptor_set_layout_text(vulkan_context& vulkan_context, Descriptor& descriptor);
void create_descriptor_pool_text(vulkan_context& vulkan_context, Descriptor& descriptor);
void create_descriptor_sets_text(vulkan_context& vulkan_context, Texture& texture, Descriptor& descriptor);


#endif //VK_DESCRIPTOR_H
