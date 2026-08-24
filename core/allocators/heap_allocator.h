#ifndef HEAP_ALLOCATOR_H
#define HEAP_ALLOCATOR_H


#include <stdbool.h>

#include "defines.h"


//TODO: rn I dont track have a concept of tracking individual memory allocations
// TODO: turn into a red black tree if performance ever becomes a problem

// An intrusive linked list for the free memory blocks
typedef struct Heap_Block
{
    struct Heap_Block* next;
    struct Heap_Block* prev;
    u64 block_size;
    u64 is_free;
} Heap_Block;


/*
 *recently freed cache
 *typedef struct  Heap_Free_List_Cache{
    struct Heap_Free_List_Cache* next;
    Heap_Block* ref;

}Heap_Free_List_Cache;*/

#define FREE_LIST_CACHE_SIZE 16

typedef struct Heap_Allocator
{
    void* data;

    Heap_Block* head;
    // Free_List_Cache* free_cache_head;

    u64 capacity;
    u64 used;
    // u8 free_cache_count;
    u64 allocation_calls;
    u64 free_calls;

} Heap_Allocator;



void allocator_heap_init(Heap_Allocator* ha, void* backing_memory,  size_t memory_size);

void* allocator_heap_alloc(Heap_Allocator* ha,  size_t size);
void* allocator_heap_alloc_aligned(Heap_Allocator* ha, size_t size, size_t alignment);


void allocator_heap_free(Heap_Allocator* ha, void* ptr);
void allocator_heap_free_all(Heap_Allocator* ha);


void allocator_heap_debug_print(Heap_Allocator* ha);

void allocator_heap_test(void);


#endif //FREE_LIST_ALLOCATOR_H
