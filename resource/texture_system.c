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


    memset(texture_system->texture_asset, 0, MAX_TEXTURE_COUNT * sizeof(Asset_Runtime));

    // texture_system->hash_map_loaded_texture = HASH_MAP_CREATE(sizeof(UUID_HASH), sizeof(Texture_Handle), MAX_TEXTURE_COUNT*2, asset_system->allocator);



    //create our debug texture
    asset_converter_texture(asset_system, "../z_assets/textures/error_texture.png",
                            &texture_system->default_texture_uuid);
    /*
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

MADNESS_UUID texture_system_get_default_uuid(Asset_System* asset_system)
{
    return asset_system->texture_system->default_texture_uuid;
}


bool texture_system_get_texture(Texture_System* texture_system, Texture_Handle handle, Madness_Texture* out_texture)
{
    Asset_Runtime* asset = &texture_system->texture_asset[handle.handle];

    if (asset->generation != handle.generation)
    {
        WARN("texture_system_get_texture: trying to get a font. use get_font");
        // out_texture = &texture_system->textures[0];
        out_texture = NULL;
        return false;
    }

    if (texture_system->textures[handle.handle].type != TEXTURE_TYPE_TEXTURE)
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

bool texture_system_does_texture_exist(Texture_System* texture_system, Asset_Record* asset, Texture_Handle* out_handle)
{
    MASSERT(texture_system);
    MASSERT(asset);


    PROFILE_ZONE(texture_system_does_texture_exist);

    //check the runtime assets
    for (u32 i = 0; i < texture_system->max_textures; i++)
    {
        Asset_Runtime* runtime = &texture_system->texture_asset[i];

        if (runtime->hash == asset->hash)
        {
            runtime->reference_count++;
            *out_handle = runtime->asset_handles.texture_handle;
            PROFILE_ZONE_END(texture_system_does_texture_exist);
            return true;
        }
    }

    PROFILE_ZONE_END(texture_system_does_texture_exist);
    return false;
}


bool texture_system_get_font(Texture_System* texture_system, const Texture_Handle handle, Madness_Font* out_font)
{
    Madness_Texture* texture = &texture_system->textures[handle.handle];
    if (texture_system->textures[handle.handle].type != TEXTURE_TYPE_FONT)
    {
        WARN("texture_system_get_font: trying to get a normal texture. use get_font");
        MASSERT_FALSE();
        return false;
    }

    *out_font = texture_system->font_textures[texture->font_index];
    return true;
}


bool texture_system_acquire(Asset_System* asset_system, Asset_Record* record,
                            Madness_Texture texture_data, u8* pixel_data, Texture_Handle* out_handle)
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

    //set texture data
    Madness_Texture* texture = &texture_system->textures[free_index];

    texture->width = texture_data.width;
    texture->height = texture_data.height;
    texture->channels = texture_data.channels;
    texture->format = texture_data.format;
    texture->pixels_size = texture_data.pixels_size;
    texture->type = texture_data.type;


    //queue for upload on the gpu
    Texture_GPU_Upload upload_texture = {0};
    upload_texture.madness_texture = texture;
    upload_texture.pixel_data = pixel_data;
    upload_texture.texture_memory_allocator = asset_system->texture_allocator;
    upload_texture.texture_index = free_index;
    ring_enqueue(texture_system->texture_gpu_upload_queue, &upload_texture);


    Asset_Runtime* runtime = &texture_system->texture_asset[free_index];
    asset_runtime_set_info_from_record(runtime, record, ASSET_TEXTURE);
    runtime->asset_handles.texture_handle = (Texture_Handle){free_index, runtime->generation};

    //fill out the handle
    out_handle->handle = free_index;
    out_handle->generation = runtime->generation;

    texture_system->in_use_textures_count++;


    return out_handle;
}

bool texture_system_release(Asset_System* asset_system, Texture_Handle handle)
{
    Texture_System* texture_system = asset_system->texture_system;


    Asset_Runtime* runtime = &texture_system->texture_asset[handle.handle];
    if (runtime->generation != handle.generation)
    {
        MASSERT_MSG_FALSE("texture_system_release: INVALID TEXTURE GENERATION SHOULD NEVER IDEALLY HAPPEN");
        return false;
    }

    //TODO: we could zero this but idk?
    Madness_Texture* texture = &texture_system->textures[handle.handle];

    if (runtime_asset_release(runtime))
    {
        ring_enqueue(texture_system->available_texture_queue, &handle.handle);

        if (texture->type == TEXTURE_TYPE_FONT)
        {
            ring_enqueue(texture_system->available_font_queue, &texture->font_index);
        }
    }

    return true;
}


bool texture_system_acquire_font(Asset_System* asset_system,
                                 Asset_Record* record, Madness_Texture texture_data, Madness_Font texture_font_data,
                                 u8* pixel_data, Texture_Handle* out_handle)
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


    Texture_GPU_Upload upload_texture = {0};
    upload_texture.madness_texture = texture;
    upload_texture.pixel_data = pixel_data;
    upload_texture.texture_memory_allocator = asset_system->texture_allocator;
    upload_texture.texture_index = free_index;


    //send to renderer for upload
    ring_enqueue(texture_system->texture_gpu_upload_queue, &upload_texture);

    Asset_Runtime* runtime = &texture_system->texture_asset[free_index];
    asset_runtime_set_info_from_record(runtime, record, ASSET_FONT);

    runtime->asset_handles.texture_handle = (Texture_Handle){free_index, runtime->generation};

    //fill out the handle
    *out_handle = (Texture_Handle){free_index, runtime->generation};


    return true;
}
