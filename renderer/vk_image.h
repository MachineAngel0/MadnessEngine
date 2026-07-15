#ifndef TEXTURE_H
#define TEXTURE_H
#include "vk_buffer.h"

//TODO: look into unified image layouts
// https://www.khronos.org/blog/so-long-image-layouts-simplifying-vulkan-synchronisation

void vulkan_image_create(vulkan_context* context,
                         u32 width, u32 height,
                         VkFormat format, VkImageTiling tiling,
                         VkImageUsageFlags usage,
                         VkMemoryPropertyFlags memory_flags,
                         b32 create_view,
                         VkImageAspectFlags view_aspect_flags,
                         Vulkan_Texture* out_texture);

void vulkan_texture_create_from_image(vulkan_context* context, vulkan_command_buffer* command_buffer,
                          Texture* texture_data, Vulkan_Texture* out_texture);

void vulkan_texture_create_shadowmap(vulkan_context* context, u32 width, u32 height, VkFormat format,
    vulkan_command_buffer* command_buffer, Vulkan_Texture* out_texture);


void vulkan_image_view_create(vulkan_context* context, VkFormat format,
                              VkImageAspectFlags aspect_flags, Vulkan_Texture* texture);

void vulkan_image_destroy(vulkan_context* context, Vulkan_Texture* image);



void create_texture_image(vulkan_context* context, vulkan_command_buffer* command_buffer,
                          const char* filepath, Vulkan_Texture* out_texture);

void transition_image_layout(vulkan_context* vulkan_context, vulkan_command_buffer* command_buffer_context,
    VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

void copyBufferToImage(vulkan_context* vulkan_context, vulkan_command_buffer* command_buffer_context, VkBuffer buffer,
    VkImage image, uint32_t width, uint32_t height);

void create_texture_sampler(Renderer* renderer, Vulkan_Texture* texture);

void image_insert_memory_barrier(
    VkCommandBuffer cmdbuffer,
    VkImage image,
    VkAccessFlags srcAccessMask,
    VkAccessFlags dstAccessMask,
    VkImageLayout oldImageLayout,
    VkImageLayout newImageLayout,
    VkPipelineStageFlags srcStageMask,
    VkPipelineStageFlags dstStageMask,
    VkImageSubresourceRange subresourceRange);

void create_texture_glyph(Renderer* renderer, vulkan_command_buffer* command_buffer,
                          Vulkan_Texture* texture, const unsigned char* pixel_data, uint32_t width, uint32_t height);


VkBool32 formatIsFilterable(VkPhysicalDevice physicalDevice, VkFormat format, VkImageTiling tiling);

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
