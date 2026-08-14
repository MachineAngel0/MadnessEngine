#ifndef LOCK_FREE_QUEUE_H
#define LOCK_FREE_QUEUE_H
#include "defines.h"


//circular queue / array based queue
typedef struct Lock_Free_Queue
{
    // an array of void*
    void* data;

    //you can also think of these as the head and tail
    s32 front;
    s32 back;

    //number of items in the queue
    u32 num_items;
    //size of the void* data
    u32 data_size;
    // total size of our queue, used to keep track of the memory size
    u32 capacity;


} Lock_Free_Queue;






#endif
