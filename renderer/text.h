

#ifndef TEXT_H
#define TEXT_H







void text_system_init(Text_System& text_system);

bool load_font(Text_System& text_system, const char* filepath, vulkan_context& vulkan_context, Command_Buffer_Context&
               command_buffer_context);


Vertex_Text* text_create_quad(vec2 pos, vec2 size, vec3 color, vec2 tex);

void do_text(UI_System* ui_state, String text, vec2 pos, vec3 color = {1.0f,1.0f,1.0f}, float font_size = 48.0f); // default the colors to white

void do_text_screen_percentage(UI_System* ui_state, String text, vec2 pos, vec2 screen_percentage_size, vec3 color, float
                               font_size);


void text_vertex_buffer_update(vulkan_context& vulkan_context, Command_Buffer_Context& command_buffer_context, Buffer_Context& buffer_context, Text_System& text_system);


void text_update(Text_System& text_system);

void create_text_vertex_buffer_new(vulkan_context& vulkan_context, Command_Buffer_Context& command_buffer_context, Buffer_Context& buffer_context);



#endif //TEXT_H
