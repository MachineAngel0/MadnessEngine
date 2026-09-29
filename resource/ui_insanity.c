#include "ui_insanity.h"
#include "math_lib.h"


bool insanity_ui_init(Memory_System* memory_system, Input_System* input_system,
                      Asset_System* asset_system)
{
    insanity_ui = memory_system_alloc(memory_system, sizeof(Insanity_UI), MEMORY_SUBSYSTEM_UI);
    MASSERT(insanity_ui);

    u64 ui_arena_mem_size = MB(16);
    u64 ui_frame_arena_mem_size = MB(16);


    insanity_ui->allocator = memory_system_allocator_create(memory_system, ui_arena_mem_size, MEMORY_SUBSYSTEM_UI);
    insanity_ui->frame_allocator = memory_system_allocator_create(memory_system, ui_frame_arena_mem_size,
                                                                  MEMORY_SUBSYSTEM_UI);

    insanity_ui->input_system = input_system;
    insanity_ui->asset_system = asset_system;


    insanity_ui->default_font_size = INSANITY_DEFAULT_FONT_SIZE;
    insanity_ui->editor_font_size = INSANITY_EDITOR_FONT_SIZE;
    insanity_ui->text_outline = INSANITY_TEXT_OUTLINE;


    //interaction and events
    insanity_ui->hot_last_frame = 0;
    insanity_ui->hot_this_frame = 0;
    insanity_ui->active = 0;


    insanity_ui->mouse_pos_x = -1.0f;
    insanity_ui->mouse_pos_y = -1.0f;

    insanity_ui->mouse_down = 0;
    insanity_ui->mouse_released_unique = 0;

    //TODO: replace with an in param
    insanity_ui->screen_size = (vec2s){400.0f, 400.0f};


    insanity_ui->default_font_handle = asset_load_font_path(asset_system, "arial_msdf");

    insanity_ui->editor_style = (Insanity_UI_Editor_Style){
        .layout_color = COLOR_PURPLE_PALETTE_DARK, .layout_accent_color = COLOR_PURPLE_PALETTE_PURPLE,
        .text_color = COLOR_PURPLE_PALETTE_LIGHT, .textbox_color = COLOR_PURPLE_PALETTE_DARK2,
        .custom_widget_color = COLOR_PURPLE_PALETTE_PURPLE_LIGHT,
        .color = COLOR_PURPLE_PALETTE_PURPLE_STRONG, .hovered_color = COLOR_PURPLE_PALETTE_PURPLE_LIGHT2,
        .pressed_color = COLOR_PURPLE_PALETTE_DARK2,
        .outline_color = COLOR_PURPLE_PALETTE_PURPLE_LIGHT,
        .permanent_active = COLOR_HOT_PINK,
        .header_color = (vec3s){0.425, 0.05, 0.456},
        .pop_up_color = (vec3s){0.110, 0.120, 0.162},
        .scrollbox_color = COLOR_HOT_PINK,
        .error_color = COLOR_GREEN,
    };


    insanity_ui->dummy_node = (IUI_Node){0};
    insanity_ui->dummy_node.name_id = "dummy_node";
    insanity_ui->dummy_node.color = COLOR_HOT_PINK;

    insanity_ui->internal_screen_resolution.x = INSANITY_UI_DEFAULT_RESOLUTION_WIDTH;
    insanity_ui->internal_screen_resolution.y = INSANITY_UI_DEFAULT_RESOLUTION_HEIGHT;

    INFO("INSANITY UI CREATED");
    return true;
}

bool insanity_ui_deinit(void)
{
    return true;
}

void insanity_ui_begin(s32 screen_size_x, s32 screen_size_y)
{
    MASSERT(insanity_ui);

    PROFILE_ZONE(insanity_ui_begin)


    //clear node/draw info
    allocator_clear(insanity_ui->frame_allocator);

    insanity_ui->ui_nodes = array_create(IUI_Node, INSANITY_UI_MAX_NODE_COUNT, insanity_ui->frame_allocator);
    insanity_ui->string_slice_array = allocator_alloc(insanity_ui->frame_allocator,
                                                      sizeof(IUI_String_Slice) * INSANITY_UI_MAX_STRING_SLICE);
    insanity_ui->string_slice_count = 0;
    insanity_ui->string_slice_count_max = INSANITY_UI_MAX_STRING_SLICE;


    //clear ui interaction state
    insanity_ui->hash = hash_32_continous_start();


    insanity_ui->screen_size.x = screen_size_x;
    insanity_ui->screen_size.y = screen_size_y;

    insanity_ui->screen_resolution_scale = glms_vec2_div(insanity_ui->screen_size,
                                                         insanity_ui->internal_screen_resolution);


    array_zero(insanity_ui->ui_nodes);
    array_clear(insanity_ui->ui_nodes);


    insanity_ui->interaction_node_count = 0;
    insanity_ui->navigation_node_count = 0;

    insanity_ui->hot_last_frame = insanity_ui->hot_this_frame;


    //query our input state
    insanity_ui->mouse_released_unique = input_is_mouse_button_released_unique(MOUSE_BUTTON_LEFT);
    insanity_ui->mouse_down = input_was_mouse_button_pressed(MOUSE_BUTTON_LEFT);
    insanity_ui->mouse_down_unique = input_is_mouse_button_pressed_unique(MOUSE_BUTTON_LEFT);

    input_get_mouse_pos(&insanity_ui->mouse_pos_x, &insanity_ui->mouse_pos_y);
    input_get_mouse_change(&insanity_ui->mouse_delta_x, &insanity_ui->mouse_delta_y);
    insanity_ui->first_released_key = input_get_first_released_key();


    insanity_ui->mouse_wheel_up = input_is_mouse_wheel_up();
    insanity_ui->mouse_wheel_down = input_is_mouse_wheel_down();
    input_get_mouse_wheel_value(&insanity_ui->mouse_wheel_delta);


    insanity_ui->key_shift = input_is_key_pressed(KEY_LSHIFT) || input_is_key_pressed(KEY_RSHIFT);
    insanity_ui->key_alt = input_is_key_pressed(KEY_LALT) || input_is_key_pressed(KEY_RALT);
    insanity_ui->key_ctrl = input_is_key_pressed(KEY_LCONTROL) || input_is_key_pressed(KEY_RCONTROL);
    insanity_ui->key_backspace = input_is_key_pressed(KEY_BACKSPACE);

    insanity_ui->scroll_array_count = 0;

    //TODO: move into its own function
    //input debug state
    /*DEBUG("INSANITY UI MOUSE POS: %d, %d", insanity_ui->mouse_pos_x, insanity_ui->mouse_pos_y);
    DEBUG("INSANITY UI MOUSE DELTA: %d, %d", insanity_ui->mouse_delta_x, insanity_ui->mouse_delta_y);
    DEBUG("INSANITY UI MOUSE Wheel: UP: %d, DOWN:%d: delta", insanity_ui->mouse_wheel_up,
          insanity_ui->mouse_wheel_down);


    if (insanity_ui->key_shift)
    {
        DEBUG("INSANITY UI SHIFT PRESSED")
    }
    if (insanity_ui->key_alt)
    {
        DEBUG("INSANITY UI ALT PRESSED")
    }
    if (insanity_ui->key_ctrl)
    {
        DEBUG("INSANITY UI CTRL PRESSED")
    }
    if (insanity_ui->key_backspace)
    {
        DEBUG("INSANITY UI BACKSPACE")
    }*/

    PROFILE_ZONE_END(insanity_ui_begin)
}


Insanity_UI_Render_Packet insanity_ui_end(void)
{
    MASSERT(insanity_ui);
    PROFILE_ZONE(insanity_ui_end)


    insanity_ui_resolve_interaction();

    u32 render_node_count = 0;
    for (u32 i = 0; i < insanity_ui->ui_nodes->num_items; i++)
    {
        IUI_Node* node = &array_get(insanity_ui->ui_nodes, i, IUI_Node);
        switch (node->type)
        {
        case Insanity_UI_Node_Type_Rect:
            render_node_count++;
            break;
        case Insanity_UI_Node_Type_Text:

            for (u32 slice_idx = 0; slice_idx < node->slice_count; slice_idx++)
            {
                render_node_count += node->slice_pointer[slice_idx].slices.length;
            }
            break;
        case Insanity_UI_Node_Type_Scissor_Start:
            render_node_count++;
            break;
        case Insanity_UI_Node_Type_Scissor_End:
            render_node_count++;
            break;
        }
    }


    insanity_ui->render_node_array = allocator_alloc(insanity_ui->frame_allocator,
                                                     render_node_count * sizeof(UI_Render_Node));
    insanity_ui->render_node_array_count = render_node_count;


    insanity_ui->draw_command_array = allocator_alloc(insanity_ui->frame_allocator,
                                                      render_node_count * sizeof(UI_Draw_Command));
    insanity_ui->draw_command_count = render_node_count;
    insanity_ui->current_draw_command_count = 0;

    u32 render_node_instance = 0;
    for (u32 i = 0; i < insanity_ui->ui_nodes->num_items; i++)
    {
        IUI_Node* node_data = &array_get(insanity_ui->ui_nodes, i, IUI_Node);

        switch (node_data->type)
        {
        case Insanity_UI_Node_Type_Rect:

            UI_Render_Node* render_node = &insanity_ui->render_node_array[render_node_instance];

            render_node->ui_flags = node_data->ui_flags;
            render_node->pos = glms_vec2_div(node_data->pos, insanity_ui->screen_size);
            // render_node->pos
            render_node->size = glms_vec2_div(node_data->size, insanity_ui->screen_size);
            render_node->rotation = deg_to_rad(node_data->rotation);

            render_node->thickness = node_data->thickness;

            render_node->rounded_radius = node_data->rounded_radius;

            render_node->color = node_data->color;

            render_node->texture_handle = node_data->texture_handle.handle;
            render_node->uv_offset = node_data->uv_offset;
            render_node->uv_size = node_data->uv_size;

            render_node->outline_color = node_data->outline_color;
            render_node->outline_thickness = node_data->outline_thickness;

            ui_add_draw_command(insanity_ui->draw_command_array, insanity_ui->current_draw_command_count,
                                UI_DRAW_TYPE_DRAW, glms_vec2_zero(), glms_vec2_zero());

            render_node_instance++;

            break;
        case Insanity_UI_Node_Type_Text:
            //expand string
            //generate the actual text now that we have the proper position
            // node_data->pos = glms_vec2_div(node_data->pos, insanity_ui->internal_screen_resolution);

            f32 font_scalar = insanity_ui->editor_font_size / insanity_ui->default_font_size;
            Madness_Font font_data;
            texture_system_get_font(insanity_ui->asset_system->texture_system, insanity_ui->default_font_handle,
                                    &font_data);

            for (u32 slice_idx = 0; slice_idx < node_data->slice_count; slice_idx++)
            {
                IUI_String_Slice* slice = &node_data->slice_pointer[slice_idx];

                // vec2s text_current_pos = node_data->pos;
                vec2s text_current_pos = glms_vec2_zero();
                switch (slice->alignment_x)
                {
                case UI_ALIGNMENT_LEFT_TOP:
                    text_current_pos.x = node_data->pos.x ;
                    break;
                case UI_ALIGNMENT_CENTER:
                    text_current_pos.x = node_data->pos.x + (node_data->size.x - slice->slice_width)/2;
                    break;
                case UI_ALIGNMENT_RIGHT_BOTTOM:
                    text_current_pos.x = node_data->pos.x + node_data->size.x - slice->slice_width -1 ;
                    break;
                }

                f32 remaining_height = (node_data->size.y - node_data->total_slice_height) / node_data->slice_count;
                switch (slice->alignment_y)
                {
                case UI_ALIGNMENT_LEFT_TOP:
                    text_current_pos.y = node_data->pos.y + slice->slice_height;
                    break;
                case UI_ALIGNMENT_CENTER:
                    text_current_pos.y = node_data->pos.y + (remaining_height/2) + slice->slice_height;
                    break;
                case UI_ALIGNMENT_RIGHT_BOTTOM:
                    text_current_pos.y = node_data->pos.y + remaining_height + slice->slice_height - 1;

                    break;
                }


                //we start from the string slices offset, until we reach the end of the length we sliced
                for (u64 text_idx = slice->slices.offset; text_idx < slice->slices.offset + slice->slices.length;
                     text_idx++)
                {
                    const char c = slice->slices.original_string->chars[text_idx];

                    if (c < 32 || c >= 128) continue; // skip unsupported characters

                    Glyph* g = &font_data.glyphs[c - 32];

                    f32 x_position = text_current_pos.x + ((float)g->xoff * font_scalar);
                    f32 y_position = text_current_pos.y + ((float)g->yoff * font_scalar);

                    f32 x_width = ((f32)g->width * font_scalar);
                    f32 y_height = ((f32)g->height * font_scalar);

                    UI_Render_Node* text_render_node = &insanity_ui->render_node_array[render_node_instance];
                    text_render_node->pos = (vec2s){
                        /*node_data->pos.x +*/ x_position, /*node_data->pos.y +*/ y_position
                    };
                    text_render_node->pos = glms_vec2_div(text_render_node->pos,
                                                          insanity_ui->screen_size);

                    text_render_node->size = (vec2s){x_width, y_height};
                    text_render_node->size = glms_vec2_div(text_render_node->size,
                                                           insanity_ui->screen_size);


                    text_render_node->rotation = deg_to_rad(node_data->rotation);
                    text_render_node->uv_offset = (vec2s){g->u0, g->v0};
                    text_render_node->uv_size = (vec2s){g->u1 - g->u0, g->v1 - g->v0};
                    text_render_node->color = node_data->color;
                    text_render_node->texture_handle = insanity_ui->default_font_handle.handle;
                    text_render_node->ui_flags = node_data->ui_flags;
                    text_render_node->ui_flags |= UI_FLAG_TEXT;

                    // render_node->outline_thickness = insanity_ui->text_outline;
                    // render_node->outline_color = insanity_ui->text_outline_color;
                    // render_node->thickness = node_data->thickness;

                    text_current_pos.x += (g->advance) * font_scalar; // move offset forward
                    ui_add_draw_command(insanity_ui->draw_command_array, insanity_ui->current_draw_command_count,
                                        UI_DRAW_TYPE_DRAW, glms_vec2_zero(), glms_vec2_zero());
                    render_node_instance++;
                }
            }

            break;
        case Insanity_UI_Node_Type_Scissor_Start:
            //no need to fill out the render nodes, they are zeroed anyway
            ui_add_draw_command(insanity_ui->draw_command_array, insanity_ui->current_draw_command_count,
                                UI_DRAW_TYPE_SCISSOR_START, glms_vec2_zero(), glms_vec2_zero());
            render_node_instance++;
            break;
        case Insanity_UI_Node_Type_Scissor_End:
            ui_add_draw_command(insanity_ui->draw_command_array, insanity_ui->current_draw_command_count,
                                UI_DRAW_TYPE_SCISSOR_START, glms_vec2_zero(), glms_vec2_zero());
            render_node_instance++;
            break;
        }
    }


    PROFILE_ZONE_END(insanity_ui_end)

    return (Insanity_UI_Render_Packet){
        .material_data = insanity_ui->render_node_array,
        .material_data_count = insanity_ui->render_node_array_count,
        .material_bytes = insanity_ui->render_node_array_count * sizeof(UI_Render_Node),
        .draw_command = insanity_ui->draw_command_array,
        .draw_command_count = insanity_ui->draw_command_count,
        // .draw_command_count = insanity_ui->current_draw_command_count,
    };
}

void insanity_ui_resolve_interaction(void)
{
    //TODO: if we are a hot node and we become pressed, even if not hovered over, it becomes hovered
    //idk if i want to resolve this per node, as any node should in theory be hovered

    // resolve hot
    u32 new_hot = 0;

    if (insanity_ui->active != 0 && !insanity_ui->mouse_released_unique)
    {
        // active pins hot — nothing can steal it mid-gesture, skip hit-testing entirely
        new_hot = insanity_ui->active;
    }
    else
    {
        s32 best_z = -1;
        for (u32 i = 0; i < insanity_ui->interaction_node_count; i++)
        {
            IUI_Node* node = insanity_ui->interaction_node_array[i];
            if (!insanity_ui_rect_hit(node)) continue;

            if (node->z_order >= best_z) // topmost wins, then last in array
            {
                best_z = node->z_order;
                new_hot = node->hash_id;
                // WARN("new hot: %llu", new_hot);
            }
        }
    }
    insanity_ui->hot_this_frame = new_hot;

    // claim active on unique press
    if (insanity_ui->active == 0 && new_hot != 0 && insanity_ui->mouse_down_unique)
    {
        insanity_ui->active = new_hot;
    }

    // clear active, click only counts if released over the SAME node ---
    insanity_ui->clicked_this_frame = 0;
    if (insanity_ui->active != 0 && insanity_ui->mouse_released_unique)
    {
        if (insanity_ui->hot_this_frame == insanity_ui->active)
            insanity_ui->clicked_this_frame = insanity_ui->active;

        insanity_ui->active = 0;
    }

    // build the result once, we dont want to do this with every query
    insanity_ui->interaction_result = (Insanity_UI_Event){0};

    insanity_ui->interaction_result.mouse_delta_x = insanity_ui->mouse_delta_x;
    insanity_ui->interaction_result.mouse_delta_y = insanity_ui->mouse_delta_y;
    insanity_ui->interaction_result.mouse_scrolled = insanity_ui->mouse_wheel_down || insanity_ui->mouse_wheel_up;
    insanity_ui->interaction_result.mouse_wheel_delta = insanity_ui->mouse_wheel_delta;
    insanity_ui->interaction_result.mouse_pos_x = insanity_ui->mouse_pos_x;
    insanity_ui->interaction_result.mouse_pos_y = insanity_ui->mouse_pos_y;

    if (insanity_ui->hot_this_frame != 0)
    {
        insanity_ui->interaction_result.hovered = true;
        insanity_ui->interaction_result.clicked = (insanity_ui->clicked_this_frame == insanity_ui->hot_this_frame);
    }
    if (insanity_ui->active != 0)
    {
        insanity_ui->interaction_result.pressed = insanity_ui->mouse_down;
    }


    // DEBUG("INSANITY UI: INTERACTION STATE: HOT: %llu, ACTIVE: %llu", insanity_ui->hot_this_frame, insanity_ui->active)
}


Insanity_UI_Event insanity_ui_event(IUI_Node* node, IUI_Event_Flags event_flags)
{
    if (event_flags & Insanity_UI_Event_Flags_Interaction)
    {
        insanity_ui->interaction_node_array[insanity_ui->interaction_node_count++] = node;
    }
    if (event_flags & Insanity_UI_Event_Flags_Navigation)
    {
        insanity_ui->navigation_node_array[insanity_ui->navigation_node_count++] = node;
    }

    /*if (event_flags & Insanity_UI_Event_Flags_Individual_Interaction)
    {
        Insanity_UI_Event out_event = {0};
    }*/


    if (node->hash_id == insanity_ui->hot_last_frame || node->hash_id == insanity_ui->active)
    {
        //we want to do a compare with our selected nodes flags to ensure that we are returning the results we actually wanted
        // we dont want to be listening for a pressed event if we didnt specify so
        //otherwise the interaction events get set by default

        return insanity_ui->interaction_result;
    }

    bool hit = insanity_ui_rect_hit(node);

    Insanity_UI_Event out_event = {0};
    if ((event_flags & Insanity_UI_Event_Flags_Individual_Hover) ||
        (event_flags & Insanity_UI_Event_Flags_Individual_Interaction))
    {
        if (hit)
        {
            out_event.hovered = true;
        }
    }

    if (event_flags & Insanity_UI_Event_Flags_Individual_Interaction)
    {
        if (hit)
        {
            out_event.mouse_delta_x = insanity_ui->mouse_delta_x;
            out_event.mouse_delta_y = insanity_ui->mouse_delta_y;
            out_event.mouse_scrolled = insanity_ui->mouse_wheel_down || insanity_ui->mouse_wheel_up;
            out_event.mouse_wheel_delta = insanity_ui->mouse_wheel_delta;
            out_event.mouse_pos_x = insanity_ui->mouse_pos_x;
            out_event.mouse_pos_y = insanity_ui->mouse_pos_y;

            // insanity_ui->interaction_result.hovered = true;
            out_event.clicked = insanity_ui->mouse_released_unique;
            out_event.pressed = insanity_ui->mouse_down;
        }
    }


    return out_event;
}


IUI_Node* insanity_ui_node(const char* name)
{
    IUI_Node* return_node = (IUI_Node*)_array_get(insanity_ui->ui_nodes,
                                                  insanity_ui->ui_nodes->num_items++);


    return_node->name_id = name;
    insanity_ui->hash = hash_32_continous(insanity_ui->hash, (u8*)name, strlen(name));
    return_node->hash_id = insanity_ui->hash;
    // return_node->ui_flags;
    return_node->type = Insanity_UI_Node_Type_Rect;
    return_node->color = insanity_ui->editor_style.error_color;


    return return_node;
}


IUI_Node* insanity_ui_node_cut_left(IUI_Node* parent, const char* name, f32 size)
{
    IUI_Node* new_node = insanity_ui_node(name);

    new_node->pos = parent->pos;
    new_node->size = (vec2s){.x = size, .y = parent->size.y};


    //shift parent to the right and reduce size
    parent->pos.x += new_node->size.x;
    parent->size.x -= new_node->size.x;

    return new_node;
}

IUI_Node* insanity_ui_node_cut_right(IUI_Node* parent, const char* name, f32 size)
{
    IUI_Node* new_node = insanity_ui_node(name);

    //size node and shift it to the proper spot
    new_node->size = (vec2s){.x = size, .y = parent->size.y};
    new_node->pos = (vec2s){parent->pos.x + parent->size.x - new_node->size.x, parent->pos.y};


    //reduce parent node size
    parent->size.x -= new_node->size.x;


    return new_node;
}

IUI_Node* insanity_ui_node_cut_top(IUI_Node* parent, const char* name, f32 size)
{
    IUI_Node* new_node = insanity_ui_node(name);

    //place new node where parent is, shift parent down and reduce parent size
    new_node->pos = parent->pos;
    new_node->size = (vec2s){.x = parent->size.x, .y = size};

    parent->pos.y += new_node->size.y;
    parent->size.y -= new_node->size.y;

    return new_node;
}

IUI_Node* insanity_ui_node_cut_bottom(IUI_Node* parent, const char* name, f32 size)
{
    IUI_Node* new_node = insanity_ui_node(name);


    //size node and shift it to the proper spot
    new_node->size = (vec2s){.x = parent->size.x, .y = size};
    new_node->pos = (vec2s){parent->pos.x, parent->pos.y + parent->size.y - new_node->size.y};


    //reduce parent node size
    parent->size.y -= new_node->size.y;

    return new_node;
}

IUI_Node* insanity_ui_node_cut_left_percent(IUI_Node* parent, const char* name, f32 percent_size)
{
    percent_size = clamp_f32(percent_size, 0.f, 1.f);
    return insanity_ui_node_cut_right(parent, name, parent->size.x * percent_size);
}

IUI_Node* insanity_ui_node_cut_right_percent(IUI_Node* parent, const char* name, f32 percent_size)
{
    percent_size = clamp_f32(percent_size, 0.f, 1.f);
    return insanity_ui_node_cut_right(parent, name, parent->size.x * percent_size);
}

IUI_Node* insanity_ui_node_cut_top_percent(IUI_Node* parent, const char* name, f32 percent_size)
{
    percent_size = clamp_f32(percent_size, 0.f, 1.f);
    return insanity_ui_node_cut_top(parent, name, parent->size.y * percent_size);
}

IUI_Node* insanity_ui_node_cut_bottom_percent(IUI_Node* parent, const char* name, f32 percent_size)
{
    percent_size = clamp_f32(percent_size, 0.f, 1.f);
    return insanity_ui_node_cut_bottom(parent, name, parent->size.y * percent_size);
}

IUI_Scroll* insanity_ui_scroll_begin(const char* name, vec2s pos, vec2s size)
{
    IUI_Node* scroll_view = insanity_ui_node(name);


    scroll_view->pos = pos;
    scroll_view->size = size;


    insanity_ui->scroll_state.starting_pos = scroll_view->pos;
    insanity_ui->scroll_state.scroll_container = scroll_view;
    insanity_ui->scroll_state.scroll_cursor = (vec2s){
        scroll_view->pos.x, scroll_view->pos.y - insanity_ui->scroll_state.scroll_offset
    };

    return &insanity_ui->scroll_state;
}


void insanity_scroll_end(IUI_Scroll* scroll)
{
    IUI_Node* scroll_bar = insanity_ui_node("bar");
    scroll_bar->color = COLOR_BLUE;

    scroll->scroll_bar = scroll_bar;


    float content_height = scroll->scroll_cursor.y - scroll->starting_pos.y + scroll->scroll_offset;
    float content_overflow = content_height - scroll->scroll_container->size.y - scroll->scroll_offset;
    float content_visible_range = content_height - scroll->scroll_container->size.y;

    //check if we need to show scroll bar
    //and catches edge case where overflow is 0 cause we are fully scrolled down
    if (content_overflow > 0 || scroll->scroll_bar_percent_offset >= 0.99)
    {
        //scroll bar
        scroll->scroll_bar->size = (vec2s){8.f, scroll->scroll_container->size.y * 0.2};

        //determines where the slider should be proportionally
        float scroll_bar_pos_x = scroll->scroll_container->pos.x + scroll->scroll_container->size.x - scroll->scroll_bar
            ->size.x;

        float scroll_bar_pos_y = scroll->starting_pos.y + ((scroll->scroll_container->size.y - scroll->scroll_bar->size.
                y) * scroll->
            scroll_bar_percent_offset);
        scroll->scroll_bar->pos = (vec2s){scroll_bar_pos_x, scroll_bar_pos_y};

        Insanity_UI_Event slider_bar_result =
            insanity_ui_event(scroll->scroll_bar, Insanity_UI_Event_Flags_Interaction);
        if (slider_bar_result.hovered)
        {
            scroll->scroll_bar->color = insanity_ui->editor_style.hovered_color;
            //handle window scrolling
            if (input_is_mouse_wheel_up())
            {
                scroll->scroll_bar_percent_offset = clamp_f32(scroll->scroll_bar_percent_offset - 0.1, 0, 1);
                // scroll->scroll_offset = clamp_f32(scroll->scroll_offset, 0, insanity_ui->screen_size.y);
                scroll->scroll_offset = content_visible_range * scroll->scroll_bar_percent_offset;
            }
            if (input_is_mouse_wheel_down())
            {
                // state.scroll_offset += 10;
                scroll->scroll_bar_percent_offset = clamp_f32(scroll->scroll_bar_percent_offset + 0.1, 0, 1);
                // scroll->scroll_offset = clamp_f32(scroll->scroll_offset, 0, insanity_ui->screen_size.y);
                scroll->scroll_offset = content_visible_range * scroll->scroll_bar_percent_offset;
            }
        }
        if (slider_bar_result.pressed)
        {
            float track_width = scroll->starting_pos.y - scroll->scroll_bar->size.y;
            float relative_y = insanity_ui->mouse_pos_y - scroll->starting_pos.y - (scroll->scroll_bar->size.y * 0.5f);
            float t = clamp_f32(relative_y / track_width, 0.0f, 1.0f);

            scroll->scroll_bar_percent_offset = 0 + t * (1 - 0);
            scroll->scroll_offset = (content_visible_range) * scroll->scroll_bar_percent_offset;
            scroll->scroll_bar->pos.y = scroll->starting_pos.y + ((scroll->scroll_container->size.y - scroll->scroll_bar
                ->size.y) * scroll->scroll_bar_percent_offset);
        }

        if (insanity_ui_event(scroll->scroll_container, Insanity_UI_Event_Flags_Individual_Hover).hovered)
        {
            //handle window scrolling
            if (input_is_mouse_wheel_up())
            {
                scroll->scroll_bar_percent_offset = clamp_f32(scroll->scroll_bar_percent_offset - 0.1, 0, 1);
                // scroll->scroll_offset = clamp_f32(scroll->scroll_offset, 0, insanity_ui->screen_size.y);
                scroll->scroll_offset = content_visible_range * scroll->scroll_bar_percent_offset;
            }
            if (input_is_mouse_wheel_down())
            {
                // state.scroll_offset += 10;
                scroll->scroll_bar_percent_offset = clamp_f32(scroll->scroll_bar_percent_offset + 0.1, 0, 1);
                // scroll->scroll_offset = clamp_f32(scroll->scroll_offset, 0, insanity_ui->screen_size.y);
                scroll->scroll_offset = content_visible_range * scroll->scroll_bar_percent_offset;
            }
        }

        /*//resize the window up
        scroll->window_region_size.y = content_height + (madness_ui_get_default_element_height() * 2);
        scroll->window_region_size.y = clamp_f32(state->window_region_size.y, MIN_UI_NODE_SCREEN_SIZE,
                                                madness_ui->screen_size.y - state->window_region_pos.y -
                                                (madness_ui_get_default_element_height()));*/
    }
}

void insanity_ui_scroll_advance(IUI_Scroll* scroll, IUI_Node* node)
{
    scroll->scroll_cursor.y += node->size.y;
}

void insanity_ui_scroll_advance_size(IUI_Scroll* scroll, vec2s size)
{
    scroll->scroll_cursor.y += size.y;
}


void insanity_ui_scroll_end()
{
    insanity_ui->scroll_array_count--;
}


IUI_Node* insanity_ui_text(const char* text)
{
    IUI_Node* text_node = insanity_ui_node(text);
    text_node->type = Insanity_UI_Node_Type_Text;
    text_node->ui_flags |= UI_FLAG_TEXT;
    text_node->color = INSANITY_UI_DEFAULT_TEXT_COLOR;
    //we need to calculate the text size
    text_node->outline_thickness = insanity_ui->text_outline;
    text_node->outline_color = insanity_ui->text_outline_color;


    String* text_string = STRING_CREATE_FROM_BUFFER_ALLOCATOR(text, insanity_ui->frame_allocator);
    text_node->slice_count = 1;

    text_node->slice_pointer = &insanity_ui->string_slice_array[insanity_ui->string_slice_count++];
    text_node->slice_pointer->slices.original_string = text_string;
    text_node->slice_pointer->slices.offset = 0;
    text_node->slice_pointer->slices.length = text_string->length;
    text_node->slice_pointer->slice_height = 0;
    text_node->total_slice_height = insanity_ui->editor_font_size;

    vec2s text_size = insanity_ui_text_calculate_size_fast(text, text_string->length);
    text_node->slice_pointer->slice_width = text_size.x;
    text_node->size = text_size;


    return text_node;
}

IUI_Node* insanity_ui_text_wrapped(const char* text, float max_width, UI_Text_Wrap wrap_mode)
{
    IUI_Node* text_node = insanity_ui_node(text);
    // text_node->text = STRING_CREATE_FROM_BUFFER_ALLOCATOR(text, insanity_ui->frame_allocator);
    text_node->type = Insanity_UI_Node_Type_Text;
    text_node->ui_flags |= UI_FLAG_TEXT;
    text_node->color = INSANITY_UI_DEFAULT_TEXT_COLOR;
    //we need to calculate the text size
    text_node->outline_thickness = insanity_ui->text_outline;
    text_node->outline_color = insanity_ui->text_outline_color;

    text_node->size.x = max_width;
    text_node->size.y = 0;


    //text wrapping algorithm
    // text_node->size
    f32 font_scalar = insanity_ui->editor_font_size / insanity_ui->default_font_size;

    Madness_Font font_data;
    texture_system_get_font(insanity_ui->asset_system->texture_system, insanity_ui->default_font_handle, &font_data);

    // this just calculates the height of the text wrapping
    //TODO: extract this function in case the user wants to re-adjust the wrapping,or even wrap later with a normal text node

    //for now this only sizes itself


    switch (wrap_mode)
    {
    case UI_Text_Wrap_None:
        break;
    case UI_Text_Wrap_Wrap:
        /*u32 current_line_text_length = 0;
        u32 cur_char_idx = 0;
        while (cur_char_idx < text_node->text->length)
        {
            //edge cases: what if one word is larger than the line
            //

            const char c = text[cur_char_idx];


            if (c < 32 || c >= 128) continue; // skip unsupported characters

            // Skip leading spaces at the beginning of a new line
            if (c == ' ')
            {
                cur_char_idx++;
            }


            Glyph* g = &font_data.glyphs[c - 32];

            current_line_text_length += (g->advance) * font_scalar; // move offset forward


            //the word is overflowing
            if (current_line_text_length > container->size.x)
            {
                //hit the end of the container we have to back track and find the first white space
                //edge case: no white space is found, therefore this is one word

                u32 cur_pos = cur_char_idx;
                while (cur_pos != 0)
                {
                    const char backtracked_char = text[cur_char_idx];
                    if (backtracked_char == ' ')
                    {
                        //found our breakline
                        break;
                    }
                    cur_pos--;
                }

                if (cur_pos == 0)
                {
                    //this means the entire word was on the line, which we just leave be as is, and move forward the index
                }
                else
                {
                    //reset the while index back to the start of the white space
                    cur_char_idx = cur_pos;
                }

                text_node->size.y += insanity_ui->editor_font_size;

                current_line_text_length = 0;
            }


            cur_char_idx++;
        }*/
        break;
    case UI_Text_Wrap_Newline:


        //loop till we hit a new line, then move down and keep going
        String* text_string = STRING_CREATE_FROM_BUFFER_ALLOCATOR(text, insanity_ui->frame_allocator);
        text_node->slice_pointer = &insanity_ui->string_slice_array[insanity_ui->string_slice_count++];
        text_node->slice_count = 1;
        // text_node->slice_count = 1;
        // text_node->slice_pointer->slices.original_string = text_string;
        // text_node->slice_pointer->slices.offset = 0;
        // text_node->slice_pointer->slices.length = text_string->length;
        // text_node->slice_pointer->slice_pos = glms_vec2_zero();

        vec2s text_size = insanity_ui_text_calculate_size_fast(text, text_string->length);
        // text_node->slice_pointer->slice_width = text_size.x;
        // text_node->size = text_size;

        u32 current_line = 0;
        u32 current_string_width = 0;
        u32 last_char_index = 0;
        u32 max_char_height = 0;
        u32 accumulated_height = 0;
        for (u32 char_index = 0; char_index < text_string->length; char_index++)
        {
            const char c = text[char_index];

            if (c < 32 || c >= 128) // skip unsupported characters
            {
                Glyph* g = &font_data.glyphs[c - 32];
                current_string_width += (g->advance) * font_scalar; // move offset forward
                max_char_height = max_f(max_char_height, g->height * font_scalar);
            }

            //see if we hit a newline
            if (c == '\n')
            {
                //increase the nodes size
                text_node->size.y += insanity_ui->editor_font_size;

                //create a new slice
                IUI_String_Slice* new_slice = &insanity_ui->string_slice_array[insanity_ui->string_slice_count++];
                text_node->slice_count++;
                // IUI_String_Slice* new_slice = &text_node->slice_pointer[text_node->slice_count - 1];
                // text_node->slice_pointer = &insanity_ui->string_slice_array[insanity_ui->string_slice_count++];
                new_slice->slices.original_string = text_string;
                new_slice->slices.length = char_index - last_char_index;
                new_slice->slices.offset = last_char_index;
                new_slice->debug_string = text_string->chars + new_slice->slices.offset;

                new_slice->slice_width = current_string_width;

                f32 intermediate = max_f(insanity_ui->editor_font_size, max_char_height );
                new_slice->slice_height = accumulated_height;
                accumulated_height += intermediate;
                text_node->total_slice_height += intermediate;

                max_char_height = 0;
                last_char_index = char_index + 1;
                current_string_width = 0;
                current_line++;
            }
        }

        break;
    }

    return text_node;
}



vec2s insanity_ui_text_calculate_size(const char* text)
{
    size_t text_size = strlen(text);
    return insanity_ui_text_calculate_size_fast(text, text_size);
}

vec2s insanity_ui_text_calculate_size_fast(const char* text, u32 string_size)
{
    f32 font_scalar = insanity_ui->editor_font_size / insanity_ui->default_font_size;

    vec2s out_text_size = glms_vec2_zero();

    Madness_Font font_data;
    texture_system_get_font(insanity_ui->asset_system->texture_system, insanity_ui->default_font_handle, &font_data);

    for (u64 i = 0; i < string_size; i++)
    {
        const char c = text[i];

        if (c < 32 || c >= 128) continue; // skip unsupported characters

        Glyph* g = &font_data.glyphs[c - 32];

        f32 y_height = ((f32)g->height * font_scalar);

        out_text_size.y = max_f(y_height, out_text_size.y);

        //printf("xpos %f, ypos%f, w%f, h%f\n", xpos, ypos, w, h);

        out_text_size.x += (g->advance) * font_scalar; // move offset forward
    }

    return out_text_size;
}


IUI_Node* insanity_ui_image(const char* name, Texture_Handle handle)
{
    IUI_Node* image_node = insanity_ui_node(name);
    image_node->ui_flags |= UI_FLAG_IMAGE;
    image_node->texture_handle = handle;
    image_node->uv_offset = (vec2s){0, 0};
    image_node->uv_size = (vec2s){1.0, 1.0};
    image_node->color = COLOR_WHITE;
    return image_node;
}

void insanity_ui_node_offset_x(IUI_Node* node_to_offset, IUI_Node* anchor_node,
                               float x_offset)
{
    node_to_offset->pos.x = anchor_node->pos.x + x_offset;
}

void insanity_ui_node_offset_y(IUI_Node* node_to_offset, IUI_Node* anchor_node,
                               float y_offset)
{
    node_to_offset->pos.y = anchor_node->pos.y + y_offset;
}

void insanity_ui_node_offset(IUI_Node* node_to_offset, IUI_Node* anchor_node, vec2s offset)
{
    insanity_ui_node_offset_x(node_to_offset, anchor_node, offset.x);
    insanity_ui_node_offset_y(node_to_offset, anchor_node, offset.y);
}

float _insanity_ui_node_align_axis(const float node_size_axis, const float container_pos_axis,
                                   const float container_size_axis,
                                   const UI_Alignment alignment)
{
    float node_pos_axis = container_pos_axis;
    float axis_space_remaining = 0;
    switch (alignment)
    {
    case UI_ALIGNMENT_LEFT_TOP:
        node_pos_axis = container_pos_axis;
        break;
    case UI_ALIGNMENT_X_CENTER:
        axis_space_remaining = container_size_axis - node_size_axis;
        node_pos_axis = container_pos_axis + (axis_space_remaining / 2.f);
        break;
    case UI_ALIGNMENT_RIGHT_BOTTOM:
        axis_space_remaining = container_size_axis - node_size_axis;
        node_pos_axis = container_pos_axis + axis_space_remaining;
        break;
    }


    return node_pos_axis;
}

void insanity_ui_node_align_x(IUI_Node* node_to_align, IUI_Node* container,
                              UI_Alignment x_alignment)
{
    node_to_align->pos.x = _insanity_ui_node_align_axis(node_to_align->size.x,
                                                        container->pos.x,
                                                        container->size.x,
                                                        x_alignment);
}

void insanity_ui_node_align_y(IUI_Node* node_to_align, IUI_Node* container,
                              UI_Alignment alignment)
{
    node_to_align->pos.y = _insanity_ui_node_align_axis(node_to_align->size.y,
                                                        container->pos.y,
                                                        container->size.y,
                                                        alignment);
}


void insanity_ui_node_align(IUI_Node* node_to_align, IUI_Node* container,
                            UI_Alignment x_alignment, UI_Alignment y_alignment)
{
    insanity_ui_node_align_x(node_to_align, container, x_alignment);
    insanity_ui_node_align_y(node_to_align, container, y_alignment);
    if (node_to_align->type == Insanity_UI_Node_Type_Text)
    {
        for (u32 i = 0; i < node_to_align->slice_count; i++)
        {
            IUI_String_Slice* slice = &node_to_align->slice_pointer[i];
            slice->alignment_x = x_alignment;
            slice->alignment_y = y_alignment;
        }
    }

}


void insanity_ui_node_expand(IUI_Node* node_to_expand, IUI_Node* container)
{
    node_to_expand->size = container->size;
}


void insanity_ui_node_expand_x(IUI_Node* node_to_expand, IUI_Node* container)
{
    node_to_expand->size.x = container->size.x;
}

void insanity_ui_node_expand_y(IUI_Node* node_to_expand, IUI_Node* container)
{
    node_to_expand->size.y = container->size.y;
}

void insanity_ui_node_expand_percent_x(IUI_Node* node_to_expand, IUI_Node* container, float percent)
{
    node_to_expand->size.x = container->size.x * percent;
}

void insanity_ui_node_expand_percent_y(IUI_Node* node_to_expand, IUI_Node* container, float percent)
{
    node_to_expand->size.y = container->size.y * percent;
}

void insanity_ui_node_expand_percent(IUI_Node* node_to_expand, IUI_Node* container, vec2s percent)
{
    insanity_ui_node_expand_percent_x(node_to_expand, container, percent.x);
    insanity_ui_node_expand_percent_y(node_to_expand, container, percent.y);
}


void insanity_ui_node_constraint_size(IUI_Node* node_to_constraint, IUI_Node* container)
{
    node_to_constraint->size.x = clamp_f32(node_to_constraint->size.x, 0, container->size.x);
    node_to_constraint->size.y = clamp_f32(node_to_constraint->size.y, 0, container->size.y);
}


bool insanity_ui_rect_hit(IUI_Node* node)
{
    //check if we are inside a ui_object
    //we are using the screen coordinates from the mouse,
    //and hopefully the passed in pos and size

    vec2s pos = node->pos;
    vec2s size = node->size;


    //top left
    if (pos.x > insanity_ui->mouse_pos_x) return false;
    if (pos.y > insanity_ui->mouse_pos_y) return false;

    // bottom left
    if (pos.x > insanity_ui->mouse_pos_x) return false;
    if (pos.y + size.y < insanity_ui->mouse_pos_y) return false;


    //top right
    if (pos.x + size.x < insanity_ui->mouse_pos_x) return false;
    if (pos.y > insanity_ui->mouse_pos_y) return false;

    // bottom right
    if (pos.x + size.x < insanity_ui->mouse_pos_x) return false;
    if (pos.y + size.y < insanity_ui->mouse_pos_y) return false;

    return true;
}

f32 lerp_f32(f32 a, f32 b, f32 t)
{
    return a + (b - a) * t;
}

void insanity_ui_test(float dt, float elapsed_time)
{
    PROFILE_ZONE(insanity_ui_test)


    insanity_ui_button("button", (vec2s){.x = 500.f, .y = 500.f});




    /*
    //normal text
    IUI_Node* text_container = insanity_ui_node("container");
    text_container->pos = (vec2s){.x = 300.f, .y = 300.f};
    text_container->size = (vec2s){.x = 100.f, .y = 100.f};
    text_container->color = COLOR_MAGENTA;

    IUI_Node* text_align_container = insanity_ui_node("container2");
    text_align_container->color = COLOR_RED;
    text_align_container->size = (vec2s){.x = 50.f, .y = 50.f};
    insanity_ui_node_align(text_align_container, text_container,
                           UI_ALIGNMENT_CENTER, UI_ALIGNMENT_CENTER);
    IUI_Node* test_text = insanity_ui_text("a\nb\nc\n");
    test_text->size = (vec2s){.x = 50.f, .y = 50.f};
    insanity_ui_node_align(test_text, text_container,
                           UI_ALIGNMENT_CENTER, UI_ALIGNMENT_CENTER);*/


    //new line wrapped
    IUI_Node* test_container2 = insanity_ui_node("container");
    test_container2->pos = (vec2s){.x = 100.f, .y = 100.f};
    test_container2->size = (vec2s){.x = 100.f, .y = 100.f};
    test_container2->color = COLOR_MAGENTA;

    IUI_Node* test_align_container = insanity_ui_node("container2");
    test_align_container->color = COLOR_RED;
    test_align_container->size = (vec2s){.x = 50.f, .y = 50.f};
    insanity_ui_node_align(test_align_container, test_container2,
                           UI_ALIGNMENT_CENTER, UI_ALIGNMENT_CENTER);
    IUI_Node* test_text_wrap_newline = insanity_ui_text_wrapped("a\nb\nc\n",
                                                                test_container2->size.x, UI_Text_Wrap_Newline);
    test_text_wrap_newline->size = (vec2s){.x = 50.f, .y = 50.f};
    insanity_ui_node_align(test_text_wrap_newline, test_container2,
                           UI_ALIGNMENT_CENTER, UI_ALIGNMENT_CENTER);




    /*IUI_Node* rect_container = insanity_ui_node("container");
    rect_container->size = (vec2s){100, 1000};
    IUI_Node* rect_bottom = insanity_ui_node_cut_top(rect_container, "blah", 100);
    rect_bottom->color = COLOR_RED;
    IUI_Node* rect_bottom2 = insanity_ui_node_cut_top(rect_container, "blah", 100);
    rect_bottom2->color = COLOR_BLUE;
    IUI_Node* rect_bottom3 = insanity_ui_node_cut_top(rect_container, "blah", 100);
    rect_bottom3->color = COLOR_MAGENTA;


    //example of animation, we can set specific functions anim_float(id, pos.x, on_hover, bool do_once)
    static f32 anim_time = 0;
    if (insanity_ui_event(rect_container, Insanity_UI_Event_Flags_Interaction).hovered)
    {
        anim_time += dt * 2.f;
        anim_time = clamp_f32(anim_time, 0, 1);
        rect_container->pos.x = glm_lerp(rect_container->pos.x, rect_container->pos.x+50.f, anim_time);
    }else
    {
        anim_time = 0.f;
    }*/

    // I want to be able to move the wrapped text and not break the other slices,
    // which means we can just defer moving all the other positions at the end


    // IUI_Node* test_text = insanity_ui_text("container");
    // insanity_ui_node_align(test_text, test_container, UI_ALIGNMENT_CENTER, UI_ALIGNMENT_CENTER);


    /*
    IUI_Node* container = insanity_ui_node("container");
    Insanity_UI_Event container_result = insanity_ui_event(container, Insanity_UI_Event_Flags_Interaction);
    container->pos = (vec2s){.x = 100, .y = (sinf(elapsed_time) * 500.f) + 100.f};
    container->size = (vec2s){100, 100};
    if (container_result.hovered)
    {
        container->color = COLOR_RED;
    }
    if (container_result.pressed)
    {
        container->color = COLOR_BLUE;
    }
    if (container_result.clicked)
    {
        FATAL("CLICKED");
    }


    IUI_Node* container2 = insanity_ui_node("container2");
    insanity_ui_event(container2, Insanity_UI_Event_Flags_Interaction);
    container2->pos = (vec2s){.x = 200, 200};
    container2->size = (vec2s){.x = 200, .y = (sinf(elapsed_time) * 100.f) + 200.f};
    insanity_ui_node_offset_y(container2, container, 50.f);
    if (container2->pos.y <= 0)
    {
        container2->pos.y = 0;
    }


    IUI_Node* text = insanity_ui_text("image");
    text->pos = (vec2s){container2->pos.x, container2->pos.y};
    */



    /*
    IUI_Node* image = insanity_ui_image("image", (Texture_Handle){0, 0});
    // insanity_ui_node_add_interaction(image, UI_EVENT_CLICK);
    image->pos = (vec2s){500, 500};
    image->size = (vec2s){100, 100};



    IUI_Node* rect_cut = insanity_ui_node("rect");
    rect_cut->pos = (vec2s){1200, 800};
    rect_cut->size = (vec2s){100, 100};
    rect_cut->color = COLOR_WHITE;
    if (insanity_ui_event(rect_cut, Insanity_UI_Event_Flags_Interaction | Insanity_UI_Event_Flags_Navigation).hovered)
    {
        rect_cut->color = COLOR_BLUE;
    }
    IUI_Node* rect_l = insanity_ui_node_cut_right_percent(rect_cut, "rect_l", 0.5);
    rect_l->color = COLOR_RED;
    // rect_l->size = glms_vec2_zero();
    if (insanity_ui_event(rect_l, Insanity_UI_Event_Flags_Interaction | Insanity_UI_Event_Flags_Navigation).hovered)
    {
        rect_l->color = COLOR_VIOLET;
        // rect_l->size = glms_vec2_mul(rect_l->size, (vec2s){0.5, 0.5});
        rect_l->pos.x += 5;
    }

    insanity_ui_drag(rect_cut);
    */


    IUI_Scroll* scroll = insanity_ui_scroll_begin("scroll", (vec2s){1200, 200}, (vec2s){400, 200});
    {
        scroll->scroll_container->color = COLOR_VIOLET;

        insanity_ui_scroll_advance_size(scroll, (vec2s){0, 16});
        for (u32 i = 0; i < 8; i++)
        {
            IUI_Node* button = insanity_ui_node("rect");
            button->pos = scroll->scroll_cursor;
            button->size = (vec2s){scroll->scroll_container->size.x * 0.9, 50};
            insanity_ui_node_align_x(button, scroll->scroll_container, UI_ALIGNMENT_CENTER);
            button->color = (vec3s){0.5 * (i + 1), 0.1 * (i + 1), 0.5 * (i + 1)};
            button->rounded_radius = 0.8f;
            button->ui_flags |= UI_FLAG_ROUND_CORNER;

            IUI_Node* icon = insanity_ui_image("icon", (Texture_Handle){0, 0});
            icon->pos = button->pos;
            // icon->size = (vec2s){button->size.x * 0.1,button->size.y};
            icon->size = (vec2s){button->size.y, button->size.y};
            icon->thickness = 1.0f;
            icon->ui_flags |= UI_FLAG_CIRCLE;

            char a[1000];
            size_t size = snprintf(NULL, 0, "ability %d", i);
            snprintf(a, sizeof(a), "ability %d, %d", i, 10);


            IUI_Node* text_ability = insanity_ui_text(a);
            insanity_ui_node_align(text_ability, button, UI_ALIGNMENT_CENTER, UI_ALIGNMENT_CENTER);

            if (insanity_ui_event(button, Insanity_UI_Event_Flags_Interaction | Insanity_UI_Event_Flags_Navigation).
                hovered)
            {
                button->color = glms_vec3_mul(button->color, (vec3s){0.5 * (i + 1), 0.1 * (i + 1), 0.5 * (i + 1)});
            }
            if (insanity_ui_event(button, Insanity_UI_Event_Flags_Interaction | Insanity_UI_Event_Flags_Navigation).
                pressed)
            {
                button->color = glms_vec3_add(button->color, (vec3s){0.5 * (i + 1), 0.1 * (i + 1), 0.5 * (i + 1)});
            }


            insanity_ui_scroll_advance_size(scroll, (vec2s){0, 16});
            insanity_ui_scroll_advance(scroll, button);
        }
    }
    insanity_scroll_end(scroll);


    PROFILE_ZONE_END(insanity_ui_test)
}
