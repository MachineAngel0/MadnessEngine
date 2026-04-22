#include "ring_queue.h"

ring_queue* ring_queue_create(u32 data_size, u32 capacity)
{
    ring_queue* queue = (ring_queue*)malloc(sizeof(ring_queue));
    queue->data = malloc(queue->data_size * capacity);
    queue->data_size = data_size;
    queue->capacity = capacity;

    queue->num_items = 0;

    queue->front = 0;
    queue->back = -1;


    return queue;
}

void ring_queue_free(ring_queue* queue)
{
    MASSERT(queue);
    //TODO: fix this up
    free(queue->data);
    free(queue);
}


void ring_queue_clear(ring_queue* queue)
{
    MASSERT(queue);
    queue->num_items = 0;
    queue->front = 0;
    queue->back = -1;
}

bool ring_queue_is_empty(ring_queue* queue)
{
    MASSERT(queue);
    return queue->num_items == 0;
}

bool ring_queue_is_full(ring_queue* queue)
{
    MASSERT(queue);
    return queue->num_items >= queue->capacity;
}

bool ring_enqueue(ring_queue* q, void* data)
{
    MASSERT(q && data);

    if (q->num_items == q->capacity)
    {
        WARN("RING ENQUEUE: QUEUE FULL COULD NOT ADD ITEM")
        return false;
    }

    q->back = (q->back + 1) % q->capacity;
    memcpy((u8*)q->data + (q->back * q->data_size), data, q->data_size);
    q->num_items++;

    return true;
}



bool ring_dequeue(ring_queue* q, void* out_data)
{
    MASSERT(q);

    if (q->num_items == 0)
    {
        WARN("RING DEQUEUE: NOTHING TO DEQUEUE")
        return false;
    }

    memcpy(out_data, (u8*)q->data + (q->front * q->data_size), q->data_size);
    q->front = (q->front + 1) % q->capacity;
    q->num_items--;


    return true;
}

bool ring_queue_peek(const ring_queue* q, void* out_data)
{
    MASSERT(q);

    if (q->num_items == 0)
    {
        WARN("RING DEQUEUE: NOTHING TO DEQUEUE")
        return false;
    }

    memcpy((u8*)q->data + (q->front * q->data_size), out_data, q->data_size);

    return true;
}

int ring_queue_size(ring_queue* queue)
{
    MASSERT(queue);
    return queue->num_items;
}

void ring_queue_print(ring_queue* queue, void (*print_func)(void*))
{
    MASSERT(queue);
    //TODO:
}

void ring_queue_test()
{
    TEST_START("RING QUEUE");

    int queue_capacity = 10;
    ring_queue* q = ring_queue_create(sizeof(int), queue_capacity);

    int default_val = 10;
    for (int i = 0; i < queue_capacity; i++)
    {
        ring_enqueue(q, &default_val);
    }
    TEST_DEBUG(ring_queue_is_full(q));
    TEST_DEBUG(!ring_enqueue(q, &default_val)); //should log a warn

    int dequeue_val;
    for (int i = 0; i < queue_capacity; i++)
    {
        ring_dequeue(q, &dequeue_val);
        TEST_DEBUG(dequeue_val == default_val);
    }
    TEST_DEBUG(ring_queue_is_empty(q));
    TEST_DEBUG(!ring_dequeue(q, &dequeue_val)); //should log a warn


    TEST_END("RING QUEUE");
}
