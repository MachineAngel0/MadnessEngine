#include "allocator_heap.h"

#include "asserts.h"


void allocator_heap_free_all(Heap_Allocator* fl)
{

    fl->head = fl->data;
    fl->head->free = true;
    fl->head->block_size = fl->capacity - sizeof(Heap_Block);
    fl->head->next = NULL;
    fl->head->prev = NULL;

    // fl->free_cache_head = NULL;
    // fl->free_cache_tail = NULL;
}

void allocator_heap_init(Heap_Allocator* fl, void* backing_memory, const size_t memory_size)
{
    MASSERT(fl)
    MASSERT(backing_memory)
    MASSERT(memory_size > 0)


    fl->data = backing_memory;
    fl->capacity = memory_size;
    fl->used = 0;

    fl->head = fl->data;
    fl->head->free = true;
    fl->head->block_size = memory_size - sizeof(Heap_Block);
    fl->head->next = NULL;
    fl->head->prev = NULL;



}

void* allocator_heap_alloc_aligned(Heap_Allocator* fl, size_t size, size_t alignment)
{
    MASSERT(fl);
    MASSERT(size > 0);


    Heap_Block* current = fl->head;
    //check to see if we have enough memory
    while (current)
    {
        if (current->free && current->block_size > size + sizeof(Heap_Block))
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
    new_block->free = true;


    current->free = false;
    current->block_size = size;
    current->next = new_block;

    fl->head = new_block;
    fl->used += size + sizeof(Heap_Block);


    return (void*)((u8*)current + sizeof(Heap_Block));

}

void* allocator_heap_alloc(Heap_Allocator* fl, const size_t size)
{
    return allocator_heap_alloc_aligned(fl, size,DEFAULT_ALIGNMENT);
}

void allocator_heap_free(Heap_Allocator* fl, void* ptr)
{
    Heap_Block* free_block = (Heap_Block*)((u8*)ptr - sizeof(Heap_Block));
    free_block->free = true;
    fl->used -= free_block->block_size + sizeof(Heap_Block);




    if (free_block->prev && free_block->prev->free)
    {
        free_block->prev->block_size += free_block->block_size;
        // [prev] [current] [next]
        //[prev] -> [next]
        free_block->prev->next = free_block->next->next;

        free_block->next = NULL;
        free_block->prev = NULL;
    }

    if (free_block->next && free_block->next->free)
    {
        Heap_Block* next_block = free_block->next;
        free_block->block_size += free_block->next->block_size;

        // [current] [next] [next next]
        // [current] -> [next next]
        free_block->next = free_block->next->next;

        next_block->next = NULL;
        next_block->prev = NULL;
    }


}

void allocator_heap_debug_print(Heap_Allocator* fl)
{
    Heap_Block* current = fl->head;
    u64 block_number = 0;
    while (current)
    {
        INFO("Current block [%llu]: size: %llu, free: %d", block_number++, current->block_size, current->free);
        current = current->next;
    }

    /*
    current = fl->free_cache;

    while (current)
    {
        printf("Recently Free blocks : size: %llu\n", current->block_size);
       current = current->next;
    }*/

}

void allocator_heap_test(void)
{
    TEST_START("FREE LIST ALLOCATOR");

    Heap_Allocator* fl = malloc(sizeof(Heap_Allocator));
    u64 memory_amount = MB(1);
    void* backing_memory = malloc(memory_amount);
    allocator_heap_init(fl, backing_memory, memory_amount);
    allocator_heap_debug_print(fl);


    s32* i = allocator_heap_alloc(fl, sizeof(s32));
    *i = 4;
    s32* i2 = allocator_heap_alloc(fl, sizeof(s32));
    s64* i3 = allocator_heap_alloc(fl, sizeof(s64));
    f32* f = allocator_heap_alloc(fl, sizeof(f32));
    allocator_heap_debug_print(fl);


    allocator_heap_free(fl, i);
    allocator_heap_free(fl, i2);
    allocator_heap_debug_print(fl);
    s32* i4 = allocator_heap_alloc(fl, sizeof(s32));
    allocator_heap_debug_print(fl);

    s32 array_s[100];



    s32* array_boi = allocator_heap_alloc(fl, sizeof(s32) * 100);
    array_boi[99] = 1;
    allocator_heap_debug_print(fl);
    allocator_heap_free(fl, array_boi );
    allocator_heap_debug_print(fl);





    allocator_heap_free_all(fl);
    allocator_heap_debug_print(fl);

    free(backing_memory);
    free(fl);

    TEST_END("FREE LIST ALLOCATOR");


}
