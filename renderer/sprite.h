#ifndef SPRITE_H
#define SPRITE_H
#include "vulkan_types.h"

#define MAX_SPRITE_COUNT 100


Sprite_System* sprite_system_init(Renderer* renderer);

void sprite_begin(Sprite_System* sprite_system, i32 screen_size_x, i32 screen_size_y);

void sprite_upload_draw_data(Renderer* renderer, Sprite_System* sprite_system);
//TODO:
// void sprite_upload_draw_data_packet(Renderer* renderer, Sprite_System* sprite_system, Renderer_Packet_List* render_packet);

void sprite_draw(Sprite_System* sprite_system, Renderer* renderer, vulkan_command_buffer* command_buffer);


// this is specifally for the sprite system
Sprite_Handle sprite_system_create_sprite(Sprite_System* sprite_system, vec2 pos, vec2 size, vec3 color, Texture_Handle texture,
                   Sprite_Pipeline_Flags material_flags);


// this is for any other system that is managing the sprites on their own
Sprite_Data* sprite_create(Frame_Arena* frame_arena, vec2 pos, vec2 size, vec3 color, Texture_Handle texture,
                               Sprite_Pipeline_Flags material_flags);

//doesn't set any params
Sprite_Data* sprite_create_minimal(Frame_Arena* frame_arena);

Sprite_Data* sprite_create_textured(Arena* frame_arena, vec2 pos, vec2 size, vec3 color, vec2 uv0, vec2 uv1);


#endif //SPRITE_H
