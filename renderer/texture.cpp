#include "texture.h"
#include <stb_image.h>
#include <stdexcept>
#include <iostream>

#include "vk_buffer.h"
#include "vk_command_buffer.h"
#include "vk_device.h"


void create_texture_image(Vulkan_Context& vulkan_context, Command_Buffer_Context& command_buffer_context, Texture& texture, const char* filepath)
{
     //load the texture
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(filepath, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    //The pixels are laid out row by row with 4 bytes per pixel in the case of STBI_rgb_alpha for a total of texWidth * texHeight * 4 values.
    VkDeviceSize imageSize = texWidth * texHeight * 4; // 4 stride rgba


    if (!pixels)
    {
        throw std::runtime_error("failed to load texture image!");
    }

    //create a staging buffer
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    buffer_create(vulkan_context, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer,
                  stagingBufferMemory);

    //allocate memory
    void* data;
    vkMapMemory(vulkan_context.logical_device, stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(vulkan_context.logical_device, stagingBufferMemory);
    //free texture
    stbi_image_free(pixels);

    //create texture image
    create_image(vulkan_context, texture, texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB,
                 VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    transition_image_layout(vulkan_context, command_buffer_context, texture.texture_image,
                            VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyBufferToImage(vulkan_context, command_buffer_context, stagingBuffer, texture.texture_image,
                      static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
    transition_image_layout(vulkan_context, command_buffer_context, texture.texture_image,
                            VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}




void create_image(Vulkan_Context& vulkan_context, Texture& texture, uint32_t width,
                  uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties)
{
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    imageInfo.flags = 0; // Optional


    if (vkCreateImage(vulkan_context.logical_device, &imageInfo, nullptr, &texture.texture_image) !=
        VK_SUCCESS)
    {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(vulkan_context.logical_device, texture.texture_image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(vulkan_context, memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(vulkan_context.logical_device, &allocInfo, nullptr,
                         &texture.texture_image_memory) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate image memory!");
    }

    vkBindImageMemory(vulkan_context.logical_device, texture.texture_image,
                      texture.texture_image_memory, 0);
}

void transition_image_layout(Vulkan_Context& vulkan_context, Command_Buffer_Context& command_buffer_context,
    VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{
    VkCommandBuffer commandBuffer = command_buffer_begin_single_use(vulkan_context, command_buffer_context.command_pool);

    //ensure the buffer is created before being written to
    //allows us to, if we want, transition image layouts, and transfer queue family ownership (if using VK_SHARING_MODE_EXCLUSIVE)
    VkImageMemoryBarrier image_memory_barrier{};
    image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    //The first two fields specify layout transition.
    //It is possible to use VK_IMAGE_LAYOUT_UNDEFINED as oldLayout if you don't care about the existing contents of the image.
    image_memory_barrier.oldLayout = oldLayout;
    image_memory_barrier.newLayout = newLayout;
    image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    image_memory_barrier.image = image;
    image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    image_memory_barrier.subresourceRange.baseMipLevel = 0;
    image_memory_barrier.subresourceRange.levelCount = 1;
    image_memory_barrier.subresourceRange.baseArrayLayer = 0;
    image_memory_barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        image_memory_barrier.srcAccessMask = 0;
        image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else
    {
        throw std::invalid_argument("unsupported layout transition!");
    }

    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &image_memory_barrier
    );

    command_buffer_end_single_use(vulkan_context, command_buffer_context.command_pool, commandBuffer);
}

void copyBufferToImage(Vulkan_Context& vulkan_context, Command_Buffer_Context& command_buffer_context, VkBuffer buffer,
    VkImage image, uint32_t width, uint32_t height)
{

    VkCommandBuffer commandBuffer = command_buffer_begin_single_use(vulkan_context, command_buffer_context.command_pool);
    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = {0, 0, 0};
    region.imageExtent = {
        width,
        height,
        1
    };

    vkCmdCopyBufferToImage(
        commandBuffer,
        buffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region
    );
    command_buffer_end_single_use(vulkan_context, command_buffer_context.command_pool, commandBuffer);
}



void create_texture_image_view(Vulkan_Context& vulkan_context, Texture& texture, VkFormat format)
{
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = texture.texture_image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(vulkan_context.logical_device, &viewInfo, nullptr, &texture.texture_image_view) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create image view!");
    }

    if (texture.texture_image_view == VK_NULL_HANDLE)
    {
        throw std::runtime_error("failed to create texture image view! NULL");
    }

}

void create_texture_sampler(Vulkan_Context& vulkan_context, Texture& texture)
{
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    /*
    * VK_SAMPLER_ADDRESS_MODE_REPEAT: Repeat the texture when going beyond the image dimensions.
    *VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT: Like repeat, but inverts the coordinates to mirror the image when going beyond the dimensions.
    *VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE: Take the color of the edge closest to the coordinate beyond the image dimensions.
    *VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE: Like clamp to edge, but instead uses the edge opposite to the closest edge.
    *VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER: Return a solid color when sampling beyond the dimensions of the image.
     */
    samplerInfo.anisotropyEnable = VK_TRUE;
    //get maxAnisotropy
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(vulkan_context.physical_device, &properties);
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    if (vkCreateSampler(vulkan_context.logical_device, &samplerInfo, nullptr, &texture.texture_sampler)
        != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create texture sampler!");
    }
}

void create_texture_glyph(Vulkan_Context& vulkan_context, Command_Buffer_Context& command_buffer_context,
                          Texture& texture, const unsigned char* pixel_data, uint32_t width, uint32_t height)
{

    //text_system.glyph_textures[text_system.glyphs[glyph]] = texture;
    std::cout << "IMAGE SIZE: " << width << " " << height << '\n';
    VkDeviceSize imageSize = width * height * 4; // 4 stride rgba


    if (!pixel_data)
    {
        throw std::runtime_error("NO BITMAPS AVAILABLE!");
    }


    //create a staging buffer
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    buffer_create(vulkan_context, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer,
                  stagingBufferMemory);

    //allocate memory
    void* data;
    vkMapMemory(vulkan_context.logical_device, stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixel_data, imageSize);
    vkUnmapMemory(vulkan_context.logical_device, stagingBufferMemory);

    //create texture image
    create_image(vulkan_context, texture, width, height, VK_FORMAT_R8G8B8A8_SRGB,
                 VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    transition_image_layout(vulkan_context, command_buffer_context, texture.texture_image,
                            VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyBufferToImage(vulkan_context, command_buffer_context, stagingBuffer, texture.texture_image,
                      width, height);
    transition_image_layout(vulkan_context, command_buffer_context, texture.texture_image,
                            VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    create_texture_image_view(vulkan_context, texture, VK_FORMAT_R8G8B8A8_SRGB);
    create_texture_sampler(vulkan_context, texture);


}
