#include "renderer_packet.h"

Render_Packet* render_packet_init(Arena* arena)
{
    MASSERT(arena);
    Render_Packet* render_packet = arena_alloc(arena, sizeof(Render_Packet));
    return render_packet;
}

void render_packet_clear(Render_Packet* renderer_packets)
{
    memset(renderer_packets, 0, sizeof(Render_Packet));
}


