#include "debug_draw.h"

#include "vk_buffer.h"
#include "vulkan_struct_types.h"
#include "vk_command_buffer.h"
#include "vk_shader.h"


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



void debug_system_upload_frame_data(Renderer* renderer)
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

    //TODO: create a bool flag for this on the rneder, and a ui for the renderer options in general
    //generally want to do this so that we can see our directions


    if (dynamic_array_is_empty(debug_draw_system.debug_lines))
    {
        return;
    }

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

}

void debug_system_draw(Renderer* renderer)
{
    if (dynamic_array_is_empty(debug_draw_system.debug_lines))
    {
        return;
    }

    Vulkan_Command_Buffer* graphics_command_buffer = NULL;
    vulkan_queue_system_get_graphics_command_buffer(renderer, &graphics_command_buffer);
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
