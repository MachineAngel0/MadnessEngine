#include "texture_system.h"


Texture_System* texture_system_init(Memory_System* memory_system)
{
    Texture_System* texture_system = memory_system_alloc(memory_system, sizeof(Texture_System),
                                                         MEMORY_SUBSYSTEM_TEXTURE);


    texture_system->in_use_textures_count = 0;
    texture_system->max_textures = MAX_TEXTURE_COUNT;

    memset(texture_system->textures_array, 0, MAX_TEXTURE_COUNT * sizeof(Texture));
    memset(texture_system->font_array, 0, MAX_TEXTURE_COUNT * sizeof(Madness_Font));
    memset(texture_system->texture_handles, 0, MAX_TEXTURE_COUNT * sizeof(Texture_Handle));

    texture_system->available_idx_queue = ring_queue_create(sizeof(u32), MAX_TEXTURE_COUNT);

    for (u32 i = 0; i < MAX_TEXTURE_COUNT; i++)
    {
        texture_system->texture_handles[i].handle = i;
        ring_enqueue(texture_system->available_idx_queue, &i);
    }


    texture_system->texture_filepath_to_idx = HASH_TABLE_CREATE(u32, MAX_TEXTURE_COUNT);
    texture_system->textures_ring_queue = ring_queue_create(sizeof(Texture), MAX_TEXTURE_COUNT);


    //create our debug texture
    if (!texture_system_load_texture(texture_system, "../renderer/texture/error_texture.png",
                                     &texture_system->default_texture_handle))
    {
        MASSERT_MSG(false, "TEXTURE SYSTEM FAILED TO LOAD DEFAULT TEXTURE");
    };

    return texture_system;
}

bool texture_system_shutdown(Texture_System* texture_system, Memory_System* memory_system)
{
    //TODO: unload all the textures and dsa's;
    MASSERT(texture_system);
    memory_system_memory_free(memory_system, texture_system);

    return true;
}


//pass out the texture index
bool texture_system_load_texture(Texture_System* texture_system, char const* file_path, Texture_Handle* out_handle)
{
    u32 texture_idx;

    //check if the texture has already been loaded in
    if (hash_table_get(texture_system->texture_filepath_to_idx, file_path, &texture_idx))
    {
        out_handle = &texture_system->texture_handles[texture_idx];
        texture_system->texture_meta_data[texture_idx].reference_count++;
        return true;
    }

    if (texture_system->in_use_textures_count >= texture_system->max_textures)
    {
        FATAL("MAX TEXTURES REACHED");
        out_handle = texture_system_get_default_texture(texture_system);
        return false;
    }

    //get a free index
    u32 free_index;
    if (!ring_dequeue(texture_system->available_idx_queue, &free_index))
    {
        MASSERT("OUT OF TEXTURE IDX's")
    }
    texture_system->in_use_textures_count++;

    //get an available index
    Resource_MetaData* meta_data = &texture_system->texture_meta_data[free_index];
    meta_data->type = RESOURCE_TEXTURE;
    meta_data->id = free_index;
    meta_data->file_path = file_path;
    meta_data->reference_count = 1;


    *out_handle = texture_system->texture_handles[free_index];
    Texture* texture = &texture_system->textures_array[free_index];
    texture->handle = *out_handle;


    //add to hash table
    hash_table_insert(texture_system->texture_filepath_to_idx, file_path, &free_index);

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
        out_handle = texture_system_get_default_texture(texture_system);
        return false;
    }


    //used by the renderer to upload to the gpu, then we can free the cpu data
    ring_enqueue(texture_system->textures_ring_queue, texture);


    return true;
}

bool texture_system_unload_texture(Texture_System* texture_system, Texture_Handle handle)
{
    if (handle.handle == 0)
    {
        WARN("texture_system_unload_texture: TRIED UNLOADING DEFAULT TEXTURE")
        return false;
    }

    //get the handle for use
    u32 texture_id = handle.handle;


    Resource_MetaData* texture_metadata = &texture_system->texture_meta_data[texture_id];
    texture_metadata->reference_count--;
    if (texture_metadata->reference_count <= 0)
    {
        //change the resource generation
        texture_system->texture_handles[texture_id].generation++;
        //put it into the available list
        ring_enqueue(texture_system->available_idx_queue, &texture_id);
        texture_system->in_use_textures_count--;
    }

    return true;
}

bool texture_system_get_texture(Texture_System* texture_system, Texture_Handle handle, Texture* out_texture)
{
    if (texture_system->texture_handles->generation != handle.generation)
    {
        out_texture = NULL;
        return false;
    }

    out_texture = &texture_system->textures_array[handle.handle];
    return true;
}

Texture_Handle texture_system_update_texture(Texture_System* texture_system, Texture_Handle handle,
                                             const char* filepath)
{
    UNIMPLEMENTED();
    return (Texture_Handle){0};
}


Texture_Handle* texture_system_get_default_texture(Texture_System* texture_system)
{
    return &texture_system->default_texture_handle;
}

bool texture_system_load_font(Texture_System* texture_system, const char* file_path, Texture_Handle* out_handle,
                              Arena* arena)
{
    u32 texture_id;

    if (hash_table_get(texture_system->texture_filepath_to_idx, file_path, &texture_id))
    {
        out_handle = &texture_system->texture_handles[texture_id];
        texture_system->texture_meta_data[texture_id].reference_count++;
        return true;
    }

    if (texture_system->in_use_textures_count >= texture_system->max_textures)
    {
        FATAL("MAX TEXTURES REACHED");
        out_handle->handle = 0;
        return false;
    }


    //get a free index
    u32 free_index;
    if (!ring_dequeue(texture_system->available_idx_queue, &free_index))
    {
        MASSERT("OUT OF TEXTURE IDX's")
        return false;
    }
    texture_system->in_use_textures_count++;

    *out_handle = texture_system->texture_handles[free_index];
    Madness_Font* font_structure = &texture_system->font_array[free_index];
    Texture* texture = &texture_system->textures_array[free_index];
    texture->handle = *out_handle;

    //add to hash table
    hash_table_insert(texture_system->texture_filepath_to_idx, file_path, &free_index);

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
    stbtt_fontinfo font_info;
    if (!stbtt_InitFont(&font_info, ttf_buffer, stbtt_GetFontOffsetForIndex(ttf_buffer, 0)))
    {
        WARN("Failed to initialize font\n");
        free(ttf_buffer);
        return out_handle;
    }

    // Generate bitmap atlas
    float scale = stbtt_ScaleForPixelHeight(&font_info, DEFAULT_FONT_CREATION_SIZE);
    int atlas_width = 1024 * 4;
    int atlas_height = 1024 * 4;
    char* atlasPixels = arena_alloc(arena, atlas_width * atlas_height);

    int x = 0, y = 0, rowHeight = 0;

    // Get font metrics for baseline calculation
    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&font_info, &ascent, &descent, &lineGap);
    float baseline = ascent * scale;


    for (int c = 32; c < 128; c++)
    {
        int width, height, xoff, yoff;
        unsigned char* bitmap = stbtt_GetCodepointBitmap(&font_info, 0, scale, c,
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
        stbtt_GetCodepointHMetrics(&font_info, c, &advance, &lsb);
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

bool texture_system_unload_font(Texture_System* texture_system, Texture_Handle handle)
{
    //get the handle for use
    u32 texture_id = handle.handle;

    if (texture_id == 0)
    {
        WARN("texture_system_unload_texture: TRIED UNLOADING DEFAULT TEXTURE")
        return false;
    }

    Resource_MetaData* texture_metadata = &texture_system->texture_meta_data[texture_id];
    texture_metadata->reference_count--;
    if (texture_metadata->reference_count <= 0)
    {
        //change the resource generation
        texture_system->texture_handles[texture_id].generation++;
        //put it into the available list
        ring_enqueue(texture_system->available_idx_queue, &texture_id);
        texture_system->in_use_textures_count--;
    }

    return true;
}

bool texture_system_get_font(Texture_System* texture_system, const Texture_Handle handle, Madness_Font* out_font)
{
    *out_font = texture_system->font_array[handle.handle];
    return true;
}

bool texture_system_load_msdf_font(Texture_System* texture_system, const char* file_path, Texture_Handle* out_handle,
                                   Frame_Arena* frame_arena)
{
    u32 texture_idx;

    //check if the texture has already been loaded in
    if (hash_table_get(texture_system->texture_filepath_to_idx, file_path, &texture_idx))
    {
        out_handle = &texture_system->texture_handles[texture_idx];
        texture_system->texture_meta_data[texture_idx].reference_count++;
        return true;
    }

    if (texture_system->in_use_textures_count >= texture_system->max_textures)
    {
        FATAL("MAX TEXTURES REACHED");
        out_handle = texture_system_get_default_texture(texture_system);
        return false;
    }

    //get a free index
    u32 free_index;
    if (!ring_dequeue(texture_system->available_idx_queue, &free_index))
    {
        MASSERT("OUT OF TEXTURE IDX's")
    }
    texture_system->in_use_textures_count++;

    //get an available index
    Resource_MetaData* meta_data = &texture_system->texture_meta_data[free_index];
    meta_data->type = RESOURCE_FONT;
    meta_data->id = free_index;
    meta_data->file_path = file_path;
    meta_data->reference_count = 1;


    *out_handle = texture_system->texture_handles[free_index];
    Texture* texture = &texture_system->textures_array[free_index];
    Madness_Font* font_structure = &texture_system->font_array[free_index];
    texture->handle = *out_handle;

    //add to hash table
    hash_table_insert(texture_system->texture_filepath_to_idx, file_path, &free_index);

    //load the image data from file
    int texture_width, texture_height, tex_channels;
    texture->pixels = stbi_load(file_path, &texture_width, &texture_height, &tex_channels, STBI_rgb_alpha);
    //The pixels are laid out row by row with 4 bytes per pixel in the case of STBI_rgb_alpha for a total of texWidth * texHeight * 4 values.
    texture->width = texture_width; // 4 stride rgba
    texture->height = texture_height; // 4 stride rgba
    texture->channels = STBI_rgb_alpha; // 4 stride rgba
    texture->image_size = texture_width * texture_height * 4; // 4 stride rgba
    texture->is_font = false;

    MASSERT_MSG(texture->pixels, "FAILED TO LOAD FONT");

    if (!texture->pixels)
    {
        WARN("TEXTURE SYSTEM LOAD TEXTURE: failed to load texture image!");
        out_handle = texture_system_get_default_texture(texture_system);
        return false;
    }


    //used by the renderer to upload to the gpu, then we can free the cpu data
    ring_enqueue(texture_system->textures_ring_queue, texture);


    char* file_name = c_string_ext_strip(file_path, frame_arena);
    char* csv_path = c_string_concat(file_name, "csv", frame_arena);

    FILE* file = fopen(csv_path, "r");
    MASSERT(file)


    float texture_size = 256.f; // this can be gotten from stbi -> 256*256 rgba(*4)
    float glyph_size = 32.f; // this you would just have to know, or force it based on texture_size

    char buffer[256];
    float ascender = 0.0f;

    while (fgets(buffer, sizeof(buffer), file))
    {
        int index;
        float advance, bound_left, bound_top, bound_right, bound_bottom, u0, v0, u1, v1;

        int parsed = sscanf(buffer, "%d,%f,%f,%f,%f,%f,%f,%f,%f,%f",
                            &index,
                            &advance,
                            &bound_left, &bound_top, &bound_right, &bound_bottom,
                            &u0, &v0, &u1, &v1
        );

        if (bound_top < ascender) { ascender = bound_top; };

        Glyph* g = &font_structure->glyphs[index - GLYPH_START];

        g->advance = advance * glyph_size;
        g->xoff = bound_left * glyph_size;
        g->yoff = bound_top * glyph_size;
        g->width = (bound_right - bound_left) * glyph_size;
        g->height = (bound_bottom - bound_top) * glyph_size;
        g->u0 = u0 / texture_size;
        g->v0 = v0 / texture_size;
        g->u1 = u1 / texture_size;
        g->v1 = v1 / texture_size;
    }

    // bake ascender into yoff so draw_text needs no correction
    float ascender_px = ascender * 32.0f;
    for (int i = 0; i < GLYPH_LENGTH; i++)
    {
        font_structure->glyphs[i].yoff -= ascender_px;
    }

    return true;
}
