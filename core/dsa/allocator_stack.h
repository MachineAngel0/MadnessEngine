#ifndef ARENA_STACK_H
#define ARENA_STACK_H


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
typedef struct Stack_Allocator_Header
{
    uint8_t prev_header;
    uint8_t data_size;
} Stack_Allocator_Header;

//TODO: it would be cool to have some sort of id, with each allocation passed forward and back,
// so that we enforce our lifo allocation, but then that would have to be reinforced by something,
//  which might be more trouble than worth
typedef struct Stack_Allocator
{
    uint8_t* memory;
    size_t current_offset; // where in our memory we are
    size_t capacity; // how large our arena is
} Stack_Allocator;


//used for arenas who get memory from other arenas
void stack_allocator_init(Stack_Allocator* a, void* mem, const size_t mem_size);

// in the event we just want a default alignment
void* stack_allocator_alloc(Stack_Allocator* a, size_t mem_request);

void stack_allocator_clear(Stack_Allocator* a);

void stack_allocator_pop(Stack_Allocator* a, void* ptr);

size_t calc_padding_with_header(uintptr_t ptr, uintptr_t alignment, size_t header_size);

//you can use align = 1, if you dont care about alignment, otherwise typically 4 or 8
void* stack_allocator_alloc_align(Stack_Allocator* a, const size_t mem_request, size_t alignment);

void arena_stack_debug_print(Stack_Allocator* a);

//INTERFACE
MAPI void* stack_allocator_interface_alloc(void* allocator,  u64 memory_byte_request, u8 alignment);
MAPI void stack_allocator_interface_free(void* allocator, void* memory_block);



void stack_allocator_test(void);

#endif //ARENA_STACK_H
