#ifndef RENDERER_PACKET_H
#define RENDERER_PACKET_H


#include "UI.h"
#include "vulkan_types.h"


typedef struct Sprite_Packet
{
    Sprite_Data* sprite_data;
    u32 sprite_data_size;
} Sprite_Packet;

typedef struct UI_Packet
{
    UI_Text_Draw_Data* text_draw_data;
    UI_Quad_Draw_Data* ui_draw_data;
} UI_Packet;


ARRAY_GENERATE_TYPE(Sprite_Packet)
ARRAY_GENERATE_TYPE(UI_Packet)

//the update function of any system like game or ui, just adds the draw data into the array
typedef struct Render_Packet
{
   Sprite_Data_array* Sprite_Data_packet;
   Sprite_Packet_array* Sprite_packet;
   UI_Packet_array* UI_packets;
} Render_Packet;

void render_packet_clear(Render_Packet* renderer_packets);

void render_packet_add_sprite_data(Render_Packet* renderer_packets, Sprite_Packet* Sprite_Packet);

void Render_packet_add_UI_data(Render_Packet* renderer_packets, UI_Packet* UI_Packet);



#endif
