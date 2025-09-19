#pragma once

#include "Arena.h"

typedef struct Arena
{
    void* memory; //block of memory
    u64 size; // total memory size for this arena
    u64 allocated; //memory currently allocated
}Arena;


Arena* arena_init(u64 arena_memory_size);

void arena_destroy(Arena* arena);

//return the memory for the user to use
void* arena_alloc(Arena* arena, u64 allocation_size);

//TODO: shift the used back by the amount
//void* arena_dealloc(Arena* arena, u64 free_amounts);
