#ifndef ARENA_STACK_H
#define ARENA_STACK_H

#ifndef DEFAULT_ALIGNMENT
//most likely to be 4 (32bit) or 8 (64bit) (* 2)
#define DEFAULT_ALIGNMENT (2*sizeof(void *))
#endif


// how this works Basically
// | Memory |
// Alloc -> |Header|Data_Size| Memory
//                 |Cur
//
// Alloc -> |Header|Data_Size| Header|Data_Size| Memory
//                                   |Cur
//
// Pop -> |Header|Data_Size| Memory
//               |Cur
// users only get access to the data portion


//as drawn above this gets stored in the memory, not the arena struct
typedef struct Arena_Stack_Header
{
    uint8_t prev_header;
    uint8_t data_size;
} Arena_Stack_Header;

//TODO: it would be cool to have some sort of id, with each allocation passed forward and back,
// so that we enforce our lifo allocation, but then that would have to be reinforced by something,
//  which might be more trouble than worth
typedef struct Arena_Stack
{
    uint8_t* memory;
    size_t current_offset; // where in our memory we are
    size_t capacity; // how large our arena is
} Arena_Stack;


//used for arenas who get memory from other arenas
void arena_stack_init(Arena_Stack* a, void* mem, const size_t mem_size);

void arena_stack_clear(Arena_Stack* a);

void arena_stack_pop(Arena_Stack* a, void* ptr);

size_t calc_padding_with_header(uintptr_t ptr, uintptr_t alignment, size_t header_size);

//you can use align = 1, if you dont care about alignment, otherwise typically 4 or 8
void* arena_stack_alloc_align(Arena_Stack* a, const size_t mem_request, size_t alignment);

// in the event we just want a default alignment
void* arena_stack_alloc(Arena_Stack* a, size_t mem_request);

void arena_stack_debug_print(Arena_Stack* a);

void arena_stack_test(void);

#endif //ARENA_STACK_H
