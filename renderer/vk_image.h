#ifndef TEXTURE_H
#define TEXTURE_H
#include "vk_buffer.h"
#include "vulkan_enum_types.h"

//TODO: look into unified image layouts
// https://www.khronos.org/blog/so-long-image-layouts-simplifying-vulkan-synchronisation



void vulkan_image_create(
    Renderer* renderer, u32 width,
    u32 height, VkFormat format,
    VkImageTiling tiling,
    VkImageUsageFlags usage,
    VkMemoryPropertyFlags memory_flags,
    b32 create_view,
    VkImageAspectFlags view_aspect_flags, Vulkan_Texture* out_texture);



void vulkan_texture_create_shadowmap(Renderer* renderer, u32 width, u32 height,
                                     VkFormat format, Vulkan_Texture* out_texture);


void vulkan_image_view_create(Renderer* renderer,
                              VkFormat format, VkImageAspectFlags aspect_flags, Vulkan_Texture* texture);

void vulkan_texture_free(Renderer* renderer, Vulkan_Texture* image);


void transition_image_layout(Vulkan_Command_Buffer* command_buffer_context,
                             VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, Renderer* renderer);

void copyBufferToImage(Renderer* renderer,
                       VkBuffer buffer, VkImage image, u32 width, u32 height);

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



VkBool32 formatIsFilterable(VkPhysicalDevice physicalDevice, VkFormat format, VkImageTiling tiling);

//does not handle any image transitions or synchronization
void vulkan_texture_create_image_new(Renderer* renderer,
                                     Texture_GPU_Upload* texture_data,
                                     Vulkan_Texture* out_texture);


void initial_image_layout_transition(Vulkan_Command_Buffer* command_buffer,
                                 VkImage image);

void second_image_layout_transition(Renderer* renderer, Vulkan_Command_Buffer* command_buffer,
                                 VkImage image,
                                 Vulkan_Queue_Type source_queue,
                                 Vulkan_Queue_Type destination_queue);
void buffer_to_image_copy_new(Vulkan_Command_Buffer* command_buffer, VkBuffer buffer,
                       VkImage image, u32 width, u32 height);
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
