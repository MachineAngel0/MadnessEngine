#include "../resource/ui_madness.h"

#include "hash_table.h"
#include "logger.h"
#include "str.h"
#include "compiler/reflection_system.h"


void madness_ui_init(Memory_System* memory_system, Input_System* input_system,
                     Resource_System* resource_system)
{
    madness_ui = memory_system_alloc(memory_system, sizeof(Madness_UI), MEMORY_SUBSYSTEM_UI);

    u64 ui_arena_mem_size = MB(128);
    u64 ui_frame_arena_mem_size = MB(128);

    madness_ui->free_list_allocator = memory_system_alloc(memory_system, sizeof(Heap_Allocator),
                                                          MEMORY_SUBSYSTEM_UI);
    madness_ui->allocator = memory_system_alloc(memory_system, sizeof(Allocator), MEMORY_SUBSYSTEM_UI);
    madness_ui->frame_arena = memory_system_alloc(memory_system, sizeof(Allocator), MEMORY_SUBSYSTEM_UI);

    void* free_list_memory = memory_system_alloc(memory_system, ui_arena_mem_size, MEMORY_SUBSYSTEM_UI);
    void* allocator_memory = memory_system_alloc(memory_system, ui_arena_mem_size, MEMORY_SUBSYSTEM_UI);
    void* frame_allocator_memory = memory_system_alloc(memory_system, ui_frame_arena_mem_size, MEMORY_SUBSYSTEM_UI);

    allocator_heap_init(madness_ui->free_list_allocator, free_list_memory, ui_arena_mem_size);
    allocator_init(madness_ui->allocator, allocator_memory, ui_arena_mem_size);
    allocator_init(madness_ui->frame_arena, frame_allocator_memory, ui_arena_mem_size);

    madness_ui->input_system_reference = input_system;
    madness_ui->resource_system = resource_system;


    madness_ui->default_font_size = DEFAULT_FONT_CREATION_SIZE;
    madness_ui->editor_font_size = EDITOR_FONT_SIZE;

    madness_ui->ui_nodes = array_create(UI_Node, MAX_UI_NODE_COUNT, madness_ui->allocator);
    madness_ui->pop_up_ui_nodes = array_create(UI_Node,
                                               MAX_UI_NODE_COUNT, madness_ui->allocator);


    madness_ui->string_builder = string_builder_create(100, madness_ui->allocator);


    madness_ui->draw_command_list = dynamic_array_create(UI_Draw_Command, MAX_UI_DRAW_COUNT,
                                                         madness_ui->free_list_allocator);


    madness_ui->window_state_hash = hash_table_string_create(sizeof(Window_State),
                                                             10,
                                                             madness_ui->allocator,
                                                             false);


    madness_ui->pop_up_stack = stack_create(sizeof(Pop_Up_State), 100, madness_ui->allocator);
    madness_ui->pop_up_frame_state = array_create(Pop_Up_State, 100, madness_ui->allocator);
    madness_ui->nuke_pop_up = false;

    //TODO: change the allocator interface
    madness_ui->window_states_stack = stack_create(sizeof(Window_State), 100, madness_ui->allocator);


    madness_ui->window_pos_stack = stack_create(sizeof(vec2s), 100, madness_ui->allocator);
    madness_ui->window_size_stack = stack_create(sizeof(vec2s), 100, madness_ui->allocator);

    madness_ui->cursor_pos = glms_vec2_zero();

    madness_ui->menu_bar_state = (Menu_Bar_State){
        .menu_bar_pos = glms_vec2_zero(),
        .menu_bar_size = glms_vec2_zero(),
        .active_menu_item = STRING("INVALID"),
    };


    madness_ui->active = -1;
    madness_ui->hot = -1;


    madness_ui->mouse_pos_x = -1.0f;
    madness_ui->mouse_pos_y = -1.0f;

    madness_ui->mouse_down = 0;
    madness_ui->mouse_released_unique = 0;

    //TODO: replace with an in param
    madness_ui->screen_size = (vec2s){800.0f, 600.0f};

    madness_ui->element_padding_x = 8.0f;
    madness_ui->element_padding_y = 8.0f;

    madness_ui->text_padding_x = 8.0f;
    madness_ui->text_padding_y = 8.0f;


    //std::cout << "MOUSE STATE:" << Madness_UI.mouse_down << '\n';
    madness_ui->editor_style = (UI_Editor_Style){
        .layout_color = COLOR_PURPLE_PALETTE_DARK, .layout_accent_color = COLOR_PURPLE_PALETTE_PURPLE,
        .text_color = COLOR_PURPLE_PALETTE_LIGHT, .textbox_color = COLOR_PURPLE_PALETTE_DARK2,
        .custom_widget_color = COLOR_PURPLE_PALETTE_PURPLE_LIGHT,
        .color = COLOR_PURPLE_PALETTE_PURPLE_STRONG, .hovered_color = COLOR_PURPLE_PALETTE_PURPLE_LIGHT2,
        .pressed_color = COLOR_PURPLE_PALETTE_DARK2,
        .outline_color = COLOR_PURPLE_PALETTE_PURPLE_LIGHT,
        .permanent_active = COLOR_HOT_PINK,
        .header_color = (vec3s){0.425, 0.05, 0.456},
        .pop_up_color = (vec3s){0.110, 0.120, 0.162},
    };


    //TODO: THERE IS NO WAY I AM LOADING THIS FILE FROM THERE, WHAT ABOUT LINUX
    // font_init(madness_ui, renderer, "c:/windows/fonts/arialbd.ttf");
    // if (!texture_system_load_font(resource_system->texture_system, "../z_assets/fonts/arialbd.ttf",
    //                               &madness_ui->default_font_handle,
    //                               madness_ui->arena))
    // {
    //     MASSERT_MSG(false, "UI SYSTEM Failed to load default font");
    // };

    if (!texture_system_load_msdf_font(resource_system->texture_system, "../z_assets/msdf_fonts/arial_msdf.png",
                                       &madness_ui->default_font_handle,
                                       madness_ui->allocator))
    {
        MASSERT_MSG(false, "UI SYSTEM Failed to load default msdf font");
    };


    INFO("MADNESS UI SYSTEM CREATED");

}

bool madness_ui_shutdown(void)
{
    return true;
}


void madness_ui_begin(s32 screen_size_x, s32 screen_size_y)
{
    //clear draw info and reset the hot id
    allocator_clear(madness_ui->frame_arena);


    //on resize, scale the ui window size's accordingly
    if (madness_ui->screen_size.x != screen_size_x || madness_ui->screen_size.y != screen_size_y)
    {
        float x_update_size = screen_size_x / madness_ui->screen_size.x;
        float y_update_size = screen_size_y / madness_ui->screen_size.y;


        //do a state update on the window positions
        hash_table_string* hts = madness_ui->window_state_hash;
        for (u32 i = 0; i < hts->capacity; i++)
        {
            if (hts->key_data[i].chars)
            {
                Window_State window_state;
                hash_table_string_get(hts, hts->key_data[i], &window_state);
                window_state.window_region_pos.x *= x_update_size;
                window_state.window_region_pos.y *= y_update_size;
                window_state.window_region_size.x *= x_update_size;
                window_state.window_region_size.y *= y_update_size;
                window_state.scroll_bar_percent_offset = 0;
                window_state.scroll_offset = 0;


                hash_table_string_set(hts, hts->key_data[i], &window_state);
            }
        }
    }

    madness_ui->screen_size.x = screen_size_x;
    madness_ui->screen_size.y = screen_size_y;


    madness_ui->prev_item_size = glms_vec2_zero();
    stack_clear(madness_ui->window_pos_stack);
    stack_clear(madness_ui->window_size_stack);

    array_zero(madness_ui->ui_nodes);
    array_clear(madness_ui->ui_nodes);

    array_zero(madness_ui->pop_up_ui_nodes);
    array_clear(madness_ui->pop_up_ui_nodes);


    madness_ui->current_draw_command.count = 0;
    madness_ui->current_draw_command.offset = 0;
    madness_ui->current_draw_command.type = UI_DRAW_TYPE_DRAW;
    dynamic_array_clear(madness_ui->draw_command_list);


    stack_clear(madness_ui->window_states_stack);
    stack_clear(madness_ui->pop_up_stack);


    madness_ui->cursor_pos = glms_vec2_zero();
    madness_ui->prev_item_size = glms_vec2_zero();


    madness_ui->current_window_screen_pos = glms_vec2_zero();
    madness_ui->current_window_screen_size = glms_vec2_zero();


    madness_ui->hot = -1;

    madness_ui->mouse_down = input_is_mouse_button_pressed(madness_ui->input_system_reference, MOUSE_BUTTON_LEFT);
    madness_ui->mouse_released_unique = input_is_mouse_button_released_unique(
        madness_ui->input_system_reference, MOUSE_BUTTON_LEFT);
    //this can be 0 if invalid
    madness_ui->released_key = input_get_first_released_key(madness_ui->input_system_reference);

    if (madness_ui->nuke_pop_up == true)
    {
        madness_ui->active_combo_box = STRING("INVALID");
        madness_ui->menu_bar_state.active_menu_item = STRING("INVALID");
    }
    else if (madness_ui->mouse_released_unique)
    {
        for (u32 i = 0; i < madness_ui->pop_up_frame_state->num_items; i++)
        {
            Pop_Up_State pop_up_state = array_get(madness_ui->pop_up_frame_state, Pop_Up_State, i);

            if (!region_hit(pop_up_state.pop_up_pos, pop_up_state.pop_up_size))
            {
                madness_ui->active_combo_box = STRING("INVALID");
                madness_ui->menu_bar_state.active_menu_item = STRING("INVALID");
                break;
            }
        }
    }


    madness_ui->nuke_pop_up = false;
    array_clear(madness_ui->pop_up_frame_state);
}


void madness_ui_end(void)
{
    //add the deffered draw list
    for (u32 i = 0; i < madness_ui->pop_up_ui_nodes->num_items; i++)
    {
        UI_Node* deffered_node = (UI_Node*)_array_get(madness_ui->pop_up_ui_nodes, i);
        if (array_get(madness_ui->pop_up_ui_nodes, UI_Node, i).flags & UI_FLAG_SCISSOR_START)
        {
            madness_ui_new_scissor_start(deffered_node->scissor_pos, deffered_node->scissor_size);
        }
        else if (array_get(madness_ui->pop_up_ui_nodes, UI_Node, i).flags & UI_FLAG_SCISSOR_END)
        {
            madness_ui_new_scissor_end();
        }
        else
        {
            UI_Node* node = madness_ui_get_new_node();
            memcpy(node, deffered_node, sizeof(UI_Node));
        }
    }

    //push anything left into the draw list
    dynamic_array_push(madness_ui->draw_command_list, &madness_ui->current_draw_command);

    //Generate Draw Data
    // madness_ui->ui_draw_data = allocator_alloc(madness_ui->frame_arena,
    //                                            madness_ui->ui_nodes->num_items * sizeof(UI_Node_Draw_Data));
    madness_ui->ui_draw_data = allocator_alloc(madness_ui->frame_arena,
                                               madness_ui->ui_nodes->num_items * sizeof(UI_Node_Draw_Data));
    madness_ui->ui_draw_data_count = madness_ui->ui_nodes->num_items; // needs to be here for the render draw count

    for (u32 i = 0; i < madness_ui->ui_nodes->num_items; i++)
    {
        UI_Node* node_data = (UI_Node*)_array_get(madness_ui->ui_nodes, i);

        // UI_Node* node_data = &array_get(madness_ui->ui_nodes, UI_Node, i);
        UI_Node_Draw_Data* draw_data = &madness_ui->ui_draw_data[i];

        draw_data->ui_flags = node_data->flags;
        draw_data->pos = glms_vec2_div(node_data->pos, madness_ui->screen_size);
        draw_data->size = glms_vec2_div(node_data->size, madness_ui->screen_size);
        draw_data->rotation = deg_to_rad(node_data->rotation);

        draw_data->thickness = node_data->thickness;

        draw_data->outline_thickness = node_data->outline_thickness;

        draw_data->color = node_data->color;

        draw_data->texture_handle = node_data->texture_handle.handle;
        draw_data->uv_offset = node_data->uv_offset;
        draw_data->uv_size = node_data->uv_size;

        draw_data->outline_color = node_data->outline_color;
        draw_data->outline_thickness = node_data->outline_thickness;

        // draw_data->rounded_radius = node_data->rounded_radius;
        // draw_data->background_color = node_data->background_color;
    }


    //SET UI STATE FOR NEXT FRAME //

    //check if mouse is released, if so reset the active id
    //also update the mouse state

    //printf("HOT ID: %d, HOT LAYER: %d\n", Madness_UI->hot.ID, Madness_UI->hot.layer);
    //printf("ACTIVE ID: %d, ACTIVE LAYER: %d\n", Madness_UI->active.ID, Madness_UI->active.layer);
    //TODO: clear anything whos state relies on a click


    if (input_is_mouse_button_released(madness_ui->input_system_reference, MOUSE_BUTTON_LEFT))
    {
        madness_ui->active = -1;
    }

    //update mouse state
    // DEBUG("MOUSE DOWN %d", Madness_UI->mouse_down)
    //update mouse pos
    input_get_mouse_pos(madness_ui->input_system_reference, &madness_ui->mouse_pos_x, &madness_ui->mouse_pos_y);
    //update mouse delta/change
    input_get_mouse_change(madness_ui->input_system_reference, &madness_ui->mouse_delta_x, &madness_ui->mouse_delta_y);
}

UI_Render_Packet madness_ui_get_ui_render_data(void)
{
    return (UI_Render_Packet){
        .ui_material_data = madness_ui->ui_draw_data,
        .ui_material_data_count = madness_ui->ui_draw_data_count,
        .ui_material_bytes = madness_ui->ui_draw_data_count * sizeof(UI_Node_Draw_Data),
        .draw_command = madness_ui->draw_command_list->data,
        .draw_command_count = madness_ui->draw_command_list->num_items,
    };
}


void madness_ui_print_state(void)
{
    DEBUG("HOT: %d, ACTIVE: %d, CURSOR POS: %f %f, MOUSE DOWN: %d,",
          madness_ui->hot,
          madness_ui->active,
          madness_ui->cursor_pos.x,
          madness_ui->cursor_pos.y,
          madness_ui->mouse_down)
    // madness_ui->mouse_pos_x;
    // madness_ui->mouse_pos_y;
}


bool is_ui_hot(int id)
{
    return madness_ui->hot == id;
}

bool is_ui_active(int id)
{
    return madness_ui->active == id;
}


bool region_hit(vec2s pos, vec2s size)
{
    //check if we are inside a ui_object
    //we are using the screen coordinates from the mouse,
    //and hopefully the passed in pos and size

    //top left
    if (pos.x > madness_ui->mouse_pos_x) return false;
    if (pos.y > madness_ui->mouse_pos_y) return false;

    // bottom left
    if (pos.x > madness_ui->mouse_pos_x) return false;
    if (pos.y + size.y < madness_ui->mouse_pos_y) return false;


    //top right
    if (pos.x + size.x < madness_ui->mouse_pos_x) return false;
    if (pos.y > madness_ui->mouse_pos_y) return false;

    // bottom right
    if (pos.x + size.x < madness_ui->mouse_pos_x) return false;
    if (pos.y + size.y < madness_ui->mouse_pos_y) return false;

    return true;
}


/*
bool button(Madness_UI& Madness_UI, int id, int x, int y)
{
    if (region_hit())
    {
        ui = hot;
        if (acitve == 0 and mouse down)
            activeitem = id
    }
}*/

//check if we can use the button
bool madness_ui_use_ui_element(int id, vec2s pos, vec2s size)
{
    //checking if we released the mouse button, are active, and we are inside the hit region

    if (madness_ui->mouse_down == false &&
        madness_ui->active == id &&
        region_hit(pos, size))
    {
        return true;
    }


    return false;
}


void set_hot(int id)
{
    madness_ui->hot = id;
    //printf("ID: %d, is hot\n", id);
}

void set_active(int id)
{
    madness_ui->active = id;
}

bool can_be_active(void)
{
    return madness_ui->active == -1 && madness_ui->mouse_down;
}

bool is_active(int id)
{
    return madness_ui->active == id;
}

bool is_hot(int id)
{
    return madness_ui->hot == id;
}

void madness_ui_add_draw_command(UI_Draw_Command_Type draw_type)
{
    if (madness_ui->current_draw_command.type == draw_type)
    {
        madness_ui->current_draw_command.count++;
    }
    else
    {
        if (madness_ui->current_draw_command.count > 0) // don't push empty commands
        {
            dynamic_array_push(madness_ui->draw_command_list, &madness_ui->current_draw_command);
            madness_ui->current_draw_command.offset += madness_ui->current_draw_command.count;
        }
        madness_ui->current_draw_command.count = 1; // one since we are adding a new draw
        madness_ui->current_draw_command.type = draw_type;
    }
}


UI_Node* madness_ui_get_new_node(void)
{
    MASSERT(madness_ui->ui_nodes->num_items < madness_ui->ui_nodes->capacity);
    //check if we get a pop up node or a normal node
    if (!stack_is_empty(madness_ui->pop_up_stack))
    {
        MASSERT(madness_ui->pop_up_ui_nodes->num_items < madness_ui->pop_up_ui_nodes->capacity);
        UI_Node* out_node = (UI_Node*)_array_get(madness_ui->ui_nodes, madness_ui->ui_nodes->num_items++);

        return out_node;
    }
    // UI_Node* out_node = &madness_ui->ui_nodes->data[madness_ui->ui_nodes->num_items++];
    UI_Node* out_node = (UI_Node*)_array_get(madness_ui->ui_nodes, madness_ui->ui_nodes->num_items++);

    madness_ui_add_draw_command(UI_DRAW_TYPE_DRAW);
    return out_node;
}


UI_Node* madness_ui_get_pop_up_node(void)
{
    UI_Node* out_node = (UI_Node*)_array_get(madness_ui->pop_up_ui_nodes, madness_ui->pop_up_ui_nodes->num_items++);

    return out_node;
}


UI_Node* madness_ui_new_scissor_start(vec2s scissor_pos, vec2s scissor_size)
{
    //UI_node is passed out in the event the scissor size is not really known

    //check if we get a pop up node or a normal node
    if (!stack_is_empty(madness_ui->pop_up_stack))
    {
        UI_Node* out_node = (UI_Node*)_array_get(madness_ui->pop_up_ui_nodes, madness_ui->pop_up_ui_nodes->num_items++);

        out_node->flags |= UI_FLAG_SCISSOR_START;
        out_node->scissor_pos = scissor_pos;
        out_node->scissor_size = scissor_size;
        return out_node;
    }

    UI_Node* scissor_node = _array_get(madness_ui->ui_nodes, madness_ui->ui_nodes->num_items++);
    scissor_node->flags |= UI_FLAG_SCISSOR_START;
    scissor_node->scissor_pos = scissor_pos;
    scissor_node->scissor_size = scissor_size;

    madness_ui_add_draw_command(UI_DRAW_TYPE_SCISSOR_START);

    return scissor_node;
}

void madness_ui_new_scissor_end(void)
{
    if (!stack_is_empty(madness_ui->pop_up_stack))
    {
        UI_Node* scissor_node = _array_get(madness_ui->pop_up_ui_nodes, madness_ui->pop_up_ui_nodes->num_items++);
        scissor_node->flags |= UI_FLAG_SCISSOR_END;
        return;
    }

    UI_Node* scissor_node = _array_get(madness_ui->ui_nodes, madness_ui->ui_nodes->num_items++);
    scissor_node->flags |= UI_FLAG_SCISSOR_END;
    madness_ui_add_draw_command(UI_DRAW_TYPE_SCISSOR_END);
}


UI_Node* madness_ui_get_last_used_node(Madness_UI* madness_ui)
{
    if (madness_ui->ui_nodes->num_items > 0)
    {
        return _array_get(madness_ui->ui_nodes, madness_ui->ui_nodes->num_items - 1);
    }
    return NULL;
}


void madness_ui_same_line(void)
{
    madness_ui->cursor_pos.x = madness_ui->prev_line.x + madness_ui->prev_item_size.x + madness_ui->element_padding_x;
    madness_ui->cursor_pos.y -= madness_ui->prev_item_size.y + madness_ui->element_padding_y;
}

void madness_ui_advance_cursor(vec2s ui_screen_size)
{
    // if (madness_ui->same_line)
    // {
    //     madness_ui->cursor_pos.x += ui_screen_size.x;
    // }
    // else
    // {
    //     madness_ui->cursor_pos.x = madness_ui->current_layout_screen_pos.x;
    //     madness_ui->cursor_pos.y += ui_screen_size.y; // + madness_ui->element_padding_y;
    // }

    madness_ui->prev_item_size = ui_screen_size;
    madness_ui->prev_line = madness_ui->cursor_pos;
    madness_ui->cursor_pos.x = madness_ui->current_window_screen_pos.x + madness_ui->element_padding_x;
    madness_ui->cursor_pos.y += madness_ui->prev_item_size.y + madness_ui->element_padding_y;
}

void madness_ui_advance_cursor_horizontal(vec2s ui_screen_size)
{
    //only meant for the menu bar
    madness_ui->cursor_pos.x = ui_screen_size.x + madness_ui->element_padding_x;
}


void madness_ui_center_child_node(vec2s parent_pos, vec2s parent_size, vec2s child_size, vec2s* out_pos)
{
    //find the remaining space vertically and horizontally,
    //dividing that by two will give us the offset needed to add to the parent object to center it
    vec2s size_diff = glms_vec2_sub(parent_size, child_size);
    vec2s size_diff_centered = glms_vec2_scale(size_diff, 2);
    *out_pos = glms_vec2_add(parent_pos, size_diff_centered);
}

char* madness_ui_float_to_char(const float value)
{
    int len = snprintf(NULL, 0, "%.3f", value);
    char* result = allocator_alloc(madness_ui->frame_arena, len + 1);
    snprintf(result, len + 1, "%.3f", value);

    return result;
}

char* madness_ui_int_to_char(Madness_UI* madness_ui, const u32 value)
{
    int len = snprintf(NULL, 0, "%u", value);
    char* result = allocator_alloc(madness_ui->frame_arena, len + 1);
    snprintf(result, len + 1, "%u", value);

    return result;
}


void madness_ui_menu_bar_begin(String id)
{
    //create the title bar
    vec2s header_size = {
        madness_ui->screen_size.x,
        32.f
    };
    vec2s menu_pos = {0, 0};
    madness_ui->cursor_pos = menu_pos;
    madness_ui->cursor_pos.x += madness_ui->element_padding_x;


    madness_ui_new_scissor_start(menu_pos, header_size);


    UI_Node* title_bar_node = madness_ui_get_new_node();
    title_bar_node->pos = menu_pos;
    title_bar_node->size = header_size;
    title_bar_node->color = madness_ui->editor_style.textbox_color; // TODO:
    title_bar_node->string_id = id;

    madness_ui->menu_bar_state = (Menu_Bar_State){
        .menu_bar_pos = title_bar_node->pos,
        .menu_bar_size = title_bar_node->size,
        .menu_cursor_position = (vec2s){title_bar_node->pos.x + madness_ui->element_padding_x, title_bar_node->pos.y},
        .active_menu_item = madness_ui->menu_bar_state.active_menu_item,
    };

    madness_ui->cursor_pos.x += madness_ui->element_padding_x;
}

void madness_ui_menu_bar_end(void)
{
    madness_ui_new_scissor_end();
}


bool madness_ui_menu_item_begin(String menu_name)
{
    vec2s text_size = madness_ui_get_text_size(menu_name);
    vec2s button_size = (vec2s){
        text_size.x + madness_ui->text_padding_x,
        madness_ui_get_default_element_height(),
    };

    UI_Node* background_node = madness_ui_get_new_node();
    background_node->pos = madness_ui->menu_bar_state.menu_cursor_position;
    background_node->size = button_size;
    //center node
    float vertical_space_remaining = madness_ui->menu_bar_state.menu_bar_size.y - background_node->size.y;
    background_node->pos.y += (vertical_space_remaining / 2);

    background_node->string_id = menu_name;
    background_node->hash_id = string_hash_u64(menu_name);
    background_node->color = madness_ui->editor_style.textbox_color;


    madness_ui_string_internal(menu_name, background_node->pos, background_node->size,
                               UI_ALIGNMENT_CENTER,
                               UI_ALIGNMENT_CENTER);


    madness_ui->menu_bar_state.menu_cursor_position.x += background_node->size.x + madness_ui->element_padding_x;


    madness_ui_set_interaction_state(background_node);

    if (is_hot(background_node->hash_id))
    {
        background_node->color = madness_ui->editor_style.hovered_color;
    }
    if (is_active(background_node->hash_id))
    {
        background_node->color = madness_ui->editor_style.pressed_color;
    }


    if (madness_ui_use_ui_element(background_node->hash_id, background_node->pos, background_node->size))
    {
        madness_ui->menu_bar_state.active_menu_item = menu_name;
    }
    if (string_compare(&madness_ui->menu_bar_state.active_menu_item, &menu_name))
    {
        background_node->color = madness_ui->editor_style.permanent_active;
        madness_ui->current_window_screen_pos = madness_ui->cursor_pos;
        madness_ui->current_window_screen_size = glms_vec2_scale(background_node->size, 3.f);

        madness_ui_pop_up_open(menu_name, (vec2s){
                                   background_node->pos.x,
                                   background_node->pos.y + background_node->size.y + madness_ui->element_padding_y
                               });

        return true;
    }


    return false;
}

bool madness_ui_menu_item_end(void)
{
    madness_ui_pop_up_close();
    return false;
}

bool madness_ui_pop_up_open(String pop_up_name, vec2s pop_up_start_location)
{
    UI_Node* pop_up_scissor = madness_ui_get_pop_up_node();
    pop_up_scissor->flags |= UI_FLAG_SCISSOR_START;
    UI_Node* pop_up_start_node = madness_ui_get_pop_up_node();

    Pop_Up_State pop_up_state = {
        .pop_up_name = pop_up_name,
        .cursor_original_pos = madness_ui->cursor_pos,
        .pop_up_pos = pop_up_start_location,
        .pop_up_size = glms_vec2_zero(),
        .pop_up_node = pop_up_start_node,
        .pop_up_scissor_start_node = pop_up_scissor,
    };


    madness_ui->cursor_pos = pop_up_start_location;
    madness_ui->cursor_pos.x += madness_ui->element_padding_x;
    madness_ui->cursor_pos.y += madness_ui->element_padding_y;
    madness_ui->current_window_screen_pos = pop_up_start_location;


    stack_push(madness_ui->pop_up_stack, &pop_up_state);
    return false;
}

bool madness_ui_pop_up_close(void)
{
    if (stack_is_empty(madness_ui->pop_up_stack)) return false;


    Pop_Up_State state = stack_top(madness_ui->pop_up_stack, Pop_Up_State);

    float content_height = madness_ui->cursor_pos.y - state.cursor_original_pos.y;

    state.pop_up_node->pos = state.pop_up_pos;
    state.pop_up_node->color = madness_ui->editor_style.pop_up_color;
    state.pop_up_node->size = (vec2s){
        madness_ui->current_window_screen_size.x + madness_ui->element_padding_x,
        content_height + madness_ui->element_padding_y
    };
    state.pop_up_node->outline_color = COLOR_ORANGE;
    state.pop_up_node->outline_thickness = 0.02;
    state.pop_up_size = state.pop_up_node->size;

    state.pop_up_scissor_start_node->scissor_size = (vec2s){MIN_UI_NODE_SCREEN_SIZE, content_height};


    madness_ui->cursor_pos = state.cursor_original_pos;

    array_push(madness_ui->pop_up_frame_state, &state);

    madness_ui_new_scissor_end();
    stack_pop(madness_ui->pop_up_stack);

    return false;
}


void madness_ui_window_begin(String header_name)
{
    Window_State window_state;
    if (!hash_table_string_get(madness_ui->window_state_hash, header_name, &window_state))
    {
        vec2s pos = madness_ui_get_window_pos();
        vec2s size = madness_ui_get_window_size();

        vec2s starting_size = {.x = 0.2, .y = 0.8};
        window_state = (Window_State){
            .window_name = header_name,
            .window_type = UI_WINDOW_TYPE_WINDOW,
            .window_region_pos = pos,
            .window_region_size = size,
            .header_size = glms_vec2_zero(),
            .scroll_offset = 0,
        };
        hash_table_string_insert(madness_ui->window_state_hash, header_name, &window_state);
    }


    madness_ui->current_window_screen_pos = window_state.window_region_pos;
    madness_ui->current_window_screen_size = window_state.window_region_size;

    madness_ui->cursor_pos = madness_ui->current_window_screen_pos;
    madness_ui->cursor_pos.x += madness_ui->element_padding_x;


    //create the background
    UI_Node* background_node = madness_ui_get_new_node();
    background_node->pos = madness_ui->current_window_screen_pos;
    background_node->size = madness_ui->current_window_screen_size;
    background_node->color = madness_ui->editor_style.layout_color;
    background_node->string_id = header_name;


    //create the header
    vec2s header_size = {
        madness_ui->current_window_screen_size.x,
        madness_ui_get_default_element_height(),
    };

    UI_Node* header_node = madness_ui_get_new_node();
    header_node->pos = madness_ui->current_window_screen_pos;
    header_node->size = header_size;
    header_node->color = madness_ui->editor_style.header_color;
    header_node->string_id = header_name;
    header_node->hash_id = string_hash_u64(header_name);

    window_state.header_size = header_node->size;

    //Window Name
    // madness_ui_text_new(madness_ui, header_name);
    madness_ui_string_internal(header_name, madness_ui->cursor_pos, header_node->size, UI_ALIGNMENT_LEFT,
                               UI_ALIGNMENT_CENTER);

    madness_ui_advance_cursor(header_node->size);


    madness_ui_new_scissor_start(glms_vec2_add(madness_ui->current_window_screen_pos, header_size),
                                 (vec2s){
                                     madness_ui->current_window_screen_size.x,
                                     madness_ui->current_window_screen_size.y - header_size.y
                                 });


    madness_ui_set_interaction_state(header_node);
    if (is_active(header_node->hash_id))
    {
        if (madness_ui->mouse_down)
        {
            window_state.window_region_pos.x += madness_ui->mouse_delta_x;
            window_state.window_region_pos.y += madness_ui->mouse_delta_y;
        }
    }
    else if (is_hot(header_node->hash_id))
    {
        header_node->color = madness_ui->editor_style.hovered_color;
    }


    //set proper cursor offset for the scroll region
    madness_ui->cursor_pos.y -= window_state.scroll_offset;


    hash_table_string_set(madness_ui->window_state_hash, header_name, &window_state);
    stack_push(madness_ui->window_states_stack, &window_state);
}


void madness_ui_window_end(void)
{
    madness_ui_new_scissor_end();

    Window_State state = stack_top(madness_ui->window_states_stack, Window_State);
    stack_pop(madness_ui->window_states_stack);

    float scroll_region_start_pos = (state.window_region_pos.y + state.header_size.y);
    float scroll_region_size_y = (state.window_region_size.y - state.header_size.y);

    //NOTE: we add the scroll offset here because when we begin call madness_ui_window_begin(),
    // the content start position is offset by the scroll amount, applied to the cursor_pos.y position
    // giving us a smaller size of what the content size should really be
    float content_height = madness_ui->cursor_pos.y - scroll_region_start_pos + state.scroll_offset;
    float content_overflow = content_height - scroll_region_size_y;

    if (content_overflow > 0)
    {
        //scroll bar
        UI_Node* slider_bar = madness_ui_get_new_node();
        slider_bar->size = (vec2s){8.f, state.window_region_size.y * 0.1};
        slider_bar->color = madness_ui->editor_style.layout_accent_color;
        slider_bar->string_id = *string_concat(&state.window_name, &STRING("SLIDER"), madness_ui->frame_arena);
        slider_bar->hash_id = string_hash_u64(slider_bar->string_id);
        slider_bar->color = COLOR_BLUE;

        //determines where the slider should be proportionally
        float scroll_bar_pos_x = state.window_region_pos.x + state.window_region_size.x - slider_bar->size.x;
        float scroll_bar_pos_y = scroll_region_start_pos + ((scroll_region_size_y - slider_bar->size.y) * state.
            scroll_bar_percent_offset);
        slider_bar->pos = (vec2s){scroll_bar_pos_x, scroll_bar_pos_y};

        madness_ui_set_interaction_state(slider_bar);
        if (is_hot(slider_bar->hash_id))
        {
            slider_bar->color = madness_ui->editor_style.hovered_color;
            //handle window scrolling
            if (input_is_mouse_wheel_up(madness_ui->input_system_reference))
            {
                state.scroll_bar_percent_offset = clamp_float(state.scroll_bar_percent_offset - 0.1, 0, 1);
                state.scroll_offset = clamp_float(state.scroll_offset, 0, madness_ui->screen_size.y);
                state.scroll_offset = content_overflow * state.scroll_bar_percent_offset;
            }
            if (input_is_mouse_wheel_down(madness_ui->input_system_reference))
            {
                // state.scroll_offset += 10;
                state.scroll_bar_percent_offset = clamp_float(state.scroll_bar_percent_offset + 0.1, 0, 1);
                state.scroll_offset = clamp_float(state.scroll_offset, 0, madness_ui->screen_size.y);
                state.scroll_offset = content_overflow * state.scroll_bar_percent_offset;
            }
        }
        if (is_active(slider_bar->hash_id))
        {
            float track_width = scroll_region_size_y - slider_bar->size.y;
            float relative_y = madness_ui->mouse_pos_y - scroll_region_start_pos - (slider_bar->size.y * 0.5f);
            float t = clamp_float(relative_y / track_width, 0.0f, 1.0f);

            state.scroll_bar_percent_offset = 0 + t * (1 - 0);
            state.scroll_offset = content_overflow * state.scroll_bar_percent_offset;
            slider_bar->pos.y = scroll_region_start_pos + ((scroll_region_size_y - slider_bar->size.y) * state.
                scroll_bar_percent_offset);
        }
    }

    // resize bar

    UI_Node* resize_node = madness_ui_get_new_node();
    resize_node->size = (vec2s){12, 12};
    resize_node->string_id = *string_concat(&state.window_name, &STRING("resize"), madness_ui->frame_arena);
    resize_node->hash_id = string_hash_u64(resize_node->string_id);

    vec2s pos_before_adjustment = glms_vec2_add(state.window_region_pos, state.window_region_size);

    resize_node->pos = glms_vec2_sub(pos_before_adjustment, resize_node->size);
    resize_node->color = COLOR_GREEN;

    madness_ui_set_interaction_state(resize_node);

    if (is_active(resize_node->hash_id))
    {
        state.window_region_size.x += madness_ui->mouse_delta_x;
        state.window_region_size.y += madness_ui->mouse_delta_y;
        state.window_region_size.x = clamp_float(state.window_region_size.x, MIN_UI_NODE_SCREEN_SIZE,
                                                 madness_ui->screen_size.x);
        state.window_region_size.y = clamp_float(state.window_region_size.y, MIN_UI_NODE_SCREEN_SIZE,
                                                 madness_ui->screen_size.y);
    }

    hash_table_string_set(madness_ui->window_state_hash, state.window_name, &state);
}


void madness_scroll_box_begin(String id)
{
    u32 scroll_height_in_elements = 5;
    vec2s scroll_box_size = (vec2s){
        madness_ui->current_window_screen_size.x * 0.95,
        madness_ui_get_default_element_height() * scroll_height_in_elements
    };

    madness_ui_new_scissor_start(madness_ui->cursor_pos, scroll_box_size);

    UI_Node* scroll_box_node = madness_ui_get_new_node();
    scroll_box_node->string_id = id;
    scroll_box_node->pos = madness_ui->cursor_pos;
    scroll_box_node->size = scroll_box_size;
    scroll_box_node->color = COLOR_HOT_PINK;

    //we mainly just want the scroll bar offset
    Window_State window_state;
    if (!hash_table_string_get(madness_ui->window_state_hash, id, &window_state))
    {
        // vec2 pos = madness_ui_get_window_pos(madness_ui);
        // vec2 size = madness_ui_get_window_size(madness_ui);

        window_state = (Window_State){
            .window_name = id,
            .window_type = UI_WINDOW_TYPE_SCROLLBAR,
            .window_region_pos = scroll_box_node->pos,
            .window_region_size = scroll_box_node->size,
            .header_size = glms_vec2_zero(),
            .scroll_bar_percent_offset = 0,
            .scroll_offset = 0,
        };
        hash_table_string_insert(madness_ui->window_state_hash, id, &window_state);
    }


    window_state.window_region_pos = scroll_box_node->pos;
    window_state.window_region_size = scroll_box_node->size;

    //offset by scroll offset
    madness_ui->cursor_pos.y -= window_state.scroll_offset;

    stack_push(madness_ui->window_states_stack, &window_state);
}


void madness_scroll_box_end(void)
{
    madness_ui_new_scissor_end();

    Window_State state = stack_top(madness_ui->window_states_stack, Window_State);
    stack_pop(madness_ui->window_states_stack);


    //scroll bar
    float scroll_region_start_pos = (state.window_region_pos.y + state.header_size.y);
    float scroll_region_size_y = (state.window_region_size.y - state.header_size.y);

    float content_height = madness_ui->cursor_pos.y - scroll_region_start_pos + state.scroll_offset;
    float content_overflow = content_height - scroll_region_size_y;

    if (content_overflow > 0)
    {
        UI_Node* slider_bar = madness_ui_get_new_node();
        slider_bar->size = (vec2s){8.f, state.window_region_size.y * 0.1};
        slider_bar->color = madness_ui->editor_style.layout_accent_color;
        slider_bar->string_id = *string_concat(&state.window_name, &STRING("SLIDER"), madness_ui->frame_arena);
        slider_bar->hash_id = string_hash_u64(slider_bar->string_id);
        slider_bar->color = COLOR_BLUE;

        //determines where the slider should be proportionally
        float scroll_bar_pos_x = state.window_region_pos.x + state.window_region_size.x - slider_bar->size.x;
        float scroll_bar_pos_y = scroll_region_start_pos + ((scroll_region_size_y - slider_bar->size.y) * state.
            scroll_bar_percent_offset);
        slider_bar->pos = (vec2s){scroll_bar_pos_x, scroll_bar_pos_y};

        madness_ui_set_interaction_state(slider_bar);
        if (is_hot(slider_bar->hash_id))
        {
            slider_bar->color = madness_ui->editor_style.hovered_color;
            //handle window scrolling
            if (input_is_mouse_wheel_up(madness_ui->input_system_reference))
            {
                state.scroll_bar_percent_offset = clamp_float(state.scroll_bar_percent_offset - 0.1, 0, 1);
                state.scroll_offset = clamp_float(state.scroll_offset, 0, madness_ui->screen_size.y);
            }
            if (input_is_mouse_wheel_down(madness_ui->input_system_reference))
            {
                // state.scroll_offset += 10;
                state.scroll_bar_percent_offset = clamp_float(state.scroll_bar_percent_offset + 0.1, 0, 1);
                state.scroll_offset = clamp_float(state.scroll_offset, 0, madness_ui->screen_size.y);
            }
        }
        if (is_active(slider_bar->hash_id))
        {
            float track_width = scroll_region_size_y - slider_bar->size.y;
            float relative_y = madness_ui->mouse_pos_y - scroll_region_start_pos - (slider_bar->size.y * 0.5f);
            float t = clamp_float(relative_y / track_width, 0.0f, 1.0f);

            state.scroll_bar_percent_offset = 0 + t * (1 - 0);
            state.scroll_offset = content_overflow * state.scroll_bar_percent_offset;
            slider_bar->pos.y = scroll_region_start_pos + ((scroll_region_size_y - slider_bar->size.y) * state.
                scroll_bar_percent_offset);
        }
    }


    madness_ui->cursor_pos = state.window_region_pos;
    madness_ui_advance_cursor(state.window_region_size);


    hash_table_string_set(madness_ui->window_state_hash, state.window_name, &state);
}


void madness_ui_set_window_pos(u32 x, u32 y)
{
    vec2s push_data = {.x = x, .y = y};
    stack_push(madness_ui->window_pos_stack, &push_data);
}

void madness_ui_set_window_size(u32 width, u32 height)
{
    vec2s push_data = {.x = width, .y = height};
    stack_push(madness_ui->window_size_stack, &push_data);
}

vec2s madness_ui_get_window_pos(void)
{
    if (stack_is_empty(madness_ui->window_pos_stack))
    {
        return (vec2s){.x = MIN_UI_NODE_SCREEN_SIZE, .y = MIN_UI_NODE_SCREEN_SIZE};
    }
    vec2s out = stack_top(madness_ui->window_pos_stack, vec2s);
    stack_pop(madness_ui->window_pos_stack);
    return out;
}

vec2s madness_ui_get_window_size(void)
{
    if (stack_is_empty(madness_ui->window_size_stack))
    {
        return (vec2s){.x = MIN_UI_NODE_SCREEN_SIZE, .y = MIN_UI_NODE_SCREEN_SIZE};
    }
    vec2s out = stack_top(madness_ui->window_size_stack, vec2s);
    stack_pop(madness_ui->window_size_stack);
    return out;
}


vec2s madness_ui_get_text_size(String text)
{
    f32 font_scalar = madness_ui->editor_font_size / madness_ui->default_font_size;

    vec2s out_text_size = glms_vec2_zero();

    Madness_Font font_data;
    texture_system_get_font(madness_ui->resource_system->texture_system, madness_ui->default_font_handle, &font_data);

    for (u64 i = 0; i < text.length; i++)
    {
        const char c = text.chars[i];

        if (c < 32 || c >= 128) continue; // skip unsupported characters


        Glyph* g = &font_data.glyphs[c - 32];

        f32 y_height = ((f32)g->height * font_scalar);

        out_text_size.y = max(y_height, out_text_size.y);

        //printf("xpos %f, ypos%f, w%f, h%f\n", xpos, ypos, w, h);

        out_text_size.x += (g->advance) * font_scalar; // move offset forward
    }

    return out_text_size;
}

float madness_ui_get_default_element_height(void)
{
    return madness_ui->editor_font_size + madness_ui->text_padding_y;
}


void madness_calculate_text_size(String text, vec2s screen_position, vec2s* out_text_size)
{
    f32 font_scalar = madness_ui->editor_font_size / madness_ui->default_font_size;

    vec2s start_position = screen_position;
    float max_height_y = 0;

    Madness_Font font_data;
    texture_system_get_font(madness_ui->resource_system->texture_system, madness_ui->default_font_handle, &font_data);

    for (u64 i = 0; i < text.length; i++)
    {
        const char c = text.chars[i];

        if (c < 32 || c >= 128) continue; // skip unsupported characters


        Glyph* g = &font_data.glyphs[c - 32];

        f32 y_height = ((f32)g->height * font_scalar);

        max_height_y = max(y_height, max_height_y);

        //printf("xpos %f, ypos%f, w%f, h%f\n", xpos, ypos, w, h);

        screen_position.x += (g->advance) * font_scalar; // move offset forward
    }

    if (out_text_size)
    {
        out_text_size->x = screen_position.x; // - start_position.x;
        out_text_size->y = max_height_y;
    }
}

bool madness_ui_drop_down(String label, bool* state)
{
    UI_Node* drop_down_header_node = madness_ui_get_new_node();
    drop_down_header_node->pos = madness_ui->cursor_pos;
    drop_down_header_node->size = (vec2s){
        madness_ui->current_window_screen_size.x * 0.8,
        madness_ui_get_default_element_height()
    };
    drop_down_header_node->string_id = label;
    drop_down_header_node->hash_id = string_hash_u64(label);
    drop_down_header_node->color = madness_ui->editor_style.color;

    String* modified_label;
    if (*state)
    {
        modified_label = string_concat(&STRING("(<>) "), &label, madness_ui->frame_arena);
        drop_down_header_node->color = madness_ui->editor_style.permanent_active;
    }
    else
    {
        modified_label = string_concat(&STRING("(><) "), &label, madness_ui->frame_arena);
    }

    madness_ui_string_internal(*modified_label, drop_down_header_node->pos, drop_down_header_node->size, UI_ALIGNMENT_LEFT,
                               UI_ALIGNMENT_CENTER);

    madness_ui_set_interaction_state(drop_down_header_node);

    if (is_hot(drop_down_header_node->hash_id))
    {
        drop_down_header_node->color = madness_ui->editor_style.hovered_color;
    }
    if (is_active(drop_down_header_node->hash_id))
    {
        drop_down_header_node->color = madness_ui->editor_style.pressed_color;
        if (madness_ui->mouse_released_unique)
        {
            *state = !*state;
        }
    }

    madness_ui_advance_cursor(drop_down_header_node->size);


    return *state;
}

UI_Node* madness_ui_string(String text)
{
    UI_Node* ui_node = madness_ui_string_internal(text, madness_ui->cursor_pos, (vec2s){0, 0}, UI_ALIGNMENT_LEFT,
                                                  UI_ALIGNMENT_LEFT);
    madness_ui_advance_cursor(ui_node->size);
    return ui_node;
}


UI_Node* madness_ui_string_internal(String text, vec2s parent_pos,
                                    vec2s parent_size, UI_Alignment alignment_x, UI_Alignment alignment_y)
{
    //generate the text size
    vec2s text_size = madness_ui_get_text_size(text);


    vec2s text_pos = parent_pos;

    switch (alignment_x)
    {
    case UI_ALIGNMENT_LEFT:
        //do nothing
        break;
    case UI_ALIGNMENT_CENTER:
        // parent_pos = madness_ui->cursor_pos;
        float horizontal_space_remaining = parent_size.x - text_size.x;
        text_pos.x += (horizontal_space_remaining / 2);
        break;
    case UI_ALIGNMENT_RIGHT:
        //TODO: if i want this i need to store the window start position, and then offset it from the right, form where the cursor is
        //do nothing
        break;
    }

    switch (alignment_y)
    {
    case UI_ALIGNMENT_LEFT:
        //do nothing
        break;
    case UI_ALIGNMENT_CENTER:
        // parent_pos = madness_ui->cursor_pos;
        float vertical_space_remaining = parent_size.y - text_size.y;
        text_pos.y += (vertical_space_remaining / 2);
        break;
    case UI_ALIGNMENT_RIGHT:
        //TODO: if i want this i need to store the window start position, and then offset it from the right, form where the cursor is
        //do nothing
        break;
    }


    // debug the location of the text
    UI_Node* debug_text_node = madness_ui_get_new_node();
    // text_node->pos = madness_ui->cursor_pos;
    debug_text_node->pos = text_pos;
    debug_text_node->size = text_size;
    debug_text_node->color = COLOR_BLACK; // TODO: transparency
    debug_text_node->text = text;
    debug_text_node->string_id = text;
    debug_text_node->hash_id = string_hash_u64(text);

    //generate the actual text now that we have the proper position
    vec2s text_current_pos = text_pos;
    f32 font_scalar = madness_ui->editor_font_size / madness_ui->default_font_size;
    Madness_Font font_data;
    texture_system_get_font(madness_ui->resource_system->texture_system, madness_ui->default_font_handle,
                            &font_data);

    for (u64 i = 0; i < text.length; i++)
    {
        const char c = text.chars[i];

        if (c < 32 || c >= 128) continue; // skip unsupported characters

        Glyph* g = &font_data.glyphs[c - 32];

        f32 x_position = text_current_pos.x + ((float)g->xoff * font_scalar);
        f32 y_position = text_current_pos.y + ((float)g->yoff * font_scalar);

        f32 x_width = ((f32)g->width * font_scalar);
        f32 y_height = ((f32)g->height * font_scalar);

        UI_Node* text_node = madness_ui_get_new_node();
        text_node->pos = (vec2s){x_position, y_position};
        text_node->size = (vec2s){x_width, y_height};
        text_node->uv_offset = (vec2s){g->u0, g->v0};
        text_node->uv_size = (vec2s){g->u1 - g->u0, g->v1 - g->v0};
        text_node->color = COLOR_WHITE;
        text_node->texture_handle = madness_ui->default_font_handle;
        text_node->flags |= UI_FLAG_TEXT;
        text_node->character = c;

        text_current_pos.x += (g->advance) * font_scalar; // move offset forward
    }


    return debug_text_node;
}

UI_Node* madness_ui_c_string(const char* text)
{
    UI_Node* ui_node = madness_ui_string_internal(STRING_STRLEN(text), madness_ui->cursor_pos, (vec2s){0, 0}, UI_ALIGNMENT_LEFT,
                                                  UI_ALIGNMENT_LEFT);
    madness_ui_advance_cursor(ui_node->size);
    return ui_node;
}


void madness_ui_set_interaction_state(UI_Node* new_node)
{
    if (region_hit(new_node->pos, new_node->size))
    {
        set_hot(new_node->hash_id);

        //check if we have the mouse pressed and nothing else is selected
        //TODO: so there is a bug with can_be_active, in that the first ui called on the screen will take active focus,
        //TODO: this is despite there bieng another ui in front of it
        //TODO: for now imma just leave it be and dont draw things on top of others
        if (can_be_active())
        {
            set_active(new_node->hash_id);
        }
    }
}

bool madness_ui_button(const String label)
{
    vec2s text_size = madness_ui_get_text_size(label);
    vec2s button_size = (vec2s){
        text_size.x + madness_ui->text_padding_x,
        madness_ui_get_default_element_height(),
    };

    UI_Node* button_node = madness_ui_get_new_node();
    button_node->string_id = label;
    button_node->hash_id = string_hash_u64(label);
    button_node->pos = madness_ui->cursor_pos;
    button_node->size = button_size;
    button_node->flags = UI_FLAG_CLICKABLE;
    button_node->color = madness_ui->editor_style.color;

    madness_ui_string_internal(label, button_node->pos, button_node->size, UI_ALIGNMENT_CENTER, UI_ALIGNMENT_CENTER);

    madness_ui_advance_cursor(button_size);

    madness_ui_set_interaction_state(button_node);

    //active state
    if (is_active(button_node->hash_id))
    {
        button_node->color = madness_ui->editor_style.pressed_color;
    }
    //hot state
    else if (is_hot(button_node->hash_id))
    {
        button_node->color = madness_ui->editor_style.hovered_color;
    }

    return madness_ui_use_ui_element(button_node->hash_id, button_node->pos, button_node->size);
}

bool madness_ui_check_box(String label, bool* check_box_state)
{
    //TODO: make into an image or at least have the button colors change somehow

    UI_Node* checkbox_node = madness_ui_get_new_node();
    checkbox_node->string_id = label;
    checkbox_node->hash_id = generate_hash_key_64bit((u8*)label.chars, label.length);
    checkbox_node->pos = madness_ui->cursor_pos;
    checkbox_node->size = (vec2s){
        madness_ui_get_default_element_height(), madness_ui_get_default_element_height()
    };

    checkbox_node->flags |= UI_FLAG_OUTLINE;

    checkbox_node->outline_color = COLOR_BLACK;
    checkbox_node->outline_thickness = 0.3;

    checkbox_node->color = COLOR_WHITE;
    if (*check_box_state)
    {
        checkbox_node->color = COLOR_RED;
    }

    madness_ui_advance_cursor(checkbox_node->size);

    madness_ui_same_line();
    UI_Node* text_node = madness_ui_string_internal(label, madness_ui->cursor_pos, checkbox_node->size, UI_ALIGNMENT_LEFT,
                                                    UI_ALIGNMENT_CENTER);

    madness_ui_advance_cursor((vec2s){text_node->size.x, checkbox_node->size.y});


    madness_ui_set_interaction_state(checkbox_node);

    if (is_active(checkbox_node->hash_id))
    {
        checkbox_node->color = madness_ui->editor_style.pressed_color;
    }
    else if (is_hot(checkbox_node->hash_id))
    {
        checkbox_node->color = madness_ui->editor_style.hovered_color;
    }

    if (madness_ui_use_ui_element(checkbox_node->hash_id, checkbox_node->pos, checkbox_node->size))
    {
        //set the bool to its opposite
        *check_box_state = !(*check_box_state);
    }


    return *check_box_state;
}

void madness_image(String id, Texture_System* texture_system, const char* icon_path)
{
    //TODO:
    FATAL("UNIMPLEMENTED")
}

void madness_image_handle(Texture_Handle handle)
{
    UI_Node* checkbox_node = madness_ui_get_new_node();
    checkbox_node->string_id = STRING("image");
    checkbox_node->hash_id = string_hash_u64(checkbox_node->string_id);
    checkbox_node->pos = madness_ui->cursor_pos;
    checkbox_node->size = (vec2s){
        64, 64
    };
    checkbox_node->flags |= UI_FLAG_IMAGE;
    checkbox_node->texture_handle = handle;
    checkbox_node->color = COLOR_WHITE;
    checkbox_node->uv_offset = (vec2s){0,0};
    checkbox_node->uv_size = (vec2s){1.0,1.0};

    madness_ui_advance_cursor((vec2s){checkbox_node->size.x, checkbox_node->size.y});

}

float map_range(float v, float a, float b, float x, float y)
{
    return x + (v - a) * (y - x) / (b - a);
}

void madness_ui_slider_scroll(String id, float* slider_val, float min, float max)
{
    //draw the rect, then based on the cur val, draw it proportionally to where it should be

    const float s = max - min;
    float t = (*slider_val - min) / s;

    // proper screen pos and size


    //grab a node
    UI_Node* quad_node = madness_ui_get_new_node();
    quad_node->string_id = id;
    quad_node->hash_id = generate_hash_key_64bit((u8*)id.chars, id.length);
    quad_node->pos = madness_ui->cursor_pos;
    quad_node->size = (vec2s){
        madness_ui->current_window_screen_size.x * 0.8, madness_ui_get_default_element_height()
    };
    quad_node->color = madness_ui->editor_style.color;


    //grab another
    UI_Node* slider_node = madness_ui_get_new_node();
    slider_node->string_id = id;
    slider_node->hash_id = generate_hash_key_64bit((u8*)id.chars, id.length);
    // slider_node->pos = t;
    slider_node->size = (vec2s){quad_node->size.x * 0.2, quad_node->size.y};
    slider_node->pos = (vec2s){quad_node->pos.x + ((quad_node->size.x - slider_node->size.x) * t), quad_node->pos.y};
    slider_node->color = madness_ui->editor_style.custom_widget_color;

    madness_ui_set_interaction_state(slider_node);

    //check if we are hovering over the slider
    //TODO: have the size be the entire strip where the slider is at
    if (region_hit(quad_node->pos, quad_node->size))
    {
        slider_node->color = madness_ui->editor_style.hovered_color;

        if (input_is_mouse_wheel_up(madness_ui->input_system_reference))
        {
            *slider_val += 0.1;
        }
        if (input_is_mouse_wheel_down(madness_ui->input_system_reference))
        {
            *slider_val -= 0.1;
        }
    }
    if (is_active(slider_node->hash_id))
    {
        slider_node->color = madness_ui->editor_style.pressed_color;
        if (madness_ui->mouse_down)
        {
            float track_width = quad_node->size.x - slider_node->size.x;
            float relative_x = madness_ui->mouse_pos_x - quad_node->pos.x - (slider_node->size.x * 0.5f);
            float t = clamp_float(relative_x / track_width, 0.0f, 1.0f);

            *slider_val = min + t * (max - min);
            slider_node->pos.x = quad_node->pos.x + t * track_width;
        }
    }

    *slider_val = clamp_float(*slider_val, min, max);


    char* float_char = madness_ui_float_to_char(*slider_val);
    String float_string = STRING_STRLEN(float_char);

    madness_ui_string_internal(float_string, quad_node->pos, quad_node->size, UI_ALIGNMENT_CENTER, UI_ALIGNMENT_CENTER);


    //update ui state for the next element
    madness_ui_advance_cursor(quad_node->size);
}

void madness_ui_slider_arrow(String id, float* slider_val, float min, float max)
{
    //draw the rect, then based on the cur val, draw it proportionally to where it should be

    const float s = max - min;
    float t = (*slider_val - min) / s;

    // proper screen pos and size

    //quad
    UI_Node* quad_node = madness_ui_get_new_node();
    quad_node->string_id = id;
    quad_node->hash_id = generate_hash_key_64bit((u8*)id.chars, id.length);
    quad_node->pos = madness_ui->cursor_pos;
    quad_node->size = (vec2s){
        madness_ui->current_window_screen_size.x * 0.7, madness_ui_get_default_element_height()
    };
    quad_node->color = madness_ui->editor_style.color;


    char* float_char = madness_ui_float_to_char(*slider_val);
    String float_string = STRING_STRLEN(float_char);

    madness_ui_string_internal(float_string, quad_node->pos, quad_node->size, UI_ALIGNMENT_CENTER, UI_ALIGNMENT_CENTER);


    //update ui state for the next element
    madness_ui_advance_cursor(quad_node->size);

    madness_ui_same_line();
    UI_Node* left_arrow = madness_ui_get_new_node();
    left_arrow->hash_id = string_hash_u64(id);
    left_arrow->pos = madness_ui->cursor_pos;
    left_arrow->size = (vec2s){
        madness_ui_get_default_element_height(), madness_ui_get_default_element_height()
    };
    left_arrow->color = madness_ui->editor_style.color;
    madness_ui_advance_cursor(left_arrow->size);
    madness_ui_same_line();
    UI_Node* right_arrow = madness_ui_get_new_node();
    right_arrow->hash_id = string_hash_u64(id) / 2;
    right_arrow->pos = madness_ui->cursor_pos;
    right_arrow->size = (vec2s){
        madness_ui_get_default_element_height(), madness_ui_get_default_element_height()
    };
    right_arrow->color = madness_ui->editor_style.color;

    madness_ui_set_interaction_state(left_arrow);
    madness_ui_set_interaction_state(right_arrow);

    if (is_hot(left_arrow->hash_id))
    {
        left_arrow->color = madness_ui->editor_style.hovered_color;
    }
    if (is_hot(right_arrow->hash_id))
    {
        right_arrow->color = madness_ui->editor_style.hovered_color;
    }

    if (is_active(left_arrow->hash_id))
    {
        left_arrow->color = madness_ui->editor_style.pressed_color;
        if (madness_ui->mouse_released_unique)
        {
            *slider_val -= ((max - min) / 10.f);
        }
    }
    if (is_active(right_arrow->hash_id))
    {
        right_arrow->color = madness_ui->editor_style.pressed_color;
        if (madness_ui->mouse_released_unique)
        {
            *slider_val += ((max - min) / 10.f);
        }
    }

    *slider_val = clamp_float(*slider_val, min, max);


    //update ui state for the next element
    madness_ui_advance_cursor(quad_node->size);
}

void madness_ui_slider_arrow_u32(String id, u32* slider_val, const u32 min, const u32 max)
{
    //draw the rect, then based on the cur val, draw it proportionally to where it should be
    const u32 s = max - min;

    // proper screen pos and size

    //quad
    UI_Node* quad_node = madness_ui_get_new_node();
    quad_node->string_id = id;
    quad_node->hash_id = generate_hash_key_64bit((u8*)id.chars, id.length);
    quad_node->pos = madness_ui->cursor_pos;
    quad_node->size = (vec2s){
        madness_ui->current_window_screen_size.x * 0.7, madness_ui_get_default_element_height()
    };
    quad_node->color = madness_ui->editor_style.color;


    char float_char[12]; // Large enough to hold the digits, sign, and null terminator

    // Safely write the integer into the character array
    snprintf(float_char, sizeof(float_char), "%u", *slider_val);
    String float_string = STRING_STRLEN(float_char);

    madness_ui_string_internal(float_string, quad_node->pos, quad_node->size, UI_ALIGNMENT_CENTER, UI_ALIGNMENT_CENTER);


    //update ui state for the next element
    madness_ui_advance_cursor(quad_node->size);

    madness_ui_same_line();
    UI_Node* left_arrow = madness_ui_get_new_node();
    left_arrow->hash_id = string_hash_u64(id);
    left_arrow->pos = madness_ui->cursor_pos;
    left_arrow->size = (vec2s){
        madness_ui_get_default_element_height(), madness_ui_get_default_element_height()
    };
    left_arrow->color = madness_ui->editor_style.color;
    madness_ui_advance_cursor(left_arrow->size);
    madness_ui_same_line();
    UI_Node* right_arrow = madness_ui_get_new_node();
    right_arrow->hash_id = string_hash_u64(id) / 2;
    right_arrow->pos = madness_ui->cursor_pos;
    right_arrow->size = (vec2s){
        madness_ui_get_default_element_height(), madness_ui_get_default_element_height()
    };
    right_arrow->color = madness_ui->editor_style.color;

    madness_ui_set_interaction_state(left_arrow);
    madness_ui_set_interaction_state(right_arrow);

    if (is_hot(left_arrow->hash_id))
    {
        left_arrow->color = madness_ui->editor_style.hovered_color;
    }
    if (is_hot(right_arrow->hash_id))
    {
        right_arrow->color = madness_ui->editor_style.hovered_color;
    }

    if (is_active(left_arrow->hash_id))
    {
        left_arrow->color = madness_ui->editor_style.pressed_color;
        if (madness_ui->mouse_released_unique)
        {
            *slider_val -= 1;
        }
    }
    if (is_active(right_arrow->hash_id))
    {
        right_arrow->color = madness_ui->editor_style.pressed_color;
        if (madness_ui->mouse_released_unique)
        {
            *slider_val += 1;
        }
    }

    *slider_val = clamp_int(*slider_val, min, max);


    //update ui state for the next element
    madness_ui_advance_cursor(quad_node->size);
}

void madness_ui_slider_arrow_u16(String id, u16* slider_val, u16 min, u16 max)
{
    //draw the rect, then based on the cur val, draw it proportionally to where it should be
    const u32 s = max - min;

    // proper screen pos and size

    //quad
    UI_Node* quad_node = madness_ui_get_new_node();
    quad_node->string_id = id;
    quad_node->hash_id = generate_hash_key_64bit((u8*)id.chars, id.length);
    quad_node->pos = madness_ui->cursor_pos;
    quad_node->size = (vec2s){
        madness_ui->current_window_screen_size.x * 0.7, madness_ui_get_default_element_height()
    };
    quad_node->color = madness_ui->editor_style.color;


    char float_char[12]; // Large enough to hold the digits, sign, and null terminator

    // Safely write the integer into the character array
    snprintf(float_char, sizeof(float_char), "%u", *slider_val);
    String float_string = STRING_STRLEN(float_char);

    madness_ui_string_internal(float_string, quad_node->pos, quad_node->size, UI_ALIGNMENT_CENTER, UI_ALIGNMENT_CENTER);


    //update ui state for the next element
    madness_ui_advance_cursor(quad_node->size);

    madness_ui_same_line();
    UI_Node* left_arrow = madness_ui_get_new_node();
    left_arrow->hash_id = string_hash_u64(id);
    left_arrow->pos = madness_ui->cursor_pos;
    left_arrow->size = (vec2s){
        madness_ui_get_default_element_height(), madness_ui_get_default_element_height()
    };
    left_arrow->color = madness_ui->editor_style.color;
    madness_ui_advance_cursor(left_arrow->size);
    madness_ui_same_line();
    UI_Node* right_arrow = madness_ui_get_new_node();
    right_arrow->hash_id = string_hash_u64(id) / 2;
    right_arrow->pos = madness_ui->cursor_pos;
    right_arrow->size = (vec2s){
        madness_ui_get_default_element_height(), madness_ui_get_default_element_height()
    };
    right_arrow->color = madness_ui->editor_style.color;

    madness_ui_set_interaction_state(left_arrow);
    madness_ui_set_interaction_state(right_arrow);

    if (is_hot(left_arrow->hash_id))
    {
        left_arrow->color = madness_ui->editor_style.hovered_color;
    }
    if (is_hot(right_arrow->hash_id))
    {
        right_arrow->color = madness_ui->editor_style.hovered_color;
    }

    if (is_active(left_arrow->hash_id))
    {
        left_arrow->color = madness_ui->editor_style.pressed_color;
        if (madness_ui->mouse_released_unique)
        {
            *slider_val -= 1;
        }
    }
    if (is_active(right_arrow->hash_id))
    {
        right_arrow->color = madness_ui->editor_style.pressed_color;
        if (madness_ui->mouse_released_unique)
        {
            *slider_val += 1;
        }
    }

    *slider_val = clamp_int(*slider_val, min, max);


    //update ui state for the next element
    madness_ui_advance_cursor(quad_node->size);
}

bool madness_ui_u8(String text, u8* i, u32 increment_value)
{
    madness_ui_string(text);
    madness_ui_same_line();

    char float_char[12]; // Large enough to hold the digits, sign, and null terminator

    // Safely write the integer into the character array
    snprintf(float_char, sizeof(float_char), "%d", *i);
    String float_string = STRING_STRLEN(float_char);

    vec2s text_size = madness_ui_get_text_size(float_string);


    UI_Node* node = madness_ui_get_new_node();
    node->string_id = text;
    node->hash_id = string_hash_u64(text);
    node->pos = madness_ui->cursor_pos;
    node->size = (vec2s){
        text_size.x + madness_ui->text_padding_x,
        madness_ui_get_default_element_height(),
    };
    node->color = madness_ui->editor_style.color;


    // madness_ui_text_new(madness_ui, float_string);
    madness_ui_string_internal(float_string, node->pos, node->size, UI_ALIGNMENT_CENTER, UI_ALIGNMENT_CENTER);

    madness_ui_advance_cursor(node->size);


    bool has_changed = false;

    madness_ui_set_interaction_state(node);

    if (is_active(node->hash_id))
    {
        node->color = madness_ui->editor_style.pressed_color;

        if (madness_ui->mouse_down)
        {
            s16 mouse_change_x;
            s16 mouse_change_y;

            input_get_mouse_change(madness_ui->input_system_reference, &mouse_change_x, &mouse_change_y);

            if (mouse_change_x > 0)
            {
                *i += increment_value;
                // *f += increment_override;
                has_changed = true;
            }
            if (mouse_change_x < 0)
            {
                *i -= increment_value;
                // *f -= increment_override;
                has_changed = true;
            }
        }
    }
    else if (is_hot(node->hash_id))
    {
        node->color = madness_ui->editor_style.hovered_color;
        if (input_is_mouse_wheel_up(madness_ui->input_system_reference))
        {
            *i += increment_value;
            has_changed = true;
        }
        if (input_is_mouse_wheel_down(madness_ui->input_system_reference))
        {
            *i -= increment_value;
            has_changed = true;
        }

        set_hot(node->hash_id);

        if (can_be_active())
        {
            set_active(node->hash_id);
        }
    }

    return has_changed;
}

bool madness_ui_u16(String text, u16* i, u32 increment_value)
{
    madness_ui_string(text);
    madness_ui_same_line();

    char float_char[12]; // Large enough to hold the digits, sign, and null terminator

    // Safely write the integer into the character array
    snprintf(float_char, sizeof(float_char), "%d", *i);
    String float_string = STRING_STRLEN(float_char);

    vec2s text_size = madness_ui_get_text_size(float_string);


    UI_Node* node = madness_ui_get_new_node();
    node->string_id = text;
    node->hash_id = string_hash_u64(text);
    node->pos = madness_ui->cursor_pos;
    node->size = (vec2s){
        text_size.x + madness_ui->text_padding_x,
        madness_ui_get_default_element_height(),
    };
    node->color = madness_ui->editor_style.color;


    // madness_ui_text_new(madness_ui, float_string);
    madness_ui_string_internal(float_string, node->pos, node->size, UI_ALIGNMENT_CENTER, UI_ALIGNMENT_CENTER);

    madness_ui_advance_cursor(node->size);


    bool has_changed = false;

    madness_ui_set_interaction_state(node);

    if (is_active(node->hash_id))
    {
        node->color = madness_ui->editor_style.pressed_color;

        if (madness_ui->mouse_down)
        {
            s16 mouse_change_x;
            s16 mouse_change_y;

            input_get_mouse_change(madness_ui->input_system_reference, &mouse_change_x, &mouse_change_y);

            if (mouse_change_x > 0)
            {
                *i += increment_value;
                // *f += increment_override;
                has_changed = true;
            }
            if (mouse_change_x < 0)
            {
                *i -= increment_value;
                // *f -= increment_override;
                has_changed = true;
            }
        }
    }
    else if (is_hot(node->hash_id))
    {
        node->color = madness_ui->editor_style.hovered_color;
        if (input_is_mouse_wheel_up(madness_ui->input_system_reference))
        {
            *i += increment_value;
            has_changed = true;
        }
        if (input_is_mouse_wheel_down(madness_ui->input_system_reference))
        {
            *i -= increment_value;
            has_changed = true;
        }

        set_hot(node->hash_id);

        if (can_be_active())
        {
            set_active(node->hash_id);
        }
    }

    return has_changed;
}

bool madness_ui_u32(String text, u32* i, u32 increment_value)
{
    madness_ui_string(text);
    madness_ui_same_line();

    char float_char[12]; // Large enough to hold the digits, sign, and null terminator

    // Safely write the integer into the character array
    snprintf(float_char, sizeof(float_char), "%u", *i);
    String float_string = STRING_STRLEN(float_char);

    vec2s text_size = madness_ui_get_text_size(float_string);


    UI_Node* node = madness_ui_get_new_node();
    node->string_id = text;
    node->hash_id = string_hash_u64(text);
    node->pos = madness_ui->cursor_pos;
    node->size = (vec2s){
        text_size.x + madness_ui->text_padding_x,
        madness_ui_get_default_element_height(),
    };
    node->color = madness_ui->editor_style.color;


    // madness_ui_text_new(madness_ui, float_string);
    madness_ui_string_internal(float_string, node->pos, node->size, UI_ALIGNMENT_CENTER, UI_ALIGNMENT_CENTER);

    madness_ui_advance_cursor(node->size);


    bool has_changed = false;

    madness_ui_set_interaction_state(node);

    if (is_active(node->hash_id))
    {
        node->color = madness_ui->editor_style.pressed_color;

        if (madness_ui->mouse_down)
        {
            s16 mouse_change_x;
            s16 mouse_change_y;

            input_get_mouse_change(madness_ui->input_system_reference, &mouse_change_x, &mouse_change_y);

            if (mouse_change_x > 0)
            {
                *i += increment_value;
                // *f += increment_override;
                has_changed = true;
            }
            if (mouse_change_x < 0)
            {
                *i -= increment_value;
                // *f -= increment_override;
                has_changed = true;
            }
        }
    }
    else if (is_hot(node->hash_id))
    {
        node->color = madness_ui->editor_style.hovered_color;
        if (input_is_mouse_wheel_up(madness_ui->input_system_reference))
        {
            *i += increment_value;
            has_changed = true;
        }
        if (input_is_mouse_wheel_down(madness_ui->input_system_reference))
        {
            *i -= increment_value;
            has_changed = true;
        }

        set_hot(node->hash_id);

        if (can_be_active())
        {
            set_active(node->hash_id);
        }
    }

    return has_changed;
}

bool madness_ui_s32(String text, s32* i, u32 increment_value)
{
    madness_ui_string(text);
    madness_ui_same_line();

    char float_char[12]; // Large enough to hold the digits, sign, and null terminator

    // Safely write the integer into the character array
    snprintf(float_char, sizeof(float_char), "%d", *i);
    String float_string = STRING_STRLEN(float_char);

    vec2s text_size = madness_ui_get_text_size(float_string);


    UI_Node* node = madness_ui_get_new_node();
    node->string_id = text;
    node->hash_id = string_hash_u64(text);
    node->pos = madness_ui->cursor_pos;
    node->size = (vec2s){
        text_size.x + madness_ui->text_padding_x,
        madness_ui_get_default_element_height(),
    };
    node->color = madness_ui->editor_style.color;


    // madness_ui_text_new(madness_ui, float_string);
    madness_ui_string_internal(float_string, node->pos, node->size, UI_ALIGNMENT_CENTER, UI_ALIGNMENT_CENTER);

    madness_ui_advance_cursor(node->size);


    bool has_changed = false;

    madness_ui_set_interaction_state(node);

    if (is_active(node->hash_id))
    {
        node->color = madness_ui->editor_style.pressed_color;

        if (madness_ui->mouse_down)
        {
            s16 mouse_change_x;
            s16 mouse_change_y;

            input_get_mouse_change(madness_ui->input_system_reference, &mouse_change_x, &mouse_change_y);

            if (mouse_change_x > 0)
            {
                *i += increment_value;
                // *f += increment_override;
                has_changed = true;
            }
            if (mouse_change_x < 0)
            {
                *i -= increment_value;
                // *f -= increment_override;
                has_changed = true;
            }
        }
    }
    else if (is_hot(node->hash_id))
    {
        node->color = madness_ui->editor_style.hovered_color;
        if (input_is_mouse_wheel_up(madness_ui->input_system_reference))
        {
            *i += increment_value;
            has_changed = true;
        }
        if (input_is_mouse_wheel_down(madness_ui->input_system_reference))
        {
            *i -= increment_value;
            has_changed = true;
        }

        set_hot(node->hash_id);

        if (can_be_active())
        {
            set_active(node->hash_id);
        }
    }

    return has_changed;
}


void madness_ui_text_box(String id)
{
    UI_Node* label_node = madness_ui_string_internal(STRING("Text Box:"), madness_ui->cursor_pos, (vec2s){0, 0},
                                                     UI_ALIGNMENT_LEFT,
                                                     UI_ALIGNMENT_LEFT);

    madness_ui_advance_cursor(label_node->size);
    madness_ui_same_line();


    //grab a node
    UI_Node* text_box = madness_ui_get_new_node();
    text_box->string_id = id;
    text_box->hash_id = generate_hash_key_64bit((u8*)id.chars, id.length);
    text_box->pos = madness_ui->cursor_pos;

    float size_x = clamp_float(madness_ui->current_window_screen_size.x - label_node->size.x, 4, 10000);
    text_box->size = (vec2s){size_x * 0.9, madness_ui_get_default_element_height()};
    text_box->color = madness_ui->editor_style.textbox_color;


    if (region_hit(text_box->pos, text_box->size))
    {
        text_box->color = madness_ui->editor_style.layout_accent_color;

        //check for any keypressed and update the text
        if (input_key_released_unique(madness_ui->input_system_reference, KEY_BACKSPACE))
        {
            string_builder_decrement(madness_ui->string_builder);
        }

        if (madness_ui->released_key)
        {
            string_builder_append_c_string(madness_ui->string_builder, &madness_ui->released_key);
        }
    }


    String* display_string = string_builder_to_string(madness_ui->string_builder);

    madness_ui_string_internal(*display_string, madness_ui->cursor_pos, text_box->size,
                               UI_ALIGNMENT_LEFT,
                               UI_ALIGNMENT_CENTER);


    madness_ui_advance_cursor(text_box->size);
}

bool madness_ui_float_internal(Madness_UI* madness_ui, String text, float* f, float increment_value)
{
    char* float_char = madness_ui_float_to_char(*f);
    String float_string = STRING_STRLEN(float_char);
    vec2s text_size = madness_ui_get_text_size(float_string);


    UI_Node* node = madness_ui_get_new_node();
    node->string_id = text;
    node->hash_id = string_hash_u64(text);
    node->pos = madness_ui->cursor_pos;
    node->size = (vec2s){
        text_size.x + madness_ui->text_padding_x,
        madness_ui_get_default_element_height(),
    };
    node->color = madness_ui->editor_style.color;


    // madness_ui_text_new(madness_ui, float_string);
    madness_ui_string_internal(float_string, node->pos, node->size, UI_ALIGNMENT_CENTER, UI_ALIGNMENT_CENTER);

    madness_ui_advance_cursor(node->size);


    bool has_changed = false;

    madness_ui_set_interaction_state(node);

    if (is_active(node->hash_id))
    {
        node->color = madness_ui->editor_style.pressed_color;

        if (madness_ui->mouse_down)
        {
            s16 mouse_change_x;
            s16 mouse_change_y;

            input_get_mouse_change(madness_ui->input_system_reference, &mouse_change_x, &mouse_change_y);

            //we dont want to increment by the full value every single frame
            float increment_smoother_value = 8.f;
            float increment_override = 0.1f;
            if (mouse_change_x > 0)
            {
                *f += increment_value / increment_smoother_value;
                // *f += increment_override;
                has_changed = true;
            }
            if (mouse_change_x < 0)
            {
                *f -= increment_value / increment_smoother_value;
                // *f -= increment_override;
                has_changed = true;
            }
        }
    }
    else if (is_hot(node->hash_id))
    {
        node->color = madness_ui->editor_style.hovered_color;
        if (input_is_mouse_wheel_up(madness_ui->input_system_reference))
        {
            *f += increment_value;
            has_changed = true;
        }
        if (input_is_mouse_wheel_down(madness_ui->input_system_reference))
        {
            *f -= increment_value;
            has_changed = true;
        }

        set_hot(node->hash_id);

        if (can_be_active())
        {
            set_active(node->hash_id);
        }
    }

    return has_changed;
}


bool madness_ui_float(String text, float* f, float increment_value)
{
    madness_ui_string(text);
    madness_ui_same_line();

    return madness_ui_float_internal(madness_ui, text, f, increment_value);
}


bool madness_ui_float2(String text, float* x, float* y, float increment_value)
{
    madness_ui_string(text);
    madness_ui_same_line();

    String* x_id = string_concat(&text, &STRING("x"), madness_ui->frame_arena);
    String* y_id = string_concat(&text, &STRING("y"), madness_ui->frame_arena);


    bool has_moved1 = madness_ui_float_internal(madness_ui, *x_id, x, increment_value);
    madness_ui_same_line();
    bool has_moved2 = madness_ui_float_internal(madness_ui, *y_id, y, increment_value);


    if (has_moved1) { return true; }
    if (has_moved2) { return true; }


    return false;
}

bool madness_ui_float3(String text, float* x, float* y, float* z, float increment_value)
{
    madness_ui_string(text);

    String* x_id = string_concat(&text, &STRING("x"), madness_ui->frame_arena);
    String* y_id = string_concat(&text, &STRING("y"), madness_ui->frame_arena);


    bool has_moved1 = madness_ui_float_internal(madness_ui, *x_id, x, increment_value);
    madness_ui_same_line();
    bool has_moved2 = madness_ui_float_internal(madness_ui, *y_id, y, increment_value);
    madness_ui_same_line();
    bool has_moved3 = madness_ui_float_internal(madness_ui, *y_id, y, increment_value);


    if (has_moved1) { return true; }
    if (has_moved2) { return true; }
    if (has_moved3) { return true; }


    return false;
}

bool madness_ui_vec2(String label, vec2s* v, float increment_value)
{
    madness_ui_string(label);
    madness_ui_same_line();


    String* x_id = string_concat(&label, &STRING("x"), madness_ui->frame_arena);
    String* y_id = string_concat(&label, &STRING("y"), madness_ui->frame_arena);


    bool has_moved1 = madness_ui_float_internal(madness_ui, *x_id, &v->x, increment_value);
    madness_ui_same_line();
    bool has_moved2 = madness_ui_float_internal(madness_ui, *y_id, &v->y, increment_value);


    if (has_moved1) { return true; }
    if (has_moved2) { return true; }


    return false;
}

bool madness_ui_vec3(String label, vec3s* v, float increment_value)
{
    //draw text on top, then below the vec values
    madness_ui_string(label);
    madness_ui_same_line();


    String* x_id = string_concat(&label, &STRING("x"), madness_ui->frame_arena);
    String* y_id = string_concat(&label, &STRING("y"), madness_ui->frame_arena);
    String* z_id = string_concat(&label, &STRING("z"), madness_ui->frame_arena);


    bool has_moved1 = madness_ui_float_internal(madness_ui, *x_id, &v->x, increment_value);
    madness_ui_same_line();
    bool has_moved2 = madness_ui_float_internal(madness_ui, *y_id, &v->y, increment_value);
    madness_ui_same_line();
    bool has_moved3 = madness_ui_float_internal(madness_ui, *z_id, &v->z, increment_value);


    if (has_moved1) { return true; }
    if (has_moved2) { return true; }
    if (has_moved3) { return true; }


    return false;
}

bool madness_ui_vec4(String label, vec4s* v, float increment_value)
{
    //draw text on top, then below the vec values
    madness_ui_string(label);
    madness_ui_same_line();


    String* x_id = string_concat(&label, &STRING("x"), madness_ui->frame_arena);
    String* y_id = string_concat(&label, &STRING("y"), madness_ui->frame_arena);
    String* z_id = string_concat(&label, &STRING("z"), madness_ui->frame_arena);
    String* w_id = string_concat(&label, &STRING("w"), madness_ui->frame_arena);


    bool has_moved1 = madness_ui_float_internal(madness_ui, *x_id, &v->x, increment_value);
    madness_ui_same_line();
    bool has_moved2 = madness_ui_float_internal(madness_ui, *y_id, &v->y, increment_value);
    madness_ui_same_line();
    bool has_moved3 = madness_ui_float_internal(madness_ui, *z_id, &v->z, increment_value);
    madness_ui_same_line();
    bool has_moved4 = madness_ui_float_internal(madness_ui, *w_id, &v->w, increment_value);

    if (has_moved1) { return true; }
    if (has_moved2) { return true; }
    if (has_moved3) { return true; }
    if (has_moved4) { return true; }


    return false;
}


bool madness_ui_combo_box(String id, u32* selected_value, String* string_array,
                          u32 string_array_size)
{
    //TODO: should size to the largest element or currently named string
    String selected_string = string_array[*selected_value];
    vec2s text_size = madness_ui_get_text_size(selected_string);

    UI_Node* combo_box_node = madness_ui_get_new_node();
    combo_box_node->string_id = id;
    combo_box_node->hash_id = string_hash_u64(id);
    combo_box_node->pos = madness_ui->cursor_pos;
    combo_box_node->size = (vec2s){
        text_size.x + madness_ui->text_padding_x, madness_ui_get_default_element_height()
    };
    combo_box_node->color = madness_ui->editor_style.color;


    madness_ui_string_internal(selected_string, combo_box_node->pos, combo_box_node->size, UI_ALIGNMENT_CENTER,
                               UI_ALIGNMENT_CENTER);
    madness_ui_advance_cursor(combo_box_node->size);


    madness_ui_set_interaction_state(combo_box_node);
    //active state
    if (is_active(combo_box_node->hash_id))
    {
        combo_box_node->color = madness_ui->editor_style.pressed_color;
        madness_ui->active_combo_box = id;
    }
    //hot state
    else if (is_hot(combo_box_node->hash_id))
    {
        combo_box_node->color = madness_ui->editor_style.hovered_color;
    }

    //basically we want to defer this draw after everything else
    if (string_compare(&madness_ui->active_combo_box, &id))
    {
        String* pop_up_name = string_concat(&id, &STRING("combo_box"), madness_ui->frame_arena);
        madness_ui_pop_up_open(*pop_up_name, madness_ui->cursor_pos);

        /*
        //TODO: probably should be a scroll box here
        UI_Node* drop_down_node = madness_ui_get_pop_up_node(madness_ui);
        drop_down_node->string_id = id;
        drop_down_node->hash_id = string_hash_u64(id);
        drop_down_node->pos = madness_ui->cursor_pos;
        drop_down_node->size = (vec2){combo_box_node->size.x, 0};
        drop_down_node->color = madness_ui->editor_style.textbox_color;

        vec2 temp_cursor = madness_ui->cursor_pos;
        for (u32 i = 0; i < string_array_size; i++)
        {
            String draw = string_array[i];
            UI_Node* string_node = madness_ui_text_internal(madness_ui, draw, temp_cursor,
                                                            combo_box_node->size,
                                                            UI_ALIGNMENT_LEFT,
                                                            UI_ALIGNMENT_CENTER);
            string_node->string_id = draw;
            string_node->hash_id = string_hash_u64(draw);

            madness_ui_set_interaction_state(madness_ui, string_node);
            if (is_hot(madness_ui, string_node->hash_id))
            {
                string_node->color = madness_ui->editor_style.hovered_color;
            }

            if (region_hit(madness_ui, string_node->pos, string_node->size))
            {
                if (madness_ui->mouse_down)
                {
                    *selected_value = i;
                }
            }
            // madness_ui_advance_cursor(madness_ui, combo_box_node->size);
            drop_down_node->size.y += string_node->size.y + madness_ui->element_padding_y;
            temp_cursor.y += string_node->size.y + madness_ui->element_padding_y;
        }*/

        for (u32 i = 0; i < string_array_size; i++)
        {
            String draw = string_array[i];
            UI_Node* string_node = madness_ui_string_internal(draw, madness_ui->cursor_pos, combo_box_node->size,
                                                              UI_ALIGNMENT_LEFT,
                                                              UI_ALIGNMENT_CENTER);
            madness_ui_set_interaction_state(string_node);
            if (is_hot(string_node->hash_id))
            {
                string_node->color = madness_ui->editor_style.hovered_color;
            }

            if (region_hit(string_node->pos, string_node->size))
            {
                if (madness_ui->mouse_down)
                {
                    *selected_value = i;
                    madness_ui->nuke_pop_up = true;
                }
            }
            // madness_ui_advance_cursor(madness_ui, combo_box_node->size);
            madness_ui_advance_cursor(string_node->size);
        }

        madness_ui_pop_up_close();
    }


    // return madness_ui_use_ui_element(madness_ui, combo_box_node->hash_id, combo_box_node->pos, combo_box_node->size);
    // this should return when somehting has changed or on click, and let the user decide
    return false;
}

bool madness_ui_combo_box_char(String id, u32* selected_value, char** char_array,
                               u32 char_array_size)
{
    //TODO: should size to the largest element or currently named string
    char* temp = char_array[*selected_value];
    String* selected_string = string_create_allocator(temp, strlen(temp), madness_ui->frame_arena);
    vec2s text_size = madness_ui_get_text_size(*selected_string);

    UI_Node* combo_box_node = madness_ui_get_new_node();
    combo_box_node->string_id = id;
    combo_box_node->hash_id = string_hash_u64(id);
    combo_box_node->pos = madness_ui->cursor_pos;
    combo_box_node->size = (vec2s){
        text_size.x + madness_ui->text_padding_x, madness_ui_get_default_element_height()
    };
    combo_box_node->color = madness_ui->editor_style.color;


    madness_ui_string_internal(*selected_string, combo_box_node->pos, combo_box_node->size, UI_ALIGNMENT_LEFT,
                               UI_ALIGNMENT_CENTER);
    madness_ui_advance_cursor(combo_box_node->size);


    madness_ui_set_interaction_state(combo_box_node);
    //active state
    if (is_active(combo_box_node->hash_id))
    {
        combo_box_node->color = madness_ui->editor_style.pressed_color;
        madness_ui->active_combo_box = id;
    }
    //hot state
    else if (is_hot(combo_box_node->hash_id))
    {
        combo_box_node->color = madness_ui->editor_style.hovered_color;
    }

    //basically we want to defer this draw after everything else
    if (string_compare(&madness_ui->active_combo_box, &id))
    {
        String* pop_up_name = string_concat(&id, &STRING("combo_box"), madness_ui->frame_arena);
        madness_ui_pop_up_open(*pop_up_name, madness_ui->cursor_pos);

        for (u32 i = 0; i < char_array_size; i++)
        {
            char* inner_temp = char_array[i];
            String* draw = string_create_allocator(inner_temp, strlen(inner_temp), madness_ui->frame_arena);
            UI_Node* string_node = madness_ui_string_internal(*draw, madness_ui->cursor_pos, combo_box_node->size,
                                                              UI_ALIGNMENT_LEFT,
                                                              UI_ALIGNMENT_CENTER);

            madness_ui_set_interaction_state(string_node);
            if (is_hot(string_node->hash_id))
            {
                string_node->color = madness_ui->editor_style.hovered_color;
            }

            if (region_hit(string_node->pos, string_node->size))
            {
                if (madness_ui->mouse_down)
                {
                    *selected_value = i;
                    madness_ui->nuke_pop_up = true;
                }
            }
            // madness_ui_advance_cursor(madness_ui, combo_box_node->size);
            madness_ui_advance_cursor(string_node->size);
        }

        madness_ui_pop_up_close();
    }


    // return madness_ui_use_ui_element(madness_ui, combo_box_node->hash_id, combo_box_node->pos, combo_box_node->size);
    // this should return when somehting has changed or on click, and let the user decide
    return false;
}

void madness_ui_padding(const char* identifier)
{
    madness_ui_advance_cursor((vec2s){madness_ui->element_padding_x, madness_ui->element_padding_y});
}

bool madness_ui_color_picker(String label, vec3s* color_value)
{
    UI_Node* text_node = madness_ui_string_internal(label, madness_ui->cursor_pos, (vec2s){0, 0}, UI_ALIGNMENT_LEFT,
                                                    UI_ALIGNMENT_LEFT);
    madness_ui_advance_cursor(text_node->size);

    madness_ui_same_line();
    UI_Node* color_node = madness_ui_get_new_node();
    color_node->color = *color_value;
    color_node->pos = madness_ui->cursor_pos;
    color_node->size = (vec2s){
        madness_ui_get_default_element_height(), madness_ui_get_default_element_height()
    };
    madness_ui_advance_cursor(color_node->size);


    String* x_id = string_concat(&label, &STRING("x"), madness_ui->frame_arena);
    String* y_id = string_concat(&label, &STRING("y"), madness_ui->frame_arena);
    String* z_id = string_concat(&label, &STRING("z"), madness_ui->frame_arena);

    float increment_value = 0.05;
    madness_ui_same_line();
    madness_ui_float_internal(madness_ui, *x_id, &color_value->x, increment_value);
    madness_ui_same_line();
    madness_ui_float_internal(madness_ui, *y_id, &color_value->y, increment_value);
    madness_ui_same_line();
    madness_ui_float_internal(madness_ui, *z_id, &color_value->z, increment_value);

    color_value->x = clamp_float(color_value->x, 0.0, 1.0);
    color_value->y = clamp_float(color_value->y, 0.0, 1.0);
    color_value->z = clamp_float(color_value->z, 0.0, 1.0);

    madness_ui_advance_cursor(color_node->size);

    return false;
}


bool madness_ui_circle(String id, float* thickness)
{
    *thickness = clamp_float(*thickness, 0.0f, 1.0f);

    // 0-1 range
    const float button_ratio_to_layout_size = 0.8f;
    const float button_vertical_normalized_size = 0.03f;
    vec2s button_screen_size = {
        madness_ui->current_window_screen_size.x * button_ratio_to_layout_size,
        madness_ui->current_window_screen_size.x * button_ratio_to_layout_size
    };

    // proper screen pos and size
    vec2s button_screen_pos = madness_ui->cursor_pos;
    button_screen_pos.x += (madness_ui->current_window_screen_size.x - button_screen_size.x) / 2.f;

    //grab a node
    UI_Node* new_node = madness_ui_get_new_node();
    new_node->string_id = id;
    new_node->hash_id = generate_hash_key_64bit((u8*)id.chars, id.length);
    new_node->pos = button_screen_pos;
    new_node->size = button_screen_size;
    new_node->thickness = *thickness;
    new_node->flags |= UI_FLAG_CIRCLE;


    if (region_hit(new_node->pos, new_node->size))
    {
        set_hot(new_node->hash_id);

        //check if we have the mouse pressed and nothing else is selected
        //TODO: so there is a bug with can_be_active, in that the first ui called on the screen will take active focus,
        //TODO: this is despite there bieng another ui in front of it
        //TODO: for now imma just leave it be and dont draw things on top of others
        if (can_be_active())
        {
            set_active(new_node->hash_id);
        }
    }


    //active state
    if (is_active(new_node->hash_id))
    {
        new_node->color = madness_ui->editor_style.pressed_color;
    }
    //hot state
    else if (is_hot(new_node->hash_id))
    {
        new_node->color = madness_ui->editor_style.hovered_color;
    }
    // normal state
    else
    {
        new_node->color = madness_ui->editor_style.color;
    }


    //update ui state for the next element
    madness_ui_advance_cursor(button_screen_size);


    //check if we clicked the button
    return madness_ui_use_ui_element(new_node->hash_id, button_screen_pos, button_screen_size);
}

bool madness_ui_progress_bar(String label, float current, float max)
{
    vec2s text_size = madness_ui_get_text_size(label);
    vec2s bar_size = (vec2s){
        text_size.x + madness_ui->text_padding_x,
        madness_ui_get_default_element_height(),
    };

    UI_Node* background_bar = madness_ui_get_new_node();
    background_bar->string_id = label;
    background_bar->hash_id = string_hash_u64(label);
    background_bar->pos = madness_ui->cursor_pos;
    background_bar->size = bar_size;
    background_bar->flags = 0;
    background_bar->color = COLOR_GREY;

    float fill_bar_x = bar_size.x * clamp_float(current / max, 0.f, 1.f);

    UI_Node* fill_bar = madness_ui_get_new_node();
    fill_bar->string_id = label;
    fill_bar->hash_id = string_hash_u64(label);
    fill_bar->pos = madness_ui->cursor_pos;
    fill_bar->size = (vec2s){fill_bar_x, bar_size.y};
    fill_bar->flags = 0;
    fill_bar->color = COLOR_RED;

    char float_display[64];
    snprintf(float_display, 64, "%.1f / %.1f", current, max);
    String float_string = STRING_STRLEN(float_display);

    madness_ui_string_internal(float_string, background_bar->pos, background_bar->size, UI_ALIGNMENT_CENTER,
                               UI_ALIGNMENT_CENTER);

    madness_ui_advance_cursor(bar_size);


    /*
    madness_ui_set_interaction_state(madness_ui, fill_bar);

    //active state
    if (is_active(madness_ui, fill_bar->hash_id))
    {
        fill_bar->color = madness_ui->editor_style.pressed_color;
    }
    //hot state
    else if (is_hot(madness_ui, fill_bar->hash_id))
    {
        fill_bar->color = madness_ui->editor_style.hovered_color;
    }
*/
    // return madness_ui_use_ui_element(madness_ui, fill_bar->hash_id, fill_bar->pos, fill_bar->size);
    return false;
}

bool madness_ui_reflection_test(Reflection_Registry* reflection_registry,
                                const char* struct_name, const char* identifier)
{
    Reflection_Runtime_Data runtime_data = reflection_registry_get_or_create_runtime_data(
        reflection_registry, struct_name, identifier);
    Reflection_Runtime_Struct struct_info = reflection_registry_get_struct_from_runtime_data(
        reflection_registry, runtime_data);


    // madness_ui_text(madness_ui, *string_create_allocator(struct_info.name, strlen(struct_info.name), madness_ui->frame_arena));
    madness_ui_string(STRING_STRLEN(struct_info.name));

    for (u32 field_index = 0; field_index < struct_info.field_count; field_index++)
    {
        Reflection_Runtime_Struct_Field field_info = struct_info.fields[field_index];
        void* data = (u8*)runtime_data.data + field_info.offset;

        String* intermediate_name = string_concat(&STRING_STRLEN(struct_name), &STRING_STRLEN(identifier),
                                                  madness_ui->frame_arena);
        String* custom_name = string_concat(intermediate_name, &STRING_STRLEN(field_info.name),
                                            madness_ui->frame_arena);

        switch (field_info.type)
        {
        case REFLECTION_TYPE_INVALID:
            break;
        case REFLECTION_TYPE_U8:
            madness_ui_u8(*custom_name, data, 1);
            break;
        case REFLECTION_TYPE_U16:
            madness_ui_u16(*custom_name, data, 1);
            break;
        case REFLECTION_TYPE_U32:
            madness_ui_u32(*custom_name, data, 1);
            break;
        case REFLECTION_TYPE_U64:
            madness_ui_float(*custom_name, data, 1.0);
            break;
        case REFLECTION_TYPE_S8:
            madness_ui_float(*custom_name, data, 1.0);
            break;
        case REFLECTION_TYPE_S16:
            madness_ui_float(*custom_name, data, 1.0);
            break;
        case REFLECTION_TYPE_S32:
            madness_ui_s32(*custom_name, data, 1.0);
            break;
        case REFLECTION_TYPE_S64:
            madness_ui_float(*custom_name, data, 1.0);
            break;
        case REFLECTION_TYPE_F32:
            madness_ui_float(*custom_name, data, 1.0);
            break;
        case REFLECTION_TYPE_F64:
            madness_ui_float(*custom_name, data, 1.0);
            break;
        case REFLECTION_TYPE_SIZE_T:
            madness_ui_float(*custom_name, data, 1.0);
            break;
        case REFLECTION_TYPE_BOOL:
            madness_ui_check_box(*custom_name, data);
            break;
        case REFLECTION_TYPE_STRING:
            madness_ui_text_box(*custom_name);
            break;
        case REFLECTION_TYPE_CHAR:
            madness_ui_text_box(*custom_name);
            break;
        case REFLECTION_TYPE_ENUM:
            Reflection_Runtime_Enum runtime_enum = reflection_registry_get_enum(
                reflection_registry, field_info.type_name);
            madness_ui_combo_box_char(*custom_name, data, runtime_enum.enum_names,
                                      runtime_enum.count);
            break;
        case REFLECTION_TYPE_STRUCT:
            madness_ui_reflection_test(reflection_registry, field_info.type_name, identifier);
            break;
        case REFLECTION_TYPE_MAX:
            break;
        case REFLECTION_TYPE_CHAR_STRING:
            break;
        }
    }
    return false;
}

bool madness_ui_node_simple(String id, vec2s pos, String inputs[], u8 input_size,
                            String outputs[], u8 output_size, u32 node_id)
{
    u32 size_y_padding = 20;
    u32 size_y_increment = madness_ui->editor_font_size + size_y_padding;

    UI_Node* background = madness_ui_get_new_node();
    background->pos.x = pos.x;
    background->pos.y = pos.y;
    background->size.x = 100;
    background->color = madness_ui->editor_style.color;

    float x_pos = pos.x;
    float y_pos = pos.y;

    u32 size_input = 0; //madness_ui->editor_font_size;
    u32 size_output = 0; //madness_ui->editor_font_size;

    for (u8 i = 0; i < input_size; i++)
    {
        size_input += (i * size_y_increment);
        y_pos += (i * size_y_increment);
        vec2s tex_pos = {x_pos, y_pos};
        vec2s text_size;
        madness_calculate_text_size(inputs[i], tex_pos, &text_size);
        // madness_draw_text(madness_ui, inputs[i], tex_pos);


        if (region_hit(tex_pos, text_size))
        {
            if (madness_ui->mouse_down)
            {
                madness_ui->input_pressed = true;
                madness_ui->input_pressed_id = node_id;
                madness_ui->input_pos = tex_pos;
            }
        };
    }


    for (u8 i = 0; i < output_size; i++)
    {
        size_input += (i * size_y_increment);
        y_pos += (i * size_y_increment);
        vec2s tex_pos = {x_pos + 50, y_pos};
        vec2s text_size;
        madness_calculate_text_size(outputs[i], tex_pos, &text_size);
        // madness_draw_text(madness_ui, outputs[i], tex_pos);

        if (region_hit(tex_pos, text_size))
        {
            if (madness_ui->mouse_down)
            {
                madness_ui->output_pressed = true;
                madness_ui->output_pressed_id = node_id;
            }
        };

        // TODO: this should be for the link number and not the node id
        if (madness_ui->output_pressed_id == node_id)
        {
            if (madness_ui->input_pressed)
            {
                UI_Node* connection = madness_ui_get_new_node();
                connection->pos = tex_pos;
                connection->size = (vec2s){glms_vec2_sub(madness_ui->input_pos, tex_pos).x, 10};
                connection->color = madness_ui->editor_style.hovered_color;
            }
        };
    }

    background->size.y = max_f(size_input, size_output);
    return false;
}

bool madness_ui_node(String id, String inputs[], u8 input_size, String outputs[],
                     u8 output_size)
{
    u32 size_y_padding = 20;
    u32 size_y_increment = madness_ui->editor_font_size + size_y_padding;

    UI_Node* background = madness_ui_get_new_node();
    background->pos.x = 50;
    background->pos.y = 50;
    background->size.x = 100;
    background->color = madness_ui->editor_style.color;

    u32 size_input = 0; //madness_ui->editor_font_size;
    u32 size_output = 0; //madness_ui->editor_font_size;
    for (u8 i = 0; i < input_size; i++)
    {
        size_input += (i * size_y_increment);
        // madness_draw_text(madness_ui, inputs[i], (vec2){50, 50 + (i * size_y_increment)});
    }
    for (u8 i = 0; i < output_size; i++)
    {
        size_output += (i * size_y_increment);
        // madness_draw_text(madness_ui, outputs[i], (vec2){100, 50 + (i * size_y_increment)});
    }

    background->size.y = max_f(size_input, size_output);


    return false;
}

bool madness_ui_drag_test(vec2s* pos)
{
    UI_Node* background = madness_ui_get_new_node();
    background->pos = *pos;
    background->size.x = 150;
    background->size.y = 150;
    background->color = madness_ui->editor_style.color;


    if (region_hit(background->pos, background->size))
    {
        if (madness_ui->mouse_down)
        {
            // pos->x = madness_ui->mouse_pos_x - background->size.x/2;
            // pos->y = madness_ui->mouse_pos_y - background->size.y/2;
            pos->x += madness_ui->mouse_delta_x;
            pos->y += madness_ui->mouse_delta_y;
            return true;
        }
    }

    return false;
}

bool madness_ui_quadratic_bezier(vec2s* pos1, vec2s* pos2, vec2s* pos3)
{
    bool has_moved = false;

    UI_Node* node1 = madness_ui_get_new_node();
    node1->pos = *pos1;
    node1->size.x = 150;
    node1->size.y = 150;
    node1->color = COLOR_GREEN;
    node1->thickness = 1.f;
    node1->flags = UI_FLAG_CIRCLE;

    if (region_hit(node1->pos, node1->size))
    {
        if (madness_ui->mouse_down)
        {
            // pos->x = madness_ui->mouse_pos_x - background->size.x/2;
            // pos->y = madness_ui->mouse_pos_y - background->size.y/2;
            pos1->x += madness_ui->mouse_delta_x;
            pos1->y += madness_ui->mouse_delta_y;
            has_moved = true;
        }
    }


    UI_Node* node2 = madness_ui_get_new_node();
    node2->pos = *pos2;
    node2->size.x = 150;
    node2->size.y = 150;
    node2->color = COLOR_RED;
    node2->thickness = 1.f;
    node2->flags = UI_FLAG_CIRCLE;

    if (region_hit(node2->pos, node2->size))
    {
        if (madness_ui->mouse_down)
        {
            // pos->x = madness_ui->mouse_pos_x - background->size.x/2;
            // pos->y = madness_ui->mouse_pos_y - background->size.y/2;
            pos2->x += madness_ui->mouse_delta_x;
            pos2->y += madness_ui->mouse_delta_y;
            has_moved = true;
        }
    }


    UI_Node* node3 = madness_ui_get_new_node();
    node3->pos = *pos3;
    node3->size.x = 150;
    node3->size.y = 150;
    node3->color = COLOR_BLUE;
    node3->thickness = 1.f;
    node3->flags = UI_FLAG_CIRCLE;

    if (region_hit(node3->pos, node3->size))
    {
        if (madness_ui->mouse_down)
        {
            // pos->x = madness_ui->mouse_pos_x - background->size.x/2;
            // pos->y = madness_ui->mouse_pos_y - background->size.y/2;
            pos3->x += madness_ui->mouse_delta_x;
            pos3->y += madness_ui->mouse_delta_y;
            has_moved = true;
        }
    }


    u32 steps = 100;
    for (u32 i = 0; i <= steps; i++)
    {
        UI_Node* background = madness_ui_get_new_node();
        background->size.x = 10;
        background->size.y = 10;
        background->color = madness_ui->editor_style.color;

        float t = (float)i / (float)steps;
        background->pos = quadratic_bezier(*pos1, *pos2, *pos3, t);
    }

    return has_moved;
}

bool madness_ui_cubic_bezier(vec2s* pos1, vec2s* pos2, vec2s* pos3, vec2s* pos4)
{
    bool has_moved = false;

    UI_Node* node1 = madness_ui_get_new_node();
    node1->pos = *pos1;
    node1->size.x = 150;
    node1->size.y = 150;
    node1->color = COLOR_GREEN;
    node1->thickness = 1.f;
    node1->flags = UI_FLAG_CIRCLE;

    if (region_hit(node1->pos, node1->size))
    {
        if (madness_ui->mouse_down)
        {
            // pos->x = madness_ui->mouse_pos_x - background->size.x/2;
            // pos->y = madness_ui->mouse_pos_y - background->size.y/2;
            pos1->x += madness_ui->mouse_delta_x;
            pos1->y += madness_ui->mouse_delta_y;
            has_moved = true;
        }
    }


    UI_Node* node2 = madness_ui_get_new_node();
    node2->pos = *pos2;
    node2->size.x = 150;
    node2->size.y = 150;
    node2->color = COLOR_RED;
    node2->thickness = 1.f;
    node2->flags = UI_FLAG_CIRCLE;

    if (region_hit(node2->pos, node2->size))
    {
        if (madness_ui->mouse_down)
        {
            // pos->x = madness_ui->mouse_pos_x - background->size.x/2;
            // pos->y = madness_ui->mouse_pos_y - background->size.y/2;
            pos2->x += madness_ui->mouse_delta_x;
            pos2->y += madness_ui->mouse_delta_y;
            has_moved = true;
        }
    }


    UI_Node* node3 = madness_ui_get_new_node();
    node3->pos = *pos3;
    node3->size.x = 150;
    node3->size.y = 150;
    node3->color = COLOR_BLUE;
    node3->thickness = 1.f;
    node3->flags = UI_FLAG_CIRCLE;

    if (region_hit(node3->pos, node3->size))
    {
        if (madness_ui->mouse_down)
        {
            // pos->x = madness_ui->mouse_pos_x - background->size.x/2;
            // pos->y = madness_ui->mouse_pos_y - background->size.y/2;
            pos3->x += madness_ui->mouse_delta_x;
            pos3->y += madness_ui->mouse_delta_y;
            has_moved = true;
        }
    }

    UI_Node* node4 = madness_ui_get_new_node();
    node4->pos = *pos4;
    node4->size.x = 150;
    node4->size.y = 150;
    node4->color = COLOR_MAGENTA;
    node4->thickness = 1.f;
    node4->flags = UI_FLAG_CIRCLE;

    if (region_hit(node4->pos, node4->size))
    {
        if (madness_ui->mouse_down)
        {
            // pos->x = madness_ui->mouse_pos_x - background->size.x/2;
            // pos->y = madness_ui->mouse_pos_y - background->size.y/2;
            pos4->x += madness_ui->mouse_delta_x;
            pos4->y += madness_ui->mouse_delta_y;
            has_moved = true;
        }
    }


    u32 steps = 100;
    for (u32 i = 0; i <= steps; i++)
    {
        UI_Node* background = madness_ui_get_new_node();
        background->size.x = 10;
        background->size.y = 10;
        background->color = madness_ui->editor_style.color;

        float t = (float)i / (float)steps;
        background->pos = cubic_bezier(*pos1, *pos2, *pos3, *pos4, t);
        float a = 2;
    }

    return has_moved;
}


void madness_ui_test(void)
{
    /*
    do_text(madness_ui,
            STRING("!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"),
            (vec2){0, 70}, (vec2){10, 10},
            COLOR_WHITE,
            DEFAULT_FONT_SIZE);
    */

    madness_ui_window_begin(STRING("Madness UI Test Layout"));

    if (madness_ui_button(STRING("these are buttons")))
    {
        FATAL("YOU HAVE PRESSED THE BUTTON OH LORD WHY!");
    };
    if (madness_ui_button(STRING("le_button 2")))
    {
        FATAL("YOU HAVE PRESSED THE BUTTON OH LORD WHY!");
    }
    madness_ui_button(STRING("Le Button 3"));
    madness_ui_string(STRING("editor_font_size"));
    madness_ui_float(STRING("editor_font_slider"), &madness_ui->editor_font_size, 1.f);
    madness_ui_string(STRING("default_font_size"));
    madness_ui_float(STRING("default_font_slider"), &madness_ui->default_font_size, 1.f);

    {
        //if we want a layout change, specify so,
        static bool check_box_test;

        if (madness_ui_check_box(STRING("Check Box"), &check_box_test))
        {
            madness_ui->editor_style.text_color = COLOR_PURPLE_PALETTE_LIGHT;
            WARN("%d", check_box_test);
        }
        else
        {
            madness_ui->editor_style.text_color = COLOR_WHITE;
        }

        madness_ui_check_box(STRING("Other"), &check_box_test);
        madness_ui_check_box(STRING("Next"), &check_box_test);
    }

    madness_ui_string(STRING("GOD DAMN IT BOBBY"));
    if (madness_ui_button(STRING("AND SO IT GOES")))
    {
        FATAL(" BUTTONS AND DEATH");
    }


    static float slider_val;
    float slider_min = 0;
    float slider_max = 1;
    madness_ui_slider_scroll(STRING("slider"), &slider_val, slider_min, slider_max);

    madness_ui_slider_arrow(STRING("slider arrow"), &slider_val, slider_min, slider_max);

    madness_ui_text_box(STRING("textbox"));

    static vec3s vec3_test;
    float vec3_change_value = 10.5f;
    madness_ui_vec3(STRING("Position"), &vec3_test, vec3_change_value);

    static vec2s vec2_test;
    madness_ui_vec2(STRING("Sprite Position"), &vec2_test, vec3_change_value);

    static vec3s color_test;
    madness_ui_color_picker(STRING("Color Picker"), &color_test);


    madness_scroll_box_begin(STRING("scroll box"));
    {
        madness_ui_button(STRING("Scroll Around 1"));
        madness_ui_button(STRING("Scroll Around 2"));
        if (madness_ui_button(STRING("Scroll Around 3")))
        {
            FATAL("BUTTONS AND DEATH");
        }
        madness_ui_button(STRING("Scroll Around 4"));
        madness_ui_button(STRING("Scroll Around 5"));
        madness_ui_button(STRING("Scroll Around 6"));
    }
    madness_scroll_box_end();
}

void madness_ui_example(void)
{
    madness_ui_menu_bar_begin(STRING("Menu Bar"));
    {
        if (madness_ui_menu_item_begin(STRING("File")))
        {
            static bool state2 = false;
            if (madness_ui_drop_down(STRING("saodoa"), &state2))
            {
                if (madness_ui_button(STRING("File oh nobutton")))
                {
                    FATAL("A Fatal Poem");
                }
            }

            madness_scroll_box_begin(STRING("File Scroll"));
            {
                if (madness_ui_button(STRING("File button")))
                {
                    FATAL("BUTTONS AND DEATH");
                };
            }

            madness_scroll_box_end();
            madness_ui_button(STRING("File oh oh yes "));
            madness_ui_button(STRING("File oh oh yes "));
            madness_ui_button(STRING("File oh oh yes "));
            madness_ui_button(STRING("File oh oh yes "));
        }
        madness_ui_menu_item_end();
        if (madness_ui_menu_item_begin(STRING("Menu")))
        {
            madness_scroll_box_begin(STRING("File Scroll"));
            {
                static float a = 0;
                madness_ui_slider_scroll(STRING("sad"), &a, -1, 10);
                madness_ui_text_box(STRING("lama"));
            }
            madness_scroll_box_end();
        }
        madness_ui_menu_item_end();

        if (madness_ui_menu_item_begin(STRING("Quit")))
        {
            if (madness_ui_button(STRING("File button")))
            {
                FATAL("BUTTONS AND DEATH");
            }
            madness_ui_button(STRING("File button"));
            madness_ui_button(STRING("File button"));
            madness_ui_button(STRING("File button"));
            madness_ui_button(STRING("File button"));
            madness_ui_button(STRING("File button"));
            madness_ui_button(STRING("File button"));
            madness_ui_button(STRING("File button"));
        }
        madness_ui_menu_item_end();
    }
    madness_ui_menu_bar_end();

    madness_ui_set_window_pos(100, 100);
    madness_ui_set_window_size(250, 500);

    madness_ui_window_begin(STRING("Madness UI Example"));
    {
        if (madness_ui_button(STRING("Click Me")))
        {
            FATAL("I HAVE BEEN CLICKED");
        }
        madness_ui_string(STRING("Yous a bitch"));

        //
        static float a;
        madness_ui_float(STRING("floating"), &a, 1);

        static vec2s v2;
        madness_ui_vec2(STRING("vec2"), &v2, 1);

        static vec3s v3;
        madness_ui_vec3(STRING("vec3"), &v3, 1);

        static bool checkbox;
        madness_ui_check_box(STRING("the box is evil"), &checkbox);

        static u32 combo_box_selected = 0;
        String string_arr[] = {STRING("oh no"), STRING("whats up 2"), STRING("oaisd")};
        madness_ui_combo_box(STRING("combo box 2"), &combo_box_selected, string_arr, ARRAY_SIZE(string_arr));

        madness_ui_text_box(STRING("text box"));

        static float slider_val;
        static float slider_arrow;
        madness_ui_slider_scroll(STRING("slider"), &slider_val, 0, 1);
        madness_ui_slider_arrow(STRING("Arrow Slider"), &slider_arrow, 0, 1);

        madness_scroll_box_begin(STRING("scroll box"));
        {
            if (madness_ui_button(STRING("Click Tha")))
            {
                FATAL("I HAVE BEEN CLICKED");
            }
            madness_ui_button(STRING("Click Wha"));
            madness_ui_button(STRING("Click Nah"));
        }
        madness_scroll_box_end();


        static bool drop_down;
        if (madness_ui_drop_down(STRING("Drop Down"), &drop_down))
        {
            madness_ui_button(STRING("hahahah"));
            madness_ui_button(STRING("mooooooooo"));
        }
        madness_ui_button(STRING("After Drop Down"));

        madness_ui_progress_bar(STRING("Progress bar"), 50, 100);
    }
    madness_ui_window_end();
    madness_ui_config_menu();
}


void madness_ui_config_menu(void)
{
    madness_ui_set_window_pos(700, 100);
    madness_ui_set_window_size(350, 500);
    madness_ui_window_begin(STRING("UI Config"));
    {
        madness_ui_string(STRING("font size"));
        madness_ui_float(STRING("font size"), &madness_ui->editor_font_size, 1);

        madness_ui_float2(STRING("element Paddingx&y"), &madness_ui->element_padding_x,
                          &madness_ui->element_padding_y, 1);

        madness_ui_float2(STRING("Text Padding x&y"), &madness_ui->text_padding_x,
                          &madness_ui->text_padding_y, 1);

        madness_ui_color_picker(STRING("Default Color"), &madness_ui->editor_style.color);
        madness_ui_color_picker(STRING("Header Color"), &madness_ui->editor_style.header_color);
        madness_ui_color_picker(STRING("Pop Up Color"), &madness_ui->editor_style.pop_up_color);
        madness_ui_color_picker(STRING("Layout Color"), &madness_ui->editor_style.layout_color);

        madness_ui_float2(STRING("Window Size"), &madness_ui->screen_size.x,
                          &madness_ui->screen_size.y, 0);
    }
    madness_ui_window_end();
}
