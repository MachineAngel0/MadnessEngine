#include "sprite_system.h"

#include "allocator.h"
#include "maths/math_types.h"


Sprite_System* sprite_system_init(Memory_System* memory_system)
{
    u64 allocator_memory = MB(1);


    Sprite_System* sprite_system = memory_system_alloc(memory_system, sizeof(Sprite_System), MEMORY_SUBSYSTEM_SPRITE);


    sprite_system->allocator =memory_system_allocator_create(
        memory_system, allocator_memory, MEMORY_SUBSYSTEM_SPRITE, "sprite persistent");
    sprite_system->frame_allocator = memory_system_allocator_create(
        memory_system, allocator_memory, MEMORY_SUBSYSTEM_SPRITE, "sprite transient");


    //create one sprite, that will get transformed based on the instance
    //we also keep its pivot at the center
    memcpy(sprite_system->sprites, default_sprite, sizeof(default_sprite));
    memcpy(sprite_system->sprite_indices, default_sprite_indices, sizeof(default_sprite_indices));


    sprite_system->sprites_array = array_create(Sprite_Data, MAX_SPRITE_COUNT, sprite_system->allocator);
    sprite_system->sprites_transient_array = array_create(Sprite_Data, MAX_SPRITE_TRANSIENT_COUNT, sprite_system->allocator);



    return sprite_system;
}

bool sprite_system_shutdown(Sprite_System* sprite_system, Memory_System* memory_system)
{

    MASSERT(sprite_system);
    memory_system_memory_free(memory_system, sprite_system, MEMORY_SUBSYSTEM_SPRITE);

    sprite_system = NULL;


    return true;
}

void sprite_system_begin(Sprite_System* sprite_system)
{
    MASSERT(sprite_system)


    array_clear(sprite_system->sprites_transient_array);

    allocator_clear(sprite_system->frame_allocator);
    // Sprite_Data_array_clear(sprite_system->sprites_data);
}

Sprite_Handle sprite_acquire(Sprite_System* sprite_system)
{
    _array_get(sprite_system->sprites_array, sprite_system->sprites_array->num_items++);

    return (Sprite_Handle){sprite_system->sprites_array->num_items-1, 0};
}

Sprite_Data* sprite_system_get_sprite_data(Sprite_System* sprite_system, Sprite_Handle sprite_handle)
{
    //TODO: generation
    return &array_get(sprite_system->sprites_array, sprite_handle.handle, Sprite_Data);
}

void sprite_release(Sprite_System* sprite_system, Sprite_Handle sprite_handle)
{
    //TODO:
    MASSERT_FALSE();
}

void sprite_transient_draw(Sprite_System* sprite_system, Sprite_Data data)
{
    array_push(sprite_system->sprites_transient_array, &data);
}


Sprite_Data* sprite_system_new_get(Sprite_System* sprite_system)
{
    return _array_get(sprite_system->sprites_transient_array, sprite_system->sprites_transient_array->num_items++);
}



bool sprite_system_generate_render_packet(Sprite_System* sprite_system,
                                          Render_Packet_Sprite* out_sprite_packet)
{

    out_sprite_packet->system_name = "Sprite System";
    out_sprite_packet->sprite_data = sprite_system->sprites_array;
    out_sprite_packet->sprite_data_transient = sprite_system->sprites_transient_array;


    return true;
}


Sprite_Data* sprite_create_minimal(Frame_Allocator* frame_arena)
{
    return (Sprite_Data*)allocator_alloc(frame_arena, sizeof(Sprite_Data));
}
