#ifndef QUEUE_H
#define QUEUE_H


//circular queue / array based queue
//TODO: make it thread safe/ring buffer

typedef struct queue
{
    // an array of void*
    void** data;

    //you can also think of these as the head and tail
    size_t front;
    size_t back;

    // total size of our queue, used to keep track of the memory size
    size_t capacity;
    //size of the void* data
    size_t size;

    //NOTE: TO MYSELF: I had a num_entries here but apparently thats not thread safe

    //TODO: MAYBE
    //void (*print_func)(void*);
} queue;


queue* queue_create(size_t capacity, size_t data_size);

void queue_resize(queue* queue, size_t new_capacity);
void queue_free(queue* queue);

void queue_clear(queue* queue);
//empty and full prevents us from having our front and back indexes overlap, depending on the operation
bool queue_is_empty(queue* queue);

bool queue_is_full(queue* queue);

//adds to the back of the queue
void enqueue(queue* queue, void* data);

//pops(not technically a pop) from the front of the queue, also returns it
void* dequeue(queue* queue);

void* queue_peek(queue* queue);

int queue_size(queue* queue);

void queue_print(queue* queue, void (*print_func)(void*));

void queue_test();
#endif //QUEUE_H
