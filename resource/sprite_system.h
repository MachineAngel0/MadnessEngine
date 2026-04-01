#ifndef SPRITE_H
#define SPRITE_H


#include "../renderer/vulkan_types.h"


typedef struct Sprite_System_Config
{
    u64 sprite_transient_count;
    u64 sprite_count;
} Sprite_System_Config;

// Sprite System
Sprite_System* sprite_system_init(Memory_System* memory_system);
bool sprite_system_shutdown(Sprite_System* sprite_system);

MAPI void sprite_system_begin(Sprite_System* sprite_system, i32 screen_size_x, i32 screen_size_y);

MAPI Sprite_Handle sprite_system_get_new_sprite(Sprite_System* sprite_system, Sprite_Data* out_sprite_data);

MAPI Sprite_Handle sprite_system_get_sprite(Sprite_System* sprite_system,  Sprite_Handle sprite_handle);
MAPI Sprite_Handle sprite_system_sprite_release(Sprite_System* sprite_system, Sprite_Handle sprite_handle);


MAPI Sprite_Data* sprite_system_get_new_sprite_transient(Sprite_System* sprite_system);

MAPI Sprite_Data* sprite_system_get_ui_sprite(Sprite_System* sprite_system);
MAPI Sprite_Data* sprite_system_get_text_sprite(Sprite_System* sprite_system);


// these handles cant be from the transient or ui/text sprites
MAPI Sprite_Data* sprite_system_get_new_sprite_handle(Sprite_System* sprite_system, Sprite_Handle* sprite_handle);

bool sprite_system_generate_render_packet(Sprite_System* sprite_system, Render_Packet_UI* out_ui_packet,
    Render_Packet_Sprite* out_sprite_packet);

#endif //SPRITE_H
