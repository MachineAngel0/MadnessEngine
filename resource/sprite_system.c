#include "sprite_system.h"

#include "arena.h"
#include "maths/math_types.h"


Sprite_System* sprite_system_init(Memory_System* memory_system)
{
    u64 memory_capacity = MB(1);


    Sprite_System* sprite_system = memory_system_alloc(memory_system, sizeof(Sprite_System), MEMORY_SUBSYSTEM_SPRITE);

    sprite_system->arena = memory_system_alloc(memory_system, sizeof(Arena), MEMORY_SUBSYSTEM_SPRITE);
    sprite_system->frame_arena = memory_system_alloc(memory_system, sizeof(Frame_Arena), MEMORY_SUBSYSTEM_SPRITE);

    void* arena_memory = memory_system_alloc(memory_system, memory_capacity, MEMORY_SUBSYSTEM_SPRITE);
    void* frame_arena_memory = memory_system_alloc(memory_system, memory_capacity, MEMORY_SUBSYSTEM_SPRITE);

    arena_init(sprite_system->arena, arena_memory, memory_capacity, NULL);
    arena_init(sprite_system->arena, frame_arena_memory, memory_capacity, NULL);


    //create one sprite, that will get transformed based on the instance
    //we also keep its pivot at the center
    memcpy(sprite_system->sprites, default_sprite, sizeof(default_sprite));
    memcpy(sprite_system->sprite_indices, default_sprite_indices, sizeof(default_sprite_indices));


    sprite_system->sprites_data = Sprite_Data_array_create(MAX_SPRITE_COUNT);
    sprite_system->sprites_data_transient = Sprite_Data_array_create(MAX_SPRITE_COUNT);
    sprite_system->ui_sprite_data = Sprite_Data_array_create(MAX_SPRITE_COUNT);
    sprite_system->text_sprite_data = Sprite_Data_array_create(MAX_SPRITE_COUNT);

    //TODO: replace with a fill function later
    Sprite_Data instance_data = {0};
    for (u64 i = 0; i < sprite_system->sprites_data->capacity; i++)
    {
        sprite_system->sprites_data->data[i] = instance_data;
    }


    return sprite_system;
}

bool sprite_system_shutdown(Sprite_System* sprite_system, Memory_System* memory_system)
{

    MASSERT(sprite_system);
    memory_system_memory_free(memory_system, sprite_system);

    sprite_system = NULL;


    return true;
}

void sprite_system_begin(Sprite_System* sprite_system, i32 screen_size_x, i32 screen_size_y)
{
    MASSERT(sprite_system)
    sprite_system->screen_size.x = (float)screen_size_x;
    sprite_system->screen_size.y = (float)screen_size_y;

    Sprite_Data_array_clear(sprite_system->sprites_data_transient);
    Sprite_Data_array_clear(sprite_system->ui_sprite_data);
    Sprite_Data_array_clear(sprite_system->text_sprite_data);

    arena_clear(sprite_system->frame_arena);
    // Sprite_Data_array_clear(sprite_system->sprites_data);
}


Sprite_Data* sprite_system_get_new_sprite_transient(Sprite_System* sprite_system)
{
    return &sprite_system->sprites_data_transient->data[sprite_system->sprites_data_transient->num_items++];
}

Sprite_Data* sprite_system_get_ui_sprite(Sprite_System* sprite_system)
{
    return &sprite_system->ui_sprite_data->data[sprite_system->ui_sprite_data->num_items++];
}

Sprite_Data* sprite_system_get_text_sprite(Sprite_System* sprite_system)
{
    return &sprite_system->text_sprite_data->data[sprite_system->text_sprite_data->num_items++];
}

bool sprite_system_generate_render_packet(Sprite_System* sprite_system, Render_Packet_UI* out_ui_packet,
                                          Render_Packet_Sprite* out_sprite_packet)
{
    out_ui_packet->system_name = "Madness UI";
    out_ui_packet->text_data_packet = sprite_system->text_sprite_data;
    out_ui_packet->ui_data_packet = sprite_system->ui_sprite_data;
    out_ui_packet->ui_index_type = VK_INDEX_TYPE_UINT16;
    out_ui_packet->text_index_type = VK_INDEX_TYPE_UINT16;

    out_sprite_packet->system_name = "Sprite System";
    out_sprite_packet->sprite_data = sprite_system->sprites_data;
    out_sprite_packet->sprite_data_transient = sprite_system->sprites_data_transient;
    memcpy(out_sprite_packet->sprite_indices, sprite_system->sprite_indices, sizeof(u16) * 6);


    return true;
}


Sprite_Data* sprite_create_minimal(Frame_Arena* frame_arena)
{
    return (Sprite_Data*)arena_alloc(frame_arena, sizeof(Sprite_Data));
}
