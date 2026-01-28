//
// Created by Adams Humbert on 9/10/2025.
//

#ifndef TEXT_H
#define TEXT_H

#include "vulkan_types.h"



struct Vertex_Text
{
    glm::vec2 pos;
    glm::vec3 color;
    glm::vec2 tex;
};

inline std::vector<Vertex_Text> text_verticies = {
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
};

VkVertexInputBindingDescription get_text_binding_description();

std::array<VkVertexInputAttributeDescription, 3> get_text_attribute_descriptions();


struct Glyph {
    int width, height;
    int xoff, yoff;
    float advance;
    float u0, v0, u1, v1; // UV coordinates in atlas
};

struct Text_System
{
    stbtt_fontinfo font;
    float default_font_size; // the larger the more clear the text looks
    Glyph glyphs[96];
    Texture font_texture;

    std::vector<Vertex_Text> dynamic_vertices{};
    std::vector<uint16_t> dynamic_indices{};

};

void text_system_init(Text_System& text_system);

bool load_font(Text_System& text_system, const char* filepath, vulkan_context& vulkan_context, Command_Buffer_Context&
               command_buffer_context);


std::vector<Vertex_Text> text_create_quad(glm::vec2 pos, glm::vec2 size, glm::vec3 color, glm::vec2 tex);

void do_text(UI_STATE* ui_state, std::string text, glm::vec2 pos, glm::vec3 color = {1.0f,1.0f,1.0f}, float font_size = 48.0f); // default the colors to white

void do_text_screen_percentage(UI_STATE* ui_state, std::string text, glm::vec2 pos, glm::vec2 screen_percentage_size, glm::vec3 color, float
                               font_size);


void text_vertex_buffer_update(vulkan_context& vulkan_context, Command_Buffer_Context& command_buffer_context, Buffer_Context& buffer_context, Text_System& text_system);


void text_update(Text_System& text_system, Screen_Size_Push_Constants push_constants);

void create_text_vertex_buffer_new(vulkan_context& vulkan_context, Command_Buffer_Context& command_buffer_context, Buffer_Context& buffer_context);



#endif //TEXT_H
