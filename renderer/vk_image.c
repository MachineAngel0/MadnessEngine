#include "vk_image.h"

#include "stb_image.h"


void vulkan_image_create(vulkan_context* context, u32 width, u32 height, VkFormat format,
                         VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags memory_flags,
                         b32 create_view,
                         VkImageAspectFlags view_aspect_flags, Texture* out_texture)
{
    // Copy params
    // out_image->width = width;
    // out_image->height = height;


    // Creation info.
    VkImageCreateInfo image_create_info = {0};
    image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
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

    VK_CHECK(vkCreateImage(context->device.logical_device, &image_create_info, context->allocator, &out_texture->texture_image));


    // Query memory requirements.
    VkMemoryRequirements memory_requirements;
    vkGetImageMemoryRequirements(context->device.logical_device, out_texture->texture_image, &memory_requirements);

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
        vkAllocateMemory(context->device.logical_device, &memory_allocate_info, context->allocator, &out_texture->texture_image_memory
        ));


    // Bind the memory
    VK_CHECK(vkBindImageMemory(context->device.logical_device, out_texture->texture_image, out_texture->texture_image_memory, 0));
    // TODO: configurable memory offset.

    // Create view
    if (create_view)
    {
        out_texture->texture_image_view = 0;
        vulkan_image_view_create(context, format, view_aspect_flags, out_texture);
    }
}


void vulkan_image_view_create(vulkan_context* context, VkFormat format,
                              VkImageAspectFlags aspect_flags, Texture* texture)
{
    VkImageViewCreateInfo view_create_info = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    view_create_info.image = texture->texture_image;
    view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D; // TODO: Make configurable.
    view_create_info.format = format;
    view_create_info.subresourceRange.aspectMask = aspect_flags;
    // TODO: Make configurable
    view_create_info.subresourceRange.baseMipLevel = 0;
    view_create_info.subresourceRange.levelCount = 1;
    view_create_info.subresourceRange.baseArrayLayer = 0;
    view_create_info.subresourceRange.layerCount = 1;

    VK_CHECK(vkCreateImageView(context->device.logical_device, &view_create_info, context->allocator, &texture->texture_image_view));
}


void vulkan_image_destroy(vulkan_context* context, Texture* image)
{
    if (image->texture_image_view)
    {
        vkDestroyImageView(context->device.logical_device, image->texture_image_view, context->allocator);
        image->texture_image_view = 0;
    }

    if (image->texture_image_memory)
    {
        vkFreeMemory(context->device.logical_device, image->texture_image_memory, context->allocator);
        image->texture_image_memory = 0;
    }

    if (image->texture_image)
    {
        vkDestroyImage(context->device.logical_device, image->texture_image, context->allocator);
        image->texture_image = 0;
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

    MASSERT_MSG(pixels, "FAILED TO LOAD TEXTURE");


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
    memcpy(data, pixels, imageSize);
    vkUnmapMemory(context->device.logical_device, stagingBufferMemory);
    //free texture
    stbi_image_free(pixels);

    //create texture image

    VkImageCreateInfo image_create_info = {0};
    image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO; // might need to be an image in the future
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


    transition_image_layout(context, command_buffer, out_texture->texture_image,
                            VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyBufferToImage(context, command_buffer, stagingBuffer, out_texture->texture_image,
                      (uint32_t) (texWidth), (uint32_t) (texHeight));
    transition_image_layout(context, command_buffer, out_texture->texture_image,
                            VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    // TODO: configurable memory offset.
    // Create view
    out_texture->texture_image_view = 0;
    vulkan_image_view_create(context, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, out_texture);


    VkSamplerCreateInfo sampler_info = {0};
    sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_info.magFilter = VK_FILTER_LINEAR;
    sampler_info.minFilter = VK_FILTER_LINEAR;
    sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.anisotropyEnable = VK_TRUE;
    sampler_info.maxAnisotropy = context->device.properties.limits.maxSamplerAnisotropy;
    sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    sampler_info.unnormalizedCoordinates = VK_FALSE;
    sampler_info.compareEnable = VK_FALSE;
    sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
    sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_info.mipLodBias = 0.0f;
    sampler_info.minLod = 0.0f;
    sampler_info.maxLod = 0.0f;

    VK_CHECK(vkCreateSampler(context->device.logical_device, &sampler_info, 0, &out_texture->texture_sampler));
}


void transition_image_layout(vulkan_context* vulkan_context, vulkan_command_buffer* command_buffer_context,
                             VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{
    //TODO: lots of this should probably be extracted out with the functions in command_buffer.h/c
    //

    vulkan_command_buffer commandBuffer;
    VkCommandPool pool = vulkan_context->graphics_command_pool;
    vulkan_command_buffer* out_command_buffer = &commandBuffer;
    memset(out_command_buffer, 0, sizeof(out_command_buffer));

    VkCommandBufferAllocateInfo allocate_info = {0};
    allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocate_info.commandPool = pool;
    allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocate_info.commandBufferCount = 1;
    allocate_info.pNext = 0;

    VK_CHECK(
        vkAllocateCommandBuffers(vulkan_context->device.logical_device, &allocate_info, &out_command_buffer->
            handle));

    vulkan_command_buffer* command_buffer = &commandBuffer;
    VkCommandBufferBeginInfo begin_info = {0};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    // begin_info.pInheritanceInfo = NULL; //used if its a secondary command buffer
    begin_info.flags = 0;
    begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    VK_CHECK(vkBeginCommandBuffer(command_buffer->handle, &begin_info));


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

    VkQueue queue = vulkan_context->device.graphics_queue;
    VK_CHECK(vkEndCommandBuffer(command_buffer->handle));

    VkSubmitInfo submit_info = {0};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer->handle;

    VK_CHECK(vkQueueSubmit(queue, 1, &submit_info, 0));

    //wait for command buffer to finish then free
    VK_CHECK(vkQueueWaitIdle(queue));
    vulkan_command_buffer_free(vulkan_context, pool, command_buffer);
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
        commandBuffer.handle,
        buffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region
    );
    vulkan_command_buffer_end_single_use(vulkan_context, vulkan_context->graphics_command_pool, &commandBuffer,
                                         vulkan_context->device.graphics_queue);
}

void create_texture_sampler(renderer* renderer, Texture* texture)
{
    VkSamplerCreateInfo samplerInfo = {0};
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
    VkPhysicalDeviceProperties properties = {0};
    vkGetPhysicalDeviceProperties(renderer->context.device.physical_device, &properties);
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    VK_CHECK(vkCreateSampler(renderer->context.device.logical_device, &samplerInfo, NULL, &texture->texture_sampler));
}

void image_insert_memory_barrier(VkCommandBuffer cmdbuffer, VkImage image, VkAccessFlags srcAccessMask,
                              VkAccessFlags dstAccessMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout,
                              VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask,
                              VkImageSubresourceRange subresourceRange)
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
