//
// Created by Adams Humbert on 7/31/2025.
//

#include "Arena.h"
#include <stdlib.h>

Arena* arena_init(u64 arena_memory_size)
{
    Arena* arena = malloc(sizeof(arena));
    arena->allocated = 0;
    arena->size = arena_memory_size;
    arena->memory = malloc(arena_memory_size);
    // Zero new memory
    memset(arena->memory, 0, arena_memory_size);
    return arena;
}

void arena_destroy(Arena* arena)
{

    //free the memory
    free(arena->memory);
    //zero out everything
    arena->allocated = 0;
    arena->memory = 0;
    arena->size = 0;

    //idk if your suppose to call this
    //free(arena);

}

void* arena_alloc(Arena* arena, u64 allocation_size)
{
    //check to make sure we have enough memory left
    if (arena->allocated + allocation_size > arena->size)
    {
        printf("allocation is too big, arena is out of memory");
        return NULL;
    };

    //TODO: this line below may be wrong
    //void *ptr = &arena->memory[arena->allocated]; // another way i found of doing this
    void* block = ((u8*)arena->memory) + arena->allocated; // get a pointer to the block of memory requested
    arena->allocated += allocation_size; // increase allocted size

    return block;
}
