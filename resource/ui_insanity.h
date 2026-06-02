#ifndef INSANITY_UI_H
#define INSANITY_UI_H

#include "allocator.h"
#include "stack.h"
#include "ui_types.h"

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
#define INSANITY_MAX_UI_NODE_CHILD_COUNT 100

//UI


typedef struct Insanity_UI_Interaction_Result
{
    bool hovered;
    bool clicked;
    bool dragged;
    bool scrolled;
    bool float_change;
    float float_value;
} Insanity_UI_Interaction_Result;

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


typedef struct Insanity_UI_Node
{
    UI_Property_Flags ui_flags;

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

    Texture_Handle texture_handle;
    vec2 uv_offset;
    vec2 uv_size;
    //Text
    String text; // will also need the texture handle
    float text_total_width;
    float text_max_height;
    Texture_Handle font_handle;

    //colors
    vec3 color;
    vec3 background_color;

    //TODO: for widgets that need some sort of child node, like a scroll box
    struct Insanity_UI_Node* parent;
    struct Insanity_UI_Node* children[INSANITY_MAX_UI_NODE_CHILD_COUNT];
    u32 child_count;

    vec2 padding;
    vec2 child_padding;
    Insanity_UI_Layout layout;


} Insanity_UI_Node;



ARRAY_GENERATE_TYPE(Insanity_UI_Node)







//rn this is purely a ui for the editor, in game ui is for another time, when the game comes along
typedef struct Insanity_UI
{
    Allocator* allocator; // rn mainly just for loading fonts, would be better as a pool arena
    Frame_Allocator* frame_allocator;
    Memory_Tracker* mem_tracker;

    Input_System* input_system_reference; // does not own memory
    Resource_System* resource_system;

    //this should be an array at some point
    Texture_Handle default_font_handle;
    float default_font_size;
    float editor_font_size;
    // Font fonts[100];

    Insanity_UI_Node_array* ui_nodes;
    // UI_Node_array* button_nodes; //TODO: we will see

    Insanity_UI_Node* ui_stack[100];
    u32 ui_stack_count;


    UI_Node_Draw_Data* node_draw_data_array;
    u64 node_draw_data_array_size;

    UI_Node_Draw_Data* text_draw_data_array;
    u64 text_draw_data_array_size;


    int hot;
    int active;

    //
    bool mouse_down;
    bool mouse_released_unique;
    s16 mouse_pos_x;
    s16 mouse_pos_y;
    s16 mouse_delta_x;
    s16 mouse_delta_y;

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
    stack* padding_stack;


    stack* style_stack;
    stack* flag_stack;


    String string_stack;
    Texture_Handle image_stack;
    float rounded_radius_stack;
    float outline_thickness_stack;
    UI_Alignment text_alignment_stack;

    stack* float_stack;
    float increment_value_stack;
} Insanity_UI;


MAPI bool insanity_ui_init(Memory_System* memory_system, Input_System* input_system,
                           Resource_System* resource_system);


MAPI bool insanity_ui_shutdown(void);

//pass in the size every frame, in the event the size changes
MAPI void insanity_ui_begin(s32 screen_size_x, s32 screen_size_y);


//Note: needs to be called right before the renderers update method, to generate the appropriate render data
MAPI void insanity_ui_end(void);

MAPI UI_Render_Packet insanity_get_render_data();


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

MAPI void insanity_ui_push_flags(UI_Property_Flags flags);
MAPI void insanity_ui_push_pos(vec2 pos);
MAPI void insanity_ui_push_size(vec2 size);


MAPI void insanity_ui_push_layout(Insanity_UI_Layout layout);
MAPI void insanity_ui_pop_layout(void);

//TODO:
MAPI void insanity_ui_push_loop_count(void);
MAPI void insanity_ui_pop_loop_count(void);


MAPI void insanity_ui_push_padding(vec2 padding);

MAPI void insanity_ui_push_image(const char* texture_file);

//Text related params
//NOTE: size does not affect ui
//TODO: push formatted text
MAPI void insanity_ui_push_text(String text);
MAPI void insanity_ui_push_text_float(float val);
// MAPI void insanity_ui_push_font_image();
// MAPI void insanity_ui_push_text_alignment();
// MAPI void insanity_ui_push_font_size();



MAPI UI_Property_Flags insanity_ui_get_flags();
MAPI Texture_Handle insanity_ui_get_image(void);


//Utility
MAPI Insanity_UI_Node* insanity_ui_get_new_node();
MAPI Insanity_UI_Node* insanity_ui_get_parent_node();
MAPI Insanity_UI_Node* insanity_ui_get_top_node();


MAPI bool insanity_rect_hit(vec2 pos, vec2 size);
MAPI char* insanity_ui_float_to_char(float value);

//Test
MAPI void insanity_ui_test();


#endif //INSANITY_UI_H
