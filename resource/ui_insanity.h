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


    //nagivation events
    bool nav_hovered;
    bool nav_pressed;
    bool nav_clicked;
    bool nav_returned;
    // bool nav_up; ...etc for all directions
} Insanity_UI_Event;


typedef struct Insanity_UI_Node
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

    Texture_Handle texture_handle;
    vec2s uv_offset;
    vec2s uv_size;

    //Text
    String* text; //the full text string, also max height and width are stored in the vec2s size


    //colors
    vec3s color;

    s32 z_order;
} Insanity_UI_Node;

typedef struct Insanity_UI_Scroll
{
    vec2s starting_pos;
    vec2s starting_size;
    Insanity_UI_Node* scroll_container_node;
    vec2s cursor;

    // vec2s current_size;
    bool outside_view;
} Insanity_UI_Scroll;

typedef struct Insanity_UI
{
    Allocator* allocator; // rn mainly just for loading fonts, would be better as a pool arena
    Allocator* scratch_allocator; //small amount of memory
    Frame_Allocator* frame_allocator;

    Input_System* input_system; // does not own memory

    //NOTE: this could be a context pointer with function ptr for getting what you need, if you wanted to make this a library
    Asset_System* asset_system; // does not own memory,

    vec2s screen_size; // this gets queried every frame in the begin effect


    //this should be an array at some point
    Texture_Handle default_font_handle;
    float default_font_size;
    float editor_font_size;
    float text_outline;
    // Font fonts[100];


    //Interaction
    Insanity_UI_Node* interaction_node_array[INSANITY_UI_MAX_NODE_COUNT];
    u32 interaction_node_count;

    Insanity_UI_Node* navigation_node_array[INSANITY_UI_MAX_NODE_COUNT];
    u32 navigation_node_count;

    ARRAY_TYPE(Insanity_UI_Node)* ui_nodes;

    Insanity_UI_Node dummy_node;

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
    Insanity_UI_Scroll scroll_array[100];
    u32 scroll_array_count;
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
Insanity_UI_Node* insanity_ui_node(const char* name);
Insanity_UI_Node* insanity_ui_node_create_copy(const char* name, Insanity_UI_Node* node);

Insanity_UI_Node* insanity_ui_node_rect_left(const char* name, Insanity_UI_Node* parent, vec2 percent_pos,
                                             vec2 percent_size);
Insanity_UI_Node* insanity_ui_node_rect_right(const char* name, Insanity_UI_Node* parent, vec2 percent_pos,
                                              vec2 percent_size);
Insanity_UI_Node* insanity_ui_node_rect_up(const char* name, Insanity_UI_Node* parent, vec2 percent_pos,
                                           vec2 percent_size);
Insanity_UI_Node* insanity_ui_node_rect_down(const char* name, Insanity_UI_Node* parent, vec2 percent_pos,
                                             vec2 percent_size);

//TODO:
// void madness_ui_new_scissor_start(vec2s scissor_pos, vec2s scissor_size);
// void madness_ui_new_scissor_end(void);

//Windows
Insanity_UI_Scroll* scroll_begin(const char* name, vec2s pos, vec2s size);
void scroll_end();

//an idea, we can pass back a garbage node, so any modifications dont matter
//this is here as an optimization, inside a loop, so that we are not need
bool insanity_ui_is_scroll_outside_view(Insanity_UI_Node* node)
{
    Insanity_UI_Scroll* state = &insanity_ui->scroll_array[insanity_ui->scroll_array_count - 1];
    if (state->starting_pos.y + state->starting_size.y <= node->pos.y)
    {
        node->size = (vec2s){0, 0};
        state->outside_view = true;
        return true;
    }
    return false;
}


bool scrollbox_is_cursor_outside_view(Insanity_UI_Scroll* scroll)
{
    if (scroll->starting_pos.y + scroll->starting_size.y <= scroll->cursor.y)
    {
        return true;
    }
    if (scroll->starting_pos.x + scroll->starting_size.x <= scroll->cursor.y)
    {
        return true;
    }

}


//returns true if we went outside the view of the scrollbox, will cull all future nodes
bool scrollbox_advance_down(Insanity_UI_Scroll* scroll, Insanity_UI_Node* node, bool cull_nodes_outside_view)
{
    if (cull_nodes_outside_view)
    {
        insanity_ui_is_scroll_outside_view(node);
    }
    scroll->cursor.y += node->size.y;

}

bool scrollbox_advance_right(Insanity_UI_Scroll* scroll, Insanity_UI_Node* node, bool cull_nodes_outside_view)
{
    if (cull_nodes_outside_view)
    {
        insanity_ui_is_scroll_outside_view(node);
    }
    scroll->cursor.y += node->size.y;
}


void pop_up_begin();
void pop_up_end();


//STRING
Insanity_UI_Node* insanity_ui_text(const char* text);
// Insanity_UI_Node* insanity_ui_text_wrapped(const char* text);
Insanity_UI_Node* insanity_ui_text_fast(const char* text, u32 string_size);

vec2s insanity_ui_text_calculate_size(const char* text);
vec2s insanity_ui_text_calculate_size_fast(const char* text, u32 string_size);


// TODO: should think about this, since i would probably want multiple node to represent this, maybe pass back the first and last nodes?
// void insanity_ui_text_wrap(const char* text, Insanity_UI_Node* parent);


//TEXTURES AND FONTS
void insanity_ui_set_font(Texture_Handle handle);
void insanity_ui_set_font_default();
Insanity_UI_Node* insanity_ui_image(const char* name, Texture_Handle handle);


//LAYOUT and Contraints
void insanity_ui_node_offset_from_node_x(Insanity_UI_Node* node_to_offset, Insanity_UI_Node* anchor_node,
                                         float x_offset);
void insanity_ui_node_offset_from_node_y(Insanity_UI_Node* node_to_offset, Insanity_UI_Node* anchor_node,
                                         float y_offset);
void insanity_ui_node_offset_from_node(Insanity_UI_Node* node_to_offset, Insanity_UI_Node* anchor_node, vec2s offset);


void insanity_ui_node_align_x(Insanity_UI_Node* node_to_align, Insanity_UI_Node* container,
                              UI_Alignment x_alignment);
void insanity_ui_node_align_y(Insanity_UI_Node* node_to_align, Insanity_UI_Node* container,
                              UI_Alignment y_alignment);

void insanity_ui_node_align(Insanity_UI_Node* node_to_align, Insanity_UI_Node* container,
                            UI_Alignment x_alignment, UI_Alignment y_alignment);

void insanity_ui_node_expand_xy(Insanity_UI_Node* node_to_expand, Insanity_UI_Node* container);
void insanity_ui_node_expand_x(Insanity_UI_Node* node_to_expand, Insanity_UI_Node* container);
void insanity_ui_node_expand_y(Insanity_UI_Node* node_to_expand, Insanity_UI_Node* container);

void insanity_ui_node_expand_x_percent(Insanity_UI_Node* node_to_expand, Insanity_UI_Node* container, float percent);
void insanity_ui_node_expand_y_percent(Insanity_UI_Node* node_to_expand, Insanity_UI_Node* container, float percent);


void insanity_ui_node_expand_and_align_y(Insanity_UI_Node* node_to_expand, Insanity_UI_Node* container, float percent,
                                         UI_Alignment y_alignment);


vec2s insanity_ui_node_get_screen_size_percent(float x_percent, float y_percent);


void insanity_ui_node_constraint_size(Insanity_UI_Node* node_to_constraint, Insanity_UI_Node* container);


/*inserts the node into an array for later resolving, and there is one frame of delay for getting the interaction*/
Insanity_UI_Event insanity_ui_event(Insanity_UI_Node* node, bool interactable, bool navigatable);

bool insanity_ui_rect_hit(Insanity_UI_Node* node);


//Test
void insanity_ui_test(float dt, float elapsed_time);


// void insanity_ui_autoplace_and_size(Insanity_UI_Node* node);


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


#endif //INSANITY_UI_H
