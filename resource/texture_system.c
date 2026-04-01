#include "texture_system.h"

Texture_System* texture_system_init(Memory_System* memory_system)
{
    Texture_System* texture_system = memory_system_alloc(memory_system, sizeof(Texture_System));


    texture_system->available_texture_indexes = 0;
    texture_system->max_textures = MAX_TEXTURE_COUNT;

    //hash table
    memset(texture_system->textures_array, 0, MAX_TEXTURE_COUNT * sizeof(Texture));
    memset(texture_system->font_array, 0, MAX_TEXTURE_COUNT * sizeof(Madness_Font));
    texture_system->texture_file_to_handle = HASH_TABLE_CREATE(Texture_Reference, MAX_TEXTURE_COUNT);
    texture_system->textures_ring_queue = ring_queue_create(sizeof(Texture), MAX_TEXTURE_COUNT);


    //create our debug texture
     if (!texture_system_load_texture(texture_system, "../renderer/texture/error_texture.png", &texture_system->default_texture_handle))
     {
         MASSERT_MSG(false, "TEXTURE SYSTEM FAILED TO LOAD DEFAULT TEXTURE");
     };

    return texture_system;
}

bool texture_system_shutdown(Texture_System* texture_system)
{
    //TODO: unload all the textures;
    return true;
}




//pass out the texture index
bool texture_system_load_texture(Texture_System* texture_system, char const* file_path, Texture_Handle* out_handle)
{
    Texture_Reference texture_reference;

    if (hash_table_contains_and_get(texture_system->texture_file_to_handle, file_path, &texture_reference))
    {
        texture_reference.reference_count++;
        out_handle = &texture_reference.handle;
        return true;
    }

    if (texture_system->available_texture_indexes >= MAX_TEXTURE_COUNT)
    {
        FATAL("MAX TEXTURES REACHED");
        out_handle->handle = 0;
        return false;
    }

    //get an available index
    Resource_MetaData* meta_data = &texture_system->texture_meta_data[texture_system->available_texture_indexes];
    meta_data->type = RESOURCE_TEXTURE;
    meta_data->handle = texture_system->available_texture_indexes;
    meta_data->file_path = file_path;

    texture_reference.handle.handle = texture_system->available_texture_indexes;
    texture_reference.reference_count = 1;

    out_handle->handle = texture_system->available_texture_indexes;
    Texture* texture = &texture_system->textures_array[texture_system->available_texture_indexes];
    texture_system->available_texture_indexes++;
    texture->handle = *out_handle;


    //add to hash table
    hash_table_insert(texture_system->texture_file_to_handle, file_path, &out_handle);

    //load the image data from file
    int texWidth, texHeight, texChannels;
    texture->pixels = stbi_load(file_path, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    //The pixels are laid out row by row with 4 bytes per pixel in the case of STBI_rgb_alpha for a total of texWidth * texHeight * 4 values.
    texture->width = texWidth; // 4 stride rgba
    texture->height = texHeight; // 4 stride rgba
    texture->channels = STBI_rgb_alpha; // 4 stride rgba
    texture->image_size = texWidth * texHeight * 4; // 4 stride rgba
    texture->is_font = false;

    MASSERT_MSG(texture->pixels, "FAILED TO LOAD TEXTURE");

    if (!texture->pixels)
    {
        WARN("TEXTURE SYSTEM LOAD TEXTURE: failed to load texture image!");
        out_handle = &texture_system->default_texture_handle;
        return false;
    }



    ring_enqueue(texture_system->textures_ring_queue, texture);


    return true;


    //TODO: send to the renderer to batch upload
    /*

    //create the texture
    Vulkan_Texture* out_texture = &system->textures[out_texture_handle.handle];
    create_texture_image(&renderer->context, renderer->context.graphics_command_buffer, filepath, out_texture);

    //TODO: batch this upload once a frame
    update_texture_bindless_descriptor_set(renderer, renderer->descriptor_system, out_texture_handle);

    */
}



Texture_Handle texture_system_update_texture(Texture_System* texture_system, Texture_Handle handle,
                                            const char* filepath)
{

    return (Texture_Handle){0};
}

Texture_Handle* texture_system_get_default_texture(Texture_System* texture_system)
{
    return &texture_system->default_texture_handle;
}

bool texture_system_load_font(Texture_System* texture_system, const char* file_path, Texture_Handle* out_handle, Arena* arena)
{
    //TODO: find if the texture exists
    Texture_Reference texture_reference;

    if (hash_table_contains_and_get(texture_system->texture_file_to_handle, file_path, &texture_reference))
    {
        texture_reference.reference_count++;
        out_handle = &texture_reference.handle;
        return true;
    }

    if (texture_system->available_texture_indexes >= MAX_TEXTURE_COUNT)
    {
        FATAL("LOAD FONT: MAX TEXTURES REACHED")
        out_handle->handle = 0;
        return false;
    }
    out_handle->handle = texture_system->available_texture_indexes;
    Madness_Font* font_structure = &texture_system->font_array[texture_system->available_texture_indexes];
    Texture* texture = &texture_system->textures_array[texture_system->available_texture_indexes];
    texture_system->available_texture_indexes++;


    // Load font file
    FILE* font_file = fopen(file_path, "rb");
    if (!font_file)
    {
        printf("Failed to open font file: %s\n", file_path);
        out_handle->handle = 0;
        return false;
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
    float scale = stbtt_ScaleForPixelHeight(&font_structure->font_info, DEFAULT_FONT_CREATION_SIZE);
    int atlas_width = 1024 * 4;
    int atlas_height = 1024 * 4;
    char* atlasPixels = arena_alloc(arena, atlas_width * atlas_height);

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
        if (x + width > atlas_width)
        {
            x = 0;
            y += rowHeight;
            rowHeight = 0;
        }

        if (y + height > atlas_height)
        {
            WARN("Error: Texture atlas too small!\n");
            stbtt_FreeBitmap(bitmap, NULL);
            free(ttf_buffer);
            return out_handle;
        }

        // Copy glyph bitmap into atlas
        for (int row = 0; row < height; row++)
        {
            memcpy(&atlasPixels[(y + row) * atlas_width + x],
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
        g->u0 = (float)x / (float)atlas_width;
        g->v0 = (float)y / (float)atlas_height;
        g->u1 = (float)(x + width) / (float)atlas_width;
        g->v1 = (float)(y + height) / (float)atlas_height;

        /*printf("Glyph '%c': size=(%d,%d), offset=(%d,%.1f), advance=%.1f, UV=(%.3f,%.3f)-(%.3f,%.3f)\n",
               c, width, height, xoff, g.yoff, g.advance, g.u0, g.v0, g.u1, g.v1);
               */

        x += width + 48; // Add 1 pixel padding
        if (height > rowHeight) rowHeight = height + 1;

        stbtt_FreeBitmap(bitmap, NULL);
    }

    free(ttf_buffer);

    // Convert to RGBA
    u64 atlasRGBA_size = atlas_width * atlas_height * 4;
    unsigned char* atlas_RGBA_pixels = arena_alloc(arena, atlasRGBA_size);
    for (int i = 0; i < atlas_width * atlas_height; i++)
    {
        unsigned char v = atlasPixels[i];
        atlas_RGBA_pixels[i * 4 + 0] = 255; // R
        atlas_RGBA_pixels[i * 4 + 1] = 255; // G
        atlas_RGBA_pixels[i * 4 + 2] = 255; // B
        atlas_RGBA_pixels[i * 4 + 3] = v; // A (alpha from glyph)
    }

    // Save atlas to file for debugging, will be under cmake-build-debug
    const char* debug_filename = "font_atlas_debug.ppm";
    FILE* debug_file = fopen(debug_filename, "wb");
    if (debug_file)
    {
        // Write PPM header (P6 format for RGB)
        // DEBUG(debug_file, "P6\n%d %d\n255\n", atlasWidth, atlasHeight);
        DEBUG("P6\n%d %d\n255\n", atlas_width, atlas_height);

        // Write RGB data to PPM filing using the debug data, which is basically not flipped
        for (int i = 0; i < atlas_width * atlas_height; i++)
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

    fwrite(atlas_RGBA_pixels, 1, atlasRGBA_size, raw_file);
    DEBUG("Raw RGBA atlas data saved to: %s (%dx%d RGBA)\n",
          raw_filename, atlas_width, atlas_height);
    fclose(raw_file);


    texture->width = atlas_width;
    texture->height = atlas_height;
    texture->channels = 4;
    texture->image_size = atlasRGBA_size;
    texture->pixels = atlas_RGBA_pixels;
    texture->handle = *out_handle;
    texture->is_font = true;
    ring_enqueue(texture_system->textures_ring_queue, texture);

    DEBUG("Font loaded successfully: %s\n", file_path);

    return out_handle;
}

bool texture_system_get_font(Texture_System* texture_system, const Texture_Handle handle, Madness_Font* out_font)
{
    *out_font = texture_system->font_array[handle.handle];
    return true;
}
