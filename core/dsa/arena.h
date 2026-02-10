#ifndef ARENA_H
#define ARENA_H

#include <stdlib.h>


#include "../memory/memory_tracker.h"

#ifndef DEFAULT_ALIGNMENT
//most likely to be 4 (32bit) or 8 (64bit) (* 2)
#define DEFAULT_ALIGNMENT (2*sizeof(void *))
#endif


typedef struct Arena
{
    u8* memory;
    u64 current_offset; // where in our memory we are
    u64 capacity; // how large our arena is
} Arena;

typedef Arena Frame_Arena;
typedef Arena Renderer_Arena;
typedef Arena Entity_Arena;


//TODO: remove this
//will malloc for memory, should only be called by the application or whatever manages the entire lifetime of the program
//see arena_init for using an already existing arena memory/ block of memory
MAPI Arena* arena_init_malloc(const u64 capacity);


MAPI void arena_init(Arena* a, void* backing_buffer, const u64 backing_buffer_size,
                const memory_subsystem_type subsystem_type);

MAPI void arena_clear(Arena* a);

//Should only ever be called if we own the memory, which is most likely only the application arena
MAPI void arena_free(Arena* a);


//you can use align = 1, if you dont care about alignment, otherwise typically 4 or 8
MAPI void* arena_alloc_align(Arena* a, const u64 mem_request, const u64 align);

// in the event we just want a default alignment
MAPI void* arena_alloc(Arena* a, const u64 mem_request);

MAPI u64 arena_get_memory_left(const Arena* a);

MAPI void arena_debug_print(Arena* a);


//also called a Scratch Arena
//used arena memory temporarily before resetting it back to its prev offset
typedef struct Arena_Temp
{
    Arena* arena;
    size_t prev_offset;
} Arena_Temp;

//typically this will be used in local scope, and freed off the stack when done,
//so its recommended that it's not a pointer
MAPI Arena_Temp temp_arena_memory_begin(Arena* a);

MAPI void temp_arena_memory_end(Arena_Temp temp);


void arena_test(void);


#endif
