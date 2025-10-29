
#pragma once

#include <stdio.h>
#include <stdlib.h>
typedef struct DArray
{
    void** data; //array of void* data
    size_t data_size; // size of each void* data
    size_t capacity; // size of the array
    size_t num_items; // current/top index in our array
}DArray;

DArray* darray_create(size_t data_size, size_t capacity)
{
    DArray* arr = malloc(sizeof(DArray));

    //alloc and zero
    arr->data = malloc(capacity * sizeof(void *));
    memset(arr->data, 0, capacity * sizeof(void *));

    arr->num_items = 0;
    arr->data_size = data_size;
    arr->capacity = capacity;
    return arr;
}

void darray_free(DArray* array)
{
    free(array->data);
    free(array);
}

void darray_clear(DArray* array)
{
    array->num_items = 0;
}

void darray_resize(DArray* array, size_t new_capacity)
{
    array->data = realloc(array->data, new_capacity * sizeof(void *));

    if (array->num_items < array->capacity)
    {
        //memset new memory
        memset(array->data + array->capacity, 0, (new_capacity - array->capacity) * sizeof(void *));
    }

    //in the case we size down
    if (array->num_items > array->capacity)
    {

        array->num_items = array->capacity;
    }

    array->capacity = new_capacity;

}


void darray_print(DArray* array, void (*print_func)(void*))
{
    for (size_t i = 0; i < array->num_items; i++)
    {
        print_func(array->data[i]);
    }
    printf("\n");
}

void darray_print_range(DArray* array, size_t start, size_t end, void (*print_func)(void*))
{
    //technically there should be a check to make sure the values are not zero but seriouly come on
    if (start > end)
    {
        WARN("DARRAY PRINT RANGE: START IS GREATER THAN END");
        return;
    }
    if (start > array->num_items)
    {
        WARN("DARRAY PRINT RANGE: START IS GREATER ARRAY SIZE");
        return;
    }
    if (end > array->num_items)
    {
        WARN("DARRAY PRINT RANGE: END IS GREATER ARRAY SIZE");
        return;
    }


    for (size_t i = start; i < end; i++)
    {
        print_func(array->data[i]);
    }
    printf("\n");

}



bool darray_is_empty(const DArray* array)
{
    return array->num_items == 0;
}

bool darray_is_full(const DArray* array)
{
    return array->num_items >= array->capacity;
}

bool darray_valid_index(DArray* array, size_t index)
{
    //return true if the index is in a valid range, of already set items
    return index < array->num_items;
}

void* darray_get(DArray* array, size_t index)
{
    if (!darray_valid_index(array, index))
    {
        WARN("DARRAY GET FAILED, INVALID INDEX");
        return NULL;
    }
    return array->data[index];
}

void darray_set(DArray* array, void* data, size_t pos)
{
    if (!darray_valid_index(array, pos))
    {
        WARN("DARRAY SET FAILED, INVALID INDEX");
        return;
    }
    array->data[pos] = data;
}

//TODO:
// fills entire array
// void darray_fill(Array* array, void* data);
// fills on a range array
// void darray_fill_range(Array* array, size_t start, size_t end, void* data);

void darray_emplace(DArray* array, void* new_data)
{
    if (darray_is_full(array))
    {
        darray_resize(array, array->capacity * 2);
    }
    array->data[array->num_items] = new_data;
    array->num_items++;
}

void darray_pop(DArray* array)
{
    if (darray_is_empty(array))
    {
        WARN("DARRAY POP: ARRAY IS EMPTY");
        return;
    }
    // were only setting this to zero so the print is correct

    array->num_items--;
};


//shift the array, maintaining order
void darray_remove(DArray* array, size_t index)
{
    if (darray_is_empty(array))
    {
        WARN("DARRAY REMOVE: ARRAY IS EMPTY, NOTHING TO REMOVE");
        return;
    }
    if (!darray_valid_index(array, index))
    {
        WARN("DARRAY REMOVE: INVALID INDEX");
        return;
    };


    for (size_t i = index; i < array->num_items; i++)
    {
        array->data[i] = array->data[i + 1];
    }

    array->num_items--;
}

//replaces the index value with the last item in the array, then decrementing the array num count
void darray_remove_swap(DArray* array, size_t index)
{
    if (darray_is_empty(array))
    {
        WARN("DARRAY REMOVE SWAP: ARRAY IS EMPTY, NOTHING TO REMOVE");
        return;
    }
    if (!darray_valid_index(array, index))
    {
        WARN("DARRAY REMOVE SWAP FAILED, INVALID INDEX");
        return;
    };

    //minus one cause num_items always points to a free spot/ or nothing if full
    array[index] = array[array->num_items-1];
    array->num_items--;
}



void darray_counting_sort(DArray* array);

void darray_merge_sort(DArray* array);

void darray_radix_sort(DArray* array);


void darray_test()
{
    printf("DARRAY START\n");



    printf("DARRAY END\n\n");

}


