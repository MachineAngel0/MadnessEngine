#include "asset_converter.h"
#include "asserts.h"
#include "resource_types.h"
#include "stb_image.h"
#include "stb_truetype.h"

bool asset_convert_file(Asset_System* asset_system, const char* file_path, MADNESS_UUID* out_uuid)
{
    const char* extension_name = c_string_path_get_extension(file_path, asset_system->frame_allocator);
    if (strcmp(extension_name, ".png") == 0)
    {
        asset_converter_texture(asset_system, file_path, out_uuid);
        return true;
    }
    if (strcmp(extension_name, ".jpg") == 0)
    {
        asset_converter_texture(asset_system, file_path, out_uuid);
        return true;
    }
    //TODO: system fonts like .ttf into msdf fonts
    if (strcmp(extension_name, ".gltf") == 0)
    {
        asset_converter_gltf_mesh(asset_system, file_path);
        return true;
    }


    WARN("ASSET CONVERT FILE: NO VALID FILE EXT FOUND");
    return false;
}

bool asset_converter_texture(Asset_System* asset_system, const char* file_path, MADNESS_UUID* out_uuid)
{
    if (out_uuid)
    {
        if (asset_registry_exists_by_source_path(asset_system,
                                                 STRING_CREATE_FROM_BUFFER_ALLOCATOR(
                                                     file_path, asset_system->frame_allocator),
                                                 out_uuid))
        {
            return true;
        }
    }


    Madness_Texture_Runtime runtime_texture = {0};
    runtime_texture.version = 1;
    //load the image data from file
    int texWidth, texHeight, texChannels;
    u8* pixel_data = stbi_load(file_path, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

    MASSERT(pixel_data);

    //The pixels are laid out row by row with 4 bytes per pixel in the case of STBI_rgb_alpha for a total of texWidth * texHeight * 4 values.
    runtime_texture.texture.width = texWidth; // 4 stride rgba
    runtime_texture.texture.height = texHeight; // 4 stride rgba
    runtime_texture.texture.channels = STBI_rgb_alpha; // 4 stride rgba
    runtime_texture.texture.pixels_size = texWidth * texHeight * 4; // 4 stride rgba
    runtime_texture.texture.format = Texture_Format_Default;
    runtime_texture.texture.type = ASSET_FONT;
    runtime_texture.pixel_data = pixel_data;

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

    asset_texture_serialize(&runtime_texture, fptr);

    stbi_image_free(pixel_data);

    fclose(fptr);


    Asset_MetaData meta_data = {0};
    meta_data.source_file = STRING_CREATE_FROM_BUFFER_HEAP_ALLOCATOR(file_path, asset_system->heap_allocator);
    meta_data.binary_file = string_builder_to_string(str_builder);
    meta_data.uuid = madness_uuid_generate_return();
    meta_data.hash = madness_uuid_hash(&meta_data.uuid);
    meta_data.type = ASSET_TEXTURE;
    asset_registry_add_asset(asset_system->asset_registry, &meta_data);

    if (out_uuid)
    {
        *out_uuid = meta_data.uuid;
    }

    return true;
}


bool asset_converter_font(Asset_System* asset_system, const char* file_path)
{
    Madness_Font font_structure = {0};
    Madness_Texture texture = {0};

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


    texture.width = atlas_width;
    texture.height = atlas_height;
    texture.channels = 4;
    texture.pixels_size = atlasRGBA_size;
    texture.type = ASSET_FONT;


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

    u8 version = 1;

    // asset_font_serialize(&font_structure, fptr);

    fwrite(&version, sizeof(version), 1, fptr);
    fwrite(&font_structure, sizeof(Madness_Font), 1, fptr);
    fwrite(&texture, sizeof(Madness_Texture), 1, fptr);
    fwrite(atlas_RGBA_pixels, texture.pixels_size, 1, fptr);


    fclose(fptr);


    Asset_MetaData meta_data = {0};
    meta_data.source_file = STRING_CREATE_FROM_BUFFER_HEAP_ALLOCATOR(file_path, asset_system->heap_allocator);
    meta_data.binary_file = string_builder_to_string(str_builder);
    meta_data.uuid = madness_uuid_generate_return();
    meta_data.hash = madness_uuid_hash(&meta_data.uuid);
    meta_data.type = ASSET_FONT;
    asset_registry_add_asset(asset_system->asset_registry, &meta_data);

    return true;
}

bool asset_converter_msdf_font(Asset_System* asset_system, const char* file_path)
{
    Madness_Font_Runtime engine_texture = {0};
    engine_texture.version = 1.0;

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
    engine_texture.texture.width = texture_width; // 4 stride rgba
    engine_texture.texture.height = texture_height; // 4 stride rgba
    engine_texture.texture.channels = STBI_rgb_alpha; // 4 stride rgba
    engine_texture.texture.pixels_size = texture_width * texture_height * 4; // 4 stride rgba
    engine_texture.texture.type = ASSET_FONT;
    engine_texture.pixel_data = pixel_data;

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

        Glyph* g = &engine_texture.font_texture.glyphs[index - GLYPH_START];

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
        engine_texture.font_texture.glyphs[i].yoff -= ascender_px;
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

    asset_font_serialize(&engine_texture, fptr);

    stbi_image_free(pixel_data);

    fclose(fptr);

    Asset_MetaData meta_data = {0};
    meta_data.source_file = STRING_CREATE_FROM_BUFFER_HEAP_ALLOCATOR(file_path, asset_system->heap_allocator);
    meta_data.binary_file = string_builder_to_string(str_builder);
    meta_data.uuid = madness_uuid_generate_return();
    meta_data.hash = madness_uuid_hash(&meta_data.uuid);
    meta_data.type = ASSET_FONT;
    asset_registry_add_asset(asset_system->asset_registry, &meta_data);

    return true;
}

bool asset_converter_gltf_mesh(Asset_System* asset_system, const char* gltf_path)
{
    Frame_Allocator* frame_allocator = asset_system->frame_allocator;

    if (!c_string_path_is_extension(gltf_path, ".gltf") && !c_string_path_is_extension(gltf_path, ".glb"))
    {
        FATAL("DID NOT PASS IN A GLTF FILE");
        return false;
    }


    cgltf_options options = {0};
    cgltf_data* data = NULL;
    cgltf_result result = cgltf_parse_file(&options, gltf_path, &data);

    if (result != cgltf_result_success)
    {
        fprintf(stderr, "Failed to parse glTF file: %s\n", gltf_path);
        return false;
    }
    result = cgltf_load_buffers(&options, data, gltf_path);
    MASSERT(result == cgltf_result_success)


    Madness_Mesh* madness_mesh = allocator_alloc(asset_system->frame_allocator,
                                           sizeof(Madness_Mesh));

    madness_mesh->mesh_count = data->meshes_count;
    madness_mesh->mesh_data = allocator_alloc(asset_system->frame_allocator,
                                         sizeof(Madness_SubMesh) * data->meshes_count);

    Madness_Mesh_GPU_Data* gpu_data = allocator_alloc(asset_system->frame_allocator,
                                       sizeof(Madness_Mesh_GPU_Data) * data->meshes_count);
    Material_Instance* material_instances = allocator_alloc(asset_system->frame_allocator,
                                   sizeof(Material_Instance) * data->meshes_count);

    //check if we are loading a skinned or normal mesh
    Madness_Skinned_SubMesh* skinned_mesh = allocator_alloc(asset_system->frame_allocator,
                                           sizeof(Madness_Skinned_SubMesh) * data->meshes_count);
    Madness_SkMesh_GPU_Data* skinned_gpu_data = allocator_alloc(asset_system->frame_allocator,
                                       sizeof(Madness_SkMesh_GPU_Data) * data->meshes_count);
    GLTF_Animation_Data* animation_data = allocator_alloc(asset_system->frame_allocator,
                                       sizeof(GLTF_Animation_Data));
    if (data->skins_count > 0)
    {
        skinned_mesh = allocator_alloc(asset_system->frame_allocator,
                                               sizeof(Madness_Skinned_SubMesh) * data->meshes_count);
        skinned_gpu_data = allocator_alloc(asset_system->frame_allocator,
                                           sizeof(Madness_SkMesh_GPU_Data) * data->meshes_count);
        animation_data = allocator_alloc(asset_system->frame_allocator,
                                           sizeof(GLTF_Animation_Data));
    }


     Material_Default* default_mats = allocator_alloc(asset_system->frame_allocator, sizeof(Material_Default) * data->meshes_count);

    for (size_t mesh_idx = 0; mesh_idx < data->meshes_count; mesh_idx++)
    {
        Madness_SubMesh* submesh = &madness_mesh->mesh_data[mesh_idx];
        Madness_Mesh_GPU_Data* submesh_gpu = &gpu_data[mesh_idx];


        /* Find position accessor */
        const cgltf_accessor* pos_accessor = cgltf_find_accessor(data->meshes[mesh_idx].primitives,
                                                                 cgltf_attribute_type_position,
                                                                 0);
        //only neccesary attribute which is basically manadatory for us to have
        MASSERT(pos_accessor);
        if (pos_accessor)
        {
            //get size information
            cgltf_size num_floats = cgltf_accessor_unpack_floats(pos_accessor, NULL, 0);
            cgltf_size float_bytes = num_floats * sizeof(float);
            submesh->vertex_bytes = float_bytes;
            submesh->vertex_count = submesh->vertex_bytes/sizeof(vec3s);

            //alloc and copy data
            float* pos_data = allocator_alloc(frame_allocator, float_bytes);
            submesh_gpu->vertex = allocator_alloc(frame_allocator, float_bytes);
            cgltf_accessor_unpack_floats(pos_accessor, pos_data, num_floats);
            memcpy(submesh_gpu->vertex, pos_data, float_bytes);
        }

        // Find normal accessor
        const cgltf_accessor* norm_accessor = cgltf_find_accessor(data->meshes[mesh_idx].primitives,
                                                                  cgltf_attribute_type_normal,
                                                                  0);
        if (norm_accessor)
        {
            //get size information
            cgltf_size norm_floats = cgltf_accessor_unpack_floats(norm_accessor, NULL, 0);
            cgltf_size norm_bytes = norm_floats * sizeof(float);
            submesh->normal_bytes = norm_bytes;

            //alloc and copy data
            float* normal_data = allocator_alloc(frame_allocator, norm_bytes);
            submesh_gpu->normal = allocator_alloc(frame_allocator, norm_bytes);
            cgltf_accessor_unpack_floats(norm_accessor, normal_data, norm_floats);
            memcpy(submesh_gpu->normal, normal_data, norm_bytes);
        }

        //  Find tangent accessor
        const cgltf_accessor* tangent_accessor = cgltf_find_accessor(data->meshes[mesh_idx].primitives,
                                                                     cgltf_attribute_type_tangent, 0);

        if (tangent_accessor)
        {
            //get size information
            cgltf_size tangent_floats = cgltf_accessor_unpack_floats(tangent_accessor, NULL, 0);
            cgltf_size tangent_bytes = tangent_floats * sizeof(float);
            submesh->tangent_bytes = tangent_bytes;


            //alloc and copy data
            float* tangent_data = allocator_alloc(frame_allocator, tangent_bytes);
            submesh_gpu->tangent = allocator_alloc(frame_allocator, tangent_bytes);
            cgltf_accessor_unpack_floats(tangent_accessor, tangent_data, tangent_floats);
            memcpy(submesh_gpu->tangent, tangent_data, tangent_bytes);
        }

        //  Find texcoord accessor
        const cgltf_accessor* texcoord_accessor = cgltf_find_accessor(data->meshes[mesh_idx].primitives,
                                                                      cgltf_attribute_type_texcoord, 0);
        if (texcoord_accessor)
        {
            //get size information
            cgltf_size uv_floats_count = cgltf_accessor_unpack_floats(texcoord_accessor, NULL, 0);
            cgltf_size uv_byte_size = uv_floats_count * sizeof(float);
            submesh->uv_bytes = uv_byte_size;


            //alloc and copy data
            float* uv_data = allocator_alloc(frame_allocator, uv_byte_size);
            cgltf_accessor_unpack_floats(texcoord_accessor, uv_data, uv_floats_count);
            submesh_gpu->uv = allocator_alloc(frame_allocator, uv_byte_size);
            memcpy(submesh_gpu->uv, uv_data, uv_byte_size);
        }

        const cgltf_accessor* color_accessor = cgltf_find_accessor(data->meshes[mesh_idx].primitives,
                                                                   cgltf_attribute_type_color, 0);
        if (color_accessor)
        {
            //get size information
            cgltf_size color_floats_count = cgltf_accessor_unpack_floats(texcoord_accessor, NULL, 0);
            cgltf_size color_byte_size = color_floats_count * sizeof(float);
            submesh->vertex_color_bytes = color_byte_size;

            //alloc and copy data
            float* vertex_color_data = allocator_alloc(frame_allocator, color_byte_size);
            cgltf_accessor_unpack_floats(texcoord_accessor, vertex_color_data, color_floats_count);
            submesh_gpu->vertex_color = allocator_alloc(frame_allocator, color_byte_size);
            memcpy(submesh_gpu->vertex_color, vertex_color_data, color_byte_size);
        }
        else
        {
            //TODO: generate white/black colors for everything i guess
        }
        // Load indices
        // SEE componentType in the specs for more detail 3.6.2
        u8 index_stride = data->meshes[mesh_idx].primitives[0].indices->stride;
        if (index_stride == 2)
        {
            submesh->index_type = INDEX_TYPE_U16;
        }
        else if (index_stride == 4)
        {
            submesh->index_type = INDEX_TYPE_U16;
        }
        else
        {
            WARN("GLTF MESH LOADING: UNKNOWN INDEX TYPE STRIDE");
        }

        //TODO: there can be multiple primitices/indices, will come back to
        submesh->indices_bytes = data->meshes[mesh_idx].primitives->indices->count *
            index_stride;
        submesh_gpu->indices = allocator_alloc(frame_allocator,
                                           submesh->indices_bytes);
        submesh->index_count = submesh->indices_bytes/index_stride;


        const uint8_t* index_buffer_data = cgltf_buffer_view_data(
            data->meshes[mesh_idx].primitives->indices->buffer_view);
        memcpy(submesh_gpu->indices, index_buffer_data,
               submesh->indices_bytes);


        //LOAD TEXTURES/MATERIALS
        // GET BASE PATH
        char* base_path = c_string_path_strip(gltf_path, frame_allocator);

        //every mesh just gets loaded in with a default pbr, well convert the material later into a custom format
        Material_Default* cur_mat = &default_mats[mesh_idx];
        if (!data->meshes[mesh_idx].primitives->material)
        {
            MASSERT(false)
            WARN("NO MATERIAL DATA FOUND FOR GLTF MESH");
            return false;
        }

        //COLOR TEXTURE
        cgltf_texture* color_texture = data->meshes[mesh_idx].primitives->material->pbr_metallic_roughness.
                                                              base_color_texture.texture;
        if (color_texture && color_texture->image->uri)
        {
            cur_mat->flags |= MESH_PIPELINE_COLOR;
            size_t allocation_size = strlen(base_path) + strlen(color_texture->image->uri) + 1;

            char* texture_path = allocator_alloc(frame_allocator,
                                                 allocation_size);
            // takes a buffer, message format, then the remaining strings
            snprintf(texture_path, allocation_size, "%s%s", base_path, color_texture->image->uri);
            TRACE("COLOR Texture Path:  %s", texture_path);
            asset_converter_texture(asset_system, texture_path, &cur_mat->color_texture);
            memcpy(cur_mat->color.raw,
                   data->meshes[mesh_idx].primitives->material->pbr_metallic_roughness.base_color_factor,
                   sizeof(vec4s));


            //base color
            TRACE("No Color Texture using fall back color");
            memcpy(cur_mat->color.raw,
                   data->meshes[mesh_idx].primitives->material->pbr_metallic_roughness.base_color_factor,
                   sizeof(vec4s));

        }
        else
        {
            // default_material->flags |= MESH_PIPELINE_COLOR;
            TRACE("No Color Texture using fall back color");
            memcpy(cur_mat->color.raw,
                   data->meshes[mesh_idx].primitives->material->pbr_metallic_roughness.base_color_factor,
                   sizeof(vec4s));
        }

        //METAL-ROUGHNESS
        cgltf_texture* metal_roughness_texture = data->meshes[mesh_idx].
                                                 primitives->material->pbr_metallic_roughness.
                                                 metallic_roughness_texture.texture;
        if (metal_roughness_texture)
        {
            if (metal_roughness_texture->image->uri)
            {
                cur_mat->flags |= MESH_PIPELINE_ROUGHNESS;
                cur_mat->flags |= MESH_PIPELINE_METALLIC;
                size_t allocation_size = strlen(base_path) +
                    strlen(metal_roughness_texture->image->uri) + 1;
                char* texture_path = allocator_alloc(frame_allocator, allocation_size);
                // takes a buffer, message format, then the remaining strings
                snprintf(texture_path, allocation_size, "%s%s", base_path, metal_roughness_texture->image->uri);
                TRACE("METAL/ROUGHNESS Texture Path:  %s", texture_path);

                asset_converter_texture(asset_system, texture_path, &cur_mat->roughness_texture);
                asset_converter_texture(asset_system, texture_path, &cur_mat->metallic_texture);
            }
            if (metal_roughness_texture->image->buffer_view)
            {
                //TODO: load texture data from the shader system
                INFO("WHOA A BUFFER VIEW ");
                const u8* metal_roughness_texture_image_data = cgltf_buffer_view_data(
                    metal_roughness_texture->image->buffer_view);
                INFO("WHOA A BUFFER VIEW ");
            }
        }

        // AO
        //NOTE: this material in theory can be included in the pbr-metal-roughness texture, in which case, just return a handle
        cgltf_texture* AO_texture = data->meshes[mesh_idx].primitives->material->occlusion_texture.texture;
        if (AO_texture && AO_texture->image->uri)
        {
            cur_mat->flags |= MESH_PIPELINE_AO;
            size_t allocation_size = strlen(base_path) + strlen(AO_texture->image->uri) + 1;
            char* AO_texture_path = allocator_alloc(frame_allocator, allocation_size);
            // takes a buffer, message format, then the remaining strings
            snprintf(AO_texture_path, allocation_size, "%s%s", base_path, AO_texture->image->uri);

            TRACE("AO Texture Path:  %s", AO_texture_path);

            asset_converter_texture(asset_system, AO_texture_path, &cur_mat->ambient_occlusion_texture);
        }


        //NORMAL TEXTURE
        // data->meshes[mesh_idx].primitives->material->has_pbr_metallic_roughness
        cgltf_texture* normal_texture = data->meshes[mesh_idx].primitives->material->normal_texture.texture;
        if (normal_texture && normal_texture->image->uri)
        {
            cur_mat->flags |= MESH_PIPELINE_NORMAL;
            size_t allocation_size = strlen(base_path) + strlen(normal_texture->image->uri) + 1;
            char* texture_path = allocator_alloc(frame_allocator, allocation_size);
            // takes a buffer, message format, then the remaining strings
            snprintf(texture_path, allocation_size, "%s%s", base_path, normal_texture->image->uri);
            TRACE("NORMAL Texture Path:  %s", texture_path);
            asset_converter_texture(asset_system, texture_path, &cur_mat->normal_texture);
        }

        //EMISSIVE TEXTURE
        cgltf_texture* emissive_texture = data->meshes[mesh_idx].primitives->material->emissive_texture.
                                                                 texture;
        if (emissive_texture && emissive_texture->image->uri)
        {
            cur_mat->flags |= MESH_PIPELINE_EMISSIVE;
            size_t allocation_size = strlen(base_path) + strlen(emissive_texture->image->uri) + 1;

            char* texture_path = allocator_alloc(frame_allocator, allocation_size);
            // takes a buffer, message format, then the remaining strings
            snprintf(texture_path, allocation_size, "%s%s", base_path, emissive_texture->image->uri);
            TRACE("EMISSIVE Texture Path:  %s", texture_path);

            asset_converter_texture(asset_system, texture_path, &cur_mat->emissive_texture);
        }

        Asset_Type material_mesh_type;
        if (data->skins_count > 0)
        {
            material_mesh_type = ASSET_SKINNED_MESH;
        }
        else
        {
            material_mesh_type = ASSET_STATIC_MESH;
        }

        Material_Instance* mat_inst = &material_instances[mesh_idx];
        asset_converter_material_asset(asset_system, "Material_Default",
                                 data->meshes[mesh_idx].primitives->material->name, asset_system->material_system->reflection_registry,
                                 material_mesh_type, &mat_inst->uuid_material_asset);
        material_instances[mesh_idx].material_data = cur_mat;
        material_instances[mesh_idx].data_size = sizeof(Material_Default);
    }



    if (data->skins_count > 0)
    {
        //TODO: this is an unhandled case, and honestly why would a mesh have more than 1 skin???
        MASSERT(data->skins_count <= 1);

        for (u32 skmesh_idx = 0; skmesh_idx < data->meshes_count; skmesh_idx++)
        {
            Madness_Skinned_SubMesh* skinned_submesh = &skinned_mesh[skmesh_idx];
            Madness_SkMesh_GPU_Data* skinned_submesh_gpu = &skinned_gpu_data[skmesh_idx];

            const cgltf_accessor* joint_accessor = cgltf_find_accessor(data->meshes[skmesh_idx].primitives,
                                                                       cgltf_attribute_type_joints,
                                                                       0);
            if (joint_accessor)
            {
                //get size information
                cgltf_size num_floats = cgltf_accessor_unpack_floats(joint_accessor, NULL, 0);
                cgltf_size float_bytes = num_floats * sizeof(float);
                skinned_submesh->joint_bytes = float_bytes;

                //alloc and copy data
                float* joint_data = allocator_alloc(frame_allocator, float_bytes);
                skinned_submesh_gpu->joints = allocator_alloc(frame_allocator, float_bytes);
                cgltf_accessor_unpack_floats(joint_accessor, joint_data, num_floats);
                memcpy(skinned_submesh_gpu->joints, joint_data, float_bytes);
            }

            const cgltf_accessor* weight_accessor = cgltf_find_accessor(data->meshes[skmesh_idx].primitives,
                                                                        cgltf_attribute_type_weights,
                                                                        0);
            if (weight_accessor)
            {
                //get size information

                cgltf_size num_floats = cgltf_accessor_unpack_floats(weight_accessor, NULL, 0);
                cgltf_size float_bytes = num_floats * sizeof(float);
                skinned_submesh->weight_bytes = float_bytes;

                //alloc and copy data
                float* weight_data = allocator_alloc(frame_allocator, float_bytes);
                skinned_submesh_gpu->weights = allocator_alloc(frame_allocator, float_bytes);
                cgltf_accessor_unpack_floats(weight_accessor, weight_data, num_floats);
                memcpy(skinned_submesh_gpu->weights, weight_data, float_bytes);
            }
        }


        hash_table* joint_name_to_index = HASH_TABLE_CREATE(size_t, 200);

        animation_data->animations = allocator_alloc(frame_allocator, sizeof(Animation) * data->animations_count);
        animation_data->animations_count = data->animations_count;


        for (size_t skin_idx = 0; skin_idx < data->skins_count; skin_idx++)
        {
            cgltf_skin* skin_data = &data->skins[skin_idx];
            animation_data->joint_count = skin_data->joints_count;
            animation_data->joints = allocator_alloc(frame_allocator, skin_data->joints_count * sizeof(Joint));
            animation_data->resting_pose_local_matrix = allocator_alloc(
                frame_allocator, skin_data->joints_count * sizeof(mat4s));

            // Inverse bind matrices — one 4x4 float matrix per joint
            if (skin_data->inverse_bind_matrices)
            {
                //get size information
                cgltf_size ibm_floats = cgltf_accessor_unpack_floats(skin_data->inverse_bind_matrices, NULL, 0);
                cgltf_size ibm_bytes = ibm_floats * sizeof(float);

                //alloc and copy data
                float* ibm_date = allocator_alloc(frame_allocator, ibm_bytes);
                animation_data->inverse_bind_matrix = allocator_alloc(frame_allocator, ibm_bytes);

                cgltf_accessor_unpack_floats(skin_data->inverse_bind_matrices, ibm_date, ibm_floats);
                memcpy(animation_data->inverse_bind_matrix, ibm_date, ibm_bytes);
            }
            else
            {
                // glTF spec: absent inverse_bind_matrices means identity per joint
                animation_data->inverse_bind_matrix = allocator_alloc(frame_allocator,
                                                                      sizeof(mat4s) * skin_data->joints_count);

                for (cgltf_size j = 0; j < skin_data->joints_count; j++)
                {
                    animation_data->inverse_bind_matrix[j] = glms_mat4_identity();
                }
            }

            // cur_joint->inverse_bind_matrix = data->skins[skin_idx].inverse_bind_matrices[joint_idx].;
            for (size_t joint_idx = 0; joint_idx < skin_data->joints_count; joint_idx++)
            {
                Joint* cur_joint = &animation_data->joints[joint_idx];
                cgltf_node* cgltf_joint = skin_data->joints[joint_idx];

                cur_joint->joint_name = c_string_duplicate_allocator(cgltf_joint->name, frame_allocator);
                cur_joint->id = joint_idx;

                hash_table_insert(joint_name_to_index, cur_joint->joint_name, &joint_idx);
            }
            //this pass is to get the parent id's and parent nodes
            for (size_t joint_idx = 0; joint_idx < data->skins[skin_idx].joints_count; joint_idx++)
            {
                Joint* cur_joint = &animation_data->joints[joint_idx];
                cgltf_node* cgltf_joint = data->skins[skin_idx].joints[joint_idx];

                mat4s* local_mat = &animation_data->resting_pose_local_matrix[joint_idx];
                cgltf_node_transform_local(cgltf_joint, local_mat->raw); // 16 floats, column-major

                size_t parent_idx = 0;
                if (hash_table_get(joint_name_to_index, cgltf_joint->parent->name, &parent_idx))
                {
                    cur_joint->parent_idx = parent_idx;
                }
            }

            //sampler inputs - keyframes
            //sampler output - keyframe values
            // sampler interpolation

            for (size_t animation_idx = 0; animation_idx < data->animations_count; animation_idx++)
            {
                Animation* cur_animation = &animation_data->animations[animation_idx];
                cgltf_animation* anim_data = &data->animations[animation_idx];

                if (anim_data->name)
                {
                    cur_animation->animation_name =
                        STRING_CREATE_FROM_BUFFER_ALLOCATOR(anim_data->name, frame_allocator);
                }
                else
                {
                    cur_animation->animation_name = STRING_CREATE_FROM_BUFFER_ALLOCATOR(
                        data->meshes[0].name, frame_allocator);
                    cur_animation->animation_name = string_concat(cur_animation->animation_name,
                                                                  string_from_int(
                                                                      animation_idx, frame_allocator),
                                                                  frame_allocator);
                }
                cgltf_size channel_count = anim_data->channels_count;
                cgltf_size sampler_count = anim_data->samplers_count;
                cur_animation->channel_count = channel_count;
                cur_animation->sampler_count = sampler_count;

                cur_animation->channels = allocator_alloc(frame_allocator, sizeof(Animation_Channel) * channel_count);
                cur_animation->samplers = allocator_alloc(frame_allocator, sizeof(Animation_Sampler) * sampler_count);


                for (size_t channel_idx = 0; channel_idx < channel_count; channel_idx++)
                {
                    Animation_Channel* cur_channel = &cur_animation->channels[channel_idx];
                    cgltf_animation_channel* anim_channel = &anim_data->channels[channel_idx];

                    cur_channel->animation_path_type = Animation_Path_Type_gltf_to_engine[anim_channel->target_path];
                    cur_channel->sampler_idx = cgltf_animation_sampler_index(anim_data, anim_channel->sampler);
                    // TODO: TEST

                    //get a reference to find where the target joint is in the array
                    size_t joint_idx;
                    if (hash_table_get(joint_name_to_index, anim_channel->target_node->name, &joint_idx))
                    {
                        cur_channel->joint_index = joint_idx;
                    }
                }


                for (size_t sampler_idx = 0; sampler_idx < sampler_count; sampler_idx++)
                {
                    Animation_Sampler* cur_sampler = &cur_animation->samplers[sampler_idx];
                    cur_sampler->sampler_start = FLT_MAX;
                    cur_sampler->sampler_end = -FLT_MAX;
                    cgltf_animation_sampler* anim_sampler = &anim_data->samplers[sampler_idx];


                    //read gltf input data
                    cur_sampler->timestamps_count = anim_sampler->input->count;
                    cur_sampler->timestamps =
                        allocator_alloc(frame_allocator, sizeof(float) * cur_sampler->timestamps_count);
                    //get size information
                    cgltf_size num_floats = cgltf_accessor_unpack_floats(anim_sampler->input, NULL, 0);
                    cgltf_size float_bytes = num_floats * sizeof(float);
                    //alloc and copy data
                    float* timestamp_data = allocator_alloc(frame_allocator, float_bytes);
                    cgltf_accessor_unpack_floats(anim_sampler->input, timestamp_data, num_floats);
                    memcpy(cur_sampler->timestamps, timestamp_data, float_bytes);


                    for (u32 timestamp_idx = 0; timestamp_idx < cur_sampler->timestamps_count; timestamp_idx++)
                    {
                        cur_sampler->sampler_start = min_f(cur_sampler->sampler_start,
                                                           cur_sampler->timestamps[timestamp_idx]);
                        cur_sampler->sampler_end = max_f(cur_sampler->sampler_end,
                                                         cur_sampler->timestamps[timestamp_idx]);
                    }

                    cur_animation->anim_start = min_f(cur_animation->anim_start, cur_sampler->sampler_start);
                    cur_animation->anim_end = max_f(cur_animation->anim_end, cur_sampler->sampler_end);

                    //read gltf output data
                    cur_sampler->trs_interpolation_count = anim_sampler->output->count;
                    // const uint8_t* trs_buffer_data = cgltf_buffer_view_data(anim_sampler->output->buffer_view);
                    cgltf_size output_num_floats = cgltf_accessor_unpack_floats(anim_sampler->output, NULL, 0);
                    cgltf_size output_float_bytes = output_num_floats * sizeof(float);
                    float* trs_buffer_data = allocator_alloc(frame_allocator, output_float_bytes);
                    cgltf_accessor_unpack_floats(anim_sampler->output, trs_buffer_data, output_num_floats);

                    cur_sampler->interpolation_type = Animation_Interpolation_Type_gltf_to_engine[anim_sampler->
                        interpolation];
                    switch (anim_sampler->output->type)
                    {
                    //these are the only supported formats in the gltf spec for samplers
                    case cgltf_type_scalar: // weights
                        cur_sampler->interperlation_data.trs_float = allocator_alloc(frame_allocator,
                            sizeof(float) * output_num_floats);
                        memcpy(cur_sampler->interperlation_data.trs_float, trs_buffer_data, output_float_bytes);
                        break;
                    case cgltf_type_vec3: // translation, scale
                        cur_sampler->interperlation_data.trs_vec3 = allocator_alloc(
                            frame_allocator, sizeof(float) * output_num_floats);
                        memcpy(cur_sampler->interperlation_data.trs_vec3, trs_buffer_data, output_float_bytes);
                        break;
                    case cgltf_type_vec4: // rotation
                        cur_sampler->interperlation_data.trs_vec4 = allocator_alloc(
                            frame_allocator, sizeof(float) * output_num_floats);
                        memcpy(cur_sampler->interperlation_data.trs_vec4, trs_buffer_data, output_float_bytes);
                        break;
                    default:
                        FATAL("UNSUPPORTED CGLTF SAMPLER TYPE");
                        break;
                    }
                }
            }


            hash_table_destroy(joint_name_to_index);
        }
    }


    //build our engine format
    Madness_Mesh_Runtime engine_format = {0};
    engine_format.version = 1;
    engine_format.mesh_count = madness_mesh->mesh_count;
    engine_format.submeshes = madness_mesh->mesh_data;
    engine_format.mesh_gpu_upload = gpu_data;
    engine_format.material_instance = material_instances;
    //TODO: material

    //write it out to the file

    if (data->skins_count > 0)
    {
        String_Builder* file_path_strip = string_builder_create(256, asset_system->frame_allocator);
        string_builder_append_c_string(file_path_strip, gltf_path);
        string_builder_strip_extension(file_path_strip);
        string_builder_strip_path(file_path_strip);

        String_Builder* str_builder = string_builder_create(256, asset_system->frame_allocator);
        string_builder_append_c_string(str_builder, ENGINE_SK_MESH_PATH);
        string_builder_append_builder(str_builder, file_path_strip);
        string_builder_append_c_string(str_builder, ENGINE_SK_MESH_PATH);

        const char* output_path = string_builder_to_c_string(str_builder);

        //write out the engine format data
        FILE* fptr = fopen(output_path, "wb");

        if (!fptr)
        {
            MASSERT(false);
        }

        fwrite(&engine_format.version, sizeof(engine_format.version), 1, fptr);
        fwrite(&engine_format.mesh_count, sizeof(engine_format.mesh_count), 1, fptr);
        fwrite(engine_format.submeshes, sizeof(Madness_SubMesh) * engine_format.mesh_count, 1, fptr);
        //submesh contains the material uuid

        //mesh data
        for (u32 i = 0; i < engine_format.mesh_count; ++i)
        {
            Madness_SubMesh* sub_mesh = &engine_format.submeshes[i];
            fwrite(engine_format.mesh_gpu_upload[i].tangent, sub_mesh->tangent_bytes, 1, fptr);
            fwrite(engine_format.mesh_gpu_upload[i].vertex_color, sub_mesh->vertex_color_bytes, 1, fptr);
            fwrite(engine_format.mesh_gpu_upload[i].vertex, sub_mesh->vertex_bytes, 1, fptr);
            fwrite(engine_format.mesh_gpu_upload[i].normal, sub_mesh->normal_bytes, 1, fptr);
            fwrite(engine_format.mesh_gpu_upload[i].uv, sub_mesh->uv_bytes, 1, fptr);
            fwrite(engine_format.mesh_gpu_upload[i].indices, sub_mesh->indices_bytes, 1, fptr);
        }

        //skeletal data
        for (u32 i = 0; i < engine_format.mesh_count; ++i)
        {
        }
        //animation data


        // write out metadata
        Asset_MetaData meta_data = {0};
        meta_data.source_file = STRING_CREATE_FROM_BUFFER_HEAP_ALLOCATOR(gltf_path, asset_system->heap_allocator);
        meta_data.binary_file = string_builder_to_string(str_builder);
        meta_data.type = ASSET_SKINNED_MESH;
        meta_data.uuid = madness_uuid_generate_return();
        meta_data.hash = madness_uuid_hash(&meta_data.uuid);
        asset_registry_add_asset(asset_system->asset_registry, &meta_data);

        fclose(fptr);
    }
    else
    {
        String_Builder* file_path_strip = string_builder_create(256, asset_system->frame_allocator);
        string_builder_append_c_string(file_path_strip, gltf_path);
        string_builder_strip_extension(file_path_strip);
        string_builder_strip_path(file_path_strip);

        String_Builder* str_builder = string_builder_create(256, asset_system->frame_allocator);
        string_builder_append_c_string(str_builder, ENGINE_MESH_PATH);
        string_builder_append_builder(str_builder, file_path_strip);
        string_builder_append_c_string(str_builder, ENGINE_MESH_EXTENSION);

        const char* output_path = string_builder_to_c_string(str_builder);

        //write out the engine format data
        //write out the engine format data
        FILE* fptr = fopen(output_path, "wb");

        if (!fptr)
        {
            MASSERT(false);
        }

        asset_mesh_serialize(&engine_format, fptr);



        // write out metadata
        Asset_MetaData meta_data = {0};
        meta_data.source_file = STRING_CREATE_FROM_BUFFER_HEAP_ALLOCATOR(gltf_path, asset_system->heap_allocator);
        meta_data.binary_file = string_builder_to_string(str_builder);
        meta_data.uuid = madness_uuid_generate_return();
        meta_data.hash = madness_uuid_hash(&meta_data.uuid);
        meta_data.type = ASSET_STATIC_MESH;
        asset_registry_add_asset(asset_system->asset_registry, &meta_data);

        fclose(fptr);
    }


    cgltf_free(data);
    return true;
}

bool asset_converter_material_asset(Asset_System* asset_system, const char* material_name, const char* asset_name,
                              Reflection_Registry* reflection_registry_material,
                              Asset_Type asset_type, MADNESS_UUID* out_uuid)
{
    //We are always going to assume that our reflection registry is up to date and that it is the source of truth

    Material_Asset asset = {0};
    if (asset_type == ASSET_STATIC_MESH)
    {
        asset.material_info.shader_name = &(STRING_STRLEN(MESH_DEFAULT_SHADER));
        asset.material_info.mesh_type = Shader_Mesh_Type_Mesh;
    }
    else if (ASSET_SKINNED_MESH)
    {
        asset.material_info.shader_name = &(STRING_STRLEN(SKINNED_MESH_DEFAULT_SHADER));
        asset.material_info.mesh_type = Shader_Mesh_Type_Skinned;
    }
    else { MASSERT(false); }
    asset.material_info.material_name = &(STRING_STRLEN(MATERIAL_DEFAULT_NAME));
    asset.material_info.shader_stage = Shader_Stage_Type_Graphics;
    asset.material_info.shader_pass = Shader_Pass_Type_Opaque;
    asset.material_info.blend_mode = Shader_Blend_Mode_Default;

    Reflection_Runtime_Struct reflection_material = reflection_registry_get_struct(
        reflection_registry_material, MATERIAL_DEFAULT_NAME);
    asset.reflection_material_data = &reflection_material;


    // do a conversion of any texture types to u32 for bindless
    asset.material_gpu_definition = allocator_alloc(asset_system->frame_allocator, sizeof(Reflection_Runtime_Struct));
    asset.material_gpu_definition->field_count = reflection_material.field_count;
    asset.material_gpu_definition->name_hashes = allocator_alloc(asset_system->frame_allocator,
                                                                 sizeof(u64) *
                                                                 reflection_material.field_count);
    asset.material_gpu_definition->field_offsets = allocator_alloc(asset_system->frame_allocator,
                                                                   sizeof(u32) *
                                                                   reflection_material.field_count);
    asset.material_gpu_definition->types = allocator_alloc(asset_system->frame_allocator,
                                                           sizeof(Reflection_Type) *
                                                           reflection_material.field_count);

    asset.material_gpu_definition->struct_size = 0;

    u32 offset = 0;
    for (u32 i = 0; i < reflection_material.field_count; i++)
    {
        asset.material_gpu_definition->name_hashes[i] =
            c_string_hash_u64(reflection_material.fields[i].name);

        if (reflection_material.fields[i].type == REFLECTION_TYPE_UUID)
        {
            asset.material_gpu_definition->types[i] = REFLECTION_TYPE_U32;
        }
        else
        {
            asset.material_gpu_definition->types[i] = reflection_material.fields[i].type;
        }
        asset.material_gpu_definition->field_offsets[i] = offset;
        asset.material_gpu_definition->struct_size += reflection_type_get_size(
            asset.material_gpu_definition->types[i]);
        offset += reflection_type_get_size(asset.material_gpu_definition->types[i]);
    }


    //write out the file

    String_Builder* str_builder = string_builder_create(256, asset_system->frame_allocator);
    string_builder_append_c_string(str_builder, ENGINE_MATERIAL_PATH);
    string_builder_append_c_string(str_builder, "/");
    string_builder_append_c_string(str_builder, material_name);
    string_builder_append_c_string(str_builder, "/");
    string_builder_append_c_string(str_builder, asset_name);
    string_builder_append_c_string(str_builder, ENGINE_MATERIAL_EXTENSION);

    const char* output_path = string_builder_to_c_string(str_builder);
    FILE* fptr = fopen(output_path, "wb");
    if (!fptr)
    {
        MASSERT(false);
    }

    Material_Asset_Runtime asset_editor = {0};
    asset_editor.version = 1;
    asset_editor.asset = &asset;
    asset_material_serialize(&asset_editor, fptr);



    Asset_MetaData meta_data = {0};
    meta_data.source_file = STRING_CREATE_FROM_BUFFER_HEAP_ALLOCATOR(asset_name, asset_system->heap_allocator);
    meta_data.binary_file = string_builder_to_string(str_builder);
    meta_data.uuid = madness_uuid_generate_return();
    meta_data.hash = madness_uuid_hash(&meta_data.uuid);
    meta_data.type = ASSET_MATERIAL;
    asset_registry_add_asset(asset_system->asset_registry, &meta_data);

    *out_uuid = meta_data.uuid;
}
