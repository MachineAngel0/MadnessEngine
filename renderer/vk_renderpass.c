#include "vk_renderpass.h"


void vulkan_renderpass_create(vulkan_context* context, vulkan_renderpass* out_renderpass, vec4 screen_pos, vec4 clear_color,
                              f32 depth, u32 stencil)
{

    out_renderpass->screen_pos = screen_pos;
    out_renderpass->clear_color = clear_color;
    out_renderpass->depth = depth;
    out_renderpass->stencil = stencil;



    // Main subpass
    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

    // Attachments TODO: make this configurable.
    //TODO: free
    u32 attachment_description_count = 2;
    VkAttachmentDescription* attachment_descriptions = malloc(sizeof(VkAttachmentDescription) * attachment_description_count);

    // Color attachment
    VkAttachmentDescription color_attachment;
    color_attachment.format = context->swapchain.surface_format.format; // TODO: configurable
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    // Do not expect any particular layout before render pass starts.


    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // Transitioned to after the render pass
    color_attachment.flags = 0;
    attachment_descriptions[0] = color_attachment;

    VkAttachmentReference color_attachment_reference;
    color_attachment_reference.attachment = 0; // Attachment description array index
    color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_reference;


    // Depth attachment, if there is one
    VkAttachmentDescription depth_attachment = {};
    depth_attachment.format = context->device.depth_format;
    depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    attachment_descriptions[1] = depth_attachment;


    // Depth attachment reference
    VkAttachmentReference depth_attachment_reference;
    depth_attachment_reference.attachment = 1;
    depth_attachment_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;


    // TODO: other attachment types (input, resolve, preserve)
    // Depth stencil data.
    subpass.pDepthStencilAttachment = &depth_attachment_reference;

    // Input from a shader
    subpass.inputAttachmentCount = 0;
    subpass.pInputAttachments = 0;

    // Attachments used for multisampling colour attachments
    subpass.pResolveAttachments = 0;

    // Attachments not used in this subpass, but must be preserved for the next.
    subpass.preserveAttachmentCount = 0;
    subpass.pPreserveAttachments = 0;

    // Render pass dependencies. TODO: make this configurable.
    VkSubpassDependency dependency;
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependency.dependencyFlags = 0;


    // Render pass create.
    VkRenderPassCreateInfo render_pass_create_info = {VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
    render_pass_create_info.attachmentCount = attachment_description_count;
    render_pass_create_info.pAttachments = attachment_descriptions;
    render_pass_create_info.subpassCount = 1;
    render_pass_create_info.pSubpasses = &subpass;
    render_pass_create_info.dependencyCount = 1;
    render_pass_create_info.pDependencies = &dependency;
    render_pass_create_info.pNext = 0;
    render_pass_create_info.flags = 0;

    VK_CHECK(vkCreateRenderPass(
        context->device.logical_device,
        &render_pass_create_info,
        context->allocator,
        &out_renderpass->handle));
}

void vulkan_renderpass_destroy(vulkan_context* context, vulkan_renderpass* renderpass)
{
    if (renderpass && renderpass->handle)
    {
        vkDestroyRenderPass(context->device.logical_device, renderpass->handle, context->allocator);
        renderpass->handle = 0;
    }
}

void vulkan_renderpass_begin(vulkan_command_buffer* command_buffer, vulkan_renderpass* renderpass,
                             VkFramebuffer frame_buffer)
{
    VkRenderPassBeginInfo begin_info = {VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
    begin_info.renderPass = renderpass->handle;
    begin_info.framebuffer = frame_buffer;
    begin_info.renderArea.offset.x = renderpass->screen_pos.x;
    begin_info.renderArea.offset.y = renderpass->screen_pos.y;
    begin_info.renderArea.extent.height = renderpass->screen_pos.h;
    begin_info.renderArea.extent.width = renderpass->screen_pos.w;

    VkClearValue clear_values[2] = {0};

    clear_values[0].color.float32[0] = renderpass->clear_color.r;
    clear_values[0].color.float32[1] = renderpass->clear_color.g;
    clear_values[0].color.float32[2] = renderpass->clear_color.b;
    clear_values[0].color.float32[3] = renderpass->clear_color.a;
    clear_values[1].depthStencil.depth = renderpass->depth;
    clear_values[1].depthStencil.stencil = renderpass->stencil;

    begin_info.clearValueCount = 2;
    begin_info.pClearValues = clear_values;

    vkCmdBeginRenderPass(command_buffer->command_buffer_handle, &begin_info, VK_SUBPASS_CONTENTS_INLINE);
    command_buffer->state = COMMAND_BUFFER_STATE_IN_RENDER_PASS;
}

void vulkan_renderpass_end(vulkan_command_buffer* command_buffer, vulkan_renderpass* renderpass)
{
    vkCmdEndRenderPass(command_buffer->command_buffer_handle);


    command_buffer->state = COMMAND_BUFFER_STATE_RECORDING;
}


/*
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


    //unsigned char do_clear_colour = (clear_flags & RENDER_PASS_CLEAR_COLOR_BUFFER_FLAG) != 0;
    //if (do_clear_colour) {
    //    kcopy_memory(clear_values[begin_info.clearValueCount].color.float32, renderpass->clear_colour.elements, sizeof(float) * 4);
    //    begin_info.clearValueCount++;
    //}

    //unsigned char do_clear_depth = (clear_flags & RENDER_PASS_CLEAR_DEPTH_BUFFER_FLAG) != 0;
    //if (do_clear_depth) {
    //    kcopy_memory(clear_values[begin_info.clearValueCount].color.float32, renderpass->clear_colour.elements, sizeof(f32) * 4);
    //    clear_values[begin_info.clearValueCount].depthStencil.depth = renderpass->depth;
//
  //      b8 do_clear_stencil = (renderpass->clear_flags & RENDERPASS_CLEAR_STENCIL_BUFFER_FLAG) != 0;
    //    clear_values[begin_info.clearValueCount].depthStencil.stencil = do_clear_stencil ? renderpass->stencil : 0;
      //  begin_info.clearValueCount++;
    //}


    vkCmdBeginRenderPass(command_buffer->command_buffer[image_index], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
}

void renderpass_end(Command_Buffer_Context* command_buffer, uint32_t frame)
{
    vkCmdEndRenderPass(command_buffer->command_buffer[frame]);
}
*/
