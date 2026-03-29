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
#define EDITOR_FONT_SIZE 16.0f

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
    UI_LAYOUT_HORIZONTAL,
    UI_LAYOUT_VERTICAL,
} UI_Layout_Direction;

typedef struct UI_Padding
{
    //not using a vec4 cause its not clear what the values are suppose to be
    float left;
    float right;
    float top;
    float bottom;
} UI_Padding;


typedef struct UI_Editor_Style
{
    vec3 layout_color;
    vec3 layout_accent_color;

    vec3 text_color;
    vec3 textbox_color;

    vec3 custom_widget_color;


    //colors for things like buttons and checkboxes
    vec3 color;
    vec3 hovered_color;
    vec3 pressed_color;

    vec3 outline_color;
} UI_Editor_Style;


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
    // screen size and pos, not normalized
    vec2 pos;
    vec2 size;
    const char* debug_id;
    u64 hash_id;

    //draw data
    //consider here what actually needs to be done for something to rendered, instead of passing in the entire config
    vec3 color;
    Texture_Handle texture_handle;
    Sprite_Pipeline_Flags flags;



} UI_Node;

typedef struct UI_Node_Text
{
    vec2 pos;
    vec2 size;

    // offset into a texture atlas if using one, otherwise {0, 0}
    vec2 text_uv_offset;
    // start from offset and this will give us our bottom right uv, which tells us all the other info we need
    vec2 text_uv_size;

    bool is_text;
    bool start_text; // is the first character in the text
}UI_Node_Text;


ARRAY_GENERATE_TYPE(UI_Node)
ARRAY_GENERATE_TYPE(UI_Node_Text)


typedef struct Font_Handle
{
    u32 handle;
} Font_Handle;

typedef struct scroll_box_state
{
    u32 max_nodes_to_display; // user sets info

    //internal
    u32 scroll_amount;

    vec2 scroll_box_cursor_pos;
    vec2 scroll_box_cursor_size;

    u32 scroll_box_node_start_count;

    u32 scroll_box_text_start_count;
    u32 scroll_box_text_end_count;


}scroll_box_state;

typedef struct
{
    vec2* pos;
    vec3* color;
    //there is an interesting optimization given that all UI's are squares
    VkDrawIndexedIndirectCommand* indirect_draw_array;
} UI_Draw_data_new;

//rn this is purely a ui for the editor, in game ui is for another time, when the game comes along
typedef struct Madness_UI
{
    Arena* arena; // rn mainly just for loading fonts, would be better as a pool arena
    Frame_Arena* frame_arena;

    Renderer* renderer_reference;
    Input_System* input_system_reference; // does not own memory

    //this should be an array at some point
    Madness_Font default_font;
    float default_font_size;
    float editor_font_size;
    // Font fonts[100];

    UI_Node_array* ui_nodes;
    // UI_Node_array* button_nodes; //TODO: we will see

    int hot;
    int active;

    bool mouse_down;
    bool mouse_released_unique;
    i16 mouse_pos_x;
    i16 mouse_pos_y;

    //TODO: keep track if backspace has been held down for a certain period of time
    // for the backspace functionality of the textbox

    char released_key;

    //stores id and the ui state they are in the previous frame, if applicable
    hash_table* button_hash_states; // only for buttons
    hash_table* check_box_states; // only for checkboxes


    //Keep an array of strings used in textboxes
    //should be an array at some point,
    String_Builder* string_builder;
    hash_table* textbox_ids; //maps textbox names to their id in the string builder array

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

    UI_Editor_Style editor_style;

    vec2 current_layout_pos; // converted pos of current layout
    vec2 current_layout_size; // converted size of current layout
    vec2 current_layout_screen_pos; // converted pos of current layout
    vec2 current_layout_screen_size; // converted size of current layout


    vec2 ghost_pos; //used for transitioning layouts, where we want to know the last element
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



//API START (besides init/shutdown, begin/end)
MAPI void madness_ui_begin_layout(Madness_UI* madness_ui, const char* id, vec2 pos, vec2 size);

MAPI void madness_set_layout_direction(Madness_UI* madness_ui, UI_Layout_Direction layout_direction);

MAPI bool madness_button(Madness_UI* madness_ui, const char* id);
MAPI void madness_text(Madness_UI* madness_ui, const char* id, String text);

MAPI bool madness_button_text(Madness_UI* madness_ui, const char* id, String text);
MAPI bool madness_check_box(Madness_UI* madness_ui, const char* id, String text, bool* check_box_state);

MAPI void madness_icon(Madness_UI* madness_ui, const char* id, const char* icon_path);

MAPI void madness_slider_scroll(Madness_UI* madness_ui, const char* id, float* slider_val, float min, float max);
MAPI void madness_slider_arrow(Madness_UI* madness_ui, const char* id, float* slider_val, float min, float max);

MAPI void madness_text_box(Madness_UI* madness_ui, const char* id);

MAPI void madness_ui_float(Madness_UI* madness_ui, const char* id, float* f, float increment_value);
MAPI void madness_ui_vec2(Madness_UI* madness_ui, const char* id,  String text, vec2* v2, float increment_value);
MAPI void madness_ui_vec3(Madness_UI* madness_ui, const char* id, String text, vec3* v3, float increment_value);


MAPI bool madness_ui_color_picker(Madness_UI* madness_ui, const char* id, vec3* color_value);

MAPI void madness_scroll_box_begin(Madness_UI* madness_ui, const char* id, scroll_box_state* scroll_box_state);
MAPI void madness_scroll_box_end(Madness_UI* madness_ui, const char* id, scroll_box_state* scroll_box_state);



//API END


//these is only meant for internal use and not part of the API
void madness_draw_quad(Madness_UI* madness_ui, const char* id, vec2* out_pos, vec2* out_size, UI_Node** out_node);
void madness_draw_text(Madness_UI* madness_ui, String text, vec2 screen_position);
void madness_draw_text_centered(Madness_UI* madness_ui, String text, vec2 parent_pos, vec2 parent_size);
void madness_calculate_text_size(Madness_UI* madness_ui, String text, vec2 screen_position, vec2* out_text_size);



//debug and test
void madness_ui_print_state(Madness_UI* madness_ui);

void madness_ui_test(Madness_UI* madness_ui);


//utility
UI_Node* madness_ui_get_new_node(Madness_UI* madness_ui);
void madness_ui_update_next_element_pos(Madness_UI* madness_ui, vec2 ui_screen_size);

void madness_ui_center_child_node(vec2 parent_pos, vec2 parent_size, vec2 child_size, vec2* out_pos);
char* madness_ui_float_to_char(Madness_UI* madness_ui, float value);


bool is_ui_hot(Madness_UI* madness_ui, int id);

bool is_ui_active(Madness_UI* madness_ui, int id);

bool region_hit(Madness_UI* madness_ui, vec2 pos, vec2 size);

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
bool use_ui_element(Madness_UI* madness_ui, int id, vec2 pos, vec2 size);

int generate_id(Madness_UI* madness_ui);

void set_hot(Madness_UI* madness_ui, int id);

void set_active(Madness_UI* madness_ui, int id);

bool can_be_active(Madness_UI* madness_ui);
bool is_active(Madness_UI* madness_ui, int id);

bool is_hot(Madness_UI* madness_ui, int id);


//API
//TODO: add UI_Alignment
bool do_button(Madness_UI* madness_ui, int id, vec2 pos, vec2 screen_percentage,
               vec3 color, vec3 hovered_color, vec3 pressed_color);

bool do_button_text(Madness_UI* madness_ui, int id, String text, vec2 pos, vec2 size,
                    vec2 text_padding, vec3 color, vec3 hovered_color, vec3 pressed_color);

void do_text(Madness_UI* madness_ui, String text, vec2 pos, vec2 screen_percentage_size,
             vec3 color, float font_size);

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


//Vulkan


UI_Renderer_Backend* ui_render_init(Renderer* renderer);

void madness_ui_generate_render_data(Madness_UI* madness_ui, Render_Packet* render_packet);
void ui_renderer_upload_draw_data(UI_Renderer_Backend* ui_renderer, Renderer* renderer, Render_Packet* render_packet);
void ui_renderer_draw(UI_Renderer_Backend* ui_renderer, Renderer* renderer, vulkan_command_buffer* command_buffer,
                      Render_Packet* render_packet);

#endif //UI_H
