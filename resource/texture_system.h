#ifndef TEXTURE_SYSTEM_H
#define TEXTURE_SYSTEM_H
#include "resource_types.h"


//TODO: rn textures dont free their pixel data, which can be a really big problem

Texture_System* texture_system_init(Memory_System* memory_system);
MAPI bool texture_system_shutdown(Texture_System* texture_system, Memory_System* memory_system);


MAPI bool texture_system_load_texture(Texture_System* texture_system, char const* file_path, Texture_Handle* out_handle);
MAPI bool texture_system_unload_texture(Texture_System* texture_system, Texture_Handle handle);

//loads in a new texture in place of the old one
MAPI Texture_Handle texture_system_update_texture(Texture_System* texture_system, Texture_Handle handle,
                                            const char* filepath);

MAPI bool texture_system_get_texture(Texture_System* texture_system, Texture_Handle handle, Texture* out_texture);
MAPI Texture_Handle* texture_system_get_default_texture(Texture_System* texture_system);



MAPI bool texture_system_load_font(Texture_System* texture_system, const char* file_path, Texture_Handle* out_handle, Arena* arena);
MAPI bool texture_system_unload_font(Texture_System* texture_system, Texture_Handle handle);
MAPI bool texture_system_get_font(Texture_System* texture_system, Texture_Handle handle, Madness_Font* out_font);





#endif //TEXTURE_SYSTEM_H