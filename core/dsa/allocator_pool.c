#include "allocator_pool.h"





void pool_allocator_init(Pool_Allocator* allocator, void* backing_buffer, size_t backing_buffer_length,
                                    size_t chunk_size, size_t chunk_alignment)
{
    // Align backing buffer to the specified chunk alignment
    uintptr_t initial_start = (uintptr_t)backing_buffer;
    uintptr_t start = align_forward_uintptr(initial_start, (uintptr_t)chunk_alignment);
    backing_buffer_length -= (size_t)(start - initial_start);

    // Align chunk size up to the required chunk_alignment
    chunk_size = align_forward_size(chunk_size, chunk_alignment);

    allocator->memory = (u8*)backing_buffer;
    allocator->chunk_size = chunk_size;
    allocator->capacity = backing_buffer_length;
    allocator->head = NULL;

    // Set up the free list for free chunks
    pool_allocator_free_all(allocator);

}

void* pool_allocator_alloc(Pool_Allocator* p)
{
    // Get latest free node
    Pool_Free_Node* node = p->head;

    if (node == NULL)
    {
        MASSERT(0 && "Pool allocator has no free memory");
        return NULL;
    }

    // Pop free node
    p->head = p->head->next;

    // return and zero memory by default
    return memset(node, 0, p->chunk_size);
}



void pool_allocator_free(Pool_Allocator* a, void* ptr)
{
    Pool_Free_Node* node;

    void* start = a->memory;
    void* end = &a->memory[a->capacity];

    if (ptr == NULL)
    {
        // Ignore NULL pointers
        return;
    }

    if (!(start <= ptr && ptr < end))
    {
        MASSERT(0 && "Memory is out of bounds of the buffer in this pool");
        return;
    }

    // Push free node
    node = (Pool_Free_Node*)ptr;
    node->next = a->head;
    a->head = node;
}

void pool_allocator_free_all(Pool_Allocator* a)
{
    size_t chunk_count = a->capacity / a->chunk_size;

    // Set all chunks to be free
    for (size_t i = 0; i < chunk_count; i++)
    {
        void* ptr = &a->memory[i * a->chunk_size];
        Pool_Free_Node* node = (Pool_Free_Node*)ptr;
        // Push free node onto the free list
        node->next = a->head;
        a->head = node;
    }
}

uintptr_t align_forward_uintptr(uintptr_t ptr, uintptr_t align)
{
    uintptr_t a, p, modulo;

    MASSERT(is_power_of_two(align));

    a = align;
    p = ptr;
    modulo = p & (a - 1);
    if (modulo != 0)
    {
        p += a - modulo;
    }
    return p;
}

size_t align_forward_size(size_t ptr, size_t align)
{
    size_t a, p, modulo;

    MASSERT(is_power_of_two((uintptr_t)align));

    a = align;
    p = ptr;
    modulo = p & (a - 1);
    if (modulo != 0)
    {
        p += a - modulo;
    }
    return p;
}
