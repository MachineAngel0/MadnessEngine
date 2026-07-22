#include "asset_converter.h"
#include "asserts.h"
#include "resource_types.h"
#include "stb_image.h"
#include "stb_truetype.h"

bool asset_convert_file(Asset_System* asset_system, const char* file_path)
{
    const char* extension_name =c_string_path_get_extension(file_path, asset_system->frame_allocator);
    if (strcmp(extension_name, ".png") == 0)
    {
        asset_converter_texture(asset_system, file_path);
        return true;
    }
    if (strcmp(extension_name, ".jpg") == 0)
    {
        asset_converter_texture(asset_system, file_path);
        return true;
    }
    if (strcmp(extension_name, ".png") == 0)
    {
        asset_converter_texture(asset_system, file_path);
        return true;
    }
    //TODO: system fonts like .ttf into msdf fonts
    if (strcmp(extension_name, ".gltf") == 0)
    {
        asset_converter_load_gltf_mesh(asset_system, file_path);
        return true;
    }


    WARN("ASSET CONVERT FILE: NO VALID FILE TYPE FOUND");
    return false;

}

bool asset_converter_texture(Asset_System* asset_system, const char* file_path)
{
    Madness_Texture_Editor editor_texture = {0};
    editor_texture.version = 1;
    //load the image data from file
    int texWidth, texHeight, texChannels;
    u8* pixel_data = stbi_load(file_path, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

    MASSERT(pixel_data);

    //The pixels are laid out row by row with 4 bytes per pixel in the case of STBI_rgb_alpha for a total of texWidth * texHeight * 4 values.
    editor_texture.texture.width = texWidth; // 4 stride rgba
    editor_texture.texture.height = texHeight; // 4 stride rgba
    editor_texture.texture.channels = STBI_rgb_alpha; // 4 stride rgba
    editor_texture.texture.pixels_size = texWidth * texHeight * 4; // 4 stride rgba
    editor_texture.texture.format = Texture_Format_Default;

    String_Builder* file_path_strip = string_builder_create(256, asset_system->frame_allocator);
    string_builder_append_c_string(file_path_strip, file_path);
    string_builder_strip_extension(file_path_strip);
    string_builder_strip_path(file_path_strip);

    String_Builder* str_builder = string_builder_create(256, asset_system->frame_allocator);
    string_builder_append_c_string(str_builder, ENGINE_TEXTURE_PATH);
    string_builder_append_builder(str_builder, file_path_strip);
    string_builder_append_c_string(str_builder, ENGINE_TEXTURE_EXTENSION);


    const char* output_path = string_builder_to_c_string(str_builder);
    FILE* fptr = fopen(output_path, "wb");
    if (!fptr)
    {
        MASSERT(false);
    }

    fwrite(&editor_texture, sizeof(Madness_Texture), 1, fptr);
    fwrite(&editor_texture.version, sizeof(editor_texture.version), 1, fptr);
    fwrite(pixel_data, editor_texture.texture.pixels_size, 1, fptr);

    stbi_image_free(pixel_data);

    fclose(fptr);


    Asset_MetaData meta_data = {0};
    meta_data.source_file = STRING_CREATE_FROM_BUFFER_HEAP_ALLOCATOR(file_path, asset_system->heap_allocator);
    meta_data.binary_file = string_builder_to_string(str_builder);
    meta_data.uuid = madness_uuid_generate_return();
    meta_data.type = ASSET_TEXTURE;
    asset_registry_add_asset(asset_system->asset_registry, &meta_data);


    return true;
}


bool asset_converter_font(Asset_System* asset_system, const char* file_path)
{
    Madness_Font font_structure = {0};

    // Load font file
    FILE* font_file = fopen(file_path, "rb");
    if (!font_file)
    {
        MASSERT(false);
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
        MASSERT(false);
        return false;
    }

    // Generate bitmap atlas
    float scale = stbtt_ScaleForPixelHeight(&font_info, DEFAULT_FONT_CREATION_SIZE);
    int atlas_width = 1024 * 4;
    int atlas_height = 1024 * 4;
    char* atlasPixels = allocator_alloc(asset_system->frame_allocator, atlas_width * atlas_height);

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
            MASSERT(false);
            return false;
        }

        // Copy glyph bitmap into atlas
        for (int row = 0; row < height; row++)
        {
            memcpy(&atlasPixels[(y + row) * atlas_width + x],
                   &bitmap[row * width], width);
        }


        // Store glyph metrics
        Glyph* g = &font_structure.glyphs[c - 32];
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
    unsigned char* atlas_RGBA_pixels = allocator_alloc(asset_system->frame_allocator, atlasRGBA_size);
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


    font_structure.texture.width = atlas_width;
    font_structure.texture.height = atlas_height;
    font_structure.texture.channels = 4;
    font_structure.texture.pixels_size = atlasRGBA_size;
    // font_structure.texture.pixels = atlas_RGBA_pixels;


    DEBUG("Font loaded successfully: %s\n", file_path);


    //write it out to the file
    String_Builder* file_path_strip = string_builder_create(256, asset_system->frame_allocator);
    string_builder_append_c_string(file_path_strip, file_path);
    string_builder_strip_extension(file_path_strip);
    string_builder_strip_path(file_path_strip);

    String_Builder* str_builder = string_builder_create(256, asset_system->frame_allocator);
    string_builder_append_c_string(str_builder, ENGINE_FONTS_PATH);
    string_builder_append_builder(str_builder, file_path_strip);
    string_builder_append_c_string(str_builder, ENGINE_FONTS_EXTENSION);

    const char* output_path = string_builder_to_c_string(str_builder);
    FILE* fptr = fopen(output_path, "wb");
    if (!fptr)
    {
        MASSERT(false);
    }

    fwrite(&font_structure, sizeof(font_structure), 1, fptr);
    fwrite(atlas_RGBA_pixels, font_structure.texture.pixels_size, 1, fptr);

    fclose(fptr);


    Asset_MetaData meta_data = {0};
    meta_data.source_file = STRING_CREATE_FROM_BUFFER_HEAP_ALLOCATOR(file_path, asset_system->heap_allocator);
    meta_data.binary_file = string_builder_to_string(str_builder);
    meta_data.uuid = madness_uuid_generate_return();
    meta_data.type = ASSET_FONT;
    asset_registry_add_asset(asset_system->asset_registry, &meta_data);

    return true;
}

bool asset_converter_msdf_font(Asset_System* asset_system, const char* file_path)
{
    Madness_Font font_structure = {0};

    //load the image data from file
    //load the image data from file
    int texture_width, texture_height, texChannels;
    u8* pixel_data = stbi_load(file_path, &texture_width, &texture_height, &texChannels, STBI_rgb_alpha);

    if (!pixel_data)
    {
        WARN("TEXTURE SYSTEM LOAD TEXTURE: failed to load texture image!");
        MASSERT_MSG(pixel_data, "FAILED TO LOAD MSDF FONT");
        return false;
    }

    //The pixels are laid out row by row with 4 bytes per pixel in the case of STBI_rgb_alpha for a total of texWidth * texHeight * 4 values.
    font_structure.texture.width = texture_width; // 4 stride rgba
    font_structure.texture.height = texture_height; // 4 stride rgba
    font_structure.texture.channels = STBI_rgb_alpha; // 4 stride rgba
    font_structure.texture.pixels_size = texture_width * texture_height * 4; // 4 stride rgba


    const char* file_name = c_string_ext_strip(file_path, asset_system->frame_allocator);
    const char* csv_path = c_string_concat(file_name, "csv", asset_system->frame_allocator);

    FILE* file = fopen(csv_path, "r");
    MASSERT(file)


    // float texture_size = 256.f; // this can be gotten from stbi -> 256*256 rgba(*4)
    // float glyph_size = 32.f; // this you would just have to know, or force it based on texture_size

    float texture_size = 256.f; // this can be gotten from stbi -> 256*256 rgba(*4)
    float glyph_size = 40.f; // this you would just have to know, or force it based on texture_size

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

        Glyph* g = &font_structure.glyphs[index - GLYPH_START];

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
    float ascender_px = ascender * glyph_size;
    for (int i = 0; i < GLYPH_LENGTH; i++)
    {
        font_structure.glyphs[i].yoff -= ascender_px;
    }


    //write it out to the file
    String_Builder* file_path_strip = string_builder_create(256, asset_system->frame_allocator);
    string_builder_append_c_string(file_path_strip, file_path);
    string_builder_strip_extension(file_path_strip);
    string_builder_strip_path(file_path_strip);

    String_Builder* str_builder = string_builder_create(256, asset_system->frame_allocator);
    string_builder_append_c_string(str_builder, ENGINE_FONTS_PATH);
    string_builder_append_builder(str_builder, file_path_strip);
    string_builder_append_c_string(str_builder, ENGINE_FONTS_EXTENSION);

    const char* output_path = string_builder_to_c_string(str_builder);
    FILE* fptr = fopen(output_path, "wb");
    if (!fptr)
    {
        MASSERT(false);
    }

    fwrite(&font_structure, sizeof(font_structure), 1, fptr);
    fwrite(pixel_data, font_structure.texture.pixels_size, 1, fptr);

    stbi_image_free(pixel_data);

    fclose(fptr);

    Asset_MetaData meta_data = {0};
    meta_data.source_file = STRING_CREATE_FROM_BUFFER_HEAP_ALLOCATOR(file_path, asset_system->heap_allocator);
    meta_data.binary_file = string_builder_to_string(str_builder);
    meta_data.uuid = madness_uuid_generate_return();
    meta_data.type = ASSET_FONT;
    asset_registry_add_asset(asset_system->asset_registry, &meta_data);

    return true;
}
