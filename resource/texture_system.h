#ifndef TEXTURE_SYSTEM_H
#define TEXTURE_SYSTEM_H
#include "resource_types.h"


//NOTE: since the image goes from cpu to gpu, the renderer takes care of freeing the pixel data, if loaded from a file

MAPI bool texture_system_init(Asset_System* asset_system, Texture_System* texture_system, Memory_System* memory_system);
MAPI bool texture_system_shutdown(Texture_System* texture_system, Memory_System* memory_system);


//loads in a new texture in place of the old one
MAPI Texture_Handle texture_system_update_texture(Texture_System* texture_system, Texture_Handle handle,
                                                  const char* filepath);

MAPI Texture_Handle texture_system_get_default_texture(Texture_System* texture_system);


MAPI bool texture_system_exists(Asset_System* asset_system, Texture_Handle* out_handle, u64 hash);

MAPI bool texture_system_texture_free(Asset_System* asset_system, Texture_Handle handle);


MAPI bool texture_system_upload_new_texture(Asset_System* asset_system, MADNESS_UUID uuid, u64 hash, Madness_Texture texture_data,
                                            u8* pixel_data, Texture_Handle* out_handle);

MAPI bool texture_system_upload_new_font(Asset_System* asset_system, MADNESS_UUID uuid, u64 hash,
                                         Madness_Texture texture_data, Madness_Font texture_font_data, u8* pixel_data, Texture_Handle* out_handle);


MAPI bool texture_system_get_texture(Texture_System* texture_system, Texture_Handle handle,
                                     Madness_Texture* out_texture);
MAPI bool texture_system_get_font(Texture_System* texture_system, Texture_Handle handle, Madness_Font* out_font);


#endif //TEXTURE_SYSTEM_H
