#ifndef UI_H
#define UI_H

#include "resource_types.h"

//IMMEDIATE MODE UI

//NOTE: the smallest likely screen size for a laptop is likely to be a 1024*600 or more commonly 1024 * 768
// smalles ill let it be is 600*600

//Vertex Data


typedef struct PC_UI
{
    u32 ubo_buffer_idx;
    VkDeviceAddress material_bda;
} PC_UI;

typedef struct Material_2D_Param_Data
{
    u32 feature_mask;
    u32 texture_index;
} Material_2D_Param_Data;

#define EDITOR_FONT_SIZE 16.0f


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

    vec3 permanent_active;

    vec3 header_color;
    vec3 pop_up_color;



} UI_Editor_Style;


//TODO:  just a temporary value for now, will increase later
#define MAX_UI_NODE_COUNT 1000
#define MAX_UI_TEXT_NODE_COUNT 1000
#define MAX_UI_NODE_CHILD_COUNT 10
#define MAX_UI_DRAW_COUNT 500

#define MIN_UI_NODE_SCREEN_SIZE 250


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

    //outline
    vec3 outline_color;
    float outline_thickness; // 0-1 :: ideally should be something small like 0.05-0.1

    String string_id;
    u64 hash_id;

    //Text
    String text;
    char character;

    //draw data
    //consider here what actually needs to be done for something to rendered, instead of passing in the entire config
    vec3 color;
    UI_Property_Flags flags;

    Texture_Handle texture_handle;
    // offset into a texture atlas if using one, otherwise {0, 0}
    vec2 uv_offset;
    // start from offset and this will give us our bottom right uv, which tells us all the other info we need
    vec2 uv_size;

    vec2 scissor_pos;
    vec2 scissor_size;
} UI_Node;



ARRAY_GENERATE_TYPE(UI_Node)


typedef struct scroll_box_state
{
    u32 max_nodes_to_display; // user sets info

    //internal
    u32 scroll_amount;
} scroll_box_state;


typedef enum UI_Window_Type
{
    UI_WINDOW_TYPE_NONE,
    UI_WINDOW_TYPE_WINDOW,
    UI_WINDOW_TYPE_SCROLLBAR,
    UI_WINDOW_TYPE_POPUP,
    UI_WINDOW_TYPE_MAX,
} UI_Window_Type;



typedef struct Window_State
{
    String window_name;
    UI_Window_Type window_type;

    vec2 window_region_pos;
    vec2 window_region_size;

    // only for actual windows and not scroll boxes
    // should be used as an offset to get to the proper scroll region
    //NOTE:  the header position is at the same spot where the window starts01
    vec2 header_size;

    float scroll_offset; // should ideally be in a range of size, and then we increment the size by that
    float scroll_bar_percent_offset; // should ideally be in a range of size, and then we increment the size by that
} Window_State;

typedef struct Pop_Up_State
{
    String pop_up_name;

    vec2 cursor_original_pos;
    vec2 pop_up_pos;
    vec2 pop_up_size;

    UI_Node* pop_up_node;
    UI_Node* pop_up_scissor_start_node;



} Pop_Up_State;


typedef struct Menu_Bar_State
{
    vec2 menu_bar_pos;
    vec2 menu_bar_size;

    vec2 menu_cursor_position;

    String active_menu_item;

} Menu_Bar_State;


//meant to be used as an editor only UI, made for simplicity and fast iteration
typedef struct Madness_UI
{
    Allocator* allocator; // rn mainly just for loading fonts, would be better as a pool arena
    Frame_Allocator* frame_arena;
    Memory_Tracker* mem_tracker;

    Input_System* input_system_reference; // does not own memory
    Resource_System* resource_system;

    //this should be an array at some point
    Texture_Handle default_font_handle;
    float default_font_size;
    float editor_font_size;

    UI_Node_array* ui_nodes;

    UI_Node_array* pop_up_ui_nodes; // pop up nodes


    //DRAW DATA //
    UI_Node_Draw_Data* ui_draw_data;
    u64 ui_draw_data_count;



    //a window is anything with which things are drawn to inside of it
    HASH_TABLE_STR_TYPE(Window_State)* window_state_hash;
    STACK_TYPE(Window_State)* window_states_stack;

    STACK_TYPE(Pop_Up_State)* pop_up_stack;
    ARRAY_TYPE(Pop_Up_State)* pop_up_frame_state;
    bool nuke_pop_up;

    STACK_TYPE(vec2)* window_pos_stack;
    STACK_TYPE(vec2)* window_size_stack;

    // HASH_TABLE_STR_TYPE(String_Builder*)* text_box_states; // TODO:

    String active_combo_box;
    Menu_Bar_State menu_bar_state;



    // Mouse/Key STATE // // TODO: gamepad and proper keyboard navigation

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


    //Keep an array of strings used in textboxes
    //should be an array at some point,
    String_Builder* string_builder;
    hash_table* textbox_ids; //maps textbox names to their id in the string builder array

    vec2 screen_size; // this gets queried every frame


    //new UI

    UI_Editor_Style editor_style;

    vec2 current_window_screen_pos; // converted pos of current layout
    vec2 current_window_screen_size; // converted size of current layout


    vec2 cursor_pos;
    vec2 prev_line;
    vec2 prev_item_size;
    // bool same_line;

    float element_padding_x; // space between each ui element, when using same line
    float element_padding_y; // space between each ui element, when advancing the cursor

    // space between each ui element and its inner text
    float text_padding_x;
    float text_padding_y;

    //MISC //

    //Material Node
    bool input_pressed;
    u32 input_pressed_id;
    vec2 input_pos;
    bool output_pressed;
    u32 output_pressed_id;


    // DRAW LIST

    //type,  type
    // push push push, push push
    // count 3, count 2
    // offset 3, count 5
    DYNAMIC_ARRAY_TYPE(UI_Draw_Command)* draw_command_list;
    UI_Draw_Command current_draw_command;
} Madness_UI;




// the best way im thinking to solve it is to have a formal concept of a pop up context,
// where when we get nodes, they are added to a pop up context, which is permanent in state,
// until we click outside the pop up area, and ideally if a pop up creates another pop up,
// then we need to know that as well

MAPI Madness_UI* madness_ui_init(Memory_System* memory_system, Input_System* input_system,
                                 Resource_System* resource_system);
MAPI bool madness_ui_shutdown(Madness_UI* madness_ui);

//pass in the size every frame, in the event the size changes
MAPI void madness_ui_begin(Madness_UI* madness_ui, i32 screen_size_x, i32 screen_size_y);
//Note: needs to be called right before the renderers update method, to generate the appropriate render data
MAPI void madness_ui_end(Madness_UI* madness_ui);


//NOTE: must be retrieved after madness_ui_end
MAPI UI_Render_Packet madness_ui_get_ui_render_data(Madness_UI* madness_ui);


//API START (besides init/shutdown, begin/end)

void madness_ui_menu_bar_begin(Madness_UI* madness_ui, String id);
void madness_ui_menu_bar_end(Madness_UI* madness_ui);
bool madness_ui_menu_item_begin(Madness_UI* madness_ui, String menu_name);
bool madness_ui_menu_item_end(Madness_UI* madness_ui);

bool madness_ui_pop_up_open(Madness_UI* madness_ui, String pop_up_name, vec2 pop_up_start_location);
bool madness_ui_pop_up_close(Madness_UI* madness_ui);


MAPI void madness_ui_window_begin(Madness_UI* madness_ui, String header_name);
MAPI void madness_ui_window_end(Madness_UI* madness_ui);


MAPI void madness_ui_set_window_pos(Madness_UI* madness_ui, u32 x, u32 y);
MAPI void madness_ui_set_window_size(Madness_UI* madness_ui, u32 width, u32 height);
MAPI vec2 madness_ui_get_window_pos(Madness_UI* madness_ui);
MAPI vec2 madness_ui_get_window_size(Madness_UI* madness_ui);




MAPI void madness_scroll_box_begin(Madness_UI* madness_ui, String id);
MAPI void madness_scroll_box_end(Madness_UI* madness_ui);


//TEXT//
MAPI void madness_ui_text_box(Madness_UI* madness_ui, String id);

MAPI UI_Node* madness_ui_text(Madness_UI* madness_ui, String text); // TODO: should replace ui_text at some point
MAPI UI_Node* madness_ui_text_internal(Madness_UI* madness_ui, String text, vec2 parent_pos, vec2 parent_size,
                                       UI_Alignment alignment_x, UI_Alignment alignment_y); // TODO: pass in the pos



MAPI bool madness_ui_button(Madness_UI* madness_ui, String label);
MAPI bool madness_ui_check_box(Madness_UI* madness_ui, String label, bool* check_box_state);

MAPI void madness_image(Madness_UI* madness_ui, String id, Texture_System* texture_system, const char* icon_path);
MAPI void madness_image_handle(Madness_UI* madness_ui, String id, Texture_Handle handle);

MAPI void madness_slider_scroll(Madness_UI* madness_ui, String id, float* slider_val, float min, float max);
//TODO: might get rid of the arrow version
MAPI void madness_slider_arrow(Madness_UI* madness_ui, String id, float* slider_val, float min, float max);


MAPI bool madness_ui_float(Madness_UI* madness_ui, String text, float* f, float increment_value);
MAPI bool madness_ui_float2(Madness_UI* madness_ui, String text, float* x, float* y, float increment_value);
MAPI bool madness_ui_float3(Madness_UI* madness_ui, String text, float* x, float* y, float* z, float increment_value);
MAPI bool madness_ui_vec2(Madness_UI* madness_ui, String label, vec2* v, float increment_value);
MAPI bool madness_ui_vec3(Madness_UI* madness_ui, String label, vec3* v, float increment_value);


MAPI bool madness_ui_drop_down(Madness_UI* madness_ui, String label, bool* state);

//TODO:
MAPI bool madness_ui_drop_down_tree(Madness_UI* madness_ui, String id, String text);
// >thing
//   >thing
//   >thing

MAPI bool madness_ui_combo_box(Madness_UI* madness_ui, String id, u32* selected_value, String* string_array,
                                      u32 string_array_size);
MAPI bool madness_ui_combo_box_char(Madness_UI* madness_ui, String id, u32* selected_value, char** char_array,
                                      u32 char_array_size);
// MAPI bool madness_ui_combo_box_enum(Madness_UI* madness_ui, String id, int* selected_value, char** string_array);
// < enum day <selected dat> -> enum day <Tuesday>
// > monday
// > tuesday
// > wednesday
MAPI bool madness_ui_grid_start(Madness_UI* madness_ui, String id, String text, int x_size, int y_size);
MAPI bool madness_ui_grid_end(Madness_UI* madness_ui, String id, String text);
MAPI void madness_ui_padding(Madness_UI* madness_ui, String text);


MAPI bool madness_ui_color_picker(Madness_UI* madness_ui, String label, vec3* color_value);


MAPI bool madness_ui_circle(Madness_UI* madness_ui, String id, float* thickness);


MAPI bool madness_ui_reflection_test(Madness_UI* madness_ui, Reflection_System* reflection_system, const char* struct_name);


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

MAPI bool madness_ui_node_simple(Madness_UI* madness_ui, String id, vec2 pos, String inputs[], u8 input_size,
                                 String outputs[], u8 output_size, u32 node_id);

MAPI bool madness_ui_node_complex(Madness_UI* madness_ui, String id, String inputs[], u8 input_size, String outputs[],
                                  u8 output_size);


MAPI bool madness_ui_node(Madness_UI* madness_ui, String id, String inputs[], u8 input_size, String outputs[],
                          u8 output_size);

MAPI bool madness_ui_drag_test(Madness_UI* madness_ui, vec2* pos);


MAPI bool madness_ui_quadratic_bezier(Madness_UI* madness_ui, vec2* pos1, vec2* pos2, vec2* pos3);
MAPI bool madness_ui_cubic_bezier(Madness_UI* madness_ui, vec2* pos1, vec2* pos2, vec2* pos3, vec2* pos4);


void madness_ui_same_line(Madness_UI* madness_ui);
void madness_ui_advance_cursor(Madness_UI* madness_ui, vec2 ui_screen_size);
void madness_ui_advance_cursor_horizontal(Madness_UI* madness_ui, vec2 ui_screen_size);

void madness_ui_set_button_size(Madness_UI* madness_ui, float button_size);
void madness_ui_set_font_size(Madness_UI* madness_ui, float font_size);
void madness_ui_set_padding_x(Madness_UI* madness_ui, float x);
void madness_ui_set_padding_y(Madness_UI* madness_ui, float y);
void madness_ui_set_padding_xy(Madness_UI* madness_ui, float x, float y);

//menu for showing configs
MAPI void madness_ui_config_menu(Madness_UI* madness_ui);

//API END

MAPI void madness_ui_test(Madness_UI* madness_ui);
MAPI void madness_ui_example(Madness_UI* madness_ui);


//these is only meant for internal use and not part of the API
MAPI void madness_calculate_text_size(Madness_UI* madness_ui, String text, vec2 screen_position, vec2* out_text_size);

MAPI vec2 madness_ui_get_text_size(Madness_UI* madness_ui, String text);
MAPI float madness_ui_get_default_element_height(Madness_UI* madness_ui);


//debug and test
MAPI void madness_ui_print_state(Madness_UI* madness_ui);


//utility
MAPI UI_Node* madness_ui_get_new_node(Madness_UI* madness_ui);
UI_Node* madness_ui_get_pop_up_node(Madness_UI* madness_ui);

UI_Node* madness_ui_new_text_node(Madness_UI* madness_ui);
UI_Node* madness_ui_new_scissor_start(Madness_UI* madness_ui, vec2 scissor_pos, vec2 scissor_size);
void madness_ui_new_scissor_end(Madness_UI* madness_ui);


MAPI void madness_ui_center_child_node(vec2 parent_pos, vec2 parent_size, vec2 child_size, vec2* out_pos);
MAPI char* madness_ui_float_to_char(Madness_UI* madness_ui, float value);


//draw list
void madness_ui_add_draw_command(Madness_UI* madness_ui, UI_Draw_Command_Type draw_type);


MAPI bool is_ui_hot(Madness_UI* madness_ui, int id);

MAPI bool is_ui_active(Madness_UI* madness_ui, int id);

MAPI bool region_hit(Madness_UI* madness_ui, vec2 pos, vec2 size);


//UTILITY

//check if we can use the button
MAPI bool madness_ui_use_ui_element(Madness_UI* madness_ui, int id, vec2 pos, vec2 size);

MAPI int generate_id(Madness_UI* madness_ui);

MAPI void set_hot(Madness_UI* madness_ui, int id);

MAPI void set_active(Madness_UI* madness_ui, int id);

MAPI bool can_be_active(Madness_UI* madness_ui);
MAPI bool is_active(Madness_UI* madness_ui, int id);

MAPI bool is_hot(Madness_UI* madness_ui, int id);

void madness_ui_set_interaction_state(Madness_UI* madness_ui, UI_Node* new_node);

#endif //UI_H
