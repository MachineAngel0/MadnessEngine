#include "ring_queue.h"

ring_queue* ring_queue_create(u32 capacity, u32 data_size)
{
    return NULL;
}

void ring_queue_resize(ring_queue* queue, u32 new_capacity)
{
}

void ring_queue_free(ring_queue* queue)
{
}

void ring_queue_clear(ring_queue* queue)
{
}

bool ring_queue_is_empty(ring_queue* queue)
{
    return true;
}

bool ring_queue_is_full(ring_queue* queue)
{
    return true;
}

void ring_enqueue(ring_queue* queue, void* data)
{
}

void* ring_dequeue(ring_queue* queue)
{
    return NULL;
}

void* ring_queue_peek(ring_queue* queue)
{
    return NULL;
}

int ring_queue_size(ring_queue* queue)
{
    return 0;
}

void ring_queue_print(ring_queue* queue, void (*print_func)(void*))
{
}

void ring_queue_test()
{
}
