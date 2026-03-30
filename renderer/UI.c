#include "UI.h"

#include <complex.h>

#include "logger.h"

#define MAX_UI_SPRITE_COUNT 1000
#define MAX_TEXT_SPRITE_COUNT 1000
#define MAX_BUTTON_COUNT 1000


Madness_UI* madness_ui_init(Memory_System* memory_system, Renderer* renderer)
{
    Madness_UI* madness_ui = memory_system_alloc(memory_system, sizeof(Madness_UI));

    u64 ui_arena_mem_size = MB(1024);
    u64 ui_frame_arena_mem_size = MB(1024);
    madness_ui->mem_tracker = memory_system_get_memory_tracker(memory_system->memory_tracker_system, STRING("MADNESS UI"),
                                                               ui_arena_mem_size + ui_frame_arena_mem_size);

    madness_ui->arena  = memory_system_alloc(memory_system, sizeof(Arena));
    madness_ui->frame_arena  = memory_system_alloc(memory_system, sizeof(Arena));

    void* arena_memory = memory_system_alloc(memory_system, ui_arena_mem_size);
    void* frame_arena_memory = memory_system_alloc(memory_system, ui_frame_arena_mem_size);

    arena_init(madness_ui->arena , arena_memory, ui_arena_mem_size, madness_ui->mem_tracker);
    arena_init(madness_ui->frame_arena , frame_arena_memory, ui_arena_mem_size, madness_ui->mem_tracker);

    madness_ui->renderer_reference = renderer;
    madness_ui->input_system_reference = renderer->input_system_debug;

    madness_ui->index_type = VK_INDEX_TYPE_UINT16;

    madness_ui->ui_data = Sprite_Data_array_create(MAX_UI_SPRITE_COUNT);
    madness_ui->text_data = Sprite_Data_array_create(MAX_TEXT_SPRITE_COUNT);

    madness_ui->default_font_size = DEFAULT_FONT_CREATION_SIZE;
    madness_ui->editor_font_size = EDITOR_FONT_SIZE;

    madness_ui->ui_nodes = UI_Node_array_create(MAX_UI_NODE_COUNT);
    madness_ui->ui_nodes_text = UI_Node_Text_array_create(MAX_UI_TEXT_NODE_COUNT);
    madness_ui->string_builder = string_builder_create(100);


    //ui button state is the u32
    madness_ui->button_hash_states = HASH_TABLE_CREATE(u32, MAX_BUTTON_COUNT);

    madness_ui->editor_style = (UI_Editor_Style){
        .layout_color = COLOR_PURPLE_PALETTE_DARK, .layout_accent_color = COLOR_PURPLE_PALETTE_PURPLE,
        .text_color = COLOR_PURPLE_PALETTE_LIGHT, .textbox_color = COLOR_PURPLE_PALETTE_DARK2,
        .custom_widget_color = COLOR_PURPLE_PALETTE_PURPLE_LIGHT,
        .color = COLOR_PURPLE_PALETTE_PURPLE_STRONG, .hovered_color = COLOR_PURPLE_PALETTE_PURPLE_LIGHT2,
        .pressed_color = COLOR_PURPLE_PALETTE_DARK2,
        .outline_color = COLOR_PURPLE_PALETTE_PURPLE_LIGHT
    };

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


    //TODO: THERE IS NO WAY I AM LOADING THIS FILE FROM THERE, WHAT ABOUT LINUX
    font_init(madness_ui, renderer, "c:/windows/fonts/arialbd.ttf");

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

    //std::cout << "MOUSE STATE:" << Madness_UI.mouse_down << '\n';

    madness_ui->screen_size.x = screen_size_x;
    madness_ui->screen_size.y = screen_size_y;

    Sprite_Data_array_clear(madness_ui->ui_data);
    Sprite_Data_array_clear(madness_ui->text_data);


    UI_Node_array_zero(madness_ui->ui_nodes);
    UI_Node_array_clear(madness_ui->ui_nodes);

    UI_Node_Text_array_zero(madness_ui->ui_nodes_text);
    UI_Node_Text_array_clear(madness_ui->ui_nodes_text);


    madness_ui->cursor_pos = vec2_zero();
    madness_ui->layout_direction = UI_LAYOUT_VERTICAL;
    madness_ui->element_padding = 10.0f;

    madness_ui->hot = -1;

    madness_ui->mouse_down = input_is_mouse_button_pressed(madness_ui->input_system_reference, MOUSE_BUTTON_LEFT);
    madness_ui->mouse_released_unique = input_is_mouse_button_released_unique(
        madness_ui->input_system_reference, MOUSE_BUTTON_LEFT);
    //this can be 0 if invalid
    madness_ui->released_key = input_get_first_released_key(madness_ui->input_system_reference);
}

void madness_ui_end(Madness_UI* madness_ui)
{
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
    update_ui_mouse_pos(madness_ui);
}


Font_Handle font_init(Madness_UI* madness_ui, Renderer* renderer, const char* filepath)
{
    //TODO: rn only loads the default font, should get an available font at startup
    Madness_Font* font_structure = &madness_ui->default_font;

    Font_Handle out_handle = {-1};

    // Load font file
    FILE* font_file = fopen(filepath, "rb");
    if (!font_file)
    {
        printf("Failed to open font file: %s\n", filepath);
        return out_handle;
    }

    fseek(font_file, 0, SEEK_END);
    size_t size = ftell(font_file);
    rewind(font_file);

    unsigned char* ttf_buffer = (unsigned char*)malloc(size);
    fread(ttf_buffer, 1, size, font_file);
    fclose(font_file);

    // Initialize stb_truetype
    if (!stbtt_InitFont(&font_structure->font_info, ttf_buffer, stbtt_GetFontOffsetForIndex(ttf_buffer, 0)))
    {
        WARN("Failed to initialize font\n");
        free(ttf_buffer);
        return out_handle;
    }

    // Generate bitmap atlas
    float scale = stbtt_ScaleForPixelHeight(&font_structure->font_info, madness_ui->default_font_size);
    int atlasWidth = 1024 * 4;
    int atlasHeight = 1024 * 4;
    char* atlasPixels = arena_alloc(madness_ui->arena, atlasWidth * atlasHeight);

    int x = 0, y = 0, rowHeight = 0;

    // Get font metrics for baseline calculation
    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&font_structure->font_info, &ascent, &descent, &lineGap);
    float baseline = ascent * scale;


    for (int c = 32; c < 128; c++)
    {
        int width, height, xoff, yoff;
        unsigned char* bitmap = stbtt_GetCodepointBitmap(&font_structure->font_info, 0, scale, c,
                                                         &width, &height, &xoff, &yoff);

        // Handle atlas overflow
        if (x + width > atlasWidth)
        {
            x = 0;
            y += rowHeight;
            rowHeight = 0;
        }

        if (y + height > atlasHeight)
        {
            WARN("Error: Texture atlas too small!\n");
            stbtt_FreeBitmap(bitmap, NULL);
            free(ttf_buffer);
            return out_handle;
        }

        // Copy glyph bitmap into atlas
        for (int row = 0; row < height; row++)
        {
            memcpy(&atlasPixels[(y + row) * atlasWidth + x],
                   &bitmap[row * width], width);
        }


        // Store glyph metrics
        Glyph* g = &font_structure->glyphs[c - 32];
        g->width = width;
        g->height = height;
        g->xoff = xoff;
        g->yoff = baseline + yoff; // Adjust for baseline

        int advance, lsb;
        stbtt_GetCodepointHMetrics(&font_structure->font_info, c, &advance, &lsb);
        g->advance = advance * scale;


        // Calculate UV coordinates
        g->u0 = (float)x / (float)atlasWidth;
        g->v0 = (float)y / (float)atlasHeight;
        g->u1 = (float)(x + width) / (float)atlasWidth;
        g->v1 = (float)(y + height) / (float)atlasHeight;

        /*printf("Glyph '%c': size=(%d,%d), offset=(%d,%.1f), advance=%.1f, UV=(%.3f,%.3f)-(%.3f,%.3f)\n",
               c, width, height, xoff, g.yoff, g.advance, g.u0, g.v0, g.u1, g.v1);
               */

        x += width + 48; // Add 1 pixel padding
        if (height > rowHeight) rowHeight = height + 1;

        stbtt_FreeBitmap(bitmap, NULL);
    }

    free(ttf_buffer);

    // Convert to RGBA
    u64 atlasRGBA_size = atlasWidth * atlasHeight * 4;
    unsigned char* atlas_RGBA = arena_alloc(madness_ui->arena, atlasRGBA_size);
    for (int i = 0; i < atlasWidth * atlasHeight; i++)
    {
        unsigned char v = atlasPixels[i];
        atlas_RGBA[i * 4 + 0] = 255; // R
        atlas_RGBA[i * 4 + 1] = 255; // G
        atlas_RGBA[i * 4 + 2] = 255; // B
        atlas_RGBA[i * 4 + 3] = v; // A (alpha from glyph)
    }

    // Save atlas to file for debugging, will be under cmake-build-debug
    const char* debug_filename = "font_atlas_debug.ppm";
    FILE* debug_file = fopen(debug_filename, "wb");
    if (debug_file)
    {
        // Write PPM header (P6 format for RGB)
        // DEBUG(debug_file, "P6\n%d %d\n255\n", atlasWidth, atlasHeight);
        DEBUG("P6\n%d %d\n255\n", atlasWidth, atlasHeight);

        // Write RGB data to PPM filing using the debug data, which is basically not flipped
        for (int i = 0; i < atlasWidth * atlasHeight; i++)
        {
            fwrite(&atlasPixels[i], 1, 1, debug_file); // R
            fwrite(&atlasPixels[i], 1, 1, debug_file); // G
            fwrite(&atlasPixels[i], 1, 1, debug_file); // B
        }
        fclose(debug_file);
        DEBUG("Font atlas saved to: %s\n", debug_filename);
    }
    else
    {
        printf("Warning: Could not save font atlas debug file\n");
    }

    // Also save as raw RGBA data
    const char* raw_filename = "font_atlas_debug.raw";
    FILE* raw_file = fopen(raw_filename, "wb");
    if (!raw_file)
    {
        WARN("FONT_INIT: COULDN'T OPEN FONT ATLAS DEBUG RAW");
        fclose(raw_file);
    }

    fwrite(atlas_RGBA, 1, atlasRGBA_size, raw_file);
    DEBUG("Raw RGBA atlas data saved to: %s (%dx%d RGBA)\n",
          raw_filename, atlasWidth, atlasHeight);
    fclose(raw_file);

    // Upload to Vulkan texture
    madness_ui->default_font.font_texture_handle = shader_system_add_texture_font(
        renderer, renderer->shader_system, atlas_RGBA, atlasWidth, atlasHeight);
    /*update_texture_bindless_descriptor_set(renderer,
                                           renderer->descriptor_system,
                                           Madness_UI->default_font.font_texture_handle);*/

    // create_texture_glyph(renderer, renderer->context.graphics_command_buffer, &font_structure->font_texture,
    // atlas_RGBA, atlasWidth, atlasHeight);

    DEBUG("Font loaded successfully: %s\n", filepath);

    return out_handle;
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


void update_ui_mouse_pos(Madness_UI* madness_ui)
{
    input_get_mouse_pos(madness_ui->input_system_reference, &madness_ui->mouse_pos_x, &madness_ui->mouse_pos_y);
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
    return madness_ui->ui_nodes->data[madness_ui->ui_nodes->num_items - 1].parent;
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


void madness_ui_update_next_element_pos(Madness_UI* madness_ui, vec2 ui_screen_size)
{
    switch (madness_ui->layout_direction)
    {
    case UI_LAYOUT_HORIZONTAL:
        madness_ui->cursor_pos.x += ui_screen_size.x + madness_ui->element_padding;
        break;
    case UI_LAYOUT_VERTICAL:
        madness_ui->cursor_pos.y += ui_screen_size.y + madness_ui->element_padding;
        break;
    }
    madness_ui->ghost_pos = ui_screen_size;
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
    char* result = arena_alloc(madness_ui->frame_arena, len + 1);
    snprintf(result, len + 1, "%.3f", value);

    return result;
}


void madness_ui_begin_layout(Madness_UI* madness_ui, const char* id, vec2 pos, vec2 size)
{
    if (pos.x > 100 || pos.x < 1 || pos.y > 100 || pos.y < 1)
    {
        WARN("MADNESS REGION BEGIN ID: %s, INCORRECT DIMENSIONS", id);
    }
    //normalizt the position and size
    madness_ui->current_layout_pos = vec2_div_scalar(pos, 100.0f);
    madness_ui->current_layout_size = vec2_div_scalar(size, 100.0f);

    madness_ui->cursor_pos = vec2_mul(madness_ui->current_layout_pos, madness_ui->screen_size);

    madness_ui->current_layout_screen_pos = vec2_mul(madness_ui->current_layout_pos, madness_ui->screen_size);
    madness_ui->current_layout_screen_size = vec2_mul(madness_ui->current_layout_size, madness_ui->screen_size);


    //create the background
    UI_Node* background_node = madness_ui_get_new_node(madness_ui);
    background_node->pos = madness_ui->current_layout_screen_pos;
    background_node->size = madness_ui->current_layout_screen_size;
    background_node->color = madness_ui->editor_style.layout_color;
    background_node->debug_id = id;

    //create the header
    vec2 header_size = {
        madness_ui->current_layout_screen_size.x,
        madness_ui->current_layout_screen_size.y * 0.04
    };

    UI_Node* header_node = madness_ui_get_new_node(madness_ui);
    header_node->pos = madness_ui->current_layout_screen_pos;
    header_node->size = header_size;
    header_node->color = madness_ui->editor_style.layout_accent_color;
    header_node->debug_id = id;

    String header_text = {id, strlen(id)};

    //sizing and position of text
    vec2 text_size;
    madness_calculate_text_size(madness_ui, header_text, madness_ui->current_layout_screen_pos, &text_size);
    vec2 text_pos;
    madness_ui_center_child_node(madness_ui->current_layout_screen_pos, header_size, text_size, &text_pos);

    madness_draw_text(madness_ui, header_text,
                      (vec2){madness_ui->current_layout_screen_pos.x + madness_ui->element_padding, text_pos.y});

    madness_ui_update_next_element_pos(madness_ui, header_size);
}


void madness_set_layout_direction(Madness_UI* madness_ui, UI_Layout_Direction layout_direction)
{
    //same line is asking if we want to be on the same line as the previous ui element
    madness_ui->layout_direction = layout_direction;

    //the y value always stays the same, we only want to change the x
    madness_ui->cursor_pos.x = vec2_mul(madness_ui->current_layout_pos, madness_ui->screen_size).x;

    switch (layout_direction)
    {
    case UI_LAYOUT_HORIZONTAL:
        // madness_ui->cursor_pos.x += madness_ui->ghost_pos.x;
        break;
    case UI_LAYOUT_VERTICAL:
        madness_ui->cursor_pos.y += madness_ui->ghost_pos.y + madness_ui->element_padding;
        break;
    }
}

void madness_draw_quad(Madness_UI* madness_ui, const char* id, vec2* out_pos, vec2* out_size, UI_Node** out_node)
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
    new_node->debug_id = id;
    new_node->hash_id = generate_hash_key_64bit((u8*)id, strlen(id));
    new_node->pos = ui_screen_pos;
    new_node->size = ui_screen_size;
    new_node->color = madness_ui->editor_style.color;

    *out_pos = ui_screen_pos;
    *out_size = ui_screen_size;
    *out_node = new_node;
}

void madness_draw_text(Madness_UI* madness_ui, String text, vec2 screen_position)
{
    f32 font_scalar = madness_ui->editor_font_size / madness_ui->default_font_size;


    for (u64 i = 0; i < text.length; i++)
    {
        const char c = text.chars[i];

        if (c < 32 || c >= 128) continue; // skip unsupported characters
        Glyph* g = &madness_ui->default_font.glyphs[c - 32];

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
        text_node->texture_handle = madness_ui->default_font.font_texture_handle;

        if (i == 0)
        {
            text_node->start_text = true;
        }

        screen_position.x += (g->advance) * font_scalar; // move offset forward
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

    for (u64 i = 0; i < text.length; i++)
    {
        const char c = text.chars[i];

        if (c < 32 || c >= 128) continue; // skip unsupported characters
        Glyph* g = &madness_ui->default_font.glyphs[c - 32];

        f32 y_height = ((f32)g->height * font_scalar);

        max_height_y = max(y_height, max_height_y);

        //printf("xpos %f, ypos%f, w%f, h%f\n", xpos, ypos, w, h);

        screen_position.x += (g->advance) * font_scalar; // move offset forward
    }

    if (out_text_size)
    {
        out_text_size->x = screen_position.x - start_position.x;
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


bool madness_button(Madness_UI* madness_ui, const char* id)
{
    //we want to center the button, and have it be roughly 80% the horizontal size of the layout

    // 0-1 range
    const float button_ratio_to_layout_size = 0.8f;
    const float button_vertical_normalized_size = 0.03f;
    vec2 normalized_size;
    normalized_size.x = madness_ui->current_layout_size.x * button_ratio_to_layout_size;
    normalized_size.y = button_vertical_normalized_size;

    // proper screen pos and size
    vec2 ui_screen_pos = madness_ui->cursor_pos;
    vec2 ui_screen_size = vec2_mul(normalized_size, madness_ui->screen_size);

    ui_screen_pos.x += (madness_ui->current_layout_screen_size.x - ui_screen_size.x) / 2.f;


    //grab a node
    UI_Node* new_node = madness_ui_get_new_node(madness_ui);
    new_node->debug_id = id;
    new_node->hash_id = generate_hash_key_64bit((u8*)id, strlen(id));
    new_node->pos = ui_screen_pos;
    new_node->size = ui_screen_size;


    //check our button state, ideally this should just be hashed
    /*
    if (region_hit_new(madness_ui, ui_final_pos, ui_screen_size))
    {
        set_hot(madness_ui, temp_id );

        //check if we have the mouse pressed and nothing else is selected
        //TODO: so there is a bug with can_be_active, in that the first ui called on the screen will take active focus,
        //TODO: this is despite there bieng another ui in front of it
        //TODO: for now imma just leave it be and dont draw things on top of others
        if (can_be_active(madness_ui))
        {
            set_active(madness_ui, temp_id);
        }
    }
    */

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


    //set color based on style
    //update ui state for the next element
    madness_ui_update_next_element_pos(madness_ui, ui_screen_size);


    //check if we clicked the button
    if (use_ui_element(madness_ui, new_node->hash_id, ui_screen_pos, ui_screen_size)) return true;

    return false;
}


void madness_text(Madness_UI* madness_ui, const char* id, String text)
{
    //We take the desired font size, scale it down proportional to the font size we created it at
    //final size of the font ex: 36/48 = 0.75, 48*0.75 = 36
    f32 font_scalar = madness_ui->editor_font_size / madness_ui->default_font_size;

    //roughly 80% the size of the layout
    const float button_ratio_to_layout_size = 0.8f;
    const float button_vertical_normalized_size = 0.1f;
    vec2 normalized_size;
    normalized_size.x = madness_ui->current_layout_size.x * button_ratio_to_layout_size;
    normalized_size.y = button_vertical_normalized_size;


    // proper screen pos and size
    vec2 ui_screen_pos = madness_ui->cursor_pos;
    vec2 ui_screen_size = vec2_mul(normalized_size, madness_ui->screen_size);

    vec2 screen_position = ui_screen_pos;

    //generate the text
    vec2 text_size;
    madness_calculate_text_size(madness_ui, text, screen_position, &text_size);
    madness_draw_text(madness_ui, text, screen_position);

    //update ui state for the next element
    madness_ui_update_next_element_pos(madness_ui, text_size);
}

bool madness_button_text(Madness_UI* madness_ui, const char* id, String text)
{
    if (skip_node(madness_ui))
    {
        return false;
    }


    // 0-1 range
    const float button_ratio_to_layout_size = 0.8f;
    const float button_vertical_normalized_size = 0.03f;
    vec2 normalized_size;
    normalized_size.x = madness_ui->current_layout_size.x * button_ratio_to_layout_size;
    normalized_size.y = button_vertical_normalized_size;


    // proper screen pos and size
    vec2 button_screen_pos = madness_ui->cursor_pos;
    vec2 button_screen_size = vec2_mul(normalized_size, madness_ui->screen_size);
    button_screen_pos.x += (madness_ui->current_layout_screen_size.x - button_screen_size.x) / 2.f;

    //grab a node
    UI_Node* new_node = madness_ui_get_new_node(madness_ui);
    new_node->debug_id = id;
    new_node->hash_id = generate_hash_key_64bit((u8*)id, strlen(id));
    new_node->pos = button_screen_pos;
    new_node->size = button_screen_size;


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
    madness_draw_text_centered(madness_ui, text, button_screen_pos, button_screen_size);


    //update ui state for the next element
    madness_ui_update_next_element_pos(madness_ui, button_screen_size);


    //check if we clicked the button
    return use_ui_element(madness_ui, new_node->hash_id, button_screen_pos, button_screen_size);
}

bool madness_check_box(Madness_UI* madness_ui, const char* id, String text, bool* check_box_state)
{
    //TODO: make into an image or at least have the button colors change somehow


    vec2 out_pos_outline;
    vec2 out_size_outline;
    UI_Node* out_node_outline = NULL;
    madness_draw_quad(madness_ui, id, &out_pos_outline, &out_size_outline, &out_node_outline);
    out_size_outline.x *= 0.1f;
    out_node_outline->size = out_size_outline;
    out_node_outline->color = madness_ui->editor_style.color;

    vec2 out_pos;
    vec2 out_size;
    UI_Node* out_node = NULL;
    madness_draw_quad(madness_ui, id, &out_pos, &out_size, &out_node);
    //cut down the size, just a bit smaller from the outline box
    out_size = out_size_outline;
    out_size = vec2_mul_scalar(out_size, 0.6f);
    out_node->size = out_size;
    //position the quad in the middle of the outline
    vec2 size_diff = vec2_sub(out_size_outline, out_size);
    out_pos.x += size_diff.x / 2.0f;
    out_pos.y += size_diff.y / 2.0f;
    out_node->pos = out_pos;


    //draw text next to the quad
    vec2 text_pos = {out_pos_outline.x + out_size_outline.x, out_pos_outline.y};
    vec2 text_size;
    madness_calculate_text_size(madness_ui, text, text_pos, &text_size);
    text_pos.y = out_pos_outline.y + ((out_size_outline.y - text_size.y) / 2.0f);
    madness_draw_text(madness_ui, text, text_pos);


    if (region_hit(madness_ui, out_pos_outline, out_size_outline))
    {
        set_hot(madness_ui, out_node->hash_id);

        //check if we have the mouse pressed and nothing else is selected
        //TODO: so there is a bug with can_be_active, in that the first ui called on the screen will take active focus,
        //TODO: this is despite there bieng another ui in front of it
        //TODO: for now imma just leave it be and dont draw things on top of others
        if (can_be_active(madness_ui))
        {
            set_active(madness_ui, out_node->hash_id);
        }
    };


    //active state
    if (is_active(madness_ui, out_node->hash_id))
    {
        out_node->color = madness_ui->editor_style.pressed_color;
    }
    //hot state
    else if (is_hot(madness_ui, out_node->hash_id))
    {
        out_node->color = madness_ui->editor_style.hovered_color;
    }
    // normal state
    else
    {
        if (*check_box_state)
        {
            out_node->color = COLOR_BLACK;
        }
        else
        {
            out_node->color = COLOR_WHITE;
        }
    }

    if (use_ui_element(madness_ui, out_node->hash_id, out_pos_outline, out_size_outline))
    {
        //set the bool to its opposite
        *check_box_state = !(*check_box_state);
    }


    madness_ui_update_next_element_pos(madness_ui, (vec2){text_size.x + out_size_outline.x, out_size_outline.y});

    return *check_box_state;
}

float map_range(float v, float a, float b, float x, float y)
{
    return x + (v - a) * (y - x) / (b - a);
}

void madness_slider_scroll(Madness_UI* madness_ui, const char* id, float* slider_val, float min, float max)
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
    new_node->debug_id = id;
    new_node->hash_id = generate_hash_key_64bit((u8*)id, strlen(id));
    new_node->pos = ui_screen_pos;
    new_node->size = ui_screen_size;
    new_node->color = madness_ui->editor_style.color;

    //grab another
    UI_Node* slider_node = madness_ui_get_new_node(madness_ui);
    madness_ui->ui_nodes->num_items++;
    slider_node->debug_id = id;
    slider_node->hash_id = generate_hash_key_64bit((u8*)id, strlen(id));
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
    madness_ui_update_next_element_pos(madness_ui, ui_screen_size);
}

void madness_slider_arrow(Madness_UI* madness_ui, const char* id, float* slider_val, float min, float max)
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
    new_node->debug_id = id;
    new_node->hash_id = generate_hash_key_64bit((u8*)id, strlen(id));
    new_node->pos = ui_screen_pos;
    new_node->size = ui_screen_size;
    new_node->color = madness_ui->editor_style.layout_accent_color;

    // left arrow
    UI_Node* left_node = madness_ui_get_new_node(madness_ui);
    left_node->debug_id = id;
    left_node->hash_id = generate_hash_key_64bit((u8*)id, strlen(id));
    left_node->pos = ui_screen_pos;
    left_node->size = arrow_size;
    left_node->color = madness_ui->editor_style.color;

    // right arrow
    UI_Node* right_node = madness_ui_get_new_node(madness_ui);
    right_node->debug_id = id;
    right_node->hash_id = generate_hash_key_64bit((u8*)id, strlen(id));
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
    madness_ui_update_next_element_pos(madness_ui, ui_screen_size);
}


void madness_text_box(Madness_UI* madness_ui, const char* id)
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

    madness_text(madness_ui, id, STRING("Text Box:"));

    vec2 out_pos;
    vec2 out_size;
    UI_Node* quad_node = NULL;
    madness_draw_quad(madness_ui, "text box quad", &out_pos, &out_size, &quad_node);
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

    madness_ui_update_next_element_pos(madness_ui, out_size);
}

void madness_ui_float(Madness_UI* madness_ui, const char* id, float* f, float increment_value)
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
    madness_draw_quad(madness_ui, id, &out_pos, &out_size, &quad_node);
    out_size.x *= size_shink_value;
    quad_node->size.x *= size_shink_value;
    madness_draw_text_centered(madness_ui, float_string, out_pos, out_size);
    madness_ui_update_next_element_pos(madness_ui, out_size);

    //check if we are inside any of the squares
    if (region_hit(madness_ui, out_pos, out_size))
    {
        quad_node->color = madness_ui->editor_style.hovered_color;
        if (input_is_mouse_wheel_up(madness_ui->input_system_reference))
        {
            *f += increment_value;
        }
        if (input_is_mouse_wheel_down(madness_ui->input_system_reference))
        {
            *f -= increment_value;
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
            }
            if (mouse_change_x < 0)
            {
                *f -= increment_value / increment_smoother_value;
                // *f -= increment_override;
            }
        }
    }
}

void madness_ui_vec2(Madness_UI* madness_ui, const char* id, String text, vec2* v2, float increment_value)
{
    madness_text(madness_ui, id, text);

    UI_Layout_Direction last_layout_direction = madness_ui->layout_direction;
    madness_set_layout_direction(madness_ui, UI_LAYOUT_HORIZONTAL);

    const char* x_id = c_string_concat(id, "x", madness_ui->frame_arena);
    const char* y_id = c_string_concat(id, "y", madness_ui->frame_arena);

    madness_ui_float(madness_ui, x_id, &v2->x, increment_value);
    madness_ui_float(madness_ui, y_id, &v2->y, increment_value);

    madness_set_layout_direction(madness_ui, last_layout_direction);
}

void madness_ui_vec3(Madness_UI* madness_ui, const char* id, String text, vec3* v3, float increment_value)
{
    //draw text on top, then below the vec values
    madness_text(madness_ui, id, text);

    UI_Layout_Direction last_layout_direction = madness_ui->layout_direction;
    madness_set_layout_direction(madness_ui, UI_LAYOUT_HORIZONTAL);

    const char* x_id = c_string_concat(id, "x", madness_ui->frame_arena);
    const char* y_id = c_string_concat(id, "y", madness_ui->frame_arena);
    const char* z_id = c_string_concat(id, "z", madness_ui->frame_arena);

    madness_ui_float(madness_ui, x_id, &v3->x, increment_value);
    madness_ui_float(madness_ui, y_id, &v3->y, increment_value);
    madness_ui_float(madness_ui, z_id, &v3->z, increment_value);

    madness_set_layout_direction(madness_ui, last_layout_direction);
}

bool madness_ui_color_picker(Madness_UI* madness_ui, const char* id, vec3* color_value)
{
    madness_text(madness_ui, id, STRING("Color Picker"));

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

    madness_ui_update_next_element_pos(madness_ui, out_size);


    UI_Layout_Direction last_layout_direction = madness_ui->layout_direction;
    madness_set_layout_direction(madness_ui, UI_LAYOUT_HORIZONTAL);

    const char* x_id = c_string_concat(id, "x", madness_ui->frame_arena);
    const char* y_id = c_string_concat(id, "y", madness_ui->frame_arena);
    const char* z_id = c_string_concat(id, "z", madness_ui->frame_arena);

    float increment_value = 0.05;
    madness_ui_float(madness_ui, x_id, &color_value->x, increment_value);
    madness_ui_float(madness_ui, y_id, &color_value->y, increment_value);
    madness_ui_float(madness_ui, z_id, &color_value->z, increment_value);

    color_value->x = clamp_float(color_value->x, 0.0, 1.0);
    color_value->y = clamp_float(color_value->y, 0.0, 1.0);
    color_value->z = clamp_float(color_value->z, 0.0, 1.0);


    madness_set_layout_direction(madness_ui, last_layout_direction);


    return out_result;
}


void madness_scroll_box_begin(Madness_UI* madness_ui, const char* id, scroll_box_state* scroll_box_state)
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
    sroll_box_node->debug_id = id;
    sroll_box_node->pos = madness_ui->cursor_pos;


    madness_ui->parent_node_state.is_active = true;
    madness_ui->parent_node_state.current_attempt_count = 0;
    madness_ui->parent_node_state.slider_count = scroll_box_state->scroll_amount;
    madness_ui->parent_node_state.max_nodes = scroll_box_state->max_nodes_to_display;
}


void madness_scroll_box_end(Madness_UI* madness_ui, const char* id, scroll_box_state* scroll_box_state)
{
    madness_ui->parent_node_state.is_active = false;

    //get the parent node

    //last node should have a parent, which is the scroll box
    UI_Node* scroll_box = madness_ui_get_parent_node(madness_ui);

    float width_x = 0;
    float height_y = 0;

    for (u32 child_idx = 0; child_idx < scroll_box->child_node_count; child_idx++)
    {
        vec2 size = scroll_box->children[child_idx]->size;
        width_x = max_f(width_x, size.x);
        height_y += size.y + madness_ui->element_padding;
    }
    height_y -= madness_ui->element_padding;


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
    slider_node->debug_id = id;
    slider_node->hash_id = generate_hash_key_64bit((u8*)id, strlen(id));
    slider_node->pos = thumb_ui_screen_pos;
    slider_node->size = thumb_ui_screen_size;
    slider_node->color = madness_ui->editor_style.custom_widget_color;

    //size of the scroll box and the slider
    vec2 hit_size = (vec2){scroll_box_screen_size.x + thumb_ui_screen_size.x, scroll_box_screen_size.y};

    if (region_hit(madness_ui, button_screen_pos, hit_size))
    {
        //slider specific check
        if (region_hit(madness_ui, thumb_ui_screen_pos, (vec2){thumb_ui_screen_size.x, hit_size.y}))
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
    };
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

    madness_ui_begin_layout(madness_ui, "Madness UI Test Layout", (vec2){5, 5}, (vec2){20, 90});

    if (madness_button_text(madness_ui, "first button", STRING("these are buttons")))
    {
        FATAL("YOU HAVE PRESSED THE BUTTON OH LORD WHY!");
    };
    if (madness_button(madness_ui, "test button 2"))
    {
        FATAL("YOU HAVE PRESSED THE BUTTON OH LORD WHY!");
    }
    madness_button(madness_ui, "test button 3");
    madness_button(madness_ui, "test button 4;");
    {
        //if we want a layout change, specify so,
        madness_set_layout_direction(madness_ui, UI_LAYOUT_HORIZONTAL);
        static bool check_box_test;

        if (madness_check_box(madness_ui, "check_box", STRING("Check Box"), &check_box_test))
        {
            madness_ui->editor_style.text_color = COLOR_PURPLE_PALETTE_LIGHT;
            WARN("%d", check_box_test);
        }
        else
        {
            madness_ui->editor_style.text_color = COLOR_WHITE;
        }

        madness_check_box(madness_ui, "check_box2", STRING("Other"), &check_box_test);
        madness_check_box(madness_ui, "check_box3", STRING("Next"), &check_box_test);
    }

    madness_set_layout_direction(madness_ui, UI_LAYOUT_VERTICAL);
    madness_text(madness_ui, "text test", STRING("GOD DAMN IT BOBBY"));
    if (madness_button_text(madness_ui, "test button text", STRING("AND SO IT GOES")))
    {
        FATAL(" BUTTONS AND DEATH");
    }


    static float slider_val;
    float slider_min = 0;
    float slider_max = 1;
    madness_slider_scroll(madness_ui, "slider", &slider_val, slider_min, slider_max);

    madness_slider_arrow(madness_ui, "slider arrow", &slider_val, slider_min, slider_max);

    madness_text_box(madness_ui, "textbox");

    static vec3 vec3_test;
    float vec3_change_value = 10.5f;
    madness_ui_vec3(madness_ui, "Character position", STRING("Position"), &vec3_test, vec3_change_value);

    static vec2 vec2_test;
    madness_ui_vec2(madness_ui, "Sprite Pos", STRING("Sprite Position"), &vec2_test, vec3_change_value);

    static vec3 color_test;
    madness_ui_color_picker(madness_ui, "Color Picker", &color_test);


    static scroll_box_state scroll_box_state_test;
    scroll_box_state_test.max_nodes_to_display = 3;

    madness_scroll_box_begin(madness_ui, "scroll box", &scroll_box_state_test);
    {
        madness_button_text(madness_ui, "Scollbox Button 1", STRING("Scroll Around 1"));
        madness_button_text(madness_ui, "Scollbox Button 2", STRING("Scroll Around 2"));
        if (madness_button_text(madness_ui, "Scollbox Button 3", STRING("Scroll Around 3")))
        {
            FATAL("BUTTONS AND DEATH");
        }
        madness_button_text(madness_ui, "Scollbox Button 4", STRING("Scroll Around 4"));
        madness_button_text(madness_ui, "Scollbox Button 5", STRING("Scroll Around 5"));
        madness_button_text(madness_ui, "Scollbox Button 6", STRING("Scroll Around 6"));
    }
    madness_scroll_box_end(madness_ui, "scroll box", &scroll_box_state_test);
}


void madness_ui_generate_render_data(Madness_UI* madness_ui, Render_Packet* render_packet)
{
    f32 screen_width = madness_ui->screen_size.x;
    f32 screen_height = madness_ui->screen_size.y;

    //draw pass
    for (u32 i = 0; i < madness_ui->ui_nodes->num_items; i++)
    {
        UI_Node* node_to_draw = &madness_ui->ui_nodes->data[i];
        Sprite_Data* sprite_data = sprite_create_minimal(madness_ui->frame_arena);
        sprite_data->pos = vec2_div(node_to_draw->pos, madness_ui->screen_size);
        sprite_data->size = vec2_div(node_to_draw->size, madness_ui->screen_size);

        //TODO:
        // if (node_to_draw->flags & SPRITE_PIPELINE_COLOR)
        // {sprite_data->color = node_to_draw->color;}

        // if (node_to_draw->flags & SPRITE_PIPELINE_TEXTURE)
        // {
        // sprite_data->texture_index = node_to_draw->texture_handle.handle;
        // }
        sprite_data->color = node_to_draw->color;
        sprite_data->texture_index = node_to_draw->texture_handle.handle;
        sprite_data->flags = node_to_draw->flags;

        Sprite_Data_array_push(madness_ui->ui_data, sprite_data);

        /*
        if (node_to_draw->flags & SPRITE_PIPELINE_TEXT)
        {
            sprite_data->uv_offset = node_to_draw->uv_offset;
            sprite_data->uv_size = node_to_draw->uv_size;
           Sprite_Data_array_push(madness_ui->text_data, sprite_data);
        }
        */
    }


    for (u32 i = 0; i < madness_ui->ui_nodes_text->num_items; i++)
    {
        UI_Node_Text* node_to_draw = &madness_ui->ui_nodes_text->data[i];
        Sprite_Data* sprite_data = sprite_create_minimal(madness_ui->frame_arena);
        sprite_data->pos = vec2_div(node_to_draw->pos, madness_ui->screen_size);
        sprite_data->size = vec2_div(node_to_draw->size, madness_ui->screen_size);

        sprite_data->uv_offset = node_to_draw->uv_offset;
        sprite_data->uv_size = node_to_draw->uv_size;

        sprite_data->color = node_to_draw->color;
        sprite_data->texture_index = node_to_draw->texture_handle.handle;
        sprite_data->flags = node_to_draw->flags;

        Sprite_Data_array_push(madness_ui->text_data, sprite_data);
    }


    render_packet->ui_data_packet.ui_data_packet = madness_ui->ui_data;
    render_packet->ui_data_packet.text_data_packet = madness_ui->text_data;
    render_packet->ui_data_packet.text_index_type = madness_ui->index_type;
    render_packet->ui_data_packet.ui_index_type = madness_ui->index_type;
    render_packet->ui_data_packet.system_name = "MADNESS UI";
}

void madness_ui_generate_render_data_new(Madness_UI* madness_ui, Renderer* renderer)
{
    //draw pass
    for (u32 i = 0; i < madness_ui->ui_nodes->num_items; i++)
    {
        UI_Node* node_to_draw = &madness_ui->ui_nodes->data[i];
        Sprite_Data* sprite_data = sprite_system_get_new_sprite_transient(renderer->sprite_system);
        sprite_data->pos = vec2_div(node_to_draw->pos, madness_ui->screen_size);
        sprite_data->size = vec2_div(node_to_draw->size, madness_ui->screen_size);

        //TODO:
        // if (node_to_draw->flags & SPRITE_PIPELINE_COLOR)
        // {sprite_data->color = node_to_draw->color;}

        // if (node_to_draw->flags & SPRITE_PIPELINE_TEXTURE)
        // {
        // sprite_data->texture_index = node_to_draw->texture_handle.handle;
        // }
        sprite_data->color = node_to_draw->color;
        sprite_data->texture_index = node_to_draw->texture_handle.handle;
        sprite_data->flags = node_to_draw->flags;


        //if (node_to_draw->flags & SPRITE_PIPELINE_TEXT)
        //{
        //    sprite_data->uv_offset = node_to_draw->uv_offset;
        //    sprite_data->uv_size = node_to_draw->uv_size;
        //   Sprite_Data_array_push(madness_ui->text_data, sprite_data);
        //}
    }


    for (u32 i = 0; i < madness_ui->ui_nodes_text->num_items; i++)
    {
        UI_Node_Text* node_to_draw = &madness_ui->ui_nodes_text->data[i];
        Sprite_Data* sprite_data = sprite_system_get_new_sprite_transient(renderer->sprite_system);
        sprite_data->pos = vec2_div(node_to_draw->pos, madness_ui->screen_size);
        sprite_data->size = vec2_div(node_to_draw->size, madness_ui->screen_size);

        sprite_data->uv_offset = node_to_draw->uv_offset;
        sprite_data->uv_size = node_to_draw->uv_size;

        sprite_data->color = node_to_draw->color;
        sprite_data->texture_index = node_to_draw->texture_handle.handle;
        sprite_data->flags = node_to_draw->flags;

        Sprite_Data_array_push(madness_ui->text_data, sprite_data);
    }
}

UI_Renderer_Backend* ui_render_init(Renderer* renderer)
{
    UI_Renderer_Backend* UI_Render_Info = arena_alloc(&renderer->arena, sizeof(UI_Renderer_Backend));

    //TODO: i should replace this with object counts // like max 1000 UI's on screen until i need to otherwise
    u32 ui_buffer_sizes = MB(1);

    UI_Render_Info->ui_vertex_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                   BUFFER_TYPE_VERTEX, ui_buffer_sizes);
    UI_Render_Info->ui_index_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                  BUFFER_TYPE_INDEX, ui_buffer_sizes);
    UI_Render_Info->ui_indirect_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                     BUFFER_TYPE_INDIRECT, ui_buffer_sizes);
    UI_Render_Info->ui_instance_ssbo_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                   BUFFER_TYPE_CPU_STORAGE,
                                                                   ui_buffer_sizes);
    UI_Render_Info->text_vertex_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                     BUFFER_TYPE_VERTEX,
                                                                     ui_buffer_sizes);
    UI_Render_Info->text_index_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                    BUFFER_TYPE_INDEX,
                                                                    ui_buffer_sizes);
    UI_Render_Info->text_instance_ssbo_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                     BUFFER_TYPE_CPU_STORAGE,
                                                                     ui_buffer_sizes);
    UI_Render_Info->text_indirect_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                       BUFFER_TYPE_INDIRECT,
                                                                       ui_buffer_sizes);


    UI_Render_Info->ui_vertex_staging_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                           BUFFER_TYPE_STAGING, ui_buffer_sizes);
    UI_Render_Info->ui_index_staging_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                          BUFFER_TYPE_STAGING, ui_buffer_sizes);
    UI_Render_Info->ui_quad_indirect_staging_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
        BUFFER_TYPE_STAGING, ui_buffer_sizes);

    UI_Render_Info->ui_instance_staging_ssbo_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                           BUFFER_TYPE_STAGING,
                                                                           ui_buffer_sizes);


    UI_Render_Info->text_vertex_staging_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                             BUFFER_TYPE_STAGING,
                                                                             ui_buffer_sizes);
    UI_Render_Info->text_index_staging_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                            BUFFER_TYPE_STAGING,
                                                                            ui_buffer_sizes);
    UI_Render_Info->text_instance_staging_ssbo_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                             BUFFER_TYPE_STAGING,
                                                                             ui_buffer_sizes);
    UI_Render_Info->text_indirect_staging_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                               BUFFER_TYPE_STAGING,
                                                                               ui_buffer_sizes);
    return UI_Render_Info;
}


void madness_ui_renderer_upload_draw_data(UI_Renderer_Backend* ui_renderer, Renderer* renderer,
                                          Render_Packet* render_packet)
{
    // UI
    vulkan_buffer_reset_offset(renderer, ui_renderer->ui_vertex_staging_buffer_handle);
    vulkan_buffer_reset_offset(renderer, ui_renderer->ui_index_staging_buffer_handle);
    vulkan_buffer_reset_offset(renderer, ui_renderer->ui_quad_indirect_staging_buffer_handle);
    vulkan_buffer_reset_offset(renderer, ui_renderer->ui_instance_staging_ssbo_handle);
    //text
    vulkan_buffer_reset_offset(renderer, ui_renderer->text_vertex_staging_buffer_handle);
    vulkan_buffer_reset_offset(renderer, ui_renderer->text_index_staging_buffer_handle);
    vulkan_buffer_reset_offset(renderer, ui_renderer->text_indirect_staging_buffer_handle);
    vulkan_buffer_reset_offset(renderer, ui_renderer->text_instance_staging_ssbo_handle);

    //UI Render
    vulkan_buffer_data_copy_and_upload(renderer,
                                       ui_renderer->ui_vertex_buffer_handle,
                                       ui_renderer->ui_vertex_staging_buffer_handle,
                                       default_sprite,
                                       sizeof(Sprite) * 4);

    vulkan_buffer_data_copy_and_upload(renderer,
                                       ui_renderer->ui_index_buffer_handle,
                                       ui_renderer->ui_index_staging_buffer_handle,
                                       default_sprite_indices,
                                       sizeof(u16) * 6);
    //material data
    vulkan_buffer_data_copy_and_upload(renderer,
                                       ui_renderer->ui_instance_ssbo_handle,
                                       ui_renderer->ui_instance_staging_ssbo_handle,
                                       render_packet->ui_data_packet.ui_data_packet->data,
                                       Sprite_Data_array_get_bytes_used(
                                           render_packet->ui_data_packet.ui_data_packet));
    //literally only need one
    VkDrawIndexedIndirectCommand indirect_draw_ui;

    indirect_draw_ui.firstIndex = 0;
    indirect_draw_ui.firstInstance = 0;
    indirect_draw_ui.vertexOffset = 0; // one quad is 2 triangles / 6 vertex's
    indirect_draw_ui.indexCount = ARRAY_SIZE(default_sprite_indices);
    indirect_draw_ui.instanceCount = render_packet->ui_data_packet.ui_data_packet->num_items;

    vulkan_buffer_data_copy_and_upload(renderer,
                                       ui_renderer->ui_indirect_buffer_handle,
                                       ui_renderer->ui_quad_indirect_staging_buffer_handle,
                                       &indirect_draw_ui,
                                       sizeof(VkDrawIndexedIndirectCommand)
    );


    //TEXT Render

    vulkan_buffer_data_copy_and_upload(renderer,
                                       ui_renderer->text_vertex_buffer_handle,
                                       ui_renderer->text_vertex_staging_buffer_handle,
                                       default_sprite,
                                       sizeof(Sprite) * 4);

    vulkan_buffer_data_copy_and_upload(renderer,
                                       ui_renderer->text_index_buffer_handle,
                                       ui_renderer->text_index_staging_buffer_handle,
                                       default_sprite_indices,
                                       sizeof(u16) * 6);
    //material data
    vulkan_buffer_data_copy_and_upload(renderer,
                                       ui_renderer->text_instance_ssbo_handle,
                                       ui_renderer->text_instance_staging_ssbo_handle,
                                       render_packet->ui_data_packet.text_data_packet->data,
                                       Sprite_Data_array_get_bytes_used(
                                           render_packet->ui_data_packet.text_data_packet));

    //generate indirect draws for text
    //literally only need one
    VkDrawIndexedIndirectCommand indirect_draw_text;

    indirect_draw_text.firstIndex = 0;
    indirect_draw_text.firstInstance = 0;
    indirect_draw_text.vertexOffset = 0; // one quad is 2 triangles / 6 vertex's
    indirect_draw_text.indexCount = ARRAY_SIZE(default_sprite_indices);
    indirect_draw_text.instanceCount = render_packet->ui_data_packet.text_data_packet->num_items;


    vulkan_buffer_data_copy_and_upload(renderer,
                                       ui_renderer->text_indirect_buffer_handle,
                                       ui_renderer->text_indirect_staging_buffer_handle,
                                       &indirect_draw_text,
                                       sizeof(VkDrawIndexedIndirectCommand));
}

void ui_renderer_draw(UI_Renderer_Backend* ui_renderer, Renderer* renderer, vulkan_command_buffer* command_buffer,
                      Render_Packet* render_packet)
{
    //does the draw
    u64 ui_draw_count = render_packet->ui_data_packet.ui_data_packet->num_items;
    u64 text_draw_count = render_packet->ui_data_packet.text_data_packet->num_items;

    // UI
    vulkan_buffer* vert_buffer = vulkan_buffer_get(renderer, ui_renderer->ui_vertex_buffer_handle);
    vulkan_buffer* index_buffer = vulkan_buffer_get(renderer, ui_renderer->ui_index_buffer_handle);
    vulkan_buffer* quad_indirect_buffer = vulkan_buffer_get(renderer, ui_renderer->ui_indirect_buffer_handle);


    vkCmdBindPipeline(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      renderer->ui_pipeline.handle);

    //uniform
    vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            renderer->ui_pipeline.pipeline_layout, 0, 1,
                            &renderer->descriptor_system->uniform_descriptors.descriptor_sets[renderer->context.
                                current_frame], 0, 0);

    //textures
    vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            renderer->ui_pipeline.pipeline_layout, 1, 1,
                            &renderer->descriptor_system->texture_descriptors.descriptor_sets[renderer->context.
                                current_frame], 0, 0);

    //storage buffers
    vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            renderer->ui_pipeline.pipeline_layout, 2, 1,
                            &renderer->descriptor_system->storage_descriptors.descriptor_sets[renderer->context.
                                current_frame], 0, 0);


    //grab material_handle
    PC_2D pc_2d_ui = {
        ui_renderer->ui_instance_ssbo_handle.handle,
    };

    VkPushConstantsInfo push_constant_info_ui = {0};
    push_constant_info_ui.sType = VK_STRUCTURE_TYPE_PUSH_CONSTANTS_INFO;
    push_constant_info_ui.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    push_constant_info_ui.layout = renderer->ui_pipeline.pipeline_layout;
    push_constant_info_ui.offset = 0;
    push_constant_info_ui.size = sizeof(PC_2D);
    push_constant_info_ui.pValues = &pc_2d_ui;
    push_constant_info_ui.pNext = NULL;

    vkCmdPushConstants2(command_buffer->handle, &push_constant_info_ui);


    VkDeviceSize offsets_bindless[1] = {0};
    vkCmdBindVertexBuffers(command_buffer->handle, 0, 1,
                           &vert_buffer->handle, offsets_bindless);

    vkCmdBindIndexBuffer(command_buffer->handle,
                         index_buffer->handle, 0,
                         VK_INDEX_TYPE_UINT16
    );

    // vkCmdDrawIndexed(command_buffer->handle,
    // Madness_UI->draw_info.index_count,
    // 1, 0, 0, 0);
    if (renderer->context.device.features.multiDrawIndirect)
    {
        vkCmdDrawIndexedIndirect(command_buffer->handle,
                                 quad_indirect_buffer->handle, 0,
                                 1,
                                 sizeof(VkDrawIndexedIndirectCommand));
    }
    else
    {
        // If multi draw is not available, we must issue separate draw commands
        for (u64 j = 0; j < 1; j++)
        {
            vkCmdDrawIndexedIndirect(command_buffer->handle,
                                     quad_indirect_buffer->handle,
                                     j * sizeof(VkDrawIndexedIndirectCommand), 1,
                                     sizeof(VkDrawIndexedIndirectCommand));
        }
    }

    // vkCmdDrawIndexed(command_buffer->handle,
    // Madness_UI->quad_draw_info.index_count,
    // 1, 0, 0, 0);


    //TEXT
    vulkan_buffer* text_vert_buffer = vulkan_buffer_get(renderer, ui_renderer->text_vertex_buffer_handle);
    vulkan_buffer* text_index_buffer = vulkan_buffer_get(renderer, ui_renderer->text_index_buffer_handle);
    vulkan_buffer* text_indirect = vulkan_buffer_get(renderer, ui_renderer->text_indirect_buffer_handle);


    vkCmdBindPipeline(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      renderer->text_pipeline.handle);

    //uniform
    vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            renderer->text_pipeline.pipeline_layout, 0, 1,
                            &renderer->descriptor_system->uniform_descriptors.descriptor_sets[renderer->context.
                                current_frame], 0, 0);

    //textures
    vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            renderer->text_pipeline.pipeline_layout, 1, 1,
                            &renderer->descriptor_system->texture_descriptors.descriptor_sets[renderer->context.
                                current_frame], 0, 0);

    //storage buffers
    vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            renderer->text_pipeline.pipeline_layout, 2, 1,
                            &renderer->descriptor_system->storage_descriptors.descriptor_sets[renderer->context.
                                current_frame], 0, 0);


    //grab material_handle
    PC_2D pc_2d_text = {
        ui_renderer->text_instance_ssbo_handle.handle,
    };

    VkPushConstantsInfo push_constant_info_text = {0};
    push_constant_info_text.sType = VK_STRUCTURE_TYPE_PUSH_CONSTANTS_INFO;
    push_constant_info_text.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    push_constant_info_text.layout = renderer->text_pipeline.pipeline_layout;
    push_constant_info_text.offset = 0;
    push_constant_info_text.size = sizeof(PC_2D);
    push_constant_info_text.pValues = &pc_2d_text;
    push_constant_info_text.pNext = NULL;

    vkCmdPushConstants2(command_buffer->handle, &push_constant_info_text);

    vkCmdBindVertexBuffers(command_buffer->handle, 0, 1,
                           &text_vert_buffer->handle, offsets_bindless);

    vkCmdBindIndexBuffer(command_buffer->handle,
                         text_index_buffer->handle, 0,
                         VK_INDEX_TYPE_UINT16);

    if (renderer->context.device.features.multiDrawIndirect)
    {
        vkCmdDrawIndexedIndirect(command_buffer->handle,
                                 text_indirect->handle, 0,
                                 1,
                                 sizeof(VkDrawIndexedIndirectCommand));
    }
    else
    {
        // If multi draw is not available, we must issue separate draw commands
        for (u64 j = 0; j < 1; j++)
        {
            vkCmdDrawIndexedIndirect(command_buffer->handle,
                                     text_indirect->handle,
                                     j * sizeof(VkDrawIndexedIndirectCommand), 1,
                                     sizeof(VkDrawIndexedIndirectCommand));
        }
    }
}
