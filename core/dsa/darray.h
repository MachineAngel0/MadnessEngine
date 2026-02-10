#ifndef DARRAY_H
#define DARRAY_H

#include <stdlib.h>
#include "unit_test.h"
#include "arena_stack.h"
#include "../memory/memory_tracker.h"


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
void* _darray_create_arena(const u64 intial_capacity, const u64 stride, Arena_Stack* arena);

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

bool darray_is_valid_index(void* array, const u64 index);

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

void* _darray_insert_at(void* array, const void* new_data, const u64 index);
//lets us use non stack values
#define darray_insert_at(array, value, index)                    \
{                                                           \
    __typeof__(value) temp_value = value;                   \
    array = _darray_insert_at(array, &temp_value, index);   \
}

void darray_pop(void* array);
void* darray_pop_return(void* array);


void darray_remove_shift(void* arr, const u64 index);
void darray_remove_swap(void* array, const u64 index);
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


void _darray_test(void);

#endif
