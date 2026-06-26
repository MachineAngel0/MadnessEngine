#include "array_freelist.h"

Freelist_Array* _freelist_array_create(u64 data_stride, u64 capacity, Allocator* allocator)
{
    MASSERT(allocator);

    Freelist_Array* fl_array = allocator_alloc(allocator, sizeof(Freelist_Array));

    fl_array->array = _array_create(data_stride, capacity, allocator);

    fl_array->handle = allocator_alloc(allocator, sizeof(u32) * capacity);
    fl_array->gen = allocator_alloc(allocator, sizeof(u32) * capacity);

    fl_array->free_list = allocator_alloc(allocator, sizeof(u32) * capacity);
    fl_array->count = capacity;

    MASSERT(fl_array);
    MASSERT(fl_array->array);

    for (u32 i = 0; i < capacity; i++)
    {
        fl_array->handle[i] = i;
        fl_array->gen[i] = 0;

        fl_array->free_list[i] = i;
    }


    return fl_array;
}

void fl_array_destroy(Freelist_Array* array)
{
    //Nothing for now
}

void* _fl_array_internal_get(Freelist_Array* array, u32 index)
{
    return _array_get(array->array, index);
}

Freelist_Array_Handle fl_array_new(Freelist_Array* array)
{
    u32 index = array->free_list[array->count-1];
    array->count--;
    array->array->num_items++;
    return (Freelist_Array_Handle){array->handle[index], array->gen[index]};
}

void* _fl_array_query(Freelist_Array* array, Freelist_Array_Handle out_handle)
{
    if (array->gen[out_handle.handle] == out_handle.gen)
    {
        return _fl_array_internal_get(array, out_handle.handle);
    }
    DEBUG("INVALID GENERATION")
    return _fl_array_internal_get(array, 0);
}

void fl_array_release(Freelist_Array* array, const Freelist_Array_Handle handle)
{
    array->free_list[array->count] = handle.handle;
    array->count++;
    array->array->num_items--;
    array->gen[handle.handle]++;
}

void free_list_array_test()
{

    typedef struct testing_freelist_array_thing
    {
        u32 thing;
        bool modified;
    }testing_freelist_array_thing;

    Allocator allocator;
    u64 mem_size = 10000;
    allocator_init(&allocator, malloc(mem_size), mem_size);


    Freelist_Array* fl_array = freelist_array_create(testing_freelist_array_thing, 100, &allocator);


    Freelist_Array_Handle handle = fl_array_new(fl_array);

    testing_freelist_array_thing* thing = &fl_array_query(fl_array, testing_freelist_array_thing, handle);
    thing->modified = true;
    thing->thing = 654;

    Freelist_Array_Handle handle2 = fl_array_new(fl_array);
    Freelist_Array_Handle thing3 = fl_array_new(fl_array);
    Freelist_Array_Handle thing4 = fl_array_new(fl_array);
    Freelist_Array_Handle thing5 = fl_array_new(fl_array);


    fl_array_release(fl_array, handle2);
    Freelist_Array_Handle handle22 = fl_array_new(fl_array);
    testing_freelist_array_thing* thing2_fake = &fl_array_query(fl_array, testing_freelist_array_thing, handle2);
    testing_freelist_array_thing* thing2 = &fl_array_query(fl_array, testing_freelist_array_thing, handle22);




}
