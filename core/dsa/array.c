#include "array.h"

#include "misc_util.h"
#include "unit_test.h"


Array* _array_create(const u64 data_stride, const u64 capacity, const char* type_name)
{
    Array* arr = (Array*)malloc(sizeof(Array));
    memset(arr, 0, sizeof(Array));

    //alloc and zero
    arr->data = malloc(capacity * data_stride);
    memset(arr->data, 0, capacity * data_stride);

    arr->num_items = 0;
    arr->stride = data_stride;
    arr->capacity = capacity;

#ifndef NDEBUG
    arr->debug_type_name = type_name;
    DEBUG("ARRAY CREATED WITH TYPE: %s", arr->debug_type_name);
#endif

    return arr;
}


bool _array_type_check(Array* array, const char* type_name)
{
#ifndef NDEBUG

    if (strcmp(array->debug_type_name, type_name) == 0)
    {
        DEBUG("SAME TYPES: %s", type_name)
        return true;
    }


    WARN("DIFFERENT TYPES: ARRAY TYPE: %s, PASSED IN TYPE: %s", array->debug_type_name, type_name)
    return false;
#endif

    return true; // going to assume you are using the right type, given this is release build
};

#define array_type_check(arr, type)\
    _array_type_check(arr, #type)


void array_free(Array* array)
{
    free(array->data);
    free(array);
}

void array_clear(Array* array)
{
    array->num_items = 0;
}

void array_print(Array* array, void (*print_func)(void*))
{
    for (u64 i = 0; i < array->num_items; i++)
    {
        print_func(
            (void*)((u8*)array->data + (i * array->stride))
        );
    }
    printf("\n");
}

void array_print_range(Array* array, u64 start, u64 end, void (*print_func)(void*))
{
    //technically there should be a check to make sure the values are not zero but seriouly come on
    if (start > end)
    {
        WARN("PRINT RANGE: START IS GREATER THAN END");
        return;
    }
    if (start > array->num_items)
    {
        WARN("PRINT RANGE: START IS GREATER ARRAY SIZE");
        return;
    }
    if (end > array->num_items)
    {
        WARN("PRINT RANGE: END IS GREATER ARRAY SIZE");
        return;
    }


    for (u64 i = start; i < end; i++)
    {
        print_func(
            (void*)((u8*)array->data + (i * array->stride))
        );
    }
    printf("\n");
}


bool array_is_empty(const Array* array)
{
    return array->num_items == 0;
}

bool array_is_full(const Array* array)
{
    return array->num_items >= array->capacity;
}

bool array_valid_index(const Array* array, const u64 index)
{
    //return true if the index is in a valid range, of already set items
    return index < array->num_items;
}

void* array_get(Array* array, const u64 index)
{
    if (!array)
    {
        WARN("ARRAY GET: INVALID ARRAY");
        return NULL;
    }
    if (!array_valid_index(array, index))
    {
        WARN("ARRAY GET: INVALID INDEX");
        return NULL;
    }
    if (array_is_empty(array))
    {
        WARN("ARRAY GET: ARRAY IS EMPTY");
        return NULL;
    }
    return (u8*)array->data + ((index) * array->stride);
}

void array_set(Array* array, const void* data, const u64 pos)
{
    if (!array_valid_index(array, pos))
    {
        WARN("ARRAY SET: INVALID INDEX");
        return;
    }

    //mem copy the data
    u8* dest = (u8*)array->data + (array->stride * pos);
    memcpy(dest, data, array->stride);
}

//TODO: TEST
// fills entire array
void array_fill(const Array* array, const void* data)
{
    for (u64 i = 0; i < array->num_items; i++)
    {
        u8* dest = (u8*)array->data + (array->stride * i);
        memcpy(dest, data, array->stride);
    }
}

//TODO: TEST
// fills on a range array
void array_fill_range(Array* array, u64 start, u64 end, void* data)
{
    //check that start is larger than end
    if (start < end)
    {
        WARN("PRINT RANGE: START IS GREATER THAN END");
        return;
    }
    //check that end is not greater than num items
    if (array_valid_index(array, end))
    {
        WARN("ARRAY FILL RANGE: END GREATER THAN ARRAY SIZE");
        return;
    }

    for (u64 i = start; i < end; i++)
    {
        u8* dest = (u8*)array->data + (array->stride * i);
        memcpy(dest, data, array->stride);
    }
}

void array_push(Array* array, void* new_data)
{
    if (array_is_full(array))
    {
        WARN("ARRAY PUSH}:  ARRAY IS FULL, CAN'T PUSH");
        return;
    }
    u8* dest = (u8*)array->data + (array->stride * (array->num_items));
    memcpy(dest, new_data, array->stride);

    array->num_items++;
}

void array_pop(Array* array)
{
    if (array_is_empty(array))
    {
        WARN("ARRAY POP: ARRAY IS EMPTY");
        return;
    }
    array->num_items--;
}


//shift the array, maintaining order
void array_remove(Array* array, const u64 index)
{
    if (array_is_empty(array))
    {
        WARN("ARRAY REMOVE: ARRAY IS EMPTY, NOTHING TO REMOVE");
        return;
    }
    if (!array_valid_index(array, index))
    {
        WARN("ARRAY REMOVE: INVALID INDEX");
        return;
    };

    //shift the array left from the index spot of removal

    memcpy((u8*)array->data + (array->stride * index),
           (u8*)array->data + (array->stride * (index + 1)),
           array->stride * (array->num_items - index - 1));

    array->num_items--;
}

//replaces the index value with the last item in the array, then decrementing the array num count
void array_remove_swap(Array* array, u64 index)
{
    if (array_is_empty(array))
    {
        WARN("ARRAY REMOVE SWAP: ARRAY IS EMPTY, NOTHING TO REMOVE");
        return;
    }
    if (!array_valid_index(array, index))
    {
        WARN("ARRAY REMOVE SWAP FAILED, INVALID INDEX");
        return;
    };

    //memcpy the last item into the removal spot
    memcpy((u8*)array->data + (array->stride * index),
           (u8*)array->data + (array->stride - 1),
           array->stride);
    //minus one cause num_items always points to a free spot/ or nothing if full
    array->num_items--;
}

//TODO: all the sorting functions
void array_counting_sort(Array* array, int (*cmp_func)(void*, void*));

void array_merge_sort(Array* array, int (*cmp_func)(void*, void*));

void array_radix_sort(Array* array, int (*cmp_func)(void*, void*));





void array_test()
{
    TEST_START("ARRAY");

    Array* balling_arr = array_create(int, 10);
    array_type_check(balling_arr, int);
    array_type_check(balling_arr, float);


    printf("ARRAY START\n");
    int num3 = 3;
    int num5 = 5;
    int num10 = 10;
    int num20 = 20;
    int arr_capacity = 5;
    Array* arr = array_create(int, arr_capacity);

    printf("ARRAY EMPLACE START\n");
    array_push(arr, &num5);
    array_push(arr, &num10);
    array_push(arr, &num10);
    array_push(arr, &num10);
    array_push(arr, &num10);
    array_push(arr, &num10); //this should fail
    array_print(arr, print_int);
    TEST_DEBUG(arr->num_items == 5);
    printf("ARRAY EMPLACE END\n\n");

    printf("ARRAY POP START\n");
    array_pop(arr);
    array_pop(arr);
    array_pop(arr);
    array_print(arr, print_int);
    TEST_DEBUG(arr->num_items == 2);
    array_pop(arr);
    array_pop(arr);
    array_pop(arr);
    TEST_DEBUG(arr->num_items == 0);
    printf("ARRAY POP END\n\n");

    array_push(arr, &num10);
    array_push(arr, &num5);
    printf("ARRAY GET START\n");
    print_int(array_get(arr, 0));
    TEST_DEBUG(*(int*)array_get(arr, 0) == num10);

    printf("\n");
    printf("ARRAY GET END\n\n");

    printf("ARRAY SET START\n");
    array_set(arr, &num3, 2); // this should be invalid
    array_set(arr, &num3, 1); //overwrites the 10
    TEST_DEBUG(*(int*)array_get(arr, 1) == num3);
    array_print(arr, print_int);
    printf("ARRAY SET END\n\n");

    printf("ARRAY REMOVE START\n");
    array_remove(arr, 0);
    TEST_DEBUG(*(int*)array_get(arr, 0) == num3);
    array_print(arr, print_int);
    array_push(arr, &num20);
    array_push(arr, &num10);
    array_push(arr, &num5);
    array_push(arr, &num20);
    array_print(arr, print_int);

    array_remove(arr, arr_capacity); //should warn
    array_remove(arr, arr_capacity - 1);
    array_remove(arr, arr_capacity - 1); //should warn
    array_print(arr, print_int);

    array_remove(arr, 0);
    array_print(arr, print_int);

    printf("ARRAY REMOVE END\n\n");


    printf("ARRAY END\n\n");

    array_free(arr);

    TEST_REPORT("ARRAY");
}
