#ifndef CHAOS_SE_UI_H
#define CHAOS_SE_UI_H


#include "defines.h"



//NOTE: an auto layout ui, that i dont really like using that much, but its here in case I do want to use it

////// Defines and Constants //////

// #define INSANITY_UI_DEBUG_PRINT


#define CHAOS_DEFAULT_FONT_SIZE 32.0f
#define CHAOS_EDITOR_FONT_SIZE 16.0f
#define CHAOS_TEXT_OUTLINE 0.5f

#define CHAOS_UI_MAX_NODE_COUNT 1000
#define CHAOS_UI_MAX_WINDOW_COUNT 100
#define CHAOS_MAX_UI_NODE_CHILD_COUNT 32


////// RENDER //////
typedef enum CUI_Render_Command_Type
{
    Chaos_UI_Render_Command_Type_Draw,
    Chaos_UI_Render_Command_Type_Scissor,
} CUI_Render_Command_Type;

typedef enum CUI_Render_Type
{
    CUI_Render_Flags_Quad,
    CUI_Render_Flags_Text,
    CUI_Render_Flags_Image,
    CUI_Render_Flags_Bezier,
    CUI_Render_Flags_Scissor,
} CUI_Render_Type;

typedef struct CUI_Render_Item
{
    CUI_Render_Type render_type;

    //the position and size double up as the scissor position and size
    vec2s pos;
    vec2s size;
    float rotation;

    //rounded rect
    float rounded_corners; // maybe a vec4 at some point

    //text
    u32 texture_id;
    vec2s uv_offset;
    vec2s uv_size;


    //circle
    float radius;


    vec4s color;

    float outline_thickness;
    vec4s outline_color;

    // vec4s gradient[4]; // this would replace all the color param
} CUI_Render_Item;


typedef struct Chaos_UI_Render_Command
{
    u32 draw_count;
    u32 buffer_offset; // offset in counts
    CUI_Render_Command_Type type;
} Chaos_UI_Render_Command;


typedef struct Chaos_UI_Render_Packet
{
    Chaos_UI_Render_Command* render_command_array;
    u32 render_command_count;

    //arrays / buffers
    CUI_Render_Item* render_items;
    u32 render_items_count;
    u32 render_items_byte_size;

} Chaos_UI_Render_Packet;


////// UI NODE //////

typedef enum CUI_Input_Flags
{
    CUI_Property_Flags_0 = BITFLAG(0),
} CUI_Property_Flags;


typedef enum CUI_Scroll_Flags
{
    CUI_Scroll_Flags_0 = BITFLAG(0),
    CUI_Scroll_Flags_1 = BITFLAG(1),
    CUI_Scroll_Flags_2 = BITFLAG(2),
    CUI_Scroll_Flags_3 = BITFLAG(3),
    CUI_Scroll_Flags_4 = BITFLAG(4),

    CUI_Scroll_Flags_31 = BITFLAG(31),
} CUI_Scroll_Flags;


typedef struct CUI_Scroll
{
    CUI_Scroll_Flags flags;

    f32 y_scroll;

    f32 viewport_width;
    f32 viewport_height;

    f32 content_width;
    f32 content_height;

    f32 max_scroll;

    float scroll_speed;

    vec2s last_frame_viewport_dimensions;

    struct CUI_Node* scroll_node;

} CUI_Scroll;

typedef enum CUI_Sizing
{
    CUI_Sizing_Fixed,
    CUI_Sizing_Grow, // sizes to take up remaining space
    CUI_Sizing_Fit, //sizes to content
    CUI_Sizing_Percent,
} CUI_Sizing;

typedef enum CUI_Alignment
{
    CUI_ALIGNMENT_TOP_LEFT,
    CUI_ALIGNMENT_CENTER,
    CUI_ALIGNMENT_BOTTOM_RIGHT,
} CUI_Alignment;

typedef enum CUI_Text_Wrap
{
    CUI_Text_Wrap_None,
    CUI_Text_Wrap_Wrap,
    CUI_Text_Wrap_Newline,
} CUI_Text_Wrap;


typedef enum CUI_Layout_Direction
{
    CUI_Layout_Horizontal,
    CUI_Layout_Vertical,
} CUI_Layout_Direction;

typedef struct CUI_Padding
{
    u32 left;
    u32 right;
    u32 top;
    u32 bottom;
} CUI_Padding;


typedef struct Chaos_Node
{
    const char* name_id;
    u32 hash_id;
    CUI_Render_Type render_type;

    vec2s pos; // relative (until layout step)
    vec2s size;
    float rotation;

    vec2s max_size;
    vec2s min_size;

    float view_offset; //position offset by any scroll views


    CUI_Padding padding;
    f32 child_padding; // for all directions

    CUI_Sizing sizing_type_x; // fixed, percent, auto
    CUI_Sizing sizing_type_y; // fixed, percent, auto
    CUI_Alignment x_alignment;
    CUI_Alignment y_alignment;
    CUI_Layout_Direction layout_direction;


    //basic render properties
    vec4s color;

    vec4s outline_color;
    float outline_thickness;


    //rounded rect
    float thickness;
    float rounded_radius;


    //text
    String* text;
    u16 font_size;
    u16 letter_spacing;
    CUI_Text_Wrap text_wrap;
    //vec2 max_wrap_width;

    //image
    u32 texture_handle;
    vec2s uv_offset;
    vec2s uv_size;


    //scissor
    vec2s scissor_pos;
    vec2s scissor_size;

    struct Chaos_Node* parent;
    struct Chaos_Node* child[CHAOS_MAX_UI_NODE_CHILD_COUNT];
    u8 child_count;
} CUI_Node;


typedef struct CUI_Default_Style
{
    vec4s layout_color;
    vec4s layout_accent_color;
    vec4s text_color;

    vec4s textbox_color;
    vec4s custom_widget_color;
    // colors for things like buttons and checkboxes

    vec4s color;

    vec4s hovered_color;
    vec4s pressed_color;

    vec4s outline_color;

    vec4s permanent_active;

    vec4s header_color;
    vec4s pop_up_color;

    vec4s scrollbox_color;

    vec4s error_color;
} CUI_Default_Style;




#endif
