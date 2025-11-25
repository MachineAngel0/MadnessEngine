#ifndef TEXTURE_H
#define TEXTURE_H

#include <vulkan/vulkan.h>
#include "vulkan_types.h"





void vulkan_image_create(vulkan_context* context,
                         u32 width, u32 height,
                         VkFormat format, VkImageTiling tiling,
                         VkImageUsageFlags usage,
                         VkMemoryPropertyFlags memory_flags,
                         b32 create_view,
                         VkImageAspectFlags view_aspect_flags,
                         vulkan_image* out_image);


void vulkan_image_view_create(vulkan_context* context, VkFormat format, vulkan_image* image,
                              VkImageAspectFlags aspect_flags);

void vulkan_image_destroy(vulkan_context* context, vulkan_image* image);



void create_texture_image(vulkan_context* context, vulkan_command_buffer* command_buffer,
                          const char* filepath, Texture* out_texture);

void transition_image_layout(vulkan_context* vulkan_context, vulkan_command_buffer* command_buffer_context,
    VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

void copyBufferToImage(vulkan_context* vulkan_context, vulkan_command_buffer* command_buffer_context, VkBuffer buffer,
    VkImage image, uint32_t width, uint32_t height);
/*TEXTURE IMAGE*/

//
// void create_image(vulkan_context* vulkan_context, Texture* texture, uint32_t width, uint32_t height, VkFormat format,
//                   VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
//
// void transition_image_layout(vulkan_context* vulkan_context, Command_Buffer_Context& command_buffer_context,
//                              VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
//
// void copyBufferToImage(vulkan_context* vulkan_context, Command_Buffer_Context& command_buffer_context, VkBuffer buffer,
//                        VkImage image, uint32_t width, uint32_t height);
//
//
// /*Texture Image Views*/
// void create_texture_image_view(vulkan_context& vulkan_context, Texture& texture, VkFormat format);
//
// /*Texture Sampler*/
// void create_texture_sampler(vulkan_context& vulkan_context, Texture& texture);
//
//
// /*FOR TEXT*/
// void create_texture_glyph(vulkan_context& vulkan_context, Command_Buffer_Context& command_buffer_context,
//                           Texture& texture, const unsigned char* pixel_data, uint32_t width, uint32_t height);

#endif //TEXTURE_H
