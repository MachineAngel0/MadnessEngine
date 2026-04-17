#ifndef SPRITE_RENDER_H
#define SPRITE_RENDER_H
#include "../vulkan_types.h"


//Sprite Renderer
Sprite_Renderer* sprite_render_init(Renderer* renderer, Resource_System* resource_system);
MAPI void sprite_upload_draw_data(Renderer* renderer,
                                  Sprite_Renderer* sprite_backend, Render_Packet_Sprite* sprite_render_packet, vulkan_command_buffer* command_buffer);
MAPI void sprite_renderer_draw(Renderer* renderer, Sprite_Renderer* sprite_backend,
                      vulkan_command_buffer* command_buffer);





#endif //SPRITE_RENDER_H
