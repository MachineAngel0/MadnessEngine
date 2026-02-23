#include "array_new.h"

#include <time.h>

//NOTE: this is here for testing purposes to make sure there as little bugs as possilbe in the macros
typedef struct u8_internal_array
{
    u64 capacity;
    u64 stride;
    u64 num_items;
    u8* data;
} u8_internal_array;

u8_internal_array* _u8_internal_array_create(u64 capacity)
{
    u8_internal_array* arr = malloc(sizeof(u8_internal_array));
    memset(arr, 0, sizeof(u8_internal_array));
    arr->data = malloc(sizeof(u8) * capacity);
    memset(arr->data, 0, sizeof(u8) * capacity);
    arr->capacity = capacity;
    arr->stride = sizeof(u8);
    arr->num_items = 0;
    return arr;
};

void _u8_internal_array_push(u8_internal_array* array, const u8* data)
{
    if (!array)
    {
        WARN("ARRAY MACRO PUSH: NULL ARRAY")
        return;
    }
    if (array->num_items >= array->capacity)
    {
        WARN("ARRAY MACRO PUSH: NULL ARRAY")
        return;
    }
    array->data[array->num_items] = *data;
    array->num_items++;
}

void _u8_internal_array_pop(u8_internal_array* array)
{
    if (!array)
    {
        WARN("ARRAY MACRO POP: NULL ARRAY")
        return;
    }
    if (array->num_items <= 0)
    {
        WARN("ARRAY MACRO POP: NOTHING TO POP")
        return;
    }
    array->num_items--;
}

void _u8_internal_array_clear(u8_internal_array* array)
{
    if (!array)
    {
        WARN("ARRAY MACRO CLEAR: NULL ARRAY")
        return;
    }
    array->num_items = 0;
}

void _u8_internal_array_zero(u8_internal_array* array)
{
    if (!array)
    {
        WARN("ARRAY MACRO ZERO: NULL ARRAY")
        return;
    }
    memset(array->data, 0, array->stride * array->capacity);
}


typedef struct u8_internal_array_slice
{
    u64 length;
    u8* ptr;
} u8_internal_array_slice;

u8_internal_array_slice _u8_internal_array_slice_create(u8_internal_array* array, u64 slice_length)
{
    MASSERT(slice_length <= array->capacity);
    return (u8_internal_array_slice){.ptr = array->data, .length = slice_length};
}

u8_internal_array_slice _u8_internal_array_slice_create_offset(u8_internal_array* array, u64 slice_start,
                                                              u64 slice_length)
{
    MASSERT(slice_start < array->capacity)
    MASSERT(slice_length <= array->capacity);
    MASSERT(slice_start + slice_length <= array->capacity);
    return (u8_internal_array_slice){.ptr = array->data + slice_start, .length = slice_length};
}

void array_macro_test()
{
    TEST_START("ARRAY MACRO");

    u64 test_capacity = 100;
    u8_array* arr = u8_array_create(test_capacity);
    u8_internal_array* arr_internal = _u8_internal_array_create(test_capacity);

    TEST_DEBUG(arr->capacity == test_capacity);
    TEST_DEBUG(arr->stride == sizeof(u8));

    TEST_DEBUG(arr->capacity == arr_internal->capacity);
    TEST_DEBUG(arr->stride == arr_internal->stride);


    srand((u64)time(NULL));

    for (u64 i = 0; i < test_capacity; i++)
    {
        u8 val = (u8)(rand() % test_capacity);
        u8_array_push(arr, &val);
        _u8_internal_array_push(arr_internal, &val);
    }

    for (int i = 0; i < test_capacity; i++)
    {
        TEST_DEBUG(arr->data[i] == arr_internal->data[i] );
    }

    for (int i = 0; i < test_capacity; i++)
    {
        u8_array_pop(arr);
        _u8_internal_array_pop(arr_internal);
    }

    for (u64 i = 0; i < test_capacity; i++)
    {
        u8 val = (u8)(rand() % test_capacity);
        u8_array_push(arr, &val);
        _u8_internal_array_push(arr_internal, &val);
    }

    u8_array_clear(arr);
    _u8_internal_array_clear(arr_internal);


    //TODO: test slices
    u8_array_slice slice = u8_array_slice_create(arr, test_capacity / 2);
    u8_internal_array_slice slice_internal = _u8_internal_array_slice_create(arr_internal, test_capacity / 2);

    for (u64 i = 0; i < slice.length; i++)
    {
        TEST_DEBUG(slice.ptr[i] ==  slice_internal.ptr[i] );
    }
    for (u64 i = 0; i < slice_internal.length; i++)
    {
        TEST_DEBUG(slice.ptr[i] ==  slice_internal.ptr[i] );
    }


    TEST_END("ARRAY MACRO");
}
