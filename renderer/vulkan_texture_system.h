#ifndef VULKAN_TEXTURE_SYSTEM_H
#define VULKAN_TEXTURE_SYSTEM_H


#include "vulkan_struct_types.h"

//TODO: pass in texture config amount
Vulkan_Texture_System* vulkan_texture_system_init(Renderer* renderer);

void vulkan_texture_update_async_resources(Renderer* renderer, Render_Packet* packet,
                                           Vulkan_Command_Buffer* command_buffer);
void vulkan_texture_system_upload_new_textures(Renderer* renderer, Render_Packet* packet);


Vulkan_Texture* vulkan_texture_system_get_vulkan_texture(Vulkan_Texture_System* system, u32 bindless_index);

Texture_Handle vulkan_texture_system_add_texture_file(Renderer* renderer, Vulkan_Texture_System* system,
                                                      char const* filepath);

bool vulkan_texture_system_free(Renderer* renderer, Texture_Handle handle);



void vulkan_texture_system_internal(Renderer* renderer, u32 width, u32 height,
                                     VkFormat format, Vulkan_Texture* out_texture);

void vulkan_texture_create_shadowmap(Renderer* renderer, u32 width, u32 height,
                                     VkFormat format, Vulkan_Texture* vk_texture);
void vulkan_texture_create_shadowmap_point_light(Renderer* renderer, u32 extent,
                                                 VkFormat format, Vulkan_Texture_Cubemap* vk_texture);


#endif //VULKAN_TEXTURE_SYSTEM_H
