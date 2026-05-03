#ifndef POOL_ALLOCATOR_H
#define POOL_ALLOCATOR_H


//a very lean linked list
typedef struct Pool_Free_Node Pool_Free_Node;

struct Pool_Free_Node
{
    Pool_Free_Node* next;
};

// Differs from the free list in that all allocations are of equal size
typedef struct Pool_Allocator
{
    u8* memory;
    size_t capacity;
    size_t chunk_size;

    Pool_Free_Node* head;
} Pool_Allocator;


Pool_Allocator* pool_allocator_init(void* backing_buffer, size_t backing_buffer_length,
                            size_t chunk_size, size_t chunk_alignment);

void pool_allocator_free_all(Pool_Allocator* a);

void pool_allocator_free(Pool_Allocator* a, void* ptr);

void* pool_allocator_alloc(Pool_Allocator* p);

//INTERFACE
MAPI void* pool_allocator_interface_alloc(void* allocator,  u64 memory_byte_request, u8 alignment);
MAPI void pool_allocator_interface_free(void* allocator, void* memory_block);


uintptr_t align_forward_uintptr(uintptr_t ptr, uintptr_t align);
size_t align_forward_size(size_t ptr, size_t align);

#endif
