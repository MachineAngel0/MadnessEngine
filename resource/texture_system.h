#ifndef TEXTURE_SYSTEM_H
#define TEXTURE_SYSTEM_H
#include "asset_converter.h"
#include "asset_system.h"
#include "resource_types.h"


// #define DEFAULT_TEXTURE_NAME "default"
// #define ERROR_TEXTURE_NAME "error"

//NOTE: since the image goes from cpu to gpu, the renderer takes care of freeing the pixel data, if loaded from a file

MAPI bool texture_system_init(Asset_System* asset_system, Texture_System* texture_system, Memory_System* memory_system);
MAPI bool texture_system_shutdown(Texture_System* texture_system, Memory_System* memory_system);


//loads in a new texture in place of the old one
MAPI Texture_Handle texture_system_update_texture(Texture_System* texture_system, Texture_Handle handle,
                                                  const char* filepath);

MAPI Texture_Handle texture_system_get_default_texture(Texture_System* texture_system);


MAPI bool texture_system_texture_free(Asset_System* asset_system, Texture_Handle handle);


MAPI bool texture_system_acquire(Asset_System* asset_system, Asset_Record* record,
                                 Madness_Texture texture_data, u8* pixel_data, Texture_Handle* out_handle);

MAPI bool texture_system_release(Asset_System* asset_system, Texture_Handle handle);

MAPI bool texture_system_acquire_font(Asset_System* asset_system,
                                      Asset_Record* record, Madness_Texture texture_data,
                                      Madness_Font texture_font_data, u8* pixel_data, Texture_Handle* out_handle);

bool texture_system_does_texture_exist(Texture_System* texture_system, Asset_Record* asset, Texture_Handle* out_handle);


//only reloads textures if they are in use, otherwise we just write it out to a file
bool texture_reload(Asset_System* asset_system, Texture_System* texture_system,
                    Asset_Record* asset)
{
    //check the runtime assets
    for (u32 i = 0; i < texture_system->max_textures; i++)
    {
        Asset_Runtime* runtime = &texture_system->texture_asset[i];

        if (runtime->hash == asset->hash)
        {
            runtime->reference_count++;
            return true;
        }
    }

    //reconvert the asset again
    asset_converter_texture(asset_system,
                            string_to_c_string_allocator(asset->source_file, asset_system->frame_allocator),
                            NULL);

    //load it in again
    // asset_load_texture_uuid();

    //send it off to the gpu


    return true;

}


//TODO:
/*
MAPI bool texture_system_release(Asset_System* asset_system, Asset_Record* record,
                                       Madness_Texture texture_data, u8* pixel_data, Texture_Handle* out_handle);

MAPI bool texture_system_release_font(Asset_System* asset_system, MADNESS_UUID uuid, u64 hash,
                                         Madness_Texture texture_data, Madness_Font texture_font_data, u8* pixel_data, Texture_Handle* out_handle, String*
                                         engine_path);
*/


MADNESS_UUID texture_system_get_default_uuid(Asset_System* asset_system);

MAPI bool texture_system_get_texture(Texture_System* texture_system, Texture_Handle handle,
                                     Madness_Texture* out_texture);

MAPI bool texture_system_get_font(Texture_System* texture_system, Texture_Handle handle, Madness_Font* out_font);


#endif //TEXTURE_SYSTEM_H
