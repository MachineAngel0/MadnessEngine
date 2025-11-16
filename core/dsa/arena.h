#pragma once
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "../core/logger.h"
#include "dsa_utility.h"
#include "unit_test.h"
#include "memory_tracker.h"

#ifndef DEFAULT_ALIGNMENT
//most likely to be 4 (32bit) or 8 (64bit) (* 2)
#define DEFAULT_ALIGNMENT (2*sizeof(void *))
#endif


typedef struct Arena
{
    uint8_t* memory;
    u64 current_offset; // where in our memory we are
    u64 capacity; // how large our arena is
} Arena;


//will malloc for memory, should only be called by the application or whatever manages the entire lifetime of the program
//see arena_init for using an already existing arena memory/ block of memory
Arena* arena_init_malloc(const u64 capacity)
{
    Arena* a = (Arena *) malloc(sizeof(Arena));

    if (!a)
    {
        MASSERT("ARENA ALLOC FAILED");
        return NULL;
    }

    a->memory = (uint8_t *) malloc(capacity);
    if (!a->memory)
    {
        MASSERT("ARENA MALLOC FAILED");
        return NULL;
    }

    memory_container_alloc(MEMORY_CONTAINER_APPLICATION_ARENA, sizeof(a));
    memory_container_alloc(MEMORY_CONTAINER_APPLICATION_ARENA, capacity);


    a->current_offset = 0;
    a->capacity = capacity;
    return a;
}


void arena_init(Arena* a, void* backing_buffer, const u64 backing_buffer_size)
{
    //we pass in an already allocated chunk of memory in the event,
    //we want to pass in an already allocated arena memory, say a global arena, and then one for audio for something similar
    a->memory = (uint8_t *) backing_buffer;
    a->current_offset = 0;
    a->capacity = backing_buffer_size;

    //TODO: so it should be noted,
    // we dont actually keep track of how much memory an arena is giving out
    // which im going to think about for now
    memory_container_alloc(MEMORY_CONTAINER_ARENA, backing_buffer_size);
}


void arena_clear(Arena* a)
{
    a->current_offset = 0;
    memset(a->memory, 0, a->capacity);
}

//Should only ever be called if we own the memory, which is most likely only the application arena
void arena_free(Arena* a)
{
    memory_container_alloc(MEMORY_CONTAINER_APPLICATION_ARENA, a->capacity);
    free(a->memory);
    free(a);

}


//you can use align = 1, if you dont care about alignment, otherwise typically 4 or 8
void* arena_alloc_align(Arena* a, const u64 mem_request, const u64 align)
{
    //align the memory
    uintptr_t curr_ptr = (uintptr_t) a->memory + (uintptr_t) a->current_offset; // get current memory address
    uintptr_t offset = align_forward(curr_ptr, align); // offset needed to align memory
    offset -= (uintptr_t) a->memory; // if zero then memory was already aligned

    //see if we have space left
    if (offset + mem_request > a->capacity)
    {
        // MASSERT("ARENA OUT OF MEMORY");
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
void* arena_alloc(Arena* a, const u64 mem_request)
{
    return arena_alloc_align(a, mem_request, DEFAULT_ALIGNMENT);
}

u64 arena_get_memory_left(const Arena* a)
{
    return (a->capacity - a->current_offset);
}


void arena_debug_print(Arena* a)
{
    if (!a) return;
    INFO("ARENA CAPACITY: %llu", a->capacity);
    INFO("ARENA MEMORY USED: %llu", a->current_offset);
    INFO("ARENA MEMORY LEFT: %llu", a->capacity - a->current_offset);
}


//TODO: i would only use this as a means to debug, in the event there is no memory
//void* arena_resize(arena* arena, size_t mem_request, size_t align)

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


void arena_test()
{
    TEST_START("ARENA");
    Arena a = {0};
    const u64 arena_size = MB(1);
    void* mem = malloc(arena_size);
    if (!mem) { MASSERT("ARENA ALLOC FAILED"); }
    arena_init(&a, mem, arena_size);
    TEST_DEBUG(a.capacity == MB(1));


    //we dont have to do anything with the arr, just make sure we dont touch it again

    int* arr1 = arena_alloc(&a, 10 * sizeof(int));
    arr1[2] = 15;

    TEST_DEBUG(arr1[2] == 15);
    TEST_DEBUG(10 * sizeof(int) == a.current_offset);
    TEST_DEBUG(MB(1) == a.capacity);
    TEST_DEBUG(MB(1)-40 == a.capacity - a.current_offset);

    arena_clear(&a);
    TEST_DEBUG(a.current_offset==0);

    int* arr2 = arena_alloc(&a, 50 * sizeof(int));
    arr2[2] = 100;
    TEST_DEBUG(arr2[2] == 100);
    TEST_DEBUG((50 * sizeof(int)) == a.current_offset);
    TEST_DEBUG(MB(1) == a.capacity);
    TEST_DEBUG(MB(1) - (50 * sizeof(int)) == a.capacity - a.current_offset);

    arena_clear(&a);
    TEST_DEBUG(a.current_offset==0);


    char* char_arr = arena_alloc(&a, 100 * sizeof(char));
    TEST_DEBUG(a.current_offset == 100);
    u64* u64_arr = arena_alloc(&a, 100 * sizeof(u64));
    //912 due to alignemnt
    TEST_DEBUG(a.current_offset == 912);
    i32* i32_arr = arena_alloc(&a, 100 * sizeof(i32));
    TEST_DEBUG(a.current_offset == 1312);


    free(mem);
    mem = NULL;
    TEST_DEBUG(mem == NULL);


    Arena* a2 = arena_init_malloc(arena_size);
    TEST_DEBUG(a2);
    TEST_DEBUG(arena_size == a2->capacity);
    TEST_DEBUG(arena_size == a2->capacity - a2->current_offset);

    int* arr3 = arena_alloc(a2, 10 * sizeof(int));
    arr3[2] = 15;
    TEST_DEBUG(40 == a2->current_offset);
    TEST_DEBUG(arena_size == a2->capacity);
    TEST_DEBUG(arena_size-40 == a2->capacity - a2->current_offset);

    int* arr4 = arena_alloc(a2, 10 * sizeof(int));
    arr4[2] = 100;
    //its going to be 88 due to alignment
    TEST_DEBUG(88 == a2->current_offset);
    TEST_DEBUG(arena_size == a2->capacity);
    TEST_DEBUG(arena_size-88 == a2->capacity - a2->current_offset);


    arena_free(a2);

    TEST_REPORT("ARENA");
}
