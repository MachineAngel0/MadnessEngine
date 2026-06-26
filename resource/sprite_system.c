#include "sprite_system.h"

#include "allocator.h"
#include "maths/math_types.h"


Sprite_System* sprite_system_init(Memory_System* memory_system)
{
    u64 memory_capacity = MB(1);


    Sprite_System* sprite_system = memory_system_alloc(memory_system, sizeof(Sprite_System), MEMORY_SUBSYSTEM_SPRITE);

    sprite_system->allocator = memory_system_alloc(memory_system, sizeof(Allocator), MEMORY_SUBSYSTEM_SPRITE);
    sprite_system->frame_arena = memory_system_alloc(memory_system, sizeof(Frame_Allocator), MEMORY_SUBSYSTEM_SPRITE);

    void* arena_memory = memory_system_alloc(memory_system, memory_capacity, MEMORY_SUBSYSTEM_SPRITE);
    void* frame_arena_memory = memory_system_alloc(memory_system, memory_capacity, MEMORY_SUBSYSTEM_SPRITE);

    allocator_init(sprite_system->allocator, arena_memory, memory_capacity);
    allocator_init(sprite_system->allocator, frame_arena_memory, memory_capacity);


    //create one sprite, that will get transformed based on the instance
    //we also keep its pivot at the center
    memcpy(sprite_system->sprites, default_sprite, sizeof(default_sprite));
    memcpy(sprite_system->sprite_indices, default_sprite_indices, sizeof(default_sprite_indices));


    sprite_system->sprites_data = freelist_array_create(Sprite_Data, MAX_SPRITE_COUNT, sprite_system->allocator);
    sprite_system->sprites_frame_data = array_create(Sprite_Data, MAX_SPRITE_COUNT, sprite_system->allocator);



    return sprite_system;
}

bool sprite_system_shutdown(Sprite_System* sprite_system, Memory_System* memory_system)
{

    MASSERT(sprite_system);
    memory_system_memory_free(memory_system, sprite_system, MEMORY_SUBSYSTEM_SPRITE);

    sprite_system = NULL;


    return true;
}

void sprite_system_begin(Sprite_System* sprite_system, s32 screen_size_x, s32 screen_size_y)
{
    MASSERT(sprite_system)
    sprite_system->screen_size.x = (float)screen_size_x;
    sprite_system->screen_size.y = (float)screen_size_y;

    array_clear(sprite_system->sprites_frame_data);

    allocator_clear(sprite_system->frame_arena);
    // Sprite_Data_array_clear(sprite_system->sprites_data);
}

Sprite_Handle sprite_system_new_sprite(Sprite_System* sprite_system)
{
    Freelist_Array_Handle handle = fl_array_new(sprite_system->sprites_data);
    return (Sprite_Handle){handle.handle, handle.gen};
}

Sprite_Data* sprite_system_get_sprite_data(Sprite_System* sprite_system, Sprite_Handle* sprite_handle)
{
    return _fl_array_query(sprite_system->sprites_data, (Freelist_Array_Handle){sprite_handle->handle, sprite_handle->gen});
}

void sprite_system_sprite_release(Sprite_System* sprite_system, Sprite_Handle sprite_handle)
{
    fl_array_release(sprite_system->sprites_data, (Freelist_Array_Handle){sprite_handle.handle, sprite_handle.gen});
}


Sprite_Data* sprite_system_new_frame_sprite(Sprite_System* sprite_system)
{
    return _array_get(sprite_system->sprites_frame_data, sprite_system->sprites_frame_data->num_items++);
}



bool sprite_system_generate_render_packet(Sprite_System* sprite_system,
                                          Render_Packet_Sprite* out_sprite_packet)
{

    out_sprite_packet->system_name = "Sprite System";
    out_sprite_packet->sprite_data = sprite_system->sprites_data->array;
    out_sprite_packet->sprite_data_transient = sprite_system->sprites_frame_data;
    memcpy(out_sprite_packet->sprite_indices, sprite_system->sprite_indices, sizeof(u16) * 6);


    return true;
}


Sprite_Data* sprite_create_minimal(Frame_Allocator* frame_arena)
{
    return (Sprite_Data*)allocator_alloc(frame_arena, sizeof(Sprite_Data));
}
