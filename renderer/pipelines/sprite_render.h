#ifndef SPRITE_RENDER_H
#define SPRITE_RENDER_H
#include "vulkan_struct_types.h"


//Sprite Renderer
Sprite_Renderer* sprite_render_init(Renderer* renderer);
MAPI void sprite_upload_draw_data(Renderer* renderer,
                                  Sprite_Renderer* sprite_backend, Render_Packet_Sprite* sprite_render_packet, Vulkan_Command_Buffer* command_buffer);
MAPI void sprite_renderer_draw(Renderer* renderer, Sprite_Renderer* sprite_backend,
                      Vulkan_Command_Buffer* command_buffer);





#endif //SPRITE_RENDER_H
