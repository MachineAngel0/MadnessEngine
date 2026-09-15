#include "ui_insanity.h"
#include "math_lib.h"


static Insanity_UI* insanity_ui;


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


    insanity_ui->ui_nodes = array_create(Insanity_UI_Node, INSANITY_UI_MAX_NODE_COUNT, insanity_ui->allocator);
    Insanity_UI_Node node = {0};
    array_fill(insanity_ui->ui_nodes, &node);

    insanity_ui->pop_up_nodes = array_create(Insanity_UI_Node, INSANITY_UI_MAX_NODE_COUNT, insanity_ui->allocator);
    insanity_ui->modal_nodes = array_create(Insanity_UI_Node, INSANITY_UI_MAX_NODE_COUNT, insanity_ui->allocator);


    //interaction and events
    insanity_ui->active = -1;
    insanity_ui->hot = -1;


    insanity_ui->mouse_pos_x = -1.0f;
    insanity_ui->mouse_pos_y = -1.0f;

    insanity_ui->mouse_down = 0;
    insanity_ui->mouse_released_unique = 0;

    //TODO: replace with an in param
    insanity_ui->screen_size = (vec2s){800.0f, 600.0f};


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
    };

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

    //clear node/draw info
    allocator_clear(insanity_ui->frame_allocator);

    //clear ui interaction state
    insanity_ui->hash = hash_32_continous_start();


    insanity_ui->screen_size.x = screen_size_x;
    insanity_ui->screen_size.y = screen_size_y;


    array_zero(insanity_ui->ui_nodes);
    array_clear(insanity_ui->ui_nodes);

    array_clear(insanity_ui->pop_up_nodes);
    array_clear(insanity_ui->modal_nodes);

    insanity_ui->interaction_node_count = 0;


    //query our input state
    insanity_ui->mouse_released_unique = input_is_mouse_button_released_unique(MOUSE_BUTTON_LEFT);
    insanity_ui->mouse_down = input_is_mouse_button_pressed(MOUSE_BUTTON_LEFT);
    input_get_mouse_pos(&insanity_ui->mouse_pos_x, &insanity_ui->mouse_pos_y);
    input_get_mouse_change(&insanity_ui->mouse_delta_x, &insanity_ui->mouse_delta_y);
    insanity_ui->first_released_key = input_get_first_released_key();

    insanity_ui->key_shift = input_is_key_pressed(KEY_LSHIFT) || input_is_key_pressed(KEY_RSHIFT);
    insanity_ui->key_alt = input_is_key_pressed(KEY_LALT) || input_is_key_pressed(KEY_RALT);
    insanity_ui->key_ctrl = input_is_key_pressed(KEY_LCONTROL) || input_is_key_pressed(KEY_RCONTROL);
    insanity_ui->key_backspace = input_is_key_pressed(KEY_BACKSPACE);


    //input debug state
    DEBUG("INSANITY UI MOUSE POS: %d, %d", insanity_ui->mouse_pos_x, insanity_ui->mouse_pos_y);
    DEBUG("INSANITY UI MOUSE DELTA: %d, %d", insanity_ui->mouse_delta_x, insanity_ui->mouse_delta_y);

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
    }
}


Insanity_UI_Render_Packet insanity_ui_end(void)
{
    MASSERT(insanity_ui);

    insanity_ui_resolve_interaction();

    //OPTIMIZE: figure it out
    u32 render_node_count = 0;
    for (u32 i = 0; i < insanity_ui->ui_nodes->num_items; i++)
    {
        Insanity_UI_Node* node = &array_get(insanity_ui->ui_nodes, i, Insanity_UI_Node);
        switch (node->type)
        {
        case Insanity_UI_Node_Type_Rect:
            render_node_count++;
            break;
        case Insanity_UI_Node_Type_Text:
            render_node_count += node->text->length;
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
        Insanity_UI_Node* node_data = &array_get(insanity_ui->ui_nodes, i, Insanity_UI_Node);

        switch (node_data->type)
        {
        case Insanity_UI_Node_Type_Rect:

            UI_Render_Node* render_node = &insanity_ui->render_node_array[render_node_instance];

            render_node->ui_flags = node_data->ui_flags;
            render_node->pos = glms_vec2_div(node_data->pos, insanity_ui->screen_size);
            render_node->size = glms_vec2_div(node_data->size, insanity_ui->screen_size);
            render_node->rotation = deg_to_rad(node_data->rotation);

            render_node->thickness = node_data->thickness;

            render_node->outline_thickness = node_data->outline_thickness;

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
            vec2s text_current_pos = node_data->pos;
            f32 font_scalar = insanity_ui->editor_font_size / insanity_ui->default_font_size;
            Madness_Font font_data;
            texture_system_get_font(insanity_ui->asset_system->texture_system, insanity_ui->default_font_handle,
                                    &font_data);

            for (u64 text_idx = 0; text_idx < node_data->text->length; text_idx++)
            {
                const char c = node_data->text->chars[text_idx];

                if (c < 32 || c >= 128) continue; // skip unsupported characters

                Glyph* g = &font_data.glyphs[c - 32];

                f32 x_position = text_current_pos.x + ((float)g->xoff * font_scalar);
                f32 y_position = text_current_pos.y + ((float)g->yoff * font_scalar);

                f32 x_width = ((f32)g->width * font_scalar);
                f32 y_height = ((f32)g->height * font_scalar);

                UI_Render_Node* text_render_node = &insanity_ui->render_node_array[render_node_instance];
                text_render_node->pos = (vec2s){/*node_data->pos.x +*/ x_position, /*node_data->pos.y +*/ y_position};
                text_render_node->pos = glms_vec2_div(text_render_node->pos, insanity_ui->screen_size);

                text_render_node->size = (vec2s){x_width, y_height};
                text_render_node->size = glms_vec2_div(text_render_node->size, insanity_ui->screen_size);

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
    insanity_ui->hot = 0;
    insanity_ui->active = 0;
    for (u32 interaction_index = 0; interaction_index < insanity_ui->interaction_node_count; interaction_index++)
    {
        Insanity_UI_Interaction_Node* interaction_node = &insanity_ui->interaction_node_array[interaction_index];
        if (interaction_node->flags | UI_EVENT_CLICK)
        {
            if (insanity_ui_rect_hit(interaction_node->node))
            {
                WARN("INTERACTION RESOLVE: NODE HOT: %s", interaction_node->node->name_id)
                //do something
                insanity_ui->hot = interaction_node->node->hash_id;
                insanity_ui->active = interaction_node->node->hash_id;
            }
        }
    }

    DEBUG("INSANITY UI: INTERACTION STATE: HOT: %d, ACTIVE: %d", insanity_ui->hot, insanity_ui->active)
}


Insanity_UI_Node* insanity_ui_node(const char* name, Insanity_UI_Interaction_Flags interaction_flags)
{
    Insanity_UI_Node* return_node;
    return_node = (Insanity_UI_Node*)_array_get(insanity_ui->ui_nodes, insanity_ui->ui_nodes->num_items++);
    // Insanity_UI_Node* node = (Insanity_UI_Node*)_array_get(insanity_ui->pop_up_nodes, insanity_ui->pop_up_nodes->num_items++);
    // Insanity_UI_Node* node = (Insanity_UI_Node*)_array_get(insanity_ui->overlay_nodes, insanity_ui->overlay_nodes->num_items++);

    return_node->name_id = name;
    insanity_ui->hash = hash_32_continous(insanity_ui->hash, (u8*)name, strlen(name));
    return_node->hash_id = insanity_ui->hash;
    // return_node->ui_flags;
    return_node->type = Insanity_UI_Node_Type_Rect;
    return_node->color = COLOR_GREEN;

    //add our node to the interaction resolve array
    insanity_ui_node_add_interaction(return_node, interaction_flags);


    return return_node;
}


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
            .intial_position_percent = initial_percent_pos,
            .intial_size_percent = initial_percent_size,
            .window_pos = glms_vec2_mul(insanity_ui->screen_size, initial_percent_pos),
            .window_size = glms_vec2_mul(insanity_ui->screen_size, initial_percent_size),
            .header_pos = glms_vec2_zero(),
            .header_size = glms_vec2_zero(),
            .scroll_offset = 0,
            .scroll_bar_percent_offset = 0,
            .window_relative_cursor_pos = glms_vec2_zero(),
            .flags = flags,
        };
    }


    if (flags & Insanity_UI_Window_Flag_Header)
    {
    }

    if (flags & Insanity_UI_Window_Flag_Dont_Save_Position)
    {
    }

    if (flags & Insanity_UI_Window_Flag_Scrollable)
    {
    }

    if (flags & Insanity_UI_Window_Flag_Resizable)
    {
    }

    if (flags & Insanity_UI_Window_Flag_Movable)
    {
    }

    if (flags & Insanity_UI_Window_Flag_Closable)
    {
        //node on the top right with a interaction
    }

    if (flags & Insanity_UI_Window_Flag_No_Background_Color)
    {
    }

    Insanity_UI_Window window = {.name = window_name, .flags = flags};

    stack_push(insanity_ui->window_states_stack, &window);
}

void insanity_ui_window_end()
{
    Insanity_UI_Window window = stack_pop(insanity_ui->window_states_stack, Insanity_UI_Window);

    //do stuff
}

void insanity_ui_window_cursor_offset(vec2s offset)
{
    glms_vec2_add(insanity_ui->cursor_position, offset);
}

void insanity_ui_window_cursor_advance_down(Insanity_UI_Node* node)
{
    insanity_ui->cursor_position.y += node->size.y;
}

Insanity_UI_Node* insanity_ui_text(const char* text)
{
    Insanity_UI_Node* text_node = insanity_ui_node(text, 0);
    text_node->text = STRING_CREATE_FROM_BUFFER_ALLOCATOR(text, insanity_ui->frame_allocator);
    text_node->type = Insanity_UI_Node_Type_Text;
    text_node->ui_flags |= UI_FLAG_TEXT;
    text_node->color = COLOR_WHITE;
    //we need to calculate the text size
    size_t text_size = strlen(text);
    text_node->size = insanity_ui_text_calculate_size_fast(text, text_size);


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


Insanity_UI_Node* insanity_ui_image(const char* name, Texture_Handle handle)
{
    Insanity_UI_Node* image_node = insanity_ui_node(name, 0);
    image_node->ui_flags |= UI_FLAG_IMAGE;
    image_node->texture_handle = handle;
    image_node->uv_offset = (vec2s){0, 0};
    image_node->uv_size = (vec2s){1.0, 1.0};
    image_node->color = COLOR_WHITE;
    return image_node;
}

void insanity_ui_node_offset_from_node_x(Insanity_UI_Node* anchor_node, Insanity_UI_Node* node_to_offset,
                                         float x_offset)
{
    node_to_offset->pos.x = anchor_node->pos.x + x_offset;
}

void insanity_ui_node_offset_from_node_y(Insanity_UI_Node* anchor_node, Insanity_UI_Node* node_to_offset,
                                         float y_offset)
{
    node_to_offset->pos.y = anchor_node->pos.y + y_offset;
}

void insanity_ui_node_offset_from_node(Insanity_UI_Node* anchor_node, Insanity_UI_Node* node_to_offset, vec2s offset)
{
    insanity_ui_node_offset_from_node_x(anchor_node, node_to_offset, offset.x);
    insanity_ui_node_offset_from_node_y(anchor_node, node_to_offset, offset.y);
}

void insanity_ui_node_align_to_node_horizontal(Insanity_UI_Node* container, Insanity_UI_Node* node_to_align,
                                               UI_Alignment x_alignment)
{
    float horizontal_space_remaining = 0;
    switch (x_alignment)
    {
    case UI_ALIGNMENT_LEFT:
        node_to_align->pos.x = container->pos.x;
        break;
    case UI_ALIGNMENT_CENTER:
        horizontal_space_remaining = container->size.x - node_to_align->size.x;
        node_to_align->pos.x = container->pos.x + (horizontal_space_remaining / 2.f);
        break;
    case UI_ALIGNMENT_RIGHT:
        horizontal_space_remaining = container->size.x - node_to_align->size.x;
        node_to_align->pos.x = container->pos.x + horizontal_space_remaining;
        break;
    }
}

void insanity_ui_node_align_to_node_vertical(Insanity_UI_Node* container, Insanity_UI_Node* node_to_align,
                                             UI_Alignment y_alignment)
{
    float vertical_space_remaining = 0;
    switch (y_alignment)
    {
    case UI_ALIGNMENT_LEFT:
        node_to_align->pos.y = container->pos.y;
        break;
    case UI_ALIGNMENT_CENTER:
        vertical_space_remaining = container->size.y - node_to_align->size.y;
        node_to_align->pos.y = container->pos.y + (vertical_space_remaining / 2.f);
        break;
    case UI_ALIGNMENT_RIGHT:
        vertical_space_remaining = container->size.y - node_to_align->size.y;
        node_to_align->pos.y = container->pos.y + vertical_space_remaining;
        break;
    }
}


void insanity_ui_node_align_to_node(Insanity_UI_Node* container, Insanity_UI_Node* node_to_align,
                                    UI_Alignment x_alignment, UI_Alignment y_alignment)
{
    insanity_ui_node_align_to_node_horizontal(container, node_to_align, x_alignment);

    insanity_ui_node_align_to_node_vertical(container, node_to_align, y_alignment);
}

Insanity_UI_Interaction_Result insanity_ui_node_get_interaction(Insanity_UI_Node* node)
{
    if (node->hash_id == insanity_ui->hot || node->hash_id == insanity_ui->active)
    {
        return insanity_ui->interaction_result;
    }

    return (Insanity_UI_Interaction_Result){0};
}

void insanity_ui_node_add_interaction(Insanity_UI_Node* node, Insanity_UI_Interaction_Flags flags)
{
    if (flags == 0)
    {
        INFO("insanity_ui_node_add_interaction: no flags passed in ");
        return;
    }
    insanity_ui->interaction_node_array[insanity_ui->interaction_node_count++] = (Insanity_UI_Interaction_Node){
        .node = node,
        .flags = flags,
    };
}

bool insanity_ui_rect_hit(Insanity_UI_Node* node)
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

void insanity_ui_test(float dt, float elapsed_time)
{
    //scenarios - making an ability list
    // window_begin(scrollbar || autosize | max_size_clamp)
    //   loop
    //      container = Node()
    //      icon = Node()
    //      Node_offset(conatiner, icon, right_offset)
    //      text = Node()
    //      Node_offset(icon, text)
    //      Node_align(container, text)
    //      Advance_cursor(container)
    //      //handling input
    //      if(container.event.pressed)
    //   loop end
    // window_end()


    /*
    insanity_ui_window_begin("insanity", (vec2s){0.5, 0.5}, (vec2s){0.5, 0.5}, 0);
    {
        //for loop here
        insanity_ui_window_cursor_offset(insanity_ui->default_padding);
        Insanity_UI_Node* container = insanity_ui_node("container", UI_EVENT_CLICK);
        Insanity_UI_Node* icon = insanity_ui_image("icon", (Texture_Handle){0, 0});
        insanity_ui_window_cursor_advance_down(icon);
        Insanity_UI_Node* text = insanity_ui_text("container");
        insanity_ui_node_align_to_node(container, text, UI_ALIGNMENT_CENTER, UI_ALIGNMENT_CENTER);
        insanity_ui_window_cursor_advance_down(icon);
        if (insanity_ui_node_get_interaction(container).clicked)
        {
            //do something with the ability
            DEBUG("ABILITY CLICKED")
        }
    }
    insanity_ui_window_end();
    */


    Insanity_UI_Node* container = insanity_ui_node("container", UI_EVENT_CLICK);
    container->pos = (vec2s){.x = 100, .y = (sinf(elapsed_time) * 500.f) + 100.f};
    container->size = (vec2s){100, 100};
    if (insanity_ui_node_get_interaction(container).hovered)
    {
        container->color = COLOR_RED;
        FATAL("container CLICKED")
    }

    Insanity_UI_Node* container2 = insanity_ui_node("container2", UI_EVENT_CLICK);
    container2->pos = (vec2s){.x = 200, 200};
    container2->size = (vec2s){.x = 200, .y = (sinf(elapsed_time) * 100.f) + 200.f};
    insanity_ui_node_offset_from_node_y(container, container2, 50.f);
    if (container2->pos.y <= 0)
    {
        container2->pos.y = 0;
    }


    Insanity_UI_Node* image = insanity_ui_image("image", (Texture_Handle){0, 0});
    // insanity_ui_node_add_interaction(image, UI_EVENT_CLICK);
    image->pos = (vec2s){500, 500};
    image->size = (vec2s){100, 100};

    Insanity_UI_Node* text = insanity_ui_text("image");
    text->pos = (vec2s){container2->pos.x , container2->pos.y};

    if (insanity_ui_button("button").hovered)
    {
        DEBUG("BUTTON HOVERED");
    }
}

Insanity_UI_Interaction_Result insanity_ui_button(const char* label)
{
    //TODO: check the window state
    Insanity_UI_Node* container = insanity_ui_node(label, UI_EVENT_CLICK);
    container->pos = (vec2s){1000, 500};
    container->size = (vec2s){100, 100};
    container->color = insanity_ui->editor_style.color;
    Insanity_UI_Node* text = insanity_ui_text(label);
    // text->pos = container->pos;
    insanity_ui_node_align_to_node(container, text, UI_ALIGNMENT_CENTER, UI_ALIGNMENT_CENTER);

    Insanity_UI_Interaction_Result result = insanity_ui_node_get_interaction(container);


    if (result.hovered)
    {
        container->color = insanity_ui->editor_style.hovered_color;
    }
    if (result.pressed)
    {
        container->color = insanity_ui->editor_style.pressed_color;
    }

    return result;
}
