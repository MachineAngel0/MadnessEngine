#ifndef INSANITY_UI_H
#define INSANITY_UI_H

#include "allocator.h"
#include "ui_types.h"

//IMMEDIATE MODE UI


//TODO: the rewrite and features needed:
// UI sizes and positions should be modifiable with a GUI and saved into a file somewhere for loading, without me needing to constantly recompile the game
// think of the ui system in silver case
// things need to be able to be ordered as lists, and need to be able to be nested inside other ui elements
// explore functions like, draw_below or draw inbetween two nodes, and something like an early resolve
// animations - specify the state of one thing, and then the end state of that thing, size, pos, rotation, color,

//NOTE: the smallest likely screen size for a laptop is likely to be a 1024*600 or more commonly 1024 * 768
// smalles ill let it be is 600*600

//TODO: flip this on and off for usage
#define INSANITY_UI_DEBUG_PRINT

// button -> clickable, hoverable, box, text (maybe)
// slider -> clickable, hoverable, box, slider (horizontal/vertical),
// layout -> start and end, children, scrollable, slider(horizontal/vertical), draggable
// float -> box, text, scroll thing
// vec3 -> vertical layout, box, /children ->  background, wheel scroll


#define INSANITY_DEFAULT_FONT_SIZE 32.0f
#define INSANITY_EDITOR_FONT_SIZE 16.0f
#define INSANITY_TEXT_OUTLINE 0.5f

#define INSANITY_UI_MAX_NODE_COUNT 1000
#define INSANITY_UI_MAX_WINDOW_COUNT 100
#define INSANITY_MAX_UI_NODE_CHILD_COUNT 32
#define INSANITY_UI_MAX_STRING_SLICE 10000

#define INSANITY_UI_DEFAULT_RESOLUTION_WIDTH 1920llu
#define INSANITY_UI_DEFAULT_RESOLUTION_HEIGHT 1080llu

#define INSANITY_UI_DEFAULT_TEXT_COLOR COLOR_WHITE


typedef enum UI_Text_Wrap
{
    UI_Text_Wrap_Wrap,
    UI_Text_Wrap_Newline,
    UI_Text_Wrap_None,
} UI_Text_Wrap;


typedef struct Insanity_UI_Editor_Style
{
    vec3s layout_color;
    vec3s layout_accent_color;

    vec3s text_color;
    vec3s textbox_color;

    vec3s custom_widget_color;

    //colors for things like buttons and checkboxes
    vec3s color;
    vec3s hovered_color;
    vec3s pressed_color;

    vec3s outline_color;

    vec3s permanent_active;

    vec3s header_color;
    vec3s pop_up_color;
    vec3s scrollbox_color;
    vec3s error_color;
} Insanity_UI_Editor_Style;


typedef struct Insanity_UI_Event
{
    //interaction events
    bool hovered; //is this node currently bieng hovered over
    bool pressed; //is this node active and bieng pressed
    bool clicked; // has the mouse been released while hovering over this node

    bool mouse_scrolled;
    // bool mouse_up;
    // bool mouse_down;
    s32 mouse_wheel_delta;

    float mouse_delta_x;
    float mouse_delta_y;

    s16 mouse_pos_x;
    s16 mouse_pos_y;

    //nagivation events
    bool nav_hovered;
    bool nav_pressed;
    bool nav_clicked;
    bool nav_returned;
    // bool nav_up; ...etc for all directions
} Insanity_UI_Event;


typedef struct IUI_String_Slice
{
    // relative position from (0,0) resolved at the end to get proper positions
    f32 slice_width;
    f32 slice_height;

    String_Slice slices;
    char* debug_string;

    UI_Alignment alignment_x;
    UI_Alignment alignment_y;
} IUI_String_Slice;

typedef struct IUI_Node
{
    UI_Property_Flags ui_flags;
    Insanity_UI_Node_Type type;

    // screen size and pos, not normalized
    vec2s pos;
    vec2s size;
    float rotation; // degrees, but gets converted to radians at draw time

    //rounded
    float rounded_radius; // 0-1 range

    //outline
    vec3s outline_color;
    float outline_thickness; // 0-1 :: ideally should be something small like 0.05-0.1

    //for circles
    float thickness; // size of the circle is determined by the pos and size

    const char* name_id;
    u32 hash_id;

    vec2s min;
    vec2s max;

    Texture_Handle texture_handle;
    vec2s uv_offset;
    vec2s uv_size;

    //pointer to the first slice within the slices array
    IUI_String_Slice* slice_pointer;
    u32 slice_count; // how many of these we have
    u32 total_slice_height; // height of all the slices

    //colors
    vec3s color;

    s32 z_order;
} IUI_Node;

typedef enum IUI_Scroll_Flags
{
    IUI_Scroll_Flags_Size_By_Screen_Size_Percent,
    IUI_Scroll_Flags_Align, //use global state to align to something, if nothing will be the screen size
} IUI_Scroll_Flags;


typedef struct IUI_Scroll
{
    u32 id;

    vec2s starting_pos;
    vec2s scroll_cursor;
    float content_height;
    float scroll_offset;
    float scroll_bar_percent_offset;
    IUI_Node* scroll_container;
    IUI_Node* scroll_bar;
} IUI_Scroll;


typedef struct IUI_Window
{
    u32 id;
} IUI_Window;


typedef struct IUI_Panel
{
    u32 id;
} IUI_Panel;


typedef struct IUI_Drag_State
{
    u32 drag_id;
    vec2s drag_position;
} IUI_Drag_State;

typedef enum Insanity_UI_Persistent_State
{
    Insanity_UI_Persistent_State_Drag = BITFLAG(0),
    Insanity_UI_Persistent_State_Scroll = BITFLAG(0),
} Insanity_UI_Persistent_State;

typedef enum IUI_Event_Flags
{
    Insanity_UI_Event_Flags_Interaction = BITFLAG(0),
    Insanity_UI_Event_Flags_Navigation = BITFLAG(1),
    Insanity_UI_Event_Flags_Individual_Interaction = BITFLAG(2),
    // ignores hot/active states and does an individual interaction test
    Insanity_UI_Event_Flags_Individual_Hover = BITFLAG(3), // just checks for hover ignoring other state
} IUI_Event_Flags;

typedef struct Insanity_UI
{
    Allocator* allocator; // rn mainly just for loading fonts, would be better as a pool arena
    Allocator* scratch_allocator; //small amount of memory
    Frame_Allocator* frame_allocator;

    Input_System* input_system; // does not own memory

    //NOTE: this could be a context pointer with function ptr for getting what you need, if you wanted to make this a library
    Asset_System* asset_system; // does not own memory,

    //whats gets used by the ui system, and then scaled by the actual screen size/resolution
    vec2s internal_screen_resolution;


    vec2s screen_size; // this gets queried every frame in the begin effect
    vec2s screen_resolution_scale; //calculated based on the screen size

    //for invalid states, pass this back instead of crashing
    IUI_Node dummy_node;

    //this should be an array at some point
    Texture_Handle default_font_handle;
    float default_font_size;
    float editor_font_size;
    float text_outline;
    vec3s text_outline_color;
    // Font fonts[100];

    IUI_String_Slice* string_slice_array;
    u32 string_slice_count;
    u32 string_slice_count_max;


    //Persistent Information
    IUI_Drag_State drag_state[100];
    u32 drag_node_count;

    //temp and should be more than one
    IUI_Scroll scroll_state;

    //Interaction
    IUI_Node* interaction_node_array[INSANITY_UI_MAX_NODE_COUNT];
    u32 interaction_node_count;

    IUI_Node* navigation_node_array[INSANITY_UI_MAX_NODE_COUNT];
    u32 navigation_node_count;

    ARRAY_TYPE(IUI_Node)* ui_nodes;


    //Render
    UI_Render_Node* render_node_array;
    u64 render_node_array_count;
    UI_Draw_Command* draw_command_array;
    u64 draw_command_count;
    u64 current_draw_command_count;


    Insanity_UI_Editor_Style editor_style;


    //snapshot of our inputs, used for telling any hot/active nodes what happened
    Insanity_UI_Event interaction_result;
    u32 hot_last_frame;
    u32 hot_this_frame;
    u32 active;
    u32 hover_hot;

    //TODO:
    // u32 navigation_focus_id;
    // u32 navigation_hot_id;
    // u32 navigation_active_id;

    u32 hash; // a continous hash to give nodes a unique id

    //Mouse
    bool mouse_down;
    bool mouse_down_unique;
    bool mouse_released_unique;
    s16 mouse_pos_x;
    s16 mouse_pos_y;
    s16 mouse_delta_x;
    s16 mouse_delta_y;
    s32 clicked_this_frame;


    bool mouse_wheel_up;
    bool mouse_wheel_down;
    s32 mouse_wheel_delta;

    //Keyboard
    char first_released_key;
    // char mod_key;

    //checking if any of the mod keys are pressed
    bool key_shift;
    bool key_alt;
    bool key_ctrl;
    // bool key_super;

    bool key_backspace;

    //Scroll
    IUI_Scroll scroll_array[100];
    u32 scroll_array_count;

    vec2 scroll_max_view;
} Insanity_UI;

static Insanity_UI* insanity_ui;


//API
bool insanity_ui_init(Memory_System* memory_system, Input_System* input_system,
                      Asset_System* asset_system);
bool insanity_ui_deinit(void);

//pass in the size every frame, in the event the size changes
void insanity_ui_begin(s32 screen_size_x, s32 screen_size_y);


//Note: needs to be called right before the renderers update method, to generate the appropriate render data
Insanity_UI_Render_Packet insanity_ui_end(void);
void insanity_ui_resolve_interaction(void);


UI_Render_Packet insanity_get_render_packet(void);


//Building Blocks
IUI_Node* insanity_ui_node(const char* name);
// positions itself automatically based on the screen size
/*inserts the node into an array for later resolving, and there is one frame of delay for getting the interaction*/
Insanity_UI_Event insanity_ui_event(IUI_Node* node, IUI_Event_Flags event_flags);


// rect cut pixels
IUI_Node* insanity_ui_node_cut_left(IUI_Node* parent, const char* name, f32 size);
IUI_Node* insanity_ui_node_cut_right(IUI_Node* parent, const char* name, f32 size);
IUI_Node* insanity_ui_node_cut_top(IUI_Node* parent, const char* name, f32 size);
IUI_Node* insanity_ui_node_cut_bottom(IUI_Node* parent, const char* name, f32 size);
// rect cut percents
IUI_Node* insanity_ui_node_cut_left_percent(IUI_Node* parent, const char* name, f32 percent_size);
IUI_Node* insanity_ui_node_cut_right_percent(IUI_Node* parent, const char* name, f32 percent_size);
IUI_Node* insanity_ui_node_cut_top_percent(IUI_Node* parent, const char* name, f32 percent_size);
IUI_Node* insanity_ui_node_cut_bottom_percent(IUI_Node* parent, const char* name, f32 percent_size);

//TODO:
// void madness_ui_new_scissor_start(vec2s scissor_pos, vec2s scissor_size);
// void madness_ui_new_scissor_end(void);


//Windows
IUI_Scroll* insanity_ui_window_begin(const char* name, vec2s pos, vec2s size);
void insanity_ui_window_end(IUI_Scroll* window_state);

//scroll
IUI_Scroll* insanity_ui_scroll_begin(const char* name, vec2s pos, vec2s size);
void insanity_scroll_end(IUI_Scroll* scroll);
// might just want to have pos and size instead of node
void insanity_ui_scroll_advance(IUI_Scroll* scroll, IUI_Node* node);
void insanity_ui_scroll_advance_size(IUI_Scroll* scroll, vec2s size);


//tree view
IUI_Scroll* insanity_ui_tree_view_begin(const char* name, vec2s pos, vec2s size);
void insanity_ui_tree_view_push_indent(IUI_Scroll* scroll);
void insanity_ui_tree_view_pop_indent(IUI_Scroll* scroll);
void insanity_ui_tree_scroll_advance(IUI_Scroll* scroll, IUI_Node* node);
IUI_Scroll* insanity_ui_tree_view_end(const char* name, vec2s pos, vec2s size);

//combo box
IUI_Scroll* insanity_ui_combo_begin(const char* name, vec2s pos, vec2s size);
void insanity_ui_combo_advance(IUI_Scroll* scroll, IUI_Node* node);
IUI_Scroll* insanity_ui_combo_end(const char* name, vec2s pos, vec2s size);


//pop up
void pop_up_begin();
void pop_up_end();

//combo box???? if yes, then nodes need to be fixed size

//STRING
IUI_Node* insanity_ui_text(const char* text);
IUI_Node* insanity_ui_text_fast(const char* text, u32 string_size);

IUI_Node* insanity_ui_text_wrapped(const char* text, float max_width, UI_Text_Wrap wrap_mode);


//NOTE: just an idea, so that text will resize (not wrap for now, you would have to think that through) to the containers size
// IUI_Node* insanity_ui_text_resize(const char* text, IUI_Node* container, UI_Text_Wrap_Mode wrap_mode);

vec2s insanity_ui_text_calculate_size(const char* text);
vec2s insanity_ui_text_calculate_size_fast(const char* text, u32 string_size);


// TODO: should think about this, since i would probably want multiple node to represent this, maybe pass back the first and last nodes?
// void insanity_ui_text_wrap(const char* text, Insanity_UI_Node* parent);


//TEXTURES AND FONTS
void insanity_ui_set_font(Texture_Handle handle);
void insanity_ui_set_font_default();
IUI_Node* insanity_ui_image(const char* name, Texture_Handle handle);


//LAYOUT and CONSTRAINTS
void insanity_ui_node_offset_x(IUI_Node* node_to_offset, IUI_Node* anchor_node,
                               float x_offset);
void insanity_ui_node_offset_y(IUI_Node* node_to_offset, IUI_Node* anchor_node,
                               float y_offset);
void insanity_ui_node_offset(IUI_Node* node_to_offset, IUI_Node* anchor_node, vec2s offset);

//TODO:
// void insanity_ui_node_offset_from_end(IUI_Node* node_to_offset, IUI_Node* anchor_node, vec2s offset);

//returns the new position
float _insanity_ui_node_align_axis(float node_size_axis, float container_pos_axis,
                                   float container_size_axis,
                                   UI_Alignment alignment);

void insanity_ui_node_align_x(IUI_Node* node_to_align, IUI_Node* container,
                              UI_Alignment x_alignment);
void insanity_ui_node_align_y(IUI_Node* node_to_align, IUI_Node* container,
                              UI_Alignment alignment);
void insanity_ui_node_align(IUI_Node* node_to_align, IUI_Node* container,
                            UI_Alignment x_alignment, UI_Alignment y_alignment);

void insanity_ui_node_expand(IUI_Node* node_to_expand, IUI_Node* container);
void insanity_ui_node_expand_x(IUI_Node* node_to_expand, IUI_Node* container);
void insanity_ui_node_expand_y(IUI_Node* node_to_expand, IUI_Node* container);

void insanity_ui_node_expand_percent_x(IUI_Node* node_to_expand, IUI_Node* container, float percent);
void insanity_ui_node_expand_percent_y(IUI_Node* node_to_expand, IUI_Node* container, float percent);
void insanity_ui_node_expand_percent(IUI_Node* node_to_expand, IUI_Node* container, vec2s percent);


vec2s insanity_ui_screen_size_percent(float x_percent, float y_percent)
{
    return (vec2s){
        .x = x_percent * insanity_ui->screen_size.x,
        .y = y_percent * insanity_ui->screen_size.y
    };
}

vec2s insanity_ui_node_align_to_screen_size(IUI_Node* node, UI_Alignment_X x_alignment, UI_Alignment_X y_alignment)
{
    vec2s out_pos = {0};
    float horizontal_space_remaining = 0;
    switch (x_alignment)
    {
    case UI_ALIGNMENT_X_LEFT:
        node->pos.x = 0;
        break;
    case UI_ALIGNMENT_X_CENTER:
        horizontal_space_remaining = insanity_ui->screen_size.x - node->size.x;
        node->pos.x = (horizontal_space_remaining / 2.f);
        break;
    case UI_ALIGNMENT_X_RIGHT:
        horizontal_space_remaining = insanity_ui->screen_size.x - node->size.x;
        node->pos.x = horizontal_space_remaining;
        break;
    }

    float vertical_space_remaining = 0;
    switch (y_alignment)
    {
    case UI_ALIGNMENT_X_LEFT:
        node->pos.y = 0;
        break;
    case UI_ALIGNMENT_X_CENTER:
        vertical_space_remaining = insanity_ui->screen_size.y - node->size.y;
        node->pos.y = (vertical_space_remaining / 2.f);
        break;
    case UI_ALIGNMENT_X_RIGHT:
        vertical_space_remaining = insanity_ui->screen_size.y - node->size.y;
        node->pos.y = vertical_space_remaining;
        break;
    }


    return out_pos;
}


void insanity_ui_node_constraint_size(IUI_Node* node_to_constraint, IUI_Node* container);


//TODO: going to need for auto sizing
// void insanity_ui_node_enforce_min_size(Insanity_UI_Node* node_to_constraint, vec2s min_size_pixels);
// void insanity_ui_node_enforce_max_size(Insanity_UI_Node* node_to_constraint, vec2s max_size_pixels);


//Utility
bool insanity_ui_rect_hit(IUI_Node* node);


//Test
void insanity_ui_test(float dt, float elapsed_time);


// void insanity_ui_autoplace_and_size(Insanity_UI_Node* node);


IUI_Drag_State* insanity_ui_get_drag_state(IUI_Node* node)
{
    IUI_Drag_State* drag_state = NULL;
    for (u32 i = 0; i < insanity_ui->drag_node_count; i++)
    {
        if (insanity_ui->drag_state[i].drag_id == node->hash_id)
        {
            drag_state = &insanity_ui->drag_state[i];
        }
    }
    if (!drag_state)
    {
        u32 drag_index = insanity_ui->drag_node_count++;
        insanity_ui->drag_state[drag_index].drag_id = node->hash_id;
        drag_state = &insanity_ui->drag_state[drag_index];
        drag_state->drag_position = node->pos;
    }
    MASSERT(drag_state);
    return drag_state;
}

void insanity_ui_drag(IUI_Node* node)
{
    //find the drag state, then adjust position is it moves
    IUI_Drag_State* drag_state = insanity_ui_get_drag_state(node);
    node->pos.x = drag_state->drag_position.x;
    node->pos.y = drag_state->drag_position.y;

    Insanity_UI_Event event = insanity_ui_event(node, Insanity_UI_Event_Flags_Interaction);
    if (event.pressed)
    {
        drag_state->drag_position.x += event.mouse_delta_x;
        drag_state->drag_position.y += event.mouse_delta_y;
    }
}

void insanity_ui_drag_and_snap_center_to_mouse_position(IUI_Node* node)
{
    //find the drag state, then adjust position is it moves
    IUI_Drag_State* drag_state = insanity_ui_get_drag_state(node);
    node->pos.x = drag_state->drag_position.x;
    node->pos.y = drag_state->drag_position.y;

    Insanity_UI_Event event = insanity_ui_event(node, Insanity_UI_Event_Flags_Interaction);
    if (event.pressed)
    {
        //want to drag from the center of the node
        drag_state->drag_position.x = (float)event.mouse_pos_x - (node->size.x / 2);
        drag_state->drag_position.y = (float)event.mouse_pos_y - (node->size.y / 2);
    }
}


// what about interaction
// what about keyboard/controller input
// what about text wrapping or text shrinking
// how about text effects, like floating text
// handling animations for the ui's?
// what about clipping items outside a window context?
// what about clipping items outside a window context?
// should the system handle movable or the user? the system informs, the user should handle it what happens, provided some generally utility functions


// how about querying for interactions,
// if(button()){}  // simply
// if(get_interaction(flags)) // simply and explicit, nonambigious
// if(button.event.pressed) // simply and explicit, nonambigious, can't have node return types, and can't query individal nodes


//scenarios - making a scroll list
// window_begin(scrollable | header | collapsable )
//   loop
//      Node()
//      Advance_cursor()
//   loop end
// window_end()


//scenarios - making an ability list
// window_begin(scrollbar || autosize | max_size_clamp)
//   loop
//      container = Node()
//      icon = Node()
//      Node_offset(container, icon, right_offset)
//      text = Node()
//      Node_offset(icon, text)
//      Node_align(container, text)
//      Advance_cursor(container)
//      //handling input
//      if(container.event.pressed)
//   loop end
// window_end()

//scenarios - making the player and enemy view
//      box = Node()
//      box2 = Node_Rect_Right(half_size/percent) // will split the box into the left region, and box2 takes the right region
//      player_text = Node()
//      node_align_center(box, player_text)
//      enemy_text = Node()
//      node_align_center(box2, enemy_text)
//
//      set_cursor_pos(box) // so that we can properly position the starting element elements
//      loop() //the character info
//          get_cursor_pos();
//          view = Node()
//          Node_offset(view, padding)
//          player_name = Node()
//          Node_offset(view, player_name)
//          health_bar = Node()
//          Node_offset(player_name, health_bar)
//          health_bar_text = Node()
//          Node_offset(health_bar, health_bar_text)
//          (repeat for the mp bar)
//          set_cursor_pos(view) // so that we can properly position the elements for the next iteration
//      loop() end

//scenarios - jiggly text
//     text = Node()
//     Text_String = Node_expand(text)
//     for text_string
//          jiggly_function(text_character)


//scenarios - menu bar
//          menu_bar_container = Node()
//          file_bar = rect_left(0.1%) // node take a portion from the left
//          file_text = Node("File")
//          node_align_center(file_bar, file_text)
//          if(file_bar.event.pressed){
//              window_pos()
//              window_begin(scrollable | popup)
//              for(){
//                  options and text align
//              }
//              Node()
//          }
//          node_align_center(file_bar, file_text)
//

//scenarios - exit pop up
//       button = Node()
//       if(button.event.pressed){
//          modal_begin()
//          container = Node()
//          button1, button2
//          if(interaction(button1).pressed){exit; do other state}
//          if(interaction(button2).pressed){exit; do other state}
//          modal_end()
//       };

//scenarios - button1 - hard code everything
//       button = Node()
//       text = Node()
//       node_align(text, button, center, center);


//scenarios - button2 - use a child node
//       button = Node()
//       text = Node_child(button)
//       node_align_to_parent(text, center, center);
//       node_resolve_layout(button(root node));
//  //note but now you can move just the parent, and its applied to the child

//scenarios - button3 - size button to the child plus some padding
//       button = Node()
//       (place buttom somewhere)
//       text = Node_child(button)
//       node_expand = Node(button, text)
//       node_padding_with_clamp(button)
//       node_align(text, button, center, center);

//  //note but now you can moce just the parent, and its applied to the child

//scenarios - scroll bar
//       scroll = node_scroll() -> start scissor
//       thing = Node() // position node
//        node_scroll_end(scroll) -> end scissor and state
//       //later -> cull node
//       // the clipper can be immediate mode as well, like clip_range() and figure how that works

//scenarios - menu with arrows on object 3
//       scroll = node_scroll()
//       assume auto positioning
//       thing = Node()
//       thing2 = Node()
//       thing3 = Node()
//       thing4 = Node()
//       thing5 = Node()
//       node_scroll_end(scroll, {resolve_layout_true_false}) // not this should be true with a push flag for false if needed
//       node_resolve_layout(scroll)
//       left_arrow = node();
//       right_arrow = node();
//       left_arrow.pos = thing3.pos.x - padding.x;
//       right.pos = thing3.pos.x + thing3.size.x + padding.x;


//nice utilities
Insanity_UI_Event insanity_ui_button(const char* label, vec2s position)
{
    IUI_Node* button = insanity_ui_node("button");
    button->pos = position;
    button->color = insanity_ui->editor_style.color;
    IUI_Node* button_text = insanity_ui_text(label);
    insanity_ui_node_expand(button, button_text);
    f32 x_padding = 10;
    f32 y_padding = 10;
    button->size.x += x_padding;
    button->size.y += y_padding;

    insanity_ui_node_align(button_text, button, UI_ALIGNMENT_CENTER, UI_ALIGNMENT_CENTER);

    Insanity_UI_Event event = insanity_ui_event(
        button, Insanity_UI_Event_Flags_Interaction | Insanity_UI_Event_Flags_Navigation);
    if (event.hovered)
    {
        button->color = insanity_ui->editor_style.hovered_color;
    }
    if (event.pressed)
    {
        button->color = insanity_ui->editor_style.pressed_color;
    }


    return event;
}


typedef struct Material_Link
{
    u32 slot_in;
    u32 slot_out;
} Material_Link;

typedef struct Material_Slot
{
    //NOTE: for ergonomic reasons, im not making them SOA's
    Shader_Type input_shader_types;
    const char* input_names;
} Material_Slot;

typedef struct Material_node
{
    u32 id;
    const char* node_name;

    Material_Slot input_slots;
    u32 input_count;

    Material_Slot output_slots;
    u32 output_count;
} Material_node;

void material_node_create(const char* node_name, Material_Slot* input_slots,
                          u32 input_count,
                          Material_Slot* output_slots,
                          u32 output_count)
{
    //TODO: render
    //TODO: add to a list of some kind to find out connections
}


void material_node_add()
{
    Material_Slot inputs[] = {
        (Material_Slot){.input_shader_types = SHADER_TYPE_FLOAT, .input_names = "Float 1"},
        (Material_Slot){.input_shader_types = SHADER_TYPE_FLOAT, .input_names = "Float 2"},
    };

    Material_Slot ouputs[] = {

    };

    material_node_create("add", inputs, ARRAY_SIZE(inputs), ouputs, ARRAY_SIZE(ouputs));
}



#endif //INSANITY_UI_H
