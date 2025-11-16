#ifndef STACK_H
#define STACK_H
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


typedef struct stack
{
    void* data; //pointer to where the data starts
    u64 num_items; // current/top index in our array
    u64 stride; // size/stride of each  data
    u64 capacity; // size of the array
} stack;


stack* stack_create(const u64 stride, const u64 capacity)
{
    stack* s = malloc(sizeof(stack));
    memset(s, 0, sizeof(stack));
    s->data = malloc(stride * capacity);
    memset(s->data, 0, stride * capacity);

    s->stride = stride;
    s->capacity = capacity;
    s->num_items = 0;

    return s;
}


void stack_free(stack* s)
{
    if (!s)
    {
        WARN("STACK FREE: TRYING TO FREE AN INVALID STACK")
        return;
    }
    free(s->data);
    free(s);
}

void stack_clear(stack* s)
{
    if (!s)
    {
        WARN("STACK CLEAR: TRYING TO USE AN INVALID STACK")
        return;
    }
    s->num_items = 0;
}

void stack_copy(stack* dest, const stack* src)
{
    memcpy(dest, src, sizeof(stack));
}

bool stack_is_empty(const stack* s)
{
    return s->num_items == 0;
}

bool stack_is_full(const stack* s)
{
    return s->num_items >= s->capacity;
}

void stack_resize(stack* stack, size_t new_capacity)
{
    realloc(stack, new_capacity);
}

void stack_push(stack* s, void* data)
{
    if (stack_is_full(s))
    {
        WARN("STACK PUSH: Trying to push into a full stack")
        return;
    }

    // get the start location, then memcpy it
    uint8_t* dest = (uint8_t *) s->data + (s->stride * (s->num_items));
    memcpy(dest, data, s->stride);


    s->num_items++;
}

void stack_pop(stack* s)
{
    if (stack_is_empty(s))
    {
        WARN("STACK POP: Trying to Pop on an empty stack")
        return;
    }
    s->num_items--;
}

// Peek element
void* stack_peek(stack* s)
{
    if (!s) return NULL;
    if (stack_is_empty(s)) return NULL;
    // return &s->data[s->stride * s->num_items];
    return (uint8_t *) s->data + (s->stride * (s->num_items-1));
}

// Peek element
void stack_copy_top(stack* s, void* dest_data)
{
    if (!s) return;
    if (stack_is_empty(s)) return;

    memcpy(dest_data,
           ((uint8_t*)s->data + (s->stride * (s->num_items-1))),
           s->stride);
}

int stack_size(stack* s)
{
    return s->num_items;
}

void stack_print(stack* stack, void (*print_func)(void*))
{
    for (int i = 0; i < stack->num_items; i++)
    {
        print_func(&stack->data[i * stack->stride]);
    }
    printf("\n"); // number of bytes
}


void stack_test()
{
    printf("Stack Start\n"); // number of bytes

    int num = 3;
    int stack_default_capacity = 3;
    stack* stack = stack_create(sizeof(num), stack_default_capacity);

    //stack_is_empty(stack);

    int num10 = 10;
    int num15 = 15;
    int num20 = 20;

    stack_push(stack, &num);
    stack_push(stack, &num10);
    stack_push(stack, &num15);
    stack_push(stack, &num20);
    stack_print(stack, print_int);
    printf("PEEK: %d\n", *(int *) stack_peek(stack));

    stack_pop(stack);

    stack_print(stack, print_int);

    stack_pop(stack);
    stack_pop(stack);
    stack_pop(stack);
    stack_pop(stack);
    stack_print(stack, print_int);


    stack_free(stack);


    printf("Stack End\n\n"); // number of bytes
}

#endif //STACK_H
