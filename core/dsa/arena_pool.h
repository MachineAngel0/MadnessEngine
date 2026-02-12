#ifndef ARENA_POOL_H
#define ARENA_POOL_H


//a very lean linked list
typedef struct Pool_Free_Node Pool_Free_Node;

struct Pool_Free_Node
{
    Pool_Free_Node* next;
};

// Differs from the free list in that all allocations are of equal size
typedef struct Arena_Pool
{
    u8* memory;
    size_t capacity;
    size_t chunk_size;

    Pool_Free_Node* head;
} Arena_Pool;


Arena_Pool* arena_pool_init(void* backing_buffer, size_t backing_buffer_length,
                            size_t chunk_size, size_t chunk_alignment);



void arena_pool_free_all(Arena_Pool* a);

void arena_pool_free(Arena_Pool* a, void* ptr);

void* arena_pool_alloc(Arena_Pool* p);




uintptr_t align_forward_uintptr(uintptr_t ptr, uintptr_t align);
size_t align_forward_size(size_t ptr, size_t align);

#endif
