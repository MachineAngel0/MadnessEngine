#include "heap_allocator.h"

#include "asserts.h"


void allocator_heap_free_all(Heap_Allocator* ha)
{
    ha->head = ha->data;
    ha->head->is_free = true;
    ha->head->block_size = ha->capacity - sizeof(Heap_Block);
    ha->head->next = NULL;
    ha->head->prev = NULL;
    ha->allocation_calls = 0;
    ha->free_calls = 0;
    //  ha->free_cache_head = NULL;
    // ha->free_cache_tail = NULL;
}

void allocator_heap_init(Heap_Allocator* ha, void* backing_memory, const size_t memory_size)
{
    MASSERT(ha)
    MASSERT(backing_memory)
    MASSERT(memory_size > 0)


    ha->data = backing_memory;
    ha->capacity = memory_size;
    ha->used = 0;

    ha->head = ha->data;
    ha->head->is_free = true;
    ha->head->block_size = memory_size - sizeof(Heap_Block);
    ha->head->next = NULL;
    ha->head->prev = NULL;
}

void* allocator_heap_alloc_aligned(Heap_Allocator* ha, size_t size, size_t alignment)
{
    //TODO: alignment
    MASSERT(ha);
    if (size <= 0)
    {
        WARN("allocator_heap_alloc_aligned: size is 0 or less");
        return NULL;
    }


    Heap_Block* current = ha->head;
    //check to see if we have enough memory
    while (current)
    {
        if ((current->is_free == true) && current->block_size > size + sizeof(Heap_Block))
        {
            break;
        }
        current = current->next;
    }

    if (!current)
    {
        MASSERT_MSG(false, "ALLOCATOR FREE LIST: RAN OUT OF MEMORY");
    }

    //look at the next block spot in memory
    Heap_Block* new_block = (Heap_Block*)((u8*)current + sizeof(Heap_Block) + size);
    new_block->prev = current;
    new_block->next = NULL;
    new_block->block_size = current->block_size - sizeof(Heap_Block) - size;
    new_block->is_free = true;


    current->is_free = false;
    current->block_size = size;
    current->next = new_block;

    ha->head = new_block;
    ha->used += size + sizeof(Heap_Block);

    /*void* out_data = (void*)((u8*)current + sizeof(Heap_Block));
    memset(out_data,0,current->block_size);*/
    ha->allocation_calls++;

    return (void*)((u8*)current + sizeof(Heap_Block));
}

void* allocator_heap_alloc(Heap_Allocator* ha, const size_t size)
{
    return allocator_heap_alloc_aligned(ha, size,DEFAULT_ALIGNMENT);
}

void allocator_heap_free(Heap_Allocator* ha, void* ptr)
{
    Heap_Block* free_block = (Heap_Block*)((u8*)ptr - sizeof(Heap_Block));
    free_block->is_free = true;
    ha->used -= free_block->block_size + sizeof(Heap_Block);


    if (free_block->prev && (free_block->prev->is_free == true))
    {
        free_block->prev->block_size += free_block->block_size;
        // [prev] [current] [next]
        //[prev] -> [next]
        free_block->prev->next = free_block->next->next;

        free_block->next = NULL;
        free_block->prev = NULL;
    }

    if (free_block->next && (free_block->next->is_free == true))
    {
        Heap_Block* next_block = free_block->next;
        free_block->block_size += free_block->next->block_size;

        // [current] [next] [next next]
        // [current] -> [next next]
        free_block->next = free_block->next->next;

        next_block->next = NULL;
        next_block->prev = NULL;
    }
    ha->free_calls++;

}

void allocator_heap_debug_print(Heap_Allocator* ha)
{
    Heap_Block* current = ha->head;
    u64 block_number = 0;
    while (current)
    {
        INFO("Current block [%llu]: size: %llu, free: %d", block_number++, current->block_size, current->is_free);
        current = current->next;
    }

    /*
    current = ha->free_cache;

    while (current)
    {
        printf("Recently Free blocks : size: %llu\n", current->block_size);
       current = current->next;
    }*/
}

void allocator_heap_test(void)
{
    TEST_START("FREE LIST ALLOCATOR");

    Heap_Allocator* ha = malloc(sizeof(Heap_Allocator));
    u64 memory_amount = MB(1);
    void* backing_memory = malloc(memory_amount);
    allocator_heap_init(ha, backing_memory, memory_amount);
    allocator_heap_debug_print(ha);


    s32* i = allocator_heap_alloc(ha, sizeof(s32));
    *i = 4;
    s32* i2 = allocator_heap_alloc(ha, sizeof(s32));
    s64* i3 = allocator_heap_alloc(ha, sizeof(s64));
    f32* f = allocator_heap_alloc(ha, sizeof(f32));
    allocator_heap_debug_print(ha);


    allocator_heap_free(ha, i);
    allocator_heap_free(ha, i2);
    allocator_heap_debug_print(ha);
    s32* i4 = allocator_heap_alloc(ha, sizeof(s32));
    allocator_heap_debug_print(ha);

    s32 array_s[100];


    s32* array_boi = allocator_heap_alloc(ha, sizeof(s32) * 100);
    array_boi[99] = 1;
    allocator_heap_debug_print(ha);
    allocator_heap_free(ha, array_boi);
    allocator_heap_debug_print(ha);


    allocator_heap_free_all(ha);
    allocator_heap_debug_print(ha);

    free(backing_memory);
    free(ha);

    TEST_END("FREE LIST ALLOCATOR");
}
