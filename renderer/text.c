//
// Created by Adams Humbert on 9/10/2025.
//

#include "text.h"


Vertex_Text* text_create_quad(vec2 pos, vec2 size, vec3 color)
{
    Vertex_Text out_vertex_text[] = {
        {{pos.x - size.x, pos.y - size.y}, color},
        {{pos.x - size.x, pos.y + size.y}, color},
        {{pos.x + size.x, pos.y + size.y}, color},
        {{pos.x + size.x, pos.y - size.y}, color}
    };

    return out_vertex_text;
}

void do_text(UI_System* ui_state, std::string text, glm::vec2 pos, glm::vec3 color, float font_size)
{
    if (pos.x > 100 || pos.x < 0 || pos.y > 100 || pos.y < 0)
    {
        printf("YOU ARE STUPID 2: TEXT");
    }

    //move to the shader as a push constant
    float screen_width = ui_state->screen_size.x;
    float screen_height = ui_state->screen_size.y;
    //std::vector<Vertex_Text> new_quad = text_create_quad({0.5f,0.5f}, {0.1f,0.1f}, {1.0f,1.0f,1.0f});


    vec2 converted_pos = pos / 100.0f;
    vec2 final_pos = {
        screen_width * converted_pos.x,
        screen_height * converted_pos.y
    };

    float font_scalar = font_size / ui_state->text_system.default_font_size;


    for (const char& c : text)
    {
        if (c < 32 || c >= 128) continue; // skip unsupported characters
        Glyph& g = ui_state->text_system.glyphs[c - 32];


        // Quad position in screen coords
        float xpos = final_pos.x + g.xoff * font_scalar;
        float ypos = final_pos.y + g.yoff * font_scalar;
        float w = (float)(g.width) * font_scalar;
        float h = (float)(g.height) * font_scalar;

        //printf("xpos %f, ypos%f, w%f, h%f\n", xpos, ypos, w, h);

        // Convert screen coords to NDC [-1,1]
        float ndc_x0 = (xpos / screen_width) * 2.0f - 1.0f;
        float ndc_x1 = ((xpos + w) / screen_width) * 2.0f - 1.0f;
        float ndc_y0 = (ypos / screen_height) * 2.0f - 1.0f; // invert Y
        float ndc_y1 = ((ypos + h) / screen_height) * 2.0f - 1.0f;

        /* here for reference
        {{pos.x - size.x, pos.y - size.y}, {color}},
        {{pos.x - size.x, pos.y + size.y}, {color}},
        {{pos.x + size.x, pos.y + size.y}, {color}},
        {{pos.x + size.x, pos.y - size.y}, {color}}
        */
        // UVs from the atlas
        vec2 uv0 = {g.u0, g.v0};
        vec2 uv1 = {g.u1, g.v1};

        //glm::vec2(xpos / text_system.push_constants.screenSize) * 2.0f - 1.0f;

        std::vector<Vertex_Text> new_quad = {
            {{ndc_x0, ndc_y0}, color, {uv0.x, uv0.y}},
            {{ndc_x0, ndc_y1}, color, {uv0.x, uv1.y}},
            {{ndc_x1, ndc_y1}, color, {uv1.x, uv1.y}},
            {{ndc_x1, ndc_y0}, color, {uv1.x, uv0.y}},
        };

        /* TODO:  fruity colors, maybe ill make a seperate function that allows you to color each edge, cause its kinda cool, realistically you can just overlay another texture but that's fucking lame
        std::vector<Vertex_Text> new_quad = {
            {{ndc_x0, ndc_y0}, {0.0f,1.0f,1.0f}, {uv0.x, uv0.y}},
            {{ndc_x0, ndc_y1}, {1.0f,0.0f,1.0f}, {uv0.x, uv1.y}},
            {{ndc_x1, ndc_y1}, {1.0f,1.0f,1.0f}, {uv1.x, uv1.y}},
            {{ndc_x1, ndc_y0}, {1.0f,1.0f,0.0f}, {uv1.x, uv0.y}},
        };*/


        uint16_t base_index = (uint16_t)(ui_state->text_system.dynamic_vertices.size());


        // create indices (two triangles per quad)
        std::vector<uint16_t> quad_indices = {
            static_cast<uint16_t>(base_index + 0),
            static_cast<uint16_t>(base_index + 1),
            static_cast<uint16_t>(base_index + 2),
            static_cast<uint16_t>(base_index + 2),
            static_cast<uint16_t>(base_index + 3),
            static_cast<uint16_t>(base_index + 0)
        };

        // Add vertices
        ui_state->text_system.dynamic_vertices.insert(ui_state->text_system.dynamic_vertices.end(), new_quad.begin(),
                                                      new_quad.end());

        // Add indices
        ui_state->text_system.dynamic_indices.insert(ui_state->text_system.dynamic_indices.end(),
                                                     quad_indices.begin(), quad_indices.end());

        final_pos.x += g.advance * font_scalar; // move offset forward
    }
}

void do_text_screen_percentage(UI_System* ui_state, std::string text, vec2 pos, vec2 screen_percentage_size,
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
    float screen_width = ui_state->push_constants.screenSize.x;
    float screen_height = ui_state->push_constants.screenSize.y;

    //convert 0-00 -> 0-1
    glm::vec2 converted_pos = pos / 100.0f;
    glm::vec2 converted_size = screen_percentage_size / 100.0f;

    glm::vec2 final_pos = {
        screen_width * converted_pos.x,
        screen_height * converted_pos.y
    };
    glm::vec2 final_size = {
        (screen_width * converted_size.x),
        (screen_height * converted_size.y)
    };

    //We take the desired font size, scale it down proportional to the font size we created it at
    //final size of the font ex: 36/48 = 0.75, 48*0.75 = 36
    float font_scalar = font_size / ui_state->text_system.default_font_size;

    for (const char& c : text)
    {
        if (c < 32 || c >= 128) continue; // skip unsupported characters
        Glyph& g = ui_state->text_system.glyphs[c - 32];


        //take the x position and move it left based on the size we wanted it at
        float xpos = (final_pos.x - final_size.x + g.xoff);
        float ypos = (final_pos.y + g.yoff);

        //scales the texture
        float w = static_cast<float>(g.width) * font_scalar;
        float h = static_cast<float>(g.height) * font_scalar;

        //printf("xpos %f, ypos%f, w%f, h%f\n", xpos, ypos, w, h);


        // Convert screen coords to NDC [-1,1]
        float ndc_x0 = ((xpos) / screen_width) * 2.0f - 1.0f;
        float ndc_x1 = (((xpos + w)) / screen_width) * 2.0f - 1.0f;
        float ndc_y0 = ((ypos) / screen_height) * 2.0f - 1.0f; // invert Y
        float ndc_y1 = (((ypos + h)) / screen_height) * 2.0f - 1.0f;

        /*
        // Convert screen coords to NDC [-1,1]
        float ndc_x0 = ((xpos - final_size.x)/ screen_width) * 2.0f - 1.0f;
        float ndc_x1 = (((xpos + w) + final_size.x) / screen_width) * 2.0f - 1.0f;
        float ndc_y0 = ((ypos - final_size.x) / screen_height) * 2.0f- 1.0f; // invert Y
        float ndc_y1 = (((ypos + h) - final_size.x) / screen_height) * 2.0f - 1.0f;
        */

        /* here for reference
        {{pos.x - size.x, pos.y - size.y}, {color}},
        {{pos.x - size.x, pos.y + size.y}, {color}},
        {{pos.x + size.x, pos.y + size.y}, {color}},
        {{pos.x + size.x, pos.y - size.y}, {color}}
        */

        // UVs from the atlas
        glm::vec2 uv0(g.u0, g.v0);
        glm::vec2 uv1(g.u1, g.v1);


        std::vector<Vertex_Text> new_quad = {
            {{ndc_x0, ndc_y0}, color, {uv0.x, uv0.y}},
            {{ndc_x0, ndc_y1}, color, {uv0.x, uv1.y}},
            {{ndc_x1, ndc_y1}, color, {uv1.x, uv1.y}},
            {{ndc_x1, ndc_y0}, color, {uv1.x, uv0.y}},
        };

        /* TODO:  fruity colors, maybe ill make a seperate function that allows you to color each edge, cause its kinda cool, realistically you can just overlay another texture but that's fucking lame
        std::vector<Vertex_Text> new_quad = {
            {{ndc_x0, ndc_y0}, {0.0f,1.0f,1.0f}, {uv0.x, uv0.y}},
            {{ndc_x0, ndc_y1}, {1.0f,0.0f,1.0f}, {uv0.x, uv1.y}},
            {{ndc_x1, ndc_y1}, {1.0f,1.0f,1.0f}, {uv1.x, uv1.y}},
            {{ndc_x1, ndc_y0}, {1.0f,1.0f,0.0f}, {uv1.x, uv0.y}},
        };*/

        uint16_t base_index = static_cast<uint16_t>(ui_state->text_system.dynamic_vertices.size());

        // create indices (two triangles per quad)
        std::vector<uint16_t> quad_indices = {
            static_cast<uint16_t>(base_index + 0),
            static_cast<uint16_t>(base_index + 1),
            static_cast<uint16_t>(base_index + 2),
            static_cast<uint16_t>(base_index + 2),
            static_cast<uint16_t>(base_index + 3),
            static_cast<uint16_t>(base_index + 0)
        };

        // Add vertices
        ui_state->text_system.dynamic_vertices.insert(ui_state->text_system.dynamic_vertices.end(), new_quad.begin(),
                                                      new_quad.end());

        // Add indices
        ui_state->text_system.dynamic_indices.insert(ui_state->text_system.dynamic_indices.end(),
                                                     quad_indices.begin(), quad_indices.end());

        final_pos.x += g.advance * font_scalar; // move offset forward
    }
}


void create_text_vertex_buffer_new(Vulkan_Context& vulkan_context, Command_Buffer_Context& command_buffer_context,
                                   Buffer_Context& buffer_context)
{
    uint32_t max_object_count_t = 1000000;
    uint32_t vertices_per_object_t = 4;
    uint32_t MAX_VERTICES_text = max_object_count_t * vertices_per_object_t;

    // Create buffer large enough for maximum vertices, not just initial vertices
    buffer_context.vertex_buffer_capacity = sizeof(Vertex_Text) * MAX_VERTICES_text;
    // Use MAX_VERTICES instead of vertices.size()

    buffer_create(vulkan_context, buffer_context.vertex_buffer_capacity, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                  buffer_context.vertex_staging_buffer, buffer_context.vertex_staging_buffer_memory);

    // Only copy initial vertices data, but allocate full buffer
    VkDeviceSize initial_data_size = sizeof(text_verticies[0]) * vertices.size();

    vkMapMemory(vulkan_context.logical_device, buffer_context.vertex_staging_buffer_memory, 0,
                buffer_context.vertex_buffer_capacity, 0,
                &buffer_context.data_vertex);

    // Zero out the entire buffer first
    memset(buffer_context.data_vertex, 0, buffer_context.vertex_buffer_capacity);
    // Then copy initial data
    memcpy(buffer_context.data_vertex, vertices.data(), initial_data_size);
    vkUnmapMemory(vulkan_context.logical_device, buffer_context.vertex_staging_buffer_memory);

    // Create device local buffer with full size
    buffer_create(vulkan_context, buffer_context.vertex_buffer_capacity,
                  VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer_context.vertex_buffer,
                  buffer_context.vertex_buffer_memory);

    // Copy entire buffer (including zeros for unused space)
    buffer_copy(vulkan_context, command_buffer_context, buffer_context.vertex_staging_buffer,
                buffer_context.vertex_buffer,
                buffer_context.vertex_buffer_capacity);


    std::cout << "CREATED TEXT VERTEX BUFFER SUCCESS (Size: " << buffer_context.vertex_buffer_capacity << " bytes for "
        << MAX_VERTICES <<
        " vertices)\n";
}

void text_vertex_buffer_update(Vulkan_Context& vulkan_context, Command_Buffer_Context& command_buffer_context,
                               Buffer_Context& buffer_context, Text_System& text_system)
{
    //TODO: so while not happening rn, we can use an offset to only copy the new data, instead of the whole buffer over
    //so instead of copy buffer we can use copy_buffer_region, which ill have to double check how it works

    // Only copy we are currently using vertices data
    VkDeviceSize current_vertex_data_size = sizeof(text_verticies[0]) * text_system.dynamic_vertices.size();

    //map and copy data
    //NOTE: YOU TECHNICALLY DONT NEED TO MAP AND UNMAP THE DATA, YOU CAN JUST COPY THE DATA OVER

    //vkMapMemory(vulkan_context.logical_device, command_buffer_context.vertex_staging_buffer_memory, 0, vertex_buffer_capacity, 0,
    //           &data_vertex);
    memcpy(buffer_context.data_vertex, text_system.dynamic_vertices.data(), current_vertex_data_size);
    //vkUnmapMemory(vulkan_context.logical_device, command_buffer_context.vertex_staging_buffer_memory);

    //transfer from storage buffer/cpu buffer to gpu buffer
    buffer_copy(vulkan_context, command_buffer_context, buffer_context.vertex_staging_buffer,
                buffer_context.vertex_buffer,
                current_vertex_data_size);
    //last param in copy_buffer used to be vertex_buffer_capacity and index_buffer_capacity respectively


    VkDeviceSize current_index_data_size = sizeof(indices[0]) * text_system.dynamic_indices.size();

    //vkMapMemory(vulkan_context.logical_device, command_buffer_context.index_staging_buffer_memory, 0, index_buffer_capacity, 0, &data_index);
    memcpy(buffer_context.data_index, text_system.dynamic_indices.data(), current_index_data_size);
    //vkUnmapMemory(vulkan_context.logical_device, command_buffer_context.index_staging_buffer_memory);

    buffer_copy(vulkan_context, command_buffer_context, buffer_context.index_staging_buffer,
                buffer_context.index_buffer,
                current_index_data_size);
}

void text_system_init(Text_System& text_system)
{
    text_system.default_font_size = 128.0f;
    Texture font_texture;
    Vertex_Text dynamic_vertices;
    uint16_t dynamic_indices;
}

bool load_font(Text_System& text_system, const char* filepath,
               Vulkan_Context& vulkan_context, Command_Buffer_Context& command_buffer_context)
{
    // Load font file
    FILE* font_file = fopen(filepath, "rb");
    if (!font_file)
    {
        printf("Failed to open font file: %s\n", filepath);
        return false;
    }

    fseek(font_file, 0, SEEK_END);
    size_t size = ftell(font_file);
    rewind(font_file);

    char* ttf_buffer = (char*)malloc(size);
    fread(ttf_buffer, 1, size, font_file);
    fclose(font_file);

    // Initialize stb_truetype
    if (!stbtt_InitFont(&text_system.font, ttf_buffer, stbtt_GetFontOffsetForIndex(ttf_buffer, 0)))
    {
        printf("Failed to initialize font\n");
        free(ttf_buffer);
        return false;
    }

    // Generate bitmap atlas
    float scale = stbtt_ScaleForPixelHeight(&text_system.font, text_system.default_font_size);
    int atlasWidth = 1024 * 4;
    int atlasHeight = 1024 * 4;
    std::vector<unsigned char> atlasPixels(atlasWidth* atlasHeight, 0);

    int x = 0, y = 0, rowHeight = 0;

    // Get font metrics for baseline calculation
    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&text_system.font, &ascent, &descent, &lineGap);
    float baseline = ascent * scale;


    for (int c = 32; c < 128; c++)
    {
        int width, height, xoff, yoff;
        unsigned char* bitmap = stbtt_GetCodepointBitmap(&text_system.font, 0, scale, c,
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
            stbtt_FreeBitmap(bitmap, nullptr);
            free(ttf_buffer);
            return false;
        }

        // Copy glyph bitmap into atlas
        for (int row = 0; row < height; row++)
        {
            memcpy(&atlasPixels[(y + row) * atlasWidth + x],
                   &bitmap[row * width], width);
        }


        // Store glyph metrics
        Glyph& g = text_system.glyphs[c - 32];
        g.width = width;
        g.height = height;
        g.xoff = xoff;
        g.yoff = baseline + yoff; // Adjust for baseline

        int advance, lsb;
        stbtt_GetCodepointHMetrics(&text_system.font, c, &advance, &lsb);
        g.advance = advance * scale;


        // Calculate UV coordinates
        g.u0 = float(x) / atlasWidth;
        g.v0 = float(y) / atlasHeight;
        g.u1 = float(x + width) / atlasWidth;
        g.v1 = float(y + height) / atlasHeight;

        /*printf("Glyph '%c': size=(%d,%d), offset=(%d,%.1f), advance=%.1f, UV=(%.3f,%.3f)-(%.3f,%.3f)\n",
               c, width, height, xoff, g.yoff, g.advance, g.u0, g.v0, g.u1, g.v1);
               */

        x += width + 48; // Add 1 pixel padding
        if (height > rowHeight) rowHeight = height + 1;

        stbtt_FreeBitmap(bitmap, nullptr);
    }

    free(ttf_buffer);

    // Convert to RGBA
    vector<unsigned char> atlasRGBA(atlasWidth* atlasHeight *4);
    for (int i = 0; i < atlasWidth * atlasHeight; i++)
    {
        unsigned char v = atlasPixels[i];
        atlasRGBA[i * 4 + 0] = 255; // R
        atlasRGBA[i * 4 + 1] = 255; // G
        atlasRGBA[i * 4 + 2] = 255; // B
        atlasRGBA[i * 4 + 3] = v; // A (alpha from glyph)
    }

    // Save atlas to file for debugging, will be under cmake-build-debug
    std::string debug_filename = "font_atlas_debug.ppm";
    FILE* debug_file = fopen(debug_filename.c_str(), "wb");
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
        printf("Font atlas saved to: %s\n", debug_filename.c_str());
    }
    else
    {
        printf("Warning: Could not save font atlas debug file\n");
    }

    // Also save as raw RGBA data
    std::string raw_filename = "font_atlas_debug.raw";
    FILE* raw_file = fopen(raw_filename.c_str(), "wb");
    if (raw_file)
    {
        fwrite(atlasRGBA.data(), 1, atlasRGBA.size(), raw_file);
        fclose(raw_file);
        printf("Raw RGBA atlas data saved to: %s (%dx%d RGBA)\n",
               raw_filename.c_str(), atlasWidth, atlasHeight);
    }

    // Upload to Vulkan texture
    text_system.font_texture = {};
    create_texture_glyph(vulkan_context, command_buffer_context, text_system.font_texture,
                         atlasRGBA.data(), atlasWidth, atlasHeight);

    printf("Font loaded successfully: %s\n", filepath);
    return true;
}


void text_update(Text_System& text_system)
{
    text_system.dynamic_indices.clear();
    text_system.dynamic_vertices.clear();

    //do_text(text_system, "HELLO WORLD!", {1.0f,1.0f,0.0f});
    //do_text_screen_percentage(text_system, push_constants, "HELLO WORLD!",{50.0f,50.0f}, {20.0f, 20.0f}, {1.0f,1.0f,0.0f}, 36.0f, true);
}
