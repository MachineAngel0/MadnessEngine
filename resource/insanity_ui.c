#include "insanity_ui.h"
#include "math_lib.h"


static Insanity_UI* insanity_ui;


bool insanity_ui_init(Memory_System* memory_system, Input_System* input_system,
                      Resource_System* resource_system)
{
    insanity_ui = memory_system_alloc(memory_system, sizeof(Insanity_UI), MEMORY_SUBSYSTEM_UI);
    MASSERT(insanity_ui);

    u64 ui_arena_mem_size = MB(16);
    u64 ui_frame_arena_mem_size = MB(16);
    insanity_ui->mem_tracker = memory_system_get_memory_tracker(memory_system->memory_tracker_system,
                                                                STRING("MADNESS UI"),
                                                                ui_arena_mem_size + ui_frame_arena_mem_size);

    insanity_ui->arena = memory_system_alloc(memory_system, sizeof(Allocator), MEMORY_SUBSYSTEM_UI);
    insanity_ui->frame_arena = memory_system_alloc(memory_system, sizeof(Allocator), MEMORY_SUBSYSTEM_UI);

    void* arena_memory = memory_system_alloc(memory_system, ui_arena_mem_size, MEMORY_SUBSYSTEM_UI);
    void* frame_arena_memory = memory_system_alloc(memory_system, ui_frame_arena_mem_size, MEMORY_SUBSYSTEM_UI);

    allocator_init(insanity_ui->arena, arena_memory, ui_arena_mem_size, insanity_ui->mem_tracker);
    allocator_init(insanity_ui->frame_arena, frame_arena_memory, ui_arena_mem_size, insanity_ui->mem_tracker);

    insanity_ui->input_system_reference = input_system;
    insanity_ui->resource_system = resource_system;


    insanity_ui->default_font_size = INSANITY_DEFAULT_FONT_SIZE;
    insanity_ui->editor_font_size = INSANITY_EDITOR_FONT_SIZE;

    insanity_ui->ui_nodes = Insanity_UI_Node_array_create(INSANITY_MAX_UI_NODE_COUNT, NULL); // TODO: use an allocator

    //stacks
    insanity_ui->pos_stack = stack_create(sizeof(vec2), 100, insanity_ui->arena);
    insanity_ui->size_stack = stack_create(sizeof(vec2), 100, insanity_ui->arena);

    insanity_ui->layout_stack = stack_create(sizeof(Insanity_UI_Layout), 100, insanity_ui->arena);
    insanity_ui->padding_stack = stack_create(sizeof(vec2), 100, insanity_ui->arena);

    insanity_ui->flag_stack = stack_create(sizeof(Insanity_UI_Property_Flags), 100, insanity_ui->arena);

    insanity_ui->float_stack = stack_create(sizeof(float), 100, insanity_ui->arena);
    // insanity_ui->style_stack = stack_create(sizeof(Insanity_UI_Property_Flags), 100, insanity_ui->arena);


    //states/hash tables
    insanity_ui->drag_state = HASH_TABLE_CREATE_DEFAULT_SIZE(vec2);
    insanity_ui->float_state = HASH_TABLE_CREATE_DEFAULT_SIZE(float);
    insanity_ui->text_box_state = HASH_TABLE_CREATE_DEFAULT_SIZE(String_Builder*);

    //defaults for stacks
    insanity_ui->rounded_radius_stack = 0.2;
    insanity_ui->outline_thickness_stack = 0.2;


    //interaction and events
    insanity_ui->active = -1;
    insanity_ui->hot = -1;


    insanity_ui->mouse_pos_x = -1.0f;
    insanity_ui->mouse_pos_y = -1.0f;

    insanity_ui->mouse_down = 0;
    insanity_ui->mouse_released_unique = 0;

    //TODO: replace with an in param
    insanity_ui->screen_size = (vec2){800.0f, 600.0f};


    //
    if (!texture_system_load_msdf_font(resource_system->texture_system, "../z_assets/msdf_fonts/arial_msdf.png",
                                       &insanity_ui->default_font_handle,
                                       insanity_ui->arena))
    {
        MASSERT_MSG(false, "UI SYSTEM Failed to load default msdf font");
    };


    INFO("INSANITY UI CREATED");
    return true;
}

bool insanity_ui_shutdown(void)
{
    return true;
}

void insanity_ui_begin(i32 screen_size_x, i32 screen_size_y)
{
    MASSERT(insanity_ui);
    //clear draw info and reset the hot id
    allocator_clear(insanity_ui->frame_arena);

    //std::cout << "MOUSE STATE:" << Insanity_UI.mouse_down << '\n';
    insanity_ui->editor_style = (Insanity_UI_Editor_Style){
        .layout_color = COLOR_PURPLE_PALETTE_DARK, .layout_accent_color = COLOR_PURPLE_PALETTE_PURPLE,
        .text_color = COLOR_PURPLE_PALETTE_LIGHT, .textbox_color = COLOR_PURPLE_PALETTE_DARK2,
        .custom_widget_color = COLOR_PURPLE_PALETTE_PURPLE_LIGHT,
        .color = COLOR_PURPLE_PALETTE_PURPLE_STRONG, .hovered_color = COLOR_PURPLE_PALETTE_PURPLE_LIGHT2,
        .pressed_color = COLOR_PURPLE_PALETTE_DARK2,
        .outline_color = COLOR_BLACK,
    };

    insanity_ui->screen_size.x = screen_size_x;
    insanity_ui->screen_size.y = screen_size_y;


    Insanity_UI_Node_array_zero(insanity_ui->ui_nodes);
    Insanity_UI_Node_array_clear(insanity_ui->ui_nodes);

    Insanity_UI_Property_Flags no_flag = UI_FLAG_NONE;
    stack_clear(insanity_ui->flag_stack);
    stack_push(insanity_ui->flag_stack, &no_flag);

    //pos and size
    vec2 default_pos = {0, 0};
    stack_clear(insanity_ui->pos_stack);
    stack_push(insanity_ui->pos_stack, &default_pos);

    vec2 no_size = {0, 0};
    stack_clear(insanity_ui->size_stack);
    stack_push(insanity_ui->size_stack, &no_size);

    //layout
    Insanity_UI_Layout default_layout = Insanity_UI_LAYOUT_VERTICAL;
    stack_clear(insanity_ui->layout_stack);
    stack_push(insanity_ui->layout_stack, &default_layout);


    vec2 default_padding = {0, 0};
    stack_clear(insanity_ui->padding_stack);
    stack_push(insanity_ui->padding_stack, &default_padding);


    //styling
    stack_clear(insanity_ui->float_stack);


    insanity_ui->string_stack = STRING("INVALID STRING");
    insanity_ui->image_stack = texture_system_get_default_texture(insanity_ui->resource_system->texture_system);
    // insanity_ui->rounded_radius_stack = 0.2;
    // insanity_ui->outline_thickness_stack = 0.2;

    insanity_ui->increment_value_stack = 1;


    insanity_ui->hot = -1;

    insanity_ui->mouse_down = input_is_mouse_button_pressed(insanity_ui->input_system_reference, MOUSE_BUTTON_LEFT);
    insanity_ui->mouse_released_unique = input_is_mouse_button_released_unique(
        insanity_ui->input_system_reference, MOUSE_BUTTON_LEFT);
    //this can be 0 if invalid
    insanity_ui->first_released_key = input_get_first_released_key(insanity_ui->input_system_reference);
}


void insanity_ui_end(Resource_System* resource_system)
{
    MASSERT(insanity_ui);

    insanity_ui_passes();

    insanity_ui_generate_draw();

    //SET UI STATE FOR NEXT FRAME //

    //check if mouse is released, if so reset the active id
    //also update the mouse state

    //printf("HOT ID: %d, HOT LAYER: %d\n", Madness_UI->hot.ID, Madness_UI->hot.layer);
    //printf("ACTIVE ID: %d, ACTIVE LAYER: %d\n", Madness_UI->active.ID, Madness_UI->active.layer);


    if (input_is_mouse_button_released(insanity_ui->input_system_reference, MOUSE_BUTTON_LEFT))
    {
        insanity_ui->active = -1;
    }

    //update mouse state
    // DEBUG("MOUSE DOWN %d", Madness_UI->mouse_down)
    //update mouse pos
    input_get_mouse_pos(insanity_ui->input_system_reference, &insanity_ui->mouse_pos_x, &insanity_ui->mouse_pos_y);
    //update mouse delta/change
    input_get_mouse_change(insanity_ui->input_system_reference, &insanity_ui->mouse_delta_x,
                           &insanity_ui->mouse_delta_y);
}


void insanity_ui_passes()
{
    //debug check
    for (u32 i = 0; i < insanity_ui->ui_nodes->num_items; i++)
    {
        Insanity_UI_Node* node = &insanity_ui->ui_nodes->data[i];
        if (node->size.x > 1.0f)
        {
            M_ERROR("OVER 1 X SIZE ON NODE: %s", node->id)
        }
        if (node->size.y > 1.0f)
        {
            M_ERROR("OVER 1 Y SIZE ON NODE: %s", node->id)
        }
    }

    //passes

    //child sizing
    for (u32 i = 0; i < insanity_ui->ui_nodes->num_items; i++)
    {
        //size child to a percentage of the parent
        //the order ensures all parents are sized first before the children are sized

        Insanity_UI_Node* node = &insanity_ui->ui_nodes->data[i];
        float overflow_x = 0;
        float overflow_y = 0;

        for (u32 child_idx = 0; child_idx < node->child_count; child_idx++)
        {
            Insanity_UI_Node* child_node = node->children[child_idx];

            child_node->size.x = node->size.x * child_node->size.x;
            child_node->size.y = node->size.y * child_node->size.y;

            //debug check
            switch (node->layout)
            {
            case Insanity_UI_LAYOUT_VERTICAL:
                overflow_y += child_node->size.y;
                break;
            case Insanity_UI_LAYOUT_HORIZONTAL:
                overflow_x += child_node->size.x;
                break;
            }
        }

        //debug check
        if (overflow_x > 1)
        {
            M_ERROR("OVERFLOW X ON NODE: %s", node->id)
        }
        if (overflow_y > 1)
        {
            M_ERROR("OVERFLOW Y ON NODE: %s", node->id)
        }
    }


    //position pass
    for (u32 i = 0; i < insanity_ui->ui_nodes->num_items; i++)
    {
        Insanity_UI_Node* node = &insanity_ui->ui_nodes->data[i];
        float pos_x = node->pos.x;
        float pos_y = node->pos.y;

        for (u32 child_idx = 0; child_idx < node->child_count; child_idx++)
        {
            Insanity_UI_Node* child_node = node->children[child_idx];

            child_node->pos.x = pos_x;
            child_node->pos.y = pos_y;

            switch (node->layout)
            {
            case Insanity_UI_LAYOUT_HORIZONTAL:
                pos_x += child_node->size.x;
                break;
            case Insanity_UI_LAYOUT_VERTICAL:
                pos_y += child_node->size.y;
                break;
            }
        }
    }

    // feature pass before scaling images up
    for (u32 i = 0; i < insanity_ui->ui_nodes->num_items; i++)
    {
        Insanity_UI_Node* node = &insanity_ui->ui_nodes->data[i];
        if (node->ui_flags & UI_FLAG_IMAGE)
        {
            //properly ratio the texture
            if (node->size.x > node->size.y)
            {
                float ratio = node->size.x / node->size.y;
                node->uv_size.x *= ratio;
            }
            else if (node->size.y > node->size.x)
            {
                float ratio = node->size.x / node->size.y;
                node->uv_size.y *= ratio;
            }
        }
    }


    //scale up the sizes of all the elements to the proper screen size
    //NOTE: this is done because when a state is stored like position for drag,
    // when we update it, it's best to update the relative values 0-1
    // instead of directly modifying the screen position value ie: 1280x720
    for (int i = 0; i < insanity_ui->ui_nodes->num_items; i++)
    {
        Insanity_UI_Node* node = &insanity_ui->ui_nodes->data[i];

        node->pos = vec2_mul(node->pos, insanity_ui->screen_size);
        node->size = vec2_mul(node->size, insanity_ui->screen_size);
    }


    //feature pass
    for (u32 i = 0; i < insanity_ui->ui_nodes->num_items; i++)
    {
        Insanity_UI_Node* node = &insanity_ui->ui_nodes->data[i];

        bool hover_hit = (insanity_rect_hit(node->pos, node->size));


        if (hover_hit)
        {
            if (node->ui_flags & UI_FLAG_CLICKABLE)
            {
                node->color = insanity_ui->editor_style.color;

                {
                    if (insanity_ui->mouse_down)
                    {
                        node->color = insanity_ui->editor_style.pressed_color;
                    }
                    else
                    {
                        node->color = insanity_ui->editor_style.hovered_color;
                    }
                }
            }
        }


        if (node->ui_flags & UI_FLAG_DRAGGABLE)
        {
            if (hover_hit)
            {
                if (insanity_ui->mouse_down)
                {
                    vec2 pos;
                    hash_table_get(insanity_ui->drag_state, node->id, &pos);
                    // node->pos.x += insanity_ui->mouse_delta_x;
                    // node->pos.y += insanity_ui->mouse_delta_y;
                    pos.x += insanity_ui->mouse_delta_x / insanity_ui->screen_size.x;
                    pos.y += insanity_ui->mouse_delta_y / insanity_ui->screen_size.y;
                    hash_table_set(insanity_ui->drag_state, node->id, &pos);
                }
            }
        }

        if (node->ui_flags & UI_FLAG_SCROLL_FLOAT)
        {
            if (hover_hit)
            {
                float val;
                if (hash_table_get(insanity_ui->float_state, node->id, &val))
                {
                    if (input_is_mouse_wheel_up(insanity_ui->input_system_reference))
                    {
                        val += insanity_ui->increment_value_stack;
                        hash_table_set(insanity_ui->float_state, node->id, &val);
                    }
                    if (input_is_mouse_wheel_down(insanity_ui->input_system_reference))
                    {
                        val -= insanity_ui->increment_value_stack;
                        hash_table_set(insanity_ui->float_state, node->id, &val);
                    }
                }
            }
        }
        if (node->ui_flags & UI_FLAG_TEXT_INPUT)
        {
            if (hover_hit)
            {
                String_Builder* builder;
                if (hash_table_get(insanity_ui->text_box_state, node->id, &builder))
                {
                    if (input_key_released_unique(insanity_ui->input_system_reference, KEY_BACKSPACE))
                    {
                        string_builder_decrement(builder);
                    }

                    if (insanity_ui->first_released_key)
                    {
                        string_builder_append_char(builder, &insanity_ui->first_released_key, 1);
                    }
                }
            }
        }
    }
}


void insanity_ui_generate_draw(void)
{
    //NEW DRAW DATA

    insanity_ui->node_draw_data_array = allocator_alloc(insanity_ui->frame_arena,
                                                    insanity_ui->ui_nodes->num_items * sizeof(
                                                        Insanity_UI_Node_Draw_Data));
    insanity_ui->node_draw_data_array_size = insanity_ui->ui_nodes->num_items;

    for (u32 i = 0; i < insanity_ui->ui_nodes->num_items; i++)
    {
        Insanity_UI_Node* node_data = &insanity_ui->ui_nodes->data[i];
        Insanity_UI_Node_Draw_Data* draw_data = &insanity_ui->node_draw_data_array[i];

        draw_data->ui_flags = node_data->ui_flags;
        draw_data->pos = vec2_div(node_data->pos, insanity_ui->screen_size);
        draw_data->size = vec2_div(node_data->size, insanity_ui->screen_size);
        draw_data->rotation = deg_to_rad(node_data->rotation);


        draw_data->rounded_radius = node_data->rounded_radius;

        draw_data->outline_color = node_data->outline_color;
        draw_data->outline_thickness = node_data->outline_thickness;


        draw_data->thickness = node_data->thickness;

        draw_data->texture_handle = node_data->texture_handle.handle;
        draw_data->uv_offset = node_data->uv_offset;
        draw_data->uv_size = node_data->uv_size;


        draw_data->color = node_data->color;
        draw_data->background_color = node_data->background_color;
    }
}

Insanity_UI_Render_Packet insanity_get_render_data()
{
    // return (Insanity_UI_Render_Packet){.ui_nodes = insanity_ui->node_draw_data_array, .ui_nodes_text = insanity_ui->ui_nodes_text};
    return (Insanity_UI_Render_Packet){
        .ui_data = insanity_ui->node_draw_data_array,
        .ui_data_size = insanity_ui->node_draw_data_array_size,
        .ui_data_bytes = insanity_ui->node_draw_data_array_size * sizeof(Insanity_UI_Node_Draw_Data),
    };
}

void insanity_ui_push_flags(Insanity_UI_Property_Flags flags)
{
    Insanity_UI_Property_Flags flags_t = flags;
    stack_push(insanity_ui->flag_stack, &flags_t);
}

Insanity_UI_Property_Flags insanity_ui_get_flags()
{
    return *(Insanity_UI_Property_Flags*)stack_peek(insanity_ui->flag_stack);
}

void insanity_ui_push_pos(vec2 pos)
{
    if (pos.x < 0 || pos.x > 1 || pos.y < 0 || pos.y > 1)
    {
        WARN("insanity_ui_push_pos: value greater than or less than 1 passed in for the pos")
    }
    stack_push(insanity_ui->pos_stack, &pos);
}

void insanity_ui_push_size(vec2 size)
{
    if (size.x < 0 || size.x > 1 || size.y < 0 || size.y > 1)
    {
        WARN("insanity_ui_push_size: value greater than or less than 1 passed in for the size")
    }
    stack_push(insanity_ui->size_stack, &size);
}


void insanity_ui_push_padding(vec2 padding)
{
    stack_push(insanity_ui->padding_stack, &padding);
}

void insanity_ui_push_layout(Insanity_UI_Layout layout)
{
    stack_push(insanity_ui->layout_stack, &layout);
}

void insanity_ui_pop_layout(void)
{
    stack_pop(insanity_ui->layout_stack);
}

void insanity_ui_push_text(String text)
{
    insanity_ui->string_stack = text;
}

char* insanity_ui_float_to_char(const float value)
{
    int len = snprintf(NULL, 0, "%.3f", value);
    char* result = allocator_alloc(insanity_ui->frame_arena, len + 1);
    snprintf(result, len + 1, "%.3f", value);

    return result;
}

void insanity_ui_push_text_float(float val)
{
    stack_push(insanity_ui->float_stack, &val);
    char* float_char = insanity_ui_float_to_char(val);
    String float_string = {float_char, strlen(float_char)};
    insanity_ui_push_text(float_string);
}

void insanity_ui_push_image(const char* texture_file)
{
    insanity_ui->image_stack = texture_system_load_texture_new(insanity_ui->resource_system->texture_system,
                                                               texture_file);
}

Texture_Handle insanity_ui_get_image(void)
{
    return insanity_ui->image_stack;
}

Insanity_UI_Node* insanity_ui_get_new_node()
{
    Insanity_UI_Node* out_node = &insanity_ui->ui_nodes->data[insanity_ui->ui_nodes->num_items];

    if (insanity_ui->ui_stack_count > 0)
    {
        //whatever is at the top of the stack if the parent node
        Insanity_UI_Node* parent_node = insanity_ui->ui_stack[insanity_ui->ui_stack_count - 1];

        //set the new nodes parent
        out_node->parent = parent_node;
        //set the children for the parent node
        parent_node->children[parent_node->child_count] = out_node;
        parent_node->child_count += 1;
    }

    insanity_ui->ui_nodes->num_items++;

    return out_node;
}


Insanity_UI_Node* insanity_ui_get_parent_node()
{
    //get the parent
    //we point to an empty node, so we go back one, and then grab that nodes parent
    Insanity_UI_Node* parent_node = insanity_ui->ui_nodes->data[insanity_ui->ui_nodes->num_items - 1].parent;
    if (parent_node)
    {
        return parent_node;
    }

    //fallback
    return &insanity_ui->ui_nodes->data[insanity_ui->ui_nodes->num_items - 1];
}

Insanity_UI_Node* insanity_ui_get_top_node()
{
    return &insanity_ui->ui_nodes->data[insanity_ui->ui_nodes->num_items - 1];
}

bool insanity_rect_hit(vec2 pos, vec2 size)
{
    //check if we are inside a ui_object
    //we are using the screen coordinates from the mouse,
    //and hopefully the passed in pos and size

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


Insanity_UI_Interaction_Result insanity_ui_draw_rect(const char* id)
{
    Insanity_UI_Interaction_Result out_result = {0};

    Insanity_UI_Node* node = insanity_ui_get_new_node();
    node->id = id;
    node->ui_flags = insanity_ui_get_flags();

    node->pos = *(vec2*)stack_peek(insanity_ui->pos_stack);
    node->size = *(vec2*)stack_peek(insanity_ui->size_stack);

    node->layout = *(Insanity_UI_Layout*)stack_peek(insanity_ui->layout_stack);
    node->padding = *(vec2*)stack_peek(insanity_ui->padding_stack);


    node->color = insanity_ui->editor_style.color;


    if (node->ui_flags & UI_FLAG_TEXT)
    {
        node->text = insanity_ui->string_stack;
        // proper screen pos and size
        f32 font_scalar = ((insanity_ui->editor_font_size) / insanity_ui->default_font_size);

        Madness_Font font_data;
        texture_system_get_font(insanity_ui->resource_system->texture_system, insanity_ui->default_font_handle,
                                &font_data);
        node->text_total_width;
        node->text_max_height;
        for (u64 i = 0; i < node->text.length; i++)
        {
            const char c = node->text.chars[i];
            if (c < 32 || c >= 128) continue; // skip unsupported characters
            Glyph* g = &font_data.glyphs[c - 32];
            node->text_total_width += (g->advance) * font_scalar;
            node->text_max_height = max_f(node->text_max_height, g->advance * font_scalar);
        }
    }


    if (node->ui_flags & UI_FLAG_OUTLINE)
    {
        node->outline_color = insanity_ui->editor_style.outline_color;
        node->outline_thickness = insanity_ui->outline_thickness_stack;
    }
    if (node->ui_flags & UI_FLAG_ROUND_CORNER)
    {
        node->rounded_radius = insanity_ui->rounded_radius_stack;
    }
    //handle drag state
    if (node->ui_flags & UI_FLAG_DRAGGABLE)
    {
        vec2 pos = {0};
        if (hash_table_get(insanity_ui->drag_state, node->id, &pos))
        {
            node->pos = pos;
        }
        else
        {
            hash_table_insert(insanity_ui->drag_state, node->id, &node->pos);
        }
    }
    if (node->ui_flags & UI_FLAG_SCROLL_FLOAT)
    {
        bool stack_result = stack_is_empty(insanity_ui->float_stack);
        if (stack_result)
        {
            FATAL("PUSH A FLOAT ONTO THE STACK, NODE ID: %s", node->id);
            MASSERT(stack_result);
        }

        float val = {0};
        if (hash_table_get(insanity_ui->float_state, node->id, &val))
        {
            out_result.float_value = val;
            insanity_ui_push_text_float(val);
        }
        else
        {
            hash_table_insert(insanity_ui->float_state, node->id, stack_peek(insanity_ui->float_stack));
        }
    }
    if (node->ui_flags & UI_FLAG_TEXT_INPUT)
    {
        //if text is set with this, we probably just want to let that be for the first iteration and ignore every other time
        String_Builder* string_builder = {0};
        if (hash_table_get(insanity_ui->text_box_state, node->id, &string_builder))
        {
            insanity_ui_push_text(string_builder_to_string_non_pointer(string_builder));
        }
        else
        {
            string_builder = string_builder_create(100);
            string_builder_append_string(string_builder, &insanity_ui->string_stack);
            hash_table_insert(insanity_ui->text_box_state, node->id, &string_builder);
        }
    }

    if (node->ui_flags & UI_FLAG_IMAGE)
    {
        node->texture_handle = insanity_ui_get_image();
        node->uv_size = (vec2){1., 1.};
    }

    //TODO: get this from a hash table
    return out_result;
}

void insanity_ui_text()
{
    // proper screen pos and size
    vec2 base_position = *(vec2*)stack_peek(insanity_ui->pos_stack);
    Insanity_UI_Property_Flags flags = insanity_ui_get_flags();
    String text = insanity_ui->string_stack;

    f32 font_scalar = ((insanity_ui->editor_font_size) / insanity_ui->default_font_size);

    Madness_Font font_data;
    texture_system_get_font(insanity_ui->resource_system->texture_system, insanity_ui->default_font_handle,
                            &font_data);

    base_position = vec2_mul(base_position, insanity_ui->screen_size);

    for (u64 i = 0; i < text.length; i++)
    {
        const char c = text.chars[i];

        if (c < 32 || c >= 128) continue; // skip unsupported characters


        Glyph* g = &font_data.glyphs[c - 32];

        // Quad position in screen coords and scaled by the font scalar
        f32 x_position = base_position.x + ((float)g->xoff * font_scalar);
        f32 y_position = base_position.y + ((float)g->yoff * font_scalar);

        f32 x_width = ((f32)g->width * font_scalar);
        f32 y_height = ((f32)g->height * font_scalar);

        //printf("xpos %f, ypos%f, w%f, h%f\n", xpos, ypos, w, h);


        // UVs from the atlas
        // vec2 uv0 = {g->u0, g->v0}; // uv pos/offset
        // vec2 uv1 = {g->u1, g->v1}; // uv size

        Insanity_UI_Node* text_node = insanity_ui_get_new_node();
        text_node->ui_flags = flags;
        // text_node->character = c;
        text_node->pos = vec2_div((vec2){x_position, y_position}, insanity_ui->screen_size);
        text_node->size = vec2_div((vec2){x_width, y_height}, insanity_ui->screen_size);
        // text_node->pos = vec2_div((vec2){x_position, y_position}, insanity_ui->screen_size);
        // text_node->size = vec2_div((vec2){x_width, y_height}, insanity_ui->screen_size);
        text_node->uv_offset = (vec2){g->u0, g->v0};
        text_node->uv_size = (vec2){g->u1 - g->u0, g->v1 - g->v0};
        text_node->color = COLOR_WHITE;
        text_node->texture_handle = insanity_ui->default_font_handle;


        base_position.x += (g->advance) * font_scalar; // move offset forward
    }
}

void insanity_ui_push_parent(const char* id)
{
    //draw the item like normal then add it to the stack
    insanity_ui_draw_rect(id);

    //get the node we just drew
    Insanity_UI_Node* new_parent = &insanity_ui->ui_nodes->data[insanity_ui->ui_nodes->num_items - 1];

    insanity_ui->ui_stack[insanity_ui->ui_stack_count] = new_parent;
    insanity_ui->ui_stack_count++;
}


void insanity_ui_pop_parent(void)
{
    if (insanity_ui->ui_stack_count == 0)
    {
        FATAL("insanity_ui_parent_pop: trying to pop from parent node")
    }
    insanity_ui->ui_stack_count--;
}

static float x_size = 1;


static uint8_t _UI_LATCH;
#define insanity_ui_draw_parent(id) \
for(_UI_LATCH = (insanity_ui_push_parent(id), 0);\
_UI_LATCH <= 0; \
_UI_LATCH = 1, insanity_ui_pop_parent())

#define insanity_ui_draw(id) \
    insanity_ui_draw_rect(id)


void insanity_ui_test()
{
    MASSERT(insanity_ui);
    x_size += 0.1f * 0.01; // emulating delta time

    // {
    //     insanity_ui_push_sizing_xy(Insanity_UI_SIZING_PERCENT);
    //     insanity_ui_push_flags(UI_FLAG_DRAGGABLE);
    //     insanity_ui_push_layout(Insanity_UI_LAYOUT_VERTICAL);
    //     insanity_ui_push_pos((vec2){0, 0});
    //     insanity_ui_push_size((vec2){0.5 /**( cos(x_size) +1)*/ , 0.5 /** ( cos(x_size) +1)*/});
    //     insanity_ui_draw_parent("parent test")
    //     {
    //         insanity_ui_push_flags(UI_FLAG_IMAGE);
    //         insanity_ui_push_sizing_xy(Insanity_UI_SIZING_PERCENT);
    //         insanity_ui_push_size((vec2){0.5, 0.5});
    //         insanity_ui_draw("child1");
    //         insanity_ui_push_sizing_xy(Insanity_UI_SIZING_PERCENT);
    //         insanity_ui_push_size((vec2){0.1, 0.1});
    //         insanity_ui_push_flags(UI_FLAG_TEXT|  UI_FLAG_CLICKABLE | UI_FLAG_IMAGE);
    //         insanity_ui_draw("child8");
    //         insanity_ui_push_flags(UI_FLAG_CLICKABLE);
    //         insanity_ui_push_sizing_xy(Insanity_UI_SIZING_EXPAND);
    //         insanity_ui_draw("child2");
    //         insanity_ui_draw_parent("child3")
    //         {
    //             insanity_ui_push_flags(UI_FLAG_IMAGE);
    //             insanity_ui_draw("child4");
    //             insanity_ui_draw_parent("child5")
    //             {
    //                 insanity_ui_push_flags(UI_FLAG_CLICKABLE);
    //                 insanity_ui_push_sizing_xy(Insanity_UI_SIZING_PERCENT);
    //                 insanity_ui_draw("child6");
    //                 insanity_ui_draw("child7");
    //             }
    //         }
    //     }
    // }

    //game ui recreaction

    insanity_ui_push_pos((vec2){.03, .1});
    insanity_ui_push_size((vec2){.2, .05});
    insanity_ui_draw("turn phase");

    insanity_ui_push_flags(UI_FLAG_SCROLL_VIEW);
    insanity_ui_push_pos((vec2){.1, .2});
    insanity_ui_push_size((vec2){.2, .5});
    insanity_ui_draw_parent("abilities list")
    {
        for (u64 i = 0; i < 5; i++)
        {
            insanity_ui_push_flags(UI_FLAG_CLICKABLE | UI_FLAG_ROUND_CORNER);
            // insanity_ui_push_layout(Insanity_UI_LAYOUT_HORIZONTAL);
            insanity_ui_push_layout(Insanity_UI_LAYOUT_VERTICAL);
            insanity_ui_push_size((vec2){1., .2});
            insanity_ui_draw_parent("ability")
            {
                insanity_ui_push_flags(UI_FLAG_IMAGE);
                insanity_ui_push_size((vec2){.2, 1.});
                insanity_ui_draw("icon");
                insanity_ui_push_flags(UI_FLAG_IMAGE | UI_FLAG_CLICKABLE);
                insanity_ui_draw("text");
            }
        }
    }
}
