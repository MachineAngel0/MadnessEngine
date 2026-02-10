#ifndef STACK_H
#define STACK_H


#include <stdbool.h>
#include <stdlib.h>



typedef struct stack
{
    void* data; //pointer to where the data starts
    u64 num_items; // current/top index in our array
    u64 stride; // size/stride of each  data
    u64 capacity; // size of the array
} stack;

//TODO: test arena functions
stack* stack_create_malloc(const u64 stride, const u64 capacity);

stack* stack_create_arena(Arena* arena, const u64 stride, const u64 capacity);

//not worth it
// #define STACK_CREATE (arena, type, capacity) stack_create_arena(arena, sizeof(type), capacity)

//only use if allocated with malloc
void stack_free(stack* s);

void stack_clear(stack* s);

void stack_copy(stack* dest, const stack* src);

bool stack_is_empty(const stack* s);

bool stack_is_full(const stack* s);

void stack_resize(stack* stack, size_t new_capacity);

void stack_resize_arena(Arena* arena, stack* stack, const size_t new_capacity);

void stack_push(stack* s, const void* data);

void stack_push_arena(Arena* arena, stack* s, const void* data);


void stack_pop(stack* s);

// Peek element
void* stack_peek(stack* s);
// Peek element
void stack_copy_top(stack* s, void* out_data);
u64 stack_size(const stack* s);


void stack_print(const stack* stack, void (*print_func)(void*));


void stack_test();

#endif //STACK_H
