#include "vk_image.h"
#include "stb_image.h"

#include "vk_command_buffer.h"


void vulkan_image_create(Renderer* renderer, u32 width, u32 height,
                         VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
                         VkMemoryPropertyFlags memory_flags,
                         b32 create_view, VkImageAspectFlags view_aspect_flags, Vulkan_Texture* out_texture)
{
    //copy params
    out_texture->width = width;
    out_texture->height = height;


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

    VK_CHECK(
        vkCreateImage(renderer->logical_device, &image_create_info, renderer->vulkan_allocator, &out_texture->
            texture_image));


    // Query memory requirements.
    VkMemoryRequirements memory_requirements;
    vkGetImageMemoryRequirements(renderer->logical_device, out_texture->texture_image, &memory_requirements);

    // s32 memory_type = context->find_memory_index(memory_requirements.memoryTypeBits, memory_flags);
    s32 memory_type = find_memory_type(renderer, memory_requirements.memoryTypeBits, memory_flags);
    if (memory_type == -1)
    {
        M_ERROR("Required memory type not found. Image not valid.");
    }


    // Allocate memory
    VkMemoryAllocateInfo memory_allocate_info = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    memory_allocate_info.allocationSize = memory_requirements.size;
    memory_allocate_info.memoryTypeIndex = memory_type;
    VK_CHECK(
        vkAllocateMemory(renderer->logical_device, &memory_allocate_info, renderer->vulkan_allocator, &out_texture->
            texture_image_memory
        ));


    // Bind the memory
    VK_CHECK(
        vkBindImageMemory(renderer->logical_device, out_texture->texture_image, out_texture->texture_image_memory,
            0));
    // TODO: configurable memory offset.

    // Create view
    if (create_view)
    {
        out_texture->texture_image_view = 0;
        vulkan_image_view_create(renderer, format, view_aspect_flags, out_texture);
    }
}


void vulkan_image_view_create(Renderer* renderer,
                              VkFormat format, VkImageAspectFlags aspect_flags, Vulkan_Texture* texture)
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

    VK_CHECK(
        vkCreateImageView(renderer->logical_device, &view_create_info, renderer->vulkan_allocator, &texture->
            texture_image_view));
}


void vulkan_texture_free(Renderer* renderer, Vulkan_Texture* image)
{
    if (image->texture_image_view)
    {
        vkDestroyImageView(renderer->logical_device, image->texture_image_view, renderer->vulkan_allocator);
        image->texture_image_view = 0;
    }

    if (image->texture_image_memory)
    {
        vkFreeMemory(renderer->logical_device, image->texture_image_memory, renderer->vulkan_allocator);
        image->texture_image_memory = 0;
    }

    if (image->texture_image)
    {
        vkDestroyImage(renderer->logical_device, image->texture_image, renderer->vulkan_allocator);
        image->texture_image = 0;
    }
    if (image->texture_sampler)
    {
        vkDestroySampler(renderer->logical_device, image->texture_sampler, renderer->vulkan_allocator);
    }
}


void vulkan_texture_create_shadowmap(Renderer* renderer, u32 width, u32 height,
                                     VkFormat format, Vulkan_Texture* out_texture)
{
    out_texture->width = width;
    out_texture->height = width;

    // We will sample directly from the depth attachment for the shadow mapping
    VkImageUsageFlags usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    // VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT // might need
    VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
    /*
            VK_IMAGE_TILING_LINEAR: Texels are laid out in row-major order like our pixels array
            VK_IMAGE_TILING_OPTIMAL: Texels are laid out in an implementation defined order for optimal access
        Unlike the layout of an image, the tiling mode cannot be changed at a later time.
        If you want to be able to directly access texels in the memory of the image,
        then you must use VK_IMAGE_TILING_LINEAR. We will be using a staging buffer instead of a staging image,
        so this won't be necessary. We will be using VK_IMAGE_TILING_OPTIMAL for efficient access from the shader.
     */

    //shadows use a depth bit
    VkImageAspectFlags view_aspect_flags = VK_IMAGE_ASPECT_DEPTH_BIT;
    /*
    *  VK_IMAGE_ASPECT_COLOR_BIT = 0x00000001,
    VK_IMAGE_ASPECT_DEPTH_BIT = 0x00000002,
    VK_IMAGE_ASPECT_STENCIL_BIT = 0x00000004,
     */

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

    VkResult image_create_result = vkCreateImage(renderer->logical_device, &image_create_info, renderer->vulkan_allocator,
                                                 &out_texture->texture_image);
    VK_CHECK(image_create_result);


    // Query memory requirements.
    VkMemoryRequirements memory_requirements;
    vkGetImageMemoryRequirements(renderer->logical_device, out_texture->texture_image, &memory_requirements);

    // s32 memory_type = context->find_memory_index(memory_requirements.memoryTypeBits, memory_flags);
    s32 memory_type = find_memory_type(renderer, memory_requirements.memoryTypeBits,
                                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    if (memory_type == -1)
    {
        M_ERROR("Required memory type not found. Image not valid.");
    }


    // Allocate memory
    VkMemoryAllocateInfo memory_allocate_info = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    memory_allocate_info.allocationSize = memory_requirements.size;
    memory_allocate_info.memoryTypeIndex = memory_type;
    VK_CHECK(
        vkAllocateMemory(renderer->logical_device, &memory_allocate_info, renderer->vulkan_allocator, &out_texture->
            texture_image_memory
        ));

    // Bind the memory
    VK_CHECK(
        vkBindImageMemory(renderer->logical_device, out_texture->texture_image, out_texture->texture_image_memory,
            0));
    // TODO: configurable memory offset.

    // Create view
    out_texture->texture_image_view = 0;
    vulkan_image_view_create(renderer, format, view_aspect_flags, out_texture);

    VkFilter shadowmap_filter = formatIsFilterable(renderer->physical_device, format, VK_IMAGE_TILING_OPTIMAL)
                                    ? VK_FILTER_LINEAR
                                    : VK_FILTER_NEAREST;

    VkSamplerCreateInfo sampler_info = {0};
    sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_info.magFilter = shadowmap_filter;
    sampler_info.minFilter = shadowmap_filter;
    sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    sampler_info.anisotropyEnable = VK_TRUE;
    sampler_info.maxAnisotropy = renderer->properties2.properties.limits.maxSamplerAnisotropy;
    sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    sampler_info.unnormalizedCoordinates = VK_FALSE;
    sampler_info.compareEnable = VK_FALSE; // not in use
    sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
    sampler_info.mipLodBias = 0.0f;
    sampler_info.minLod = 0.0f;
    sampler_info.maxLod = 1.0f; // has to be set

    VK_CHECK(vkCreateSampler(renderer->logical_device, &sampler_info, 0, &out_texture->texture_sampler));
}




void transition_image_layout(Vulkan_Command_Buffer* command_buffer_context,
                             VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout,
                             Renderer* renderer)
{
    //TODO: lots of this should probably be extracted out with the functions in command_buffer.h/c
    //

    Vulkan_Command_Buffer commandBuffer;
    VkCommandPool pool = renderer->graphics_command_pool;
    Vulkan_Command_Buffer* out_command_buffer = &commandBuffer;
    memset(out_command_buffer, 0, sizeof(out_command_buffer));

    VkCommandBufferAllocateInfo allocate_info = {0};
    allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocate_info.commandPool = pool;
    allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocate_info.commandBufferCount = 1;
    allocate_info.pNext = 0;

    VK_CHECK(
        vkAllocateCommandBuffers(renderer->logical_device, &allocate_info, &out_command_buffer->
            handle));

    Vulkan_Command_Buffer* command_buffer = &commandBuffer;
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

    VkQueue queue = renderer->graphics_queue;
    VK_CHECK(vkEndCommandBuffer(command_buffer->handle));

    VkSubmitInfo submit_info = {0};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer->handle;

    VK_CHECK(vkQueueSubmit(queue, 1, &submit_info, 0));

    //wait for command buffer to finish then free
    VK_CHECK(vkQueueWaitIdle(queue));
    vulkan_command_buffer_free(renderer, command_buffer, pool);
}


void copyBufferToImage(Renderer* renderer,
                       VkBuffer buffer, VkImage image, u32 width, u32 height)
{
    Vulkan_Command_Buffer commandBuffer = {0};
    vulkan_command_buffer_allocate_and_begin_single_use(renderer,
                                                        renderer->graphics_command_pool,
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
    vulkan_command_buffer_end_and_submit_and_free_single_use(renderer,
                                                             renderer->graphics_command_pool,
                                                             &commandBuffer,
                                                             renderer->graphics_queue);
}

void create_texture_sampler(Renderer* renderer, Vulkan_Texture* texture)
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
    vkGetPhysicalDeviceProperties(renderer->physical_device, &properties);
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    VK_CHECK(vkCreateSampler(renderer->logical_device, &samplerInfo, NULL, &texture->texture_sampler));
}

void image_insert_memory_barrier(VkCommandBuffer cmdbuffer, VkImage image, VkAccessFlags srcAccessMask,
                                 VkAccessFlags dstAccessMask, VkImageLayout oldImageLayout,
                                 VkImageLayout newImageLayout,
                                 VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask,
                                 VkImageSubresourceRange subresourceRange)
{
    VkImageMemoryBarrier2 image_memory_barrier = {0};
    image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    image_memory_barrier.srcAccessMask = srcAccessMask;
    image_memory_barrier.dstAccessMask = dstAccessMask;
    image_memory_barrier.oldLayout = oldImageLayout;
    image_memory_barrier.newLayout = newImageLayout;
    image_memory_barrier.image = image;
    image_memory_barrier.subresourceRange = subresourceRange;
    image_memory_barrier.srcStageMask = srcStageMask;
    image_memory_barrier.dstStageMask = dstStageMask;

    VkDependencyInfo pDependencyInfo = {0};
    pDependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    pDependencyInfo.imageMemoryBarrierCount = 1;
    pDependencyInfo.pImageMemoryBarriers = &image_memory_barrier;


    vkCmdPipelineBarrier2(cmdbuffer, &pDependencyInfo);

    /*
    *    VkImageMemoryBarrier image_memory_barrier = {0};
    image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    image_memory_barrier.srcAccessMask = srcAccessMask;
    image_memory_barrier.dstAccessMask = dstAccessMask;
    image_memory_barrier.oldLayout = oldImageLayout;
    image_memory_barrier.newLayout = newImageLayout;
    image_memory_barrier.image = image;
    image_memory_barrier.subresourceRange = subresourceRange;
    vkCmdPipelineBarrier(
        cmdbuffer,
        srcStageMask,
        dstStageMask,
        0,
        0, NULL,
        0, NULL,
        1, &image_memory_barrier);*/
}


// Returns if a given format support LINEAR filtering
VkBool32 formatIsFilterable(VkPhysicalDevice physicalDevice, VkFormat format, VkImageTiling tiling)
{
    VkFormatProperties formatProps;
    vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProps);

    if (tiling == VK_IMAGE_TILING_OPTIMAL)
        {return formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;}

    if (tiling == VK_IMAGE_TILING_LINEAR)
        {return formatProps.linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;}

    return false;
}


void initial_image_layout_transition(Vulkan_Command_Buffer* command_buffer,
                                     VkImage image)
{
    VkImageMemoryBarrier2 image_memory_barrier = {0};
    image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    image_memory_barrier.pNext = 0;
    //The first two fields specify layout transition.
    //It is possible to use VK_IMAGE_LAYOUT_UNDEFINED as oldLayout if you don't care about the existing contents of the image.
    image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    image_memory_barrier.image = image;
    image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    image_memory_barrier.subresourceRange.baseMipLevel = 0;
    image_memory_barrier.subresourceRange.levelCount = 1;
    image_memory_barrier.subresourceRange.baseArrayLayer = 0;
    image_memory_barrier.subresourceRange.layerCount = 1;
    image_memory_barrier.subresourceRange.layerCount = 1;

    image_memory_barrier.srcAccessMask = 0;
    image_memory_barrier.dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;

    image_memory_barrier.srcStageMask = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
    image_memory_barrier.dstStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;


    //fill out  pipeline info
    VkDependencyInfo dependency_info = {0};
    dependency_info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    dependency_info.pNext = NULL;
    dependency_info.dependencyFlags = 0;
    dependency_info.memoryBarrierCount = 0;
    dependency_info.pMemoryBarriers = NULL;
    dependency_info.bufferMemoryBarrierCount = 0;
    dependency_info.pBufferMemoryBarriers = 0;
    dependency_info.imageMemoryBarrierCount = 1;
    dependency_info.pImageMemoryBarriers = &image_memory_barrier;

    vkCmdPipelineBarrier2(command_buffer->handle, &dependency_info);
}

void second_image_layout_transition(Renderer* renderer,
                                    Vulkan_Command_Buffer* command_buffer,
                                    VkImage image,
                                    Vulkan_Queue_Type source_queue, Vulkan_Queue_Type destination_queue)
{
    //this much match if we need a release and acquire
    VkImageLayout oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    VkImageLayout newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    if (vulkan_is_same_queue_family(renderer, source_queue, destination_queue))
    {
        VkImageMemoryBarrier2 image_memory_barrier = {0};
        image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
        image_memory_barrier.pNext = 0;

        image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

        image_memory_barrier.oldLayout = oldLayout;
        image_memory_barrier.newLayout = newLayout;
        image_memory_barrier.image = image;

        image_memory_barrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
        image_memory_barrier.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;

        image_memory_barrier.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
        image_memory_barrier.dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;

        image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        image_memory_barrier.subresourceRange.baseMipLevel = 0;
        image_memory_barrier.subresourceRange.levelCount = 1;
        image_memory_barrier.subresourceRange.baseArrayLayer = 0;
        image_memory_barrier.subresourceRange.layerCount = 1;
        image_memory_barrier.subresourceRange.layerCount = 1;

        //fill out  pipeline info
        VkDependencyInfo dependency_info = {0};
        dependency_info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
        dependency_info.pNext = NULL;
        dependency_info.dependencyFlags = 0;
        dependency_info.memoryBarrierCount = 0;
        dependency_info.pMemoryBarriers = NULL;
        dependency_info.bufferMemoryBarrierCount = 0;
        dependency_info.pBufferMemoryBarriers = 0;
        dependency_info.imageMemoryBarrierCount = 1;
        dependency_info.pImageMemoryBarriers = &image_memory_barrier;

        vkCmdPipelineBarrier2(command_buffer->handle, &dependency_info);
        return;
    }

    //different queues
    //releasing from the transfer queue
    VkImageMemoryBarrier2 image_release_barrier = {0};
    image_release_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    image_release_barrier.pNext = 0;
    image_release_barrier.srcQueueFamilyIndex = vulkan_get_queue_family_index(renderer, source_queue);
    image_release_barrier.dstQueueFamilyIndex = vulkan_get_queue_family_index(renderer, destination_queue);

    image_release_barrier.image = image;
    image_release_barrier.oldLayout = oldLayout;
    image_release_barrier.newLayout = newLayout;

    image_release_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    image_release_barrier.dstAccessMask = VK_ACCESS_2_NONE;

    image_release_barrier.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
    image_release_barrier.dstStageMask = VK_PIPELINE_STAGE_2_NONE;

    image_release_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    image_release_barrier.subresourceRange.baseMipLevel = 0;
    image_release_barrier.subresourceRange.levelCount = 1;
    image_release_barrier.subresourceRange.baseArrayLayer = 0;
    image_release_barrier.subresourceRange.layerCount = 1;
    image_release_barrier.subresourceRange.layerCount = 1;


    VkDependencyInfo release_dependency_info = {0};
    release_dependency_info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    release_dependency_info.pNext = NULL;
    release_dependency_info.dependencyFlags = 0;
    release_dependency_info.memoryBarrierCount = 0;
    release_dependency_info.pMemoryBarriers = NULL;
    release_dependency_info.bufferMemoryBarrierCount = 0;
    release_dependency_info.pBufferMemoryBarriers = 0;
    release_dependency_info.imageMemoryBarrierCount = 1;
    release_dependency_info.pImageMemoryBarriers = &image_release_barrier;

    vkCmdPipelineBarrier2(command_buffer->handle, &release_dependency_info);


    //acquiring from the transfer queue and then transitioning the image
    VkImageMemoryBarrier2 image_acquire_barrier = {0};
    image_release_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;

    image_acquire_barrier.srcQueueFamilyIndex = vulkan_get_queue_family_index(renderer, source_queue);
    image_acquire_barrier.dstQueueFamilyIndex = vulkan_get_queue_family_index(renderer, destination_queue);

    image_release_barrier.image = image;
    image_release_barrier.oldLayout = oldLayout;
    image_release_barrier.newLayout = newLayout;

    image_release_barrier.srcAccessMask = VK_ACCESS_2_NONE;
    image_release_barrier.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;

    image_release_barrier.srcStageMask = VK_PIPELINE_STAGE_2_NONE;
    image_release_barrier.dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;

    image_release_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    image_release_barrier.subresourceRange.baseMipLevel = 0;
    image_release_barrier.subresourceRange.levelCount = 1;
    image_release_barrier.subresourceRange.baseArrayLayer = 0;
    image_release_barrier.subresourceRange.layerCount = 1;
    image_release_barrier.subresourceRange.layerCount = 1;


    VkDependencyInfo acquire_dependency_info = {0};
    acquire_dependency_info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    acquire_dependency_info.pNext = NULL;
    acquire_dependency_info.dependencyFlags = 0;
    acquire_dependency_info.memoryBarrierCount = 0;
    acquire_dependency_info.pMemoryBarriers = NULL;
    acquire_dependency_info.bufferMemoryBarrierCount = 0;
    acquire_dependency_info.pBufferMemoryBarriers = 0;
    acquire_dependency_info.imageMemoryBarrierCount = 1;
    acquire_dependency_info.pImageMemoryBarriers = &image_acquire_barrier;

    vkCmdPipelineBarrier2(command_buffer->handle, &release_dependency_info);
}


void buffer_to_image_copy_new(Vulkan_Command_Buffer* command_buffer, VkBuffer buffer,
                              VkImage image, u32 width, u32 height)
{
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
        command_buffer->handle,
        buffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region
    );
}


void vulkan_texture_create_image_new(Renderer* renderer,
                                     Texture_GPU_Upload* texture_data,
                                     Vulkan_Texture* out_texture)
{
    out_texture->width = texture_data->madness_texture->width;
    out_texture->height = texture_data->madness_texture->height;
    out_texture->image_size = texture_data->madness_texture->pixels_size;


    //create texture image
    VkImageCreateInfo image_create_info = {0};
    image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO; // might need to be an image in the future
    image_create_info.imageType = VK_IMAGE_TYPE_2D; // might need to be an image in the future
    image_create_info.extent.width = out_texture->width;
    image_create_info.extent.height = out_texture->height;
    image_create_info.extent.depth = 1; // TODO: Support configurable depth.
    image_create_info.mipLevels = 4; // TODO: Support mip mapping
    image_create_info.arrayLayers = 1; // TODO: Support number of layers in the image.
    switch (texture_data->madness_texture->type)
    {
    case ASSET_TEXTURE:
        image_create_info.format = VK_FORMAT_R8G8B8A8_SRGB;
        break;
    case ASSET_FONT:
        //msdfs are not color data
        image_create_info.format = VK_FORMAT_R8G8B8A8_UNORM;
        break;
    default:
        MASSERT_FALSE();
        break;
    }
    image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;

    image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_create_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    image_create_info.samples = VK_SAMPLE_COUNT_1_BIT; // TODO: Configurable sample count.
    image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // TODO: Configurable sharing mode.

    VK_CHECK(
        vkCreateImage(renderer->logical_device, &image_create_info, renderer->vulkan_allocator, &out_texture->
            texture_image));


    // Query memory requirements.
    VkMemoryRequirements memory_requirements;
    vkGetImageMemoryRequirements(renderer->logical_device, out_texture->texture_image, &memory_requirements);

    // s32 memory_type = context->find_memory_index(memory_requirements.memoryTypeBits, memory_flags);
    s32 memory_type = find_memory_type(renderer, memory_requirements.memoryTypeBits,
                                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    if (memory_type == -1)
    {
        M_ERROR("Required memory type not found. Image not valid.");
    }


    // Allocate memory
    VkMemoryAllocateInfo memory_allocate_info = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    memory_allocate_info.allocationSize = memory_requirements.size;
    memory_allocate_info.memoryTypeIndex = memory_type;
    VkResult result1 = vkAllocateMemory(renderer->logical_device, &memory_allocate_info, renderer->vulkan_allocator,
                                        &out_texture->texture_image_memory);
    VK_CHECK(result1);
    // Bind the memory
    VkResult result2 = vkBindImageMemory(renderer->logical_device, out_texture->texture_image,
                                         out_texture->texture_image_memory, 0);
    VK_CHECK(result2)


    // TODO: configurable memory offset.
    // Create view
    out_texture->texture_image_view = 0;
    vulkan_image_view_create(renderer, image_create_info.format, VK_IMAGE_ASPECT_COLOR_BIT, out_texture);

    // Create Sampler
    VkSamplerCreateInfo sampler_info = {0};
    switch (texture_data->madness_texture->type)
    {
    case ASSET_TEXTURE:
        sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        sampler_info.magFilter = VK_FILTER_LINEAR;
        sampler_info.minFilter = VK_FILTER_LINEAR;
        sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_info.anisotropyEnable = VK_TRUE;
        sampler_info.maxAnisotropy = renderer->properties2.properties.limits.maxSamplerAnisotropy;
        sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        sampler_info.unnormalizedCoordinates = VK_FALSE;
        sampler_info.compareEnable = VK_FALSE;
        sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
        sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        sampler_info.mipLodBias = 0.0f;
        sampler_info.minLod = 0.0f;
        sampler_info.maxLod = 0.0f;
        break;
    case ASSET_FONT:
        sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        sampler_info.magFilter = VK_FILTER_LINEAR;
        sampler_info.minFilter = VK_FILTER_LINEAR;
        sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        sampler_info.anisotropyEnable = VK_FALSE;
        sampler_info.maxAnisotropy = renderer->properties2.properties.limits.maxSamplerAnisotropy;
        sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        sampler_info.unnormalizedCoordinates = VK_FALSE;
        sampler_info.compareEnable = VK_FALSE;
        sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
        sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
        sampler_info.mipLodBias = 0.0f;
        sampler_info.minLod = 0.0f;
        sampler_info.maxLod = 0.0f;
        break;
    default:
        MASSERT_FALSE()
        break;
    }


    VK_CHECK(vkCreateSampler(renderer->logical_device, &sampler_info, 0, &out_texture->texture_sampler));
}
