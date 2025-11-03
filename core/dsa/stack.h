#ifndef STACK_H
#define STACK_H
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


typedef struct stack
{
    void** data; // an array of void*

    size_t capacity; // total size of our stack, used to keep track of the array size
    size_t size; //size of the void* data

    //has to be signed since it can be -1
    ssize_t num_items; // the number of items in the stack


} stack;


stack* stack_create(size_t capacity, const size_t data_size)
{
    // create the struct memory
    stack* new_stack = (stack *) malloc(sizeof(stack));

    new_stack->data = malloc(sizeof(void *) * capacity);

    new_stack->capacity = capacity;
    // it's pretty normal to want to access the zero element, which is why we do this
    new_stack->num_items = -1;
    new_stack->size = data_size;

    return new_stack;
}

void stack_free(stack* stack)
{
    if (stack == NULL)
    {
        WARN("TRYING TO DEALLOCATE A NULL STACK")
        return;
    }
    free(stack->data);
    free(stack);
}

void stack_clear(stack* stack)
{
    //nothing to deallocate just setting reset the count
    stack->num_items = -1;
}

void stack_copy(stack* dest, stack* src)
{
    memcpy(dest, src, sizeof(stack));
}

bool stack_is_empty(stack* stack)
{
    if (stack->num_items >= -1)
    {
        return false;
    }

    return true;
}

bool stack_is_full(stack* stack)
{
    if (stack->num_items >= stack->capacity)
    {
        printf("Stack is full\n");
        return true;
    }
    return false;
}

void stack_resize(stack* stack, size_t new_capacity)
{
    stack->data = realloc(stack->data, sizeof(void *) * new_capacity);
    stack->capacity = new_capacity;
}

void stack_push(stack* stack, void* data)
{
    if (stack_is_full(stack))
    {
        stack_resize(stack, stack->capacity * 2);
    };

    stack->num_items++;
    stack->data[stack->num_items] = data;
}

void stack_pop(stack* stack)
{
    //we don't need to remove anything just decrement the size as long as it's not empty
    if (stack_is_empty(stack)) return;

    stack->num_items--;
}

// Peek element
void* stack_peek(stack* stack)
{
    if (stack_is_empty(stack)) return NULL;
    return stack->data[stack->num_items];
    //return *(stack->data + stack->count); // the same thing as above and this makes a lot of sense and is really cool
}

// Peek element
void stack_copy_top(stack* stack, void* dest_data)
{
    if (stack_is_empty(stack))
    {
        WARN("STACK IS EMPTY, COPY TOP FAILED")
        return;
    }
    memcpy(dest_data,  stack->data[stack->num_items], stack->size);
    //return *(stack->data + stack->count); // the same thing as above and this makes a lot of sense and is really cool
}

int stack_size(stack* stack)
{
    return stack->num_items;
}

void stack_print(stack* stack, void (*print_func)(void*))
{

    for (int i = 0; i < stack->num_items+1; i++)
    {
        print_func(stack->data[i]);
    }
    printf("\n"); // number of bytes
}


void stack_test()
{
    printf("Stack Start\n"); // number of bytes

    int num = 3;
    int stack_default_capacity = 3;
    stack* stack = stack_create(stack_default_capacity, sizeof(num));

    //stack_is_empty(stack);

    int num10 = 10;
    int num15 = 15;
    int num20 = 20;

    stack_push(stack, &num);
    stack_push(stack, &num10);
    stack_push(stack, &num15);
    stack_push(stack, &num20);
    stack_print(stack, print_int);
    printf("PEEK: %d\n", *(int*)stack_peek(stack));

    stack_pop(stack);

    stack_print(stack, print_int);


    stack_free(stack);


    printf("Stack End\n\n"); // number of bytes
}

#endif //STACK_H
