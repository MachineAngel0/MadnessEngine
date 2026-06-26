#ifndef ALLOCATOR_MALLOC_H
#define ALLOCATOR_MALLOC_H

#include <stddef.h>

#include "darray.h"


//This isn't meant to be fast,

typedef enum Malloc_Cxt
{
    Malloc_Cxt_Invalid,
    Malloc_Cxt_Free,
    Malloc_Cxt_Default,
    Malloc_Cxt_Max,
} Malloc_Cxt;

const char* Malloc_Cxt_String[Malloc_Cxt_Max] =
{
    [Malloc_Cxt_Default] = "Malloc_Cxt_Default",
    [Malloc_Cxt_Free] = "Malloc_Cxt_FREE",
};


typedef struct Malloc_Block
{
    Malloc_Cxt context;
    size_t alloc_size;
    void* data;
} Malloc_Block;

typedef struct Malloc_Block_Query
{
    Malloc_Cxt context;
    size_t alloc_size;
} Malloc_Block_Query;

typedef struct Allocator_Malloc
{
    Malloc_Block* blocks;
    u64 block_count;
    u64 max_block_count;

    u64 used;
    Malloc_Cxt current_cxt;
} Allocator_Malloc;

static Allocator_Malloc* _allocator_malloc;

void allocator_malloc_init()
{
    _allocator_malloc = malloc(sizeof(Allocator_Malloc));
    _allocator_malloc->max_block_count = 100;
    _allocator_malloc->blocks = calloc(_allocator_malloc->max_block_count, sizeof(Malloc_Block));

    _allocator_malloc->block_count = 0;
    _allocator_malloc->used = 0;
    _allocator_malloc->current_cxt = Malloc_Cxt_Default;
}


void allocator_malloc_set_ctx(const Malloc_Cxt cxt)
{
    _allocator_malloc->current_cxt = cxt;
}

void* allocator_malloc(const size_t size)
{
    if (!_allocator_malloc)
    {
        allocator_malloc_init();
    }

    if (_allocator_malloc->block_count == _allocator_malloc->max_block_count)
    {
        _allocator_malloc->max_block_count *= 2;
        _allocator_malloc->blocks = realloc(_allocator_malloc->blocks,
                                            sizeof(Malloc_Block) * _allocator_malloc->max_block_count);
    }


    Malloc_Block* block = &_allocator_malloc->blocks[_allocator_malloc->block_count++];
    block->context = _allocator_malloc->current_cxt;
    block->alloc_size = size;
    block->data = malloc(size);

    return block->data;
}


void allocator_malloc_free(void* ptr)
{
    for (u64 i = 0; i < _allocator_malloc->block_count; i++)
    {
        if (_allocator_malloc->blocks[i].data == ptr)
        {
            free(ptr);

            _allocator_malloc->blocks[i].alloc_size = 0;
            _allocator_malloc->blocks[i].context = Malloc_Cxt_Free;
            _allocator_malloc->blocks[i].data = NULL;
            //swap the back with the current
            _allocator_malloc->blocks[i] = _allocator_malloc->blocks[_allocator_malloc->block_count - 1];
            _allocator_malloc->block_count--;

            return;
        }
    }
}

void allocator_malloc_free_all()
{
    for (u64 i = 0; i < _allocator_malloc->block_count; i++)
    {
        Malloc_Block* block = &_allocator_malloc->blocks[i];
        free(block->data);
        block->context = Malloc_Cxt_Free;
    }
    _allocator_malloc->block_count = 0;
}


Malloc_Block_Query allocator_malloc_query_info(void* ptr)
{
    for (u64 i = 0; i < _allocator_malloc->block_count; i++)
    {
        Malloc_Block* block = &_allocator_malloc->blocks[i];
        if (block->data == ptr)
        {
            return ( Malloc_Block_Query){.context = block->context, .alloc_size = block->alloc_size};
        }
    }

    return ( Malloc_Block_Query){.context = Malloc_Cxt_Invalid, .alloc_size = 0};
}

void allocator_malloc_debug_print()
{
    if (!_allocator_malloc)
    {
        INFO("nothing allocated");
        return;
    }

    for (u64 i = 0; i < _allocator_malloc->block_count; i++)
    {
        Malloc_Block* block = &_allocator_malloc->blocks[i];

        INFO("block[%llu], %s, %llu", i, Malloc_Cxt_String[block->context], block->alloc_size);
    }
}


void allocator_malloc_test()
{
    TEST_START("ALLOCATOR MALLOC");

    s32* hi = allocator_malloc(sizeof(s32));
    f64* byte = allocator_malloc(sizeof(f64));

    allocator_malloc_debug_print();

    allocator_malloc_free(hi);
    allocator_malloc_debug_print();
    allocator_malloc_free(byte);

    allocator_malloc_debug_print();
    allocator_malloc_free_all(byte);
    allocator_malloc_debug_print();

    TEST_END("ALLOCATOR MALLOC");
}


#endif //ALLOCATOR_MALLOC_H
