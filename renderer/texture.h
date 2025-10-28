
#ifndef TEXTURE_H
#define TEXTURE_H

#include <vulkan/vulkan.h>


struct Vulkan_Context;
struct Command_Buffer_Context;
struct Text_System;

struct Texture
{
    VkImage texture_image;
    VkDeviceMemory texture_image_memory;
    VkImageView texture_image_view;
    VkSampler texture_sampler;
};

/*TEXTURE IMAGE*/
void create_texture_image(Vulkan_Context& vulkan_context, Command_Buffer_Context& command_buffer_context, Texture& texture, const char* filepath);
void create_image(Vulkan_Context& vulkan_context, Texture& texture, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
void transition_image_layout(Vulkan_Context& vulkan_context, Command_Buffer_Context& command_buffer_context, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
void copyBufferToImage(Vulkan_Context& vulkan_context, Command_Buffer_Context& command_buffer_context, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);


/*Texture Image Views*/
void create_texture_image_view(Vulkan_Context& vulkan_context, Texture& texture, VkFormat format);

/*Texture Sampler*/
void create_texture_sampler(Vulkan_Context& vulkan_context, Texture& texture);


/*FOR TEXT*/
void create_texture_glyph(Vulkan_Context& vulkan_context, Command_Buffer_Context& command_buffer_context, Texture& texture, const unsigned char* pixel_data, uint32_t width, uint32_t height);

#endif //TEXTURE_H

