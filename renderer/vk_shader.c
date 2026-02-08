#include "vk_shader.h"


VkShaderModule create_shader_module(const vulkan_context* context, const char* shader_bytes, const u64 shader_size)
{
    VkShaderModuleCreateInfo shader_module_create_info = {0};
    //shader_module_create_info.flags = 0;
    //vertex_create_info.pNext;
    shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shader_module_create_info.codeSize = shader_size;
    shader_module_create_info.pCode = (uint32_t*)shader_bytes;

    VkShaderModule shader_module;
    if (vkCreateShaderModule(context->device.logical_device, &shader_module_create_info, NULL, &shader_module) !=
        VK_SUCCESS)
    {
        FATAL("failed to create vertex shader module!");
    };
    return shader_module;
}


bool vulkan_default_shader_create(vulkan_context* context, vulkan_shader_default* shader)
{
    // Pipeline layout creation

    VkPipelineLayoutCreateInfo pipeline_layout_info = {0};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = 1;
    pipeline_layout_info.pSetLayouts = &context->default_shader_info.descriptor_set_layout;
    // pipeline_layout_info.pushConstantRangeCount = 0;
    // pipeline_layout_info.pPushConstantRanges = 0;


    // TODO: Pipeline and Push Constants
    //pipeline layout is the only thing the graphics pipeline needs, the descriptor sets can be created separately
    VkResult pipeline_result = vkCreatePipelineLayout(context->device.logical_device, &pipeline_layout_info, NULL,
                                                      &shader->default_shader_pipeline.pipeline_layout);
    VK_CHECK(pipeline_result);
    //graphics pipeline
    file_read_data vert_data = {0};
    file_read_data frag_data = {0};

    filesystem_open_and_return_bytes("../renderer/shaders/shader.vert.spv", &vert_data);
    filesystem_open_and_return_bytes("../renderer/shaders/shader.frag.spv", &frag_data);
    if (vert_data.size == 0)
    {
        FATAL("Vertex shader file not loaded! Size: %llu", vert_data.size);
    }
    if (frag_data.size == 0)
    {
        FATAL("Fragment shader file not loaded! Size: %llu", frag_data.size);
    }


    VkShaderModule vert_shader_module = create_shader_module(context, vert_data.data, vert_data.size);
    VkShaderModule fragment_shader_module = create_shader_module(context, frag_data.data, frag_data.size);


    //create the shader stage info
    VkPipelineShaderStageCreateInfo vert_shader_stage_info = {0};
    vert_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    //vertShaderStageInfo.pNext;
    //vertShaderStageInfo.flags;
    //vertShaderStageInfo.pSpecializationInfo = nullptr;
    vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vert_shader_stage_info.module = vert_shader_module;
    vert_shader_stage_info.pName = "main";

    VkPipelineShaderStageCreateInfo frag_shader_stage_info = {0};
    frag_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    //vertShaderStageInfo.pNext;
    //vertShaderStageInfo.flags;
    //frag_ShaderStageInfo.pSpecializationInfo;
    frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    frag_shader_stage_info.module = fragment_shader_module;
    frag_shader_stage_info.pName = "main";

    VkPipelineShaderStageCreateInfo shader_stages[] = {vert_shader_stage_info, frag_shader_stage_info};

    //vertex info
    VkVertexInputBindingDescription binding_description = {0};
    binding_description.binding = 0;
    binding_description.stride = sizeof(vertex_3d);
    /*
    * VK_VERTEX_INPUT_RATE_VERTEX: Move to the next data entry after each vertex
    * VK_VERTEX_INPUT_RATE_INSTANCE: Move to the next data entry after each instance
    */
    binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    spirv_reflect_input_variable_info* attribute_info =
        spriv_reflect_get_input_variable(NULL, "../renderer/shaders/shader_texture.vert.spv");
    u32 offset_total = 0;

    VkVertexInputAttributeDescription* attribute_descriptions = malloc(
        sizeof(VkVertexInputAttributeDescription) * attribute_info->input_count);
    for (u32 attribute_index = 0; attribute_index < attribute_info->input_count; attribute_index++)
    {
        attribute_descriptions[attribute_index].binding = 0;
        attribute_descriptions[attribute_index].location = attribute_info->locations[attribute_index];
        attribute_descriptions[attribute_index].format = (VkFormat)attribute_info->formats[attribute_index];

        offset_total += attribute_info->offsets[attribute_index];
        attribute_descriptions[attribute_index].offset = offset_total;
    }

    /*VkVertexInputAttributeDescription attribute_descriptions[2];
    //position
    attribute_descriptions[0].binding = 0; //referencing which VkVertexInputBindingDescription binding we are using
    attribute_descriptions[0].location = 0;
    attribute_descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attribute_descriptions[0].offset = offsetof(vertex, position); //offsetof is pretty interesting

    //color
    attribute_descriptions[1].binding = 0; //referencing which VkVertexInputBindingDescription binding we are using
    attribute_descriptions[1].location = 1;
    // attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attribute_descriptions[1].offset = offsetof(vertex, color); //offsetof is pretty interesting*/

    VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = {0};
    vertex_input_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_state_create_info.vertexBindingDescriptionCount = 1; // the number of binding_description
    vertex_input_state_create_info.pVertexBindingDescriptions = &binding_description;
    vertex_input_state_create_info.vertexAttributeDescriptionCount = 2;
    vertex_input_state_create_info.pVertexAttributeDescriptions = attribute_descriptions;
    //vertex_input_state_create_info.pNext;
    //vertex_input_state_create_info.flags;

    //The VkPipelineInputAssemblyStateCreateInfo struct describes two things: what kind of geometry will be drawn from the vertices
    //and if primitive restart should be enabled.
    VkPipelineInputAssemblyStateCreateInfo input_assembly = {0};
    input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    // input_assembly.primitiveRestartEnable = VK_FALSE;
    //pInputAssemblyState.pNext;
    //pInputAssemblyState.flags;

    VkPipelineRasterizationStateCreateInfo rasterizer = {0};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    // VK_POLYGON_MODE_LINE for wireframes, VK_POLYGON_MODE_POINT for just points, using these require gpu features
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.cullMode = VK_CULL_MODE_NONE; //TODO: temp
    // rasterizer.cullMode = VK_CULL_MODE_BACK_BIT; //discard back facing triangles
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthClampEnable = VK_FALSE; //useful for shadow maps, turn it on but need gpu features
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.lineWidth = 1.0f;
    //MIGHT BE USEFUL FOR SHADOW MAPPING
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f;
    rasterizer.depthBiasClamp = 0.0f;
    rasterizer.depthBiasSlopeFactor = 0.0f;
    //rasterizer.pNext;
    //rasterizer.flags;

    //TODO: not in use for now, but this is where we would do our anti aliasing
    VkPipelineMultisampleStateCreateInfo multisampling = {0};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.sampleShadingEnable = VK_FALSE;
    // multisampling.minSampleShading = 1.0f; // Optional
    // multisampling.pSampleMask = 0; // Optional
    // multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    // multisampling.alphaToOneEnable = VK_FALSE; // Optional

    //TODO: uncomment when needed
    // Depth and stencil testing.
    VkPipelineDepthStencilStateCreateInfo depth_stencil = {0};
    depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth_stencil.depthTestEnable = VK_TRUE;
    depth_stencil.depthWriteEnable = VK_TRUE;
    depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    depth_stencil.depthBoundsTestEnable = VK_FALSE;
    depth_stencil.back.failOp = VK_STENCIL_OP_KEEP;
    depth_stencil.back.passOp = VK_STENCIL_OP_KEEP;
    depth_stencil.back.compareOp = VK_COMPARE_OP_ALWAYS;
    depth_stencil.stencilTestEnable = VK_FALSE;
    depth_stencil.front = depth_stencil.back;


    //happens after color returns from the fragment shader
    //METHOD:
    //Mix the old and new value to produce a final color
    //Combine the old and new value using a bitwise operation
    /* Both ways showcased below
    * if (blendEnable) {
    finalColor.rgb = (srcColorBlendFactor * newColor.rgb) <colorBlendOp> (dstColorBlendFactor * oldColor.rgb);
    finalColor.a = (srcAlphaBlendFactor * newColor.a) <alphaBlendOp> (dstAlphaBlendFactor * oldColor.a);
    }
    else {
    finalColor = newColor;
    }
    finalColor = finalColor & colorWriteMask;
     */

    VkPipelineColorBlendAttachmentState colorBlendAttachment = {0};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT
        | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    //colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    //colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    //colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
    //colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    //colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    //colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

    VkPipelineColorBlendStateCreateInfo color_blending = {0};
    color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blending.logicOpEnable = VK_FALSE;
    color_blending.logicOp = VK_LOGIC_OP_COPY;
    color_blending.attachmentCount = 1;
    color_blending.pAttachments = &colorBlendAttachment; // this thing can be a darray
    color_blending.blendConstants[0] = 0.0f; // Optional
    color_blending.blendConstants[1] = 0.0f; // Optional
    color_blending.blendConstants[2] = 0.0f; // Optional
    color_blending.blendConstants[3] = 0.0f; // Optional

    VkPipelineViewportStateCreateInfo viewport_state = {0};
    viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.viewportCount = 1;
    viewport_state.scissorCount = 1;
    //viewport_state.pNext;
    //viewport_state.flags;
    //viewport_state.pViewports; these two are not needed since we are doing dynamic viewport state
    //viewport_state.pScissors;


    //for resizing the viewport, can be used for blend constants
    VkDynamicState dynamicStates[2] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamicState = {0};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = 2;
    dynamicState.pDynamicStates = dynamicStates;

    // Attachment information for dynamic rendering
    VkPipelineRenderingCreateInfoKHR pipeline_rendering_create_info = {0};
    pipeline_rendering_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
    pipeline_rendering_create_info.colorAttachmentCount = 1;
    pipeline_rendering_create_info.pColorAttachmentFormats = &context->swapchain.surface_format.format;
    pipeline_rendering_create_info.depthAttachmentFormat = context->device.depth_format;
    pipeline_rendering_create_info.stencilAttachmentFormat = context->device.depth_format;


    VkGraphicsPipelineCreateInfo graphics_pipeline_info = {0};
    graphics_pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphics_pipeline_info.stageCount = 2;
    graphics_pipeline_info.pStages = shader_stages;
    graphics_pipeline_info.pVertexInputState = &vertex_input_state_create_info;
    graphics_pipeline_info.pInputAssemblyState = &input_assembly;
    graphics_pipeline_info.pViewportState = &viewport_state;
    graphics_pipeline_info.pRasterizationState = &rasterizer;
    graphics_pipeline_info.pMultisampleState = &multisampling;
    graphics_pipeline_info.pDepthStencilState = &depth_stencil;
    graphics_pipeline_info.pColorBlendState = &color_blending;
    graphics_pipeline_info.pDynamicState = &dynamicState;
    graphics_pipeline_info.layout = shader->default_shader_pipeline.pipeline_layout;
    graphics_pipeline_info.renderPass = 0; // this has to be null if we are doing dynamic rendering
    // graphics_pipeline_info.subpass = 0;
    // graphics_pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
    // graphics_pipeline_info.basePipelineIndex = -1;
    graphics_pipeline_info.pNext = &pipeline_rendering_create_info;


    VkResult graphics_result = vkCreateGraphicsPipelines(context->device.logical_device, VK_NULL_HANDLE, 1,
                                                         &graphics_pipeline_info, NULL,
                                                         &shader->default_shader_pipeline.handle);

    if (graphics_result != VK_SUCCESS)
    {
        FATAL("failed to create graphics pipeline!");
    }

    //TODO: replace with scratch arena
    file_read_data_free(&vert_data);
    file_read_data_free(&frag_data);
    //TODO: might want move out into the shader destroy
    vkDestroyShaderModule(context->device.logical_device, fragment_shader_module, NULL);
    vkDestroyShaderModule(context->device.logical_device, vert_shader_module, NULL);

    return true;
}

void vulkan_default_shader_destroy(vulkan_context* context, vulkan_shader_pipeline* pipeline,
                                   VkDescriptorSetLayout* descriptor_set_layout)
{
    vulkan_pipeline_destroy(context, pipeline);

    vkDestroyDescriptorSetLayout(context->device.logical_device, *descriptor_set_layout, 0);
}

void vulkan_default_shader_pipeline_bind(vulkan_command_buffer* command_buffer, vulkan_shader_pipeline* pipeline)
{
    vulkan_pipeline_bind(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                         pipeline);
}

bool vulkan_textured_shader_create(vulkan_context* context, vulkan_shader_texture* textured_shader,
                                   VkDescriptorSetLayout* descriptor_layout)
{
    // Pipeline layout creation
    VkPipelineLayoutCreateInfo pipeline_layout_info = {0};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = 1;
    pipeline_layout_info.pSetLayouts = descriptor_layout;
    // pipeline_layout_info.pushConstantRangeCount = 0;
    // pipeline_layout_info.pPushConstantRanges = 0;


    // TODO: Pipeline and Push Constants
    //pipeline layout is the only thing the graphics pipeline needs, the descriptor sets can be created separately
    VkResult pipeline_result = vkCreatePipelineLayout(context->device.logical_device, &pipeline_layout_info, NULL,
                                                      &textured_shader->shader_texture_pipeline.pipeline_layout);
    VK_CHECK(pipeline_result);
    //graphics pipeline
    file_read_data vert_data = {0};
    file_read_data frag_data = {0};

    filesystem_open_and_return_bytes("../renderer/shaders/shader_texture.vert.spv", &vert_data);
    filesystem_open_and_return_bytes("../renderer/shaders/shader_texture.frag.spv", &frag_data);
    if (vert_data.size == 0)
    {
        FATAL("Vertex shader file not loaded! Size: %llu", vert_data.size);
    }
    if (frag_data.size == 0)
    {
        FATAL("Fragment shader file not loaded! Size: %llu", frag_data.size);
    }


    VkShaderModule vert_shader_module = create_shader_module(context, vert_data.data, vert_data.size);
    VkShaderModule fragment_shader_module = create_shader_module(context, frag_data.data, frag_data.size);


    //create the shader stage info
    VkPipelineShaderStageCreateInfo vert_shader_stage_info = {0};
    vert_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    //vertShaderStageInfo.pNext;
    //vertShaderStageInfo.flags;
    //vertShaderStageInfo.pSpecializationInfo = nullptr;
    vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vert_shader_stage_info.module = vert_shader_module;
    vert_shader_stage_info.pName = "main";

    VkPipelineShaderStageCreateInfo frag_shader_stage_info = {0};
    frag_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    //vertShaderStageInfo.pNext;
    //vertShaderStageInfo.flags;
    //frag_ShaderStageInfo.pSpecializationInfo;
    frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    frag_shader_stage_info.module = fragment_shader_module;
    frag_shader_stage_info.pName = "main";

    VkPipelineShaderStageCreateInfo shader_stages[] = {vert_shader_stage_info, frag_shader_stage_info};

    //vertex info
    VkVertexInputBindingDescription binding_description = {0};
    binding_description.binding = 0;
    binding_description.stride = sizeof(vertex_3d);
    /*
    * VK_VERTEX_INPUT_RATE_VERTEX: Move to the next data entry after each vertex
    * VK_VERTEX_INPUT_RATE_INSTANCE: Move to the next data entry after each instance
    */
    binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    spirv_reflect_input_variable_info* attribute_info =
        spriv_reflect_get_input_variable(NULL, "../renderer/shaders/shader_texture.vert.spv");
    u32 offset_total = 0;

    VkVertexInputAttributeDescription* attribute_descriptions = malloc(
        sizeof(VkVertexInputAttributeDescription) * attribute_info->input_count);
    for (u32 attribute_index = 0; attribute_index < attribute_info->input_count; attribute_index++)
    {
        attribute_descriptions[attribute_index].binding = 0;
        attribute_descriptions[attribute_index].location = attribute_info->locations[attribute_index];
        attribute_descriptions[attribute_index].format = (VkFormat)attribute_info->formats[attribute_index];

        offset_total += attribute_info->offsets[attribute_index];
        attribute_descriptions[attribute_index].offset = offset_total;
    }

    /*
    VkVertexInputAttributeDescription attribute_descriptions[3];
    //position
    attribute_descriptions[0].binding = 0;
    attribute_descriptions[0].location = 0;
    attribute_descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attribute_descriptions[0].offset = offsetof(vertex_tex, position);

    //color
    attribute_descriptions[1].binding = 0;
    attribute_descriptions[1].location = 1;
    attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attribute_descriptions[1].offset = offsetof(vertex_tex, color);

    //texture
    attribute_descriptions[2].binding = 0;
    attribute_descriptions[2].location = 2;
    attribute_descriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    attribute_descriptions[2].offset = offsetof(vertex_tex, texture);*/

    VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = {0};
    vertex_input_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_state_create_info.vertexBindingDescriptionCount = 1;
    vertex_input_state_create_info.pVertexBindingDescriptions = &binding_description;
    vertex_input_state_create_info.vertexAttributeDescriptionCount = 3;
    vertex_input_state_create_info.pVertexAttributeDescriptions = attribute_descriptions;
    //vertex_input_state_create_info.pNext;
    //vertex_input_state_create_info.flags;

    //The VkPipelineInputAssemblyStateCreateInfo struct describes two things: what kind of geometry will be drawn from the vertices
    //and if primitive restart should be enabled.
    VkPipelineInputAssemblyStateCreateInfo input_assembly = {0};
    input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    // input_assembly.primitiveRestartEnable = VK_FALSE;
    //pInputAssemblyState.pNext;
    //pInputAssemblyState.flags;

    VkPipelineRasterizationStateCreateInfo rasterizer = {0};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    // VK_POLYGON_MODE_LINE for wireframes, VK_POLYGON_MODE_POINT for just points, using these require gpu features
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.cullMode = VK_CULL_MODE_NONE; //TODO: temp
    // rasterizer.cullMode = VK_CULL_MODE_BACK_BIT; //discard back facing triangles
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthClampEnable = VK_FALSE; //useful for shadow maps, turn it on but need gpu features
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.lineWidth = 1.0f;
    //MIGHT BE USEFUL FOR SHADOW MAPPING
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f;
    rasterizer.depthBiasClamp = 0.0f;
    rasterizer.depthBiasSlopeFactor = 0.0f;
    //rasterizer.pNext;
    //rasterizer.flags;

    //TODO: not in use for now, but this is where we would do our anti aliasing
    VkPipelineMultisampleStateCreateInfo multisampling = {0};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.sampleShadingEnable = VK_FALSE;
    // multisampling.minSampleShading = 1.0f; // Optional
    // multisampling.pSampleMask = 0; // Optional
    // multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    // multisampling.alphaToOneEnable = VK_FALSE; // Optional

    //TODO: uncomment when needed
    // Depth and stencil testing.
    VkPipelineDepthStencilStateCreateInfo depth_stencil = {0};
    depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth_stencil.depthTestEnable = VK_TRUE;
    depth_stencil.depthWriteEnable = VK_TRUE;
    depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    depth_stencil.depthBoundsTestEnable = VK_FALSE;
    depth_stencil.back.failOp = VK_STENCIL_OP_KEEP;
    depth_stencil.back.passOp = VK_STENCIL_OP_KEEP;
    depth_stencil.back.compareOp = VK_COMPARE_OP_ALWAYS;
    depth_stencil.stencilTestEnable = VK_FALSE;
    depth_stencil.front = depth_stencil.back;


    //happens after color returns from the fragment shader
    //METHOD:
    //Mix the old and new value to produce a final color
    //Combine the old and new value using a bitwise operation
    /* Both ways showcased below
    * if (blendEnable) {
    finalColor.rgb = (srcColorBlendFactor * newColor.rgb) <colorBlendOp> (dstColorBlendFactor * oldColor.rgb);
    finalColor.a = (srcAlphaBlendFactor * newColor.a) <alphaBlendOp> (dstAlphaBlendFactor * oldColor.a);
    }
    else {
    finalColor = newColor;
    }
    finalColor = finalColor & colorWriteMask;
     */

    VkPipelineColorBlendAttachmentState colorBlendAttachment = {0};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT
        | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    //colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    //colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    //colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
    //colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    //colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    //colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

    VkPipelineColorBlendStateCreateInfo color_blending = {0};
    color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blending.logicOpEnable = VK_FALSE;
    color_blending.logicOp = VK_LOGIC_OP_COPY;
    color_blending.attachmentCount = 1;
    color_blending.pAttachments = &colorBlendAttachment; // this thing can be a darray
    color_blending.blendConstants[0] = 0.0f; // Optional
    color_blending.blendConstants[1] = 0.0f; // Optional
    color_blending.blendConstants[2] = 0.0f; // Optional
    color_blending.blendConstants[3] = 0.0f; // Optional

    VkPipelineViewportStateCreateInfo viewport_state = {0};
    viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.viewportCount = 1;
    viewport_state.scissorCount = 1;
    //viewport_state.pNext;
    //viewport_state.flags;
    //viewport_state.pViewports; these two are not needed since we are doing dynamic viewport state
    //viewport_state.pScissors;


    //for resizing the viewport, can be used for blend constants
    VkDynamicState dynamicStates[2] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamicState = {0};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = 2;
    dynamicState.pDynamicStates = dynamicStates;

    // Attachment information for dynamic rendering
    VkPipelineRenderingCreateInfoKHR pipeline_rendering_create_info = {0};
    pipeline_rendering_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
    pipeline_rendering_create_info.colorAttachmentCount = 1;
    pipeline_rendering_create_info.pColorAttachmentFormats = &context->swapchain.surface_format.format;
    pipeline_rendering_create_info.depthAttachmentFormat = context->device.depth_format;
    pipeline_rendering_create_info.stencilAttachmentFormat = context->device.depth_format;


    VkGraphicsPipelineCreateInfo graphics_pipeline_info = {0};
    graphics_pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphics_pipeline_info.stageCount = 2;
    graphics_pipeline_info.pStages = shader_stages;
    graphics_pipeline_info.pVertexInputState = &vertex_input_state_create_info;
    graphics_pipeline_info.pInputAssemblyState = &input_assembly;
    graphics_pipeline_info.pViewportState = &viewport_state;
    graphics_pipeline_info.pRasterizationState = &rasterizer;
    graphics_pipeline_info.pMultisampleState = &multisampling;
    graphics_pipeline_info.pDepthStencilState = &depth_stencil;
    graphics_pipeline_info.pColorBlendState = &color_blending;
    graphics_pipeline_info.pDynamicState = &dynamicState;
    graphics_pipeline_info.layout = textured_shader->shader_texture_pipeline.pipeline_layout;
    graphics_pipeline_info.renderPass = 0; // this has to be null if we are doing dynamic rendering
    // graphics_pipeline_info.subpass = 0;
    // graphics_pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
    // graphics_pipeline_info.basePipelineIndex = -1;
    graphics_pipeline_info.pNext = &pipeline_rendering_create_info;


    VkResult graphics_result = vkCreateGraphicsPipelines(context->device.logical_device, VK_NULL_HANDLE, 1,
                                                         &graphics_pipeline_info, NULL,
                                                         &textured_shader->shader_texture_pipeline.handle);

    if (graphics_result != VK_SUCCESS)
    {
        FATAL("failed to create graphics pipeline!");
    }

    //TODO: replace with scratch arena
    file_read_data_free(&vert_data);
    file_read_data_free(&frag_data);
    //TODO: might want move out into the shader destroy
    vkDestroyShaderModule(context->device.logical_device, fragment_shader_module, NULL);
    vkDestroyShaderModule(context->device.logical_device, vert_shader_module, NULL);

    return true;
}

bool vulkan_mesh_shader_create(renderer* renderer, vulkan_mesh_default* mesh_data)
{
    // Pipeline layout creation
    VkDescriptorSetLayout set_layouts[2] = {
        renderer->descriptor_system->uniform_descriptors.descriptor_set_layout,
        renderer->descriptor_system->texture_descriptors.descriptor_set_layout
    };

    VkPipelineLayoutCreateInfo pipeline_layout_info = {0};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = ARRAY_SIZE(set_layouts);
    pipeline_layout_info.pSetLayouts = set_layouts;
    // pipeline_layout_info.pushConstantRangeCount = 0;
    // pipeline_layout_info.pPushConstantRanges = 0;


    // TODO: Pipeline and Push Constants
    //pipeline layout is the only thing the graphics pipeline needs, the descriptor sets can be created separately
    VkResult pipeline_result = vkCreatePipelineLayout(renderer->context.device.logical_device, &pipeline_layout_info,
                                                      NULL,
                                                      &mesh_data->mesh_shader_pipeline.pipeline_layout);
    VK_CHECK(pipeline_result);
    //graphics pipeline
    file_read_data vert_data = {0};
    file_read_data frag_data = {0};

    filesystem_open_and_return_bytes("../renderer/shaders/shader_mesh.vert.spv", &vert_data);
    filesystem_open_and_return_bytes("../renderer/shaders/shader_mesh.frag.spv", &frag_data);
    if (vert_data.size == 0)
    {
        FATAL("Vertex shader file not loaded! Size: %llu", vert_data.size);
    }
    if (frag_data.size == 0)
    {
        FATAL("Fragment shader file not loaded! Size: %llu", frag_data.size);
    }


    VkShaderModule vert_shader_module = create_shader_module(&renderer->context, vert_data.data, vert_data.size);
    VkShaderModule fragment_shader_module = create_shader_module(&renderer->context, frag_data.data, frag_data.size);


    //create the shader stage info
    VkPipelineShaderStageCreateInfo vert_shader_stage_info = {0};
    vert_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    //vertShaderStageInfo.pNext;
    //vertShaderStageInfo.flags;
    //vertShaderStageInfo.pSpecializationInfo = nullptr;
    vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vert_shader_stage_info.module = vert_shader_module;
    vert_shader_stage_info.pName = "main";

    VkPipelineShaderStageCreateInfo frag_shader_stage_info = {0};
    frag_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    //vertShaderStageInfo.pNext;
    //vertShaderStageInfo.flags;
    //frag_ShaderStageInfo.pSpecializationInfo;
    frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    frag_shader_stage_info.module = fragment_shader_module;
    frag_shader_stage_info.pName = "main";

    VkPipelineShaderStageCreateInfo shader_stages[] = {vert_shader_stage_info, frag_shader_stage_info};

    //vertex info
    /*
 * VK_VERTEX_INPUT_RATE_VERTEX: Move to the next data entry after each vertex
 * VK_VERTEX_INPUT_RATE_INSTANCE: Move to the next data entry after each instance
 */
    VkVertexInputBindingDescription binding_description[4];
    binding_description[0].binding = 0;
    binding_description[0].stride = sizeof(vec3);
    binding_description[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    binding_description[1].binding = 0;
    binding_description[1].stride = sizeof(vec3);
    binding_description[1].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    binding_description[2].binding = 0;
    binding_description[2].stride = sizeof(vec4);
    binding_description[2].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    binding_description[3].binding = 0;
    binding_description[3].stride = sizeof(vec2);
    binding_description[3].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;


    spirv_reflect_input_variable_info* attribute_info =
        spriv_reflect_get_input_variable(NULL, "../renderer/shaders/shader_mesh.vert.spv");
    u32 offset_total = 0;

    VkVertexInputAttributeDescription* attribute_descriptions = malloc(
        sizeof(VkVertexInputAttributeDescription) * attribute_info->input_count);
    for (u32 attribute_index = 0; attribute_index < attribute_info->input_count; attribute_index++)
    {
        attribute_descriptions[attribute_index].binding = 0;
        attribute_descriptions[attribute_index].location = attribute_info->locations[attribute_index];
        attribute_descriptions[attribute_index].format = (VkFormat)attribute_info->formats[attribute_index];

        offset_total += attribute_info->offsets[attribute_index];
        attribute_descriptions[attribute_index].offset = offset_total;
    }


    /* TODO: REMOVE
    VkVertexInputAttributeDescription attribute_descriptions[2];
    attribute_descriptions[0].binding = 0;
    attribute_descriptions[0].location = 0;
    attribute_descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attribute_descriptions[0].offset = 0;

    attribute_descriptions[1].binding = 0;
    attribute_descriptions[1].location = 0;
    attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attribute_descriptions[1].offset = offsetof(vertex_3d, );
*/

    VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = {0};
    vertex_input_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_state_create_info.vertexBindingDescriptionCount = ARRAY_SIZE(binding_description);
    // the number of binding_description
    vertex_input_state_create_info.pVertexBindingDescriptions = binding_description;
    // vertex_input_state_create_info.vertexAttributeDescriptionCount = attribute_info->input_count;
    vertex_input_state_create_info.vertexAttributeDescriptionCount = 1;
    vertex_input_state_create_info.pVertexAttributeDescriptions = attribute_descriptions;
    //vertex_input_state_create_info.pNext;
    //vertex_input_state_create_info.flags;

    //The VkPipelineInputAssemblyStateCreateInfo struct describes two things: what kind of geometry will be drawn from the vertices
    //and if primitive restart should be enabled.
    VkPipelineInputAssemblyStateCreateInfo input_assembly = {0};
    input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    // input_assembly.primitiveRestartEnable = VK_FALSE;
    //pInputAssemblyState.pNext;
    //pInputAssemblyState.flags;

    VkPipelineRasterizationStateCreateInfo rasterizer = {0};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    // VK_POLYGON_MODE_LINE for wireframes, VK_POLYGON_MODE_POINT for just points, using these require gpu features
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.cullMode = VK_CULL_MODE_NONE; //TODO: temp
    // rasterizer.cullMode = VK_CULL_MODE_BACK_BIT; //discard back facing triangles
    // rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthClampEnable = VK_FALSE; //useful for shadow maps, turn it on but need gpu features
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.lineWidth = 1.0f;
    //MIGHT BE USEFUL FOR SHADOW MAPPING
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f;
    rasterizer.depthBiasClamp = 0.0f;
    rasterizer.depthBiasSlopeFactor = 0.0f;
    //rasterizer.pNext;
    //rasterizer.flags;

    //TODO: not in use for now, but this is where we would do our anti aliasing
    VkPipelineMultisampleStateCreateInfo multisampling = {0};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.sampleShadingEnable = VK_FALSE;
    // multisampling.minSampleShading = 1.0f; // Optional
    // multisampling.pSampleMask = 0; // Optional
    // multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    // multisampling.alphaToOneEnable = VK_FALSE; // Optional

    //TODO: uncomment when needed
    // Depth and stencil testing.
    VkPipelineDepthStencilStateCreateInfo depth_stencil = {0};
    depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth_stencil.depthTestEnable = VK_TRUE;
    depth_stencil.depthWriteEnable = VK_TRUE;
    depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    depth_stencil.depthBoundsTestEnable = VK_FALSE;
    depth_stencil.back.failOp = VK_STENCIL_OP_KEEP;
    depth_stencil.back.passOp = VK_STENCIL_OP_KEEP;
    depth_stencil.back.compareOp = VK_COMPARE_OP_ALWAYS;
    depth_stencil.stencilTestEnable = VK_FALSE;
    depth_stencil.front = depth_stencil.back;


    //happens after color returns from the fragment shader
    //METHOD:
    //Mix the old and new value to produce a final color
    //Combine the old and new value using a bitwise operation
    /* Both ways showcased below
    * if (blendEnable) {
    finalColor.rgb = (srcColorBlendFactor * newColor.rgb) <colorBlendOp> (dstColorBlendFactor * oldColor.rgb);
    finalColor.a = (srcAlphaBlendFactor * newColor.a) <alphaBlendOp> (dstAlphaBlendFactor * oldColor.a);
    }
    else {
    finalColor = newColor;
    }
    finalColor = finalColor & colorWriteMask;
     */

    VkPipelineColorBlendAttachmentState colorBlendAttachment = {0};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT
        | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    //colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    //colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    //colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
    //colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    //colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    //colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

    VkPipelineColorBlendStateCreateInfo color_blending = {0};
    color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blending.logicOpEnable = VK_FALSE;
    color_blending.logicOp = VK_LOGIC_OP_COPY;
    color_blending.attachmentCount = 1;
    color_blending.pAttachments = &colorBlendAttachment; // this thing can be a darray
    color_blending.blendConstants[0] = 0.0f; // Optional
    color_blending.blendConstants[1] = 0.0f; // Optional
    color_blending.blendConstants[2] = 0.0f; // Optional
    color_blending.blendConstants[3] = 0.0f; // Optional

    VkPipelineViewportStateCreateInfo viewport_state = {0};
    viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.viewportCount = 1;
    viewport_state.scissorCount = 1;
    //viewport_state.pNext;
    //viewport_state.flags;
    //viewport_state.pViewports; these two are not needed since we are doing dynamic viewport state
    //viewport_state.pScissors;


    //for resizing the viewport, can be used for blend constants
    VkDynamicState dynamicStates[2] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamicState = {0};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = 2;
    dynamicState.pDynamicStates = dynamicStates;

    // Attachment information for dynamic rendering
    VkPipelineRenderingCreateInfoKHR pipeline_rendering_create_info = {0};
    pipeline_rendering_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
    pipeline_rendering_create_info.colorAttachmentCount = 1;
    pipeline_rendering_create_info.pColorAttachmentFormats = &renderer->context.swapchain.surface_format.format;
    pipeline_rendering_create_info.depthAttachmentFormat = renderer->context.device.depth_format;
    pipeline_rendering_create_info.stencilAttachmentFormat = renderer->context.device.depth_format;


    VkGraphicsPipelineCreateInfo graphics_pipeline_info = {0};
    graphics_pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphics_pipeline_info.stageCount = 2;
    graphics_pipeline_info.pStages = shader_stages;
    graphics_pipeline_info.pVertexInputState = &vertex_input_state_create_info;
    graphics_pipeline_info.pInputAssemblyState = &input_assembly;
    graphics_pipeline_info.pViewportState = &viewport_state;
    graphics_pipeline_info.pRasterizationState = &rasterizer;
    graphics_pipeline_info.pMultisampleState = &multisampling;
    graphics_pipeline_info.pDepthStencilState = &depth_stencil;
    graphics_pipeline_info.pColorBlendState = &color_blending;
    graphics_pipeline_info.pDynamicState = &dynamicState;
    graphics_pipeline_info.layout = mesh_data->mesh_shader_pipeline.pipeline_layout;
    graphics_pipeline_info.renderPass = 0; // this has to be null if we are doing dynamic rendering
    // graphics_pipeline_info.subpass = 0;
    // graphics_pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
    // graphics_pipeline_info.basePipelineIndex = -1;
    graphics_pipeline_info.pNext = &pipeline_rendering_create_info;


    VkResult graphics_result = vkCreateGraphicsPipelines(renderer->context.device.logical_device, VK_NULL_HANDLE, 1,
                                                         &graphics_pipeline_info, NULL,
                                                         &mesh_data->mesh_shader_pipeline.handle);

    if (graphics_result != VK_SUCCESS)
    {
        FATAL("failed to create graphics pipeline!");
    }

    //TODO: replace with scratch arena
    file_read_data_free(&vert_data);
    file_read_data_free(&frag_data);
    //TODO: might want move out into the shader destroy
    vkDestroyShaderModule(renderer->context.device.logical_device, fragment_shader_module, NULL);
    vkDestroyShaderModule(renderer->context.device.logical_device, vert_shader_module, NULL);

    return true;
}

bool vulkan_bindless_textured_shader_create(renderer* renderer, vulkan_shader_texture* textured_shader)
{
    // Pipeline layout creation
    VkDescriptorSetLayout set_layouts[2] = {
        renderer->descriptor_system->uniform_descriptors.descriptor_set_layout,
        renderer->descriptor_system->texture_descriptors.descriptor_set_layout,
    };
    VkPipelineLayoutCreateInfo pipeline_layout_info = {0};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = ARRAY_SIZE(set_layouts);
    pipeline_layout_info.pSetLayouts = set_layouts;
    // pipeline_layout_info.pushConstantRangeCount = 0;
    // pipeline_layout_info.pPushConstantRanges = 0;


    // TODO: Pipeline and Push Constants
    //pipeline layout is the only thing the graphics pipeline needs, the descriptor sets can be created separately
    VkResult pipeline_result = vkCreatePipelineLayout(renderer->context.device.logical_device, &pipeline_layout_info,
                                                      NULL,
                                                      &textured_shader->shader_texture_pipeline.pipeline_layout);
    VK_CHECK(pipeline_result);
    //graphics pipeline
    file_read_data vert_data = {0};
    file_read_data frag_data = {0};

    filesystem_open_and_return_bytes("../renderer/shaders/shader_texture_descriptor.vert.spv", &vert_data);
    filesystem_open_and_return_bytes("../renderer/shaders/shader_texture_descriptor.frag.spv", &frag_data);
    if (vert_data.size == 0)
    {
        FATAL("Vertex shader file not loaded! Size: %llu", vert_data.size);
    }
    if (frag_data.size == 0)
    {
        FATAL("Fragment shader file not loaded! Size: %llu", frag_data.size);
    }


    VkShaderModule vert_shader_module = create_shader_module(&renderer->context, vert_data.data, vert_data.size);
    VkShaderModule fragment_shader_module = create_shader_module(&renderer->context, frag_data.data, frag_data.size);


    //create the shader stage info
    VkPipelineShaderStageCreateInfo vert_shader_stage_info = {0};
    vert_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    //vertShaderStageInfo.pNext;
    //vertShaderStageInfo.flags;
    //vertShaderStageInfo.pSpecializationInfo = nullptr;
    vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vert_shader_stage_info.module = vert_shader_module;
    vert_shader_stage_info.pName = "main";

    VkPipelineShaderStageCreateInfo frag_shader_stage_info = {0};
    frag_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    //vertShaderStageInfo.pNext;
    //vertShaderStageInfo.flags;
    //frag_ShaderStageInfo.pSpecializationInfo;
    frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    frag_shader_stage_info.module = fragment_shader_module;
    frag_shader_stage_info.pName = "main";

    VkPipelineShaderStageCreateInfo shader_stages[] = {vert_shader_stage_info, frag_shader_stage_info};

    //vertex info
    VkVertexInputBindingDescription binding_description = {0};
    binding_description.binding = 0;
    binding_description.stride = sizeof(vertex_3d);
    /*
    * VK_VERTEX_INPUT_RATE_VERTEX: Move to the next data entry after each vertex
    * VK_VERTEX_INPUT_RATE_INSTANCE: Move to the next data entry after each instance
    */
    binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    spirv_reflect_input_variable_info* attribute_info =
        spriv_reflect_get_input_variable(NULL, "../renderer/shaders/shader_texture_descriptor.vert.spv");
    u32 offset_total = 0;

    VkVertexInputAttributeDescription* attribute_descriptions = malloc(
        sizeof(VkVertexInputAttributeDescription) * attribute_info->input_count);
    for (u32 attribute_index = 0; attribute_index < attribute_info->input_count; attribute_index++)
    {
        attribute_descriptions[attribute_index].binding = 0;
        attribute_descriptions[attribute_index].location = attribute_info->locations[attribute_index];
        attribute_descriptions[attribute_index].format = (VkFormat)attribute_info->formats[attribute_index];

        offset_total += attribute_info->offsets[attribute_index];
        attribute_descriptions[attribute_index].offset = offset_total;
    }

    /*
    VkVertexInputAttributeDescription attribute_descriptions[3];
    //position
    attribute_descriptions[0].binding = 0;
    attribute_descriptions[0].location = 0;
    attribute_descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attribute_descriptions[0].offset = offsetof(vertex_tex, position);

    //color
    attribute_descriptions[1].binding = 0;
    attribute_descriptions[1].location = 1;
    attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attribute_descriptions[1].offset = offsetof(vertex_tex, color);

    //texture
    attribute_descriptions[2].binding = 0;
    attribute_descriptions[2].location = 2;
    attribute_descriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    attribute_descriptions[2].offset = offsetof(vertex_tex, texture);*/

    VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = {0};
    vertex_input_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_state_create_info.vertexBindingDescriptionCount = 1;
    vertex_input_state_create_info.pVertexBindingDescriptions = &binding_description;
    vertex_input_state_create_info.vertexAttributeDescriptionCount = 3;
    vertex_input_state_create_info.pVertexAttributeDescriptions = attribute_descriptions;
    //vertex_input_state_create_info.pNext;
    //vertex_input_state_create_info.flags;

    //The VkPipelineInputAssemblyStateCreateInfo struct describes two things: what kind of geometry will be drawn from the vertices
    //and if primitive restart should be enabled.
    VkPipelineInputAssemblyStateCreateInfo input_assembly = {0};
    input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    // input_assembly.primitiveRestartEnable = VK_FALSE;
    //pInputAssemblyState.pNext;
    //pInputAssemblyState.flags;

    VkPipelineRasterizationStateCreateInfo rasterizer = {0};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    // VK_POLYGON_MODE_LINE for wireframes, VK_POLYGON_MODE_POINT for just points, using these require gpu features
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.cullMode = VK_CULL_MODE_NONE; //TODO: temp
    // rasterizer.cullMode = VK_CULL_MODE_BACK_BIT; //discard back facing triangles
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthClampEnable = VK_FALSE; //useful for shadow maps, turn it on but need gpu features
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.lineWidth = 1.0f;
    //MIGHT BE USEFUL FOR SHADOW MAPPING
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f;
    rasterizer.depthBiasClamp = 0.0f;
    rasterizer.depthBiasSlopeFactor = 0.0f;
    //rasterizer.pNext;
    //rasterizer.flags;

    //TODO: not in use for now, but this is where we would do our anti aliasing
    VkPipelineMultisampleStateCreateInfo multisampling = {0};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.sampleShadingEnable = VK_FALSE;
    // multisampling.minSampleShading = 1.0f; // Optional
    // multisampling.pSampleMask = 0; // Optional
    // multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    // multisampling.alphaToOneEnable = VK_FALSE; // Optional

    //TODO: uncomment when needed
    // Depth and stencil testing.
    VkPipelineDepthStencilStateCreateInfo depth_stencil = {0};
    depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth_stencil.depthTestEnable = VK_TRUE;
    depth_stencil.depthWriteEnable = VK_TRUE;
    depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    depth_stencil.depthBoundsTestEnable = VK_FALSE;
    depth_stencil.back.failOp = VK_STENCIL_OP_KEEP;
    depth_stencil.back.passOp = VK_STENCIL_OP_KEEP;
    depth_stencil.back.compareOp = VK_COMPARE_OP_ALWAYS;
    depth_stencil.stencilTestEnable = VK_FALSE;
    depth_stencil.front = depth_stencil.back;


    //happens after color returns from the fragment shader
    //METHOD:
    //Mix the old and new value to produce a final color
    //Combine the old and new value using a bitwise operation
    /* Both ways showcased below
    * if (blendEnable) {
    finalColor.rgb = (srcColorBlendFactor * newColor.rgb) <colorBlendOp> (dstColorBlendFactor * oldColor.rgb);
    finalColor.a = (srcAlphaBlendFactor * newColor.a) <alphaBlendOp> (dstAlphaBlendFactor * oldColor.a);
    }
    else {
    finalColor = newColor;
    }
    finalColor = finalColor & colorWriteMask;
     */

    VkPipelineColorBlendAttachmentState colorBlendAttachment = {0};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT
        | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    //colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    //colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    //colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
    //colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    //colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    //colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

    VkPipelineColorBlendStateCreateInfo color_blending = {0};
    color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blending.logicOpEnable = VK_FALSE;
    color_blending.logicOp = VK_LOGIC_OP_COPY;
    color_blending.attachmentCount = 1;
    color_blending.pAttachments = &colorBlendAttachment; // this thing can be a darray
    color_blending.blendConstants[0] = 0.0f; // Optional
    color_blending.blendConstants[1] = 0.0f; // Optional
    color_blending.blendConstants[2] = 0.0f; // Optional
    color_blending.blendConstants[3] = 0.0f; // Optional

    VkPipelineViewportStateCreateInfo viewport_state = {0};
    viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.viewportCount = 1;
    viewport_state.scissorCount = 1;
    //viewport_state.pNext;
    //viewport_state.flags;
    //viewport_state.pViewports; these two are not needed since we are doing dynamic viewport state
    //viewport_state.pScissors;


    //for resizing the viewport, can be used for blend constants
    VkDynamicState dynamicStates[2] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamicState = {0};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = 2;
    dynamicState.pDynamicStates = dynamicStates;

    // Attachment information for dynamic rendering
    VkPipelineRenderingCreateInfoKHR pipeline_rendering_create_info = {0};
    pipeline_rendering_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
    pipeline_rendering_create_info.colorAttachmentCount = 1;
    pipeline_rendering_create_info.pColorAttachmentFormats = &renderer->context.swapchain.surface_format.format;
    pipeline_rendering_create_info.depthAttachmentFormat = renderer->context.device.depth_format;
    pipeline_rendering_create_info.stencilAttachmentFormat = renderer->context.device.depth_format;


    VkGraphicsPipelineCreateInfo graphics_pipeline_info = {0};
    graphics_pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphics_pipeline_info.stageCount = 2;
    graphics_pipeline_info.pStages = shader_stages;
    graphics_pipeline_info.pVertexInputState = &vertex_input_state_create_info;
    graphics_pipeline_info.pInputAssemblyState = &input_assembly;
    graphics_pipeline_info.pViewportState = &viewport_state;
    graphics_pipeline_info.pRasterizationState = &rasterizer;
    graphics_pipeline_info.pMultisampleState = &multisampling;
    graphics_pipeline_info.pDepthStencilState = &depth_stencil;
    graphics_pipeline_info.pColorBlendState = &color_blending;
    graphics_pipeline_info.pDynamicState = &dynamicState;
    graphics_pipeline_info.layout = textured_shader->shader_texture_pipeline.pipeline_layout;
    graphics_pipeline_info.renderPass = 0; // this has to be null if we are doing dynamic rendering
    // graphics_pipeline_info.subpass = 0;
    // graphics_pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
    // graphics_pipeline_info.basePipelineIndex = -1;
    graphics_pipeline_info.pNext = &pipeline_rendering_create_info;


    VkResult graphics_result = vkCreateGraphicsPipelines(renderer->context.device.logical_device, VK_NULL_HANDLE, 1,
                                                         &graphics_pipeline_info, NULL,
                                                         &textured_shader->shader_texture_pipeline.handle);

    if (graphics_result != VK_SUCCESS)
    {
        FATAL("failed to create graphics pipeline!");
    }

    //TODO: replace with scratch arena
    file_read_data_free(&vert_data);
    file_read_data_free(&frag_data);
    //TODO: might want move out into the shader destroy
    vkDestroyShaderModule(renderer->context.device.logical_device, fragment_shader_module, NULL);
    vkDestroyShaderModule(renderer->context.device.logical_device, vert_shader_module, NULL);

    return true;
}


bool vulkan_mesh_bda_shader_create(renderer* renderer, vulkan_mesh_default* mesh_data)
{
    VkPushConstantRange push_constants = {0};
    push_constants.size = sizeof(PC_Mesh);
    push_constants.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    push_constants.offset = 0;

    // Pipeline layout creation
    VkDescriptorSetLayout set_layouts[2] = {
        renderer->descriptor_system->uniform_descriptors.descriptor_set_layout,
        renderer->descriptor_system->texture_descriptors.descriptor_set_layout
    };

    VkPipelineLayoutCreateInfo pipeline_layout_info = {0};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = ARRAY_SIZE(set_layouts);
    pipeline_layout_info.pSetLayouts = set_layouts;
    pipeline_layout_info.pushConstantRangeCount = 1;
    pipeline_layout_info.pPushConstantRanges = &push_constants;


    //pipeline layout is the only thing the graphics pipeline needs, the descriptor sets can be created separately
    VkResult pipeline_result = vkCreatePipelineLayout(renderer->context.device.logical_device, &pipeline_layout_info,
                                                      NULL,
                                                      &mesh_data->mesh_shader_pipeline.pipeline_layout);
    VK_CHECK(pipeline_result);
    //graphics pipeline
    file_read_data vert_data = {0};
    file_read_data frag_data = {0};

    filesystem_open_and_return_bytes("../renderer/shaders/shader_mesh_bda.vert.spv", &vert_data);
    filesystem_open_and_return_bytes("../renderer/shaders/shader_mesh_bda.frag.spv", &frag_data);
    if (vert_data.size == 0)
    {
        FATAL("Vertex shader file not loaded! Size: %llu", vert_data.size);
    }
    if (frag_data.size == 0)
    {
        FATAL("Fragment shader file not loaded! Size: %llu", frag_data.size);
    }


    VkShaderModule vert_shader_module = create_shader_module(&renderer->context, vert_data.data, vert_data.size);
    VkShaderModule fragment_shader_module = create_shader_module(&renderer->context, frag_data.data, frag_data.size);


    //create the shader stage info
    VkPipelineShaderStageCreateInfo vert_shader_stage_info = {0};
    vert_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    //vertShaderStageInfo.pNext;
    //vertShaderStageInfo.flags;
    //vertShaderStageInfo.pSpecializationInfo = nullptr;
    vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vert_shader_stage_info.module = vert_shader_module;
    vert_shader_stage_info.pName = "main";

    VkPipelineShaderStageCreateInfo frag_shader_stage_info = {0};
    frag_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    //vertShaderStageInfo.pNext;
    //vertShaderStageInfo.flags;
    //frag_ShaderStageInfo.pSpecializationInfo;
    frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    frag_shader_stage_info.module = fragment_shader_module;
    frag_shader_stage_info.pName = "main";

    VkPipelineShaderStageCreateInfo shader_stages[] = {vert_shader_stage_info, frag_shader_stage_info};

    //vertex info
    /*
 * VK_VERTEX_INPUT_RATE_VERTEX: Move to the next data entry after each vertex
 * VK_VERTEX_INPUT_RATE_INSTANCE: Move to the next data entry after each instance
 */


    /* TODO: REMOVE
    VkVertexInputAttributeDescription attribute_descriptions[2];
    attribute_descriptions[0].binding = 0;
    attribute_descriptions[0].location = 0;
    attribute_descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attribute_descriptions[0].offset = 0;

    attribute_descriptions[1].binding = 0;
    attribute_descriptions[1].location = 0;
    attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attribute_descriptions[1].offset = offsetof(vertex_3d, );
*/

    VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = {0};
    vertex_input_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_state_create_info.vertexBindingDescriptionCount = 0; // the number of binding_description
    vertex_input_state_create_info.vertexAttributeDescriptionCount = 0;
    //vertex_input_state_create_info.pNext;
    //vertex_input_state_create_info.flags;

    //The VkPipelineInputAssemblyStateCreateInfo struct describes two things: what kind of geometry will be drawn from the vertices
    //and if primitive restart should be enabled.
    VkPipelineInputAssemblyStateCreateInfo input_assembly = {0};
    input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    // input_assembly.primitiveRestartEnable = VK_FALSE;
    //pInputAssemblyState.pNext;
    //pInputAssemblyState.flags;

    VkPipelineRasterizationStateCreateInfo rasterizer = {0};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    // VK_POLYGON_MODE_LINE for wireframes, VK_POLYGON_MODE_POINT for just points, using these require gpu features
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    // rasterizer.polygonMode = VK_POLYGON_MODE_LINE;
    // rasterizer.cullMode = VK_CULL_MODE_NONE; // temp for debugging
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT; //discard back facing triangles
    // rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthClampEnable = VK_FALSE; //useful for shadow maps, turn it on but need gpu features
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.lineWidth = 1.0f;
    //MIGHT BE USEFUL FOR SHADOW MAPPING
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f;
    rasterizer.depthBiasClamp = 0.0f;
    rasterizer.depthBiasSlopeFactor = 0.0f;
    //rasterizer.pNext;
    //rasterizer.flags;

    //TODO: not in use for now, but this is where we would do our anti aliasing
    VkPipelineMultisampleStateCreateInfo multisampling = {0};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.sampleShadingEnable = VK_FALSE;
    // multisampling.minSampleShading = 1.0f; // Optional
    // multisampling.pSampleMask = 0; // Optional
    // multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    // multisampling.alphaToOneEnable = VK_FALSE; // Optional

    //TODO: uncomment when needed
    // Depth and stencil testing.
    VkPipelineDepthStencilStateCreateInfo depth_stencil = {0};
    depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth_stencil.depthTestEnable = VK_TRUE;
    depth_stencil.depthWriteEnable = VK_TRUE;
    depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    depth_stencil.depthBoundsTestEnable = VK_FALSE;
    depth_stencil.back.failOp = VK_STENCIL_OP_KEEP;
    depth_stencil.back.passOp = VK_STENCIL_OP_KEEP;
    depth_stencil.back.compareOp = VK_COMPARE_OP_ALWAYS;
    depth_stencil.stencilTestEnable = VK_FALSE;
    depth_stencil.front = depth_stencil.back;


    //happens after color returns from the fragment shader
    //METHOD:
    //Mix the old and new value to produce a final color
    //Combine the old and new value using a bitwise operation
    /* Both ways showcased below
    * if (blendEnable) {
    finalColor.rgb = (srcColorBlendFactor * newColor.rgb) <colorBlendOp> (dstColorBlendFactor * oldColor.rgb);
    finalColor.a = (srcAlphaBlendFactor * newColor.a) <alphaBlendOp> (dstAlphaBlendFactor * oldColor.a);
    }
    else {
    finalColor = newColor;
    }
    finalColor = finalColor & colorWriteMask;
     */

    VkPipelineColorBlendAttachmentState colorBlendAttachment = {0};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT
        | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    //colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    //colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    //colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
    //colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    //colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    //colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

    VkPipelineColorBlendStateCreateInfo color_blending = {0};
    color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blending.logicOpEnable = VK_FALSE;
    color_blending.logicOp = VK_LOGIC_OP_COPY;
    color_blending.attachmentCount = 1;
    color_blending.pAttachments = &colorBlendAttachment; // this thing can be a darray
    color_blending.blendConstants[0] = 0.0f; // Optional
    color_blending.blendConstants[1] = 0.0f; // Optional
    color_blending.blendConstants[2] = 0.0f; // Optional
    color_blending.blendConstants[3] = 0.0f; // Optional

    VkPipelineViewportStateCreateInfo viewport_state = {0};
    viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.viewportCount = 1;
    viewport_state.scissorCount = 1;
    //viewport_state.pNext;
    //viewport_state.flags;
    //viewport_state.pViewports; these two are not needed since we are doing dynamic viewport state
    //viewport_state.pScissors;


    //for resizing the viewport, can be used for blend constants
    VkDynamicState dynamicStates[2] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamicState = {0};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = 2;
    dynamicState.pDynamicStates = dynamicStates;

    // Attachment information for dynamic rendering
    VkPipelineRenderingCreateInfoKHR pipeline_rendering_create_info = {0};
    pipeline_rendering_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
    pipeline_rendering_create_info.colorAttachmentCount = 1;
    pipeline_rendering_create_info.pColorAttachmentFormats = &renderer->context.swapchain.surface_format.format;
    pipeline_rendering_create_info.depthAttachmentFormat = renderer->context.device.depth_format;
    pipeline_rendering_create_info.stencilAttachmentFormat = renderer->context.device.depth_format;


    VkGraphicsPipelineCreateInfo graphics_pipeline_info = {0};
    graphics_pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphics_pipeline_info.stageCount = 2;
    graphics_pipeline_info.pStages = shader_stages;
    graphics_pipeline_info.pVertexInputState = &vertex_input_state_create_info;
    graphics_pipeline_info.pInputAssemblyState = &input_assembly;
    graphics_pipeline_info.pViewportState = &viewport_state;
    graphics_pipeline_info.pRasterizationState = &rasterizer;
    graphics_pipeline_info.pMultisampleState = &multisampling;
    graphics_pipeline_info.pDepthStencilState = &depth_stencil;
    graphics_pipeline_info.pColorBlendState = &color_blending;
    graphics_pipeline_info.pDynamicState = &dynamicState;
    graphics_pipeline_info.layout = mesh_data->mesh_shader_pipeline.pipeline_layout;
    graphics_pipeline_info.renderPass = 0; // this has to be null if we are doing dynamic rendering
    // graphics_pipeline_info.subpass = 0;
    // graphics_pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
    // graphics_pipeline_info.basePipelineIndex = -1;
    graphics_pipeline_info.pNext = &pipeline_rendering_create_info;


    VkResult graphics_result = vkCreateGraphicsPipelines(renderer->context.device.logical_device, VK_NULL_HANDLE, 1,
                                                         &graphics_pipeline_info, NULL,
                                                         &mesh_data->mesh_shader_pipeline.handle);

    if (graphics_result != VK_SUCCESS)
    {
        FATAL("failed to create graphics pipeline!");
    }

    //TODO: replace with scratch arena
    file_read_data_free(&vert_data);
    file_read_data_free(&frag_data);
    //TODO: might want move out into the shader destroy
    vkDestroyShaderModule(renderer->context.device.logical_device, fragment_shader_module, NULL);
    vkDestroyShaderModule(renderer->context.device.logical_device, vert_shader_module, NULL);

    return true;
}

bool vulkan_mesh_indirect_shader_create(renderer* renderer, Mesh_System* mesh_system, vulkan_shader_pipeline* mesh_indirect_pipeline)
{
    // Pipeline layout creation
    VkDescriptorSetLayout set_layouts[3] = {
        renderer->descriptor_system->uniform_descriptors.descriptor_set_layout,
        renderer->descriptor_system->texture_descriptors.descriptor_set_layout,
        renderer->descriptor_system->storage_descriptors.descriptor_set_layout,
    };

    VkPushConstantRange push_constant = {0};
    push_constant.offset = 0;
    push_constant.size = sizeof(PC_Mesh);
    push_constant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

    VkPipelineLayoutCreateInfo pipeline_layout_info = {0};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = ARRAY_SIZE(set_layouts);
    pipeline_layout_info.pSetLayouts = set_layouts;
    pipeline_layout_info.pushConstantRangeCount = 1;
    pipeline_layout_info.pPushConstantRanges = &push_constant;


    //pipeline layout is the only thing the graphics pipeline needs, the descriptor sets can be created separately
    VkResult pipeline_result = vkCreatePipelineLayout(renderer->context.device.logical_device, &pipeline_layout_info,
                                                      NULL,
                                                      &mesh_indirect_pipeline->pipeline_layout);
    VK_CHECK(pipeline_result);
    //graphics pipeline
    file_read_data vert_data = {0};
    file_read_data frag_data = {0};

    filesystem_open_and_return_bytes("../renderer/shaders/shader_mesh_indirect.vert.spv", &vert_data);
    filesystem_open_and_return_bytes("../renderer/shaders/shader_mesh_indirect.frag.spv", &frag_data);
    if (vert_data.size == 0)
    {
        FATAL("Vertex shader file not loaded! Size: %llu", vert_data.size);
    }
    if (frag_data.size == 0)
    {
        FATAL("Fragment shader file not loaded! Size: %llu", frag_data.size);
    }


    VkShaderModule vert_shader_module = create_shader_module(&renderer->context, vert_data.data, vert_data.size);
    VkShaderModule fragment_shader_module = create_shader_module(&renderer->context, frag_data.data, frag_data.size);


    //create the shader stage info
    VkPipelineShaderStageCreateInfo vert_shader_stage_info = {0};
    vert_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    //vertShaderStageInfo.pNext;
    //vertShaderStageInfo.flags;
    //vertShaderStageInfo.pSpecializationInfo = nullptr;
    vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vert_shader_stage_info.module = vert_shader_module;
    vert_shader_stage_info.pName = "main";

    VkPipelineShaderStageCreateInfo frag_shader_stage_info = {0};
    frag_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    //vertShaderStageInfo.pNext;
    //vertShaderStageInfo.flags;
    //frag_ShaderStageInfo.pSpecializationInfo;
    frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    frag_shader_stage_info.module = fragment_shader_module;
    frag_shader_stage_info.pName = "main";

    VkPipelineShaderStageCreateInfo shader_stages[] = {vert_shader_stage_info, frag_shader_stage_info};

    //vertex info
    /*
 * VK_VERTEX_INPUT_RATE_VERTEX: Move to the next data entry after each vertex
 * VK_VERTEX_INPUT_RATE_INSTANCE: Move to the next data entry after each instance
 */


    VkVertexInputBindingDescription input_binding_description[1];
    input_binding_description[0].binding = 0;
    input_binding_description[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    input_binding_description[0].stride = 12; // size of vec3


        spirv_reflect_input_variable_info* attribute_info =
        spriv_reflect_get_input_variable(NULL, "../renderer/shaders/shader_mesh_indirect.vert.spv");
    u32 offset_total = 0;
        VkVertexInputAttributeDescription* attribute_descriptions = malloc(
            sizeof(VkVertexInputAttributeDescription) * attribute_info->input_count);
        for (u32 attribute_index = 0; attribute_index < attribute_info->input_count; attribute_index++)
        {
            attribute_descriptions[attribute_index].binding = 0;
            attribute_descriptions[attribute_index].location = attribute_info->locations[attribute_index];
            attribute_descriptions[attribute_index].format = (VkFormat)attribute_info->formats[attribute_index];

            offset_total += attribute_info->offsets[attribute_index];
            attribute_descriptions[attribute_index].offset = offset_total;
        }
    /*
    */


    // VkVertexInputAttributeDescription attribute_descriptions[1];
    // attribute_descriptions[0].binding = 0;
    // attribute_descriptions[0].location = 0;
    // attribute_descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    // attribute_descriptions[0].offset = 0;


    VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = {0};
    vertex_input_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_state_create_info.pVertexBindingDescriptions = input_binding_description;
    vertex_input_state_create_info.vertexBindingDescriptionCount = 1; // the number of binding_description
    vertex_input_state_create_info.pVertexAttributeDescriptions = attribute_descriptions;
    vertex_input_state_create_info.vertexAttributeDescriptionCount = attribute_info->input_count;

    //vertex_input_state_create_info.pNext;
    //vertex_input_state_create_info.flags;

    //The VkPipelineInputAssemblyStateCreateInfo struct describes two things: what kind of geometry will be drawn from the vertices
    //and if primitive restart should be enabled.
    VkPipelineInputAssemblyStateCreateInfo input_assembly = {0};
    input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    // input_assembly.primitiveRestartEnable = VK_FALSE;
    //pInputAssemblyState.pNext;
    //pInputAssemblyState.flags;

    VkPipelineRasterizationStateCreateInfo rasterizer = {0};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    // VK_POLYGON_MODE_LINE for wireframes, VK_POLYGON_MODE_POINT for just points, using these require gpu features
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    // rasterizer.polygonMode = VK_POLYGON_MODE_LINE;
    // rasterizer.cullMode = VK_CULL_MODE_NONE; // temp for debugging
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT; //discard back facing triangles
    // rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthClampEnable = VK_FALSE; //useful for shadow maps, turn it on but need gpu features
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.lineWidth = 1.0f;
    //MIGHT BE USEFUL FOR SHADOW MAPPING
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f;
    rasterizer.depthBiasClamp = 0.0f;
    rasterizer.depthBiasSlopeFactor = 0.0f;
    //rasterizer.pNext;
    //rasterizer.flags;

    //TODO: not in use for now, but this is where we would do our anti aliasing
    VkPipelineMultisampleStateCreateInfo multisampling = {0};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.sampleShadingEnable = VK_FALSE;
    // multisampling.minSampleShading = 1.0f; // Optional
    // multisampling.pSampleMask = 0; // Optional
    // multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    // multisampling.alphaToOneEnable = VK_FALSE; // Optional

    //TODO: uncomment when needed
    // Depth and stencil testing.
    VkPipelineDepthStencilStateCreateInfo depth_stencil = {0};
    depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth_stencil.depthTestEnable = VK_TRUE;
    depth_stencil.depthWriteEnable = VK_TRUE;
    depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    depth_stencil.depthBoundsTestEnable = VK_FALSE;
    depth_stencil.back.failOp = VK_STENCIL_OP_KEEP;
    depth_stencil.back.passOp = VK_STENCIL_OP_KEEP;
    depth_stencil.back.compareOp = VK_COMPARE_OP_ALWAYS;
    depth_stencil.stencilTestEnable = VK_FALSE;
    depth_stencil.front = depth_stencil.back;


    VkPipelineColorBlendAttachmentState color_blend_attachment = {0};
    color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT
        | VK_COLOR_COMPONENT_A_BIT;
    color_blend_attachment.blendEnable = VK_FALSE;
    //colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    //colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    //colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
    //colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    //colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    //colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

    VkPipelineColorBlendStateCreateInfo color_blending = {0};
    color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blending.logicOpEnable = VK_FALSE;
    color_blending.logicOp = VK_LOGIC_OP_COPY;
    color_blending.attachmentCount = 1;
    color_blending.pAttachments = &color_blend_attachment; // this thing can be a darray
    color_blending.blendConstants[0] = 0.0f; // Optional
    color_blending.blendConstants[1] = 0.0f; // Optional
    color_blending.blendConstants[2] = 0.0f; // Optional
    color_blending.blendConstants[3] = 0.0f; // Optional

    VkPipelineViewportStateCreateInfo viewport_state = {0};
    viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.viewportCount = 1;
    viewport_state.scissorCount = 1;
    //viewport_state.pNext;
    //viewport_state.flags;
    //viewport_state.pViewports; these two are not needed since we are doing dynamic viewport state
    //viewport_state.pScissors;


    //for resizing the viewport, can be used for blend constants
    VkDynamicState dynamicStates[2] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamicState = {0};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = 2;
    dynamicState.pDynamicStates = dynamicStates;

    // Attachment information for dynamic rendering
    VkPipelineRenderingCreateInfoKHR pipeline_rendering_create_info = {0};
    pipeline_rendering_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
    pipeline_rendering_create_info.colorAttachmentCount = 1;
    pipeline_rendering_create_info.pColorAttachmentFormats = &renderer->context.swapchain.surface_format.format;
    pipeline_rendering_create_info.depthAttachmentFormat = renderer->context.device.depth_format;
    pipeline_rendering_create_info.stencilAttachmentFormat = renderer->context.device.depth_format;


    VkGraphicsPipelineCreateInfo graphics_pipeline_info = {0};
    graphics_pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphics_pipeline_info.stageCount = 2;
    graphics_pipeline_info.pStages = shader_stages;
    graphics_pipeline_info.pVertexInputState = &vertex_input_state_create_info;
    graphics_pipeline_info.pInputAssemblyState = &input_assembly;
    graphics_pipeline_info.pViewportState = &viewport_state;
    graphics_pipeline_info.pRasterizationState = &rasterizer;
    graphics_pipeline_info.pMultisampleState = &multisampling;
    graphics_pipeline_info.pDepthStencilState = &depth_stencil;
    graphics_pipeline_info.pColorBlendState = &color_blending;
    graphics_pipeline_info.pDynamicState = &dynamicState;
    graphics_pipeline_info.layout = mesh_indirect_pipeline->pipeline_layout;
    graphics_pipeline_info.renderPass = 0; // this has to be null if we are doing dynamic rendering
    // graphics_pipeline_info.subpass = 0;
    // graphics_pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
    // graphics_pipeline_info.basePipelineIndex = -1;
    graphics_pipeline_info.pNext = &pipeline_rendering_create_info;


    VkResult graphics_result = vkCreateGraphicsPipelines(renderer->context.device.logical_device, VK_NULL_HANDLE, 1,
                                                         &graphics_pipeline_info, NULL,
                                                         &mesh_indirect_pipeline->handle);

    if (graphics_result != VK_SUCCESS)
    {
        FATAL("failed to create graphics pipeline!");
    }

    //TODO: replace with scratch arena
    file_read_data_free(&vert_data);
    file_read_data_free(&frag_data);
    //TODO: might want move out into the shader destroy
    vkDestroyShaderModule(renderer->context.device.logical_device, fragment_shader_module, NULL);
    vkDestroyShaderModule(renderer->context.device.logical_device, vert_shader_module, NULL);

    return true;
}

bool ui_shader_create(renderer* renderer, vulkan_shader_pipeline* ui_pipeline)
{
    // Pipeline layout creation
    // VkDescriptorSetLayout set_layouts[3] = {
        // renderer->global_descriptors.uniform_descriptors.descriptor_set_layout,
        // renderer->global_descriptors.texture_descriptors.descriptor_set_layout,
        // renderer->global_descriptors.storage_descriptors.descriptor_set_layout,
    // };

    VkPipelineLayoutCreateInfo pipeline_layout_info = {0};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    // pipeline_layout_info.setLayoutCount = ARRAY_SIZE(set_layouts);
    // pipeline_layout_info.pSetLayouts = set_layouts;
    pipeline_layout_info.setLayoutCount = 0;
    pipeline_layout_info.pSetLayouts = NULL;
    pipeline_layout_info.pushConstantRangeCount = 0;
    pipeline_layout_info.pPushConstantRanges = NULL;


    //pipeline layout is the only thing the graphics pipeline needs, the descriptor sets can be created separately
    VkResult pipeline_result = vkCreatePipelineLayout(renderer->context.device.logical_device, &pipeline_layout_info,
                                                      NULL,
                                                      &ui_pipeline->pipeline_layout);
    VK_CHECK(pipeline_result);
    //graphics pipeline
    file_read_data vert_data = {0};
    file_read_data frag_data = {0};

    filesystem_open_and_return_bytes("../renderer/shaders/UI.vert.spv", &vert_data);
    filesystem_open_and_return_bytes("../renderer/shaders/UI.frag.spv", &frag_data);
    if (vert_data.size == 0)
    {
        FATAL("Vertex shader file not loaded! Size: %llu", vert_data.size);
    }
    if (frag_data.size == 0)
    {
        FATAL("Fragment shader file not loaded! Size: %llu", frag_data.size);
    }


    VkShaderModule vert_shader_module = create_shader_module(&renderer->context, vert_data.data, vert_data.size);
    VkShaderModule fragment_shader_module = create_shader_module(&renderer->context, frag_data.data, frag_data.size);


    //create the shader stage info
    VkPipelineShaderStageCreateInfo vert_shader_stage_info = {0};
    vert_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    //vertShaderStageInfo.pNext;
    //vertShaderStageInfo.flags;
    //vertShaderStageInfo.pSpecializationInfo = nullptr;
    vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vert_shader_stage_info.module = vert_shader_module;
    vert_shader_stage_info.pName = "main";

    VkPipelineShaderStageCreateInfo frag_shader_stage_info = {0};
    frag_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    //vertShaderStageInfo.pNext;
    //vertShaderStageInfo.flags;
    //frag_ShaderStageInfo.pSpecializationInfo;
    frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    frag_shader_stage_info.module = fragment_shader_module;
    frag_shader_stage_info.pName = "main";


    VkPipelineShaderStageCreateInfo shader_stages[] = {vert_shader_stage_info, frag_shader_stage_info};


    VkVertexInputBindingDescription input_binding_description[1];
    //vertex
    input_binding_description[0].binding = 0;
    input_binding_description[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    // input_binding_description[0].stride = sizeof(Quad_Vertex); // size of vec2
    input_binding_description[0].stride = sizeof(Quad_Vertex); // size of vec2 + vec3


    spirv_reflect_input_variable_info* attribute_info =
        spriv_reflect_get_input_variable(NULL, "../renderer/shaders/UI.vert.spv");
    u32 offset_total = 0;
    VkVertexInputAttributeDescription* attribute_descriptions = malloc(
        sizeof(VkVertexInputAttributeDescription) * attribute_info->input_count);
    for (u32 attribute_index = 0; attribute_index < attribute_info->input_count; attribute_index++)
    {
        attribute_descriptions[attribute_index].binding = 0;
        attribute_descriptions[attribute_index].location = attribute_info->locations[attribute_index];
        attribute_descriptions[attribute_index].format = (VkFormat)attribute_info->formats[attribute_index];

        offset_total += attribute_info->offsets[attribute_index];
        attribute_descriptions[attribute_index].offset = offset_total;
    }


    VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = {0};
    vertex_input_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    //vertex_input_state_create_info.pNext;
    //vertex_input_state_create_info.flags;
    vertex_input_state_create_info.vertexBindingDescriptionCount = 1;
    vertex_input_state_create_info.pVertexBindingDescriptions = input_binding_description;
    vertex_input_state_create_info.vertexAttributeDescriptionCount = attribute_info->input_count;
    vertex_input_state_create_info.pVertexAttributeDescriptions = attribute_descriptions;

    //The VkPipelineInputAssemblyStateCreateInfo struct describes two things: what kind of geometry will be drawn from the vertices
    //and if primitive restart should be enabled.
    VkPipelineInputAssemblyStateCreateInfo input_assembly = {0};
    input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_assembly.primitiveRestartEnable = VK_FALSE;
    //pInputAssemblyState.pNext;
    //pInputAssemblyState.flags;

    VkPipelineRasterizationStateCreateInfo rasterizer = {0};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT; //discard back facing triangles
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthClampEnable = VK_FALSE; //useful for shadow maps, turn it on but need gpu features
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.lineWidth = 1.0f;
    //MIGHT BE USEFUL FOR SHADOW MAPPING
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f;
    rasterizer.depthBiasClamp = 0.0f;
    rasterizer.depthBiasSlopeFactor = 0.0f;
    //rasterizer.pNext;
    //rasterizer.flags;

    //not in use for now, but this is where we would do our anti aliasing
    VkPipelineMultisampleStateCreateInfo multisampling = {0};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.sampleShadingEnable = VK_FALSE;
    //multisampling.minSampleShading = 1.0f; // Optional
    //multisampling.pSampleMask = nullptr; // Optional
    //multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    //multisampling.alphaToOneEnable = VK_FALSE; // Optional

    //TODO: uncomment when needed
    // Depth and stencil testing.
    VkPipelineDepthStencilStateCreateInfo depth_stencil = {0};
    depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth_stencil.depthTestEnable = VK_TRUE;
    depth_stencil.depthWriteEnable = VK_TRUE;
    depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    depth_stencil.depthBoundsTestEnable = VK_FALSE;
    depth_stencil.back.failOp = VK_STENCIL_OP_KEEP;
    depth_stencil.back.passOp = VK_STENCIL_OP_KEEP;
    depth_stencil.back.compareOp = VK_COMPARE_OP_ALWAYS;
    depth_stencil.stencilTestEnable = VK_FALSE;
    depth_stencil.front = depth_stencil.back;


    //happens after color returns from the fragment shader
    //METHOD:
    //Mix the old and new value to produce a final color
    //Combine the old and new value using a bitwise operation
    /* Both ways showcased below
    * if (blendEnable) {
    finalColor.rgb = (srcColorBlendFactor * newColor.rgb) <colorBlendOp> (dstColorBlendFactor * oldColor.rgb);
    finalColor.a = (srcAlphaBlendFactor * newColor.a) <alphaBlendOp> (dstAlphaBlendFactor * oldColor.a);
    }
    else {
    finalColor = newColor;
    }
    finalColor = finalColor & colorWriteMask;
     */

    //TODO: I should look more into this later, its kinda like photoshop blend modes
    // the most important is the src and dst
    VkPipelineColorBlendAttachmentState colorBlendAttachment = {0};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT
        | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    //colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    //colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    //colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
    //colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    //colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    //colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

    VkPipelineColorBlendStateCreateInfo color_blending = {0};
    color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blending.logicOpEnable = VK_FALSE;
    color_blending.logicOp = VK_LOGIC_OP_COPY;
    color_blending.attachmentCount = 1;
    color_blending.pAttachments = &colorBlendAttachment; // this thing can be a vector
    color_blending.blendConstants[0] = 0.0f; // Optional
    color_blending.blendConstants[1] = 0.0f; // Optional
    color_blending.blendConstants[2] = 0.0f; // Optional
    color_blending.blendConstants[3] = 0.0f; // Optional


    VkPipelineViewportStateCreateInfo viewport_state = {0};
    viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    //viewport_state.pNext;
    //viewport_state.flags;
    //viewport_state.pViewports; these two are not needed since we are doing dynamic viewport state
    //viewport_state.pScissors;
    viewport_state.viewportCount = 1;
    viewport_state.scissorCount = 1;

    //for resizing the viewport, can be used for blend constants
    VkDynamicState dynamicStates[2] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamicState = {0};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = 2;
    dynamicState.pDynamicStates = dynamicStates;

    VkPipelineRenderingCreateInfoKHR pipeline_rendering_create_info = {0};
    pipeline_rendering_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
    pipeline_rendering_create_info.colorAttachmentCount = 1;
    pipeline_rendering_create_info.pColorAttachmentFormats = &renderer->context.swapchain.surface_format.format;
    pipeline_rendering_create_info.depthAttachmentFormat = renderer->context.device.depth_format;
    pipeline_rendering_create_info.stencilAttachmentFormat = renderer->context.device.depth_format;


    //used to send info to the vertex/fragment shader, like in uniform buffers, to change shader behavior

    VkGraphicsPipelineCreateInfo graphics_pipeline_info = {0};
    graphics_pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphics_pipeline_info.stageCount = 2;
    graphics_pipeline_info.pStages = shader_stages;
    graphics_pipeline_info.pVertexInputState = &vertex_input_state_create_info;
    graphics_pipeline_info.pInputAssemblyState = &input_assembly;
    graphics_pipeline_info.pViewportState = &viewport_state;
    graphics_pipeline_info.pRasterizationState = &rasterizer;
    graphics_pipeline_info.pMultisampleState = &multisampling;
    graphics_pipeline_info.pDepthStencilState =  &depth_stencil; //might not need
    graphics_pipeline_info.pColorBlendState = &color_blending;
    graphics_pipeline_info.pDynamicState = &dynamicState;
    graphics_pipeline_info.layout = ui_pipeline->pipeline_layout;
    graphics_pipeline_info.renderPass = 0;
    graphics_pipeline_info.subpass = 0;
    graphics_pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
    graphics_pipeline_info.basePipelineIndex = -1;
    graphics_pipeline_info.pNext = &pipeline_rendering_create_info;


    VkResult graphics_result = vkCreateGraphicsPipelines(renderer->context.device.logical_device, VK_NULL_HANDLE, 1,
                                                         &graphics_pipeline_info, NULL,
                                                         &ui_pipeline->handle);
    if (graphics_result != VK_SUCCESS)
    {
        FATAL("failed to create graphics pipeline!");
    }

    //TODO: replace with scratch arena
    file_read_data_free(&vert_data);
    file_read_data_free(&frag_data);
    //TODO: might want move out into the shader destroy
    vkDestroyShaderModule(renderer->context.device.logical_device, fragment_shader_module, NULL);
    vkDestroyShaderModule(renderer->context.device.logical_device, vert_shader_module, NULL);

    return true;

    //TODO: replace with scratch arena
    file_read_data_free(&vert_data);
    file_read_data_free(&frag_data);
    //TODO: might want move out into the shader destroy
    vkDestroyShaderModule(renderer->context.device.logical_device, fragment_shader_module, NULL);
    vkDestroyShaderModule(renderer->context.device.logical_device, vert_shader_module, NULL);
    return true;
}


bool text_shader_create(renderer* renderer, vulkan_shader_pipeline* text_pipeline)
{
    // Pipeline layout creation
    VkDescriptorSetLayout set_layouts[3] = {
        renderer->descriptor_system->uniform_descriptors.descriptor_set_layout,
        renderer->descriptor_system->texture_descriptors.descriptor_set_layout,
        renderer->descriptor_system->storage_descriptors.descriptor_set_layout,
    };

    VkPipelineLayoutCreateInfo pipeline_layout_info = {0};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = ARRAY_SIZE(set_layouts);
    pipeline_layout_info.pSetLayouts = set_layouts;
    pipeline_layout_info.pushConstantRangeCount = 0;
    pipeline_layout_info.pPushConstantRanges = NULL;


    //pipeline layout is the only thing the graphics pipeline needs, the descriptor sets can be created separately
    VkResult pipeline_result = vkCreatePipelineLayout(renderer->context.device.logical_device, &pipeline_layout_info,
                                                      NULL,
                                                      &text_pipeline->pipeline_layout);
    VK_CHECK(pipeline_result);
    //graphics pipeline
    file_read_data vert_data = {0};
    file_read_data frag_data = {0};

    filesystem_open_and_return_bytes("../renderer/shaders/text.vert.spv", &vert_data);
    filesystem_open_and_return_bytes("../renderer/shaders/text.frag.spv", &frag_data);
    if (vert_data.size == 0)
    {
        FATAL("Vertex shader file not loaded! Size: %llu", vert_data.size);
    }
    if (frag_data.size == 0)
    {
        FATAL("Fragment shader file not loaded! Size: %llu", frag_data.size);
    }


    VkShaderModule vert_shader_module = create_shader_module(&renderer->context, vert_data.data, vert_data.size);
    VkShaderModule fragment_shader_module = create_shader_module(&renderer->context, frag_data.data, frag_data.size);

    //create the shader stage info
    VkPipelineShaderStageCreateInfo vert_shader_stage_info = {0};
    vert_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vert_shader_stage_info.module = vert_shader_module;
    vert_shader_stage_info.pName = "main";
    //vertShaderStageInfo.pNext;
    //vertShaderStageInfo.flags;
    //vertShaderStageInfo.pSpecializationInfo = nullptr;

    VkPipelineShaderStageCreateInfo frag_shader_stage_info = {0};
    frag_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    frag_shader_stage_info.module = fragment_shader_module;
    frag_shader_stage_info.pName = "main";
    //vertShaderStageInfo.pNext;
    //vertShaderStageInfo.flags;
    //frag_ShaderStageInfo.pSpecializationInfo;


    VkPipelineShaderStageCreateInfo shaderStages[] = {vert_shader_stage_info, frag_shader_stage_info};

    VkVertexInputBindingDescription binding_description[1] = {0};
    binding_description[0].binding = 0;
    // binding_description.stride = sizeof(Vertex_Text);
    binding_description[0].stride = 12 + 16 + 12;
    binding_description[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    spirv_reflect_input_variable_info* attribute_info =
        spriv_reflect_get_input_variable(NULL, "../renderer/shaders/text.vert.spv");
    u32 offset_total = 0;
    VkVertexInputAttributeDescription* attribute_descriptions = malloc(
        sizeof(VkVertexInputAttributeDescription) * attribute_info->input_count);
    for (u32 attribute_index = 0; attribute_index < attribute_info->input_count; attribute_index++)
    {
        attribute_descriptions[attribute_index].binding = 0;
        attribute_descriptions[attribute_index].location = attribute_info->locations[attribute_index];
        attribute_descriptions[attribute_index].format = (VkFormat)attribute_info->formats[attribute_index];

        offset_total += attribute_info->offsets[attribute_index];
        attribute_descriptions[attribute_index].offset = offset_total;
    }


    VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = {0};
    vertex_input_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    //vertex_input_state_create_info.pNext;
    //vertex_input_state_create_info.flags;
    vertex_input_state_create_info.vertexBindingDescriptionCount = 1; // the number of pvertexbinding descriptions
    vertex_input_state_create_info.pVertexBindingDescriptions = binding_description;
    vertex_input_state_create_info.vertexAttributeDescriptionCount = attribute_info->input_count;
    vertex_input_state_create_info.pVertexAttributeDescriptions = attribute_descriptions;

    //The VkPipelineInputAssemblyStateCreateInfo struct describes two things: what kind of geometry will be drawn from the vertices
    //and if primitive restart should be enabled.
    VkPipelineInputAssemblyStateCreateInfo input_assembly = {0};
    input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_assembly.primitiveRestartEnable = VK_FALSE;
    //pInputAssemblyState.pNext;
    //pInputAssemblyState.flags;

    VkPipelineRasterizationStateCreateInfo rasterizer = {0};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT; //discard back facing triangles
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthClampEnable = VK_FALSE; //useful for shadow maps, turn it on but need gpu features
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.lineWidth = 1.0f;
    //MIGHT BE USEFUL FOR SHADOW MAPPING
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f;
    rasterizer.depthBiasClamp = 0.0f;
    rasterizer.depthBiasSlopeFactor = 0.0f;
    //rasterizer.pNext;
    //rasterizer.flags;


    //not in use for now, but this is where we would do our anti aliasing
    VkPipelineMultisampleStateCreateInfo multisampling = {0};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    //multisampling.minSampleShading = 1.0f; // Optional
    //multisampling.pSampleMask = nullptr; // Optional
    //multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    //multisampling.alphaToOneEnable = VK_FALSE; // Optional

    VkPipelineDepthStencilStateCreateInfo depth_stencil = {0};
    depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth_stencil.depthTestEnable = VK_TRUE;
    depth_stencil.depthWriteEnable = VK_TRUE;
    depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    depth_stencil.depthBoundsTestEnable = VK_FALSE;
    depth_stencil.back.failOp = VK_STENCIL_OP_KEEP;
    depth_stencil.back.passOp = VK_STENCIL_OP_KEEP;
    depth_stencil.back.compareOp = VK_COMPARE_OP_ALWAYS;
    depth_stencil.stencilTestEnable = VK_FALSE;
    depth_stencil.front = depth_stencil.back;


    //NOTE: it's kinda like photoshop blend modes
    //blending happens before its passed to the fragment shader
    //When we consider blending two colors, we can call the color already in place the destination
    //and the new color we want to blend with it the source.

    //Enable alpha blending for text rendering

    //best to think about blending with this function:
    // blend(source,destination)=(source⋅sourceBlendFactor)blendFunction(dest⋅destinationBlendFactor)

    //classic “alpha blending” (also called “source-over” compositing).
    VkPipelineColorBlendAttachmentState color_blend_attachment = {0};
    color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    color_blend_attachment.blendEnable = VK_TRUE; // ENABLE BLENDING

    /*
    // Standard alpha blending: src_alpha * src_color + (1 - src_alpha) * dst_color
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
    */

    // Premultiplied alpha blending, will make the text less blurrier in motion
    color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
    // A_out = A_src + (1 - A_src) * A_dst
    color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo color_blending = {0};
    color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blending.logicOpEnable = VK_FALSE;
    color_blending.logicOp = VK_LOGIC_OP_COPY;
    color_blending.attachmentCount = 1;
    color_blending.pAttachments = &color_blend_attachment; // this thing can be a darray
    color_blending.blendConstants[0] = 0.0f; // Optional
    color_blending.blendConstants[1] = 0.0f; // Optional
    color_blending.blendConstants[2] = 0.0f; // Optional
    color_blending.blendConstants[3] = 0.0f; // Optional


    VkPipelineViewportStateCreateInfo viewport_state = {0};
    viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.viewportCount = 1;
    viewport_state.scissorCount = 1;
    //viewport_state.pNext;
    //viewport_state.flags;
    //viewport_state.pViewports; these two are not needed since we are doing dynamic viewport state
    //viewport_state.pScissors;


    //for resizing the viewport, can be used for blend constants
    VkDynamicState dynamicStates[2] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamicState = {0};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = 2;
    dynamicState.pDynamicStates = dynamicStates;

    // Attachment information for dynamic rendering
    VkPipelineRenderingCreateInfoKHR pipeline_rendering_create_info = {0};
    pipeline_rendering_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
    pipeline_rendering_create_info.colorAttachmentCount = 1;
    pipeline_rendering_create_info.pColorAttachmentFormats = &renderer->context.swapchain.surface_format.format;
    pipeline_rendering_create_info.depthAttachmentFormat = renderer->context.device.depth_format;
    pipeline_rendering_create_info.stencilAttachmentFormat = renderer->context.device.depth_format;


    VkGraphicsPipelineCreateInfo graphics_pipeline_info = {0};
    graphics_pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphics_pipeline_info.stageCount = 2;
    graphics_pipeline_info.pStages = shaderStages;
    graphics_pipeline_info.pVertexInputState = &vertex_input_state_create_info;
    graphics_pipeline_info.pInputAssemblyState = &input_assembly;
    graphics_pipeline_info.pViewportState = &viewport_state;
    graphics_pipeline_info.pRasterizationState = &rasterizer;
    graphics_pipeline_info.pMultisampleState = &multisampling;
    graphics_pipeline_info.pDepthStencilState = &depth_stencil; // technically dont need
    graphics_pipeline_info.pColorBlendState = &color_blending;
    graphics_pipeline_info.pDynamicState = &dynamicState;
    graphics_pipeline_info.layout = text_pipeline->pipeline_layout;
    graphics_pipeline_info.renderPass = 0;
    graphics_pipeline_info.subpass = 0;
    graphics_pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
    graphics_pipeline_info.basePipelineIndex = -1;
    graphics_pipeline_info.pNext = &pipeline_rendering_create_info;


    VkResult graphics_result = vkCreateGraphicsPipelines(renderer->context.device.logical_device, VK_NULL_HANDLE, 1,
                                                         &graphics_pipeline_info, NULL,
                                                         &text_pipeline->handle);

    if (graphics_result != VK_SUCCESS)
    {
        FATAL("failed to create graphics pipeline!");
    }

    //TODO: replace with scratch arena
    file_read_data_free(&vert_data);
    file_read_data_free(&frag_data);
    //TODO: might want move out into the shader destroy
    vkDestroyShaderModule(renderer->context.device.logical_device, fragment_shader_module, NULL);
    vkDestroyShaderModule(renderer->context.device.logical_device, vert_shader_module, NULL);

    return true;
}
