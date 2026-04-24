#ifndef RING_QUEUE_H
#define RING_QUEUE_H


//circular queue / array based queue
typedef struct ring_queue
{
    // an array of void*
    void* data;

    //you can also think of these as the head and tail
    i32 front;
    i32 back;

    //number of items in the queue
    u32 num_items;
    //size of the void* data
    u32 data_size;
    // total size of our queue, used to keep track of the memory size
    u32 capacity;

    bool is_ptr_type;

} ring_queue;


ring_queue* ring_queue_create(u32 data_size, u32 capacity);

void ring_queue_free(ring_queue* queue);

void ring_queue_clear(ring_queue* queue);

bool ring_queue_is_empty(ring_queue* queue);

bool ring_queue_is_full(ring_queue* queue);

//adds to the back of the queue
bool ring_enqueue(ring_queue* q, void* data);

//pops(not technically a pop) from the front of the queue, also returns it
bool ring_dequeue(ring_queue* q, void* out_data);

bool ring_queue_peek(const ring_queue* q, void* out_data);

//
void ring_queue_empty(ring_queue* q);

void ring_queue_print(ring_queue* queue, void (*print_func)(void*));

void ring_queue_test();


#endif //RING_QUEUE_H
