#include "texture_system.h"
#include "asset_converter.h"
#include "asset_system.h"
#include "shader_system.h"


bool texture_system_init(Asset_System* asset_system, Texture_System* texture_system, Memory_System* memory_system)
{
    //textures
    texture_system->in_use_textures_count = 0;
    texture_system->max_textures = MAX_TEXTURE_COUNT;

    memset(texture_system->textures, 0, MAX_TEXTURE_COUNT * sizeof(Madness_Texture));
    texture_system->available_texture_queue = ring_queue_create(sizeof(u32), MAX_TEXTURE_COUNT);

    texture_system->texture_gpu_upload_queue = ring_queue_create(sizeof(Texture_GPU_Upload), MAX_TEXTURE_COUNT);

    for (u32 i = 0; i < MAX_TEXTURE_COUNT; i++)
    {
        ring_enqueue(texture_system->available_texture_queue, &i);
    }

    //fonts
    memset(texture_system->font_textures, 0, MAX_FONT_COUNT * sizeof(Madness_Font));
    texture_system->available_font_queue = ring_queue_create(sizeof(u32), MAX_FONT_COUNT);

    for (u32 i = 0; i < MAX_FONT_COUNT; i++)
    {
        ring_enqueue(texture_system->available_font_queue, &i);
    }


    //hash map
    texture_system->texture_hash_map = HASH_MAP_CREATE(u64, u32, MAX_TEXTURE_COUNT*2);


    //create our debug texture
    /*
    asset_converter_texture(asset_system, "../z_assets/textures/error_texture.png", NULL);
    asset_converter_texture(asset_system, "../z_assets/textures/test_particle.png", NULL);
    asset_converter_msdf_font(asset_system, "../z_assets/msdf_fonts/arial_msdf.png");
    */


    texture_system->default_texture_handle = asset_load_texture_path(
        asset_system, "error_texture");

    return texture_system;
}

bool texture_system_shutdown(Texture_System* texture_system, Memory_System* memory_system)
{
    //TODO: unload all the textures and dsa's;
    MASSERT(texture_system);
    memory_system_memory_free(memory_system, texture_system, MEMORY_SUBSYSTEM_TEXTURE);

    return true;
}

bool texture_system_get_texture(Texture_System* texture_system, Texture_Handle handle, Madness_Texture* out_texture)
{
    if (texture_system->textures->generation != handle.generation)
    {
        WARN("texture_system_get_texture: trying to get a font. use get_font");
        // out_texture = &texture_system->textures[0];
        out_texture = NULL;
        return false;
    }

    if (texture_system->textures[handle.handle].type != ASSET_TEXTURE)
    {
        WARN("texture_system_get_texture: trying to get a font. use get_font");
        MASSERT_FALSE();
        return false;
    }
    *out_texture = texture_system->textures[handle.handle];
    return true;
}



Texture_Handle texture_system_update_texture(Texture_System* texture_system, Texture_Handle handle,
                                             const char* filepath)
{
    UNIMPLEMENTED();
    return (Texture_Handle){0};
}


Texture_Handle texture_system_get_default_texture(Texture_System* texture_system)
{
    return texture_system->default_texture_handle;
}


bool texture_system_get_font(Texture_System* texture_system, const Texture_Handle handle, Madness_Font* out_font)
{
    Madness_Texture* texture = &texture_system->textures[handle.handle];
    if (texture_system->textures[handle.handle].type != ASSET_FONT)
    {
        WARN("texture_system_get_font: trying to get a normal texture. use get_font");
        MASSERT_FALSE();
        return false;
    }

    *out_font = texture_system->font_textures[texture->font_index];
    return true;
}

bool texture_system_is_loaded(Texture_System* texture_system, Texture_Handle handle)
{
    return (texture_system->textures[handle.handle].texture_load_state == ASSET_LOAD_STATE_LOADED);
}


bool texture_system_exists(Asset_System* asset_system, Texture_Handle* out_handle, u64 hash)
{
    Texture_System* texture_system = asset_system->texture_system;
    u32 texture_idx = 0;
    if (hash_map_get(texture_system->texture_hash_map, &hash, &texture_idx))
    {
        *out_handle = (Texture_Handle){texture_idx, texture_system->textures[texture_idx].generation};
        texture_system->texture_asset[texture_idx].reference_count++;
        return true;
    }

    return false;
}


bool texture_system_upload_new_texture(Asset_System* asset_system, u64 hash,
                                       Madness_Texture texture_data, u8* pixel_data, Texture_Handle* out_handle,
                                       String* engine_path)
{
    Texture_System* texture_system = asset_system->texture_system;

    //find a free texture slot
    u32 free_index;
    if (!ring_dequeue(texture_system->available_texture_queue, &free_index))
    {
        // TODO: figure out what to do when we run out of textures, im think just allocate more space for them
        MASSERT("OUT OF TEXTURE IDX's");
        *out_handle = texture_system->default_texture_handle;
        return false;
    }

    texture_system->in_use_textures_count++;
    //set texture data
    Madness_Texture* texture = &texture_system->textures[free_index];
    MASSERT(texture->texture_load_state == ASSET_LOAD_STATE_UNLOADED);

    texture->width = texture_data.width;
    texture->height = texture_data.height;
    texture->channels = texture_data.channels;
    texture->format = texture_data.format;
    texture->pixels_size = texture_data.pixels_size;
    texture->type = texture_data.type;
    texture->texture_load_state = ASSET_LOAD_STATE_QUEUED;
    //update generation
    texture->generation++;

    //fill out the handle
    out_handle->handle = free_index;
    out_handle->generation = texture->generation;

    //queue for upload on the gpu
    Texture_GPU_Upload upload_texture = {0};
    upload_texture.madness_texture = texture;
    upload_texture.pixel_data = pixel_data;
    upload_texture.texture_memory_allocator = asset_system->texture_allocator;
    upload_texture.texture_index = free_index;
    ring_enqueue(texture_system->texture_gpu_upload_queue, &upload_texture);


    hash_map_insert(texture_system->texture_hash_map, &hash, &free_index);


    //update asset data
    Madness_Asset* meta_data = &texture_system->texture_asset[free_index];
    meta_data->engine_path = string_duplicate_heap(engine_path, asset_system->heap_allocator);
    meta_data->path_hash = hash;
    meta_data->type = ASSET_TEXTURE;
    meta_data->reference_count = 1;


    return out_handle;
}


bool texture_system_upload_new_font(Asset_System* asset_system, MADNESS_UUID uuid, u64 hash,
                                    Madness_Texture texture_data, Madness_Font texture_font_data, u8* pixel_data,
                                    Texture_Handle* out_handle, String* engine_path)
{
    Texture_System* texture_system = asset_system->texture_system;

    //find a free texture slot
    u32 free_index;
    if (!ring_dequeue(texture_system->available_texture_queue, &free_index))
    {
        // TODO: figure out what to do when we run out of textures, im think just allocate more space for them
        MASSERT("OUT OF TEXTURE IDX's");
        *out_handle = texture_system->default_texture_handle;
        return false;
    }

    u32 free_font_index;
    if (!ring_dequeue(texture_system->available_font_queue, &free_font_index))
    {
        // TODO: figure out what to do when we run out of textures, im think just allocate more space for them
        MASSERT("OUT OF TEXTURE IDX's");
        *out_handle = texture_system->default_texture_handle;
        return false;
    }

    texture_system->in_use_textures_count++;


    Madness_Font* font_texture = &texture_system->font_textures[free_font_index];
    *font_texture = texture_font_data;


    Madness_Texture* texture = &texture_system->textures[free_index];
    texture->width = texture_data.width;
    texture->height = texture_data.height;
    texture->channels = texture_data.channels;
    texture->format = texture_data.format;
    texture->pixels_size = texture_data.pixels_size;
    texture->type = texture_data.type;
    texture->font_index = free_font_index;

    //update bindless and generation
    texture->generation++;

    //fill out the handle
    out_handle->handle = free_index;
    out_handle->generation = texture->generation;

    Texture_GPU_Upload upload_texture = {0};
    upload_texture.madness_texture = texture;
    upload_texture.pixel_data = pixel_data;
    upload_texture.texture_memory_allocator = asset_system->texture_allocator;
    upload_texture.texture_index = free_index;
    // TODO: replace with texture memory allocator


    //send to renderer for upload
    ring_enqueue(texture_system->texture_gpu_upload_queue, &upload_texture);

    hash_map_insert(texture_system->texture_hash_map, &hash, &free_index);


    //update asset data
    Madness_Asset* meta_data = &texture_system->texture_asset[free_index];
    meta_data->engine_path = string_duplicate_heap(engine_path, asset_system->heap_allocator);
    meta_data->path_hash = hash;
    meta_data->type = ASSET_FONT;
    meta_data->reference_count = 1;


    return true;
}

bool texture_system_unload_texture(Asset_System* asset_system, Texture_Handle texture_handle)
{
    Texture_System* texture_system = asset_system->texture_system;


    texture_system->texture_asset[texture_handle.handle].reference_count--;
    if (texture_system->texture_asset[texture_handle.handle].reference_count <= 0)
    {
        //unload the texture
        //TODO: actually not even neccessary, we only need to delete the texture vulkan side if there is one there,
        // which honestly should just get handled by default on upload
        // vulkan_texture_system_free(asset_system->renderer, texture_handle);
    }


    return true;
}
