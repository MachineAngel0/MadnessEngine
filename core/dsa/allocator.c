#include "allocator.h"

#include "dsa_utility.h"
#include "unit_test.h"


void allocator_init(Allocator* a, void* backing_buffer, const u64 backing_buffer_size)
{
    MASSERT(a);
    MASSERT(backing_buffer);

    //we pass in an already allocated chunk of memory in the event,
    //we want to pass in an already allocated allocator memory, say a global allocator, and then one for audio for something similar
    a->memory = (u8*) backing_buffer;
    a->current_offset = 0;
    a->capacity = backing_buffer_size;


}


void allocator_clear(Allocator* a)
{
    a->current_offset = 0;
    // memset(a->memory, 0, a->capacity); //NOTE: this is apparently super fucking slow
}

//you can use align = 1, if you dont care about alignment, otherwise typically 4 or 8
void* allocator_alloc_align(Allocator* a, const u64 mem_request, const u64 align)
{
    //align the memory
    uintptr_t curr_ptr = (uintptr_t) a->memory + (uintptr_t) a->current_offset; // get current memory address
    uintptr_t offset = align_forward(curr_ptr, align); // offset needed to align memory
    offset -= (uintptr_t) a->memory; // if zero then memory was already aligned

    //see if we have space left
    if (offset + mem_request > a->capacity)
    {
        FATAL("ALLOCATOR OUT OF MEMORY");
        return NULL;
    }

    //get the requested memory
    void* ptr = &a->memory[offset]; // getting the start of the memory we are going to be returning
    a->current_offset = offset + mem_request;
    // Zero new memory by default
    memset(ptr, 0, mem_request); // already offset so no need to include it

    return ptr; // return the memory
}

// in the event we just want a default alignment
void* allocator_alloc(Allocator* a, const u64 mem_request)
{
    return allocator_alloc_align(a, mem_request, DEFAULT_ALIGNMENT);
}

u64 allocator_get_memory_left(const Allocator* a)
{
    return (a->capacity - a->current_offset);
}


void allocator_debug_print(Allocator* a)
{
    if (!a) return;
    INFO("ALLOCATOR CAPACITY: %llu", a->capacity);
    INFO("ALLOCATOR MEMORY USED: %llu", a->current_offset);
    INFO("ALLOCATOR MEMORY LEFT: %llu", a->capacity - a->current_offset);
}

void* allocator_interface_alloc(void* allocator, const u64 memory_byte_request, u8 alignment)
{
    Allocator* a = (Allocator*)allocator;
    return allocator_alloc(a, memory_byte_request);
}

void allocator_interface_free(void* allocator, void* memory_block)
{
    //should be handled manually and not by the interface
    return;
}


//typically this will be used in local scope, and freed off the stack when done,
//so its recommended that it's not a pointer
Allocator_Temp temp_allocator_memory_begin(Allocator* a)
{
    MASSERT_MSG(a, "TEMP ALLOCATOR INVALID ALLOCATOR PASSED IN");

    Allocator_Temp temp;
    temp.allocator = a;
    temp.prev_offset = a->current_offset;
    return temp;
}

void temp_allocator_memory_end(Allocator_Temp temp)
{
    temp.allocator->current_offset = temp.prev_offset;
}


void allocator_test(void)
{
    TEST_START("ALLOCATOR");
    Allocator a = {0};
    const u64 allocator_size = MB(1);
    void* mem = malloc(allocator_size);
    if (!mem) { MASSERT("ALLOCATOR ALLOC FAILED"); }
    allocator_init(&a, mem, allocator_size);
    TEST_DEBUG(a.capacity == MB(1));


    //we dont have to do anything with the arr, just make sure we dont touch it again

    int* arr1 = allocator_alloc(&a, 10 * sizeof(int));
    arr1[2] = 15;

    TEST_DEBUG(arr1[2] == 15);
    TEST_DEBUG(10 * sizeof(int) == a.current_offset);
    TEST_DEBUG(MB(1) == a.capacity);
    TEST_DEBUG(MB(1)-40 == a.capacity - a.current_offset);

    allocator_clear(&a);
    TEST_DEBUG(a.current_offset==0);

    int* arr2 = allocator_alloc(&a, 50 * sizeof(int));
    arr2[2] = 100;
    TEST_DEBUG(arr2[2] == 100);
    TEST_DEBUG((50 * sizeof(int)) == a.current_offset);
    TEST_DEBUG(MB(1) == a.capacity);
    TEST_DEBUG(MB(1) - (50 * sizeof(int)) == a.capacity - a.current_offset);

    allocator_clear(&a);
    TEST_DEBUG(a.current_offset==0);


    char* char_arr = allocator_alloc(&a, 100 * sizeof(char));
    TEST_DEBUG(a.current_offset == 100);
    u64* u64_arr = allocator_alloc(&a, 100 * sizeof(u64));
    //912 due to alignemnt
    TEST_DEBUG(a.current_offset == 912);
    s32* i32_arr = allocator_alloc(&a, 100 * sizeof(s32));
    TEST_DEBUG(a.current_offset == 1312);


    free(mem);
    mem = NULL;
    TEST_DEBUG(mem == NULL);


    TEST_REPORT("ALLOCATOR");
}


