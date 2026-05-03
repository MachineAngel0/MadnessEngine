#include "darray.h"

#include "allocator_freelist.h"
#include "logger.h"
#include "memory/memory_tracker.h"

typedef struct u8_internal_darray
{
    u64 capacity;
    u64 stride;
    u64 num_items;
    u8* data;
    Arena_Free_List* free_list;
} u8_internal_darray;


u8_internal_darray* _u8_internal_darray_create(u64 capacity, Arena_Free_List* free_list)
{
    MASSERT_MSG(capacity > 0, "DARRAY MACRO CREATE: INVALID SIZE");

    u8_internal_darray* arr;
    if (free_list)
    {
        arr = arena_free_list_alloc(free_list, sizeof(u8_internal_darray));
        arr->data = arena_free_list_alloc(free_list, sizeof(u8) * capacity);
        arr->free_list = free_list;
    }
    else
    {
        arr = malloc(sizeof(u8_internal_darray));
        memset(arr, 0, sizeof(u8_internal_darray));
        arr->data = malloc(sizeof(u8) * capacity);
        memset(arr->data, 0, sizeof(u8) * capacity);
    }

    arr->capacity = capacity;
    arr->stride = sizeof(u8);
    arr->num_items = 0;
    return arr;
}

void _u8_internal_darray_free(u8_internal_darray* array)
{
    MASSERT_MSG(array, "DARRAY MACRO FREE: NULL ARRAY")
    if (array->free_list)
    {
        arena_free_list_free(array->free_list, array->data);
        arena_free_list_free(array->free_list, array);
        return;
    }
    free(array->data);
    free(array);
}

void _u8_internal_darray_realloc(u8_internal_darray* array, u64 new_capacity)
{
    MASSERT_MSG(array, "DARRAY MACRO PUSH: NULL ARRAY");

    if (array->free_list)
    {
        void* new_data = arena_free_list_alloc(array->free_list, new_capacity * array->stride);
        memcpy(new_data, array->data, array->stride * array->num_items);
        arena_free_list_free(array->free_list, array->data);
        array->data = new_data;
        return;
    }
    void* new_data = malloc(new_capacity * array->stride);
    memcpy(new_data, array->data, array->stride * array->num_items);
    free(array->data);
    array->data = new_data;
}

void _u8_internal_darray_push(u8_internal_darray* array, const u8* data)
{
    MASSERT_MSG(array, "DARRAY MACRO PUSH: NULL ARRAY");

    if (array->num_items >= array->capacity)
    {
        _u8_internal_darray_realloc(array, array->capacity * 2);
    }
    array->data[array->num_items] = *data;
    array->num_items++;
}

void _u8_internal_darray_pop(u8_internal_darray* array)
{
    MASSERT_MSG(array, "DARRAY MACRO POP: NULL ARRAY");
    if (array->num_items <= 0)
    {
        WARN("DARRAY MACRO POP: NOTHING TO POP")
        return;
    }
    array->num_items--;
}

void _u8_internal_darray_clear(u8_internal_darray* array)
{
    MASSERT_MSG(array, "DARRAY MACRO CLEAR: NULL ARRAY");
    array->num_items = 0;
}

void _u8_internal_darray_zero_memory(u8_internal_darray* array)
{
    MASSERT_MSG(array, "DARRAY MACRO ZERO: NULL ARRAY");

    memset(array->data, 0, array->stride * array->capacity);
}

u64 _u8_internal_darray_get_bytes_used(u8_internal_darray* array)
{
    MASSERT_MSG(array, "DARRAY MACRO GET BYTES USED: NULL ARRAY");

    return array->num_items * array->stride;
}


typedef struct u8_internal_darray_slice
{
    u64 length;
    u8* ptr;
} u8_internal_darray_slice;

u8_internal_darray_slice _u8_internal_darray_slice_create(u8_internal_darray* array, u64 slice_length)
{
    MASSERT(slice_length <= array->capacity);
    return (u8_internal_darray_slice){.ptr = array->data, .length = slice_length};
}

u8_internal_darray_slice _u8_internal_array_slice_create_offset(u8_internal_darray* array, u64 slice_start,
                                                                u64 slice_length)
{
    MASSERT(slice_start < array->capacity)
    MASSERT(slice_length <= array->capacity);
    MASSERT(slice_start + slice_length <= array->capacity);


    return (u8_internal_darray_slice){.ptr = array->data + slice_start, .length = slice_length};
}

