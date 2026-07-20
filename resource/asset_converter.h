#ifndef ASSET_CONVERTER_H
#define ASSET_CONVERTER_H

#include "asserts.h"
#include "asset_system.h"
#include "resource_types.h"


bool asset_converter_texture(Asset_System* asset_system, const char* file_path)
{
    Madness_Texture texture = {0};
    //load the image data from file
    int texWidth, texHeight, texChannels;
    u8* pixel_data = stbi_load(file_path, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

    MASSERT(pixel_data);

    //The pixels are laid out row by row with 4 bytes per pixel in the case of STBI_rgb_alpha for a total of texWidth * texHeight * 4 values.
    texture.width = texWidth; // 4 stride rgba
    texture.height = texHeight; // 4 stride rgba
    texture.channels = STBI_rgb_alpha; // 4 stride rgba
    texture.pixels_size = texWidth * texHeight * 4; // 4 stride rgba

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

    fwrite(&texture, sizeof(Madness_Texture), 1, fptr);
    fwrite(pixel_data, texture.pixels_size, 1, fptr);

    stbi_image_free(pixel_data);

    fclose(fptr);


    Asset_MetaData meta_data = asset_system->texture_meta_data[asset_system->texture_meta_data_count++];
    meta_data.source_file = c_string_duplicate(file_path);
    meta_data.hash_id = c_string_hash_u64(file_path);
    meta_data.type = ASSET_TEXTURE;
    meta_data.extra_data = 0;

    return true;
}




#endif
