#ifndef WORK_STEAL_QUEUE_H
#define WORK_STEAL_QUEUE_H


#include "defines.h"
#include "thread_madness.h"


//circular queue / array based queue
typedef struct Work_Steal_Queue
{
    // an array of void*
    void* data;

    //you can also think of these as the head and tail
    Madness_Atomic_U32 top;
    Madness_Atomic_U32 bottom;

    //size of the void* data
    u32 data_size;
    // total size of our queue, used to keep track of the memory size
    u32 capacity;
} Work_Steal_Queue;


Work_Steal_Queue* work_steal_queue_init(u32 data_size, u32 capacity, Allocator* allocator)
{
    Work_Steal_Queue* queue = (Work_Steal_Queue*)allocator_alloc(allocator, sizeof(Work_Steal_Queue));
    queue->data = allocator_alloc(allocator, data_size * capacity);
    queue->data_size = data_size;
    queue->capacity = capacity;

    atomic_u32_init(&queue->top, 0, allocator);
    atomic_u32_init(&queue->bottom, 0, allocator);

    return queue;
}


void work_steal_queue_push(Work_Steal_Queue* queue, void* data)
{
    const u32 b = atomic_u32_load(&queue->bottom);
    const u32 t = atomic_u32_load(&queue->top);

    if (b - t >= queue->capacity)
    {
        // full, should never happen
        MASSERT_FALSE()
        return;
    }


    memcpy((u8*)queue->data + ((b % queue->capacity) * queue->data_size), data, queue->data_size);
    atomic_u32_fetch_add(&queue->bottom, 1);
}


bool work_steal_queue_steal(Work_Steal_Queue* queue, void* out_data)
{
    const u32 t = atomic_u32_load(&queue->top);
    const u32 b = atomic_u32_load(&queue->bottom);
    if (t < b)
    {
        // non-empty queue
        void* index_data = (u8*)queue->data + ((t % queue->capacity) * queue->data_size);
        memcpy(out_data, index_data, queue->data_size);

        if (atomic_u32_compare_and_swap(&queue->top, t + 1, t) != t)
        {
            // a concurrent steal or pop operation removed an element from the deque in the meantime.
            return false;
        }
        return true;
    }
    else
    {
        // empty queue
        return false;
    }
}

void* work_steal_queue_pop(Work_Steal_Queue* queue)
{
    u32 b = atomic_u32_load(&queue->bottom);
    b = b - 1;
    atomic_u32_store(&queue->bottom, b);

    long t = atomic_u32_load(&queue->top);
    if (t <= b)
    {
        // non-empty queue
        void* out_data = (u8*)queue->data + (b * queue->data_size);
        if (t != b)
        {
            // there's still more than one item left in the queue
            return out_data;
        }

        // this is the last item in the queue
        if (atomic_u32_compare_and_swap(&queue->top, t + 1, t) != t)
        {
            // failed race against steal operation
            out_data = NULL;
        }

        atomic_u32_store(&queue->bottom, t + 1);
        return out_data;
    }
    else
    {
        // Queue was already empty -- restore bottom.
        atomic_u32_store(&queue->bottom, t);
        return false;
    }
}


#endif
