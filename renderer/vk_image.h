#ifndef TEXTURE_H
#define TEXTURE_H
#include "vk_buffer.h"


void vulkan_image_create(vulkan_context* context,
                         u32 width, u32 height,
                         VkFormat format, VkImageTiling tiling,
                         VkImageUsageFlags usage,
                         VkMemoryPropertyFlags memory_flags,
                         b32 create_view,
                         VkImageAspectFlags view_aspect_flags,
                         Texture* out_texture);


void vulkan_image_view_create(vulkan_context* context, VkFormat format,
                              VkImageAspectFlags aspect_flags, Texture* texture);

void vulkan_image_destroy(vulkan_context* context, Texture* image);



void create_texture_image(vulkan_context* context, vulkan_command_buffer* command_buffer,
                          const char* filepath, Texture* out_texture);

void transition_image_layout(vulkan_context* vulkan_context, vulkan_command_buffer* command_buffer_context,
    VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

void copyBufferToImage(vulkan_context* vulkan_context, vulkan_command_buffer* command_buffer_context, VkBuffer buffer,
    VkImage image, uint32_t width, uint32_t height);

void create_texture_sampler(renderer* renderer, Texture* texture);

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

void create_texture_glyph(renderer* renderer, vulkan_command_buffer* command_buffer,
                          Texture* texture, const unsigned char* pixel_data, uint32_t width, uint32_t height)
{

    //text_system.glyph_textures[text_system.glyphs[glyph]] = texture;
    printf("IMAGE SIZE: width %d height  %d \n" ,width, height);
    VkDeviceSize imageSize = width * height * 4; // 4 stride rgba

    MASSERT(pixel_data);

    //create a staging buffer
    // vulkan_buffer_create(renderer, renderer->buffer_system, BUFFER_TYPE_STAGING, imageSize);


    //create a staging buffer
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    buffer_create(&renderer->context, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer,
                  &stagingBufferMemory);

    //allocate memory
    void* data;
    vkMapMemory(renderer->context.device.logical_device, stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixel_data, imageSize);
    vkUnmapMemory(renderer->context.device.logical_device, stagingBufferMemory);

    //create texture image
    vulkan_image_create(&renderer->context, width, height, VK_FORMAT_R8G8B8A8_SRGB,
                 VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                 false, VK_IMAGE_ASPECT_COLOR_BIT, texture);

    transition_image_layout(&renderer->context, command_buffer, texture->texture_image,
                            VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyBufferToImage(&renderer->context, command_buffer, stagingBuffer, texture->texture_image,
                      width, height);
    transition_image_layout(&renderer->context, command_buffer, texture->texture_image,
                            VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vulkan_image_view_create(&renderer->context,  VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, texture);
    create_texture_sampler(renderer, texture);


}




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
