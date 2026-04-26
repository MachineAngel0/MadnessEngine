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
// float -> box
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
    Insanity_UI_ALIGNMENT_TOP,
    Insanity_UI_ALIGNMENT_BOTTOM,
    Insanity_UI_ALIGNMENT_TOP_LEFT,
    Insanity_UI_ALIGNMENT_TOP_RIGHT,
    Insanity_UI_ALIGNMENT_BOTTOM_LEFT,
    Insanity_UI_ALIGNMENT_BOTTOM_RIGHT,
    Insanity_UI_ALIGNMENT_MAX,
} Insanity_UI_Alignment;


typedef enum Insanity_UI_Property_Flags
{
    UI_TYPE_NONE = BITFLAG(0),
    UI_TYPE_BACKGROUND = BITFLAG(1),
    UI_TYPE_CLICKABLE = BITFLAG(2),
    UI_TYPE_IMAGE = BITFLAG(3),
    UI_TYPE_TEXT = BITFLAG(4),
    UI_TYPE_OUTLINE = BITFLAG(5),
    UI_TYPE_SCROLL = BITFLAG(6),
    UI_TYPE_COLOR = BITFLAG(7),
    UI_TYPE_DRAGGABLE = BITFLAG(8),
    UI_TYPE_ROUND_CORNER = BITFLAG(9),
    UI_TYPE_CIRCLE = BITFLAG(10),
} Insanity_UI_Property_Flags;

typedef enum Insanity_UI_Interaction_Event
{
    UI_EVENT_HOVER = BITFLAG(0),
    UI_EVENT_CLICK = BITFLAG(1),
    UI_EVENT_DRAG = BITFLAG(2),
    UI_EVENT_SCROLL = BITFLAG(3),
    // UI_EVENT_TEXT = BITFLAG(4),
    // UI_EVENT_OUTLINE = BITFLAG(5),
    // UI_EVENT_COLOR = BITFLAG(6),
} Insanity_UI_Interaction_Event ;



typedef enum Insanity_UI_Sizing
{
    // percentage of the screen
    Insanity_UI_SIZING_PERCENT,
    //percentage of the parent
    Insanity_UI_SIZING_PARENT_PERCENT,
    // takes up space of its children or if it is a child, as much of the parent as possible
    Insanity_UI_SIZING_EXPAND,

    Insanity_UI_SIZING_MAX,
} Insanity_UI_Sizing;

typedef enum Insanity_UI_Layout_Direction
{
    Insanity_UI_LAYOUT_HORIZONTAL,
    Insanity_UI_LAYOUT_VERTICAL,
} Insanity_UI_Layout_Direction;

typedef struct Insanity_UI_Padding
{
    //not using a vec4 cause its not clear what the values are suppose to be
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

typedef struct Insanity_UI_Circle
{
    float radius;
    float donut_hole; // 1 is full, the rest create the hole in the middle
} Insanity_UI_Circle;

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

    const char* debug_id;
    u64 hash_id;

    //draw data
    //consider here what actually needs to be done for something to rendered, instead of passing in the entire config
    Texture_Handle texture_handle;
    vec2 uv_offset;
    vec2 uv_size;


    //TODO: for widgets that need some sort of child node, like a scroll box
    struct Insanity_UI_Node* parent;
    struct Insanity_UI_Node* children[INSANITY_MAX_UI_NODE_CHILD_COUNT];
    u32 child_node_count;



    //colors
    vec3 color;
    vec3 background_color;
} Insanity_UI_Node;

typedef struct Insanity_UI_Node_Text
{
    char character;
    vec2 pos;
    vec2 size;

    vec3 color;
    Texture_Handle texture_handle;
    Sprite_Flags flags;

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
}Insanity_UI_Node_Draw_Data;



typedef struct Insanity_UI_Render_Packet{
    Insanity_UI_Node_Draw_Data* ui_nodes;
    u64 ui_nodes_size;
    u64 ui_nodes_bytes;

    Insanity_UI_Node_Text_array* ui_nodes_text;


    // Insanity_UI_Node_Text_Draw_Data* ui_nodes_text;
    // u64 ui_nodes_text_size;
    // u64 ui_nodes_text_bytes;
}Insanity_UI_Render_Packet;


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

    Insanity_UI_Node_Draw_Data* node_draw_data_array;
    u64 node_draw_data_array_size;


    int hot;
    int active;

    bool mouse_down;
    bool mouse_released_unique;
    i16 mouse_pos_x;
    i16 mouse_pos_y;
    i16 mouse_delta_x;
    i16 mouse_delta_y;

    //TODO: keep track if backspace has been held down for a certain period of time
    // for the backspace functionality of the textbox

    char released_key;

    String_Builder* string_builder;

    vec2 screen_size; // this gets queried every frame

    Insanity_UI_Editor_Style editor_style;

    Sprite_Data_array* text_data;

    bool layout;

    stack* sizing_stack;
    stack* style_stack;
    stack* flag_stack;
    stack* pos_stack;

    hash_table* drag_state;

    String string_stack;
    Texture_Handle image_stack;
    float rounded_radius_stack;
    float outline_thickness_stack;


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
MAPI void insanity_ui_generate_draw(Resource_System* resource_system);


//API

MAPI void insanity_ui_draw_rect(const char* id);
MAPI void insanity_ui_text();



void insanity_ui_layout_start();
void insanity_ui_layout_end();


//style stuff

MAPI void insanity_ui_set_flags(Insanity_UI_Property_Flags flags);
MAPI Insanity_UI_Property_Flags insanity_ui_get_flags();
MAPI void insanity_ui_set_pos(vec2 pos);
MAPI void insanity_ui_set_text(String text);

MAPI void insanity_ui_set_image(const char* texture_file);
MAPI Texture_Handle insanity_ui_get_image(void);


//Utility
MAPI Insanity_UI_Node* insanity_ui_get_new_node();
MAPI Insanity_UI_Node_Text* insanity_ui_get_new_node_text();
MAPI Insanity_UI_Node* insanity_ui_get_parent_node();

MAPI bool insanity_rect_hit(vec2 pos, vec2 size);

//Test
MAPI void insanity_ui_test();


#endif //INSANITY_UI_H
