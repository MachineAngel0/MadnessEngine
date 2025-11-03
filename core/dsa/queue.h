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


queue* queue_create(size_t capacity, size_t data_size)
{
    queue* q = (queue *) malloc(sizeof(queue));
    q->data = malloc(sizeof(void *) * capacity);

    q->capacity = capacity;
    q->size = data_size;

    q->front = 0;
    q->back = 0;

    return q;
}

void queue_resize(queue* queue, size_t new_capacity)
{
    queue->data = realloc(queue->data, sizeof(void *) * new_capacity);
    queue->capacity = new_capacity;
}

void queue_free(queue* queue)
{
    free(queue->data);
    queue->data = NULL;
    free(queue);
    queue = NULL;
}

void queue_clear(queue* queue)
{
    queue->front = 0;
    queue->back = 0;
}

//empty and full prevents us from having our front and back indexes overlap, depending on the operation
bool queue_is_empty(queue* queue)
{
    return queue->front == queue->back;
}

bool queue_is_full(queue* queue)
{
    return (queue->back + 1) % queue->capacity == queue->front;
}

//adds to the back of the queue
void enqueue(queue* queue, void* data)
{
    if (queue_is_full(queue))
    {
        //TODO: REPLACE WITH A LOG
        printf("Queue is full\n");
        return;
    }

    queue->data[queue->back] = data;
    // if it ever reaches our exceeds our capacity, then it resets itself back to 0
    queue->back = (queue->back + 1) % queue->capacity;
}


//pops(not technically a pop) from the front of the queue, also returns it
void* dequeue(queue* queue)
{

    if (queue_is_empty(queue)) return NULL;

    void* return_data = queue->data[queue->front];
    // if it ever reaches our exceeds our capacity, then it resets itself back to 0
    queue->front = (queue->front + 1) % queue->capacity;

    return return_data;
}


void* queue_peek(queue* queue)
{
    if (queue_is_empty(queue)) return NULL;
    return queue->data[queue->front];
}

int queue_size(queue* queue)
{
    return (queue->back - queue->front + queue->capacity) % queue->capacity;
}

void queue_print(queue* queue, void (*print_func)(void*))
{
    size_t count = queue_size(queue);
    for (size_t i = 0, idx = queue->front; i < count; i++) {
        print_func(queue->data[idx]);
        idx = (idx + 1) % queue->capacity;
    }
    printf("\n");
}

void queue_test()
{
    printf("QUEUE START\n");

    queue* test_queue = queue_create(6, sizeof(int));
    int num10 = 10;
    int num20 = 20;
    int num30 = 30;
    int num40 = 40;
    int num50 = 50;
    int num60 = 60;

    printf("QUEUE ENQUEUE START\n");
    enqueue(test_queue, &num10);
    enqueue(test_queue, &num20);
    enqueue(test_queue, &num30);
    enqueue(test_queue, &num40);
    enqueue(test_queue, &num50);
    enqueue(test_queue, &num60); //this should fail
    printf("QUEUE ENQUEUE END\n\n");

    queue_print(test_queue, print_int);

    printf("QUEUE DEQUEUE START\n");
    dequeue(test_queue);
    dequeue(test_queue);
    dequeue(test_queue);
    queue_print(test_queue, print_int);
    printf("QUEUE DEQUEUE END\n\n");


    queue_free(test_queue);

    printf("QUEUE End\n\n");
}

#endif //QUEUE_H
