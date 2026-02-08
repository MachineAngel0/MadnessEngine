//
// Created by Adams Humbert on 9/10/2025.
//

#include "text.h"


Vertex_Text* text_create_quad(vec2 pos, vec2 size, vec3 color)
{
    Vertex_Text out_vertex_text[] = {
        {{pos.x - size.x, pos.y - size.y}, color},
        {{pos.x - size.x, pos.y + size.y}, color},
        {{pos.x + size.x, pos.y + size.y}, color},
        {{pos.x + size.x, pos.y - size.y}, color}
    };

    return out_vertex_text;
}

void do_text(UI_System* ui_state, std::string text, glm::vec2 pos, glm::vec3 color, float font_size)
{
    if (pos.x > 100 || pos.x < 0 || pos.y > 100 || pos.y < 0)
    {
        printf("YOU ARE STUPID 2: TEXT");
    }

    //move to the shader as a push constant
    float screen_width = ui_state->screen_size.x;
    float screen_height = ui_state->screen_size.y;
    //std::vector<Vertex_Text> new_quad = text_create_quad({0.5f,0.5f}, {0.1f,0.1f}, {1.0f,1.0f,1.0f});


    vec2 converted_pos = pos / 100.0f;
    vec2 final_pos = {
        screen_width * converted_pos.x,
        screen_height * converted_pos.y
    };

    float font_scalar = font_size / ui_state->text_system.default_font_size;


    for (const char& c : text)
    {
        if (c < 32 || c >= 128) continue; // skip unsupported characters
        Glyph& g = ui_state->text_system.glyphs[c - 32];


        // Quad position in screen coords
        float xpos = final_pos.x + g.xoff * font_scalar;
        float ypos = final_pos.y + g.yoff * font_scalar;
        float w = (float)(g.width) * font_scalar;
        float h = (float)(g.height) * font_scalar;

        //printf("xpos %f, ypos%f, w%f, h%f\n", xpos, ypos, w, h);

        // Convert screen coords to NDC [-1,1]
        float ndc_x0 = (xpos / screen_width) * 2.0f - 1.0f;
        float ndc_x1 = ((xpos + w) / screen_width) * 2.0f - 1.0f;
        float ndc_y0 = (ypos / screen_height) * 2.0f - 1.0f; // invert Y
        float ndc_y1 = ((ypos + h) / screen_height) * 2.0f - 1.0f;

        /* here for reference
        {{pos.x - size.x, pos.y - size.y}, {color}},
        {{pos.x - size.x, pos.y + size.y}, {color}},
        {{pos.x + size.x, pos.y + size.y}, {color}},
        {{pos.x + size.x, pos.y - size.y}, {color}}
        */
        // UVs from the atlas
        vec2 uv0 = {g.u0, g.v0};
        vec2 uv1 = {g.u1, g.v1};

        //glm::vec2(xpos / text_system.push_constants.screenSize) * 2.0f - 1.0f;

        std::vector<Vertex_Text> new_quad = {
            {{ndc_x0, ndc_y0}, color, {uv0.x, uv0.y}},
            {{ndc_x0, ndc_y1}, color, {uv0.x, uv1.y}},
            {{ndc_x1, ndc_y1}, color, {uv1.x, uv1.y}},
            {{ndc_x1, ndc_y0}, color, {uv1.x, uv0.y}},
        };

        /* TODO:  fruity colors, maybe ill make a seperate function that allows you to color each edge, cause its kinda cool, realistically you can just overlay another texture but that's fucking lame
        std::vector<Vertex_Text> new_quad = {
            {{ndc_x0, ndc_y0}, {0.0f,1.0f,1.0f}, {uv0.x, uv0.y}},
            {{ndc_x0, ndc_y1}, {1.0f,0.0f,1.0f}, {uv0.x, uv1.y}},
            {{ndc_x1, ndc_y1}, {1.0f,1.0f,1.0f}, {uv1.x, uv1.y}},
            {{ndc_x1, ndc_y0}, {1.0f,1.0f,0.0f}, {uv1.x, uv0.y}},
        };*/


        uint16_t base_index = (uint16_t)(ui_state->text_system.dynamic_vertices.size());


        // create indices (two triangles per quad)
        std::vector<uint16_t> quad_indices = {
            static_cast<uint16_t>(base_index + 0),
            static_cast<uint16_t>(base_index + 1),
            static_cast<uint16_t>(base_index + 2),
            static_cast<uint16_t>(base_index + 2),
            static_cast<uint16_t>(base_index + 3),
            static_cast<uint16_t>(base_index + 0)
        };

        // Add vertices
        ui_state->text_system.dynamic_vertices.insert(ui_state->text_system.dynamic_vertices.end(), new_quad.begin(),
                                                      new_quad.end());

        // Add indices
        ui_state->text_system.dynamic_indices.insert(ui_state->text_system.dynamic_indices.end(),
                                                     quad_indices.begin(), quad_indices.end());

        final_pos.x += g.advance * font_scalar; // move offset forward
    }
}

void do_text_screen_percentage(UI_System* ui_state, std::string text, vec2 pos, vec2 screen_percentage_size,
                               vec3 color, float font_size)
{
    //tells us how much of the screen we want based on the size percentage
    if (screen_percentage_size.x > 100 || screen_percentage_size.x < 0 || screen_percentage_size.y > 100 ||
        screen_percentage_size.y < 0)
    {
        printf("YOU ARE STUPID: TEXT");
    }
    if (pos.x > 100 || pos.x < 0 || pos.y > 100 || pos.y < 0)
    {
        printf("YOU ARE STUPID 2: TEXT");
    }

    //std::vector<Vertex_Text> new_quad = text_create_quad({0.5f,0.5f}, {0.1f,0.1f}, {1.0f,1.0f,1.0f});

    //move to the shader as a push constant
    float screen_width = ui_state->push_constants.screenSize.x;
    float screen_height = ui_state->push_constants.screenSize.y;

    //convert 0-00 -> 0-1
    glm::vec2 converted_pos = pos / 100.0f;
    glm::vec2 converted_size = screen_percentage_size / 100.0f;

    glm::vec2 final_pos = {
        screen_width * converted_pos.x,
        screen_height * converted_pos.y
    };
    glm::vec2 final_size = {
        (screen_width * converted_size.x),
        (screen_height * converted_size.y)
    };

    //We take the desired font size, scale it down proportional to the font size we created it at
    //final size of the font ex: 36/48 = 0.75, 48*0.75 = 36
    float font_scalar = font_size / ui_state->text_system.default_font_size;

    for (const char& c : text)
    {
        if (c < 32 || c >= 128) continue; // skip unsupported characters
        Glyph& g = ui_state->text_system.glyphs[c - 32];


        //take the x position and move it left based on the size we wanted it at
        float xpos = (final_pos.x - final_size.x + g.xoff);
        float ypos = (final_pos.y + g.yoff);

        //scales the texture
        float w = static_cast<float>(g.width) * font_scalar;
        float h = static_cast<float>(g.height) * font_scalar;

        //printf("xpos %f, ypos%f, w%f, h%f\n", xpos, ypos, w, h);


        // Convert screen coords to NDC [-1,1]
        float ndc_x0 = ((xpos) / screen_width) * 2.0f - 1.0f;
        float ndc_x1 = (((xpos + w)) / screen_width) * 2.0f - 1.0f;
        float ndc_y0 = ((ypos) / screen_height) * 2.0f - 1.0f; // invert Y
        float ndc_y1 = (((ypos + h)) / screen_height) * 2.0f - 1.0f;

        /*
        // Convert screen coords to NDC [-1,1]
        float ndc_x0 = ((xpos - final_size.x)/ screen_width) * 2.0f - 1.0f;
        float ndc_x1 = (((xpos + w) + final_size.x) / screen_width) * 2.0f - 1.0f;
        float ndc_y0 = ((ypos - final_size.x) / screen_height) * 2.0f- 1.0f; // invert Y
        float ndc_y1 = (((ypos + h) - final_size.x) / screen_height) * 2.0f - 1.0f;
        */

        /* here for reference
        {{pos.x - size.x, pos.y - size.y}, {color}},
        {{pos.x - size.x, pos.y + size.y}, {color}},
        {{pos.x + size.x, pos.y + size.y}, {color}},
        {{pos.x + size.x, pos.y - size.y}, {color}}
        */

        // UVs from the atlas
        glm::vec2 uv0(g.u0, g.v0);
        glm::vec2 uv1(g.u1, g.v1);


        std::vector<Vertex_Text> new_quad = {
            {{ndc_x0, ndc_y0}, color, {uv0.x, uv0.y}},
            {{ndc_x0, ndc_y1}, color, {uv0.x, uv1.y}},
            {{ndc_x1, ndc_y1}, color, {uv1.x, uv1.y}},
            {{ndc_x1, ndc_y0}, color, {uv1.x, uv0.y}},
        };

        /* TODO:  fruity colors, maybe ill make a seperate function that allows you to color each edge, cause its kinda cool, realistically you can just overlay another texture but that's fucking lame
        std::vector<Vertex_Text> new_quad = {
            {{ndc_x0, ndc_y0}, {0.0f,1.0f,1.0f}, {uv0.x, uv0.y}},
            {{ndc_x0, ndc_y1}, {1.0f,0.0f,1.0f}, {uv0.x, uv1.y}},
            {{ndc_x1, ndc_y1}, {1.0f,1.0f,1.0f}, {uv1.x, uv1.y}},
            {{ndc_x1, ndc_y0}, {1.0f,1.0f,0.0f}, {uv1.x, uv0.y}},
        };*/

        uint16_t base_index = static_cast<uint16_t>(ui_state->text_system.dynamic_vertices.size());

        // create indices (two triangles per quad)
        std::vector<uint16_t> quad_indices = {
            static_cast<uint16_t>(base_index + 0),
            static_cast<uint16_t>(base_index + 1),
            static_cast<uint16_t>(base_index + 2),
            static_cast<uint16_t>(base_index + 2),
            static_cast<uint16_t>(base_index + 3),
            static_cast<uint16_t>(base_index + 0)
        };

        // Add vertices
        ui_state->text_system.dynamic_vertices.insert(ui_state->text_system.dynamic_vertices.end(), new_quad.begin(),
                                                      new_quad.end());

        // Add indices
        ui_state->text_system.dynamic_indices.insert(ui_state->text_system.dynamic_indices.end(),
                                                     quad_indices.begin(), quad_indices.end());

        final_pos.x += g.advance * font_scalar; // move offset forward
    }
}


void create_text_vertex_buffer_new(Vulkan_Context& vulkan_context, Command_Buffer_Context& command_buffer_context,
                                   Buffer_Context& buffer_context)
{
    uint32_t max_object_count_t = 1000000;
    uint32_t vertices_per_object_t = 4;
    uint32_t MAX_VERTICES_text = max_object_count_t * vertices_per_object_t;

    // Create buffer large enough for maximum vertices, not just initial vertices
    buffer_context.vertex_buffer_capacity = sizeof(Vertex_Text) * MAX_VERTICES_text;
    // Use MAX_VERTICES instead of vertices.size()

    buffer_create(vulkan_context, buffer_context.vertex_buffer_capacity, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                  buffer_context.vertex_staging_buffer, buffer_context.vertex_staging_buffer_memory);

    // Only copy initial vertices data, but allocate full buffer
    VkDeviceSize initial_data_size = sizeof(text_verticies[0]) * vertices.size();

    vkMapMemory(vulkan_context.logical_device, buffer_context.vertex_staging_buffer_memory, 0,
                buffer_context.vertex_buffer_capacity, 0,
                &buffer_context.data_vertex);

    // Zero out the entire buffer first
    memset(buffer_context.data_vertex, 0, buffer_context.vertex_buffer_capacity);
    // Then copy initial data
    memcpy(buffer_context.data_vertex, vertices.data(), initial_data_size);
    vkUnmapMemory(vulkan_context.logical_device, buffer_context.vertex_staging_buffer_memory);

    // Create device local buffer with full size
    buffer_create(vulkan_context, buffer_context.vertex_buffer_capacity,
                  VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer_context.vertex_buffer,
                  buffer_context.vertex_buffer_memory);

    // Copy entire buffer (including zeros for unused space)
    buffer_copy(vulkan_context, command_buffer_context, buffer_context.vertex_staging_buffer,
                buffer_context.vertex_buffer,
                buffer_context.vertex_buffer_capacity);


    std::cout << "CREATED TEXT VERTEX BUFFER SUCCESS (Size: " << buffer_context.vertex_buffer_capacity << " bytes for "
        << MAX_VERTICES <<
        " vertices)\n";
}



void text_system_init(Text_System& text_system)
{
    text_system.default_font_size = 128.0f;
    Texture font_texture;
    Vertex_Text dynamic_vertices;
    uint16_t dynamic_indices;
}



void text_update(Text_System& text_system)
{
    text_system.dynamic_indices.clear();
    text_system.dynamic_vertices.clear();

    //do_text(text_system, "HELLO WORLD!", {1.0f,1.0f,0.0f});
    //do_text_screen_percentage(text_system, push_constants, "HELLO WORLD!",{50.0f,50.0f}, {20.0f, 20.0f}, {1.0f,1.0f,0.0f}, 36.0f, true);
}
