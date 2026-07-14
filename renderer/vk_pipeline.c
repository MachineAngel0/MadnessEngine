#include "vk_pipeline.h"


bool vulkan_pipeline_graphics_create(Renderer* renderer, const char* shader_name, vulkan_shader_pipeline* out_pipeline)
{
    // Pipeline layout creation
    VkDescriptorSetLayout set_layouts[3] = {
        renderer->descriptor_system->uniform_descriptors.descriptor_set_layout,
        renderer->descriptor_system->texture_descriptors.descriptor_set_layout,
        renderer->descriptor_system->storage_descriptors.descriptor_set_layout, // TODO: get rid of this
    };

    VkPushConstantRange push_constant = {0};
    push_constant.offset = 0;
    push_constant.size = sizeof(PC_General);
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
                                                      &out_pipeline->pipeline_layout);
    VK_CHECK(pipeline_result);
    //graphics pipeline
    file_read_data vert_data = {0};
    file_read_data frag_data = {0};

    String_Builder* vert_path_builder = string_builder_create(1024, &renderer->frame_allocator);
    string_builder_append_c_string(vert_path_builder, SHADER_PATH);
    string_builder_append_c_string(vert_path_builder, shader_name);
    string_builder_append_c_string(vert_path_builder, ".vert.spv");

    String_Builder* frag_path_builder = string_builder_create(1024, &renderer->frame_allocator);
    string_builder_append_c_string(frag_path_builder, SHADER_PATH);
    string_builder_append_c_string(frag_path_builder, shader_name);
    string_builder_append_c_string(frag_path_builder, ".frag.spv");

    const char* vert_shader_file = string_builder_to_c_string(vert_path_builder);
    const char* frag_shader_file = string_builder_to_c_string(frag_path_builder);

    filesystem_open_and_return_bytes(vert_shader_file, &vert_data);
    filesystem_open_and_return_bytes(frag_shader_file, &frag_data);
    if (vert_data.size == 0)
    {
        FATAL("Vertex shader file not loaded! Size: %llu", vert_data.size);
        MASSERT(false);
    }
    if (frag_data.size == 0)
    {
        FATAL("Fragment shader file not loaded! Size: %llu", frag_data.size);
        MASSERT(false);
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

    //NOTE: we dont fill this out as we pass in bda pointers for anything vertex/data layout
    VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = {0};
    vertex_input_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_state_create_info.pVertexBindingDescriptions = NULL;
    vertex_input_state_create_info.vertexBindingDescriptionCount = 0; // the number of binding_description
    vertex_input_state_create_info.pVertexAttributeDescriptions = NULL;
    vertex_input_state_create_info.vertexAttributeDescriptionCount = 0;


    //The VkPipelineInputAssemblyStateCreateInfo struct describes two things: what kind of geometry will be drawn from the vertices
    //and if primitive restart should be enabled.
    VkPipelineInputAssemblyStateCreateInfo input_assembly = {0};
    input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    // input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST; // for wireframe
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
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    // rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; // NOTE: it seems everything just gets flipped for some reason
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
    graphics_pipeline_info.layout = out_pipeline->pipeline_layout;
    graphics_pipeline_info.renderPass = 0; // this has to be null if we are doing dynamic rendering
    // graphics_pipeline_info.subpass = 0;
    // graphics_pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
    // graphics_pipeline_info.basePipelineIndex = -1;
    graphics_pipeline_info.pNext = &pipeline_rendering_create_info;


    VkResult graphics_result = vkCreateGraphicsPipelines(renderer->context.device.logical_device, renderer->pipeline_cache->handle, 1,
                                                         &graphics_pipeline_info, NULL,
                                                         &out_pipeline->handle);

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

void vulkan_pipeline_destroy(vulkan_context* context, vulkan_shader_pipeline* pipeline)
{
    if (!pipeline)
    {
        M_ERROR("VK PIPELINE DESTROY: INVALID PIPELINE");
        return;
    }
    // Destroy pipeline
    if (!pipeline->handle)
    {
        M_ERROR("VK PIPELINE DESTROY: INVALID PIPELINE HANDLE");
        return;
    }
    // Destroy layout
    if (!pipeline->pipeline_layout)
    {
        M_ERROR("VK PIPELINE DESTROY: INVALID PIPELINE LAYOUT");
        return;
    }

    vkDestroyPipelineLayout(context->device.logical_device, pipeline->pipeline_layout, context->allocator);
    pipeline->pipeline_layout = 0;
    vkDestroyPipeline(context->device.logical_device, pipeline->handle, context->allocator);
    pipeline->handle = 0;
}



void vulkan_pipeline_cache_read_from_file(Renderer* renderer, vulkan_pipeline_cache* pipeline_info,
                                          u8** pipeline_cache_data, size_t* pipeline_cache_size)
{
    pipeline_cache_file_header pipeline_cache_prefix_header;
    //TODO: load data from file if any are available, and verify if the pipeline caches are the same
    FILE* fptr = fopen(pipeline_cache_file_path, "rb");
    if (!fptr)
    {
        WARN("VULKAN_PIPELINE_CACHE_READ_FROM_FILE: COULD NOT OPEN FILE")
        return;
    }

    bool is_valid_cache_data = true;
    //read in the header
    size_t read_size = fread(
        &pipeline_cache_prefix_header, 1, sizeof(pipeline_cache_file_header),
        fptr);

    if (read_size != sizeof(pipeline_cache_file_header))
    {
        fclose(fptr);
        return;
    }

    //file verification
    //valid the pipeline header and pipeline info, if everything is fine we can read in the pipeline cache data
    if (pipeline_cache_prefix_header.magic != pipeline_cache_magic_number ||
        pipeline_cache_prefix_header.data_size < 0 ||
        pipeline_cache_prefix_header.vendor_id != renderer->context.device.properties.vendorID ||
        pipeline_cache_prefix_header.device_id != renderer->context.device.properties.deviceID ||
        pipeline_cache_prefix_header.driver_version != renderer->context.device.properties.driverVersion ||
        pipeline_cache_prefix_header.driver_abi != sizeof(void*))
    {
        is_valid_cache_data = false;
    }

    if (memcmp(pipeline_cache_prefix_header.uuid, renderer->context.device.properties.pipelineCacheUUID,
               sizeof(pipeline_cache_prefix_header.uuid)) != 0)
    {
        is_valid_cache_data = false;
    }

    if (!is_valid_cache_data)
    {
        INFO("INVALID PIPELINE CACHE LOADED");
        fclose(fptr);
        return;
    }
    INFO("VALID PIPELINE CACHE LOADED");

    //read in the data
    *pipeline_cache_size = pipeline_cache_prefix_header.data_size;
    *pipeline_cache_data = allocator_alloc(&renderer->allocator, pipeline_cache_prefix_header.data_size);
    size_t read_size2 = fread(*pipeline_cache_data, 1, pipeline_cache_prefix_header.data_size, fptr);
    if (read_size2 != pipeline_cache_prefix_header.data_size)
    {
        WARN("VULKAN PIPELINE system init: didn't read correct amount of data from the file")
        pipeline_cache_size = 0;
        *pipeline_cache_data = NULL;
        pipeline_info->handle = 0;
    }

    fclose(fptr);
}

vulkan_pipeline_cache* vulkan_pipeline_cache_initialize(Renderer* renderer)
{
    vulkan_pipeline_cache* pipeline_info = allocator_alloc(&renderer->allocator, sizeof(vulkan_pipeline_cache));

    u8* pipeline_cache_data = NULL;
    size_t pipeline_cache_data_size = 0;

    //check if we have a valid pipeline
    vulkan_pipeline_cache_read_from_file(renderer, pipeline_info, &pipeline_cache_data, &pipeline_cache_data_size);


    VkPipelineCacheCreateInfo pipeline_cache_create_info = {0};
    pipeline_cache_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    //these params are for when we load already existing data from a file
    pipeline_cache_create_info.flags = 0;
    pipeline_cache_create_info.pNext = NULL;
    pipeline_cache_create_info.initialDataSize = pipeline_cache_data_size;
    pipeline_cache_create_info.pInitialData = pipeline_cache_data;

    VkResult result = vkCreatePipelineCache(
        renderer->context.device.logical_device,
        &pipeline_cache_create_info,
        renderer->context.allocator,
        &pipeline_info->handle);

    VK_CHECK(result)

    INFO("PIPELINE CACHE INITIALIZED");

    return pipeline_info;
}

void vulkan_pipeline_cache_write_to_file(Renderer* renderer, vulkan_pipeline_cache* pipeline_cache)
{
    //get the pipeline cache data, and write it out to a file

    //must be called twice, once to get the data size, and second to get the data
    size_t data_size; // in bytes
    u8* data;
    VkResult result = vkGetPipelineCacheData(renderer->context.device.logical_device, pipeline_cache->handle,
                                             &data_size,
                                             NULL);
    VK_CHECK(result)

    data = allocator_alloc(&renderer->frame_allocator, data_size);

    VkResult result2 = vkGetPipelineCacheData(renderer->context.device.logical_device, pipeline_cache->handle,
                                              &data_size,
                                              data);
    VK_CHECK(result2)


    //generate new header data, even if we do technically have a valid one if was loaded on startup
    pipeline_cache_file_header new_file_header;
    new_file_header.magic = pipeline_cache_magic_number;
    new_file_header.data_size = data_size;
    new_file_header.device_id = renderer->context.device.properties.deviceID;
    new_file_header.driver_version = renderer->context.device.properties.driverVersion;
    new_file_header.driver_abi = sizeof(void*);
    new_file_header.vendor_id = renderer->context.device.properties.vendorID;
    memcpy(new_file_header.uuid, renderer->context.device.properties.pipelineCacheUUID, sizeof(new_file_header.uuid));

    //write into the file
    FILE* fptr = fopen(pipeline_cache_file_path, "wb");
    if (!fptr)
    {
        WARN("VULKAN PIPELINE CACHE WRITE TO FILE: COULDNT OPEN FILE")
        fclose(fptr);
        return;
    }

    //write header into the file
    size_t header_data_written = fwrite(&new_file_header, 1, sizeof(new_file_header), fptr);

    //write data into the file
    while (data_size > 0)
    {
        size_t data_written = fwrite(data, 1, data_size, fptr);
        data_size -= data_written;
    }

    fclose(fptr);
    INFO("PIPELINE CACHE WRITTEN TO FILE");
}
