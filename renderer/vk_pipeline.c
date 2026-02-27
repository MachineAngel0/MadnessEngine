#include "vk_pipeline.h"


bool vulkan_graphics_pipeline_create(vulkan_context* context, vulkan_renderpass* renderpass,
                                     u32 attribute_count,
                                     VkVertexInputAttributeDescription* attributes,
                                     u32 descriptor_set_layout_count,
                                     VkDescriptorSetLayout* descriptor_set_layouts,
                                     u32 stage_count,
                                     VkPipelineShaderStageCreateInfo* stages,
                                     VkViewport viewport,
                                     VkRect2D scissor,
                                     bool is_wireframe,
                                     vulkan_shader_pipeline* out_pipeline)
{
    // Viewport state
    VkPipelineViewportStateCreateInfo viewport_state = {VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
    viewport_state.viewportCount = 1;
    viewport_state.pViewports = &viewport;
    viewport_state.scissorCount = 1;
    viewport_state.pScissors = &scissor;

    // Rasterizer
    VkPipelineRasterizationStateCreateInfo rasterizer_create_info = {
        VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO
    };
    rasterizer_create_info.depthClampEnable = VK_FALSE;
    rasterizer_create_info.rasterizerDiscardEnable = VK_FALSE;
    rasterizer_create_info.polygonMode = is_wireframe ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL;
    rasterizer_create_info.lineWidth = 1.0f;
    rasterizer_create_info.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer_create_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer_create_info.depthBiasEnable = VK_FALSE;
    rasterizer_create_info.depthBiasConstantFactor = 0.0f;
    rasterizer_create_info.depthBiasClamp = 0.0f;
    rasterizer_create_info.depthBiasSlopeFactor = 0.0f;

    // Multisampling.
    VkPipelineMultisampleStateCreateInfo multisampling_create_info = {
        VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO
    };
    multisampling_create_info.sampleShadingEnable = VK_FALSE;
    multisampling_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling_create_info.minSampleShading = 1.0f;
    multisampling_create_info.pSampleMask = 0;
    multisampling_create_info.alphaToCoverageEnable = VK_FALSE;
    multisampling_create_info.alphaToOneEnable = VK_FALSE;

    // Depth and stencil testing.
    VkPipelineDepthStencilStateCreateInfo depth_stencil = {VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};
    depth_stencil.depthTestEnable = VK_TRUE;
    depth_stencil.depthWriteEnable = VK_TRUE;
    depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depth_stencil.depthBoundsTestEnable = VK_FALSE;
    depth_stencil.stencilTestEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState color_blend_attachment_state;
    memset(&color_blend_attachment_state, 0, sizeof(VkPipelineColorBlendAttachmentState));
    color_blend_attachment_state.blendEnable = VK_TRUE;
    color_blend_attachment_state.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    color_blend_attachment_state.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    color_blend_attachment_state.colorBlendOp = VK_BLEND_OP_ADD;
    color_blend_attachment_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    color_blend_attachment_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    color_blend_attachment_state.alphaBlendOp = VK_BLEND_OP_ADD;

    color_blend_attachment_state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    VkPipelineColorBlendStateCreateInfo color_blend_state_create_info = {
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO
    };
    color_blend_state_create_info.logicOpEnable = VK_FALSE;
    color_blend_state_create_info.logicOp = VK_LOGIC_OP_COPY;
    color_blend_state_create_info.attachmentCount = 1;
    color_blend_state_create_info.pAttachments = &color_blend_attachment_state;

    // Dynamic state
    const u32 dynamic_state_count = 3;
    // VkDynamicState dynamic_states[dynamic_state_count] = {
    VkDynamicState dynamic_states[3] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
        VK_DYNAMIC_STATE_LINE_WIDTH
    };

    VkPipelineDynamicStateCreateInfo dynamic_state_create_info = {VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO};
    dynamic_state_create_info.dynamicStateCount = dynamic_state_count;
    dynamic_state_create_info.pDynamicStates = dynamic_states;

    // Vertex input
    VkVertexInputBindingDescription binding_description;
    binding_description.binding = 0; // Binding index
    binding_description.stride = sizeof(vertex_3d);
    binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; // Move to next data entry for each vertex.

    // Attributes
    VkPipelineVertexInputStateCreateInfo vertex_input_info = {
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO
    };
    vertex_input_info.vertexBindingDescriptionCount = 1;
    vertex_input_info.pVertexBindingDescriptions = &binding_description;
    vertex_input_info.vertexAttributeDescriptionCount = attribute_count;
    vertex_input_info.pVertexAttributeDescriptions = attributes;

    // Input assembly
    VkPipelineInputAssemblyStateCreateInfo input_assembly = {
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO
    };
    input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_assembly.primitiveRestartEnable = VK_FALSE;

    // Pipeline layout
    VkPipelineLayoutCreateInfo pipeline_layout_create_info = {VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};

    // Descriptor set layouts
    pipeline_layout_create_info.setLayoutCount = descriptor_set_layout_count;
    pipeline_layout_create_info.pSetLayouts = descriptor_set_layouts;

    // Create the pipeline layout.
    VK_CHECK(vkCreatePipelineLayout(
        context->device.logical_device,
        &pipeline_layout_create_info,
        context->allocator,
        &out_pipeline->pipeline_layout));

    // Pipeline create
    VkGraphicsPipelineCreateInfo pipeline_create_info = {VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
    pipeline_create_info.stageCount = stage_count;
    pipeline_create_info.pStages = stages;
    pipeline_create_info.pVertexInputState = &vertex_input_info;
    pipeline_create_info.pInputAssemblyState = &input_assembly;

    pipeline_create_info.pViewportState = &viewport_state;
    pipeline_create_info.pRasterizationState = &rasterizer_create_info;
    pipeline_create_info.pMultisampleState = &multisampling_create_info;
    pipeline_create_info.pDepthStencilState = &depth_stencil;
    pipeline_create_info.pColorBlendState = &color_blend_state_create_info;
    pipeline_create_info.pDynamicState = &dynamic_state_create_info;
    pipeline_create_info.pTessellationState = 0;

    pipeline_create_info.layout = out_pipeline->pipeline_layout;

    pipeline_create_info.renderPass = renderpass->handle;
    pipeline_create_info.subpass = 0;
    pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
    pipeline_create_info.basePipelineIndex = -1;

    VkResult result = vkCreateGraphicsPipelines(
        context->device.logical_device,
        VK_NULL_HANDLE,
        1,
        &pipeline_create_info,
        context->allocator,
        &out_pipeline->handle);


    return true;

    // if (vulkan_result_is_success(result)) {
    // KDEBUG("Graphics pipeline created!");
    // return true;
    // }

    // M_ERROR("vkCreateGraphicsPipelines failed with %s.", vulkan_result_string(result, true));
    // return false;
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

void vulkan_pipeline_bind(vulkan_command_buffer* command_buffer, VkPipelineBindPoint bind_point,
                          vulkan_shader_pipeline* pipeline)
{
    vkCmdBindPipeline(command_buffer->handle, bind_point, pipeline->handle);
}

void vulkan_pipeline_graphics_bind(vulkan_command_buffer* command_buffer,
                                   vulkan_shader_pipeline* pipeline)
{
    vkCmdBindPipeline(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->handle);
}

void vulkan_pipeline_compute_bind(vulkan_command_buffer* command_buffer, vulkan_shader_pipeline* pipeline)
{
    vkCmdBindPipeline(command_buffer->handle, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline->handle);
}

void vulkan_pipeline_cache_read_from_file(renderer* renderer, vulkan_pipeline_cache* pipeline_info,
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
    *pipeline_cache_data = arena_alloc(&renderer->arena, pipeline_cache_prefix_header.data_size);
    size_t read_size2 = fread(*pipeline_cache_data, 1, pipeline_cache_prefix_header.data_size, fptr);
    if (read_size2 != pipeline_cache_prefix_header.data_size)
    {
        WARN("VULKAN PIPELINE system init: didn't read correct amount of data from the file")
        pipeline_cache_size = 0;
        *pipeline_cache_data = NULL;
    }

    fclose(fptr);
}

vulkan_pipeline_cache* vulkan_pipeline_cache_initialize(renderer* renderer)
{
    vulkan_pipeline_cache* pipeline_info = arena_alloc(&renderer->arena, sizeof(vulkan_pipeline_cache));

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

void vulkan_pipeline_cache_write_to_file(renderer* renderer, vulkan_pipeline_cache* pipeline_cache)
{
    //get the pipeline cache data, and write it out to a file

    //must be called twice, once to get the data size, and second to get the data
    size_t data_size; // in bytes
    u8* data;
    VkResult result = vkGetPipelineCacheData(renderer->context.device.logical_device, pipeline_cache->handle,
                                             &data_size,
                                             NULL);
    VK_CHECK(result)

    data = arena_alloc(&renderer->frame_arena, data_size);

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
