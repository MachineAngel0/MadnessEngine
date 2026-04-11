#include "vk_renderpass.h"


void vulkan_renderpass_create_new(vulkan_context* context)
{
    Render_Graph render_graph = {0};

    Attachment_Handle depth_attachment_handle = vulkan_create_attachment(context, &render_graph, Attachment_Type_Color,
                                                                         VK_FORMAT_R16G16B16A16_SFLOAT,
                                                                         context->framebuffer_width,
                                                                         context->framebuffer_height);

    Attachment_Handle normal_attachment_handle = vulkan_create_attachment(context, &render_graph, Attachment_Type_Color,
                                                                          VK_FORMAT_R16G16B16A16_SFLOAT,
                                                                          context->framebuffer_width,
                                                                          context->framebuffer_height);

    Attachment_Handle color_attachment_handle = vulkan_create_attachment(context, &render_graph, Attachment_Type_Color,
                                                                         VK_FORMAT_R8G8B8A8_UNORM,
                                                                         context->framebuffer_width,
                                                                         context->framebuffer_height);
}

void vulkan_renderpass_insert_memory_barrier(vulkan_context* context, vulkan_command_buffer* command_buffer)
{
    // A new feature of the dynamic rendering local read extension is the ability to use pipeline barriers in the dynamic render pass
    // to allow framebuffer-local dependencies (i.e. read-after-write) between draw calls using the "by region" flag
    // So with this barrier we can use the output attachments from the draw call above as input attachments in the next call


    //TODO: add the type of transition we want
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
    vkCmdPipelineBarrier2(command_buffer->handle, &dependencyInfo);
}


Attachment_Handle vulkan_create_attachment(vulkan_context* context, Render_Graph* render_graph, Attachment_Type type,
                                           VkFormat format, u32 width, u32 height)
{
    Attachment* attachment = &render_graph->attachments[render_graph->attachments_count++];
    Attachment_Handle attachment_handle = {render_graph->attachments_count - 1};
    attachment->type = type;
    attachment->image_width = width;
    attachment->image_height = height;

    VkImageUsageFlags usage = 0;
    usage |= VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;

    switch (type)
    {
    case Attachment_Type_Color:
        usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        break;
    case Attachment_Type_Depth_Stencil:
        usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        break;
        break;
    }


    VkImageAspectFlags aspect_mask = {0};
    if (usage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
    {
        aspect_mask = VK_IMAGE_ASPECT_COLOR_BIT;
    }
    if (usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
    {
        aspect_mask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
    }
    MASSERT(aspect_mask > 0);

    VkImageCreateInfo image_create_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = format,
        .extent = {
            .width = width, .height = height,
            .depth = 1
        },
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = usage,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    };

    VkResult create_image_result = vkCreateImage(context->device.logical_device, &image_create_info, context->allocator,
                                                 &attachment->image);
    VK_CHECK(create_image_result);

    VkMemoryRequirements memory_requirements;
    vkGetImageMemoryRequirements(context->device.logical_device, attachment->image, &memory_requirements);

    VkMemoryAllocateInfo memory_allocate_info = {.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    vkGetImageMemoryRequirements(context->device.logical_device, attachment->image, &memory_requirements);
    memory_allocate_info.allocationSize = memory_requirements.size;
    memory_allocate_info.memoryTypeIndex = find_memory_type(context, memory_requirements.memoryTypeBits,
                                                            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    if (memory_allocate_info.memoryTypeIndex == -1)
    {
        M_ERROR("Required memory type not found. Image not valid.");
    }

    VkResult alloc_memory_result = vkAllocateMemory(context->device.logical_device, &memory_allocate_info,
                                                    context->allocator, &attachment->memory);
    VK_CHECK(alloc_memory_result);
    VkResult bind_image_memory_result = vkBindImageMemory(context->device.logical_device, attachment->image,
                                                          attachment->memory, 0);
    VK_CHECK(bind_image_memory_result);

    VkImageViewCreateInfo image_view_create_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = attachment->image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = format,
        .subresourceRange = {
            .aspectMask = aspect_mask, .baseMipLevel = 0, .levelCount = VK_REMAINING_MIP_LEVELS, .baseArrayLayer = 0,
            .layerCount = VK_REMAINING_ARRAY_LAYERS
        },
    };
    VK_CHECK(
        vkCreateImageView(context->device.logical_device, &image_view_create_info, context->allocator, &attachment->view
        ));


    // Without render passes and their implicit layout transitions, we need to explicitly transition the attachments
    // We use a new layout introduced by this extension that makes writes to images visible via input attachments
    vulkan_command_buffer temp_command_buffer;
    vulkan_command_buffer_allocate_and_begin_single_use(context,
                                                        context->graphics_command_pool, &temp_command_buffer);


    VkImageMemoryBarrier2 image_memory_barrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2_KHR,
        .srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR,
        .dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR,
        .dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT_KHR,
        .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .newLayout = VK_IMAGE_LAYOUT_RENDERING_LOCAL_READ_KHR,
        .image = attachment->image,
        .subresourceRange = image_view_create_info.subresourceRange,
    };
    VkDependencyInfo dependency_info = {
        .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO_KHR,
        .imageMemoryBarrierCount = 1,
        .pImageMemoryBarriers = &image_memory_barrier
    };
    vkCmdPipelineBarrier2(temp_command_buffer.handle, &dependency_info);

    vulkan_command_buffer_end_single_use(context, context->graphics_command_pool,
                                         &temp_command_buffer, context->device.graphics_queue);

    return attachment_handle;
}

void vulkan_renderpass_create(vulkan_context* context, vulkan_renderpass* out_renderpass, vec4 screen_pos,
                              vec4 clear_color,
                              f32 depth, u32 stencil)
{
    out_renderpass->screen_pos = screen_pos;
    out_renderpass->clear_color = clear_color;
    out_renderpass->depth = depth;
    out_renderpass->stencil = stencil;


    // Main subpass
    VkSubpassDescription subpass = {0};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

    // Attachments TODO: make this configurable.
    //TODO: free
    u32 attachment_description_count = 2;
    VkAttachmentDescription* attachment_descriptions = malloc(
        sizeof(VkAttachmentDescription) * attachment_description_count);

    // Color attachment
    VkAttachmentDescription color_attachment = {0};
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

    VkAttachmentReference color_attachment_reference = {0};
    color_attachment_reference.attachment = 0; // Attachment description array index
    color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_reference;


    // Depth attachment, if there is one
    VkAttachmentDescription depth_attachment = {0};
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
    VkAttachmentReference depth_attachment_reference = {0};
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
    VkSubpassDependency dependency = {0};
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

void vulkan_renderpass_begin(Renderer* renderer, vulkan_command_buffer* command_buffer, u32 current_frame)
{
    // With dynamic rendering we need to explicitly add layout transitions by using barriers, this set of barriers prepares the color and depth images for output
    image_insert_memory_barrier(command_buffer->handle,
                                renderer->context.swapchain.images[current_frame],
                                0,
                                VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                                VK_IMAGE_LAYOUT_UNDEFINED,
                                VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
                                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                (VkImageSubresourceRange){VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}
    );
    image_insert_memory_barrier(command_buffer->handle,
                                renderer->context.swapchain.depth_attachment.texture_image,
                                0,
                                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                                VK_IMAGE_LAYOUT_UNDEFINED,
                                VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
                                VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
                                VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
                                (VkImageSubresourceRange){
                                    VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, 0, 1, 0, 1
                                }
    );

    //
    // // Begin the render pass.
    // vulkan_renderpass_begin(
    //     command_buffer_current_frame,
    //     &vk_context.main_renderpass,
    //     vk_context.swapchain.framebuffers[image_index].framebuffer_handle);

    //with dynamic rendering we need to add a layout transition, using barriers


    // Set up the rendering attachment info
    VkRenderingAttachmentInfo color_attachment = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .imageView = renderer->context.swapchain.image_views[current_frame],
        .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .clearValue.color = {0.0f, 0.0f, 0.2f, 0.0f},
    };

    VkRenderingInfo rendering_info = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
        .renderArea = {
            .offset = {0, 0},
            .extent = {renderer->context.framebuffer_width, renderer->context.framebuffer_height}
        },
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments = &color_attachment,
    };
    vkCmdBeginRendering(command_buffer->handle, &rendering_info);
}


void vulkan_renderpass_end(Renderer* renderer, vulkan_command_buffer* command_buffer, u32 current_frame)
{
    // Finish the current dynamic rendering section
    vkCmdEndRendering(command_buffer->handle);

    // End renderpass
}

void vulkan_renderpass_UI_begin(Renderer* renderer, vulkan_command_buffer* command_buffer, u32 current_frame)
{
    // With dynamic rendering we need to explicitly add layout transitions by using barriers, this set of barriers prepares the color and depth images for output
    VkImageSubresourceRange image_subresource_range = {0};
    image_subresource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    image_subresource_range.baseMipLevel = 0;
    image_subresource_range.levelCount = 1;
    image_subresource_range.baseArrayLayer = 0;
    image_subresource_range.layerCount = 1;


    image_insert_memory_barrier(command_buffer->handle,
                                renderer->context.swapchain.images[current_frame],
                                0,
                                VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                                VK_IMAGE_LAYOUT_UNDEFINED,
                                VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
                                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                image_subresource_range
    );

    //
    // // Begin the render pass.
    // vulkan_renderpass_begin(
    //     command_buffer_current_frame,
    //     &vk_context.main_renderpass,
    //     vk_context.swapchain.framebuffers[image_index].framebuffer_handle);

    //with dynamic rendering we need to add a layout transition, using barriers


    // Set up the rendering attachment info
    VkRenderingAttachmentInfo color_attachment = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .imageView = renderer->context.swapchain.image_views[current_frame],
        .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        // might not need to be store, can just be none if its lasts in the pipeline
        .clearValue.color = {0.0f, 0.0f, 0.2f, 0.0f},
    };

    VkRenderingInfo rendering_info = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
        .renderArea = {
            .offset = {0, 0},
            .extent = {renderer->context.framebuffer_width, renderer->context.framebuffer_height}
        },
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments = &color_attachment,
    };
    vkCmdBeginRendering(command_buffer->handle, &rendering_info);
}

void vulkan_renderpass_UI_end(Renderer* renderer, vulkan_command_buffer* command_buffer, u32 current_frame)
{
    // Finish the current dynamic rendering section
    vkCmdEndRendering(command_buffer->handle);

    // End renderpass

    // This barrier prepares the color image for presentation, we don't need to care for the depth image
    image_insert_memory_barrier(command_buffer->handle,
                                renderer->context.swapchain.images[current_frame], VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                                0,
                                VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_2_NONE,
                                (VkImageSubresourceRange){VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});
}
