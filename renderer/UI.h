#ifndef UI_H
#define UI_H
#include "stb_truetype.h"

//IMMEDIATE MODE UI

//Vertex Data

typedef struct Quad_Vertex
{
    vec2 pos;
    vec3 color;
} Quad_Vertex;

typedef struct Quad_Texture
{
    vec2 pos;
    vec2 tex;
} Quad_Texture;


//Text

typedef struct Glyph
{
    int width, height;
    int xoff, yoff;
    float advance;
    float u0, v0, u1, v1; // UV coordinates in atlas
} Glyph;

//called Madness font cause a linux library uses the struct font
typedef struct Madness_Font
{
    stbtt_fontinfo font_info; // NOTE: idk if i even need to store this
    float font_size; // the larger the more clear the text looks
    //NOTE: this will have to be larger if i support other languages
    Glyph glyphs[96]; // idk why this is 96, im assuming for all the ascii characters
    Texture font_texture;
} Madness_Font;


//UI
typedef enum UI_Alignment
{
    UI_ALIGNMENT_CENTER,
    UI_ALIGNMENT_LEFT,
    UI_ALIGNMENT_RIGHT,
    //JUSTIFIED, ?????
    UI_ALIGNMENT_MAX,
} UI_Alignment;


typedef struct UI_ID
{
    int ID;
    int layer;
} UI_ID;

//TODO: update to SOA
typedef struct UI_BUTTON
{
    UI_ID id;
    vec2 position;
    vec2 screen_percentage;
    vec3 color;
    vec3 hover_color;
    vec3 pressed_color;
    int _padding;
} UI_BUTTON;

typedef struct UI_TEXTURED_BUTTON
{
    UI_ID id;
    vec2 position;
    vec2 screen_percentage;
    Texture_Handle color;
    Texture_Handle hover_color;
    Texture_Handle pressed_color;
} UI_TEXTURED_BUTTON;


typedef struct Font_Handle
{
    u32 handle;
} Font_Handle;


//auto has the default uv's
Quad_Texture default_quad_texture_vertex[4] = {
    {{-0.5f, -0.5f}, {1.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f}}
};


uint16_t default_quad_indices[6] = {
    0, 1, 2, 2, 3, 0
};


typedef struct
{
    vec2* pos;
    vec3* color;
    //there is an interesting optimization given that all UI's are squares
    VkDrawIndexedIndirectCommand* indirect_draw_array;
} UI_Draw_data_new;

typedef struct UI_Draw_Data
{
    //u16
    VkIndexType index_type;

    //UI

    //TODO: the 100 is temporary
    Quad_Vertex quad_vertex[100];
    u32 quad_vertex_bytes;

    u16 indices[100];
    u32 index_bytes;
    u32 index_count;

    //UI - with texture

    /*
    //TODO: the 100 is temporary
    Quad_Texture quad_texture_vertex[100];
    u32 quad_texture_vertex_bytes;

    u16 quad_texture_indices[100];
    u32 quad_texture_index_bytes;
    u32 quad_texture_index_count;
    */

    //TEXT

    //TODO: the 100 is temporary
    Quad_Texture text_vertex[100];
    u32 text_vertex_bytes;

    u16 text_indices[100];
    u32 text_index_bytes;
    u32 text_index_count;
} UI_Draw_Data;


typedef enum UI_Type
{
    color,
    texture,
    // text,??????
} UI_Type;


typedef struct UI_System
{
    Arena* arena; // rn mainly just for loading fonts, would be better a pool arena
    Frame_Arena* frame_arena;

    //this should be an array at some point
    Madness_Font default_font;
    float default_font_size;
    // Font fonts[100];

    UI_ID hot;
    UI_ID active;

    int id_generation_number;

    bool mouse_down;
    bool mouse_released;
    i16 mouse_pos_x;
    i16 mouse_pos_y;

    vec2 screen_size;

    UI_Draw_Data draw_info;
    UI_BUTTON* UI_Objects; // darray or maybe even an allocator
    // Text_System text_system;

    Buffer_Handle ui_quad_vertex_buffer_handle;
    Buffer_Handle ui_quad_index_buffer_handle;
    Buffer_Handle text_vertex_buffer_handle;
    Buffer_Handle text_index_buffer_handle;


    Buffer_Handle ui_quad_vertex_staging_buffer_handle;
    Buffer_Handle ui_quad_index_staging_buffer_handle;
    Buffer_Handle text_vertex_staging_buffer_handle;
    Buffer_Handle text_index_staging_buffer_handle;

} UI_System;

//NOTE: Remove the renderer from the init, these should not be coupled
//I should only have to pass the vertex/index data to the renderer for drawing
UI_System* ui_system_init(renderer* renderer);

//pass in the size every frame, in the event the size changes
void ui_system_begin(UI_System* ui_system, i32 screen_size_x, i32 screen_size_y);
void ui_system_end(UI_System* ui_system);

//Text
Font_Handle font_init(renderer* renderer, UI_System* ui_system, const char* filepath);
//VULKAN
void ui_system_upload_draw_data(renderer* renderer, UI_System* ui_system);
void ui_system_draw(renderer* renderer, UI_System* ui_system, vulkan_command_buffer* command_buffer);


//for drawing
Quad_Vertex* UI_create_quad(UI_System* ui_system, vec2 pos, vec2 size, vec3 color);


Quad_Vertex* UI_create_quad_screen_percentage(UI_System* ui_system, vec2 pos, vec2 size, vec3 color);

bool is_ui_hot(UI_System* ui_state, int id);

bool is_ui_active(UI_System* ui_state, int id);

bool region_hit(UI_System* ui_system, vec2 pos, vec2 size);

bool region_hit_new(UI_System* ui_state, vec2 pos, vec2 size);

/*
bool button(UI_STATE& ui_state, int id, int x, int y)
{
    if (region_hit())
    {
        ui = hot;
        if (acitve == 0 and mouse down)
            activeitem = id
    }
}*/


//UTILITY

void update_ui_mouse_pos(UI_System* ui_system);

//check if we can use the button
bool use_button(UI_System* ui_state, UI_ID id, vec2 pos, vec2 size);

bool use_button_new(UI_System* ui_state, UI_ID id, vec2 pos, vec2 size);

int generate_id(UI_System* ui_state);

void set_hot(UI_System* ui_state, UI_ID id);

void set_active(UI_System* ui_state, UI_ID id);

bool can_be_active(UI_System* ui_state);
bool is_active(UI_System* ui_state, UI_ID id);

bool is_hot(UI_System* ui_state, UI_ID id);


//API
bool do_button(UI_System* ui_system, UI_ID id, vec2 pos, vec2 screen_percentage,
               vec3 color, vec3 hovered_color, vec3 pressed_color);

bool do_button_new(UI_System* ui_system, UI_ID id, vec2 pos, vec2 size,
                   UI_Alignment alignment, vec3 color,
                   vec3 hovered_color, vec3 pressed_color);

#define DO_BUTTON_TEST(UI_SYSTEM, UI_ID) do_button(UI_SYSTEM, UI_ID, (vec2){0.0f,0.0f}, (vec2){10.0f,10.0f}, (vec3){1.0f, 1.0f, 1.0f}, (vec3){1.0f, 1.0f, 1.0f}, (vec3){1.0f, 1.0f, 1.0f});


bool do_button_new_text(UI_System* ui_state, UI_ID id, String text, vec2 pos, vec2 size,
                        vec2 text_padding, vec3 color, vec3 hovered_color, vec3 pressed_color);

bool do_button_text(UI_System* ui_state, UI_ID id, String text, vec2 pos, vec2 screen_percentage,
                    vec3 color, vec3 hovered_color, vec3 pressed_color);

void do_text_screen_percentage(UI_System* ui_system, String text, vec2 pos, vec2 screen_percentage_size,
                               vec3 color, float font_size);


#endif //UI_H
