#ifndef INSANITY_UI_EDITOR_H
#define INSANITY_UI_EDITOR_H

#include "ui_insanity.h"
#include "cglm/struct/vec2.h"


typedef enum Insanity_UI_Window_Flags
{
    Insanity_UI_Window_Flag_No_Header = BITFLAG(0),
    Insanity_UI_Window_Flag_No_Resize = BITFLAG(1),
    Insanity_UI_Window_Flag_No_Auto_Resize = BITFLAG(2), // will size to the max given by initial percent size or smaller
    Insanity_UI_Window_Flag_No_Move = BITFLAG(3), // must have a header to be movable
    Insanity_UI_Window_Flag_No_Scroll = BITFLAG(4),
    Insanity_UI_Window_Flag_No_Scroll_Mouse = BITFLAG(5),
    Insanity_UI_Window_Flag_No_SavePosition = BITFLAG(6),
    Insanity_UI_Window_Flag_No_Collapse = BITFLAG(7), // you have to have a header to be able to collapse
    Insanity_UI_Window_Flag_No_Background_Color = BITFLAG(8),

} Insanity_UI_Window_Flags;

typedef enum Insanity_UI_Pop_Up_Flags
{
    Insanity_UI_Pop_Up_Flags_No_Header = BITFLAG(2),
    Insanity_UI_Pop_Up_Flags_No_Scroll = BITFLAG(2),
    Insanity_UI_Pop_Up_Flags_No_Scroll_Mouse = BITFLAG(4),

} Insanity_UI_Pop_Up_Flags;


typedef struct Insanity_UI_Window
{
    const char* name;

    Insanity_UI_Window_Flags flags;

    Insanity_UI_Pop_Up_Flags pop_flags;


    vec2s intial_position_percent;
    vec2s intial_size_percent;

    //
    vec2s window_pos;
    vec2s window_size;

    // vec2s header_size;
    // vec2s header_pos;

    float scroll_offset; // should ideally be in a range of size, and then we increment the size by that
    float scroll_bar_percent_offset; // should ideally be in a range of size, and then we increment the size by that

    vec2s window_relative_cursor_pos; // track how far down items have gone relative to the window*/

    bool is_open;
    bool none_active;
    bool writable;

    Insanity_UI_Node* container_node;
    Insanity_UI_Node* header_node;

} Insanity_UI_Window;


//EDITOR API
//everything should be created from the building blocks above, must have an active window to be used
Insanity_UI_Event insanity_ui_button(const char* label)
{
    Insanity_UI_Window state = insanity_ui_get_window();
    if (!state.writable) { return (Insanity_UI_Event){0}; }

    Insanity_UI_Node* container = insanity_ui_node(label);
    Insanity_UI_Event result = insanity_ui_event(container, true,true);

    container->pos = insanity_ui->cursor_position;
    container->size = (vec2s){100, 100};
    container->color = insanity_ui->editor_style.color;
    Insanity_UI_Node* text = insanity_ui_text(label);
    // text->pos = container->pos;
    insanity_ui_node_align(text, container, UI_ALIGNMENT_CENTER, UI_ALIGNMENT_CENTER);

    if (result.hovered)
    {
        container->color = insanity_ui->editor_style.hovered_color;
    }
    if (result.pressed)
    {
        container->color = insanity_ui->editor_style.pressed_color;
    }

    insanity_ui->cursor_position.y += container->size.y;


    return result;
}


//Windows
//returns if the window is open
bool insanity_ui_window_begin(const char* window_name, vec2s initial_percent_pos, vec2s initial_percent_size,
                              Insanity_UI_Window_Flags flags)
{
    Insanity_UI_Window* window_state = NULL;

    //find our window
    for (u32 i = 0; i < insanity_ui->window_state_count; i++)
    {
        if (strcmp(insanity_ui->window_state_array[i].name, window_name) == 0)
        {
            window_state = &insanity_ui->window_state_array[i];
            break;
        }
    }
    //if not found we create a new one
    if (!window_state)
    {
        window_state = &insanity_ui->window_state_array[insanity_ui->window_state_count++];

        *window_state = (Insanity_UI_Window){
            .name = c_string_duplicate_allocator(window_name, insanity_ui->allocator),
            .is_open = true,
            .intial_position_percent = initial_percent_pos,
            .intial_size_percent = initial_percent_size,
            .window_pos = glms_vec2_mul(insanity_ui->screen_size, initial_percent_pos),
            .window_size = glms_vec2_mul(insanity_ui->screen_size, initial_percent_size),
            // .header_pos = glms_vec2_zero(),
            // .header_size = glms_vec2_zero(),
            .scroll_offset = 0,
            .scroll_bar_percent_offset = 0,
            .window_relative_cursor_pos = glms_vec2_zero(),
            .flags = flags,
            .writable = true,
            // .pop_flags = ,
            .none_active = false,
            // .container_node = ,
            // .header_node =
        };
    }

    //window
    Insanity_UI_Node* window = insanity_ui_node("header");
    window->pos = window_state->window_pos;
    window->size = window_state->window_size;
    window->color = insanity_ui->editor_style.layout_color;
    if (!window_state->is_open)
    {
        window->size.y = 0;
    }

    //header
    Insanity_UI_Node* header = insanity_ui_node("header");
    header->pos = window_state->window_pos;
    header->size = (vec2s){window_state->window_size.x, insanity_ui->editor_font_size + insanity_ui->text_padding * 2};
    header->color = insanity_ui->editor_style.header_color;

    //header_text
    Insanity_UI_Node* header_text = insanity_ui_text(window_name);
    insanity_ui_node_align(header_text, header, UI_ALIGNMENT_LEFT, UI_ALIGNMENT_CENTER);
    insanity_ui_node_offset_x(header_text, header, insanity_ui->text_padding);


    //collapse
    Insanity_UI_Node* collapse_node = insanity_ui_node("collapse");
    collapse_node->size = (vec2s){insanity_ui->node_padding * 2, header->size.y * .6f};
    insanity_ui_node_align(collapse_node, header, UI_ALIGNMENT_RIGHT, UI_ALIGNMENT_CENTER);
    collapse_node->pos.x -= insanity_ui->node_padding;
    collapse_node->z_order = 1;
    collapse_node->color = COLOR_GREEN;

    //resize
    Insanity_UI_Node* resize_node = insanity_ui_node("resize");
    resize_node->size = (vec2s){12, 12};
    if (!window_state->is_open)
    {
        resize_node->size = (vec2s){0, 0};
    }
    vec2s pos_before_adjustment = glms_vec2_add(window_state->window_pos, window_state->window_size);
    resize_node->pos = glms_vec2_sub(pos_before_adjustment, resize_node->size);
    resize_node->color = COLOR_GREEN;

    Insanity_UI_Node* scrollbar = insanity_ui_node("scroll");


    //resolve any inputs
    Insanity_UI_Event header_result = insanity_ui_event(header, true, false);
    if (header_result.pressed)
    {
        window_state->window_pos.x += header_result.mouse_delta_x;
        window_state->window_pos.y += header_result.mouse_delta_y;
    }

    Insanity_UI_Event collapse_result = insanity_ui_event(collapse_node, true, true);
    if (collapse_result.hovered)
    {
        collapse_node->color = insanity_ui->editor_style.hovered_color;
    }
    if (collapse_result.pressed)
    {
        collapse_node->color = insanity_ui->editor_style.pressed_color;
    }
    if (collapse_result.clicked)
    {
        FATAL("Clicked");
        window_state->is_open = !window_state->is_open;
    }

    if (insanity_ui_event(resize_node, true, false).pressed)
    {
        window_state->window_size.x += madness_ui->mouse_delta_x;
        window_state->window_size.y += madness_ui->mouse_delta_y;
    }

    if (window_state->is_open)
    {
        collapse_node->color = COLOR_RED;
    }


    //validation of the window
    window_state->window_size.x = clamp_float(window_state->window_size.x, insanity_ui->window_min_size.x,
                                              madness_ui->screen_size.x);
    window_state->window_size.y = clamp_float(window_state->window_size.y, insanity_ui->window_min_size.y,
                                              madness_ui->screen_size.y);


    insanity_ui->cursor_position = window_state->window_pos;
    insanity_ui->cursor_position.y += header->size.y;


    //set proper cursor offset for the scroll region
    insanity_ui->cursor_position.y -= window_state->scroll_offset;


    stack_push(insanity_ui->window_states_stack, &window_state);


    return window_state->is_open;
}

void insanity_ui_window_end()
{
    Insanity_UI_Window window_state = stack_pop(insanity_ui->window_states_stack, Insanity_UI_Window);


    /*
    float scroll_region_start_pos = (state->window_region_pos.y + state->header_size.y);
    float scroll_region_size_y = (state->window_region_size.y - state->header_size.y);

    //NOTE: we add the scroll offset here because when we begin call insanity_ui_window_begin(),
    // the content start position is offset by the scroll amount, applied to the cursor_pos.y position
    // giving us a smaller size of what the content size should really be
    float content_height = madness_ui->cursor_pos.y - scroll_region_start_pos + state->scroll_offset;
    float content_overflow = content_height - scroll_region_size_y;

    if (content_overflow > 0)
    {
        //scroll bar
        UI_Node* slider_bar = madness_ui_get_new_node();
        slider_bar->size = (vec2s){8.f, state->window_region_size.y * 0.1};
        slider_bar->color = madness_ui->editor_style.layout_accent_color;
        slider_bar->string_id = *string_concat(state->window_name, &STRING("SLIDER"), madness_ui->frame_allocator);
        slider_bar->hash_id = string_hash_u64(slider_bar->string_id);
        slider_bar->color = COLOR_BLUE;

        //determines where the slider should be proportionally
        float scroll_bar_pos_x = state->window_region_pos.x + state->window_region_size.x - slider_bar->size.x;
        float scroll_bar_pos_y = scroll_region_start_pos + ((scroll_region_size_y - slider_bar->size.y) * state->
            scroll_bar_percent_offset);
        slider_bar->pos = (vec2s){scroll_bar_pos_x, scroll_bar_pos_y};

        madness_ui_set_interaction_state(slider_bar);
        if (is_hot(slider_bar->hash_id))
        {
            slider_bar->color = madness_ui->editor_style.hovered_color;
            //handle window scrolling
            if (input_is_mouse_wheel_up())
            {
                state->scroll_bar_percent_offset = clamp_float(state->scroll_bar_percent_offset - 0.1, 0, 1);
                state->scroll_offset = clamp_float(state->scroll_offset, 0, madness_ui->screen_size.y);
                state->scroll_offset = content_overflow * state->scroll_bar_percent_offset;
            }
            if (input_is_mouse_wheel_down())
            {
                // state.scroll_offset += 10;
                state->scroll_bar_percent_offset = clamp_float(state->scroll_bar_percent_offset + 0.1, 0, 1);
                state->scroll_offset = clamp_float(state->scroll_offset, 0, madness_ui->screen_size.y);
                state->scroll_offset = content_overflow * state->scroll_bar_percent_offset;
            }
        }
        if (is_active(slider_bar->hash_id))
        {
            float track_width = scroll_region_size_y - slider_bar->size.y;
            float relative_y = madness_ui->mouse_pos_y - scroll_region_start_pos - (slider_bar->size.y * 0.5f);
            float t = clamp_float(relative_y / track_width, 0.0f, 1.0f);

            state->scroll_bar_percent_offset = 0 + t * (1 - 0);
            state->scroll_offset = content_overflow * state->scroll_bar_percent_offset;
            slider_bar->pos.y = scroll_region_start_pos + ((scroll_region_size_y - slider_bar->size.y) * state->
                scroll_bar_percent_offset);
        }
    }

    if (content_overflow > 0)
    {
        state->window_region_size.y = content_height + (madness_ui_get_default_element_height() * 2);
        state->window_region_size.y = clamp_float(state->window_region_size.y, MIN_UI_NODE_SCREEN_SIZE,
                                                  madness_ui->screen_size.y - state->window_region_pos.y -
                                                  (madness_ui_get_default_element_height()));
    }*/
}


void insanity_ui_is_outside_window();



bool insanity_ui_popup_begin(const char* name, Insanity_UI_Pop_Up_Flags flags)
{
    Insanity_UI_Window current_window = insanity_ui_get_window();
    if (!current_window.writable) { return false; } // we are inside a windows and outside the view


    Insanity_UI_Node* pop_container = insanity_ui_node_pop_up();
    Insanity_UI_Window pop_up_window = {
        .name = name,
        .flags = 0,
        .pop_flags = flags,
        .intial_position_percent = 0,
        .intial_size_percent = 0,
        // .header_size = 0,
        // .header_pos = 0,
        .scroll_offset = 0,
        .scroll_bar_percent_offset = 0,
        .window_relative_cursor_pos = 0,
        .is_open = true,
        .none_active = false,
        .writable = true,
        .container_node = pop_container,
    };


    pop_container->pos = insanity_ui->cursor_position;

    /*
        if (flags & Insanity_UI_Pop_Up_Flags_No_Header)
        {
        }
        if (flags & Insanity_UI_Pop_Up_Flags_No_Scroll)
        {
        }
        if (flags & Insanity_UI_Pop_Up_Flags_No_Scroll_Mouse)
        {
        }
    */


    stack_push(insanity_ui->pop_up_states_stack, &pop_up_window);
}
void insanity_ui_popup_end()
{
    Insanity_UI_Window window_state = stack_pop(insanity_ui->pop_up_states_stack, Insanity_UI_Window);

    // madness_ui_set_cursor_pos(state->window_region_pos);
    // madness_ui_advance_cursor(state->window_region_size);
}


void insanity_ui_window_set_pos(vec2s pos);
void insanity_ui_window_set_size(vec2s size);

Insanity_UI_Window insanity_ui_get_window()
{
    if (!stack_is_empty(insanity_ui->modal_states_stack))
    {
        return stack_top(insanity_ui->modal_states_stack, Insanity_UI_Window);
    }
    if (!stack_is_empty(insanity_ui->pop_up_states_stack))
    {
        return stack_top(insanity_ui->pop_up_states_stack, Insanity_UI_Window);
    }
    if (!stack_is_empty(insanity_ui->window_states_stack))
    {
        return stack_top(insanity_ui->window_states_stack, Insanity_UI_Window);
    }

    Insanity_UI_Window window = {0};
    window.none_active = true;
    return window;
}


Insanity_UI_Event insanity_ui_checkbox(const char* name, bool* state);

Insanity_UI_Event insanity_ui_combo_box(const char* name);

//only one global menu bar available
void insanity_ui_menubar_begin(const char* name)
{
    //menu bar

    insanity_ui->cursor_position = (vec2s){0, 0};

    vec2s pos = {0};
    Insanity_UI_Node* menu_bar = insanity_ui_node("menu bar");


    Insanity_UI_Window window = {
        .name = name,
        .flags = 0,
        .is_open = false,
        .intial_position_percent = 0,
        .intial_size_percent = 0,
        .window_pos = {0, 0},
        .window_size = (vec2s){insanity_ui->screen_size.x, insanity_ui->node_padding * 4},
        // .header_size = 0,
        // .header_pos = 0,
        .scroll_offset = 0,
        .scroll_bar_percent_offset = 0,
        .window_relative_cursor_pos = 0,
        .container_node = menu_bar,
    };

    menu_bar->pos = window.window_pos;
    menu_bar->size = window.window_size;
    menu_bar->color = insanity_ui->editor_style.header_color;


    insanity_ui->cursor_position.x += insanity_ui->window_padding_x;
    // insanity_ui->cursor_position.y += insanity_ui->window_padding_y;


    stack_push(insanity_ui->window_states_stack, &window);
}
void insanity_ui_menubar_end(void)
{
    Insanity_UI_Window window = stack_pop(insanity_ui->window_states_stack, Insanity_UI_Window);
}

// bool return means its active, use to render widgets
bool insanity_ui_menubar_item(const char* name)
{
    Insanity_UI_Window window = stack_top(insanity_ui->window_states_stack, Insanity_UI_Window);


    Insanity_UI_Node* text_container = insanity_ui_node("container");
    text_container->pos = insanity_ui->cursor_position;
    text_container->color = insanity_ui->editor_style.header_color;

    Insanity_UI_Node* text = insanity_ui_text(name);
    text->pos = insanity_ui->cursor_position;


    //text container should be the size of the text minimum,
    //but expand out to its padding,
    //but constrained by the menu bars size
    // and centered by the menu bars size

    insanity_ui_node_expand_xy(text_container, text);
    text_container->size.x += insanity_ui->text_padding;
    text_container->size.y += insanity_ui->text_padding;


    insanity_ui_node_align_y(text_container, window.container_node, UI_ALIGNMENT_CENTER);
    insanity_ui_node_align(text, text_container, UI_ALIGNMENT_CENTER, UI_ALIGNMENT_CENTER);


    insanity_ui->cursor_position.x += text_container->size.x;
    insanity_ui->cursor_position.x += insanity_ui->node_padding;


    Insanity_UI_Event container_result = insanity_ui_event(text_container, true, true);
    if (container_result.hovered)
    {
        text_container->color = insanity_ui->editor_style.hovered_color;
    }

    return container_result.clicked;
}


Insanity_UI_Event insanity_ui_u64(const char* name, u64* value);
Insanity_UI_Event insanity_ui_s64(const char* name, u64* value);
Insanity_UI_Event insanity_ui_float(const char* name, f32* value);
Insanity_UI_Event insanity_ui_double(const char* name, f64* value);

// void insanity_ui_autoplace_and_size(Insanity_UI_Node* node);




void insanity_ui_editor_test(float dt, float elapsed_time)
{

    if (insanity_ui_window_begin("test", (vec2s){0.1, 0.1}, (vec2s){0.5, 0.5}, 0))
    {
        Insanity_UI_Event result = insanity_ui_button("button");
        if (result.hovered)
        {
            FATAL("HOVERED");
        }
        if (result.clicked)
        {
            FATAL("CLICKED");
        }
    }
    insanity_ui_window_end();

    insanity_ui_menubar_begin("menu");
    {
        if (insanity_ui_menubar_item("file"))
        {
            if (insanity_ui_popup_begin("file_explorer", 0))
            {
                insanity_ui_button("pop");
            }
            insanity_ui_popup_end();
        }
        insanity_ui_menubar_item("tools");
    }
    insanity_ui_menubar_end();
}

#endif //INSANITY_UI_EDITOR_H
