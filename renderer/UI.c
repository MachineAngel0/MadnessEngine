#include "UI.h"

#include "vk_buffer.h"


UI_System* ui_system_init(renderer* renderer)
{
    UI_System* ui_system = arena_alloc(&renderer->arena, sizeof(UI_System));
    memset(ui_system, 0, (sizeof(UI_System)));

    //TODO: TEMPORARY, request memory from the memory_system
    ui_system->arena = &renderer->arena;
    ui_system->frame_arena = &renderer->frame_arena;

    ui_system->draw_info.index_type = VK_INDEX_TYPE_UINT16;

    ui_system->default_font_size = 128.0f;
    
    ui_system->active.ID = -1;
    ui_system->active.layer = -1;
    ui_system->hot.ID = -1;
    ui_system->hot.layer = -1;


    ui_system->id_generation_number = -1;

    ui_system->mouse_pos_x = -1.0f;
    ui_system->mouse_pos_y = -1.0f;

    ui_system->mouse_down = 0;
    ui_system->mouse_released = 0;

    //TODO: replace with an in param
    ui_system->screen_size = (vec2){800.0f, 600.0f};


    // text_system_init(ui_system->text_system);

    u32 ui_buffer_sizes = MB(1);

    ui_system->ui_quad_vertex_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                  BUFFER_TYPE_VERTEX, ui_buffer_sizes);
    ui_system->ui_quad_index_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                 BUFFER_TYPE_INDEX, ui_buffer_sizes);
    ui_system->text_vertex_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system, BUFFER_TYPE_VERTEX,
                                                               ui_buffer_sizes);
    ui_system->text_index_buffer_handle = vulkan_buffer_create(renderer, renderer->buffer_system, BUFFER_TYPE_INDEX,
                                                              ui_buffer_sizes);


    //TODO: THERE IS NO WAY I AM LOADING THIS FILE FROM THERE, WHAT ABOUT LINUX0
    font_init(renderer, ui_system, "c:/windows/fonts/arialbd.ttf");



    return ui_system;
}

void ui_system_begin(UI_System* ui_system, i32 screen_size_x, i32 screen_size_y)
{
    //clear draw info and reset the hot id

    //std::cout << "MOUSE STATE:" << ui_system.mouse_down << '\n';

    ui_system->screen_size.x = screen_size_x;
    ui_system->screen_size.y = screen_size_y;

    //TODO: clear vertex and index data for UI and TEXT
    // clear_vertex_info(ui_system->draw_info.vertex_info);
    // ui_system->draw_info.UI_Objects.clear();
    ui_system->draw_info.quad_vertex_bytes = 0;
    ui_system->draw_info.index_bytes = 0;
    ui_system->draw_info.index_count = 0;


    ui_system->hot.ID = -1;
    ui_system->hot.ID = -1;
    ui_system->hot.layer = -1;

    //TODO: UPDATE TEXT
    // text_update(ui_system->text_system);
}

void ui_system_end(UI_System* ui_system)
{
    //check if mouse is released, if so reset the active id
    //also update the mouse state

    //printf("HOT ID: %d, HOT LAYER: %d\n", ui_system->hot.ID, ui_system->hot.layer);
    //printf("ACTIVE ID: %d, ACTIVE LAYER: %d\n", ui_system->active.ID, ui_system->active.layer);

    if (input_is_mouse_button_released(MOUSE_BUTTON_LEFT))
    {
        //std::cout << "mouse released\n";
        ui_system->active.ID = -1;
    }


    //update mouse state
    ui_system->mouse_down = input_is_mouse_button_pressed(MOUSE_BUTTON_LEFT);
    // DEBUG("MOUSE DOWN %d", ui_system->mouse_down)
    //update mouse pos
    update_ui_mouse_pos(ui_system);
}

Font_Handle font_init(renderer* renderer, UI_System* ui_system, const char* filepath)
{
    //TODO: rn only loads the default font, should get an available font at startup
    Madness_Font* font_structure = &ui_system->default_font;

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
        printf("Failed to initialize font\n");
        free(ttf_buffer);
        return out_handle;
    }

    // Generate bitmap atlas
    float scale = stbtt_ScaleForPixelHeight(&font_structure->font_info, ui_system->default_font_size);
    int atlasWidth = 1024 * 4;
    int atlasHeight = 1024 * 4;
    char* atlasPixels = arena_alloc(ui_system->arena, atlasWidth* atlasHeight);

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
            printf("Error: Texture atlas too small!\n");
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
        Glyph g = font_structure->glyphs[c - 32];
        g.width = width;
        g.height = height;
        g.xoff = xoff;
        g.yoff = baseline + yoff; // Adjust for baseline

        int advance, lsb;
        stbtt_GetCodepointHMetrics(&font_structure->font_info, c, &advance, &lsb);
        g.advance = advance * scale;


        // Calculate UV coordinates
        g.u0 = (float) (x / atlasWidth);
        g.v0 = (float)(y / atlasHeight);
        g.u1 = (float)((x + width) / atlasWidth);
        g.v1 = ((float)(y + height) / atlasHeight);

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
    unsigned char* atlas_RGBA = arena_alloc(ui_system->arena, atlasRGBA_size);
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
        fprintf(debug_file, "P6\n%d %d\n255\n", atlasWidth, atlasHeight);

        // Write RGB data to PPM filing using the debug data, which is basically not flipped
        for (int i = 0; i < atlasWidth * atlasHeight; i++)
        {
            fwrite(&atlasPixels[i], 1, 1, debug_file); // R
            fwrite(&atlasPixels[i], 1, 1, debug_file); // G
            fwrite(&atlasPixels[i], 1, 1, debug_file); // B
        }
        fclose(debug_file);
        printf("Font atlas saved to: %s\n", debug_filename);
    }
    else
    {
        printf("Warning: Could not save font atlas debug file\n");
    }

    // Also save as raw RGBA data
    const char* raw_filename = "font_atlas_debug.raw";
    FILE* raw_file = fopen(raw_filename, "wb");
    if (raw_file)
    {
        fwrite(atlas_RGBA, 1, atlasRGBA_size, raw_file);
        fclose(raw_file);
        printf("Raw RGBA atlas data saved to: %s (%dx%d RGBA)\n",
               raw_filename, atlasWidth, atlasHeight);
    }

    // Upload to Vulkan texture
    create_texture_glyph(renderer, renderer->context.graphics_command_buffer, &font_structure->font_texture,
                         atlas_RGBA, atlasWidth, atlasHeight);

    printf("Font loaded successfully: %s\n", filepath);

    return out_handle;

}

void ui_system_upload_draw_data(renderer* renderer, UI_System* ui_system)
{
    vulkan_buffer_reset_offset(renderer, ui_system->ui_quad_vertex_buffer_handle);
    vulkan_buffer_reset_offset(renderer, ui_system->ui_quad_index_buffer_handle);
    // vulkan_buffer_reset_offset(renderer, ui_system->text_vertex_buffer_handle);
    // vulkan_buffer_reset_offset(renderer, ui_system->text_index_buffer_handle);

    //TODO: generate draw indirect commands

    vulkan_buffer_cpu_data_copy_from_offset_handle(renderer, &ui_system->ui_quad_vertex_buffer_handle,
                                                   ui_system->draw_info.quad_vertex, ui_system->draw_info.quad_vertex_bytes);

    vulkan_buffer_cpu_data_copy_from_offset_handle(renderer, &ui_system->ui_quad_index_buffer_handle,
                                                   ui_system->draw_info.indices, ui_system->draw_info.index_bytes);
}

void ui_system_draw(renderer* renderer, UI_System* ui_system, vulkan_command_buffer* command_buffer)
{
    vulkan_buffer* vert_buffer = vulkan_buffer_get(renderer,ui_system->ui_quad_vertex_buffer_handle);
    vulkan_buffer* index_buffer = vulkan_buffer_get(renderer, ui_system->ui_quad_index_buffer_handle);

    vkCmdBindPipeline(command_buffer->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          renderer->ui_pipeline.handle);

    //TODO: realistically textures is the only thing the UI needs for me, for now
    //textures
    // vkCmdBindDescriptorSets(command_buffer_current_frame->handle, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            // vk_context.shader_texture_bindless.shader_texture_pipeline.pipeline_layout, 1, 1,
                            // &renderer_internal.global_descriptors.texture_descriptors.descriptor_sets[vk_context.
                                // current_frame],
                            // 0, 0);

    VkDeviceSize offsets_bindless[1] = {0};
    vkCmdBindVertexBuffers(command_buffer->handle, 0, 1,
                           &vert_buffer->handle, offsets_bindless);

    vkCmdBindIndexBuffer(command_buffer->handle,
                         index_buffer->handle, 0,
                         ui_system->draw_info.index_type
);

    // vkCmdDrawIndexed(command_buffer->handle,
                     // ui_system->draw_info.index_count,
                     // 1, 0, 0, 0);
    vkCmdDrawIndexed(command_buffer->handle,
                 ui_system->draw_info.index_count,
                 1, 0, 0, 0);

}

void update_ui_mouse_pos(UI_System* ui_system)
{
    input_get_mouse_pos(&ui_system->mouse_pos_x, &ui_system->mouse_pos_y);
}


Quad_Vertex* UI_create_quad(UI_System* ui_system, vec2 pos, vec2 size, vec3 color)
{
    Quad_Vertex* out_vertex = arena_alloc(ui_system->frame_arena, sizeof(Quad_Vertex) * 4);

    out_vertex[0] = (Quad_Vertex){.pos = {pos.x, pos.y}, .color = color};
    out_vertex[1] = (Quad_Vertex){.pos = {pos.x, pos.y + size.y}, .color = color};
    out_vertex[2] = (Quad_Vertex){.pos = {pos.x + size.x, pos.y + size.y}, .color = color};
    out_vertex[3] = (Quad_Vertex){.pos = {pos.x + size.x, pos.y}, .color = color};

    return out_vertex;
}

Quad_Vertex* UI_create_quad_screen_percentage(UI_System* ui_system, vec2 pos, vec2 size, vec3 color)
{

    Quad_Vertex* out_vertex = arena_alloc(ui_system->frame_arena, sizeof(Quad_Vertex) * 4);

    out_vertex[0] = (Quad_Vertex){.pos = {pos.x - size.x, pos.y - size.y}, .color = color};
    out_vertex[1] = (Quad_Vertex){.pos = {pos.x - size.x, pos.y + size.y}, .color = color};
    out_vertex[2] = (Quad_Vertex){.pos = {pos.x + size.x, pos.y + size.y}, .color = color};
    out_vertex[3] = (Quad_Vertex){.pos = {pos.x + size.x, pos.y - size.y}, .color = color};

    return out_vertex;
}


bool is_ui_hot(UI_System* ui_system, int id)
{
    return ui_system->hot.ID && ui_system->hot.ID == id;
}

bool is_ui_active(UI_System* ui_system, int id)
{
    return ui_system->active.ID && ui_system->active.ID == id;
}


bool region_hit(UI_System* ui_system, vec2 pos, vec2 size)
{
    //check if we are inside a ui_object

    // bottom left
    if (pos.x - size.x > ui_system->mouse_pos_x) return false;
    if (pos.y - size.y > ui_system->mouse_pos_y) return false;

    // top left
    if (pos.x - size.x > ui_system->mouse_pos_x) return false;
    if (pos.y + size.y < ui_system->mouse_pos_y) return false;

    // top right
    if (pos.x + size.x < ui_system->mouse_pos_x)return false;
    if (pos.y + size.y < ui_system->mouse_pos_y) return false;

    // bottom right
    if (pos.x + size.x < ui_system->mouse_pos_x) return false;
    if (pos.y - size.y > ui_system->mouse_pos_y) return false;

    return true;
}

bool region_hit_new(UI_System* ui_system, vec2 pos, vec2 size)
{
    //check if we are inside a ui_object

    //printf("DEBUG REGION HIT: MOUSE: %f, %f POS: %f, %f SIZE: %f, %f\n", ui_system->mouse_pos.x, ui_system->mouse_pos.y, pos.x, pos.y, size.x, size.y);

    //top left
    if (pos.x > ui_system->mouse_pos_x) return false;
    if (pos.y > ui_system->mouse_pos_y) return false;


    // bottom left
    if (pos.x > ui_system->mouse_pos_x) return false;
    if (pos.y + size.y < ui_system->mouse_pos_y) return false;


    //top right
    if (pos.x + size.x < ui_system->mouse_pos_x) return false;
    if (pos.y > ui_system->mouse_pos_y) return false;

    // bottom right
    if (pos.x + size.x < ui_system->mouse_pos_x) return false;
    if (pos.y + size.y < ui_system->mouse_pos_y) return false;


    return true;
}


/*
bool button(ui_system& ui_system, int id, int x, int y)
{
    if (region_hit())
    {
        ui = hot;
        if (acitve == 0 and mouse down)
            activeitem = id
    }
}*/

//check if we can use the button
bool use_button(UI_System* ui_system, UI_ID id, vec2 pos, vec2 size)
{
    //checking if we released the mouse button, are active, and we are inside the hit region
    if (ui_system->mouse_down == 0 &&
        ui_system->active.ID == id.ID &&
        region_hit(ui_system, pos, size))
        return true;

    return false;
}

bool use_button_new(UI_System* ui_system, UI_ID id, vec2 pos, vec2 size)
{
    //checking if we released the mouse button, are active, and we are inside the hit region
    if (ui_system->mouse_down == 0 &&
        ui_system->active.ID == id.ID &&
        region_hit_new(ui_system, pos, size))
        return true;

    return false;
}

int generate_id(UI_System* ui_system)
{
    return ui_system->id_generation_number++;
}

void set_hot(UI_System* ui_system, UI_ID id)
{
    if (ui_system->hot.layer <= id.layer)
    {
        ui_system->hot.ID = id.ID;
        ui_system->hot.layer = id.layer;
        //printf("ID: %d, is hot\n", id);
    }
}

void set_active(UI_System* ui_system, UI_ID id)
{
    if (ui_system->active.layer <= id.layer)
    {
        ui_system->active.ID = id.ID;
        ui_system->active.layer = id.layer;
        //printf("ID: %d, is active\n", id);
    }
}

bool can_be_active(UI_System* ui_system)
{
    return ui_system->active.ID == -1 && ui_system->mouse_down;
}

bool is_active(UI_System* ui_system, UI_ID id)
{
    return ui_system->active.ID == id.ID;
}

bool is_hot(UI_System* ui_system, UI_ID id)
{
    return ui_system->hot.ID == id.ID;
}


bool do_button(UI_System* ui_system, UI_ID id, vec2 pos, vec2 screen_percentage,
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

    // vec2 converted_pos2 = vec2_mul(pos, ui_system->screen_size);
    // vec2 converted_pos3 = vec2_div(pos, ui_system->screen_size);

    /*
    vec2 final_pos = {
        ui_system->screen_size.x * converted_pos.x,
        ui_system->screen_size.y * converted_pos.y
    };
    vec2 final_size = {
        (ui_system->screen_size.x * converted_size.x) / 2,
        (ui_system->screen_size.y * converted_size.y) / 2
    };*/

     vec2 final_pos = {
         converted_pos.x,
         converted_pos.y
     };
     vec2 final_size = {
         converted_size.x/2,
         converted_size.y/2,
     };

    vec2 mouse_hit_pos = {
        ui_system->screen_size.x * converted_pos.x,
         ui_system->screen_size.y *converted_pos.y
    };
    vec2 mouse_hit_size = {
        ui_system->screen_size.x * converted_size.x/2,
        ui_system->screen_size.y * converted_size.y/2,
    };


    /*SET UI STATE*/

    int mesh_id = ui_system->id_generation_number++;

    //check if button is hot and active
    if (region_hit(ui_system, mouse_hit_pos, mouse_hit_size))
    {
        set_hot(ui_system, id);

        //check if we have the mouse pressed and nothing else is selected
        //TODO: so there is a bug with can_be_active, in that the first ui called on the screen will take active focus,
        //TODO: this is despite there bieng another ui in front of it
        //TODO: for now imma just leave it be and dont draw things on top of others
        if (can_be_active(ui_system))
        {
            set_active(ui_system, id);
        }
    }
    /*
    else
    {
        printf("is not hot\n");
    }*/


    /*SET DRAW COLOR Based on the state*/
    Quad_Vertex* new_quad = arena_alloc(ui_system->frame_arena, sizeof(Quad_Vertex) * 4);

    //set color
    //active state
    if (is_active(ui_system, id))
    {
        new_quad = UI_create_quad_screen_percentage(ui_system, final_pos, final_size, pressed_color);
    }
    //hot state
    else if (is_hot(ui_system, id))
    {
        new_quad = UI_create_quad_screen_percentage(ui_system, final_pos, final_size, hovered_color);
    }
    // normal state
    else
    {
        new_quad = UI_create_quad_screen_percentage(ui_system, final_pos, final_size, color);
    }


    /*SET DRAW INFO*/

    // Add vertices
    memcpy(ui_system->draw_info.quad_vertex, new_quad, sizeof(Quad_Vertex) * 4);

    // create indices (two triangles per quad)
    memcpy(ui_system->draw_info.indices + ui_system->draw_info.index_bytes, default_quad_indices, sizeof(default_quad_indices));

    //increase by count
    ui_system->draw_info.quad_vertex_bytes += sizeof(Quad_Vertex) * 4;
    //increase by bytes
    ui_system->draw_info.index_bytes += sizeof(default_quad_indices);
    ui_system->draw_info.index_count += ARRAY_SIZE(default_quad_indices);

    //check if we clicked the button
    if (use_button_new(ui_system, id, final_pos, final_size)) return true;

    //check if we clicked the button
    if (use_button(ui_system, id, final_pos, final_size)) return true;

    return false;
}


bool do_button_new(UI_System* ui_system, UI_ID id, vec2 pos, vec2 size,
                   UI_Alignment alignment, vec3 color,
                   vec3 hovered_color, vec3 pressed_color)
{
    /*validation*/
    if (size.x > 100 || size.x < 0 || size.y > 100 || size.y < 0)
    {
        printf("SIZE CANNOT BE GREATER THAN 100 AND LESS THAN 0");
    }

    if (pos.x > 100 || pos.x < 0 || pos.y > 100 || pos.y < 0)
    {
        printf("POSITION CANNOT BE GREATER THAN 100 AND LESS THAN 0");
    }


    /*POS AND SIZE CALCULATIONS*/

    vec2 converted_pos = vec2_div_scalar(pos, 100.0f);
    vec2 converted_size = vec2_div_scalar(size, 100.0f);

    //TODO: this realistically can be moved out to the ui_system struct
    float quarter_size_x = ui_system->screen_size.x * 0.25;
    float quarter_size_y = ui_system->screen_size.y * 0.25;
    float half_size_x = ui_system->screen_size.x * 0.5;
    float half_size_y = ui_system->screen_size.y * 0.5;
    vec2 final_pos;

    switch (alignment)
    {
    case UI_ALIGNMENT_CENTER:
        final_pos = (vec2){
            .x = {ui_system->screen_size.x * converted_pos.x - quarter_size_x},
            .y = {ui_system->screen_size.y * converted_pos.y - quarter_size_y},
        };
        break;
    case UI_ALIGNMENT_LEFT:
        final_pos = (vec2){
            .x = {ui_system->screen_size.x * converted_pos.x},
            .y = {ui_system->screen_size.y * converted_pos.y},
        };
        break;
    case UI_ALIGNMENT_RIGHT:
        final_pos = (vec2){
            .x = {ui_system->screen_size.x * converted_pos.x + half_size_x},
            .y = {ui_system->screen_size.y * converted_pos.y},
        };
        break;
    case UI_ALIGNMENT_MAX:
        MASSERT(false);
        break;
    }


    vec2 final_size = {
        (ui_system->screen_size.x * converted_size.x),
        (ui_system->screen_size.y * converted_size.y),
    };

    /*SET UI STATE*/

    int mesh_id = ui_system->id_generation_number++;

    //check if button is hot and active
    if (region_hit_new(ui_system, final_pos, final_size))
    {
        set_hot(ui_system, id);

        //check if we have the mouse pressed and nothing else is selected
        //TODO: so there is a bug with can_be_active, in that the first ui called on the screen will take active focus,
        //TODO: this is despite there bieng another ui in front of it
        //TODO: for now imma just leave it be and dont draw things on top of others
        if (can_be_active(ui_system))
        {
            set_active(ui_system, id);
        }
    }
    /*
    else
    {
        printf("is not hot\n");
    }*/


    //SET DRAW COLOR Based on the state
    Quad_Vertex* new_quad = arena_alloc(ui_system->frame_arena, sizeof(Quad_Vertex));

    //set color
    //active state
    if (is_active(ui_system, id))
    {
        new_quad = UI_create_quad_screen_percentage(ui_system, final_pos, final_size, pressed_color);
    }
    //hot state
    else if (is_hot(ui_system, id))
    {
        new_quad = UI_create_quad_screen_percentage(ui_system, final_pos, final_size, hovered_color);
    }
    // normal state
    else
    {
        new_quad = UI_create_quad_screen_percentage(ui_system, final_pos, final_size, color);
    }


    /*SET DRAW INFO*/

    // Add vertices
    memcpy(ui_system->draw_info.quad_vertex, new_quad, sizeof(Quad_Vertex) * 4);

    // create indices (two triangles per quad)
    memcpy(ui_system->draw_info.indices + ui_system->draw_info.index_bytes, default_quad_indices, sizeof(default_quad_indices));

    //increase by count
    ui_system->draw_info.quad_vertex_bytes += sizeof(Quad_Vertex) * 4;
    //increase by bytes
    ui_system->draw_info.index_bytes += sizeof(default_quad_indices);
    ui_system->draw_info.index_count += ARRAY_SIZE(default_quad_indices);


    //check if we clicked the button
    if (use_button_new(ui_system, id, final_pos, final_size)) return true;

    return false;
}

/*
bool do_button_new_text(UI_System* ui_system, UI_ID id, glm::vec2 pos, glm::vec2 size, String text,
                        glm::vec2 text_padding = {0.0f, 0.0f}, glm::vec3 color = {1.0f, 1.0f, 1.0f},
                        glm::vec3 hovered_color = {1.0f, 1.0f, 1.0f}, glm::vec3
                        pressed_color = {1.0f, 1.0f, 1.0f})
{
    //validation
    if (size.x > 100 || size.x < 0 || size.y > 100 || size.y < 0)
    {
        printf("SIZE CANNOT BE GREATER THAN 100 AND LESS THAN 0");
    }

    if (pos.x > 100 || pos.x < 0 || pos.y > 100 || pos.y < 0)
    {
        printf("POSITION CANNOT BE GREATER THAN 100 AND LESS THAN 0");
    }


    // POS AND SIZE CALCULATIONS

    vec2 converted_pos = pos / 100.0f;
    vec2 converted_size = size / 100.0f;

    vec2 final_pos = {
        ui_system->screen_size.x * converted_pos.x,
        ui_system->screen_size.y * converted_pos.y,
    };

    vec2 final_size = {
        (ui_system->screen_size.x * converted_size.x),
        (ui_system->screen_size.y * converted_size.y),
    };

    do_text(ui_system, text, pos + text_padding, vec3{1.0, 1.0, 0.0});


    //SET UI STATE

    int mesh_id = ui_system->draw_info.vertex_info.mesh_id++;

    //check if button is hot and active
    if (region_hit_new(ui_system, final_pos, final_size))
    {
        set_hot(ui_system, id);

        //check if we have the mouse pressed and nothing else is selected
        //TODO: so there is a bug with can_be_active, in that the first ui called on the screen will take active focus,
        //TODO: this is despite there bieng another ui in front of it
        //TODO: for now imma just leave it be and dont draw things on top of others
        if (can_be_active(ui_system))
        {
            set_active(ui_system, id);
        }
    }

    //else
    //{
     //   printf("is not hot\n");
    //}


    //SET DRAW COLOR Based on the state
    std::vector<Vertex> new_quad;

    //set color
    //active state
    if (is_active(ui_system, id))
    {
        new_quad = UI_create_quad(final_pos, final_size, pressed_color);
    }
    //hot state
    else if (is_hot(ui_system, id))
    {
        new_quad = UI_create_quad(final_pos, final_size, hovered_color);
    }
    // normal state
    else
    {
        new_quad = UI_create_quad(final_pos, final_size, color);
    }


    // SET DRAW INFO

    uint16_t base_index =
    <uint16_t > (ui_system->draw_info.vertex_info.dynamic_vertices.size());

    // create indices (two triangles per quad)
    std::vector<uint16_t> quad_indices = {
        <uint16_t > (base_index + 0),
        <uint16_t > (base_index + 1),
        <uint16_t > (base_index + 2),
        <uint16_t > (base_index + 2),
        <uint16_t > (base_index + 3),
        <uint16_t > (base_index + 0)
    };

    // Add vertices
    ui_system->draw_info.vertex_info.dynamic_vertices.insert(ui_system->draw_info.vertex_info.dynamic_vertices.end(),
                                                            new_quad.begin(),
                                                            new_quad.end());
    // Add indices
    ui_system->draw_info.vertex_info.dynamic_indices.insert(ui_system->draw_info.vertex_info.dynamic_indices.end(),
                                                           quad_indices.begin(), quad_indices.end());

    //check if we clicked the button
    if (use_button_new(ui_system, id, final_pos, final_size)) return true;

    return false;
}

bool do_button_text(UI_System* ui_system, UI_ID id, String text, glm::vec2 pos, glm::vec2 screen_percentage,
                    glm::vec3 color = {1.0f, 1.0f, 1.0f}, glm::vec3 hovered_color = {1.0f, 1.0f, 1.0f}, glm::vec3
                    pressed_color = {1.0f, 1.0f, 1.0f})
{
    //validation

    if (screen_percentage.x > 100 || screen_percentage.x < 0 || screen_percentage.y > 100 || screen_percentage.y < 0)
    {
        printf("SCREEN PERCENTAGE CANNOT BE GREATER THAN 100 AND LESS THAN 0");
    }
    if (pos.x > 100 || pos.x < 0 || pos.y > 100 || pos.y < 0)
    {
        printf("POSITION CANNOT BE GREATER THAN 100 AND LESS THAN 0");
    }

    //POS AND SIZE CALCULATIONS

    glm::vec2 converted_pos = pos / 100.0f;
    glm::vec2 converted_size = screen_percentage / 100.0f;

    glm::vec2 final_pos = {
        ui_system->push_constants.screenSize.x * converted_pos.x,
        ui_system->push_constants.screenSize.y * converted_pos.y
    };
    glm::vec2 final_size = {
        (ui_system->push_constants.screenSize.x * converted_size.x) / 2,
        (ui_system->push_constants.screenSize.y * converted_size.y) / 2
    };

    //do_text(ui_system, text, final_pos, {1.0f,1.0f,1.0f});
    do_text_screen_percentage(ui_system, text, pos, screen_percentage, {0.0f, 0.0f, 1.0f}, 48);


    //SET UI STATE

    int mesh_id = ui_system->draw_info.vertex_info.mesh_id++;

    //check if button is hot and active
    if (region_hit(ui_system, final_pos, final_size))
    {
        set_hot(ui_system, id);

        //check if we have the mouse pressed and nothing else is selected
        //TODO: so there is a bug with can_be_active, in that the first ui called on the screen will take active focus,
        //TODO: this is despite there bieng another ui in front of it
        //TODO: for now imma just leave it be and dont draw things on top of others
        if (can_be_active(ui_system))
        {
            set_active(ui_system, id);
        }
    }

    //else
    //{
    //    printf("is not hot\n");
    //}


    //SET DRAW COLOR Based on the state
    std::vector<Vertex> new_quad;

    //set color
    //active state
    if (is_active(ui_system, id))
    {
        new_quad = UI_create_quad_screen_percentage(final_pos, final_size, pressed_color);
    }
    //hot state
    else if (is_hot(ui_system, id))
    {
        new_quad = UI_create_quad_screen_percentage(final_pos, final_size, hovered_color);
    }
    // normal state
    else
    {
        new_quad = UI_create_quad_screen_percentage(final_pos, final_size, color);
    }


    //SET DRAW INFO

    uint16_t base_index =
    <uint16_t > (ui_system->draw_info.vertex_info.dynamic_vertices.size());

    // create indices (two triangles per quad)
    std::vector<uint16_t> quad_indices = {
        <uint16_t > (base_index + 0),
        <uint16_t > (base_index + 1),
        <uint16_t > (base_index + 2),
        <uint16_t > (base_index + 2),
        <uint16_t > (base_index + 3),
        <uint16_t > (base_index + 0)
    };

    // Add vertices
    ui_system->draw_info.vertex_info.dynamic_vertices.insert(ui_system->draw_info.vertex_info.dynamic_vertices.end(),
                                                            new_quad.begin(),
                                                            new_quad.end());
    // Add indices
    ui_system->draw_info.vertex_info.dynamic_indices.insert(ui_system->draw_info.vertex_info.dynamic_indices.end(),
                                                           quad_indices.begin(), quad_indices.end());

    //check if we clicked the button
    if (use_button(ui_system, id, final_pos, final_size)) return true;

    return false;
}
*/
