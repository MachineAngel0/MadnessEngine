#ifndef ARRAY_H
#define ARRAY_H
#include <stdlib.h>

#include "../core/misc_util.h"
#include "../core/logger.h"

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

//fixed sized array, so no reallocating more space
typedef struct Array
{
    void** data; //array of void* data
    size_t data_size; // size/stride of each void* data
    size_t capacity; // size of the array
    size_t num_items; // current/top index in our array
} Array;

Array* array_create(size_t data_size, size_t capacity)
{
    Array* arr = (Array*) malloc(sizeof(Array));

    //alloc and zero
    arr->data = malloc(capacity * sizeof(void *));
    memset(arr->data, 0, capacity * sizeof(void *));

    arr->num_items = 0;
    arr->data_size = data_size;
    arr->capacity = capacity;
    return arr;
}

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
    for (size_t i = 0; i < array->num_items; i++)
    {
        print_func(array->data[i]);
    }
    printf("\n");
}

void array_print_range(Array* array, size_t start, size_t end, void (*print_func)(void*))
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


    for (size_t i = start; i < end; i++)
    {
        print_func(array->data[i]);
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

bool array_valid_index(Array* array, size_t index)
{
    //return true if the index is in a valid range, of already set items
    return index < array->num_items;
}

void* array_get(Array* array, size_t index)
{
    if (!array_valid_index(array, index))
    {
        WARN("ARRAY GET FAILED, INVALID INDEX");
        return NULL;
    }
    return array->data[index];
}

void array_set(Array* array, void* data, size_t pos)
{
    if (!array_valid_index(array, pos))
    {
        WARN("ARRAY SET FAILED, INVALID INDEX");
        return;
    }
    array->data[pos] = data;
}

//TODO: TEST
// fills entire array
void array_fill(Array* array, void* data)
{
    for (size_t i = 0; i < array->num_items; i++)
    {
        array->data[i] = data;
    }
}

//TODO: TEST
// fills on a range array
void array_fill_range(Array* array, size_t start, size_t end, void* data)
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

    for (size_t i = start; i < end; i++)
    {
        array->data[i] = data;
    }
}

void array_emplace(Array* array, void* new_data)
{
    if (array_is_full(array))
    {
        WARN("ARRAY IS FULL, CANT EMPLACE");
        return;
    }
    array->data[array->num_items] = new_data;
    array->num_items++;
}

void array_pop(Array* array)
{
    if (array_is_empty(array))
    {
        WARN("ARRAY POP: ARRAY IS EMPTY");
        return;
    }
    // were only setting this to zero so the print is correct

    array->num_items--;
}


//shift the array, maintaining order
void array_remove(Array* array, size_t index)
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


    for (size_t i = index; i < array->num_items; i++)
    {
        array->data[i] = array->data[i + 1];
    }

    array->num_items--;
}

//replaces the index value with the last item in the array, then decrementing the array num count
void array_remove_swap(Array* array, size_t index)
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

    //minus one cause num_items always points to a free spot/ or nothing if full
    array[index] = array[array->num_items - 1];
    array->num_items--;
}

//TODO: all the sorting functions
void array_counting_sort(Array* array, int (*cmp_func)(void*, void*));

void array_merge_sort(Array* array, int (*cmp_func)(void*, void*));

void array_radix_sort(Array* array, int (*cmp_func)(void*, void*));


void array_test()
{
    printf("ARRAY START\n");
    int num3 = 3;
    int num5 = 5;
    int num10 = 10;
    int num20 = 20;
    int arr_capacity = 5;
    Array* arr = array_create(sizeof(int), arr_capacity);

    printf("ARRAY EMPLACE START\n");
    array_emplace(arr, &num5);
    array_emplace(arr, &num10);
    array_emplace(arr, &num10);
    array_emplace(arr, &num10);
    array_emplace(arr, &num10);
    array_emplace(arr, &num10); //this should fail
    array_print(arr, print_int);
    MASSERT(arr->num_items == 5);
    printf("ARRAY EMPLACE END\n\n");

    printf("ARRAY POP START\n");
    array_pop(arr);
    array_pop(arr);
    array_pop(arr);
    array_print(arr, print_int);
    MASSERT(arr->num_items == 2);
    array_pop(arr);
    array_pop(arr);
    array_pop(arr);
    MASSERT(arr->num_items == 0);
    printf("ARRAY POP END\n\n");

    array_emplace(arr, &num5);
    array_emplace(arr, &num10);
    printf("ARRAY GET START\n");
    print_int(array_get(arr, 0));
    printf("\n");
    printf("ARRAY GET END\n\n");

    printf("ARRAY SET START\n");
    array_set(arr, &num3, 2);
    array_set(arr, &num3, 1); //overwrites the 10
    array_print(arr, print_int);
    printf("ARRAY SET END\n\n");

    printf("ARRAY REMOVE START\n");
    array_remove(arr, 0);
    array_print(arr, print_int);
    array_emplace(arr, &num20);
    array_emplace(arr, &num10);
    array_emplace(arr, &num5);
    array_emplace(arr, &num20);
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
}


#endif //ARRAY_H
