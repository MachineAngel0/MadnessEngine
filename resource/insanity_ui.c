#include "insanity_ui.h"

#include "math_lib.h"
#include "sprite_system.h"


static Insanity_UI* insanity_ui;


bool insanity_ui_init(Memory_System* memory_system, Input_System* input_system,
                      Resource_System* resource_system)
{
    insanity_ui = memory_system_alloc(memory_system, sizeof(Insanity_UI), MEMORY_SUBSYSTEM_UI);
    MASSERT(insanity_ui);

    u64 ui_arena_mem_size = MB(128);
    u64 ui_frame_arena_mem_size = MB(128);
    insanity_ui->mem_tracker = memory_system_get_memory_tracker(memory_system->memory_tracker_system,
                                                                STRING("MADNESS UI"),
                                                                ui_arena_mem_size + ui_frame_arena_mem_size);

    insanity_ui->arena = memory_system_alloc(memory_system, sizeof(Arena), MEMORY_SUBSYSTEM_UI);
    insanity_ui->frame_arena = memory_system_alloc(memory_system, sizeof(Arena), MEMORY_SUBSYSTEM_UI);

    void* arena_memory = memory_system_alloc(memory_system, ui_arena_mem_size, MEMORY_SUBSYSTEM_UI);
    void* frame_arena_memory = memory_system_alloc(memory_system, ui_frame_arena_mem_size, MEMORY_SUBSYSTEM_UI);

    arena_init(insanity_ui->arena, arena_memory, ui_arena_mem_size, insanity_ui->mem_tracker);
    arena_init(insanity_ui->frame_arena, frame_arena_memory, ui_arena_mem_size, insanity_ui->mem_tracker);

    insanity_ui->input_system_reference = input_system;
    insanity_ui->resource_system = resource_system;


    insanity_ui->ui_data = Sprite_Data_array_create(INSANITY_MAX_UI_NODE_COUNT);
    insanity_ui->text_data = Sprite_Data_array_create(INSANITY_MAX_UI_TEXT_NODE_COUNT);

    insanity_ui->default_font_size = INSANITY_DEFAULT_FONT_SIZE;
    insanity_ui->editor_font_size = INSANITY_EDITOR_FONT_SIZE;

    insanity_ui->ui_nodes = Insanity_UI_Node_array_create(MAX_INSANITY_UI_NODE_COUNT);
    insanity_ui->ui_nodes_text = Insanity_UI_Node_Text_array_create(MAX_INSANITY_UI_NODE_COUNT);
    insanity_ui->string_builder = string_builder_create(100);

    //stacks
    insanity_ui->flag_stack = stack_create(sizeof(Insanity_UI_Property_Flags), 100, insanity_ui->arena);
    insanity_ui->pos_stack = stack_create(sizeof(vec2), 100, insanity_ui->arena);
    // insanity_ui->sizing_stack = stack_create(sizeof(Insanity_UI_Property_Flags), 100, insanity_ui->arena);
    // insanity_ui->style_stack = stack_create(sizeof(Insanity_UI_Property_Flags), 100, insanity_ui->arena);

    insanity_ui->drag_state = HASH_TABLE_CREATE_DEFAULT_SIZE(vec2);


    insanity_ui->active = -1;
    insanity_ui->hot = -1;


    insanity_ui->mouse_pos_x = -1.0f;
    insanity_ui->mouse_pos_y = -1.0f;

    insanity_ui->mouse_down = 0;
    insanity_ui->mouse_released_unique = 0;

    //TODO: replace with an in param
    insanity_ui->screen_size = (vec2){800.0f, 600.0f};


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

    //std::cout << "MOUSE STATE:" << Insanity_UI.mouse_down << '\n';
    insanity_ui->editor_style = (Insanity_UI_Editor_Style){
        .layout_color = COLOR_PURPLE_PALETTE_DARK, .layout_accent_color = COLOR_PURPLE_PALETTE_PURPLE,
        .text_color = COLOR_PURPLE_PALETTE_LIGHT, .textbox_color = COLOR_PURPLE_PALETTE_DARK2,
        .custom_widget_color = COLOR_PURPLE_PALETTE_PURPLE_LIGHT,
        .color = COLOR_PURPLE_PALETTE_PURPLE_STRONG, .hovered_color = COLOR_PURPLE_PALETTE_PURPLE_LIGHT2,
        .pressed_color = COLOR_PURPLE_PALETTE_DARK2,
        .outline_color = COLOR_PURPLE_PALETTE_PURPLE_LIGHT
    };

    insanity_ui->screen_size.x = screen_size_x;
    insanity_ui->screen_size.y = screen_size_y;


    Sprite_Data_array_clear(insanity_ui->ui_data);
    Sprite_Data_array_clear(insanity_ui->text_data);


    Insanity_UI_Node_array_zero(insanity_ui->ui_nodes);
    Insanity_UI_Node_array_clear(insanity_ui->ui_nodes);

    Insanity_UI_Node_Text_array_zero(insanity_ui->ui_nodes_text);
    Insanity_UI_Node_Text_array_clear(insanity_ui->ui_nodes_text);


    stack_clear(insanity_ui->flag_stack);
    Insanity_UI_Property_Flags no_flag = 0;
    stack_push(insanity_ui->flag_stack, &no_flag);

    stack_clear(insanity_ui->pos_stack);
    vec2 default_pos = {0, 0};
    stack_push(insanity_ui->pos_stack, &default_pos);

    insanity_ui->string_stack = STRING("INVALID STRING");
    insanity_ui->image_stack = texture_system_get_default_texture(insanity_ui->resource_system->texture_system);


    insanity_ui->hot = -1;

    insanity_ui->mouse_down = input_is_mouse_button_pressed(insanity_ui->input_system_reference, MOUSE_BUTTON_LEFT);
    insanity_ui->mouse_released_unique = input_is_mouse_button_released_unique(
        insanity_ui->input_system_reference, MOUSE_BUTTON_LEFT);
    //this can be 0 if invalid
    insanity_ui->released_key = input_get_first_released_key(insanity_ui->input_system_reference);
}

void insanity_ui_generate_draw(Resource_System* resource_system)
{
    //Generate Draw Data for UI Sprites
    for (u32 i = 0; i < insanity_ui->ui_nodes->num_items; i++)
    {
        Insanity_UI_Node* node_to_draw = &insanity_ui->ui_nodes->data[i];
        Sprite_Data* sprite_data = sprite_system_get_ui_sprite(resource_system->sprite_system);
        sprite_data->pos = vec2_div(node_to_draw->pos, insanity_ui->screen_size);
        sprite_data->size = vec2_div(node_to_draw->size, insanity_ui->screen_size);
        sprite_data->thickness = 1.0;
        sprite_data->rotation = deg_to_rad(node_to_draw->rotation);


        if (node_to_draw->sprite_flags & SPRITE_FLAG_CIRCLE)
        {
            sprite_data->thickness = node_to_draw->thickness;
        }


        //TODO:
        // if (node_to_draw->flags & SPRITE_PIPELINE_COLOR)
        // {sprite_data->color = node_to_draw->color;}

        // if (node_to_draw->flags & SPRITE_PIPELINE_TEXTURE)
        // {
        // sprite_data->texture_index = node_to_draw->texture_handle.handle;
        // }
        sprite_data->color = node_to_draw->color;
        sprite_data->texture_index = node_to_draw->texture_handle.handle;
        sprite_data->flags = node_to_draw->sprite_flags;


        //if (node_to_draw->flags & SPRITE_PIPELINE_TEXT)
        //{
        //    sprite_data->uv_offset = node_to_draw->uv_offset;
        //    sprite_data->uv_size = node_to_draw->uv_size;
        //   Sprite_Data_array_push(madness_ui->text_data, sprite_data);
        //}
    }
    for (u32 i = 0; i < insanity_ui->ui_nodes_text->num_items; i++)
    {
        Insanity_UI_Node_Text* node_to_draw = &insanity_ui->ui_nodes_text->data[i];
        Sprite_Data* sprite_data = sprite_system_get_text_sprite(resource_system->sprite_system);
        sprite_data->pos = vec2_div(node_to_draw->pos, insanity_ui->screen_size);
        sprite_data->size = vec2_div(node_to_draw->size, insanity_ui->screen_size);

        sprite_data->uv_offset = node_to_draw->uv_offset;
        sprite_data->uv_size = node_to_draw->uv_size;

        sprite_data->color = node_to_draw->color;
        sprite_data->texture_index = node_to_draw->texture_handle.handle;

        sprite_data->flags = node_to_draw->flags;
    }
}


void insanity_ui_end(Resource_System* resource_system)
{
    MASSERT(insanity_ui);


    insanity_ui_passes();

    insanity_ui_generate_draw(resource_system);

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

Insanity_UI_Render_Data insanity_get_render_data()
{
    return (Insanity_UI_Render_Data){.ui_nodes = insanity_ui->ui_nodes, .ui_nodes_text = insanity_ui->ui_nodes_text};

}


void insanity_ui_passes()
{
    for (u32 i = 0; i < insanity_ui->ui_nodes->num_items; i++)
    {
        Insanity_UI_Node* node = &insanity_ui->ui_nodes->data[i];

        bool hit = (insanity_rect_hit(node->pos, node->size));

        if (hit)
        {
            if (node->ui_flags & UI_TYPE_CLICKABLE)
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


        if (node->ui_flags & UI_TYPE_DRAGGABLE)
        {
            if (hit)
            {
                if (insanity_ui->mouse_down)
                {
                    vec2 pos;
                    hash_table_get(insanity_ui->drag_state, node->debug_id, &pos);
                    // node->pos.x += insanity_ui->mouse_delta_x;
                    // node->pos.y += insanity_ui->mouse_delta_y;
                    pos.x += insanity_ui->mouse_delta_x;
                    pos.y += insanity_ui->mouse_delta_y;
                    hash_table_set(insanity_ui->drag_state, node->debug_id, &pos);
                }
            }
        }
    }
}


void insanity_ui_set_flags(Insanity_UI_Property_Flags flags)
{
    Insanity_UI_Property_Flags flags_t = flags;
    stack_push(insanity_ui->flag_stack, &flags_t);
}

Insanity_UI_Property_Flags insanity_ui_get_flags()
{
    return *(Insanity_UI_Property_Flags*)stack_peek(insanity_ui->flag_stack);
}

void insanity_ui_set_pos(vec2 pos)
{
    stack_push(insanity_ui->pos_stack, &pos);
}

void insanity_ui_set_text(String text)
{
    insanity_ui->string_stack = text;
}

void insanity_ui_set_image(const char* texture_file)
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


    //set the parent is a layout is active
    if (insanity_ui->layout)
    {
        //check if the last node had a parent, if it doesn't then it is the parent
        if (insanity_ui->ui_nodes->data[insanity_ui->ui_nodes->num_items - 1].parent)
        {
            out_node->parent = insanity_ui->ui_nodes->data[insanity_ui->ui_nodes->num_items - 1].parent;
        }
        else
        {
            out_node->parent = &insanity_ui->ui_nodes->data[insanity_ui->ui_nodes->num_items - 1];
        }

        out_node->parent->children[out_node->parent->child_node_count++] = out_node;
    }

    insanity_ui->ui_nodes->num_items++;
    return out_node;
}

Insanity_UI_Node_Text* insanity_ui_get_new_node_text()
{
    Insanity_UI_Node_Text* out_node = &insanity_ui->ui_nodes_text->data[insanity_ui->ui_nodes_text->num_items];
    insanity_ui->ui_nodes_text->num_items++;
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


void insanity_ui_draw_rect(const char* id)
{
    Insanity_UI_Node* node = insanity_ui_get_new_node();
    node->debug_id = id;
    node->ui_flags = insanity_ui_get_flags();
    node->pos = *(vec2*)stack_peek(insanity_ui->pos_stack);
    node->size = (vec2){200, 100};
    node->color = insanity_ui->editor_style.color;


    if ((node->ui_flags & UI_TYPE_OUTLINE))
    {
        node->outline_color = insanity_ui->editor_style.outline_color;
        node->outline_thickness = 0.01; // TODO:
    }

    //handle drag state
    if ((node->ui_flags & UI_TYPE_DRAGGABLE))
    {
        vec2 pos;
        if (hash_table_get(insanity_ui->drag_state, node->debug_id, &pos))
        {
            node->pos = pos;
        }
        {
            hash_table_insert(insanity_ui->drag_state, node->debug_id, &node->pos);
        }
    }
    if (node->ui_flags & UI_TYPE_TEXT)
    {
        //create the text
        insanity_ui_text();
    }
    if (node->ui_flags & UI_TYPE_IMAGE)
    {
        node->texture_handle = insanity_ui_get_image();
    }


    //close
    if (insanity_ui->layout)
    {
        //add child sizing to the parent
    }


    // return
}

void insanity_ui_text()
{
    // proper screen pos and size
    vec2 screen_position = *(vec2*)stack_peek(insanity_ui->pos_stack);
    String text = insanity_ui->string_stack;

    f32 font_scalar = ((insanity_ui->editor_font_size) / insanity_ui->default_font_size);

    Madness_Font font_data;
    texture_system_get_font(insanity_ui->resource_system->texture_system, insanity_ui->default_font_handle,
                            &font_data);

    for (u64 i = 0; i < text.length; i++)
    {
        const char c = text.chars[i];

        if (c < 32 || c >= 128) continue; // skip unsupported characters


        Glyph* g = &font_data.glyphs[c - 32];

        // Quad position in screen coords and scaled by the font scalar
        f32 x_position = screen_position.x + ((float)g->xoff * font_scalar);
        f32 y_position = screen_position.y + ((float)g->yoff * font_scalar);

        f32 x_width = ((f32)g->width * font_scalar);
        f32 y_height = ((f32)g->height * font_scalar);

        //printf("xpos %f, ypos%f, w%f, h%f\n", xpos, ypos, w, h);


        // UVs from the atlas
        // vec2 uv0 = {g->u0, g->v0}; // uv pos/offset
        // vec2 uv1 = {g->u1, g->v1}; // uv size

        Insanity_UI_Node_Text* text_node = insanity_ui_get_new_node_text();
        text_node->character = c;
        text_node->pos = (vec2){x_position, y_position};
        text_node->size = (vec2){x_width, y_height};
        text_node->uv_offset = (vec2){g->u0, g->v0};
        text_node->uv_size = (vec2){g->u1 - g->u0, g->v1 - g->v0};
        text_node->color = COLOR_WHITE;
        text_node->texture_handle = insanity_ui->default_font_handle;

        if (i == 0)
        {
            text_node->start_text = true;
        }

        screen_position.x += (g->advance) * font_scalar; // move offset forward
    }
}

void insanity_ui_layout_start(Insanity_UI_Layout_Direction layout_direction)
{
}

void insanity_ui_layout_end(Insanity_UI_Layout_Direction layout_direction)
{
}


void insanity_ui_test()
{
    MASSERT(insanity_ui);

    {
        insanity_ui_set_pos((vec2){200, 200});
        insanity_ui_set_flags(UI_TYPE_CLICKABLE | UI_TYPE_DRAGGABLE);

        insanity_ui_draw_rect("id");
    }

    {
        insanity_ui_set_flags(UI_TYPE_CLICKABLE | UI_TYPE_TEXT);
        insanity_ui_set_text(STRING("Bitch"));
        insanity_ui_set_pos((vec2){500, 200});

        insanity_ui_draw_rect("id2");
    }


    //check box
    insanity_ui_set_flags(UI_TYPE_CLICKABLE | UI_TYPE_OUTLINE);
    insanity_ui_set_pos((vec2){200, 500});
    insanity_ui_draw_rect("id3");
}
