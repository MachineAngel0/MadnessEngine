#ifndef UI_H
#define UI_H

#include "sprite.h"
#include "stb_truetype.h"

//IMMEDIATE MODE UI

//NOTE: the smallest likely screen size for a laptop is likely to be a 1024*600 or more commonly 1024 * 768

//Vertex Data


typedef struct PC_2D
{
    u32 material_buffer_idx;
} PC_2D;

typedef struct Material_2D_Param_Data
{
    u32 feature_mask;
    u32 texture_index;
} Material_2D_Param_Data;


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
    // NOTE: we make all font default size and scale up or down
    // float font_size; // the larger the more clear the text looks
    //NOTE: this will have to be larger if i support other languages or non standard characters
    Glyph glyphs[96]; // idk why this is 96, im assuming for all the ascii characters
    Texture_Handle font_texture_handle;
} Madness_Font;

#define DEFAULT_FONT_CREATION_SIZE 128.0f
#define DEFAULT_FONT_SIZE 48.0f

//UI
typedef enum UI_Alignment
{
    //basically just a nine slice
    UI_ALIGNMENT_LEFT, // left should be the default
    UI_ALIGNMENT_CENTER,
    UI_ALIGNMENT_RIGHT,
    UI_ALIGNMENT_TOP,
    UI_ALIGNMENT_BOTTOM,
    UI_ALIGNMENT_TOP_LEFT,
    UI_ALIGNMENT_TOP_RIGHT,
    UI_ALIGNMENT_BOTTOM_LEFT,
    UI_ALIGNMENT_BOTTOM_RIGHT,
    UI_ALIGNMENT_MAX,
} UI_Alignment;


typedef enum UI_TYPE
{
    UI_TYPE_RECTANGLE = BITFLAG(1),
    UI_TYPE_BUTTON = BITFLAG(2),
    UI_TYPE_SLIDER = BITFLAG(3),
    UI_TYPE_IMAGE = BITFLAG(4),
    UI_TYPE_TEXT = BITFLAG(5),
} UI_TYPE;


typedef enum UI_Sizing
{
    UI_SIZING_FIXED, //fit only the given size it was specified
    UI_SIZING_FIT, //fit only its children, which also means it won't appear in the layout if there is no padding
    UI_SIZING_FIT_EXPAND, //fit its children and plus whatever size was specified
    UI_SIZING_EXPAND, // expands to take up any space that is left
    UI_SIZING_MAX,
} UI_Sizing;

typedef enum UI_Layout_Direction
{
    UI_LAYOUT_LEFT_TO_RIGHT,
    UI_LAYOUT_TOP_TO_BOTTOM,
} UI_Layout_Direction;

typedef struct UI_Padding
{
    //not using a vec4 cause its not clear what the values are suppose to be
    float left;
    float right;
    float top;
    float bottom;
}UI_Padding;

typedef struct UI_Config
{
    // FUTURE: this should either be a vec3, transform or neither and we just pass in the transform values seperately for the 3D UI
    vec2 pos;//remove later
    vec2 size;
    UI_Padding padding;
    UI_Sizing size_type;
    UI_Alignment alignment;

    vec3 color;
    vec3 normal_color;
    vec3 hovered_color;
    vec3 pressed_color;
    UI_Layout_Direction layout_direction;
} UI_Config;

typedef struct UI_Text_Config
{
    vec2 pos;
    vec2 size;
    UI_Alignment alignment;
    vec3 color;
    vec3 font_size;
    bool wrap;
    //PASS IN STRING SEPERATELY
} UI_Text_Config;


//TODO:  just a temporary value for now, will increase later
#define MAX_UI_NODE_COUNT 100
#define MAX_UI_NODE_CHILD_COUNT 10

typedef struct UI_Node
{
    float pos_x;
    float pos_y;
    float size_x;
    float size_y;
    struct UI_Node* parent;
    //this has to be an array, 10 for now, but we will make it dynamic later
    struct UI_Node* children[MAX_UI_NODE_CHILD_COUNT];
    u64 children_length;
    UI_Config config;
    const char* debug_id;
} UI_Node;

ARRAY_GENERATE_TYPE(UI_Node)

typedef struct UI_ID
{
    int ID;
    int layer;
} UI_ID;




typedef struct Font_Handle
{
    u32 handle;
} Font_Handle;


typedef struct
{
    vec2* pos;
    vec3* color;
    //there is an interesting optimization given that all UI's are squares
    VkDrawIndexedIndirectCommand* indirect_draw_array;
} UI_Draw_data_new;

typedef struct UI_Quad_Draw_Data
{
    //u16
    VkIndexType index_type;

    //TODO: the 100 is temporary
    Quad_Vertex quad_vertex[1000];
    u32 quad_vertex_bytes;

    u16 indices[1000];
    u32 index_bytes;
    u32 index_count;

    u32 quad_draw_count;

    PC_2D pc_2d_quad;
} UI_Quad_Draw_Data;

typedef struct UI_Text_Draw_Data
{
    //every triangle is 4quad textures and 6 indices
    //index += 6 + 6
    //index_count += 6 every mesh

    //u16
    VkIndexType index_type;

    //TODO: the 100 is temporary
    Quad_Texture text_vertex[1000];
    u32 text_vertex_bytes;

    u16 text_indices[1000];
    u32 text_index_bytes;
    u32 text_index_count;


    Material_2D_Param_Data text_material_params[1000];
    u32 text_material_param_current_size;

    u32 text_draw_count;

    PC_2D pc_2d_text;
} UI_Text_Draw_Data;



typedef struct UI_System
{
    Arena* arena; // rn mainly just for loading fonts, would be better a pool arena
    Frame_Arena* frame_arena;


    Input_System* input_system_reference; // does not own memory

    //this should be an array at some point
    Madness_Font default_font;
    float default_font_size;
    // Font fonts[100];

    UI_Node** ui_stack;
    u32 ui_stack_count;
    u32 ui_stack_capacity;
    UI_Node_array* ui_nodes;


    UI_ID hot;
    UI_ID active;

    int id_generation_number;

    bool mouse_down;
    bool mouse_released;
    i16 mouse_pos_x;
    i16 mouse_pos_y;

    vec2 screen_size; // this gets queried every frame

    UI_Quad_Draw_Data quad_draw_info;
    UI_Text_Draw_Data text_draw_info;
    // Text_System text_system;

    //
    Buffer_Handle ui_quad_vertex_buffer_handle;
    Buffer_Handle ui_quad_index_buffer_handle;
    Buffer_Handle ui_quad_indirect_buffer_handle;

    Buffer_Handle text_vertex_buffer_handle;
    Buffer_Handle text_index_buffer_handle;
    Buffer_Handle text_material_ssbo_handle;
    Buffer_Handle text_indirect_buffer_handle;

    // Buffer_Handle ui_quad_texture_vertex_buffer_handle;
    // Buffer_Handle ui_quad_texture_index_buffer_handle;


    //HANDLES
    Buffer_Handle ui_quad_vertex_staging_buffer_handle;
    Buffer_Handle ui_quad_index_staging_buffer_handle;
    Buffer_Handle ui_quad_indirect_staging_buffer_handle;

    Buffer_Handle text_vertex_staging_buffer_handle;
    Buffer_Handle text_index_staging_buffer_handle;
    Buffer_Handle text_material_staging_ssbo_handle;
    Buffer_Handle text_indirect_staging_buffer_handle;
} Madness_UI_Context;



static Madness_UI_Context* Madness_UI;

//NOTE: Remove the renderer from the init, these should not be coupled
//I should only have to pass the vertex/index data to the renderer for drawing
bool madness_ui_init(renderer* renderer);
//TODO: UI_System* ui_system_shutdown(renderer* renderer);

//pass in the size every frame, in the event the size changes
void madness_ui_begin(i32 screen_size_x, i32 screen_size_y);
void madness_ui_end(void);

//Text
Font_Handle font_init(renderer* renderer, const char* filepath);
//VULKAN
void madness_ui_upload_draw_data(renderer* renderer);


bool is_ui_hot(int id);

bool is_ui_active(int id);

bool region_hit(vec2 pos, vec2 size);

bool region_hit_new(vec2 pos, vec2 size);

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

void update_ui_mouse_pos(void);

//check if we can use the button
bool use_button(UI_ID id, vec2 pos, vec2 size);

bool use_button_new(UI_ID id, vec2 pos, vec2 size);

int generate_id(void);

void set_hot(UI_ID id);

void set_active(UI_ID id);

bool can_be_active(void);
bool is_active(UI_ID id);

bool is_hot(UI_ID id);


//API
//TODO: add UI_Alignment
bool do_button(UI_ID id, vec2 pos, vec2 screen_percentage,
               vec3 color, vec3 hovered_color, vec3 pressed_color);

bool do_button_config(UI_ID id, UI_Config ui_config);

bool do_button_text(UI_ID id, String text, vec2 pos, vec2 size,
                    vec2 text_padding, vec3 color, vec3 hovered_color, vec3 pressed_color);

void do_text(String text, vec2 pos, vec2 screen_percentage_size,
             vec3 color, float font_size);

void madness_ui_add_quad_vertex(Quad_Vertex* new_quad);

void madness_ui_test();
void madness_ui_test2(renderer* renderer);

void madness_ui_open_node(const char* id, UI_Config config);
void madness_ui_close_node(const char* id);
void madness_ui_calculate_positions(void);

//Vulkan
void madness_ui_generate_draw_data(void);
void madness_ui_draw(renderer* renderer, vulkan_command_buffer* command_buffer);
void madness_ui_generate_debug_data(void);

#endif //UI_H
