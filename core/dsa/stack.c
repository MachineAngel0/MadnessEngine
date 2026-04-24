#include "stack.h"

//TODO: test arena functions
stack* stack_create_malloc(const u64 stride, const u64 capacity)
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

stack* stack_create_arena(Arena* arena, const u64 stride, const u64 capacity)
{
    stack* s = arena_alloc(arena, sizeof(stack));
    s->data = arena_alloc(arena, stride * capacity);

    s->stride = stride;
    s->capacity = capacity;
    s->num_items = 0;

    return s;
}

//not worth it
// #define STACK_CREATE (arena, type, capacity) stack_create_arena(arena, sizeof(type), capacity)

//only use if allocated with malloc
void stack_free(stack* s)
{
    MASSERT(s);
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
    MASSERT(s);
    s->num_items = 0;
}

void stack_copy(stack* dest, const stack* src)
{
    MASSERT(dest);
    MASSERT(src);
    memcpy(dest, src, sizeof(stack));
}

bool stack_is_empty(const stack* s)
{
    MASSERT(s);
    return s->num_items == 0;
}

bool stack_is_full(const stack* s)
{
    MASSERT(s);
    return s->num_items >= s->capacity;
}

void stack_push(stack* s, const void* data)
{
    MASSERT(s);
    if (stack_is_full(s))
    {
        WARN("STACK PUSH: Trying to push into a full stack")
        return;
    }

    // get the start location, then memcpy it
    uint8_t* dest = (uint8_t*)s->data + (s->stride * (s->num_items));
    memcpy(dest, data, s->stride);

    s->num_items++;
}

void stack_pop(stack* s)
{
    MASSERT(s);
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
    MASSERT(s);
    if (stack_is_empty(s)) return NULL;
    // return &s->data[s->stride * s->num_items];
    return (uint8_t*)s->data + (s->stride * (s->num_items - 1));
}

// Peek element
void stack_copy_top(stack* s, void* out_data)
{
    MASSERT(s);
    if (stack_is_empty(s)) return;

    memcpy(out_data,
           ((uint8_t*)s->data + (s->stride * (s->num_items - 1))),
           s->stride);
}

u64 stack_size(const stack* s)
{
    MASSERT(s);
    return s->num_items;
}

void stack_empty(stack* s)
{
    MASSERT(s);
    s->num_items = 0;
}

void stack_print(const stack* s, void (*print_func)(void*))
{
    MASSERT(s);
    for (int i = 0; i < s->num_items; i++)
    {
        print_func((void*)((u8*)s->data + (i * s->stride)));
    }
    printf("\n"); // number of bytes
}


void stack_test()
{
    TEST_START("STACK");
    printf("Stack Start\n"); // number of bytes

    int num = 3;
    int stack_default_capacity = 3;
    stack* stack = stack_create_malloc(sizeof(num), stack_default_capacity);

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

    stack_pop(stack);
    stack_pop(stack);
    stack_pop(stack);
    stack_pop(stack);
    stack_print(stack, print_int);


    stack_free(stack);


    printf("Stack End\n\n"); // number of bytes
    TEST_REPORT("STACK");
}
