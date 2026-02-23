#include "UI.h"

#include "logger.h"


bool madness_ui_init(renderer* renderer)
{
    Madness_UI = arena_alloc(&renderer->arena, sizeof(Madness_UI_Context));
    memset(Madness_UI, 0, (sizeof(Madness_UI_Context)));

    //TODO: TEMPORARY, request memory from the memory_system
    Madness_UI->arena = &renderer->arena;
    Madness_UI->frame_arena = &renderer->frame_arena;

    Madness_UI->quad_draw_info.index_type = VK_INDEX_TYPE_UINT16;
    Madness_UI->text_draw_info.index_type = VK_INDEX_TYPE_UINT16;

    Madness_UI->text_draw_info.text_material_param_current_size = 0;

    Madness_UI->default_font_size = DEFAULT_FONT_CREATION_SIZE;

    Madness_UI->ui_stack_count = 0;
    Madness_UI->ui_stack_capacity = MAX_UI_NODE_COUNT;
    Madness_UI->ui_stack = arena_alloc(Madness_UI->arena, sizeof(UI_Node*) * MAX_UI_NODE_COUNT);
    Madness_UI->ui_nodes = UI_Node_array_create(MAX_UI_NODE_COUNT);


    // for (u64 ui_node_index = 0; ui_node_index < MAX_UI_NODE_COUNT; ui_node_index++)
    // {
    //     UI_Node* node = arena_alloc(Madness_UI->arena, sizeof(UI_Node));
    //     Madness_UI->ui_stack[ui_node_index] = node;
    //     Madness_UI->ui_nodes_array[ui_node_index] = node;
    // }

    Madness_UI->active.ID = -1;
    Madness_UI->active.layer = -1;
    Madness_UI->hot.ID = -1;
    Madness_UI->hot.layer = -1;


    Madness_UI->id_generation_number = -1;

    Madness_UI->mouse_pos_x = -1.0f;
    Madness_UI->mouse_pos_y = -1.0f;

    Madness_UI->mouse_down = 0;
    Madness_UI->mouse_released = 0;

    //TODO: replace with an in param
    Madness_UI->screen_size = (vec2){800.0f, 600.0f};


    //TODO: i should replace this with object counts // like max 1000 UI's on screen until i need to otherwise
    u32 ui_buffer_sizes = MB(1);

    Madness_UI->ui_quad_vertex_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                   BUFFER_TYPE_VERTEX, ui_buffer_sizes);
    Madness_UI->ui_quad_index_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                  BUFFER_TYPE_INDEX, ui_buffer_sizes);
    Madness_UI->ui_quad_indirect_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                     BUFFER_TYPE_INDIRECT, ui_buffer_sizes);
    Madness_UI->text_vertex_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system, BUFFER_TYPE_VERTEX,
                                                                ui_buffer_sizes);
    Madness_UI->text_index_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system, BUFFER_TYPE_INDEX,
                                                               ui_buffer_sizes);
    Madness_UI->text_material_ssbo_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                BUFFER_TYPE_CPU_STORAGE,
                                                                ui_buffer_sizes);
    Madness_UI->text_indirect_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                  BUFFER_TYPE_INDIRECT,
                                                                  ui_buffer_sizes);


    Madness_UI->ui_quad_vertex_staging_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                           BUFFER_TYPE_STAGING, ui_buffer_sizes);
    Madness_UI->ui_quad_index_staging_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                          BUFFER_TYPE_STAGING, ui_buffer_sizes);
    Madness_UI->ui_quad_indirect_staging_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                             BUFFER_TYPE_STAGING, ui_buffer_sizes);

    Madness_UI->text_vertex_staging_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                        BUFFER_TYPE_STAGING,
                                                                        ui_buffer_sizes);
    Madness_UI->text_index_staging_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                       BUFFER_TYPE_STAGING,
                                                                       ui_buffer_sizes);
    Madness_UI->text_material_staging_ssbo_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                        BUFFER_TYPE_STAGING,
                                                                        ui_buffer_sizes);
    Madness_UI->text_indirect_staging_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                          BUFFER_TYPE_STAGING,
                                                                          ui_buffer_sizes);

    //TODO: THERE IS NO WAY I AM LOADING THIS FILE FROM THERE, WHAT ABOUT LINUX
    font_init(renderer, "c:/windows/fonts/arialbd.ttf");

    INFO("UI SYSTEM CREATED");
    return Madness_UI;
}

void madness_ui_begin(i32 screen_size_x, i32 screen_size_y)
{
    //clear draw info and reset the hot id

    //std::cout << "MOUSE STATE:" << Madness_UI.mouse_down << '\n';

    Madness_UI->screen_size.x = screen_size_x;
    Madness_UI->screen_size.y = screen_size_y;

    //TODO: clear vertex and index data for UI and TEXT
    // clear_vertex_info(Madness_UI->draw_info.vertex_info);
    // Madness_UI->draw_info.UI_Objects.clear();
    Madness_UI->quad_draw_info.quad_vertex_bytes = 0;
    Madness_UI->quad_draw_info.index_bytes = 0;
    Madness_UI->quad_draw_info.index_count = 0;
    Madness_UI->quad_draw_info.quad_draw_count = 0;


    for (u64 ui_node_index = 0; ui_node_index < Madness_UI->ui_nodes->num_items; ui_node_index++)
    {
        Madness_UI->ui_nodes->data[ui_node_index].children_length = 0;
    }
    UI_Node_array_zero(Madness_UI->ui_nodes);
    UI_Node_array_clear(Madness_UI->ui_nodes);

    Madness_UI->ui_stack_count = 0;

    Madness_UI->text_draw_info.text_vertex_bytes = 0;
    Madness_UI->text_draw_info.text_index_bytes = 0;
    Madness_UI->text_draw_info.text_index_count = 0;
    Madness_UI->text_draw_info.text_material_param_current_size = 0;
    Madness_UI->text_draw_info.text_draw_count = 0;


    Madness_UI->hot.ID = -1;
    Madness_UI->hot.ID = -1;
    Madness_UI->hot.layer = -1;
}

void madness_ui_end(void)
{
    //check if mouse is released, if so reset the active id
    //also update the mouse state

    //printf("HOT ID: %d, HOT LAYER: %d\n", Madness_UI->hot.ID, Madness_UI->hot.layer);
    //printf("ACTIVE ID: %d, ACTIVE LAYER: %d\n", Madness_UI->active.ID, Madness_UI->active.layer);

    if (input_is_mouse_button_released(MOUSE_BUTTON_LEFT))
    {
        //std::cout << "mouse released\n";
        Madness_UI->active.ID = -1;
    }


    //update mouse state
    Madness_UI->mouse_down = input_is_mouse_button_pressed(MOUSE_BUTTON_LEFT);
    // DEBUG("MOUSE DOWN %d", Madness_UI->mouse_down)
    //update mouse pos
    update_ui_mouse_pos();
}

Font_Handle font_init(renderer* renderer, const char* filepath)
{
    //TODO: rn only loads the default font, should get an available font at startup
    Madness_Font* font_structure = &Madness_UI->default_font;

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
    float scale = stbtt_ScaleForPixelHeight(&font_structure->font_info, Madness_UI->default_font_size);
    int atlasWidth = 1024 * 4;
    int atlasHeight = 1024 * 4;
    char* atlasPixels = arena_alloc(Madness_UI->arena, atlasWidth * atlasHeight);

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
    unsigned char* atlas_RGBA = arena_alloc(Madness_UI->arena, atlasRGBA_size);
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
    Madness_UI->default_font.font_texture_handle = shader_system_add_texture_font(
        renderer, renderer->shader_system, atlas_RGBA, atlasWidth, atlasHeight);
    /*update_texture_bindless_descriptor_set(renderer,
                                           renderer->descriptor_system,
                                           Madness_UI->default_font.font_texture_handle);*/

    // create_texture_glyph(renderer, renderer->context.graphics_command_buffer, &font_structure->font_texture,
    // atlas_RGBA, atlasWidth, atlasHeight);

    DEBUG("Font loaded successfully: %s\n", filepath);

    return out_handle;
}

void madness_ui_upload_draw_data(renderer* renderer)
{
    // UI Quad
    vulkan_buffer_reset_offset(renderer, Madness_UI->ui_quad_vertex_staging_buffer_handle);
    vulkan_buffer_reset_offset(renderer, Madness_UI->ui_quad_index_staging_buffer_handle);
    vulkan_buffer_reset_offset(renderer, Madness_UI->ui_quad_indirect_staging_buffer_handle);
    //text
    vulkan_buffer_reset_offset(renderer, Madness_UI->text_vertex_staging_buffer_handle);
    vulkan_buffer_reset_offset(renderer, Madness_UI->text_index_staging_buffer_handle);
    vulkan_buffer_reset_offset(renderer, Madness_UI->text_indirect_staging_buffer_handle);
    vulkan_buffer_reset_offset(renderer, Madness_UI->text_material_staging_ssbo_handle);

    //UI Render
    vulkan_buffer_data_copy_and_upload(renderer,
                                       Madness_UI->ui_quad_vertex_buffer_handle,
                                       Madness_UI->ui_quad_vertex_staging_buffer_handle,
                                       Madness_UI->quad_draw_info.quad_vertex,
                                       Madness_UI->quad_draw_info.quad_vertex_bytes);

    vulkan_buffer_data_copy_and_upload(renderer,
                                       Madness_UI->ui_quad_index_buffer_handle,
                                       Madness_UI->ui_quad_index_staging_buffer_handle,
                                       Madness_UI->quad_draw_info.indices,
                                       Madness_UI->quad_draw_info.index_bytes);

    //generate indirect draws for ui
    VkDrawIndexedIndirectCommand* indirect_draw_ui = arena_alloc(Madness_UI->frame_arena,
                                                                 sizeof(VkDrawIndexedIndirectCommand) * Madness_UI->
                                                                 text_draw_info.text_draw_count);
    for (u32 i = 0; i < Madness_UI->quad_draw_info.quad_draw_count; i++)
    {
        //since we know that quads are always 6 indices and 4 vec2 coordinates, this can easily be calculated
        u32 index_count = ARRAY_SIZE(default_quad_indices);
        indirect_draw_ui[i].firstIndex = i * index_count; // total up to this point
        indirect_draw_ui[i].firstInstance = 0;
        indirect_draw_ui[i].indexCount = index_count; // always 6
        indirect_draw_ui[i].instanceCount = 1;
        indirect_draw_ui[i].vertexOffset = i * 4; // count for every quad we wish to draw which is 4
        // indirect_draw->vertexOffset = vertex_byte_count / sizeof(vec3); // total up to this point
    }

    vulkan_buffer_data_copy_and_upload(renderer,
                                       Madness_UI->ui_quad_indirect_buffer_handle,
                                       Madness_UI->ui_quad_indirect_staging_buffer_handle,
                                       indirect_draw_ui,
                                       sizeof(VkDrawIndexedIndirectCommand) * Madness_UI->quad_draw_info.quad_draw_count
    );


    //TEXT Render
    vulkan_buffer_data_copy_and_upload(renderer,
                                       Madness_UI->text_vertex_buffer_handle,
                                       Madness_UI->text_vertex_staging_buffer_handle,
                                       Madness_UI->text_draw_info.text_vertex,
                                       Madness_UI->text_draw_info.text_vertex_bytes);

    vulkan_buffer_data_copy_and_upload(renderer,
                                       Madness_UI->text_index_buffer_handle,
                                       Madness_UI->text_index_staging_buffer_handle,
                                       Madness_UI->text_draw_info.text_indices,
                                       Madness_UI->text_draw_info.text_index_bytes);

    vulkan_buffer_data_copy_and_upload(renderer,
                                       Madness_UI->text_material_ssbo_handle,
                                       Madness_UI->text_material_staging_ssbo_handle,
                                       Madness_UI->text_draw_info.text_material_params,
                                       Madness_UI->text_draw_info.text_material_param_current_size * sizeof(
                                           Material_2D_Param_Data));

    //generate indirect draws for tex
    VkDrawIndexedIndirectCommand* indirect_draw_text = arena_alloc(Madness_UI->frame_arena,
                                                                   sizeof(VkDrawIndexedIndirectCommand) * Madness_UI->
                                                                   text_draw_info.text_draw_count);
    for (u32 i = 0; i < Madness_UI->text_draw_info.text_draw_count; i++)
    {
        //since we know that quads are always 6 indices and 4 vec2 coordinates, this can easily be calculated
        u32 index_count = ARRAY_SIZE(default_quad_indices);
        indirect_draw_text[i].firstIndex = i * index_count; // total up to this point
        indirect_draw_text[i].firstInstance = 0;
        indirect_draw_text[i].indexCount = index_count; // always 6
        indirect_draw_text[i].instanceCount = 1;
        indirect_draw_text[i].vertexOffset = i * 4; // count for every quad we wish to draw which is 4
        // indirect_draw->vertexOffset = vertex_byte_count / sizeof(vec3); // total up to this point
    }

    vulkan_buffer_data_copy_and_upload(renderer,
                                       Madness_UI->text_indirect_buffer_handle,
                                       Madness_UI->text_indirect_staging_buffer_handle,
                                       indirect_draw_text,
                                       sizeof(VkDrawIndexedIndirectCommand) * Madness_UI->text_draw_info.
                                       text_draw_count);
}




void update_ui_mouse_pos(void)
{
    input_get_mouse_pos(&Madness_UI->mouse_pos_x, &Madness_UI->mouse_pos_y);
}


bool is_ui_hot(int id)
{
    return Madness_UI->hot.ID && Madness_UI->hot.ID == id;
}

bool is_ui_active(int id)
{
    return Madness_UI->active.ID && Madness_UI->active.ID == id;
}


bool region_hit(vec2 pos, vec2 size)
{
    //check if we are inside a ui_object

    // bottom left
    if (pos.x - size.x > Madness_UI->mouse_pos_x) return false;
    if (pos.y - size.y > Madness_UI->mouse_pos_y) return false;

    // top left
    if (pos.x - size.x > Madness_UI->mouse_pos_x) return false;
    if (pos.y + size.y < Madness_UI->mouse_pos_y) return false;

    // top right
    if (pos.x + size.x < Madness_UI->mouse_pos_x)return false;
    if (pos.y + size.y < Madness_UI->mouse_pos_y) return false;

    // bottom right
    if (pos.x + size.x < Madness_UI->mouse_pos_x) return false;
    if (pos.y - size.y > Madness_UI->mouse_pos_y) return false;

    return true;
}

bool region_hit_new(vec2 pos, vec2 size)
{
    //check if we are inside a ui_object

    //printf("DEBUG REGION HIT: MOUSE: %f, %f POS: %f, %f SIZE: %f, %f\n", Madness_UI->mouse_pos.x, Madness_UI->mouse_pos.y, pos.x, pos.y, size.x, size.y);

    //top left
    if (pos.x > Madness_UI->mouse_pos_x) return false;
    if (pos.y > Madness_UI->mouse_pos_y) return false;


    // bottom left
    if (pos.x > Madness_UI->mouse_pos_x) return false;
    if (pos.y + size.y < Madness_UI->mouse_pos_y) return false;


    //top right
    if (pos.x + size.x < Madness_UI->mouse_pos_x) return false;
    if (pos.y > Madness_UI->mouse_pos_y) return false;

    // bottom right
    if (pos.x + size.x < Madness_UI->mouse_pos_x) return false;
    if (pos.y + size.y < Madness_UI->mouse_pos_y) return false;


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
bool use_button(UI_ID id, vec2 pos, vec2 size)
{
    //checking if we released the mouse button, are active, and we are inside the hit region
    if (Madness_UI->mouse_down == 0 &&
        Madness_UI->active.ID == id.ID &&
        region_hit(pos, size))
        return true;

    return false;
}

bool use_button_new(UI_ID id, vec2 pos, vec2 size)
{
    //checking if we released the mouse button, are active, and we are inside the hit region
    if (Madness_UI->mouse_down == 0 &&
        Madness_UI->active.ID == id.ID &&
        region_hit_new(pos, size))
        return true;

    return false;
}

int generate_id(void)
{
    return Madness_UI->id_generation_number++;
}

void set_hot(UI_ID id)
{
    if (Madness_UI->hot.layer <= id.layer)
    {
        Madness_UI->hot.ID = id.ID;
        Madness_UI->hot.layer = id.layer;
        //printf("ID: %d, is hot\n", id);
    }
}

void set_active(UI_ID id)
{
    if (Madness_UI->active.layer <= id.layer)
    {
        Madness_UI->active.ID = id.ID;
        Madness_UI->active.layer = id.layer;
        //printf("ID: %d, is active\n", id);
    }
}

bool can_be_active(void)
{
    return Madness_UI->active.ID == -1 && Madness_UI->mouse_down;
}

bool is_active(UI_ID id)
{
    return Madness_UI->active.ID == id.ID;
}

bool is_hot(UI_ID id)
{
    return Madness_UI->hot.ID == id.ID;
}

void madness_ui_add_quad_vertex(Quad_Vertex* new_quad)
{
    // Add vertices
    memcpy((u8*)Madness_UI->quad_draw_info.quad_vertex + Madness_UI->quad_draw_info.quad_vertex_bytes, new_quad,
           sizeof(Quad_Vertex) * 4);

    // create indices (two triangles per quad)
    memcpy((u8*)Madness_UI->quad_draw_info.indices + Madness_UI->quad_draw_info.index_bytes, default_quad_indices,
           sizeof(default_quad_indices));

    //increase by count
    Madness_UI->quad_draw_info.quad_vertex_bytes += sizeof(Quad_Vertex) * 4;
    //increase by bytes
    Madness_UI->quad_draw_info.index_bytes += sizeof(default_quad_indices);
    Madness_UI->quad_draw_info.index_count += ARRAY_SIZE(default_quad_indices);
    Madness_UI->quad_draw_info.quad_draw_count += 1;
}


bool do_button(UI_ID id, vec2 pos, vec2 screen_percentage,
               vec3 color, vec3 hovered_color, vec3 pressed_color)
{
    /*validation*/

    //TODO: make these asserts
    if (screen_percentage.x > 100 || screen_percentage.x < 0 || screen_percentage.y > 100 || screen_percentage.y < 0)
    {
        WARN("SCREEN PERCENTAGE CANNOT BE GREATER THAN 100 AND LESS THAN 0");
        return false;
    }
    if (pos.x > 100 || pos.x < 0 || pos.y > 100 || pos.y < 0)
    {
        WARN("POSITION CANNOT BE GREATER THAN 100 AND LESS THAN 0");
        return false;
    }

    /*POS AND SIZE CALCULATIONS*/

    vec2 converted_pos = vec2_div_scalar(pos, 100.0f);
    vec2 converted_size = vec2_div_scalar(screen_percentage, 100.0f);

    // vec2 converted_pos2 = vec2_mul(pos, Madness_UI->screen_size);
    // vec2 converted_pos3 = vec2_div(pos, Madness_UI->screen_size);

    /*
    vec2 final_pos = {
        Madness_UI->screen_size.x * converted_pos.x,
        Madness_UI->screen_size.y * converted_pos.y
    };
    vec2 final_size = {
        (Madness_UI->screen_size.x * converted_size.x) / 2,
        (Madness_UI->screen_size.y * converted_size.y) / 2
    };*/

    vec2 final_pos = {
        converted_pos.x,
        converted_pos.y
    };
    vec2 final_size = {
        converted_size.x / 2,
        converted_size.y / 2,
    };

    vec2 mouse_hit_pos = {
        Madness_UI->screen_size.x * converted_pos.x,
        Madness_UI->screen_size.y * converted_pos.y
    };
    vec2 mouse_hit_size = {
        Madness_UI->screen_size.x * converted_size.x / 2,
        Madness_UI->screen_size.y * converted_size.y / 2,
    };


    /*SET UI STATE*/

    int mesh_id = Madness_UI->id_generation_number++;

    //check if button is hot and active
    if (region_hit(mouse_hit_pos, mouse_hit_size))
    {
        set_hot(id);

        //check if we have the mouse pressed and nothing else is selected
        //TODO: so there is a bug with can_be_active, in that the first ui called on the screen will take active focus,
        //TODO: this is despite there bieng another ui in front of it
        //TODO: for now imma just leave it be and dont draw things on top of others
        if (can_be_active())
        {
            set_active(id);
        }
    }
    /*
    else
    {
        printf("is not hot\n");
    }*/


    /*SET DRAW COLOR Based on the state*/
    Quad_Vertex* new_quad;
    // = arena_alloc(Madness_UI->frame_arena, sizeof(Quad_Vertex) * 4);

    //set color
    //active state
    if (is_active(id))
    {
        new_quad = quad_create_screen_percentage(Madness_UI->frame_arena, final_pos, final_size, pressed_color);
    }
    //hot state
    else if (is_hot(id))
    {
        new_quad = quad_create_screen_percentage(Madness_UI->frame_arena, final_pos, final_size, hovered_color);
    }
    // normal state
    else
    {
        new_quad = quad_create_screen_percentage(Madness_UI->frame_arena, final_pos, final_size, color);
    }


    /*SET DRAW INFO*/
    madness_ui_add_quad_vertex(new_quad);

    //check if we clicked the button
    if (use_button_new(id, final_pos, final_size)) return true;

    //check if we clicked the button
    if (use_button(id, final_pos, final_size)) return true;

    return false;
}

bool do_button_config(UI_ID id, UI_Config ui_config)
{
    /*validation*/

    //TODO: make these asserts
    if (ui_config.size.x > 100 || ui_config.size.x < 0 || ui_config.size.y > 100 || ui_config.size.y < 0)
    {
        WARN("SCREEN PERCENTAGE CANNOT BE GREATER THAN 100 AND LESS THAN 0");
        return false;
    }
    if (ui_config.pos.x > 100 || ui_config.pos.x < 0 || ui_config.pos.y > 100 || ui_config.pos.y < 0)
    {
        WARN("POSITION CANNOT BE GREATER THAN 100 AND LESS THAN 0");
        return false;
    }

    /*POS AND SIZE CALCULATIONS*/

    vec2 converted_pos = vec2_div_scalar(ui_config.pos, 100.0f);
    vec2 converted_size = vec2_div_scalar(ui_config.size, 100.0f);

    // vec2 converted_pos2 = vec2_mul(pos, Madness_UI->screen_size);
    // vec2 converted_pos3 = vec2_div(pos, Madness_UI->screen_size);

    /*
    vec2 final_pos = {
        Madness_UI->screen_size.x * converted_pos.x,
        Madness_UI->screen_size.y * converted_pos.y
    };
    vec2 final_size = {
        (Madness_UI->screen_size.x * converted_size.x) / 2,
        (Madness_UI->screen_size.y * converted_size.y) / 2
    };*/

    vec2 final_pos = {
        converted_pos.x,
        converted_pos.y
    };
    vec2 final_size = {
        converted_size.x / 2,
        converted_size.y / 2,
    };

    vec2 mouse_hit_pos = {
        Madness_UI->screen_size.x * converted_pos.x,
        Madness_UI->screen_size.y * converted_pos.y
    };
    vec2 mouse_hit_size = {
        Madness_UI->screen_size.x * converted_size.x / 2,
        Madness_UI->screen_size.y * converted_size.y / 2,
    };


    /*SET UI STATE*/

    int mesh_id = Madness_UI->id_generation_number++;

    //check if button is hot and active
    if (region_hit(mouse_hit_pos, mouse_hit_size))
    {
        set_hot(id);

        //check if we have the mouse pressed and nothing else is selected
        //TODO: so there is a bug with can_be_active, in that the first ui called on the screen will take active focus,
        //TODO: this is despite there bieng another ui in front of it
        //TODO: for now imma just leave it be and dont draw things on top of others
        if (can_be_active())
        {
            set_active(id);
        }
    }
    /*
    else
    {
        printf("is not hot\n");
    }*/


    /*SET DRAW COLOR Based on the state*/
    Quad_Vertex* new_quad;
    // = arena_alloc(Madness_UI->frame_arena, sizeof(Quad_Vertex) * 4);

    //set color
    //active state
    if (is_active(id))
    {
        new_quad = quad_create_screen_percentage(Madness_UI->frame_arena, final_pos, final_size,
                                                 ui_config.pressed_color);
    }
    //hot state
    else if (is_hot(id))
    {
        new_quad = quad_create_screen_percentage(Madness_UI->frame_arena, final_pos, final_size,
                                                 ui_config.hovered_color);
    }
    // normal state
    else
    {
        new_quad = quad_create_screen_percentage(Madness_UI->frame_arena, final_pos, final_size, ui_config.color);
    }


    /*SET DRAW INFO*/

    // Add vertices
    memcpy((u8*)Madness_UI->quad_draw_info.quad_vertex + Madness_UI->quad_draw_info.quad_vertex_bytes, new_quad,
           sizeof(Quad_Vertex) * 4);

    // create indices (two triangles per quad)
    memcpy((u8*)Madness_UI->quad_draw_info.indices + Madness_UI->quad_draw_info.index_bytes, default_quad_indices,
           sizeof(default_quad_indices));

    //increase by count
    Madness_UI->quad_draw_info.quad_vertex_bytes += sizeof(Quad_Vertex) * 4;
    //increase by bytes
    Madness_UI->quad_draw_info.index_bytes += sizeof(default_quad_indices);
    Madness_UI->quad_draw_info.index_count += ARRAY_SIZE(default_quad_indices);
    Madness_UI->quad_draw_info.quad_draw_count += 1;

    //check if we clicked the button
    if (use_button_new(id, final_pos, final_size)) return true;

    //check if we clicked the button
    if (use_button(id, final_pos, final_size)) return true;

    return false;
}


bool do_button_text(UI_ID id, String text, vec2 pos, vec2 size,
                    vec2 text_padding, vec3 color, vec3 hovered_color, vec3 pressed_color)
{
    // do_text();
    // if (do_button()) return true;

    return false;
}


void do_text(String text, vec2 pos, vec2 screen_percentage_size,
             vec3 color, float font_size)
{
    //tells us how much of the screen we want based on the size percentage
    if (screen_percentage_size.x > 100 || screen_percentage_size.x < 0 || screen_percentage_size.y > 100 ||
        screen_percentage_size.y < 0)
    {
        printf("YOU ARE STUPID: TEXT");
    }
    if (pos.x > 100 || pos.x < 0 || pos.y > 100 || pos.y < 0)
    {
        printf("YOU ARE STUPID 2: TEXT");
    }

    //std::vector<Vertex_Text> new_quad = text_create_quad({0.5f,0.5f}, {0.1f,0.1f}, {1.0f,1.0f,1.0f});


    //move to the shader as a push constant
    vec2 screen_dimensions = {Madness_UI->screen_size.x, Madness_UI->screen_size.y};

    float screen_width = Madness_UI->screen_size.x;
    float screen_height = Madness_UI->screen_size.y;

    //convert 0-100 -> 0-1
    vec2 converted_pos = vec2_div_scalar(pos, 100.0f);
    vec2 converted_size = vec2_div_scalar(screen_percentage_size, 100.0f);

    /*POS AND SIZE CALCULATIONS*/
    //0 - n(screen size) positions
    vec2 screen_position = {
        converted_pos.x * screen_dimensions.x,
        converted_pos.y * screen_dimensions.y
    };
    vec2 screen_size = {
        converted_size.x * screen_width / 2,
        converted_size.y * screen_height / 2,
    };


    //We take the desired font size, scale it down proportional to the font size we created it at
    //final size of the font ex: 36/48 = 0.75, 48*0.75 = 36
    f32 font_scalar = font_size / Madness_UI->default_font_size;

    for (u64 i = 0; i < text.length + 1; i++)
    // for (const char& c : text)
    {
        const char c = text.chars[i];

        if (c < 32 || c >= 128) continue; // skip unsupported characters
        Glyph* g = &Madness_UI->default_font.glyphs[c - 32];

        // Quad position in screen coords nand scaled by the font scalar
        f32 x_position = screen_position.x + ((float)g->xoff * font_scalar);
        f32 y_position = screen_position.y + ((float)g->yoff * font_scalar);

        f32 x_width = ((f32)g->width * font_scalar);
        f32 y_height = ((f32)g->height * font_scalar);

        //printf("xpos %f, ypos%f, w%f, h%f\n", xpos, ypos, w, h);

        // Convert screen coords to NDC [0,1] -> vulkan is [-1,1] which is handled in the shader
        f32 ndc_x0 = ((x_position) / screen_width);
        f32 ndc_x1 = (((x_position + x_width)) / screen_width);
        f32 ndc_y0 = ((y_position) / screen_height); // invert Y
        f32 ndc_y1 = (((y_position + y_height)) / screen_height);

        // UVs from the atlas
        vec2 uv0 = {g->u0, g->v0};
        vec2 uv1 = {g->u1, g->v1};

        //SET DRAW COLOR Based on the state
        // vec2 temp_pos = {0.5f, 0.5f};
        // vec2 temp_size = {0.1f, 0.1f};
        // Quad_Texture* new_quad = UI_create_quad_textured(Madness_UI, temp_pos, temp_size, color, uv0, uv1);
        // Quad_Texture* new_quad = UI_create_quad_textured(Madness_UI, new_final_pos, final_size, color, uv0, uv1);

        Quad_Texture new_quad[4] = {
            {{ndc_x0, ndc_y0}, color, {uv0.x, uv0.y}},
            {{ndc_x0, ndc_y1}, color, {uv0.x, uv1.y}},
            {{ndc_x1, ndc_y1}, color, {uv1.x, uv1.y}},
            {{ndc_x1, ndc_y0}, color, {uv1.x, uv0.y}},
        };


        /*SET DRAW INFO*/

        // Add text vertices
        memcpy((u8*)Madness_UI->text_draw_info.text_vertex + Madness_UI->text_draw_info.text_vertex_bytes, new_quad,
               sizeof(Quad_Texture) * 4);

        // create indices (two triangles per quad)
        memcpy((u8*)Madness_UI->text_draw_info.text_indices + Madness_UI->text_draw_info.text_index_bytes,
               default_quad_indices,
               sizeof(default_quad_indices));

        //increase by count
        Madness_UI->text_draw_info.text_vertex_bytes += sizeof(Quad_Texture) * 4;
        //increase by bytes
        Madness_UI->text_draw_info.text_index_bytes += sizeof(default_quad_indices);
        Madness_UI->text_draw_info.text_index_count += ARRAY_SIZE(default_quad_indices);

        //add to material params
        //TODO: this logic is just temporary
        Material_2D_Param_Data* material_info = &Madness_UI->text_draw_info.text_material_params[Madness_UI->
            text_draw_info.text_material_param_current_size];
        Madness_UI->text_draw_info.text_material_param_current_size++;
        material_info->feature_mask = 0;
        material_info->texture_index = Madness_UI->default_font.font_texture_handle.handle;

        Madness_UI->text_draw_info.text_draw_count++;

        screen_position.x += (g->advance) * font_scalar; // move offset forward
    }
}


void madness_ui_test()
{
    UI_ID test_id = {0, 0};
    UI_ID test_id2 = {1, 0};
    //DO_BUTTON_TEST(Madness_UI_internal, test_id);
    // do_button(Madness_UI_internal, test_id, (vec2){1, 1}, (vec2){50, 50},
    // COLOR_RED, COLOR_GREEN, COLOR_BLUE);

    // do_button(Madness_UI_internal, test_id2, (vec2){50, 50}, (vec2){20, 20},
    // COLOR_YELLOW, COLOR_GREEN, COLOR_BLUE);

    // Source - https://stackoverflow.com/a/15736518
    // Posted by Stuart P. Bentley, modified by community. See post 'Timeline' for change history
    // Retrieved 2026-02-22, License - CC BY-SA 4.0


    do_text(STRING("!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"), (vec2){0, 70}, (vec2){10, 10},
            COLOR_WHITE,
            DEFAULT_FONT_SIZE);

    madness_ui_test2(&renderer_internal);

    // madness_ui_upload_draw_data(&renderer_internal);
}


void madness_ui_open_node(const char* id, UI_Config config)
{
    DEBUG("UI OPEN NODE: %s", id)
    //grab an available node
    UI_Node* new_node = &Madness_UI->ui_nodes->data[Madness_UI->ui_nodes->num_items];
    Madness_UI->ui_nodes->num_items++;

    DEBUG("UI OPEN NODE OBTAINED: %s", new_node->debug_id)
    new_node->debug_id = id;
    new_node->config = config;
    new_node->size_x = config.size.x;
    new_node->size_y = config.size.y;

    switch (new_node->config.size_type)
    {
    case UI_SIZING_FIXED:
        break;
    case UI_SIZING_FIT:
        //zero the sizes, as we do not want to expand past the childrens sizes
        new_node->size_x = 0;
        new_node->size_y = 0;
        break;
    case UI_SIZING_FIT_EXPAND:
        break;
    case UI_SIZING_EXPAND:
        break;
    case UI_SIZING_MAX:
        break;
    }


    //where we want to set the parent if the stack has anything on it
    if (Madness_UI->ui_stack_count > 0)
    {
        // this is always correct because any node that does not have a child,
        // will close itself immediately leaving the actual parent at the top of the stack
        //Example
        // push(parent)   // stack = [parent]
        // push(child1)   // stack = [parent, child1]
        // pop()          // stack = [parent]
        // push(child2)   // stack = [parent, child2]
        // pop()          // stack = [parent]
        // pop()          // stack = []
        new_node->parent = Madness_UI->ui_stack[Madness_UI->ui_stack_count - 1];
        //add the child to the parent
        new_node->parent->children[new_node->parent->children_length] = new_node;
        new_node->parent->children_length += 1;
    }

    //push element onto the stack
    Madness_UI->ui_stack[Madness_UI->ui_stack_count] = new_node;
    Madness_UI->ui_stack_count++;


    // node->parent->size_x;
}

void madness_ui_close_node(const char* id)
{
    DEBUG("UI CLOSE NODE: %s", id)
    //grab the open node which will always be the top node
    UI_Node* top_node = Madness_UI->ui_stack[Madness_UI->ui_stack_count - 1];
    DEBUG("UI CLOSE NODE TOP NODE OBTAINED: %s", top_node->debug_id)

    float horizontal_padding = top_node->config.padding.left + top_node->config.padding.right;
    float vertical_padding = top_node->config.padding.top + top_node->config.padding.bottom;

    switch (top_node->config.layout_direction)
    {
    case UI_LAYOUT_LEFT_TO_RIGHT:
        top_node->size_x += horizontal_padding;
        for (u64 i = 0; i < top_node->children_length; i++)
        {
            UI_Node* child_node = top_node->children[i];
            top_node->size_x += child_node->size_x;
            top_node->size_y = max_f(top_node->size_y, child_node->size_y + vertical_padding);
        }
        break;
    case UI_LAYOUT_TOP_TO_BOTTOM:
        top_node->size_y += vertical_padding;
        for (u64 i = 0; i < top_node->children_length; i++)
        {
            UI_Node* child_node = top_node->children[i];
            top_node->size_x = max_f(top_node->size_x, child_node->size_x + horizontal_padding);
            top_node->size_y += child_node->size_y;
        }
        break;
    }


    /*if (top_node->parent)
    {

        switch (top_node->parent->config.layout_direction)
        {
        case UI_LAYOUT_LEFT_TO_RIGHT:
            top_node->parent->size_x += top_node->size_x;
            top_node->parent->size_y = max_f(top_node->parent->size_y, top_node->size_y );
            break;
        case UI_LAYOUT_TOP_TO_BOTTOM:
            top_node->parent->size_x = max_f(top_node->parent->size_x, top_node->size_x );
            top_node->parent->size_y += top_node->size_y;
            break;
        }
    }*/

    //poping from the stack
    Madness_UI->ui_stack_count--;
}

#define Madness_UI(id, config)  \
for (       \
uint8_t latch = (madness_ui_open_node(id, config), 0);  \
latch < 1;      \
latch=1, madness_ui_close_node(id)           \
)

void madness_ui_test2(renderer* renderer)
{
    UI_Config parent1 = {0};
    parent1.pos = (vec2){0, 0};
    parent1.layout_direction = UI_LAYOUT_LEFT_TO_RIGHT;
    parent1.color = COLOR_RED;
    parent1.padding = (UI_Padding){50, 50, 50, 50};
    UI_Config child1 = {0};
    child1.color = COLOR_GREEN;
    child1.size = ( vec2){200, 200};
    child1.layout_direction = UI_LAYOUT_LEFT_TO_RIGHT;
    UI_Config child2 = {0};
    child2.color = COLOR_BLUE;
    // child2.size = (vec2){100, 100};
    child2.size = (vec2){0, 0};
    child2.layout_direction = UI_LAYOUT_TOP_TO_BOTTOM;
    // child2.padding = (UI_Padding){32, 16, 32, 16};
    child2.padding = (UI_Padding){16, 32, 16, 32};
    UI_Config subchild = {0};
    subchild.color = COLOR_YELLOW;
    subchild.size = (vec2){50, 50};
    subchild.layout_direction = UI_LAYOUT_LEFT_TO_RIGHT; // ui_open_node(Madness_UI, "1", parent1);
    UI_Config subchild2 = {0};
    subchild2.color = COLOR_ORANGE;
    subchild2.size = (vec2){50, 50};
    subchild2.layout_direction = UI_LAYOUT_LEFT_TO_RIGHT; // ui_open_node(Madness_UI, "1", parent1);
    UI_Config child3 = {0};
    child3.color = COLOR_PURPLE;
    child3.size = (vec2){25, 25};
    child3.layout_direction = UI_LAYOUT_LEFT_TO_RIGHT; // ui_open_node(Madness_UI, "2", child1);
    // ui_close_node(Madness_UI);
    // ui_close_node(Madness_UI);

    Madness_UI( "1", parent1)
    {
        Madness_UI( "2", child1)
        {
        }
        Madness_UI( "3", child2)
        {
            Madness_UI( "4", subchild)
            {
            }
            Madness_UI( "5", subchild2)
            {
            }
        }
        Madness_UI( "3", child3)
        {
        }
    }

    madness_ui_calculate_positions();
    madness_ui_generate_draw_data();
    madness_ui_generate_debug_data();
}

void madness_ui_calculate_positions(void)
{
    /*UI_Node* parent_node = &Madness_UI->ui_nodes_array[0];

    for (u64 children_index = 0; children_index < parent_node->children_length; children_index++)
    {
        UI_Node* child_node = parent_node->children[children_index];
        child_node->pos_x += child_node->size_x;
        // child_node->pos_y += child_node->size_y;
        float horizontal_padding = parent_node->config.padding.left;
        float vertical_padding = parent_node->config.padding.top;
        u64 left_offset = child_node->pos_x;
        u64 top_offset = child_node->pos_y;

        for (u64 sub_index = 0; sub_index < child_node->children_length; sub_index++)
        {
            UI_Node* sub_child = child_node->children[sub_index];
            sub_child->pos_x += left_offset;
            sub_child->pos_y += top_offset;
            left_offset += sub_child->size_x;
            top_offset += sub_child->size_y;
        }
    }*/

    for (u64 node_index = 0; node_index < Madness_UI->ui_nodes->num_items; node_index++)
    {
        UI_Node* parent_node = &Madness_UI->ui_nodes->data[node_index];

        float horizontal_padding = parent_node->config.padding.left;
        float vertical_padding = parent_node->config.padding.top;

        u64 left_offset = parent_node->pos_x;
        u64 top_offset = parent_node->pos_y;
        for (u64 children_index = 0; children_index < parent_node->children_length; children_index++)
        {
            UI_Node* child_node = parent_node->children[children_index];

            switch (parent_node->config.layout_direction)
            {
            case UI_LAYOUT_LEFT_TO_RIGHT:
                child_node->pos_x += left_offset + horizontal_padding;
                child_node->pos_y += top_offset + vertical_padding;
                left_offset += child_node->size_x;
                break;
            case UI_LAYOUT_TOP_TO_BOTTOM:
                child_node->pos_x += left_offset + horizontal_padding;
                child_node->pos_y += top_offset + vertical_padding;
                top_offset += child_node->size_y;
                break;
            }
        }
    }
}


void madness_ui_generate_draw_data(void)
{
    for (u32 i = 0; i < Madness_UI->ui_nodes->num_items; i++)
    {
        UI_Node* node_to_draw = &Madness_UI->ui_nodes->data[i];

        Quad_Vertex* new_quad = quad_create_screen_size(Madness_UI->frame_arena,
                                                        (vec2){node_to_draw->pos_x, node_to_draw->pos_y},
                                                        (vec2){node_to_draw->size_x, node_to_draw->size_y},
                                                        node_to_draw->config.color, Madness_UI->screen_size);
        madness_ui_add_quad_vertex(new_quad);
    }
}

void madness_ui_generate_debug_data(void)
{
    for (u64 i = 0; i < Madness_UI->ui_nodes->num_items; i++)
    {
        UI_Node* node_to_debug = &Madness_UI->ui_nodes->data[i];
        DEBUG(
            "UI NODE INFO ID %s: SIZE X %f, SIZE Y %f, POS X %f, POS Y %f\n CONFIG: SIZE X %f, SIZE Y %f, ALIGNMENT %d",
            node_to_debug->debug_id,
            node_to_debug->size_x,
            node_to_debug->size_y,
            node_to_debug->pos_x,
            node_to_debug->pos_y,
            node_to_debug->config.size.x,
            node_to_debug->config.size.y,
            node_to_debug->config.alignment)
    }
}


void madness_ui_draw(renderer* renderer, vulkan_command_buffer* command_buffer)
{
    //generate draw data
    madness_ui_upload_draw_data(&renderer_internal);

    //does the draw

    // UI
    vulkan_buffer* vert_buffer = vulkan_buffer_get(renderer, Madness_UI->ui_quad_vertex_buffer_handle);
    vulkan_buffer* index_buffer = vulkan_buffer_get(renderer, Madness_UI->ui_quad_index_buffer_handle);
    vulkan_buffer* quad_indirect_buffer = vulkan_buffer_get(renderer, Madness_UI->ui_quad_indirect_buffer_handle);

    vkCmdBindPipeline(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      renderer->ui_pipeline.handle);

    //uniform
    vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            renderer_internal.ui_pipeline.pipeline_layout, 0, 1,
                            &renderer_internal.descriptor_system->uniform_descriptors.descriptor_sets[renderer->context.
                                current_frame], 0, 0);

    //textures
    vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            renderer_internal.ui_pipeline.pipeline_layout, 1, 1,
                            &renderer_internal.descriptor_system->texture_descriptors.descriptor_sets[renderer->context.
                                current_frame], 0, 0);

    //storage buffers
    vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            renderer_internal.ui_pipeline.pipeline_layout, 2, 1,
                            &renderer_internal.descriptor_system->storage_descriptors.descriptor_sets[renderer->context.
                                current_frame], 0, 0);


    VkDeviceSize offsets_bindless[1] = {0};
    vkCmdBindVertexBuffers(command_buffer->handle, 0, 1,
                           &vert_buffer->handle, offsets_bindless);

    vkCmdBindIndexBuffer(command_buffer->handle,
                         index_buffer->handle, 0,
                         Madness_UI->quad_draw_info.index_type
    );

    // vkCmdDrawIndexed(command_buffer->handle,
    // Madness_UI->draw_info.index_count,
    // 1, 0, 0, 0);
    if (renderer->context.device.features.multiDrawIndirect)
    {
        vkCmdDrawIndexedIndirect(command_buffer->handle,
                                 quad_indirect_buffer->handle, 0,
                                 Madness_UI->quad_draw_info.quad_draw_count,
                                 sizeof(VkDrawIndexedIndirectCommand));
    }
    else
    {
        // If multi draw is not available, we must issue separate draw commands
        for (u64 j = 0; j < Madness_UI->quad_draw_info.quad_draw_count; j++)
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
    vulkan_buffer* text_vert_buffer = vulkan_buffer_get(renderer, Madness_UI->text_vertex_buffer_handle);
    vulkan_buffer* text_index_buffer = vulkan_buffer_get(renderer, Madness_UI->text_index_buffer_handle);
    vulkan_buffer* text_indirect = vulkan_buffer_get(renderer, Madness_UI->text_indirect_buffer_handle);


    vkCmdBindPipeline(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      renderer->text_pipeline.handle);

    //uniform
    vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            renderer_internal.text_pipeline.pipeline_layout, 0, 1,
                            &renderer_internal.descriptor_system->uniform_descriptors.descriptor_sets[renderer->context.
                                current_frame], 0, 0);

    //textures
    vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            renderer_internal.text_pipeline.pipeline_layout, 1, 1,
                            &renderer_internal.descriptor_system->texture_descriptors.descriptor_sets[renderer->context.
                                current_frame], 0, 0);

    //storage buffers
    vkCmdBindDescriptorSets(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            renderer_internal.text_pipeline.pipeline_layout, 2, 1,
                            &renderer_internal.descriptor_system->storage_descriptors.descriptor_sets[renderer->context.
                                current_frame], 0, 0);


    //grab material_handle
    Madness_UI->text_draw_info.pc_2d_text.material_buffer_idx = Madness_UI->text_material_ssbo_handle.handle;

    VkPushConstantsInfo push_constant_info = {0};
    push_constant_info.sType = VK_STRUCTURE_TYPE_PUSH_CONSTANTS_INFO;
    push_constant_info.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    push_constant_info.layout = renderer->text_pipeline.pipeline_layout;
    push_constant_info.offset = 0;
    push_constant_info.size = sizeof(PC_2D);
    push_constant_info.pValues = &Madness_UI->text_draw_info.pc_2d_text;
    push_constant_info.pNext = NULL;

    vkCmdPushConstants2(command_buffer->handle, &push_constant_info);

    vkCmdBindVertexBuffers(command_buffer->handle, 0, 1,
                           &text_vert_buffer->handle, offsets_bindless);

    vkCmdBindIndexBuffer(command_buffer->handle,
                         text_index_buffer->handle, 0,
                         Madness_UI->text_draw_info.index_type);

    if (renderer->context.device.features.multiDrawIndirect)
    {
        vkCmdDrawIndexedIndirect(command_buffer->handle,
                                 text_indirect->handle, 0,
                                 Madness_UI->text_draw_info.text_draw_count,
                                 sizeof(VkDrawIndexedIndirectCommand));
    }
    else
    {
        // If multi draw is not available, we must issue separate draw commands
        for (u64 j = 0; j < Madness_UI->text_draw_info.text_draw_count; j++)
        {
            vkCmdDrawIndexedIndirect(command_buffer->handle,
                                     text_indirect->handle,
                                     j * sizeof(VkDrawIndexedIndirectCommand), 1,
                                     sizeof(VkDrawIndexedIndirectCommand));
        }
    }
}
