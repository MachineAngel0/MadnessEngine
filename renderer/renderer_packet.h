#ifndef RENDERER_PACKET_H
#define RENDERER_PACKET_H


//these are all just references to the data, they do not own anything
typedef struct Render_Packet_Sprite
{
    Sprite_Data_array* sprite_data_packet;
    VkIndexType index_type;
    const char* system_name;
} Render_Packet_Sprite;

typedef struct Render_Packet_UI
{
    Sprite_Data_array* ui_data_packet;
    VkIndexType ui_index_type;

    Sprite_Data_array* text_data_packet;
    VkIndexType text_index_type;

    const char* system_name;

} Render_Packet_UI;


typedef struct Render_Packet
{
    //rn we just have one of each,
    //i don't want to deal with multiple instances of this data, if i need more create one for the specific system
    //the system just sets the data inside the struct, not the safest but fine for now
    Render_Packet_UI ui_data_packet;
    Render_Packet_Sprite sprite_data_packet;

    // geometry/mesh data at some point //TODO:
} Render_Packet;


MAPI Render_Packet* render_packet_init(Arena* arena);

//call before any system adds to the renderer packets
MAPI void render_packet_clear(Render_Packet* renderer_packets);




#endif
