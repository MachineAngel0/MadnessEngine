#ifndef VIRTUAL_ALLOCATOR_H
#define VIRTUAL_ALLOCATOR_H

#include "asserts.h"
#include "defines.h"
#include "dsa_utility.h"
#include "platform.h"


typedef struct Virtual_Allocator
{
    u8* memory;
    u64 current_offset; // where in our memory we are
    u64 current_commit_capacity;

    u64 reserved_virtual_address_space;
    size_t page_size;
} Virtual_Allocator;

typedef Virtual_Allocator VFrame_Allocator;


//virtual allocator
MAPI void virtual_allocator_init(Virtual_Allocator* a,
                                 void* backing_buffer,
                                 u64 intial_commit_size,
                                 u64 page_size,
                                 u64 reserved_virtual_address_space)
{
    MASSERT(a);
    MASSERT(backing_buffer);

    //we pass in an already allocated chunk of memory in the event,
    //we want to pass in an already allocated allocator memory, say a global allocator, and then one for audio for something similar
    a->memory = (u8*)backing_buffer;
    a->current_offset = 0;
    a->current_commit_capacity = intial_commit_size;
    a->page_size = page_size;
    a->reserved_virtual_address_space = reserved_virtual_address_space;
}

MAPI void* allocator_init_virtual_alloc_aligned(Virtual_Allocator* a, const u64 mem_request, const u64 align)
{
    //align the memory
    uintptr_t curr_ptr = (uintptr_t)a->memory + (uintptr_t)a->current_offset; // get current memory address
    uintptr_t offset = align_forward(curr_ptr, align); // offset needed to align memory
    offset -= (uintptr_t)a->memory; // if zero then memory was already aligned


    //NOTE: going past our reserved memory is a hard crash
    if (offset + mem_request > a->reserved_virtual_address_space)
    {
        MASSERT_MSG_FALSE("VIRTUAL ALLOCATOR OUT OF MEMORY")
        return NULL;
    }

    //see if we have space left
    if (offset + mem_request > a->current_commit_capacity)
    {

        MASSERT_MSG_FALSE("ALLOCATOR OUT OF MEMORY")
        return NULL;
    }

    //get the requested memory
    void* ptr = &a->memory[offset]; // getting the start of the memory we are going to be returning
    a->current_offset = offset + mem_request;
    // Zero new memory by default
    memset(ptr, 0, mem_request); // already offset so no need to include it

    return ptr; // return the memory
}

MAPI void* allocator_init_virtual_alloc(Virtual_Allocator* a, const u64 mem_request)
{
    allocator_init_virtual_alloc_aligned(a, mem_request, DEFAULT_ALIGNMENT);
}

void virtual_allocator_init_test()
{
    Virtual_Allocator a = {0};


    u64 paging_size = platform_query_page_size();
    void* backing_buffer = platform_virtual_allocate_reserve(MB(256));
    u64 initial_commit_memory = MB(256) / 4;
    platform_virtual_allocate_commit(backing_buffer, initial_commit_memory);


    virtual_allocator_init(&a,
                           backing_buffer,
                           initial_commit_memory,
                           paging_size,
                           MB(256));
}

#endif //VIRTUAL_ALLOCATOR_H
