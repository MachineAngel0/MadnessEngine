#pragma once
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "../core/logger.h"
#include "dsa_utility.h"


#ifndef DEFAULT_ALIGNMENT
//most likely to be 4 (32bit) or 8 (64bit) (* 2)
#define DEFAULT_ALIGNMENT (2*sizeof(void *))
#endif



typedef struct Arena
{
    uint8_t* memory;
    size_t current_offset; // where in our memory we are
    size_t capacity; // how large our arena is
} Arena;


//will malloc for memory, see arena_init for using an already existing arena memory
Arena* arena_init_global(size_t capacity)
{
    Arena* a = malloc(sizeof(Arena));
    a->memory = malloc(capacity);
    a->current_offset = 0;
    a->capacity = capacity;
    return a;
}

//used for arenas who get memory from other arenas
Arena* arena_init(void* backing_buffer, size_t capacity)
{
    //we pass in an already allocated chunk of memory in the event,
    //we want to pass in an already allocated arena memory, say a global arena, and then one for audio for something similar
    Arena* a = malloc(sizeof(Arena));
    a->memory = (uint8_t *) backing_buffer;
    a->current_offset = 0;
    a->capacity = capacity;
    return a;
}


void arena_clear(Arena* a)
{
    a->current_offset = 0;
}

void arena_free(Arena* a)
{
    free(a->memory);
    free(a);
}


//you can use align = 1, if you dont care about alignment, otherwise typically 4 or 8
void* arena_alloc_align(Arena* a, size_t mem_request, size_t align)
{
    //align the memory
    uintptr_t curr_ptr = (uintptr_t) a->memory + (uintptr_t) a->current_offset; // get current memory address
    uintptr_t offset = align_forward(curr_ptr, align); // offset needed to align memory
    offset -= (uintptr_t) a->memory; // if zero then memory was already aligned

    //see if we have space left
    if (offset + mem_request > a->capacity)
    {
        WARN("ARENA OUT OF MEMORY");
        return NULL;
    }

    //get the requested memory
    void* ptr = &a->memory[offset]; // getting the start of the memory we are going to be returning
    a->current_offset = offset + mem_request;
    // Zero new memory by default
    memset(ptr, 0, mem_request); // already offset so no need to include it
    return ptr; // return the memory
}

// in the event we just want a default alignment
void* arena_alloc(Arena* a, size_t mem_request)
{
    return arena_alloc_align(a, mem_request, DEFAULT_ALIGNMENT);
}


//also called a Scratch Arena
//used arena memory temporarily before resetting it back to its prev offset
typedef struct Arena_Temp
{
    Arena* arena;
    size_t prev_offset;
} Arena_Temp;

//typically this will be used in local scope, and freed off the stack when done,
//so its recommended that it's not a pointer
Arena_Temp temp_arena_memory_begin(Arena* a)
{
    Arena_Temp temp;
    temp.arena = a;
    temp.prev_offset = a->current_offset;
    return temp;
}

void temp_arena_memory_end(Arena_Temp temp)
{
    temp.arena->current_offset = temp.prev_offset;
}


//TODO:void* arena_resize(arena* arena, size_t mem_request, size_t align)
