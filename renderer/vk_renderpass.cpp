#include "vk_renderpass.h"

#include <iostream>

#include "Renderer.h"
#include "vk_command_buffer.h"
#include "vk_descriptor.h"

void renderpass_create(Vulkan_Context& vulkan_context, Swapchain_Context& swapchain_context,
                       Graphics_Context& graphics_context, unsigned char clear_flags, bool has_prev_pass, bool has_next_pass)
{
    //TODO: CHANGE TO DYNAMIC RENDERING AND REPLACES RENDER PASS AND FRAME BUFFer
    //RESOURCE: https://www.youtube.com/watch?v=m1RHLavNjKo&t=624s
    //VkPipelineRenderingCreateInfo
    //VK_KHR_dynamic_rendering_local_read dynamic_render;


    //VkPipelineLayout pipelineLayout; idk what this is here for

    unsigned char do_clear_color = (clear_flags & RENDER_PASS_CLEAR_COLOR_BUFFER_FLAG) != 0;

    //TODO: multisampling
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = swapchain_context.surface_format.format;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    //if our clear flag is true, then we clear the screen
    colorAttachment.loadOp = do_clear_color ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD; // we could use this as well
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    //if coming from a previous pass, then we want to be in color_attachment otherwise undefiend
    colorAttachment.initialLayout = has_prev_pass ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_UNDEFINED; // we could use this as well
    // if we are the last renderpass, then we want to display the image, otherwise, color attachment
    colorAttachment.finalLayout = has_next_pass ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // we could use this as well

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;


    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    //TODO:
    //VkAttachmentDescription depth_attachment{};
    //VkAttachmentReference depth_attachment{};


    if (vkCreateRenderPass(vulkan_context.logical_device, &renderPassInfo, nullptr,
                           &graphics_context.render_pass) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create render pass!");
    }

    std::cout << "CREATED RENDERPASS SUCCESS\n";
}

void renderpass_begin(Vulkan_Context& vulkan_context, Swapchain_Context& swapchain_context,
    Command_Buffer_Context* command_buffer, Graphics_Context& graphics_context, uint32_t image_index,
    unsigned char clear_flags)
{

    //start the render pass
    VkRenderPassBeginInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_info.renderPass = graphics_context.render_pass;
    render_pass_info.framebuffer = graphics_context.frame_buffers[image_index];
    render_pass_info.renderArea.offset = {0, 0};
    render_pass_info.renderArea.extent = swapchain_context.surface_capabilities.currentExtent;

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}}; //black color
    render_pass_info.clearValueCount = 1;
    render_pass_info.pClearValues = &clearColor;

    /*
    unsigned char do_clear_colour = (clear_flags & RENDER_PASS_CLEAR_COLOR_BUFFER_FLAG) != 0;
    if (do_clear_colour) {
        kcopy_memory(clear_values[begin_info.clearValueCount].color.float32, renderpass->clear_colour.elements, sizeof(float) * 4);
        begin_info.clearValueCount++;
    }*/

    /*
    unsigned char do_clear_depth = (clear_flags & RENDER_PASS_CLEAR_DEPTH_BUFFER_FLAG) != 0;
    if (do_clear_depth) {
        kcopy_memory(clear_values[begin_info.clearValueCount].color.float32, renderpass->clear_colour.elements, sizeof(f32) * 4);
        clear_values[begin_info.clearValueCount].depthStencil.depth = renderpass->depth;

        b8 do_clear_stencil = (renderpass->clear_flags & RENDERPASS_CLEAR_STENCIL_BUFFER_FLAG) != 0;
        clear_values[begin_info.clearValueCount].depthStencil.stencil = do_clear_stencil ? renderpass->stencil : 0;
        begin_info.clearValueCount++;
    }*/


    vkCmdBeginRenderPass(command_buffer->command_buffer[image_index], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
}

void renderpass_end(Command_Buffer_Context* command_buffer, uint32_t frame)
{
    vkCmdEndRenderPass(command_buffer->command_buffer[frame]);
}
