#ifndef DARRAY_H
#define DARRAY_H

#include "allocator_stack.h"
#include "array.h"


/* Array layout
 * HEADER -> num_items, stride, capacity, allocator
 * void* data
 */


#define DARRAY_RESIZE_VALUE  2
#define DARRAY_DEFAULT_CAPACITY 1

// literally does nothing, it's here just to easily see that something is a darray, without having to guess/remeber
// example:  darray_type(int*) -> int*
#define darray_type(type) type

//there is also this version that converts the type into a pointer
// example:  darray_type(int) -> int*
// #define darray_type(type) type*

typedef struct array_header
{
    //this header will be located at the front of the array
    u64 num_items; // length or top index in our array
    u64 stride; // size of each void* data
    u64 capacity; // max elements allowed in the array
    //TODO: allocator
} array_header;

void* _darray_create(const u64 capacity, const u64 stride);
void* _darray_create_arena(const u64 initial_capacity, const u64 stride, Stack_Allocator* arena);

#define darray_create(type) \
    (type*) _darray_create(DARRAY_DEFAULT_CAPACITY, sizeof(type))

#define darray_create_reserve(type, capacity) \
    (type*) _darray_create(capacity, sizeof(type))

void darray_free(void* array);

u64 darray_get_capacity(void* array);
u64 darray_get_size(void* array);
u64 darray_get_byte_size(void* array);


u64 darray_get_num_count(void* array);

u64 darray_get_stride(void* array);

bool darray_is_valid_index(void* array, u64 index);

bool darray_is_empty(void* array);

void* _darray_resize(void* array);


//NOTE: get and set are not technically needed
// but they provide a nice layer of making sure the index is valid, which is nice debugging
void* darray_get(void* array, const u64 index);

void darray_set(void* array, void* new_data, const u64 index);
void* _darray_push(void* array, const void* new_data);

//lets us use non stack values
#define darray_push(array, value)                          \
{                                                          \
    __typeof__(value) temp_value = value;         \
    array = _darray_push(array, &temp_value); \
}

void* _darray_push_range(void* array, const void* new_data, u64 new_items_count);

//lets us use non stack values
#define darray_push_range(array, value, new_items_count)   \
{                                                          \
    __typeof__(value) temp_value = value;         \
    array = _darray_push_range(array, &temp_value, new_items_count); \
}

void* _darray_insert_at(void* array, const void* new_data, u64 index);
//lets us use non stack values
#define darray_insert_at(array, value, index)                    \
{                                                           \
    __typeof__(value) temp_value = value;                   \
    array = _darray_insert_at(array, &temp_value, index);   \
}

void darray_pop(void* array);
void* darray_pop_return(void* array);


void darray_remove_shift(void* arr, u64 index);
void darray_remove_swap(void* array, u64 index);
bool darray_contains(void* array, void* key, int (*cmp_func)(void*, void*));

bool darray_contains_or_add(void* array, void* key, int (*cmp_func)(const void*, const void*));


bool darray_contains_or_add_other(void* array, void* key);


#define STBDS_ADDRESSOF(typevar, value)     ((__typeof__(typevar)[1]){value}) // literal array decays to pointer to value



#define darray_contains_or_add_test(array, value)    \
{                                                     \
    temp_value = (void*) STBDS_ADDRESSOF((array, (value)) \
    darray_contains_or_add_other(array, temp_value); \
}


void* _darray_duplicate(const void* array);


void darray_clear(void* array);


void darray_debug_header(void* array);
void darray_debug_print(void* array, void (*print_func)(void*));

void darray_debug_print_range(void* array, size_t start, size_t end, void (*print_func)(void*));

//TODO: one of them whenever I need it
// void darray_counting_sort(void* array);
// void darray_merge_sort(void* array);
// void darray_radix_sort(void* array);


void darray_test(void);


void darray_macro_test();


//THE OTHER DYNAMIC ARRAY


#define DYNAMIC_ARRAY_RESIZE_VALUE  2
#define DYNAMIC_DEFAULT_CAPACITY 1

typedef struct Dynamic_Array
{
    u64 capacity; // max elements allowed in the array
    u32 num_items; // length or top index in our array
    u32 stride; // size of each void* data
    void* data;

    //String* type_name; // TODO: for type checking and debugging
    Heap_Allocator* allocator;
} Dynamic_Array;

#define DYNAMIC_ARRAY_TYPE(type) Dynamic_Array

#define DYNAMIC_ARRAY_ITERATION_MACRO_DONT_USE(darray)\
for (u32 i; i < darray->num_items; i++)

Dynamic_Array* _dynamic_array_create(u32 data_stride, u64 capacity, Heap_Allocator* allocator)
{
    Dynamic_Array* array = allocator_heap_alloc(allocator, sizeof(Dynamic_Array));

    array->data = allocator_heap_alloc(allocator, data_stride * capacity);
    array->allocator = allocator;
    array->capacity = capacity;
    array->stride = data_stride;
    array->num_items = 0;

    return array;
}


Dynamic_Array* dynamic_array_free(Dynamic_Array* array)
{
    allocator_heap_free(array->allocator, array->data);
    allocator_heap_free(array->allocator, array);
    return array;
}

void dynamic_array_resize(Dynamic_Array* array, u64 new_capacity)
{
    if (new_capacity <= array->capacity)
    {
        WARN("DYNAMIC ARRAY: INVALID RESIZE")
        return;
    }

    void* new_data = allocator_heap_alloc(array->allocator, new_capacity * array->stride);

    memcpy(new_data, array->data, array->capacity * array->stride);
    allocator_heap_free(array->allocator, array->data);
    array->data = new_data;
    array->capacity = new_capacity;
}


void dynamic_array_push(Dynamic_Array* array, void* data)
{
    while (array->num_items >= array->capacity)
    {
        dynamic_array_resize(array, array->capacity * 2);
    }

    memcpy((u8*)array->data + (array->stride * array->num_items), data, array->stride);
    array->num_items++;
}

void dynamic_array_push_multi(Dynamic_Array* array, void* data, u32 count)
{
    //TODO:
    while (array->num_items + count >= array->capacity)
    {
        dynamic_array_resize(array, array->capacity * 2);
    }

    memcpy((u8*)array->data + (array->stride * array->num_items), data, array->stride * count);
    array->num_items += count;
}


void dynamic_array_pop(Dynamic_Array* array)
{
    if (array->num_items <= 0) return;
    array->num_items--;
}


void dynamic_array_remove_swap(Dynamic_Array* array, u32 index)
{
    MASSERT(array)
    MASSERT(index < array->num_items);
    if (array->num_items <= 0) return;

    //memcpy the last item into the removal spot
    memcpy((u8*)array->data + (array->stride * index),
           (u8*)array->data + (array->num_items - 1),
           array->stride);
    //minus one cause num_items always points to a free spot/ or nothing if full
    array->num_items--;
}

void dynamic_array_remove_shift_left(Dynamic_Array* array, u32 index)
{
    MASSERT(array)
    MASSERT(index < array->num_items);
    if (array->num_items <= 0) return;

    if (index == array->num_items)
    {
        dynamic_array_pop(array);
        return;
    }


    //take every value from the index to the last value
    u32 copy_length = array->num_items - index;


    //memcpy the last item into the removal spot
    memcpy((u8*)array->data + (array->stride * index),
           (u8*)array->data + (array->stride * index + 1), copy_length * array->stride);


    //minus one cause num_items always points to a free spot/ or nothing if full
    array->num_items--;
}


void* _dynamic_array_get(Dynamic_Array* array, u32 index)
{
    MASSERT(index < array->num_items)

    return ((u8*)array->data + (index * array->stride));
}

void dynamic_array_set(Dynamic_Array* array, void* data, u32 index)
{
    MASSERT(array)
    MASSERT(data)
    MASSERT(index < array->num_items)

    memcpy((u8*)array->data + (array->stride * index), data, array->stride);
}

void dynamic_array_clear(Dynamic_Array* array)
{
    array->num_items = 0;
}

bool dynamic_array_is_empty(Dynamic_Array* array)
{
    return array->num_items == 0;
}


Dynamic_Array* dynamic_array_copy(Dynamic_Array* array_to_copy)
{
    Dynamic_Array* out_array = _dynamic_array_create(array_to_copy->stride, array_to_copy->capacity,
                                                     array_to_copy->allocator);
    memcpy(out_array->data, array_to_copy->data, array_to_copy->num_items * array_to_copy->stride);
    return out_array;
}

Array* dynamic_array_copy_fixed_size(Dynamic_Array* array_to_copy, Allocator* allocator)
{
    Array* out_array = _array_create(array_to_copy->stride, array_to_copy->num_items, allocator);
    memcpy(out_array->data, array_to_copy->data, array_to_copy->num_items * array_to_copy->stride);
    return out_array;
}


void _dynamic_array_push_c_array(Dynamic_Array* dynamic_array, void* c_array, const u32 c_array_count)
{
    for (u32 i = 0; i < c_array_count; i++)
    {
        void* push_data = ((u8*)c_array + (dynamic_array->stride * i));
        dynamic_array_push(dynamic_array, push_data);
    }
}


#define dynamic_array_create(type, initial_capacity, allocator) \
        _dynamic_array_create(sizeof(type), initial_capacity, allocator);
#define dynamic_array_get(arr, type, index)\
        (*(type*)_dynamic_array_get(arr, index))
#define dynamic_array_push_c_array(dynamic_array, c_array) \
        _dynamic_array_push_c_array(dynamic_array, c_array, ARRAY_SIZE(c_array))


bool dynamic_array_serialize(Dynamic_Array* array, FILE* fptr)
{
    MASSERT(array);
    MASSERT(fptr);

    fwrite(&array->stride, sizeof(array->stride), 1, fptr);
    fwrite(&array->num_items, sizeof(array->num_items), 1, fptr);
    fwrite(&array->capacity, sizeof(array->capacity), 1, fptr);
    fwrite(array->data, array->num_items * array->stride, 1, fptr);

    return true;
}


bool dynamic_array_deserialize(Dynamic_Array* array, FILE* fptr)
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


#endif
