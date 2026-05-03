#ifndef INTRUSIVE_ARRAY_H
#define INTRUSIVE_ARRAY_H


#include "defines.h"
#include "_allocator_interface.h"

//fixed sized array, so no reallocating more space
//intrusive means this does not own any of the memory passed to it, meaning it stores pointers to types
//You have to use a pointer type or this won't hold the data
typedef struct Array_Intrusive
{
    u64 capacity; // size of the array
    u32 stride; // size/stride of each void* data
    u32 num_items; // current/top index in our array
    void* data; //array of void* data

    Allocator_Interface* allocator;
} Intrusive_Array;


Intrusive_Array* intrusive_array_create(u64 data_stride, u64 capacity);

#define intrusive_array_create(type, capacity)\
    _array_create(sizeof(type), capacity)

void intrusive_array_free(Intrusive_Array* intrusive_array);
void intrusive_array_clear(Intrusive_Array* intrusive_array);

bool intrusive_array_is_empty(const Intrusive_Array* intrusive_array);
bool intrusive_array_is_full(const Intrusive_Array* intrusive_array);
bool intrusive_array_valid_index(const Intrusive_Array* intrusive_array, u64 index);

void* intrusive_array_get_ptr(Intrusive_Array* intrusive_array, u64 index);
void intrusive_array_set_ptr(Intrusive_Array* intrusive_array, const void* data, u64 pos);

void intrusive_array_push(Intrusive_Array* intrusive_array, const void* data_ref);
void intrusive_array_pop(Intrusive_Array* intrusive_array);


//TODO:
void intrusive_array_test();



#endif //INTRUSIVE_ARRAY_H
