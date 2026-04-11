#ifndef VK_FENCES_H
#define VK_FENCES_H



void init_per_frame_sync(vulkan_context* context);

bool vulkan_fence_wait(vulkan_context* context, VkFence* fence, u64 timeout_ns);


void insert_memory_barrier2_test_function_do_not_use_yet(VkCommandBuffer cmdbuffer, VkImage image, VkAccessFlags srcAccessMask,
                                 VkAccessFlags dstAccessMask, VkImageLayout oldImageLayout,
                                 VkImageLayout newImageLayout,
                                 VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask,
                                 VkImageSubresourceRange subresourceRange)
{
    //FUTURE:
    //NOTE: an example
    VkMemoryBarrier2KHR memoryBarrier = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2_KHR,
        .srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        .srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
        .dstAccessMask = VK_ACCESS_2_INPUT_ATTACHMENT_READ_BIT
    };
    VkDependencyInfoKHR dependencyInfo = {
        .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO_KHR,
        .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
        .memoryBarrierCount = 1,
        .pMemoryBarriers = &memoryBarrier
    };
    vkCmdPipelineBarrier2(cmdbuffer, &dependencyInfo);
}



#endif
