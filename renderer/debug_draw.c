#include "debug_draw.h"

#include "vk_buffer.h"
#include "vulkan_struct_types.h"
#include "vk_command_buffer.h"
#include "vk_pipeline.h"


void debug_draw_system_init(Renderer* renderer, Memory_System* memory_system)
{
    debug_draw_system.debug_lines = dynamic_array_create(Debug_Line, DEFAULT_DEBUG_DRAW_COUNT,
                                                         &memory_system->application_allocator);

    debug_system_create_vulkan_shader(renderer, &debug_draw_system.debug_line_pipeline);


    debug_draw_system.debug_ssbo = vulkan_buffer_create_frame(renderer, renderer->buffer_system,
                                                              BUFFER_TYPE_STORAGE_GPU,
                                                              DEFAULT_DEBUG_DRAW_COUNT * sizeof(Debug_Line));
}

void debug_draw_system_deinit()
{
    dynamic_array_free(debug_draw_system.debug_lines);
}


vec3s get_perpendicular(const vec3s v)
{
    // Find a vector not parallel to v, then cross to get a perpendicular
    vec3s candidate = (abs(v.x) < 0.9f) ? (vec3s){1, 0, 0} : (vec3s){0, 1, 0};
    return glms_normalize(glms_cross(v, candidate));
}


void debug_draw_line(vec3s start, vec3s end, vec4s color)
{
    Debug_Line line = {
        .start = start,
        .end = end,
        .color = color
    };
    dynamic_array_push(debug_draw_system.debug_lines, &line);
}

void debug_draw_circle(const vec3s point, float radius,
                       const vec3s x_axis, const vec3s y_axis,
                       const vec4s color, int segments)
{
    vec3s initial_offset = glms_vec3_scale(x_axis, radius);
    vec3s previous_point = glms_vec3_add(point, initial_offset);

    for (int i = 1; i <= segments; ++i)
    {
        float angle = (CGLM_PI *2) * i / segments;

        vec3s x_component = glms_vec3_scale(x_axis, cos(angle));
        vec3s y_component = glms_vec3_scale(y_axis, sin(angle));
        vec3s circle_direction = glms_vec3_add(x_component, y_component);
        vec3s circle_offset = glms_vec3_scale(circle_direction, radius);
        vec3s current_point = glms_vec3_add(circle_offset, point);

        debug_draw_line(previous_point, current_point, color);
        previous_point = current_point;
    }
}

void debug_draw_hemisphere(const vec3s center, float radius, const vec3s axis,
                           const vec3s perp, const vec4s color, int segments)
{
    vec3s cross = glms_cross(axis, perp);
    for (int ring = 0; ring < segments / 4; ++ring)
    {
        //NOTE: this is pi/2
        float phi0 = CGLM_PI_2 * ring / (segments / 4);
        float phi1 = CGLM_PI_2 * (ring + 1) / (segments / 4);
        float r0 = radius * cos(phi0);
        float h0 = radius * sin(phi0);
        float r1 = radius * cos(phi1);
        float h1 = radius * sin(phi1);


        vec3s ring_center_offset = glms_vec3_scale(axis, h0);
        vec3s ring_center = glms_vec3_add(center, ring_center_offset);

        debug_draw_circle(ring_center, r0, perp, cross, color, segments);


        for (int j = 0; j < segments; ++j)
        {
            float angle = (CGLM_PI *2) * j / segments;

            vec3s perp_component = glms_vec3_scale(perp, cos(angle));
            vec3s circle_basis_component = glms_vec3_scale(cross, sin(angle));
            vec3s radial_direction = glms_vec3_add(perp_component, circle_basis_component);


            vec3s ring_offset0 = glms_vec3_scale(radial_direction, r0);
            vec3s ring_offset1 = glms_vec3_scale(radial_direction, r1);
            vec3s axis_offset_vector0 = glms_vec3_scale(axis, h0);
            vec3s axis_offset_vector1 = glms_vec3_scale(axis, h1);


            vec3s point0 = glms_vec3_add(glms_vec3_add(center, axis_offset_vector0), ring_offset0);
            vec3s point1 = glms_vec3_add(glms_vec3_add(center, axis_offset_vector1), ring_offset1);
            debug_draw_line(point0, point1, color);
        }
    }
}


void debug_draw_sphere(vec3s point, float radius, vec4s color)
{
    //could pass as a param but kinda not needed
    u32 segments = 16;
    // Draw three orthogonal circles to represent a sphere
    debug_draw_circle(point, radius, (vec3s){1, 0, 0}, (vec3s){0, 1, 0}, color, segments);
    debug_draw_circle(point, radius, (vec3s){1, 0, 0}, (vec3s){0, 0, 1}, color, segments);
    debug_draw_circle(point, radius, (vec3s){0, 1, 0}, (vec3s){0, 0, 1}, color, segments);
}

void debug_draw_bounds(const vec3s center, const vec3s half_extents,
                       const vec4s color)
{
    const vec3s e = half_extents;

    vec3s corners[8] = {
        {center.x - e.x, center.y - e.y, center.z - e.z},
        {center.x + e.x, center.y - e.y, center.z - e.z},
        {center.x + e.x, center.y + e.y, center.z - e.z},
        {center.x - e.x, center.y + e.y, center.z - e.z},

        {center.x - e.x, center.y - e.y, center.z + e.z},
        {center.x + e.x, center.y - e.y, center.z + e.z},
        {center.x + e.x, center.y + e.y, center.z + e.z},
        {center.x - e.x, center.y + e.y, center.z + e.z}
    };

    static const u32 edges[12][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0},
        {4, 5}, {5, 6}, {6, 7}, {7, 4},
        {0, 4}, {1, 5}, {2, 6}, {3, 7}
    };

    for (u32 i = 0; i < 12; ++i)
    {
        debug_draw_line(
            corners[edges[i][0]],
            corners[edges[i][1]],
            color
        );
    }
}

void debug_draw_box(vec3s center, const float size, const vec4s color)
{
    vec3s extents = {size, size, size};
    debug_draw_bounds(center, extents, color);
}

void debug_draw_capsule(const vec3s base, const vec3s tip,
                        float radius, const vec4s color)
{
    const int segments = 12;

    vec3s axis_vector = glms_vec3_sub(tip, base);
    float axis_length = glms_vec3_norm(axis_vector);


    if (axis_length < 1e-6f)
    {
        debug_draw_sphere(base, radius, color);
        return;
    }

    vec3s axis_direction = glms_vec3_scale(axis_vector, 1.0f / axis_length);

    vec3s perpendicular = get_perpendicular(axis_direction);

    vec3s circle_basis = glms_cross(axis_direction, perpendicular);

    /*
     * Draw the circular boundaries where the cylindrical body
     * meets the hemispherical caps.
     */
    debug_draw_circle(base, radius, perpendicular, circle_basis, color, segments);

    debug_draw_circle(tip, radius, perpendicular, circle_basis, color, segments);

    /*
     * Draw the four longitudinal edges of the cylindrical body.
     */
    for (int i = 0; i < 4; ++i)
    {
        float angle = glm_rad(90.0f * i);

        vec3s perpendicular_component = glms_vec3_scale(perpendicular, cosf(angle));
        vec3s circle_basis_component = glms_vec3_scale(circle_basis, sinf(angle));
        vec3s radial_direction = glms_vec3_add(perpendicular_component, circle_basis_component);
        vec3s radial_offset = glms_vec3_scale(radial_direction, radius);

        vec3s base_point = glms_vec3_add(base, radial_offset);

        vec3s tip_point = glms_vec3_add(tip, radial_offset);

        debug_draw_line(base_point, tip_point, color);
    }

    /*
     * Draw the hemispherical caps.
     */
    debug_draw_hemisphere(base, radius, glms_vec3_negate(axis_direction), perpendicular, color, segments);

    debug_draw_hemisphere(tip, radius, axis_direction, perpendicular, color, segments);
}

void debug_system_create_vulkan_shader(Renderer* renderer, Vulkan_Shader_Pipeline* out_pipeline)
{
    const char* shader_name = "debug_line";
    // Pipeline layout creation
    VkDescriptorSetLayout set_layouts[3] = {
        renderer->descriptor_system->uniform_descriptors.descriptor_set_layout,
        renderer->descriptor_system->texture_descriptors.descriptor_set_layout,
        renderer->descriptor_system->storage_descriptors.descriptor_set_layout, // TODO: get rid of this
    };

    VkPushConstantRange push_constant = {0};
    push_constant.offset = 0;
    push_constant.size = sizeof(PC_Debug_Line);
    push_constant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

    VkPipelineLayoutCreateInfo pipeline_layout_info = {0};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = ARRAY_SIZE(set_layouts);
    pipeline_layout_info.pSetLayouts = set_layouts;
    pipeline_layout_info.pushConstantRangeCount = 1;
    pipeline_layout_info.pPushConstantRanges = &push_constant;


    //pipeline layout is the only thing the graphics pipeline needs, the descriptor sets can be created separately
    VkResult pipeline_result = vkCreatePipelineLayout(renderer->logical_device, &pipeline_layout_info,
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


    VkShaderModule vert_shader_module = create_shader_module(renderer, vert_data.data, vert_data.size);
    VkShaderModule fragment_shader_module = create_shader_module(renderer, frag_data.data, frag_data.size);


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
    input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;// for wireframe
    // input_assembly.primitiveRestartEnable = VK_FALSE;
    //pInputAssemblyState.pNext;
    //pInputAssemblyState.flags;


    VkPipelineRasterizationStateCreateInfo rasterizer = {0};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    // VK_POLYGON_MODE_LINE for wireframes, VK_POLYGON_MODE_POINT for just points, using these require gpu features
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.cullMode = VK_CULL_MODE_NONE; // we dont want anything culled
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthClampEnable = VK_FALSE; //useful for shadow maps, turn it on but need gpu features
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.lineWidth = 1.0f;
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

    // Depth and stencil testing.
    VkPipelineDepthStencilStateCreateInfo depth_stencil = {0};
    depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth_stencil.depthTestEnable = VK_FALSE;
    depth_stencil.depthWriteEnable = VK_FALSE;

    //TODO: blend types for the passed in blend type
    VkPipelineColorBlendAttachmentState color_blend_attachment = {0};
    color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    color_blend_attachment.blendEnable = VK_FALSE;


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
    //TODO: maybe use this for line width if supported
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
    pipeline_rendering_create_info.pColorAttachmentFormats = &renderer->swapchain.surface_format.format;
    pipeline_rendering_create_info.depthAttachmentFormat = renderer->depth_format;
    pipeline_rendering_create_info.stencilAttachmentFormat = renderer->depth_format;


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


    VkResult graphics_result = vkCreateGraphicsPipelines(renderer->logical_device,
                                                         renderer->pipeline_cache->handle, 1,
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
    vkDestroyShaderModule(renderer->logical_device, fragment_shader_module, NULL);
    vkDestroyShaderModule(renderer->logical_device, vert_shader_module, NULL);
}

void debug_system_upload_and_draw(Renderer* renderer)
{

    //debug info
    /*for (u32 i = 0; i < debug_draw_system.debug_lines->num_items; ++i)
    {
        Debug_Line line = dynamic_array_get(debug_draw_system.debug_lines, Debug_Line, i);

        DEBUG(
            "line %u: start=(%f,%f,%f) end=(%f,%f,%f) color=(%f,%f,%f,%f)\n",
            i,
            line.start.x, line.start.y, line.start.z,
            line.end.x, line.end.y, line.end.z,
            line.color.x, line.color.y, line.color.z, line.color.w
        );
    }*/

    //frame data upload
    vulkan_buffer_get_frame(renderer, debug_draw_system.debug_ssbo);

    Vulkan_Command_Buffer* graphics_command_buffer = NULL;
    vulkan_queue_system_get_graphics_command_buffer(renderer, &graphics_command_buffer);

    vulkan_buffer_frame_staging_upload(renderer, debug_draw_system.debug_ssbo, graphics_command_buffer, debug_draw_system.debug_lines->data, dynamic_array_get_byte_size(debug_draw_system.debug_lines));

    Vulkan_Buffer* ssbo = vulkan_buffer_get_frame(renderer, debug_draw_system.debug_ssbo);


    VkBufferMemoryBarrier2 buffer_barrier = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,

        //barrier so that we finish our transfer before our shaders read the data
        .srcStageMask = VK_PIPELINE_STAGE_2_COPY_BIT,
        .srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,

        .dstStageMask = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT, // only needed in the vertex shader
        .dstAccessMask = VK_ACCESS_2_SHADER_STORAGE_READ_BIT,

        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,

        .buffer = ssbo->handle,
        .offset = 0,
        .size = ssbo->current_offset,
    };
    vulkan_command_add_submit_buffer_barrier(graphics_command_buffer, buffer_barrier);


    //draw
    vkCmdBindPipeline(graphics_command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS, debug_draw_system.debug_line_pipeline.handle);


    //uniform
    vkCmdBindDescriptorSets(graphics_command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            debug_draw_system.debug_line_pipeline.pipeline_layout, 0, 1,
                            &renderer->descriptor_system->uniform_descriptors.descriptor_sets[renderer->
                                current_frame], 0, 0);
    //texturess
    vkCmdBindDescriptorSets(graphics_command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            debug_draw_system.debug_line_pipeline.pipeline_layout, 1, 1,
                            &renderer->descriptor_system->texture_descriptors.descriptor_sets[0], 0, 0);

    //storage buffers
    vkCmdBindDescriptorSets(graphics_command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            debug_draw_system.debug_line_pipeline.pipeline_layout, 2, 1,
                            &renderer->descriptor_system->storage_descriptors.descriptor_sets[renderer->
                                current_frame], 0, 0);



    PC_Debug_Line pc = {
        .debug_line_ssbo = vulkan_buffer_get_frame_device_address(renderer, debug_draw_system.debug_ssbo),
        .padding0 = 0,
        .padding1 = 0,
    };


    VkPushConstantsInfo push_constant_info = {0};
    push_constant_info.sType = VK_STRUCTURE_TYPE_PUSH_CONSTANTS_INFO;
    push_constant_info.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    push_constant_info.layout = debug_draw_system.debug_line_pipeline.pipeline_layout;
    push_constant_info.offset = 0;
    push_constant_info.size = sizeof(PC_Debug_Line);
    push_constant_info.pValues = &pc;
    push_constant_info.pNext = NULL;
    vkCmdPushConstants2(graphics_command_buffer->handle, &push_constant_info);


    vkCmdDraw(graphics_command_buffer->handle,  debug_draw_system.debug_lines->num_items * 2, 1, 0, 0);


    dynamic_array_clear(debug_draw_system.debug_lines);

}
