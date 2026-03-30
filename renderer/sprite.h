#ifndef SPRITE_H
#define SPRITE_H
#include "vulkan_types.h"

#define MAX_SPRITE_COUNT 100


MAPI Sprite_System* sprite_system_init(Renderer* renderer);

MAPI void sprite_begin(Sprite_System* sprite_system, i32 screen_size_x, i32 screen_size_y);

MAPI void sprite_upload_draw_data(Renderer* renderer, Sprite_System* sprite_system);
//TODO:
// void sprite_upload_draw_data_packet(Renderer* renderer, Sprite_System* sprite_system, Renderer_Packet_List* render_packet);

MAPI void sprite_draw(Sprite_System* sprite_system, Renderer* renderer, vulkan_command_buffer* command_buffer);



//user sets the params
// MAPI Sprite_Data* sprite_system_get_new_sprite(Sprite_System* sprite_system);
MAPI Sprite_Data* sprite_system_get_new_sprite_transient(Sprite_System* sprite_system);
Sprite_Data* sprite_create_minimal(Frame_Arena* frame_arena); // TODO: delete later

//idk about this interface
// Sprite_Handle* sprite_system_get_sprite_handle(Sprite_System* sprite_system);
// Sprite_Handle* sprite_system_get_sprite_transient_handle(Sprite_System* sprite_system);


#endif //SPRITE_H
