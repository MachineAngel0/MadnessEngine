#include "allocator_stack.h"


#include <stdlib.h>
#include <string.h>
#include "logger.h"
#include "dsa_utility.h"


//used for arenas who get memory from other arenas
void stack_allocator_init(Stack_Allocator* a, void* mem, const size_t mem_size)
{
    //we pass in an already allocated chunk of memory in the event,
    //we want to pass in an already allocated arena memory, say a global arena, and then one for audio for something similar
    a->memory = (uint8_t*)mem;
    a->current_offset = 0;
    a->capacity = mem_size;
}


void stack_allocator_clear(Stack_Allocator* a)
{
    a->current_offset = 0;
}


void stack_allocator_pop(Stack_Allocator* a, void* ptr)
{
    //pass back the
    if (!ptr)
    {
        M_ERROR("ARENA STACK POP: PASSED IN INVALID POINTER BACK");
        return;
    };

    uintptr_t start, end, curr_addr;
    Stack_Allocator_Header* header;
    size_t prev_offset;

    start = (uintptr_t)a->memory;
    end = start + (uintptr_t)a->current_offset;
    curr_addr = (uintptr_t)ptr;

    if (!(start <= curr_addr && curr_addr < end))
    {
        MASSERT(0 && "Out of bounds memory address passed to stack allocator (free)");
        return;
    }

    if (curr_addr >= start + (uintptr_t)a->current_offset)
    {
        // Allow double frees
        return;
    }

    header = (Stack_Allocator_Header*)(curr_addr - sizeof(Stack_Allocator_Header));
    prev_offset = (size_t)(curr_addr - (uintptr_t)header->data_size - start);

    a->current_offset = prev_offset;
}


size_t calc_padding_with_header(uintptr_t ptr, uintptr_t alignment, size_t header_size)
{
    uintptr_t p, a, modulo, padding, needed_space;

    MASSERT(is_power_of_two(alignment));

    p = ptr;
    a = alignment;
    modulo = p & (a - 1); // (p % a) as it assumes alignment is a power of two

    padding = 0;
    needed_space = 0;

    if (modulo != 0)
    {
        // Same logic as 'align_forward'
        padding = a - modulo;
    }

    needed_space = (uintptr_t)header_size;

    if (padding < needed_space)
    {
        needed_space -= padding;

        if ((needed_space & (a - 1)) != 0)
        {
            padding += a * (1 + (needed_space / a));
        }
        else
        {
            padding += a * (needed_space / a);
        }
    }

    return (size_t)padding;
}


//you can use align = 1, if you dont care about alignment, otherwise typically 4 or 8
void* stack_allocator_alloc_align(Stack_Allocator* a, const size_t mem_request, size_t alignment)
{
    uintptr_t curr_addr, next_addr;
    size_t padding;
    Stack_Allocator_Header* header;


    MASSERT(is_power_of_two(alignment));

    if (alignment > 128)
    {
        // As the padding is 8 bits (1 byte), the largest alignment that can
        // be used is 128 bytes
        alignment = 128;
    }

    curr_addr = (uintptr_t)a->memory + (uintptr_t)a->current_offset;
    padding = calc_padding_with_header(curr_addr, (uintptr_t)alignment, sizeof(Stack_Allocator_Header));
    if (a->current_offset + padding + mem_request > a->capacity)
    {
        // Stack allocator is out of memory
        return NULL;
    }
    a->current_offset += padding;

    next_addr = curr_addr + (uintptr_t)padding;
    header = (Stack_Allocator_Header*)(next_addr - sizeof(Stack_Allocator_Header));
    header->data_size = (uint8_t)padding;

    a->current_offset += mem_request;

    return memset((void*)next_addr, 0, mem_request);
}

// in the event we just want a default alignment
void* stack_allocator_alloc(Stack_Allocator* a, size_t mem_request)
{
    return stack_allocator_alloc_align(a, mem_request, DEFAULT_ALIGNMENT);
}


void arena_stack_debug_print(Stack_Allocator* a)
{
    if (!a) return;
    INFO("ARENA STACK CAPACITY: %llu", a->capacity);
    INFO("ARENA STACK MEMORY USED: %llu", a->current_offset);
    INFO("ARENA STACK MEMORY LEFT: %llu", a->capacity - a->current_offset);
}

void* stack_allocator_interface_alloc(void* allocator, u64 memory_byte_request, u8 alignment)
{
    Stack_Allocator* a = (Stack_Allocator*)allocator;
    return stack_allocator_alloc(a, memory_byte_request);
}

void stack_allocator_interface_free(void* allocator, void* memory_block)
{
    //NOTE: the reason we dont use this is because we have no way of knowing what actually was on top when we free, therefore free manually
    // Stack_Allocator* a = (Stack_Allocator*) allocator;
    // stack_allocator_pop(a, memory_block);
    return;
}


void stack_allocator_test(void)
{
    TEST_START("ARENA STACK");

    u64 mem_size = MB(1);
    uint8_t* backing_buffer = malloc(mem_size);

    Stack_Allocator a_s;
    stack_allocator_init(&a_s, backing_buffer, mem_size);
    TEST_DEBUG(a_s.capacity == MB(1));

    uint32_t* arr1 = stack_allocator_alloc(&a_s, 10 * sizeof(uint32_t));
    //56 due to alignment and header padding
    arr1[0] = 10;
    TEST_DEBUG(a_s.current_offset == 56);
    TEST_DEBUG(arr1[0] == 10);


    uint32_t* arr2 = stack_allocator_alloc(&a_s, 10 * sizeof(uint32_t));
    arr1[1] = 10;
    TEST_DEBUG(a_s.current_offset == 104);

    uint32_t* arr3 = stack_allocator_alloc(&a_s, 10 * sizeof(uint32_t));
    arr1[15] = 10;
    TEST_DEBUG(a_s.current_offset == 152);


    //pops all of them off the arena
    stack_allocator_pop(&a_s, arr3);
    TEST_DEBUG(a_s.current_offset == 104);
    stack_allocator_pop(&a_s, arr2);
    TEST_DEBUG(a_s.current_offset == 56);
    stack_allocator_pop(&a_s, arr1);
    TEST_DEBUG(a_s.current_offset == 0);

    TEST_REPORT("ARENA STACK");
}
