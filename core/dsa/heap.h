

#ifndef HEAP_H
#define HEAP_H
#include <stdlib.h>

typedef struct heap
{
    int* array;
    int size;
    int capacity;
} Heap;

Heap* heap_create(int capacity)
{
    Heap* heap = (Heap*)malloc(sizeof(Heap));
    heap->size = 0;
    heap->capacity = capacity;
    heap->array = (int*)malloc(sizeof(int) * capacity);
    return heap;
}


#endif //HEAP_H
