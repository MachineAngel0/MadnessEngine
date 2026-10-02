#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stdlib.h>
#include "defines.h"



typedef struct Allocator
{
    u8* memory;
    u64 current_offset; // where in our memory we are
    u64 capacity; // how large our allocator is currently

    //u64 max_memory_used; // for debugging and memory management

    /*u64 reserved_virtual_address_space;
    size_t page_size;
    bool uses_virtual_memory;*/

    const char* name;
} Allocator;

typedef Allocator Frame_Allocator;


//NOTE: memory tracker is optional and can be NULL
MAPI void allocator_init(Allocator* a, void* backing_buffer, u64 backing_buffer_size, const char* name);

MAPI void allocator_clear(Allocator* a);
MAPI void allocator_clear_and_zero(Allocator* a);

//you can use align = 1, if you dont care about alignment, otherwise typically 4 or 8
MAPI void* allocator_alloc_align(Allocator* a, u64 mem_request, u64 align);

// in the event we just want a default alignment
MAPI void* allocator_alloc(Allocator* a, u64 mem_request);

MAPI u64 allocator_get_memory_left(const Allocator* a);

MAPI void allocator_debug_print(Allocator* a);


//also called a Scratch Allocator
//used allocator memory temporarily before resetting it back to its prev offset
typedef struct Scratch_Allocator
{
    Allocator* allocator;
    size_t prev_offset;
} Scratch_Allocator;

//typically this will be used in local scope, and freed off the stack when done,
//so its recommended that it's not a pointer
MAPI Scratch_Allocator scratch_allocator_begin(Allocator* a);

MAPI void scratch_allocator_end(Scratch_Allocator temp);


void allocator_test(void);


#endif //ALLOCATOR_H
