#ifndef ARENA_POOL_H
#define ARENA_POOL_H

#include <stdint.h>
#include "linked_list.h"

//a very lean linked list
typedef struct Pool_Free_Node Pool_Free_Node;

struct Pool_Free_Node
{
    Pool_Free_Node* next;
};

// Differs from the free list in that all allocations are of equal size
typedef struct Arena_Pool
{
    uint8_t* memory;
    size_t capacity;
    size_t chunk_size;

    // I can always use my other implemented linked list,
    // if I feel inclined and need the extra functionality
    Pool_Free_Node* head;
} Arena_Pool;


void arena_pool_free_all(Arena_Pool* a);

Arena_Pool* arena_pool_init(void* backing_buffer, size_t backing_buffer_length,
                            size_t chunk_size, size_t chunk_alignment);

void* arena_pool_alloc(Arena_Pool* p);

void arena_pool_memory_free(Arena_Pool* a);


void arena_pool_free(Arena_Pool* a, void* ptr);

void arena_pool_free_all(Arena_Pool* a);

uintptr_t align_forward_uintptr(uintptr_t ptr, uintptr_t align);
size_t align_forward_size(size_t ptr, size_t align);

#endif
