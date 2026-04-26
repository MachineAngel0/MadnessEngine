#ifndef UI_H
#define UI_H

#include "resource_types.h"

//IMMEDIATE MODE UI

//NOTE: the smallest likely screen size for a laptop is likely to be a 1024*600 or more commonly 1024 * 768
// smalles ill let it be is 600*600

//Vertex Data


typedef struct PC_2D
{
    u32 ubo_buffer_idx;
    u32 material_buffer_idx;
} PC_2D;

typedef struct Material_2D_Param_Data
{
    u32 feature_mask;
    u32 texture_index;
} Material_2D_Param_Data;

#define DEFAULT_FONT_SIZE 32.0f
#define EDITOR_FONT_SIZE 24.0f





typedef enum UI_Layout_Direction
{
    UI_LAYOUT_HORIZONTAL,
    UI_LAYOUT_VERTICAL,
} UI_Layout_Direction;




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



//TODO:  just a temporary value for now, will increase later
#define MAX_UI_NODE_COUNT 1000
#define MAX_UI_TEXT_NODE_COUNT 1000
#define MAX_UI_NODE_CHILD_COUNT 10

struct UI_Circle
{
    float radius;
    float donut_hole; // 1 is full, the rest create the hole in the middle
};

typedef struct UI_Node
{
    // screen size and pos, not normalized
    vec2 pos;
    vec2 size;
    float rotation; // degrees, but gets converted to radians at draw time

    //for circles
    float thickness;

    const char* debug_id;
    u64 hash_id;

    //draw data
    //consider here what actually needs to be done for something to rendered, instead of passing in the entire config
    vec3 color;
    Texture_Handle texture_handle;
    Sprite_Flags flags;


    //TODO: for widgets that need some sort of child node, like a scroll box
    struct UI_Node* parent;
    struct UI_Node* children[10];
    u32 child_node_count;
} UI_Node;

typedef struct UI_Node_Text
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
} UI_Node_Text;


ARRAY_GENERATE_TYPE(UI_Node)
ARRAY_GENERATE_TYPE(UI_Node_Text)


typedef struct scroll_box_state
{
    u32 max_nodes_to_display; // user sets info

    //internal
    u32 scroll_amount;
} scroll_box_state;

//this could be more general
typedef struct active_scroll_box_state
{
    //UI_Node_Type kind;

    bool is_active;

    //scroll box
    u32 slider_count; // at what point an element is allowed to be rendered
    u32 current_attempt_count; // how many up to this point have tried to render
    u32 max_nodes;
} Parent_Node_State;


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
    Memory_Tracker* mem_tracker;

    Input_System* input_system_reference; // does not own memory
    Resource_System* resource_system;

    //this should be an array at some point
    Texture_Handle default_font_handle;
    float default_font_size;
    float editor_font_size;
    // Font fonts[100];

    UI_Node_array* ui_nodes;
    UI_Node_Text_array* ui_nodes_text;
    // UI_Node_array* button_nodes; //TODO: we will see

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

    //stores id and the ui state they are in the previous frame, if applicable
    hash_table* button_hash_states; // only for buttons
    hash_table* check_box_states; // only for checkboxes

    Parent_Node_State parent_node_state;

    //Keep an array of strings used in textboxes
    //should be an array at some point,
    String_Builder* string_builder;
    hash_table* textbox_ids; //maps textbox names to their id in the string builder array

    vec2 screen_size; // this gets queried every frame


    //TODO: the 100 is temporary
    Sprite_Data_array* ui_data;
    Sprite_Data_array* text_data;


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


    //Material Node
    bool input_pressed;
    u32 input_pressed_id;
    vec2 input_pos;
    bool output_pressed;
    u32 output_pressed_id;



} Madness_UI;


// FUNCTION POINTERS //
typedef Madness_UI* (*UI_init)(Memory_System* memory_system, Input_System* input_system,
                               Resource_System* resource_system);
typedef bool (*UI_shutdown)(Madness_UI* madness_ui);
typedef void (*UI_begin)(Madness_UI* madness_ui, i32 screen_size_x, i32 screen_size_y);
typedef void (*UI_end)(Madness_UI* madness_ui, Resource_System* resource_system);

MAPI Madness_UI* madness_ui_init(Memory_System* memory_system, Input_System* input_system,
                                 Resource_System* resource_system);
MAPI bool madness_ui_shutdown(Madness_UI* madness_ui);

//pass in the size every frame, in the event the size changes
MAPI void madness_ui_begin(Madness_UI* madness_ui, i32 screen_size_x, i32 screen_size_y);
//Note: needs to be called right before the renderers update method, to generate the appropriate render data
MAPI void madness_ui_end(Madness_UI* madness_ui, Resource_System* resource_system);

//API START (besides init/shutdown, begin/end)
//TODO: drag the layout around
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

MAPI bool madness_ui_float(Madness_UI* madness_ui, const char* id, float* f, float increment_value);
MAPI bool madness_ui_vec2(Madness_UI* madness_ui, const char* id, String text, vec2* v2, float increment_value);
MAPI bool madness_ui_vec3(Madness_UI* madness_ui, const char* id, String text, vec3* v3, float increment_value);


MAPI bool madness_ui_color_picker(Madness_UI* madness_ui, const char* id, vec3* color_value);

MAPI void madness_scroll_box_begin(Madness_UI* madness_ui, const char* id, scroll_box_state* scroll_box_state);
MAPI void madness_scroll_box_end(Madness_UI* madness_ui, const char* id, scroll_box_state* scroll_box_state);

MAPI bool madness_ui_circle(Madness_UI* madness_ui, const char* id, float* thickness);


typedef struct Material_Link
{
    //nodes that we are connecting to
    u32 from_id;
    u32 to_id;
    //inputs and output links
    u8 from_node;
    u8 to_node;
} Material_Link;


typedef struct Material_Node
{
    u32 node_id;

    String* inputs;
    vec2* inputs_positions;
    Material_Link* inputs_links;
    int input_size;

    String* outputs;
    vec2* output_positions;
    Material_Link* output_links;
    int output_size;
} Material_Node;

// simple version
//press on an node (at the draw location), and store that state,
//then if we press on another node, we take the data, do some validation and connect the two by drawing a line
//the ui node function, will handle look at the connections, if there are any and do the drawing of them

// complex version
// if we press on a node, we set the state to active, then when we let go, if were hovering over another node, we connect the two,
// and like previosly, if there is a connection, the node itself will handle adding an additional function call
// might need some additional state, just cause, the node draw order could be random,
// and we will have no idea, if on a release, we are hovering over another node

MAPI bool madness_ui_node_simple(Madness_UI* madness_ui, const char* id, vec2 pos, String inputs[], u8 input_size,
                                 String outputs[], u8 output_size, u32 node_id);

MAPI bool madness_ui_node_complex(Madness_UI* madness_ui, const char* id, String inputs[], u8 input_size, String outputs[],
                          u8 output_size);


MAPI bool madness_ui_node(Madness_UI* madness_ui, const char* id, String inputs[], u8 input_size, String outputs[],
                          u8 output_size);

MAPI bool madness_ui_drag_test(Madness_UI* madness_ui, vec2* pos);


MAPI bool madness_ui_quadratic_bezier(Madness_UI* madness_ui, vec2* pos1, vec2* pos2, vec2* pos3);
MAPI bool madness_ui_cubic_bezier(Madness_UI* madness_ui, vec2* pos1, vec2* pos2, vec2* pos3, vec2* pos4);



//API END


//these is only meant for internal use and not part of the API
MAPI void madness_draw_quad(Madness_UI* madness_ui, const char* id, vec2* out_pos, vec2* out_size, UI_Node** out_node);
MAPI void madness_draw_text(Madness_UI* madness_ui, String text, vec2 screen_position);
MAPI void madness_draw_text_centered(Madness_UI* madness_ui, String text, vec2 parent_pos, vec2 parent_size);
MAPI void madness_calculate_text_size(Madness_UI* madness_ui, String text, vec2 screen_position, vec2* out_text_size);


MAPI bool skip_node(Madness_UI* madness_ui);


//debug and test
MAPI void madness_ui_print_state(Madness_UI* madness_ui);

MAPI void madness_ui_test(Madness_UI* madness_ui);


//utility
MAPI UI_Node* madness_ui_get_new_node(Madness_UI* madness_ui);
MAPI UI_Node* madness_ui_get_parent_node(Madness_UI* madness_ui);
MAPI UI_Node_Text* madness_ui_get_new_node_text(Madness_UI* madness_ui);
MAPI void madness_ui_update_next_element_pos(Madness_UI* madness_ui, vec2 ui_screen_size);

MAPI void madness_ui_center_child_node(vec2 parent_pos, vec2 parent_size, vec2 child_size, vec2* out_pos);
MAPI char* madness_ui_float_to_char(Madness_UI* madness_ui, float value);


MAPI bool is_ui_hot(Madness_UI* madness_ui, int id);

MAPI bool is_ui_active(Madness_UI* madness_ui, int id);

MAPI bool region_hit(Madness_UI* madness_ui, vec2 pos, vec2 size);

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

//check if we can use the button
MAPI bool use_ui_element(Madness_UI* madness_ui, int id, vec2 pos, vec2 size);

MAPI int generate_id(Madness_UI* madness_ui);

MAPI void set_hot(Madness_UI* madness_ui, int id);

MAPI void set_active(Madness_UI* madness_ui, int id);

MAPI bool can_be_active(Madness_UI* madness_ui);
MAPI bool is_active(Madness_UI* madness_ui, int id);

MAPI bool is_hot(Madness_UI* madness_ui, int id);











#endif //UI_H
