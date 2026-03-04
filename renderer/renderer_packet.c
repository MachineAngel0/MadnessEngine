
#include "renderer_packet.h"


void render_packet_add_sprite_data(Render_Packet* renderer_packets, Sprite_Packet* Sprite_Packet)
{
   Sprite_Packet_array_push(renderer_packets->Sprite_packet, Sprite_Packet);
}

void Render_packet_add_UI_data(Render_Packet* renderer_packets, UI_Packet* UI_Packet)
{
   UI_Packet_array_push(renderer_packets->UI_packets, UI_Packet);
}


