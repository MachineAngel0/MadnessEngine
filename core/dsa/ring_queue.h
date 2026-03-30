#ifndef RING_QUEUE_H
#define RING_QUEUE_H


//circular queue / array based queue
typedef struct ring_queue
{
    // an array of void*
    void* data;

    //you can also think of these as the head and tail
    u32 front;
    u32 back;

    // total size of our queue, used to keep track of the memory size
    u32 capacity;
    //size of the void* data
    u32 size;

} ring_queue;


ring_queue* ring_queue_create(u32 capacity, u32 data_size);

void ring_queue_resize(ring_queue* queue, u32 new_capacity);
void ring_queue_free(ring_queue* queue);

void ring_queue_clear(ring_queue* queue);
bool ring_queue_is_empty(ring_queue* queue);

bool ring_queue_is_full(ring_queue* queue);

//adds to the back of the queue
void ring_enqueue(ring_queue* queue, void* data);

//pops(not technically a pop) from the front of the queue, also returns it
void* ring_dequeue(ring_queue* queue);

void* ring_queue_peek(ring_queue* queue);

int ring_queue_size(ring_queue* queue);

void ring_queue_print(ring_queue* queue, void (*print_func)(void*));

void ring_queue_test();


#endif //RING_QUEUE_H
