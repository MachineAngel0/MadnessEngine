#include "vk_image.h"

#include "stb_image.h"
#include "vk_buffer.h"


void vulkan_image_create(vulkan_context* context, u32 width, u32 height, VkFormat format,
                         VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags memory_flags,
                         b32 create_view,
                         VkImageAspectFlags view_aspect_flags, vulkan_image* out_image)
{
    // Copy params
    out_image->width = width;
    out_image->height = height;


    // Creation info.
    VkImageCreateInfo image_create_info = {VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
    image_create_info.imageType = VK_IMAGE_TYPE_2D; // might need to be an image in the future
    image_create_info.extent.width = width;
    image_create_info.extent.height = height;
    image_create_info.extent.depth = 1; // TODO: Support configurable depth.
    image_create_info.mipLevels = 4; // TODO: Support mip mapping
    image_create_info.arrayLayers = 1; // TODO: Support number of layers in the image.
    image_create_info.format = format;
    image_create_info.tiling = tiling;
    image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_create_info.usage = usage;
    image_create_info.samples = VK_SAMPLE_COUNT_1_BIT; // TODO: Configurable sample count.
    image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // TODO: Configurable sharing mode.

    VK_CHECK(vkCreateImage(context->device.logical_device, &image_create_info, context->allocator, &out_image->handle));


    // Query memory requirements.
    VkMemoryRequirements memory_requirements;
    vkGetImageMemoryRequirements(context->device.logical_device, out_image->handle, &memory_requirements);

    // i32 memory_type = context->find_memory_index(memory_requirements.memoryTypeBits, memory_flags);
    i32 memory_type = find_memory_type(context, memory_requirements.memoryTypeBits, memory_flags);
    if (memory_type == -1)
    {
        M_ERROR("Required memory type not found. Image not valid.");
    }


    // Allocate memory
    VkMemoryAllocateInfo memory_allocate_info = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    memory_allocate_info.allocationSize = memory_requirements.size;
    memory_allocate_info.memoryTypeIndex = memory_type;
    VK_CHECK(
        vkAllocateMemory(context->device.logical_device, &memory_allocate_info, context->allocator, &out_image->memory
        ));


    // Bind the memory
    VK_CHECK(vkBindImageMemory(context->device.logical_device, out_image->handle, out_image->memory, 0));
    // TODO: configurable memory offset.

    // Create view
    if (create_view)
    {
        out_image->view = 0;
        vulkan_image_view_create(context, format, out_image, view_aspect_flags);
    }
}


void vulkan_image_view_create(vulkan_context* context, VkFormat format, vulkan_image* image,
                              VkImageAspectFlags aspect_flags)
{
    VkImageViewCreateInfo view_create_info = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    view_create_info.image = image->handle;
    view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D; // TODO: Make configurable.
    view_create_info.format = format;
    view_create_info.subresourceRange.aspectMask = aspect_flags;
    // TODO: Make configurable
    view_create_info.subresourceRange.baseMipLevel = 0;
    view_create_info.subresourceRange.levelCount = 1;
    view_create_info.subresourceRange.baseArrayLayer = 0;
    view_create_info.subresourceRange.layerCount = 1;

    VK_CHECK(vkCreateImageView(context->device.logical_device, &view_create_info, context->allocator, &image->view));
}


void vulkan_image_destroy(vulkan_context* context, vulkan_image* image)
{
    if (image->view)
    {
        vkDestroyImageView(context->device.logical_device, image->view, context->allocator);
        image->view = 0;
    }

    if (image->memory)
    {
        vkFreeMemory(context->device.logical_device, image->memory, context->allocator);
        image->memory = 0;
    }

    if (image->handle)
    {
        vkDestroyImage(context->device.logical_device, image->handle, context->allocator);
        image->handle = 0;
    }
}

void create_texture_image(vulkan_context* context, vulkan_command_buffer* command_buffer,
                          const char* filepath, Texture* out_texture)
{
    //load the texture
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(filepath, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    //The pixels are laid out row by row with 4 bytes per pixel in the case of STBI_rgb_alpha for a total of texWidth * texHeight * 4 values.
    VkDeviceSize imageSize = texWidth * texHeight * 4; // 4 stride rgba

    out_texture->width = texWidth;
    out_texture->height = texHeight;
    if (!pixels)
    {
        WARN("CREATE TEXTURE IMAGE: failed to load texture image!");
        return;
    }

    //create a staging buffer
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    buffer_create(context, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer,
                  &stagingBufferMemory);

    //allocate memory
    void* data;
    vkMapMemory(context->device.logical_device, stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixels, (size_t) (imageSize));
    vkUnmapMemory(context->device.logical_device, stagingBufferMemory);
    //free texture
    stbi_image_free(pixels);

    //create texture image


    // Creation info.
    VkImageCreateInfo image_create_info = {VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
    image_create_info.imageType = VK_IMAGE_TYPE_2D; // might need to be an image in the future
    image_create_info.extent.width = texWidth;
    image_create_info.extent.height = texHeight;
    image_create_info.extent.depth = 1; // TODO: Support configurable depth.
    image_create_info.mipLevels = 4; // TODO: Support mip mapping
    image_create_info.arrayLayers = 1; // TODO: Support number of layers in the image.
    image_create_info.format = VK_FORMAT_R8G8B8A8_SRGB;
    image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_create_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    image_create_info.samples = VK_SAMPLE_COUNT_1_BIT; // TODO: Configurable sample count.
    image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // TODO: Configurable sharing mode.

    VK_CHECK(
        vkCreateImage(context->device.logical_device, &image_create_info, context->allocator, &out_texture->
            texture_image));


    // Query memory requirements.
    VkMemoryRequirements memory_requirements;
    vkGetImageMemoryRequirements(context->device.logical_device, out_texture->texture_image, &memory_requirements);

    // i32 memory_type = context->find_memory_index(memory_requirements.memoryTypeBits, memory_flags);
    i32 memory_type = find_memory_type(context, memory_requirements.memoryTypeBits,
                                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    if (memory_type == -1)
    {
        M_ERROR("Required memory type not found. Image not valid.");
    }


    // Allocate memory
    VkMemoryAllocateInfo memory_allocate_info = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    memory_allocate_info.allocationSize = memory_requirements.size;
    memory_allocate_info.memoryTypeIndex = memory_type;
    VkResult result1 = vkAllocateMemory(context->device.logical_device, &memory_allocate_info, context->allocator,
                                        &out_texture->texture_image_memory);
    VK_CHECK(result1);
    // Bind the memory
    VkResult result2 = vkBindImageMemory(context->device.logical_device, out_texture->texture_image,
                                         out_texture->texture_image_memory, 0);
    VK_CHECK(result2)

    // TODO: configurable memory offset.

    // Create view
    out_texture->texture_image_view = 0;
    vulkan_image_view_create(context, VK_FORMAT_R8G8B8A8_SRGB, &out_texture->texture_image, VK_IMAGE_ASPECT_COLOR_BIT);

    transition_image_layout(context, command_buffer, out_texture->texture_image,
                            VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyBufferToImage(context, command_buffer, stagingBuffer, out_texture->texture_image,
                      (uint32_t)(texWidth), (uint32_t)(texHeight));
    transition_image_layout(context, command_buffer, out_texture->texture_image,
                            VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}


void transition_image_layout(vulkan_context* vulkan_context, vulkan_command_buffer* command_buffer_context,
                             VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{
    vulkan_command_buffer commandBuffer;
    vulkan_command_buffer_allocate_and_begin_single_use(vulkan_context, vulkan_context->graphics_command_pool,
                                                        &commandBuffer);

    //ensure the buffer is created before being written to
    //allows us to, if we want, transition image layouts, and transfer queue family ownership (if using VK_SHARING_MODE_EXCLUSIVE)
    VkImageMemoryBarrier image_memory_barrier = {0};
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

    VkPipelineStageFlags sourceStage = 0;
    VkPipelineStageFlags destinationStage = 0;

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
        FATAL("unsupported layout transition!");
    }

    vkCmdPipelineBarrier(
        commandBuffer.handle,
        sourceStage, destinationStage,
        0,
        0, NULL,
        0, NULL,
        1, &image_memory_barrier
    );

    vulkan_command_buffer_end_single_use(vulkan_context, vulkan_context->graphics_command_pool, command_buffer_context,
                                         vulkan_context->device.graphics_queue);
}


void copyBufferToImage(vulkan_context* vulkan_context, vulkan_command_buffer* command_buffer_context, VkBuffer buffer,
                       VkImage image, uint32_t width, uint32_t height)
{
    vulkan_command_buffer commandBuffer = {0};
    vulkan_command_buffer_allocate_and_begin_single_use(vulkan_context, vulkan_context->graphics_command_pool,
                                                        &commandBuffer);
    VkBufferImageCopy region = {0};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = (VkOffset3D){0, 0, 0};
    region.imageExtent = (VkExtent3D){
        width,
        height,
        1
    };

    vkCmdCopyBufferToImage(
        &commandBuffer,
        buffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region
    );
    vulkan_command_buffer_end_single_use(vulkan_context, vulkan_context->graphics_command_pool, &commandBuffer,
                                         vulkan_context->device.graphics_queue);
}

void insertImageMemoryBarrier(VkCommandBuffer cmdbuffer, VkImage image, VkAccessFlags srcAccessMask,
    VkAccessFlags dstAccessMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout,
    VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkImageSubresourceRange subresourceRange)
{
    VkImageMemoryBarrier imageMemoryBarrier = {0};
    imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageMemoryBarrier.srcAccessMask = srcAccessMask;
    imageMemoryBarrier.dstAccessMask = dstAccessMask;
    imageMemoryBarrier.oldLayout = oldImageLayout;
    imageMemoryBarrier.newLayout = newImageLayout;
    imageMemoryBarrier.image = image;
    imageMemoryBarrier.subresourceRange = subresourceRange;

    vkCmdPipelineBarrier(
        cmdbuffer,
        srcStageMask,
        dstStageMask,
        0,
        0, NULL,
        0, NULL,
        1, &imageMemoryBarrier);
}
