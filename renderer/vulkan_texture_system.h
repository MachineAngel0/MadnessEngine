#ifndef VULKAN_TEXTURE_SYSTEM_H
#define VULKAN_TEXTURE_SYSTEM_H


#include "vulkan_struct_types.h"

//TODO: pass in texture config amount
Vulkan_Texture_System* vulkan_texture_system_init(Renderer* renderer);

void vulkan_texture_system_update(Renderer* renderer, Render_Packet* packet);

Vulkan_Texture* vulkan_texture_system_get_vulkan_texture(Vulkan_Texture_System* system, u32 bindless_index);

Texture_Handle vulkan_texture_system_add_texture_file(Renderer* renderer, Vulkan_Texture_System* system,
                                                      char const* filepath);

bool vulkan_texture_system_free(Renderer* renderer, Texture_Handle handle);

#endif //VULKAN_TEXTURE_SYSTEM_H
