#pragma once
#include <stdint.h>

#include "arena.h"
#include "linked_list.h"


uintptr_t align_forward_uintptr(uintptr_t ptr, uintptr_t align) {
    uintptr_t a, p, modulo;

    MASSERT(is_power_of_two(align));

    a = align;
    p = ptr;
    modulo = p & (a-1);
    if (modulo != 0) {
        p += a - modulo;
    }
    return p;
}

size_t align_forward_size(size_t ptr, size_t align) {
    size_t a, p, modulo;

    MASSERT(is_power_of_two((uintptr_t)align));

    a = align;
    p = ptr;
    modulo = p & (a-1);
    if (modulo != 0) {
        p += a - modulo;
    }
    return p;
}

//a very lean linked list
typedef struct Pool_Free_Node Pool_Free_Node;
struct Pool_Free_Node {
    Pool_Free_Node *next;
};

// Differs from the free list in that all allocations are of equal size
typedef struct Arena_Pool
{
    uint8_t* memory;
    size_t capacity;
    size_t chunk_size;

    // I can always use my other implemented linked list,
    // if I feel inclined and need the extra functionality
    Pool_Free_Node *head;

} Arena_Pool;


void arena_pool_free_all(Arena_Pool *a);

Arena_Pool* arena_pool_init(void* backing_buffer, size_t backing_buffer_length,
                     size_t chunk_size, size_t chunk_alignment)
{
    Arena_Pool* a = malloc(sizeof(Arena_Pool));
    // Align backing buffer to the specified chunk alignment
    uintptr_t initial_start = (uintptr_t)backing_buffer;
    uintptr_t start = align_forward_uintptr(initial_start, (uintptr_t)chunk_alignment);
    backing_buffer_length -= (size_t)(start-initial_start);

    // Align chunk size up to the required chunk_alignment
    chunk_size = align_forward_size(chunk_size, chunk_alignment);

    a->memory = (uint8_t*)backing_buffer;
    a->chunk_size = chunk_size;
    a->capacity = backing_buffer_length;
    a->head = NULL;

    // Set up the free list for free chunks
    arena_pool_free_all(a);
}

void* arena_pool_alloc(Arena_Pool *p)
{
    // Get latest free node
    Pool_Free_Node *node = p->head;

    if (node == NULL) {
        MASSERT(0 && "Pool allocator has no free memory");
        return NULL;
    }

    // Pop free node
    p->head = p->head->next;

    // Zero memory by default
    return memset(node, 0, p->chunk_size);
}


void arena_pool_memory_free(Arena_Pool *a)
{
    free(a->memory);
    free(a);
}



void arena_pool_free(Arena_Pool *a, void* ptr)
{
    Pool_Free_Node *node;

    void *start = a->memory;
    void *end = &a->memory[a->capacity];

    if (ptr == NULL) {
        // Ignore NULL pointers
        return;
    }

    if (!(start <= ptr && ptr < end)) {
        MASSERT(0 && "Memory is out of bounds of the buffer in this pool");
        return;
    }

    // Push free node
    node = (Pool_Free_Node *)ptr;
    node->next = a->head;
    a->head = node;
}

void arena_pool_free_all(Arena_Pool *a)
{
    size_t chunk_count = a->capacity / a->chunk_size;
    size_t i;

    // Set all chunks to be free
    for (i = 0; i < chunk_count; i++) {
        void *ptr = &a->memory[i * a->chunk_size];
        Pool_Free_Node *node = (Pool_Free_Node *)ptr;
        // Push free node onto the free list
        node->next = a->head;
        a->head = node;
    }
}
