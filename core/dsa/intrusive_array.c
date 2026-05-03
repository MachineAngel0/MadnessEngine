#include "intrusive_array.h"
#include <string.h>.

#include "asserts.h"


Intrusive_Array* _intrusive_array_create(u64 data_stride, u64 capacity, Allocator_Interface* allocator)
{
    Intrusive_Array* arr;
    if (allocator)
    {
        arr = allocator->alloc(allocator->allocator, sizeof(Intrusive_Array), DEFAULT_ALIGNMENT);
        arr->data = allocator->alloc(allocator->allocator, data_stride * capacity, DEFAULT_ALIGNMENT);
    }
    else
    {
        //alloc and zero
        arr = (Intrusive_Array*)malloc(sizeof(Intrusive_Array));
        memset(arr, 0, sizeof(Intrusive_Array));
        arr->data = malloc(capacity * data_stride);
        memset(arr->data, 0, capacity * data_stride);
    }

    arr->num_items = 0;
    arr->stride = data_stride;
    arr->capacity = capacity;

    return arr;
}


void intrusive_array_free(Intrusive_Array* intrusive_array)
{
    if (intrusive_array->allocator)
    {
        intrusive_array->allocator->free(intrusive_array->allocator->allocator, intrusive_array->data);
        intrusive_array->allocator->free(intrusive_array->allocator->allocator, intrusive_array);
        return;
    }

    //alloc and zero
    free(intrusive_array->data);
    free(intrusive_array);
}

void intrusive_array_clear(Intrusive_Array* intrusive_array)
{
    intrusive_array->num_items = 0;
}

bool intrusive_array_is_empty(const Intrusive_Array* intrusive_array)
{
    return intrusive_array->num_items == 0;
}

bool intrusive_array_is_full(const Intrusive_Array* intrusive_array)
{
    return intrusive_array->num_items == intrusive_array->capacity;
}

void intrusive_array_push(Intrusive_Array* intrusive_array, const void* data_ref)
{
    MASSERT(intrusive_array);
    memcpy(((u8*)intrusive_array->data) + (intrusive_array->stride * intrusive_array->num_items), data_ref,
           intrusive_array->stride);
    intrusive_array->num_items++;
}

void intrusive_array_pop(Intrusive_Array* intrusive_array)
{
    intrusive_array->num_items--;

}

void intrusive_array_test()
{
}
