#ifndef UI_H
#define UI_H

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
} UI_Padding;


typedef struct UI_Config
{
    // FUTURE: this should either be a vec3, transform or neither and we just pass in the transform values seperately for the 3D UI
    vec2 pos; //remove later
    vec2 size;
    UI_Padding padding;
    UI_Sizing size_type;
    UI_Alignment alignment;

    vec3 color;
    vec3 normal_color;
    vec3 hovered_color;
    vec3 pressed_color;
    UI_Layout_Direction layout_direction;

    const char* texture_path;
    Texture_Handle texture_handle;
} UI_Config;


typedef struct UI_Data
{
    vec2 pos;
    vec2 size;
}UI_Data;


typedef struct UI_Style
{
    // FUTURE: this should either be a vec3, transform or neither and we just pass in the transform values seperately for the 3D UI
    UI_Padding padding;
    UI_Sizing size_type;
    UI_Alignment alignment;

    vec3 color;
    vec3 normal_color;
    vec3 hovered_color;
    vec3 pressed_color;
    UI_Layout_Direction layout_direction;

    const char* texture_path;
    Texture_Handle texture_handle;
} UI_Style;

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

typedef enum UI_Button_State
{
    BUTTON_STATE_COLD,
    BUTTON_STATE_HOT,
    BUTTON_STATE_ACTIVE,
} UI_Button_State;


//TODO:  just a temporary value for now, will increase later
#define MAX_UI_NODE_COUNT 100
#define MAX_UI_NODE_CHILD_COUNT 10

typedef struct UI_Node
{
    vec2 pos;
    vec2 size;
    struct UI_Node* parent;
    //this has to be an array, 10 for now, but we will make it dynamic later
    struct UI_Node* children[MAX_UI_NODE_CHILD_COUNT];
    u64 children_length;
    UI_Config config;
    const char* debug_id;

    Sprite_Pipeline_Flags flags;
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

typedef struct Madness_UI
{
    Arena* arena; // rn mainly just for loading fonts, would be better as a pool arena
    Frame_Arena* frame_arena;

    Renderer* renderer_reference;
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

    //stores id and the ui state they are in the previous frame, if applicable
    hash_table* ui_hash_states; // this might not be needed
    hash_table* button_hash_states; // only for buttons


    vec2 screen_size; // this gets queried every frame


    //TODO: the 100 is temporary
    Sprite_Data_array* ui_data;
    Sprite_Data_array* text_data;

    //TODO: move these out of the UI
    PC_2D pc_2d_text;
    PC_2D pc_2d_quad;
    //u16
    VkIndexType index_type;

    //new UI
    bool compose;
    vec2 cursor_pos; //position of where to draw the next ui element
    UI_Layout_Direction layout_direction; // direction to position the ui element
    float element_padding; // space between each ui element



} Madness_UI;


//NOTE: Remove the renderer from the init, these should not be coupled
//I should only have to pass the vertex/index data to the renderer for drawing
MAPI Madness_UI* madness_ui_init(Renderer* renderer);
MAPI bool madness_ui_shutdown(Madness_UI* madness_ui, Renderer* renderer);

//pass in the size every frame, in the event the size changes
MAPI void madness_ui_begin(Madness_UI* madness_ui, i32 screen_size_x, i32 screen_size_y);
MAPI void madness_ui_end(Madness_UI* madness_ui);

//Text
Font_Handle font_init(Madness_UI* madness_ui, Renderer* renderer, const char* filepath);


bool is_ui_hot(Madness_UI* madness_ui, int id);

bool is_ui_active(Madness_UI* madness_ui, int id);

bool region_hit_new(Madness_UI* madness_ui, vec2 pos, vec2 size);

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

void update_ui_mouse_pos(Madness_UI* madness_ui);

//check if we can use the button
bool use_button(Madness_UI* madness_ui, UI_ID id, vec2 pos, vec2 size);

bool use_button_new(Madness_UI* madness_ui, UI_ID id, vec2 pos, vec2 size);

int generate_id(Madness_UI* madness_ui);

void set_hot(Madness_UI* madness_ui, UI_ID id);

void set_active(Madness_UI* madness_ui, UI_ID id);

bool can_be_active(Madness_UI* madness_ui);
bool is_active(Madness_UI* madness_ui, UI_ID id);

bool is_hot(Madness_UI* madness_ui, UI_ID id);


//API
//TODO: add UI_Alignment
bool do_button(Madness_UI* madness_ui, UI_ID id, vec2 pos, vec2 screen_percentage,
               vec3 color, vec3 hovered_color, vec3 pressed_color);

bool do_button_config(Madness_UI* madness_ui, UI_ID id, UI_Config ui_config);

bool do_button_text(Madness_UI* madness_ui, UI_ID id, String text, vec2 pos, vec2 size,
                    vec2 text_padding, vec3 color, vec3 hovered_color, vec3 pressed_color);

void do_text(Madness_UI* madness_ui, String text, vec2 pos, vec2 screen_percentage_size,
             vec3 color, float font_size);

void madness_ui_test(Madness_UI* madness_ui);
void madness_ui_test2(Madness_UI* madness_ui);

void madness_ui_open_node(Madness_UI* madness_ui, const char* id, UI_Config config);
void madness_ui_close_node(Madness_UI* madness_ui, const char* id);
void madness_ui_calculate_positions(Madness_UI* madness_ui);

void madness_ui_generate_debug_data(Madness_UI* madness_ui);

// NEW API

//designing what I want it to look like from the ground up
/*
    //WANTS:
    button, text, text box, textures, on hover popup, slider, checkbox

    Configs:
    rounded rects, highlight borders, clickable,


    ui_config = {horizontal, pos, size}
    ui_button1_config = {pos, size}
    ui_item_begin(ui_config);

    if(button(ui_button1_config)){};
    if(button()){};


    vertical_box_end();


 */


void madness_ui_begin_region(Madness_UI* madness_ui, const char* id);
void madness_ui_end_region(Madness_UI* madness_ui, const char* id);


void madness_set_layout_direction(Madness_UI* madness_ui, UI_Layout_Direction layout_direction);

bool madness_button(Madness_UI* madness_ui, const char* id, UI_Config config);

void madness_button_text(const char* id, vec2 pos, vec2 size)
{
}

void madness_icon(Madness_UI* madness_ui, const char* id, const char* icon_path)
{
}

void madness_slider(Madness_UI* madness_ui, const char* id, float cur_val, float min, float max)
{
}

void madness_text_box(Madness_UI* madness_ui, const char* id)
{

}

void madness_ui_test3(Madness_UI* madness_ui);



//Vulkan


UI_Renderer_Backend* ui_render_init(Renderer* renderer);

void madness_ui_generate_render_data(Madness_UI* madness_ui, Render_Packet* render_packet);
void ui_renderer_upload_draw_data(UI_Renderer_Backend* ui_renderer, Renderer* renderer, Render_Packet* render_packet);
void ui_renderer_draw(UI_Renderer_Backend* ui_renderer, Renderer* renderer, vulkan_command_buffer* command_buffer,
                      Render_Packet* render_packet);

#endif //UI_H
