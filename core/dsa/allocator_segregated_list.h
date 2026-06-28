#ifndef ALLOCATOR_SEGREGATED_LIST_H
#define ALLOCATOR_SEGREGATED_LIST_H

#include "allocator_pool.h"

typedef struct Allocator_Segragated_List
{
    Pool_Allocator small_pool; // 1 kb and less
    Pool_Allocator meduim_pool; // 1 - 64 kb
    Pool_Allocator large_pool; // anything above 64 kb
} Allocator_Segragated_List;


void allocator_segragated_list_init(Allocator_Segragated_List* allocator, void* memory, size_t size)
{
    if (size < MB(3))
    {
        MASSERT_MSG(false, "allocator_segragated_list_init: arena too small")
    }

    u64 individual_memory_chunks = size / 3;

    pool_allocator_init(&allocator->large_pool, memory, individual_memory_chunks, KB(64), DEFAULT_ALIGNMENT);
    pool_allocator_init(&allocator->meduim_pool, memory, individual_memory_chunks, KB(64), DEFAULT_ALIGNMENT);
    pool_allocator_init(&allocator->small_pool, memory, individual_memory_chunks, KB(1), DEFAULT_ALIGNMENT);
}


void* allocator_segragated_list_alloc(Allocator_Segragated_List* allocator, size_t size)
{
    if (size >= KB(64))
    {
        return pool_allocator_alloc(&allocator->large_pool);
    }
    if (size <= KB(1))
    {
        return pool_allocator_alloc(&allocator->small_pool);
    }
    return pool_allocator_alloc(&allocator->meduim_pool);
}

void* allocator_segragated_list_free(Allocator_Segragated_List* allocator, void* ptr)
{
    // TODO: I need to change the structure up
    /*
    if (size >= KB(64))
    {
        return pool_allocator_free(&allocator->large_pool);
    }
    if (size <= KB(1))
    {
        return pool_allocator_free(&allocator->small_pool);
    }
    return pool_allocator_free(&allocator->meduim_pool);
    */

    return NULL;
}


#endif //ALLOCATOR_SEGREGATED_LIST_H
