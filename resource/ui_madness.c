#include "../resource/ui_madness.h"

#include "hash_table.h"
#include "logger.h"
#include "str.h"

#define MAX_UI_SPRITE_COUNT 1000
#define MAX_TEXT_SPRITE_COUNT 1000
#define MAX_BUTTON_COUNT 1000


Madness_UI* madness_ui_init(Memory_System* memory_system, Input_System* input_system,
                            Resource_System* resource_system)
{
    Madness_UI* madness_ui = memory_system_alloc(memory_system, sizeof(Madness_UI), MEMORY_SUBSYSTEM_UI);

    u64 ui_arena_mem_size = MB(128);
    u64 ui_frame_arena_mem_size = MB(128);
    madness_ui->mem_tracker = memory_system_get_memory_tracker(memory_system->memory_tracker_system,
                                                               STRING("MADNESS UI"),
                                                               ui_arena_mem_size + ui_frame_arena_mem_size);

    madness_ui->allocator = memory_system_alloc(memory_system, sizeof(Allocator), MEMORY_SUBSYSTEM_UI);
    madness_ui->frame_arena = memory_system_alloc(memory_system, sizeof(Allocator), MEMORY_SUBSYSTEM_UI);

    void* arena_memory = memory_system_alloc(memory_system, ui_arena_mem_size, MEMORY_SUBSYSTEM_UI);
    void* frame_arena_memory = memory_system_alloc(memory_system, ui_frame_arena_mem_size, MEMORY_SUBSYSTEM_UI);

    allocator_init(madness_ui->allocator, arena_memory, ui_arena_mem_size);
    allocator_init(madness_ui->frame_arena, frame_arena_memory, ui_arena_mem_size);

    madness_ui->input_system_reference = input_system;
    madness_ui->resource_system = resource_system;


    madness_ui->ui_data = Sprite_Data_array_create(
        MAX_UI_SPRITE_COUNT, allocator_inferface_create(madness_ui->allocator));
    madness_ui->text_data = Sprite_Data_array_create(
        MAX_TEXT_SPRITE_COUNT, allocator_inferface_create(madness_ui->allocator));

    madness_ui->default_font_size = DEFAULT_FONT_CREATION_SIZE;
    madness_ui->editor_font_size = EDITOR_FONT_SIZE;

    madness_ui->ui_nodes = UI_Node_array_create(MAX_UI_NODE_COUNT, allocator_inferface_create(madness_ui->allocator));
    madness_ui->ui_nodes_text = UI_Node_Text_array_create(
        MAX_UI_TEXT_NODE_COUNT, allocator_inferface_create(madness_ui->allocator));
    madness_ui->string_builder = string_builder_create(100);


    //ui button state is the u32
    madness_ui->button_hash_states = HASH_TABLE_CREATE(u32, MAX_BUTTON_COUNT);


    madness_ui->window_state = hash_table_string_create(sizeof(Window_State),
                                                        10,
                                                        allocator_inferface_create(madness_ui->allocator),
                                                        false);


    madness_ui->cursor_pos = vec2_zero();


    // for (u64 ui_node_index = 0; ui_node_index < MAX_UI_NODE_COUNT; ui_node_index++)
    // {
    //     UI_Node* node = arena_alloc(Madness_UI->arena, sizeof(UI_Node));
    //     Madness_UI->ui_stack[ui_node_index] = node;
    //     Madness_UI->ui_nodes_array[ui_node_index] = node;
    // }

    madness_ui->active = -1;
    madness_ui->hot = -1;


    madness_ui->mouse_pos_x = -1.0f;
    madness_ui->mouse_pos_y = -1.0f;

    madness_ui->mouse_down = 0;
    madness_ui->mouse_released_unique = 0;

    //TODO: replace with an in param
    madness_ui->screen_size = (vec2){800.0f, 600.0f};

    madness_ui->element_padding_x = 5.0f;
    madness_ui->element_padding_y = 5.0f;

    madness_ui->font_padding_x = 8.0f;
    madness_ui->font_padding_y = 8.0f;


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


    INFO("UI SYSTEM CREATED");
    return madness_ui;
}

bool madness_ui_shutdown(Madness_UI* madness_ui)
{
    return true;
}

void madness_ui_begin(Madness_UI* madness_ui, i32 screen_size_x, i32 screen_size_y)
{
    //clear draw info and reset the hot id
    allocator_clear(madness_ui->frame_arena);

    //std::cout << "MOUSE STATE:" << Madness_UI.mouse_down << '\n';
    madness_ui->editor_style = (UI_Editor_Style){
        .layout_color = COLOR_PURPLE_PALETTE_DARK, .layout_accent_color = COLOR_PURPLE_PALETTE_PURPLE,
        .text_color = COLOR_PURPLE_PALETTE_LIGHT, .textbox_color = COLOR_PURPLE_PALETTE_DARK2,
        .custom_widget_color = COLOR_PURPLE_PALETTE_PURPLE_LIGHT,
        .color = COLOR_PURPLE_PALETTE_PURPLE_STRONG, .hovered_color = COLOR_PURPLE_PALETTE_PURPLE_LIGHT2,
        .pressed_color = COLOR_PURPLE_PALETTE_DARK2,
        .outline_color = COLOR_PURPLE_PALETTE_PURPLE_LIGHT
    };

    madness_ui->screen_size.x = screen_size_x;
    madness_ui->screen_size.y = screen_size_y;


    Sprite_Data_array_clear(madness_ui->ui_data);
    Sprite_Data_array_clear(madness_ui->text_data);


    UI_Node_array_zero(madness_ui->ui_nodes);
    UI_Node_array_clear(madness_ui->ui_nodes);

    UI_Node_Text_array_zero(madness_ui->ui_nodes_text);
    UI_Node_Text_array_clear(madness_ui->ui_nodes_text);


    madness_ui->cursor_pos = vec2_zero();
    madness_ui->prev_item_size = vec2_zero();

    madness_ui->current_layout_pos = vec2_zero();
    madness_ui->current_layout_size = vec2_zero();
    madness_ui->current_layout_screen_pos = vec2_zero();
    madness_ui->current_layout_screen_size = vec2_zero();


    madness_ui->hot = -1;

    madness_ui->mouse_down = input_is_mouse_button_pressed(madness_ui->input_system_reference, MOUSE_BUTTON_LEFT);
    madness_ui->mouse_released_unique = input_is_mouse_button_released_unique(
        madness_ui->input_system_reference, MOUSE_BUTTON_LEFT);
    //this can be 0 if invalid
    madness_ui->released_key = input_get_first_released_key(madness_ui->input_system_reference);
}


void madness_ui_end(Madness_UI* madness_ui)
{
    //Generate Draw Data
    madness_ui->ui_draw_data = allocator_alloc(madness_ui->frame_arena,
                                               madness_ui->ui_nodes->num_items * sizeof(UI_Node_Draw_Data));
    madness_ui->ui_draw_data_count = madness_ui->ui_nodes->num_items;
    madness_ui->text_draw_data = allocator_alloc(madness_ui->frame_arena,
                                                 madness_ui->ui_nodes_text->num_items * sizeof(UI_Node_Draw_Data));
    madness_ui->text_draw_data_count = madness_ui->ui_nodes_text->num_items;
    for (u32 i = 0; i < madness_ui->ui_nodes->num_items; i++)
    {
        UI_Node* node_data = &madness_ui->ui_nodes->data[i];
        UI_Node_Draw_Data* draw_data = &madness_ui->ui_draw_data[i];

        draw_data->ui_flags = node_data->flags;
        draw_data->pos = vec2_div(node_data->pos, madness_ui->screen_size);
        draw_data->size = vec2_div(node_data->size, madness_ui->screen_size);
        draw_data->rotation = deg_to_rad(node_data->rotation);

        draw_data->thickness = node_data->thickness;

        draw_data->outline_thickness = node_data->outline_thickness;

        draw_data->color = node_data->color;

        draw_data->texture_handle = node_data->texture_handle.handle;
        draw_data->uv_offset = node_data->uv_offset;
        draw_data->uv_size = node_data->uv_size;

        // draw_data->rounded_radius = node_data->rounded_radius;
        // draw_data->outline_color = node_data->outline_color;
        // draw_data->outline_thickness = node_data->outline_thickness;
        // draw_data->background_color = node_data->background_color;
    }
    for (u32 i = 0; i < madness_ui->ui_nodes_text->num_items; i++)
    {
        UI_Node_Text* node_to_draw = &madness_ui->ui_nodes_text->data[i];
        UI_Node_Draw_Data* draw_data = &madness_ui->text_draw_data[i];
        draw_data->pos = vec2_div(node_to_draw->pos, madness_ui->screen_size);
        draw_data->size = vec2_div(node_to_draw->size, madness_ui->screen_size);

        draw_data->uv_offset = node_to_draw->uv_offset;
        draw_data->uv_size = node_to_draw->uv_size;

        draw_data->color = node_to_draw->color;
        draw_data->texture_handle = node_to_draw->texture_handle.handle;
        draw_data->ui_flags = node_to_draw->flags;
    }


    //SET UI STATE FOR NEXT FRAME //

    //check if mouse is released, if so reset the active id
    //also update the mouse state

    //printf("HOT ID: %d, HOT LAYER: %d\n", Madness_UI->hot.ID, Madness_UI->hot.layer);
    //printf("ACTIVE ID: %d, ACTIVE LAYER: %d\n", Madness_UI->active.ID, Madness_UI->active.layer);


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

UI_Render_Packet madness_ui_get_ui_render_data(Madness_UI* madness_ui)
{
    return (UI_Render_Packet){
        .ui_data = madness_ui->ui_draw_data,
        .ui_data_count = madness_ui->ui_draw_data_count,
        .ui_data_bytes = madness_ui->ui_draw_data_count * sizeof(UI_Node_Draw_Data),
    };
}

UI_Render_Packet madness_ui_get_text_render_data(Madness_UI* madness_ui)
{
    return (UI_Render_Packet){
        .ui_data = madness_ui->text_draw_data,
        .ui_data_count = madness_ui->text_draw_data_count,
        .ui_data_bytes = madness_ui->text_draw_data_count * sizeof(UI_Node_Draw_Data),
    };
}


void madness_ui_print_state(Madness_UI* madness_ui)
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


bool is_ui_hot(Madness_UI* madness_ui, int id)
{
    return madness_ui->hot == id;
}

bool is_ui_active(Madness_UI* madness_ui, int id)
{
    return madness_ui->active == id;
}


bool region_hit(Madness_UI* madness_ui, vec2 pos, vec2 size)
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
bool use_ui_element(Madness_UI* madness_ui, int id, vec2 pos, vec2 size)
{
    //checking if we released the mouse button, are active, and we are inside the hit region

    if (madness_ui->mouse_down == false &&
        madness_ui->active == id &&
        region_hit(madness_ui, pos, size))
        return true;

    return false;
}


void set_hot(Madness_UI* madness_ui, int id)
{
    madness_ui->hot = id;
    //printf("ID: %d, is hot\n", id);
}

void set_active(Madness_UI* madness_ui, int id)
{
    madness_ui->active = id;
}

bool can_be_active(Madness_UI* madness_ui)
{
    return madness_ui->active == -1 && madness_ui->mouse_down;
}

bool is_active(Madness_UI* madness_ui, int id)
{
    return madness_ui->active == id;
}

bool is_hot(Madness_UI* madness_ui, int id)
{
    return madness_ui->hot == id;
}

UI_Node* madness_ui_get_new_node(Madness_UI* madness_ui)
{
    UI_Node* out_node = &madness_ui->ui_nodes->data[madness_ui->ui_nodes->num_items];

    if (madness_ui->parent_node_state.is_active)
    {
        //check if the last node had a parent, if it doesn't then it is the parent
        if (madness_ui->ui_nodes->data[madness_ui->ui_nodes->num_items - 1].parent)
        {
            out_node->parent = madness_ui->ui_nodes->data[madness_ui->ui_nodes->num_items - 1].parent;
        }
        else
        {
            out_node->parent = &madness_ui->ui_nodes->data[madness_ui->ui_nodes->num_items - 1];
        }

        out_node->parent->children[out_node->parent->child_node_count++] = out_node;
    }


    madness_ui->ui_nodes->num_items++;
    return out_node;
}

UI_Node* madness_ui_get_parent_node(Madness_UI* madness_ui)
{
    //get the parent
    UI_Node* parent_node = madness_ui->ui_nodes->data[madness_ui->ui_nodes->num_items - 1].parent;
    if (parent_node)
    {
        return parent_node;
    }

    //fallback
    return &madness_ui->ui_nodes->data[madness_ui->ui_nodes->num_items - 1];
}

UI_Node_Text* madness_ui_get_new_node_text(Madness_UI* madness_ui)
{
    UI_Node_Text* out_node = &madness_ui->ui_nodes_text->data[madness_ui->ui_nodes_text->num_items];
    madness_ui->ui_nodes_text->num_items++;
    return out_node;
}

UI_Node* madness_ui_get_last_used_node(Madness_UI* madness_ui)
{
    if (madness_ui->ui_nodes->num_items > 0)
    {
        return &madness_ui->ui_nodes->data[madness_ui->ui_nodes->num_items--];
    }
    return NULL;
}


void madness_ui_same_line(Madness_UI* madness_ui)
{
    madness_ui->cursor_pos.x += madness_ui->prev_item_size.x;
    madness_ui->cursor_pos.y -= madness_ui->prev_item_size.y + madness_ui->element_padding_y;
}

void madness_ui_advance_cursor(Madness_UI* madness_ui, vec2 ui_screen_size)
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
    // madness_ui->prev_line = madness_ui->cursor_pos;
    madness_ui->cursor_pos.x = madness_ui->current_layout_screen_pos.x;
    madness_ui->cursor_pos.y += madness_ui->prev_item_size.y + madness_ui->element_padding_y;
}

void madness_ui_center_child_node(vec2 parent_pos, vec2 parent_size, vec2 child_size, vec2* out_pos)
{
    //find the remaining space vertically and horizontally,
    //dividing that by two will give us the offset needed to add to the parent object to center it
    vec2 size_diff = vec2_sub(parent_size, child_size);
    vec2 size_diff_centered = vec2_div_scalar(size_diff, 2);
    *out_pos = vec2_add(parent_pos, size_diff_centered);
}

char* madness_ui_float_to_char(Madness_UI* madness_ui, const float value)
{
    int len = snprintf(NULL, 0, "%.3f", value);
    char* result = allocator_alloc(madness_ui->frame_arena, len + 1);
    snprintf(result, len + 1, "%.3f", value);

    return result;
}

void madness_ui_menu_bar(Madness_UI* madness_ui, String id)
{
    //create the title bar
    //create the header
    vec2 header_size = {
        madness_ui->screen_size.x,
        32.f
    };

    UI_Node* title_bar_node = madness_ui_get_new_node(madness_ui);
    title_bar_node->pos = (vec2){
        0, 0
    };
    title_bar_node->size = header_size;
    title_bar_node->color = madness_ui->editor_style.textbox_color;
    title_bar_node->string_id = id;
}

bool madness_ui_menu_item(Madness_UI* madness_ui, String menu_name)
{
    vec2 text_size = {};
    madness_calculate_text_size(madness_ui, menu_name, madness_ui->cursor_pos, &text_size);
    madness_ui_text(madness_ui, menu_name);


    return region_hit(madness_ui, madness_ui->cursor_pos, text_size);
}

void madness_ui_window(Madness_UI* madness_ui, String header_name)
{
    Window_State window_state;
    if (!hash_table_string_get(madness_ui->window_state, header_name, &window_state))
    {
        window_state = (Window_State){.pos = {0.1, 0.1}, .size = {0.2, 0.8}};
        hash_table_string_insert(madness_ui->window_state, header_name, &window_state);
    }

    //normalize the position and size
    madness_ui->current_layout_pos = window_state.pos;
    madness_ui->current_layout_size = window_state.size;

    madness_ui->cursor_pos = vec2_mul(madness_ui->current_layout_pos, madness_ui->screen_size);

    madness_ui->current_layout_screen_pos = vec2_mul(madness_ui->current_layout_pos, madness_ui->screen_size);
    madness_ui->current_layout_screen_size = vec2_mul(madness_ui->current_layout_size, madness_ui->screen_size);


    //create the background
    UI_Node* background_node = madness_ui_get_new_node(madness_ui);
    background_node->pos = madness_ui->current_layout_screen_pos;
    background_node->size = madness_ui->current_layout_screen_size;
    background_node->color = madness_ui->editor_style.layout_color;
    background_node->string_id = header_name;

    //create the header
    vec2 header_size = {
        madness_ui->current_layout_screen_size.x,
        madness_ui->current_layout_screen_size.y * 0.04
    };

    UI_Node* header_node = madness_ui_get_new_node(madness_ui);
    header_node->pos = madness_ui->current_layout_screen_pos;
    header_node->size = header_size;
    header_node->color = madness_ui->editor_style.layout_accent_color;
    header_node->string_id = header_name;

    madness_ui_advance_cursor(madness_ui, header_size);

    //sizing and position of text
    vec2 text_size;
    madness_calculate_text_size(madness_ui, header_name, madness_ui->current_layout_screen_pos, &text_size);
    vec2 text_pos;
    madness_ui_center_child_node(madness_ui->current_layout_screen_pos, header_size, text_size, &text_pos);

    madness_draw_text(madness_ui, header_name,
                      (vec2){madness_ui->current_layout_screen_pos.x + madness_ui->element_padding_y, text_pos.y});


    if (region_hit(madness_ui, header_node->pos, header_size))
    {
        if (madness_ui->mouse_down)
        {
            window_state.pos.x += madness_ui->mouse_delta_x / madness_ui->screen_size.x;
            window_state.pos.y += madness_ui->mouse_delta_y / madness_ui->screen_size.y;
            hash_table_string_set(madness_ui->window_state, header_name, &window_state);
        }
    }
}


void madness_draw_quad(Madness_UI* madness_ui, String id, vec2* out_pos, vec2* out_size, UI_Node** out_node)
{
    // 0-1 range
    const float button_vertical_normalized_size = 0.03f;
    vec2 normalized_size;
    normalized_size.x = madness_ui->current_layout_size.x;
    normalized_size.y = button_vertical_normalized_size;

    // proper screen pos and size
    vec2 ui_screen_pos = madness_ui->cursor_pos;
    vec2 ui_screen_size = vec2_mul(normalized_size, madness_ui->screen_size);


    //grab a node
    UI_Node* new_node = madness_ui_get_new_node(madness_ui);
    new_node->string_id = id;
    new_node->hash_id = generate_hash_key_64bit((u8*)id.chars, id.length);
    new_node->pos = ui_screen_pos;
    new_node->size = ui_screen_size;
    new_node->color = madness_ui->editor_style.color;

    *out_pos = ui_screen_pos;
    *out_size = ui_screen_size;
    *out_node = new_node;
}

void madness_draw_text(Madness_UI* madness_ui, String text, vec2 screen_position)
{
    f32 font_scalar = ((madness_ui->editor_font_size) / madness_ui->default_font_size);

    Madness_Font font_data;
    texture_system_get_font(madness_ui->resource_system->texture_system, madness_ui->default_font_handle,
                            &font_data);
    vec2 text_size = {};

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

        UI_Node_Text* text_node = madness_ui_get_new_node_text(madness_ui);
        text_node->character = c;
        text_node->pos = (vec2){x_position, y_position};
        text_node->size = (vec2){x_width, y_height};
        text_node->uv_offset = (vec2){g->u0, g->v0};
        text_node->uv_size = (vec2){g->u1 - g->u0, g->v1 - g->v0};
        text_node->color = COLOR_WHITE;
        text_node->texture_handle = madness_ui->default_font_handle;

        if (i == 0)
        {
            text_node->start_text = true;
        }

        screen_position.x += (g->advance) * font_scalar; // move offset forward

        text_size.x += text_node->size.x;
        text_size.y = max_f(text_size.y, text_node->size.y);
    }
}

void madness_draw_text_centered(Madness_UI* madness_ui, String text, vec2 parent_pos, vec2 parent_size)
{
    //center the text
    vec2 text_size;
    madness_calculate_text_size(madness_ui, text, parent_pos, &text_size);

    vec2 text_pos;
    madness_ui_center_child_node(parent_pos, parent_size, text_size, &text_pos);
    madness_draw_text(madness_ui, text, text_pos);
}

void madness_calculate_text_size(Madness_UI* madness_ui, String text, vec2 screen_position, vec2* out_text_size)
{
    f32 font_scalar = madness_ui->editor_font_size / madness_ui->default_font_size;

    vec2 start_position = screen_position;
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

bool skip_node(Madness_UI* madness_ui)
{
    Parent_Node_State* state = &madness_ui->parent_node_state;
    if (!state->is_active) return false;

    state->current_attempt_count++;
    if (state->slider_count < state->current_attempt_count &&
        state->current_attempt_count - state->slider_count <= state->max_nodes)
    {
        return false;
    }

    return true;
}

void _madness_ui_text_internal(Madness_UI* madness_ui, String text)
{

    if (text.length == 0) return;


    //roughly 80% the size of the layout
    const float button_ratio_to_layout_size = 0.8f;
    const float button_vertical_normalized_size = 0.1f;
    vec2 normalized_size;
    normalized_size.x = madness_ui->current_layout_size.x * button_ratio_to_layout_size;
    normalized_size.y = button_vertical_normalized_size;


    // proper screen pos and size
    vec2 ui_screen_pos = madness_ui->cursor_pos;
    vec2 ui_screen_size = vec2_mul(normalized_size, madness_ui->screen_size);


    //generate the text
    vec2 text_size;
    madness_calculate_text_size(madness_ui, text, ui_screen_pos, &text_size);
    madness_draw_text(madness_ui, text, ui_screen_pos);
}

void madness_ui_text(Madness_UI* madness_ui, String text)
{
    //We take the desired font size, scale it down proportional to the font size we created it at
    //final size of the font ex: 36/48 = 0.75, 48*0.75 = 36

    if (text.length == 0) return;


    //roughly 80% the size of the layout
    const float button_ratio_to_layout_size = 0.8f;
    const float button_vertical_normalized_size = 0.1f;
    vec2 normalized_size;
    normalized_size.x = madness_ui->current_layout_size.x * button_ratio_to_layout_size;
    normalized_size.y = button_vertical_normalized_size;


    // proper screen pos and size
    vec2 ui_screen_pos = madness_ui->cursor_pos;
    vec2 ui_screen_size = vec2_mul(normalized_size, madness_ui->screen_size);


    //generate the text
    vec2 text_size;
    madness_calculate_text_size(madness_ui, text, ui_screen_pos, &text_size);
    madness_draw_text(madness_ui, text, ui_screen_pos);

    //update ui state for the next element
    madness_ui_advance_cursor(madness_ui, text_size);
}

bool madness_ui_button(Madness_UI* madness_ui, String id, String text)
{
    // if (skip_node(madness_ui))
    // {
        // return false;
    // }


    // 0-1 range
    const float button_ratio_to_layout_size = 0.8f;

    // proper screen pos and size
    vec2 button_screen_pos = madness_ui->cursor_pos;
    vec2 button_screen_size = (vec2){
        madness_ui->current_layout_screen_size.x * button_ratio_to_layout_size,
        32
    };
    button_screen_pos.x += (madness_ui->current_layout_screen_size.x - button_screen_size.x) / 2.f;

    //grab a node
    UI_Node* new_node = madness_ui_get_new_node(madness_ui);
    new_node->string_id = id;
    new_node->hash_id = generate_hash_key_64bit((u8*)id.chars, id.length);
    new_node->pos = button_screen_pos;
    // new_node->size = button_screen_size;
    new_node->size = (vec2){button_screen_size.x, madness_ui->editor_font_size + madness_ui->font_padding_y};


    if (region_hit(madness_ui, new_node->pos, new_node->size))
    {
        set_hot(madness_ui, new_node->hash_id);

        //check if we have the mouse pressed and nothing else is selected
        //TODO: so there is a bug with can_be_active, in that the first ui called on the screen will take active focus,
        //TODO: this is despite there bieng another ui in front of it
        //TODO: for now imma just leave it be and dont draw things on top of others
        if (can_be_active(madness_ui))
        {
            set_active(madness_ui, new_node->hash_id);
        }
    }


    //active state
    if (is_active(madness_ui, new_node->hash_id))
    {
        new_node->color = madness_ui->editor_style.pressed_color;
    }
    //hot state
    else if (is_hot(madness_ui, new_node->hash_id))
    {
        new_node->color = madness_ui->editor_style.hovered_color;
    }
    // normal state
    else
    {
        new_node->color = madness_ui->editor_style.color;
    }

    //draw text
    madness_ui->cursor_pos = new_node->pos;
    // madness_draw_text_centered(madness_ui, text, button_screen_pos, button_screen_size);
    // madness_ui_text(madness_ui, text);
    _madness_ui_text_internal(madness_ui, text);


    //update ui state for the next element
    madness_ui_advance_cursor(madness_ui, new_node->size);


    //check if we clicked the button
    return use_ui_element(madness_ui, new_node->hash_id, button_screen_pos, button_screen_size);
}

bool madness_ui_check_box(Madness_UI* madness_ui, String id, String text, bool* check_box_state)
{
    //TODO: make into an image or at least have the button colors change somehow

    UI_Node* checkbox_node = madness_ui_get_new_node(madness_ui);
    checkbox_node->string_id = id;
    checkbox_node->hash_id = generate_hash_key_64bit((u8*)id.chars, id.length);
    checkbox_node->pos = madness_ui->cursor_pos;
    checkbox_node->size = (vec2){32, 32};

    checkbox_node->flags |= UI_FLAG_OUTLINE;

    checkbox_node->outline_color = COLOR_BLACK;
    checkbox_node->outline_thickness = 0.3;

    checkbox_node->color = COLOR_WHITE;
    if (*check_box_state)
    {
        checkbox_node->color = COLOR_RED;
    }

    madness_ui_advance_cursor(madness_ui, checkbox_node->size);

    //draw text next to the quad
    madness_draw_text(madness_ui, text, madness_ui->cursor_pos);


    if (region_hit(madness_ui, checkbox_node->pos, checkbox_node->size))
    {
        set_hot(madness_ui, checkbox_node->hash_id);

        //check if we have the mouse pressed and nothing else is selected
        //TODO: so there is a bug with can_be_active, in that the first ui called on the screen will take active focus,
        //TODO: this is despite there bieng another ui in front of it
        //TODO: for now imma just leave it be and dont draw things on top of others
        if (can_be_active(madness_ui))
        {
            set_active(madness_ui, checkbox_node->hash_id);
        }
    };


    if (use_ui_element(madness_ui, checkbox_node->hash_id, checkbox_node->pos, checkbox_node->size))
    {
        //set the bool to its opposite
        *check_box_state = !(*check_box_state);
    }


    return *check_box_state;
}

float map_range(float v, float a, float b, float x, float y)
{
    return x + (v - a) * (y - x) / (b - a);
}

void madness_slider_scroll(Madness_UI* madness_ui, String id, float* slider_val, float min, float max)
{
    //draw the rect, then based on the cur val, draw it proportionally to where it should be

    const float s = max - min;
    float t = (*slider_val - min) / s;


    // proper screen pos and size
    const float button_ratio_to_layout_size = 0.8f;
    const float button_vertical_normalized_size = 0.03f;
    vec2 normalized_size;
    normalized_size.x = madness_ui->current_layout_size.x * button_ratio_to_layout_size;
    normalized_size.y = button_vertical_normalized_size;

    // proper screen pos and size
    vec2 ui_screen_pos = madness_ui->cursor_pos;
    vec2 ui_screen_size = vec2_mul(normalized_size, madness_ui->screen_size);
    ui_screen_size.x = madness_ui->current_layout_size.x * madness_ui->screen_size.x;
    ui_screen_size.y = normalized_size.y * madness_ui->screen_size.y;


    //slider location
    float thumb_x_location = ui_screen_size.x * t;
    vec2 thumb_ui_screen_pos = ui_screen_pos;
    thumb_ui_screen_pos.x += thumb_x_location;

    //slider size
    vec2 thumb_ui_screen_size = vec2_mul_scalar(ui_screen_size, 0.4f);


    //grab a node
    UI_Node* new_node = madness_ui_get_new_node(madness_ui);
    new_node->string_id = id;
    new_node->hash_id = generate_hash_key_64bit((u8*)id.chars, id.length);
    new_node->pos = ui_screen_pos;
    new_node->size = ui_screen_size;
    new_node->color = madness_ui->editor_style.color;

    //grab another
    UI_Node* slider_node = madness_ui_get_new_node(madness_ui);
    madness_ui->ui_nodes->num_items++;
    slider_node->string_id = id;
    slider_node->hash_id = generate_hash_key_64bit((u8*)id.chars, id.length);
    slider_node->pos = thumb_ui_screen_pos;
    slider_node->size = thumb_ui_screen_size;
    slider_node->color = madness_ui->editor_style.custom_widget_color;


    //check if we are hovering over the slider
    //TODO: have the size be the entire strip where the slider is at
    vec2 temp = (vec2){ui_screen_size.x, thumb_ui_screen_size.y};
    if (region_hit(madness_ui, ui_screen_pos, temp))
    {
        slider_node->color = madness_ui->editor_style.hovered_color;

        if (madness_ui->mouse_down)
        {
            // we can just set the slider to where the mouse is
            // madness_ui->mouse_pos_y; //TODO: will need when we do vertical slider
            thumb_ui_screen_pos.x = madness_ui->mouse_pos_x - thumb_ui_screen_size.x / 2;
            slider_node->pos = thumb_ui_screen_pos;


            //calculate how much of the mouse_pos_x is from the start and end of the ui size, in percent,
            //that will be the sliders values
            f32 start = ui_screen_pos.x;
            f32 end = ui_screen_pos.x + ui_screen_size.x;
            *slider_val = (thumb_ui_screen_pos.x - start) / (end - start);

            slider_node->color = madness_ui->editor_style.pressed_color;
        }

        if (input_is_mouse_wheel_up(madness_ui->input_system_reference))
        {
            *slider_val += 0.1;
        }
        if (input_is_mouse_wheel_down(madness_ui->input_system_reference))
        {
            *slider_val -= 0.1;
        }
        *slider_val = clamp_float(*slider_val, min, max);
    };

    //update ui state for the next element
    madness_ui_advance_cursor(madness_ui, ui_screen_size);
}

void madness_slider_arrow(Madness_UI* madness_ui, String id, float* slider_val, float min, float max)
{
    //draw the rect, draw a left arrow, then right arrow, slider val in the middle
    float increment_step = 20.f;
    float increment_slider_value = (max - min) / increment_step;

    const float button_ratio_to_layout_size = 0.8f;
    const float button_vertical_normalized_size = 0.03f;
    vec2 normalized_size;
    normalized_size.x = madness_ui->current_layout_size.x * button_ratio_to_layout_size;
    normalized_size.y = button_vertical_normalized_size;

    // proper screen pos and size
    vec2 ui_screen_pos = madness_ui->cursor_pos;
    vec2 ui_screen_size = vec2_mul(normalized_size, madness_ui->screen_size);


    float arrow_size_x_size = ui_screen_size.x / 4.0;

    float right_arrow_x_pos = ui_screen_pos.x + ui_screen_size.x - arrow_size_x_size;
    vec2 right_arrow_pos = (vec2){right_arrow_x_pos, ui_screen_pos.y};

    vec2 arrow_size = {arrow_size_x_size, ui_screen_size.y};

    //rect
    UI_Node* new_node = madness_ui_get_new_node(madness_ui);
    new_node->string_id = id;
    new_node->hash_id = generate_hash_key_64bit((u8*)id.chars, id.length);
    new_node->pos = ui_screen_pos;
    new_node->size = ui_screen_size;
    new_node->color = madness_ui->editor_style.layout_accent_color;

    // left arrow
    UI_Node* left_node = madness_ui_get_new_node(madness_ui);
    left_node->string_id = id;
    left_node->hash_id = generate_hash_key_64bit((u8*)id.chars, id.length);
    left_node->pos = ui_screen_pos;
    left_node->size = arrow_size;
    left_node->color = madness_ui->editor_style.color;

    // right arrow
    UI_Node* right_node = madness_ui_get_new_node(madness_ui);
    right_node->string_id = id;
    right_node->hash_id = generate_hash_key_64bit((u8*)id.chars, id.length);
    right_node->pos = right_arrow_pos;
    right_node->size = arrow_size;
    right_node->color = madness_ui->editor_style.color;

    //text
    char* result = madness_ui_float_to_char(madness_ui, *slider_val);
    String text = STRING(result);

    vec2 text_start_pos = {ui_screen_pos.x + arrow_size.x, ui_screen_pos.y};
    vec2 inbetween_size = {ui_screen_size.x - (2.f * arrow_size.x), ui_screen_size.y};
    madness_draw_text_centered(madness_ui, text, text_start_pos, inbetween_size);

    //check if we hit the left arrow
    if (region_hit(madness_ui, ui_screen_pos, arrow_size))
    {
        left_node->color = madness_ui->editor_style.hovered_color;
        if (madness_ui->mouse_released_unique)
        {
            *slider_val -= increment_slider_value;
            left_node->color = madness_ui->editor_style.pressed_color;
        }

        /*
        if (input_is_mouse_wheel_up(madness_ui->input_system_reference))
        {
            *slider_val += 0.1;
        }
        if (input_is_mouse_wheel_down(madness_ui->input_system_reference))
        {
            *slider_val -= 0.1;
        }
        */
    };

    //check right arrow
    if (region_hit(madness_ui, right_arrow_pos, arrow_size))
    {
        right_node->color = madness_ui->editor_style.hovered_color;

        if (madness_ui->mouse_released_unique)
        {
            *slider_val += increment_slider_value;
            right_node->color = madness_ui->editor_style.pressed_color;
        }
    }


    //clamp sanity check
    *slider_val = clamp_float(*slider_val, min, max);

    //update ui state for the next element
    madness_ui_advance_cursor(madness_ui, ui_screen_size);
}


void madness_text_box(Madness_UI* madness_ui, String id)
{
    /*
    //render a quad, then render the text inside it
    const float button_vertical_normalized_size = 0.03f;
    vec2 normalized_size;
    normalized_size.x = madness_ui->current_layout_size.x;
    normalized_size.y = button_vertical_normalized_size;

    // proper screen pos and size
    vec2 ui_screen_pos = madness_ui->cursor_pos;
    vec2 ui_screen_size = vec2_mul(normalized_size, madness_ui->screen_size);


    //grab a node
    UI_Node* new_node = madness_ui_get_node(madness_ui);
    new_node->debug_id = id;
    new_node->hash_id = generate_hash_key_64bit((u8*)id, strlen(id));
    new_node->pos = ui_screen_pos;
    new_node->size = ui_screen_size;
    */

    madness_ui_text(madness_ui, STRING("Text Box:"));

    vec2 out_pos;
    vec2 out_size;
    UI_Node* quad_node = NULL;
    madness_draw_quad(madness_ui, STRING("text box quad"), &out_pos, &out_size, &quad_node);
    quad_node->color = madness_ui->editor_style.textbox_color;


    if (region_hit(madness_ui, out_pos, out_size))
    {
        quad_node->color = madness_ui->editor_style.layout_accent_color;

        //check for any keypressed and update the text
        if (input_key_released_unique(madness_ui->input_system_reference, KEY_BACKSPACE))
        {
            string_builder_decrement(madness_ui->string_builder);
        }

        if (madness_ui->released_key)
        {
            string_builder_append_char(madness_ui->string_builder, &madness_ui->released_key, 1);
        }
    }


    String display_string = string_builder_to_string_non_pointer(madness_ui->string_builder);
    madness_draw_text_centered(madness_ui, display_string, out_pos, out_size);

    madness_ui_advance_cursor(madness_ui, out_size);
}

bool madness_ui_float(Madness_UI* madness_ui, String text, float* f, float increment_value)
{
    //get our string version of the float
    //draw quad, then text
    // check for events to increment the float value

    char* float_char = madness_ui_float_to_char(madness_ui, *f);
    String float_string = {float_char, strlen(float_char)};

    float size_shink_value = 0.3f;

    vec2 out_pos;
    vec2 out_size;
    UI_Node* quad_node = NULL;
    madness_draw_quad(madness_ui, text, &out_pos, &out_size, &quad_node);
    out_size.x *= size_shink_value;
    quad_node->size.x *= size_shink_value;
    madness_draw_text_centered(madness_ui, float_string, out_pos, out_size);
    madness_ui_advance_cursor(madness_ui, out_size);

    bool has_chaned = false;

    //check if we are inside any of the squares
    if (region_hit(madness_ui, out_pos, out_size))
    {
        quad_node->color = madness_ui->editor_style.hovered_color;
        if (input_is_mouse_wheel_up(madness_ui->input_system_reference))
        {
            *f += increment_value;
            has_chaned = true;
        }
        if (input_is_mouse_wheel_down(madness_ui->input_system_reference))
        {
            *f -= increment_value;
            has_chaned = true;
        }

        set_hot(madness_ui, quad_node->hash_id);

        if (can_be_active(madness_ui))
        {
            set_active(madness_ui, quad_node->hash_id);
        }
    }

    if (is_active(madness_ui, quad_node->hash_id))
    {
        quad_node->color = madness_ui->editor_style.pressed_color;

        if (madness_ui->mouse_down)
        {
            i16 mouse_change_x;
            i16 mouse_change_y;

            input_get_mouse_change(madness_ui->input_system_reference, &mouse_change_x, &mouse_change_y);

            //we dont want to increment by the full value every single frame
            float increment_smoother_value = 8.f;
            float increment_override = 0.1f;
            if (mouse_change_x > 0)
            {
                *f += increment_value / increment_smoother_value;
                // *f += increment_override;
                has_chaned = true;
            }
            if (mouse_change_x < 0)
            {
                *f -= increment_value / increment_smoother_value;
                // *f -= increment_override;
                has_chaned = true;
            }
        }
    }


    return has_chaned;
}


bool madness_ui_float2(Madness_UI* madness_ui, String text, float* x, float* y, float increment_value)
{
    madness_ui_text(madness_ui, text);

    String* x_id = string_concat(&text, &STRING("x"), allocator_inferface_create(madness_ui->frame_arena));
    String* y_id = string_concat(&text, &STRING("y"), allocator_inferface_create(madness_ui->frame_arena));


    bool has_moved1 = madness_ui_float(madness_ui, *x_id, x, increment_value);
    bool has_moved2 = madness_ui_float(madness_ui, *y_id, y, increment_value);


    if (has_moved1) { return true; }
    if (has_moved2) { return true; }


    return false;
}

bool madness_ui_float3(Madness_UI* madness_ui, String text, float* x, float* y, float* z, float increment_value)
{
    madness_ui_text(madness_ui, text);

    String* x_id = string_concat(&text, &STRING("x"), allocator_inferface_create(madness_ui->frame_arena));
    String* y_id = string_concat(&text, &STRING("y"), allocator_inferface_create(madness_ui->frame_arena));


    bool has_moved1 = madness_ui_float(madness_ui, *x_id, x, increment_value);
    bool has_moved2 = madness_ui_float(madness_ui, *y_id, y, increment_value);


    if (has_moved1) { return true; }
    if (has_moved2) { return true; }


    return false;
}

bool madness_ui_vec2(Madness_UI* madness_ui, String text, vec2* v, float increment_value)
{
    madness_ui_text(madness_ui, text);

    String* x_id = string_concat(&text, &STRING("x"), allocator_inferface_create(madness_ui->frame_arena));
    String* y_id = string_concat(&text, &STRING("y"), allocator_inferface_create(madness_ui->frame_arena));


    bool has_moved1 = madness_ui_float(madness_ui, *x_id, &v->x, increment_value);
    bool has_moved2 = madness_ui_float(madness_ui, *y_id, &v->y, increment_value);


    if (has_moved1) { return true; }
    if (has_moved2) { return true; }


    return false;
}

bool madness_ui_vec3(Madness_UI* madness_ui, String text, vec3* v, float increment_value)
{
    if (skip_node(madness_ui))
    {
        return false;
    }

    //draw text on top, then below the vec values
    madness_ui_text(madness_ui, text);


    String* x_id = string_concat(&text, &STRING("x"), allocator_inferface_create(madness_ui->frame_arena));
    String* y_id = string_concat(&text, &STRING("y"), allocator_inferface_create(madness_ui->frame_arena));
    String* z_id = string_concat(&text, &STRING("z"), allocator_inferface_create(madness_ui->frame_arena));


    bool has_moved1 = madness_ui_float(madness_ui, *x_id, &v->x, increment_value);
    bool has_moved2 = madness_ui_float(madness_ui, *y_id, &v->y, increment_value);
    bool has_moved3 = madness_ui_float(madness_ui, *z_id, &v->z, increment_value);


    if (has_moved1) { return true; }
    if (has_moved2) { return true; }
    if (has_moved3) { return true; }


    return false;
}


bool madness_ui_color_picker(Madness_UI* madness_ui, String id, vec3* color_value)
{
    madness_ui_text(madness_ui, STRING("Color Picker"));

    vec2 out_pos;
    vec2 out_size;
    UI_Node* out_node;
    madness_draw_quad(madness_ui, id, &out_pos, &out_size, &out_node);
    out_node->color = COLOR_BLACK;

    bool out_result = false;
    if (region_hit(madness_ui, out_pos, out_size))
    {
        out_node->color = madness_ui->editor_style.hovered_color;
        if (madness_ui->mouse_released_unique)
        {
            out_node->color = madness_ui->editor_style.pressed_color;
            out_result = true;
        }
    }


    vec2 color_pos;
    vec2 color_size;
    UI_Node* color_node;
    madness_draw_quad(madness_ui, id, &color_pos, &color_size, &color_node);
    float inset_size = 0.8f;
    color_size = vec2_mul_scalar(out_size, inset_size);
    madness_ui_center_child_node(out_pos, out_size, color_size, &color_pos);

    color_node->pos = color_pos;
    color_node->size = color_size;
    color_node->color = *color_value;

    madness_ui_advance_cursor(madness_ui, out_size);


    String* x_id = string_concat(&id, &STRING("x"), allocator_inferface_create(madness_ui->frame_arena));
    String* y_id = string_concat(&id, &STRING("y"), allocator_inferface_create(madness_ui->frame_arena));
    String* z_id = string_concat(&id, &STRING("z"), allocator_inferface_create(madness_ui->frame_arena));

    float increment_value = 0.05;
    madness_ui_float(madness_ui, *x_id, &color_value->x, increment_value);
    madness_ui_float(madness_ui, *y_id, &color_value->y, increment_value);
    madness_ui_float(madness_ui, *z_id, &color_value->z, increment_value);

    color_value->x = clamp_float(color_value->x, 0.0, 1.0);
    color_value->y = clamp_float(color_value->y, 0.0, 1.0);
    color_value->z = clamp_float(color_value->z, 0.0, 1.0);


    return out_result;
}


void madness_scroll_box_begin(Madness_UI* madness_ui, String id, scroll_box_state* scroll_box_state)
{
    /*
    //debug the size of the scroll box
    UI_Node* new_node = madness_ui_get_new_node(madness_ui);
    new_node->debug_id = id;
    new_node->hash_id = generate_hash_key_64bit((u8*)id, strlen(id));
    new_node->pos = button_screen_pos;
    new_node->size = button_screen_size;
    */

    UI_Node* sroll_box_node = madness_ui_get_new_node(madness_ui);
    sroll_box_node->string_id = id;
    sroll_box_node->pos = madness_ui->cursor_pos;


    madness_ui->parent_node_state.is_active = true;
    madness_ui->parent_node_state.current_attempt_count = 0;
    madness_ui->parent_node_state.slider_count = scroll_box_state->scroll_amount;
    madness_ui->parent_node_state.max_nodes = scroll_box_state->max_nodes_to_display;
}


void madness_scroll_box_end(Madness_UI* madness_ui, String id, scroll_box_state* scroll_box_state)
{
    //TODO: bugged if there are no children

    madness_ui->parent_node_state.is_active = false;

    //get the parent node

    //last node should have a parent, which is the scroll box, if not then we do nothing????
    UI_Node* scroll_box = madness_ui_get_parent_node(madness_ui);

    float width_x = madness_ui->current_layout_screen_size.x;
    float height_y = 0;


    for (u32 child_idx = 0; child_idx < scroll_box->child_node_count; child_idx++)
    {
        vec2 size = scroll_box->children[child_idx]->size;
        width_x = max_f(width_x, size.x);
        height_y += size.y + madness_ui->element_padding_y;
    }
    height_y -= madness_ui->element_padding_y;

    height_y = max(height_y, madness_ui->current_layout_screen_size.x);


    vec2 scroll_box_screen_size = {width_x, height_y};
    // scroll_box->size = scroll_box_screen_size;
    // scroll_box->color = COLOR_ORANGE;

    scroll_box->pos.x += (madness_ui->current_layout_screen_size.x - scroll_box_screen_size.x) / 2.f;
    vec2 button_screen_pos = scroll_box->pos;


    //draw the rect, then based on the cur val, draw it proportionally to where it should be

    const float s = scroll_box_state->max_nodes_to_display - 0;
    float t = (scroll_box_state->scroll_amount - 0) / s;


    //slider location
    float thumb_y_location = (scroll_box_screen_size.y / scroll_box_state->max_nodes_to_display) * t;
    vec2 thumb_ui_screen_pos = button_screen_pos;
    thumb_ui_screen_pos.x += scroll_box_screen_size.x; // have it sit to the right of all the ui
    thumb_ui_screen_pos.y += thumb_y_location;

    //slider size
    vec2 thumb_ui_screen_size;
    thumb_ui_screen_size.x = scroll_box_screen_size.x * 0.03f;
    thumb_ui_screen_size.y = scroll_box_screen_size.y / 3;


    //grab another
    UI_Node* slider_node = madness_ui_get_new_node(madness_ui);
    slider_node->string_id = id;
    slider_node->hash_id = generate_hash_key_64bit((u8*)id.chars, id.length);
    slider_node->pos = thumb_ui_screen_pos;
    slider_node->size = thumb_ui_screen_size;
    slider_node->color = madness_ui->editor_style.custom_widget_color;

    //size of the scroll box and the slider
    vec2 hit_size = (vec2){scroll_box_screen_size.x + thumb_ui_screen_size.x, scroll_box_screen_size.y};

    //check hit on the scroll bar
    if (region_hit(madness_ui, thumb_ui_screen_pos, thumb_ui_screen_size))
    {
        if (madness_ui->mouse_down)
        {
            slider_node->color = madness_ui->editor_style.pressed_color;

            // we can just set the slider to where the mouse is
            // madness_ui->mouse_pos_y; //TODO: will need when we do vertical slider
            i16 out_x;
            i16 out_y;
            input_get_mouse_change(madness_ui->input_system_reference, &out_x, &out_y);
            if (out_y > 0)
            {
                scroll_box_state->scroll_amount += 1;

                scroll_box_state->scroll_amount = clamp_float(scroll_box_state->scroll_amount, 0,
                                                              scroll_box_state->max_nodes_to_display);
            }
            if (out_y < 0)
            {
                if (scroll_box_state->scroll_amount > 0)
                {
                    scroll_box_state->scroll_amount -= 1;
                }
            }
        }

        if (input_is_mouse_wheel_up(madness_ui->input_system_reference))
        {
            slider_node->color = madness_ui->editor_style.pressed_color;

            if (scroll_box_state->scroll_amount > 0)
            {
                scroll_box_state->scroll_amount -= 1;
            }
        }
        if (input_is_mouse_wheel_down(madness_ui->input_system_reference))
        {
            slider_node->color = madness_ui->editor_style.pressed_color;

            scroll_box_state->scroll_amount += 1;

            scroll_box_state->scroll_amount = clamp_float(scroll_box_state->scroll_amount, 0,
                                                          scroll_box_state->max_nodes_to_display);
        }
    }
};

bool madness_ui_circle(Madness_UI* madness_ui, String id, float* thickness)
{
    if (skip_node(madness_ui))
    {
        return false;
    }

    *thickness = clamp_float(*thickness, 0.0f, 1.0f);

    // 0-1 range
    const float button_ratio_to_layout_size = 0.8f;
    const float button_vertical_normalized_size = 0.03f;
    vec2 normalized_size;
    normalized_size.x = madness_ui->current_layout_size.x * button_ratio_to_layout_size;
    normalized_size.y = madness_ui->current_layout_size.x * button_ratio_to_layout_size;


    // proper screen pos and size
    vec2 button_screen_pos = madness_ui->cursor_pos;
    vec2 button_screen_size = vec2_mul(normalized_size, madness_ui->screen_size);
    button_screen_pos.x += (madness_ui->current_layout_screen_size.x - button_screen_size.x) / 2.f;

    //grab a node
    UI_Node* new_node = madness_ui_get_new_node(madness_ui);
    new_node->string_id = id;
    new_node->hash_id = generate_hash_key_64bit((u8*)id.chars, id.length);
    new_node->pos = button_screen_pos;
    new_node->size = button_screen_size;
    new_node->thickness = *thickness;
    new_node->flags |= UI_FLAG_CIRCLE;


    if (region_hit(madness_ui, new_node->pos, new_node->size))
    {
        set_hot(madness_ui, new_node->hash_id);

        //check if we have the mouse pressed and nothing else is selected
        //TODO: so there is a bug with can_be_active, in that the first ui called on the screen will take active focus,
        //TODO: this is despite there bieng another ui in front of it
        //TODO: for now imma just leave it be and dont draw things on top of others
        if (can_be_active(madness_ui))
        {
            set_active(madness_ui, new_node->hash_id);
        }
    }


    //active state
    if (is_active(madness_ui, new_node->hash_id))
    {
        new_node->color = madness_ui->editor_style.pressed_color;
    }
    //hot state
    else if (is_hot(madness_ui, new_node->hash_id))
    {
        new_node->color = madness_ui->editor_style.hovered_color;
    }
    // normal state
    else
    {
        new_node->color = madness_ui->editor_style.color;
    }


    //update ui state for the next element
    madness_ui_advance_cursor(madness_ui, button_screen_size);


    //check if we clicked the button
    return use_ui_element(madness_ui, new_node->hash_id, button_screen_pos, button_screen_size);
}

bool madness_ui_node_simple(Madness_UI* madness_ui, String id, vec2 pos, String inputs[], u8 input_size,
                            String outputs[], u8 output_size, u32 node_id)
{
    u32 size_y_padding = 20;
    u32 size_y_increment = madness_ui->editor_font_size + size_y_padding;

    UI_Node* background = madness_ui_get_new_node(madness_ui);
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
        vec2 tex_pos = {x_pos, y_pos};
        vec2 text_size;
        madness_calculate_text_size(madness_ui, inputs[i], tex_pos, &text_size);
        madness_draw_text(madness_ui, inputs[i], tex_pos);


        if (region_hit(madness_ui, tex_pos, text_size))
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
        vec2 tex_pos = {x_pos + 50, y_pos};
        vec2 text_size;
        madness_calculate_text_size(madness_ui, outputs[i], tex_pos, &text_size);
        madness_draw_text(madness_ui, outputs[i], tex_pos);

        if (region_hit(madness_ui, tex_pos, text_size))
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
                UI_Node* connection = madness_ui_get_new_node(madness_ui);
                connection->pos = tex_pos;
                connection->size = (vec2){vec2_sub(madness_ui->input_pos, tex_pos).x, 10};
                connection->color = madness_ui->editor_style.hovered_color;
            }
        };
    }

    background->size.y = max_f(size_input, size_output);
    return false;
}

bool madness_ui_node(Madness_UI* madness_ui, String id, String inputs[], u8 input_size, String outputs[],
                     u8 output_size)
{
    u32 size_y_padding = 20;
    u32 size_y_increment = madness_ui->editor_font_size + size_y_padding;

    UI_Node* background = madness_ui_get_new_node(madness_ui);
    background->pos.x = 50;
    background->pos.y = 50;
    background->size.x = 100;
    background->color = madness_ui->editor_style.color;

    u32 size_input = 0; //madness_ui->editor_font_size;
    u32 size_output = 0; //madness_ui->editor_font_size;
    for (u8 i = 0; i < input_size; i++)
    {
        size_input += (i * size_y_increment);
        madness_draw_text(madness_ui, inputs[i], (vec2){50, 50 + (i * size_y_increment)});
    }
    for (u8 i = 0; i < output_size; i++)
    {
        size_output += (i * size_y_increment);
        madness_draw_text(madness_ui, outputs[i], (vec2){100, 50 + (i * size_y_increment)});
    }

    background->size.y = max_f(size_input, size_output);


    return false;
}

bool madness_ui_drag_test(Madness_UI* madness_ui, vec2* pos)
{
    UI_Node* background = madness_ui_get_new_node(madness_ui);
    background->pos = *pos;
    background->size.x = 150;
    background->size.y = 150;
    background->color = madness_ui->editor_style.color;


    if (region_hit(madness_ui, background->pos, background->size))
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

bool madness_ui_quadratic_bezier(Madness_UI* madness_ui, vec2* pos1, vec2* pos2, vec2* pos3)
{
    bool has_moved = false;

    UI_Node* node1 = madness_ui_get_new_node(madness_ui);
    node1->pos = *pos1;
    node1->size.x = 150;
    node1->size.y = 150;
    node1->color = COLOR_GREEN;
    node1->thickness = 1.f;
    node1->flags = UI_FLAG_CIRCLE;

    if (region_hit(madness_ui, node1->pos, node1->size))
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


    UI_Node* node2 = madness_ui_get_new_node(madness_ui);
    node2->pos = *pos2;
    node2->size.x = 150;
    node2->size.y = 150;
    node2->color = COLOR_RED;
    node2->thickness = 1.f;
    node2->flags = UI_FLAG_CIRCLE;

    if (region_hit(madness_ui, node2->pos, node2->size))
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


    UI_Node* node3 = madness_ui_get_new_node(madness_ui);
    node3->pos = *pos3;
    node3->size.x = 150;
    node3->size.y = 150;
    node3->color = COLOR_BLUE;
    node3->thickness = 1.f;
    node3->flags = UI_FLAG_CIRCLE;

    if (region_hit(madness_ui, node3->pos, node3->size))
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
        UI_Node* background = madness_ui_get_new_node(madness_ui);
        background->size.x = 10;
        background->size.y = 10;
        background->color = madness_ui->editor_style.color;

        float t = (float)i / (float)steps;
        background->pos = quadratic_bezier(*pos1, *pos2, *pos3, t);
    }

    return has_moved;
}

bool madness_ui_cubic_bezier(Madness_UI* madness_ui, vec2* pos1, vec2* pos2, vec2* pos3, vec2* pos4)
{
    bool has_moved = false;

    UI_Node* node1 = madness_ui_get_new_node(madness_ui);
    node1->pos = *pos1;
    node1->size.x = 150;
    node1->size.y = 150;
    node1->color = COLOR_GREEN;
    node1->thickness = 1.f;
    node1->flags = UI_FLAG_CIRCLE;

    if (region_hit(madness_ui, node1->pos, node1->size))
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


    UI_Node* node2 = madness_ui_get_new_node(madness_ui);
    node2->pos = *pos2;
    node2->size.x = 150;
    node2->size.y = 150;
    node2->color = COLOR_RED;
    node2->thickness = 1.f;
    node2->flags = UI_FLAG_CIRCLE;

    if (region_hit(madness_ui, node2->pos, node2->size))
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


    UI_Node* node3 = madness_ui_get_new_node(madness_ui);
    node3->pos = *pos3;
    node3->size.x = 150;
    node3->size.y = 150;
    node3->color = COLOR_BLUE;
    node3->thickness = 1.f;
    node3->flags = UI_FLAG_CIRCLE;

    if (region_hit(madness_ui, node3->pos, node3->size))
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

    UI_Node* node4 = madness_ui_get_new_node(madness_ui);
    node4->pos = *pos4;
    node4->size.x = 150;
    node4->size.y = 150;
    node4->color = COLOR_MAGENTA;
    node4->thickness = 1.f;
    node4->flags = UI_FLAG_CIRCLE;

    if (region_hit(madness_ui, node4->pos, node4->size))
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
        UI_Node* background = madness_ui_get_new_node(madness_ui);
        background->size.x = 10;
        background->size.y = 10;
        background->color = madness_ui->editor_style.color;

        float t = (float)i / (float)steps;
        background->pos = cubic_bezier(*pos1, *pos2, *pos3, *pos4, t);
        float a = 2;
    }

    return has_moved;
}


void madness_ui_test(Madness_UI* madness_ui)
{
    /*
    do_text(madness_ui,
            STRING("!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"),
            (vec2){0, 70}, (vec2){10, 10},
            COLOR_WHITE,
            DEFAULT_FONT_SIZE);
    */

    madness_ui_window(madness_ui, STRING("Madness UI Test Layout"));

    if (madness_ui_button(madness_ui, STRING("first button"), STRING("these are buttons")))
    {
        FATAL("YOU HAVE PRESSED THE BUTTON OH LORD WHY!");
    };
    if (madness_ui_button(madness_ui, STRING("test button 2"), STRING("le_button 2")))
    {
        FATAL("YOU HAVE PRESSED THE BUTTON OH LORD WHY!");
    }
    madness_ui_button(madness_ui, STRING("test button 3"), STRING("Le Button 3"));
    madness_ui_text(madness_ui, STRING("editor_font_size"));
    madness_ui_float(madness_ui, STRING("editor_font_slider"), &madness_ui->editor_font_size, 1.f);
    madness_ui_text(madness_ui, STRING("default_font_size"));
    madness_ui_float(madness_ui, STRING("default_font_slider"), &madness_ui->default_font_size, 1.f);

    {
        //if we want a layout change, specify so,
        static bool check_box_test;

        if (madness_ui_check_box(madness_ui, STRING("check_box"), STRING("Check Box"), &check_box_test))
        {
            madness_ui->editor_style.text_color = COLOR_PURPLE_PALETTE_LIGHT;
            WARN("%d", check_box_test);
        }
        else
        {
            madness_ui->editor_style.text_color = COLOR_WHITE;
        }

        madness_ui_check_box(madness_ui, STRING("check_box2"), STRING("Other"), &check_box_test);
        madness_ui_check_box(madness_ui, STRING("check_box3"), STRING("Next"), &check_box_test);
    }

    madness_ui_text(madness_ui, STRING("GOD DAMN IT BOBBY"));
    if (madness_ui_button(madness_ui, STRING("test button text"), STRING("AND SO IT GOES")))
    {
        FATAL(" BUTTONS AND DEATH");
    }


    static float slider_val;
    float slider_min = 0;
    float slider_max = 1;
    madness_slider_scroll(madness_ui, STRING("slider"), &slider_val, slider_min, slider_max);

    madness_slider_arrow(madness_ui, STRING("slider arrow"), &slider_val, slider_min, slider_max);

    madness_text_box(madness_ui, STRING("textbox"));

    static vec3 vec3_test;
    float vec3_change_value = 10.5f;
    madness_ui_vec3(madness_ui, STRING("Position"), &vec3_test, vec3_change_value);

    static vec2 vec2_test;
    madness_ui_vec2(madness_ui, STRING("Sprite Position"), &vec2_test, vec3_change_value);

    static vec3 color_test;
    madness_ui_color_picker(madness_ui, STRING("Color Picker"), &color_test);


    static scroll_box_state scroll_box_state_test;
    scroll_box_state_test.max_nodes_to_display = 3;

    madness_scroll_box_begin(madness_ui, STRING("scroll box"), &scroll_box_state_test);
    {
        madness_ui_button(madness_ui, STRING("Scollbox Button 1"), STRING("Scroll Around 1"));
        madness_ui_button(madness_ui, STRING("Scollbox Button 2"), STRING("Scroll Around 2"));
        if (madness_ui_button(madness_ui, STRING("Scollbox Button 3"), STRING("Scroll Around 3")))
        {
            FATAL("BUTTONS AND DEATH");
        }
        madness_ui_button(madness_ui, STRING("Scollbox Button 4"), STRING("Scroll Around 4"));
        madness_ui_button(madness_ui, STRING("Scollbox Button 5"), STRING("Scroll Around 5"));
        madness_ui_button(madness_ui, STRING("Scollbox Button 6"), STRING("Scroll Around 6"));
    }
    madness_scroll_box_end(madness_ui, STRING("scroll box"), &scroll_box_state_test);
}

void madness_ui_example(Madness_UI* madness_ui)
{
    madness_ui_menu_bar(madness_ui, STRING("title bar"));
    madness_ui_same_line(madness_ui);
    if (madness_ui_menu_item(madness_ui, STRING("file")))
    {
        WARN("file hover")
    };
    madness_ui_same_line(madness_ui);
    madness_ui_menu_item(madness_ui, STRING("name"));
    madness_ui_same_line(madness_ui);
    madness_ui_menu_item(madness_ui, STRING("option"));

    //TODO: text overrides the largest move down value
    madness_ui_window(madness_ui, STRING("Madness UI Test Layout"));
    madness_ui_button(madness_ui, STRING("Button 1"), STRING("Button 1"));
    madness_ui_same_line(madness_ui);
    madness_ui_button(madness_ui, STRING("Button 2"), STRING("Button 2"));
    madness_ui_button(madness_ui, STRING("Button 3"), STRING("Button 3"));
    // static bool checkbox;
    // madness_ui_check_box(madness_ui, STRING("checkbox 1"), STRING("click me"), &checkbox);
    // madness_ui_check_box(madness_ui, STRING("checkbox 2"), STRING("click me ah"), &checkbox);


    // madness_ui_config_menu(madness_ui);
}


void madness_ui_config_menu(Madness_UI* madness_ui)
{
    madness_ui_window(madness_ui, STRING("UI Config"));
    madness_ui_text(madness_ui, STRING("font size"));
    madness_ui_float(madness_ui, STRING("font size"), &madness_ui->editor_font_size, 1);

    madness_ui_float2(madness_ui, STRING("Padding f2 x&y"),
                      &madness_ui->element_padding_x, &madness_ui->element_padding_y, 1);
}
