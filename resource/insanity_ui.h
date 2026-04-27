#ifndef INSANITY_UI_H
#define INSANITY_UI_H

#include "arena.h"
#include "stack.h"
#include "str_builder.h"


//IMMEDIATE MODE UI

//NOTE: the smallest likely screen size for a laptop is likely to be a 1024*600 or more commonly 1024 * 768
// smalles ill let it be is 600*600


// button -> clickable, hoverable, box, text (maybe)
// slider -> clickable, hoverable, box, slider (horizontal/vertical),
// layout -> start and end, children, scrollable, slider(horizontal/vertical), draggable
// float -> box, text, scroll thing
// vec3 -> vertical layout, box, /children ->  background, wheel scroll


#define INSANITY_DEFAULT_FONT_SIZE 32.0f
#define INSANITY_EDITOR_FONT_SIZE 24.0f

#define INSANITY_MAX_UI_NODE_COUNT 1000
#define INSANITY_MAX_UI_TEXT_NODE_COUNT 1000
#define INSANITY_MAX_UI_NODE_CHILD_COUNT 10

//UI
typedef enum Insanity_UI_Alignment
{
    //basically just a nine slice
    Insanity_UI_ALIGNMENT_LEFT, // left should be the default
    Insanity_UI_ALIGNMENT_CENTER,
    Insanity_UI_ALIGNMENT_RIGHT,
    //TODO: not supporting the below rn
    // Insanity_UI_ALIGNMENT_TOP,
    // Insanity_UI_ALIGNMENT_BOTTOM,
    // Insanity_UI_ALIGNMENT_TOP_LEFT,
    // Insanity_UI_ALIGNMENT_TOP_RIGHT,
    // Insanity_UI_ALIGNMENT_BOTTOM_LEFT,
    // Insanity_UI_ALIGNMENT_BOTTOM_RIGHT,
    // Insanity_UI_ALIGNMENT_MAX,
} Insanity_UI_Alignment;


typedef enum Insanity_UI_Property_Flags
{
    UI_TYPE_NONE = BITFLAG(0),
    UI_TYPE_BACKGROUND = BITFLAG(1),
    UI_TYPE_CLICKABLE = BITFLAG(2),
    UI_TYPE_IMAGE = BITFLAG(3),
    UI_TYPE_TEXT = BITFLAG(4),
    UI_TYPE_OUTLINE = BITFLAG(5),
    UI_TYPE_SCROLL_VIEW = BITFLAG(6),
    UI_TYPE_COLOR = BITFLAG(7), // this is kinda implied all the time
    UI_TYPE_DRAGGABLE = BITFLAG(8), //NOTE: child elements are not draggable, only a root parent is draggable
    UI_TYPE_ROUND_CORNER = BITFLAG(9),
    UI_TYPE_CIRCLE = BITFLAG(10),
    UI_TYPE_SCROLL_FLOAT = BITFLAG(11),
    UI_TYPE_TEXT_INPUT = BITFLAG(12),
    // UI_TYPE_SCROLL_INT = BITFLAG(13),
    // UI_TYPE_ = BITFLAG(14),
    // UI_TYPE_ = BITFLAG(15),
    // UI_TYPE_ = BITFLAG(16),
} Insanity_UI_Property_Flags;

typedef enum Insanity_UI_Interaction_Event
{
    UI_EVENT_HOVER = BITFLAG(0),
    UI_EVENT_CLICK = BITFLAG(1),
    UI_EVENT_DRAG = BITFLAG(2),
    UI_EVENT_SCROLL = BITFLAG(3),
    UI_EVENT_TEXT_INPUT = BITFLAG(4),
    UI_EVENT_KEYBOARD = BITFLAG(5),
    UI_EVENT_CONTROLLER = BITFLAG(6),
    UI_EVENT_FLOAT_CHANGE = BITFLAG(7),
} Insanity_UI_Interaction_Event;

typedef struct Insanity_UI_Interaction_Result
{
    bool hovered;
    bool clicked;
    bool dragged;
    bool scrolled;
    bool float_change;
    float float_value;
} Insanity_UI_Interaction_Result;

typedef enum Insanity_UI_Sizing
{
    // if a parent takes up percentage of the screen,
    // if a child takes up percentage of the parent
    Insanity_UI_SIZING_PERCENT, //(default)
    // parent -> grows exactly to the size of its children
    // child -> takes up the rest of the parent space
    Insanity_UI_SIZING_EXPAND,
    //exact pizel size, regardless of screen size
    Insanity_UI_SIZING_PIXEL,
} Insanity_UI_Sizing;

typedef enum Insanity_UI_Layout_Direction
{
    Insanity_UI_LAYOUT_VERTICAL,
    Insanity_UI_LAYOUT_HORIZONTAL,
} Insanity_UI_Layout;


typedef struct Insanity_UI_Padding
{
    //NOTE: 0-1 range because we are basing padding on percent
    //not using a vec4 cause its not clear what the values are supposed to be
    float left;
    float right;
    float top;
    float bottom;
} Insanity_UI_Padding;


typedef struct Insanity_UI_Editor_Style
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
} Insanity_UI_Editor_Style;


//TODO:  just a temporary value for now, will increase later
#define MAX_INSANITY_UI_NODE_COUNT 1000
#define MAX_Insanity_UI_TEXT_NODE_COUNT 1000
#define MAX_Insanity_UI_NODE_CHILD_COUNT 10

typedef struct Insanity_UI_Node
{
    Insanity_UI_Property_Flags ui_flags;

    // screen size and pos, not normalized
    vec2 pos;
    vec2 size;
    float rotation; // degrees, but gets converted to radians at draw time

    //rounded
    float rounded_radius; // 0-1 range

    //outline
    vec3 outline_color;
    float outline_thickness; // 0-1 :: ideally should be something small like 0.05-0.1

    //for circles
    float thickness; // size of the circle is determined by the pos and size

    const char* id;
    u64 hash_id;

    //draw data
    //consider here what actually needs to be done for something to rendered, instead of passing in the entire config
    Texture_Handle texture_handle;
    vec2 uv_offset;
    vec2 uv_size;

    //colors
    vec3 color;
    vec3 background_color;

    //TODO: for widgets that need some sort of child node, like a scroll box
    struct Insanity_UI_Node* parent;
    struct Insanity_UI_Node* children[INSANITY_MAX_UI_NODE_CHILD_COUNT];
    u32 child_count;

    Insanity_UI_Layout layout;
    Insanity_UI_Sizing sizing;
    Insanity_UI_Padding padding;

} Insanity_UI_Node;

typedef struct Insanity_UI_Node_Text
{
    Insanity_UI_Property_Flags flags;
    char character;
    vec2 pos;
    vec2 size;

    vec3 color;
    Texture_Handle texture_handle;

    // offset into a texture atlas if using one, otherwise {0, 0}
    vec2 uv_offset;
    // start from offset and this will give us our bottom right uv, which tells us all the other info we need
    vec2 uv_size;

    // is the first character in a series of strings
    bool start_text;
} Insanity_UI_Node_Text;


ARRAY_GENERATE_TYPE(Insanity_UI_Node)
ARRAY_GENERATE_TYPE(Insanity_UI_Node_Text)

typedef struct Insanity_UI_Node_Draw_Data
{
    Insanity_UI_Property_Flags ui_flags;

    // screen size and pos, not normalized
    vec2 pos;
    vec2 size;
    float rotation; // degrees, but gets converted to radians at draw time

    //rounded
    float rounded_radius; // 0-1 range

    //outline
    vec3 outline_color;
    float outline_thickness; // 0-1 :: ideally should be something small like 0.05-0.1


    //for circles
    float thickness; // size of the circle is determined by the pos and size

    //draw data
    //consider here what actually needs to be done for something to rendered, instead of passing in the entire config
    u32 texture_handle;
    vec2 uv_offset;
    vec2 uv_size;

    //colors
    vec3 color;
    vec3 background_color;
} Insanity_UI_Node_Draw_Data;


typedef struct Insanity_UI_Render_Packet
{
    Insanity_UI_Node_Draw_Data* ui_data;
    u64 ui_data_size;
    u64 ui_data_bytes;

    Insanity_UI_Node_Draw_Data* text_data;
    u64 text_data_size;
    u64 text_data_bytes;
} Insanity_UI_Render_Packet;


//rn this is purely a ui for the editor, in game ui is for another time, when the game comes along
typedef struct Insanity_UI
{
    Arena* arena; // rn mainly just for loading fonts, would be better as a pool arena
    Frame_Arena* frame_arena;
    Memory_Tracker* mem_tracker;

    Input_System* input_system_reference; // does not own memory
    Resource_System* resource_system;

    //this should be an array at some point
    Texture_Handle default_font_handle;
    float default_font_size;
    float editor_font_size;
    // Font fonts[100];

    Insanity_UI_Node_array* ui_nodes;
    Insanity_UI_Node_Text_array* ui_nodes_text;
    // UI_Node_array* button_nodes; //TODO: we will see

    Insanity_UI_Node* ui_stack[100];
    u32 ui_stack_count;


    Insanity_UI_Node_Draw_Data* node_draw_data_array;
    u64 node_draw_data_array_size;

    Insanity_UI_Node_Draw_Data* text_draw_data_array;
    u64 text_draw_data_array_size;


    int hot;
    int active;

    //
    bool mouse_down;
    bool mouse_released_unique;
    i16 mouse_pos_x;
    i16 mouse_pos_y;
    i16 mouse_delta_x;
    i16 mouse_delta_y;

    //for input key
    char first_released_key;

    //TODO: keep track if backspace has been held down for a certain period of time
    // for the backspace functionality of the textbox


    vec2 screen_size; // this gets queried every frame

    Insanity_UI_Editor_Style editor_style;

    //Persistaent states
    hash_table* drag_state;
    hash_table* float_state;
    hash_table* text_box_state; // hash table -> string_buidler


    //STACKS
    stack* pos_stack;
    stack* size_stack;

    stack* layout_stack;
    stack* sizing_type_stack;

    stack* style_stack;
    stack* flag_stack;


    String string_stack;
    Texture_Handle image_stack;
    float rounded_radius_stack;
    float outline_thickness_stack;
    Insanity_UI_Alignment text_alignment_stack;

    stack* float_stack;
    float increment_value_stack;
} Insanity_UI;


MAPI bool insanity_ui_init(Memory_System* memory_system, Input_System* input_system,
                           Resource_System* resource_system);


MAPI bool insanity_ui_shutdown(void);

//pass in the size every frame, in the event the size changes
MAPI void insanity_ui_begin(i32 screen_size_x, i32 screen_size_y);


//Note: needs to be called right before the renderers update method, to generate the appropriate render data
MAPI void insanity_ui_end(Resource_System* resource_system);

MAPI Insanity_UI_Render_Packet insanity_get_render_data();


//part of the ui end function
MAPI void insanity_ui_passes();
MAPI void insanity_ui_generate_draw(void);


//API

MAPI Insanity_UI_Interaction_Result insanity_ui_draw_rect(const char* id);
MAPI void insanity_ui_text();


// void insanity_ui_layout_push(Insanity_UI_Layout layout);

void insanity_ui_push_parent(const char* id);
void insanity_ui_pop_parent(void);


//style stuff

//TODO: have the stack never go below 0

MAPI void insanity_ui_push_flags(Insanity_UI_Property_Flags flags);
MAPI void insanity_ui_push_pos(vec2 pos);
MAPI void insanity_ui_push_size(vec2 size);
MAPI void insanity_ui_push_sizing_type(Insanity_UI_Sizing sizing_type);

MAPI void insanity_ui_push_layout(Insanity_UI_Layout layout);
MAPI void insanity_ui_pop_layout(void);

MAPI void insanity_ui_push_text(String text);
MAPI void insanity_ui_push_text_float(float val);
MAPI void insanity_ui_push_image(const char* texture_file);

MAPI Insanity_UI_Property_Flags insanity_ui_get_flags();
MAPI Texture_Handle insanity_ui_get_image(void);


//Utility
MAPI Insanity_UI_Node* insanity_ui_get_new_node();
MAPI Insanity_UI_Node_Text* insanity_ui_get_new_node_text();
MAPI Insanity_UI_Node* insanity_ui_get_parent_node();


MAPI bool insanity_rect_hit(vec2 pos, vec2 size);
MAPI char* insanity_ui_float_to_char(float value);

//Test
MAPI void insanity_ui_test();


#endif //INSANITY_UI_H
