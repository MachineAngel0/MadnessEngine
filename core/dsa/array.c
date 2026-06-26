#include "array.h"

#include "misc_util.h"
#include "unit_test.h"


Array* _array_create(const u64 data_stride, const u64 capacity, Allocator* allocator)
{
    MASSERT(allocator)

    Array* arr = NULL;
    arr = allocator_alloc(allocator, sizeof(Array));
    arr->data = allocator_alloc(allocator, capacity * data_stride);

    /*if (fl_allocator)
    {
        arr = allocator_fl_alloc(fl_allocator, sizeof(Array));
        arr->data = allocator_fl_alloc(fl_allocator, capacity * data_stride);
        arr->allocator_fl = fl_allocator;
    }*/


    arr->num_items = 0;
    arr->stride = data_stride;
    arr->capacity = capacity;


    return arr;
}


#define array_type_check(arr, type)\
    _array_type_check(arr, #type)


void array_free(Array* array)
{
    /*if (array->allocator_fl)
    {
        allocator_fl_free(array->allocator_fl, array->data);
        allocator_fl_free(array->allocator_fl, array);
    }*/

    //rn does nothing
}

void array_clear(Array* array)
{
    array->num_items = 0;
}

void array_zero(Array* array)
{
    memset(array->data, 0, array->num_items * array->stride);
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

u64 array_get_bytes_used(const Array* array)
{
    return array->num_items * array->stride;
}

void* _array_get(Array* array, const u64 index)
{
    if (!array)
    {
        WARN("ARRAY GET: INVALID ARRAY");
        return NULL;
    }
    if (array_is_empty(array))
    {
        WARN("ARRAY GET: ARRAY IS EMPTY");
        return NULL;
    }
    if (!array_valid_index(array, index))
    {
        WARN("ARRAY GET: INVALID INDEX");
        return NULL;
    }

    return (u8*)array->data + ((index) * array->stride);
}

void* _array_top(Array* array)
{
     return _array_get(array, array->num_items);
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

void array_push(Array* array, const void* new_data)
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

void _array_push_c_array(Array* array, const void* new_data, const u32 count)
{
    for (u32 i = 0; i < count; i++)
    {
        void* push_data = ((u8*)new_data + (array->stride * i));
        array_push(array, push_data);
    }
}

bool array_serialize(Array* array, FILE* fptr)
{
    MASSERT(array);
    MASSERT(fptr);

    fwrite(&array->stride, sizeof(array->stride), 1, fptr);
    fwrite(&array->num_items, sizeof(array->num_items), 1, fptr);
    fwrite(&array->capacity, sizeof(array->capacity), 1, fptr);
    fwrite(array->data, array->num_items * array->stride, 1, fptr);

    return true;
}

bool array_deserialize(Array* array, FILE* fptr)
{
    MASSERT(array);
    MASSERT(array);
    MASSERT(fptr);

    fread(&array->stride, sizeof(array->stride), 1, fptr);
    fread(&array->num_items, sizeof(array->num_items), 1, fptr);
    fread(&array->capacity, sizeof(array->capacity), 1, fptr);
    fread(array->data, array->num_items * array->stride, 1, fptr);

    return true;
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
           (u8*)array->data + (array->num_items - 1),
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

    Allocator allocator;
    u64 memory_size = 10000;
    void* memory = malloc(memory_size);
    allocator_init(&allocator, memory, memory_size);


    Array* balling_arr = array_create(int, 10, &allocator);


    printf("ARRAY START\n");
    int num3 = 3;
    int num5 = 5;
    int num10 = 10;
    int num20 = 20;
    int arr_capacity = 5;
    Array* arr = array_create(int, arr_capacity, &allocator);

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
    print_int(_array_get(arr, 0));
    TEST_DEBUG(array_get(arr, int, 0) == num10);

    printf("\n");
    printf("ARRAY GET END\n\n");

    printf("ARRAY SET START\n");
    array_set(arr, &num3, 2); // this should be invalid
    array_set(arr, &num3, 1); //overwrites the 10
    TEST_DEBUG(array_get(arr, int, 1) == num3);
    array_print(arr, print_int);
    printf("ARRAY SET END\n\n");

    printf("ARRAY REMOVE START\n");
    array_remove(arr, 0);
    TEST_DEBUG(array_get(arr, int, 0) == num3);
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


    free(memory);

    TEST_REPORT("ARRAY");
}
