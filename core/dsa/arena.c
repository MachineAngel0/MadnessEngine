#include "arena.h"

#include "dsa_utility.h"
#include "unit_test.h"


void arena_init(Arena* a, void* backing_buffer, const u64 backing_buffer_size, Memory_Tracker* memory_tracker)
{
    MASSERT(a);
    MASSERT(backing_buffer);

    //we pass in an already allocated chunk of memory in the event,
    //we want to pass in an already allocated arena memory, say a global arena, and then one for audio for something similar
    a->memory = (u8*) backing_buffer;
    a->current_offset = 0;
    a->capacity = backing_buffer_size;

    if (memory_tracker)
    {
        a->memory_tracker = memory_tracker;
    }
}


void arena_clear(Arena* a)
{
    if (a->memory_tracker)
    {
        memory_tracker_free_allocation(a->memory_tracker, MEMORY_CONTAINER_ARENA, a->current_offset);
    }

    a->current_offset = 0;
    // memset(a->memory, 0, a->capacity); //NOTE: this is apparently super fucking slow
}

//you can use align = 1, if you dont care about alignment, otherwise typically 4 or 8
void* arena_alloc_align(Arena* a, const u64 mem_request, const u64 align)
{
    //align the memory
    uintptr_t curr_ptr = (uintptr_t) a->memory + (uintptr_t) a->current_offset; // get current memory address
    uintptr_t offset = align_forward(curr_ptr, align); // offset needed to align memory
    offset -= (uintptr_t) a->memory; // if zero then memory was already aligned

    //see if we have space left
    if (offset + mem_request > a->capacity)
    {
        FATAL("ARENA OUT OF MEMORY");
        return NULL;
    }

    //get the requested memory
    void* ptr = &a->memory[offset]; // getting the start of the memory we are going to be returning
    a->current_offset = offset + mem_request;
    // Zero new memory by default
    memset(ptr, 0, mem_request); // already offset so no need to include it

    if (a->memory_tracker)
    {
        memory_tracker_track_allocation(a->memory_tracker, MEMORY_CONTAINER_ARENA, mem_request);
    }

    return ptr; // return the memory
}

// in the event we just want a default alignment
void* arena_alloc(Arena* a, const u64 mem_request)
{
    return arena_alloc_align(a, mem_request, DEFAULT_ALIGNMENT);
}

u64 arena_get_memory_left(const Arena* a)
{
    return (a->capacity - a->current_offset);
}


void arena_debug_print(Arena* a)
{
    if (!a) return;
    INFO("ARENA CAPACITY: %llu", a->capacity);
    INFO("ARENA MEMORY USED: %llu", a->current_offset);
    INFO("ARENA MEMORY LEFT: %llu", a->capacity - a->current_offset);
}


//typically this will be used in local scope, and freed off the stack when done,
//so its recommended that it's not a pointer
Arena_Temp temp_arena_memory_begin(Arena* a)
{
    MASSERT_MSG(a, "TEMP ARENA INVALID ARENA PASSED IN");

    Arena_Temp temp;
    temp.arena = a;
    temp.prev_offset = a->current_offset;
    return temp;
}

void temp_arena_memory_end(Arena_Temp temp)
{
    temp.arena->current_offset = temp.prev_offset;
}


void arena_test(void)
{
    TEST_START("ARENA");
    Arena a = {0};
    const u64 arena_size = MB(1);
    void* mem = malloc(arena_size);
    if (!mem) { MASSERT("ARENA ALLOC FAILED"); }
    arena_init(&a, mem, arena_size, NULL);
    TEST_DEBUG(a.capacity == MB(1));


    //we dont have to do anything with the arr, just make sure we dont touch it again

    int* arr1 = arena_alloc(&a, 10 * sizeof(int));
    arr1[2] = 15;

    TEST_DEBUG(arr1[2] == 15);
    TEST_DEBUG(10 * sizeof(int) == a.current_offset);
    TEST_DEBUG(MB(1) == a.capacity);
    TEST_DEBUG(MB(1)-40 == a.capacity - a.current_offset);

    arena_clear(&a);
    TEST_DEBUG(a.current_offset==0);

    int* arr2 = arena_alloc(&a, 50 * sizeof(int));
    arr2[2] = 100;
    TEST_DEBUG(arr2[2] == 100);
    TEST_DEBUG((50 * sizeof(int)) == a.current_offset);
    TEST_DEBUG(MB(1) == a.capacity);
    TEST_DEBUG(MB(1) - (50 * sizeof(int)) == a.capacity - a.current_offset);

    arena_clear(&a);
    TEST_DEBUG(a.current_offset==0);


    char* char_arr = arena_alloc(&a, 100 * sizeof(char));
    TEST_DEBUG(a.current_offset == 100);
    u64* u64_arr = arena_alloc(&a, 100 * sizeof(u64));
    //912 due to alignemnt
    TEST_DEBUG(a.current_offset == 912);
    i32* i32_arr = arena_alloc(&a, 100 * sizeof(i32));
    TEST_DEBUG(a.current_offset == 1312);


    free(mem);
    mem = NULL;
    TEST_DEBUG(mem == NULL);


    TEST_REPORT("ARENA");
}


