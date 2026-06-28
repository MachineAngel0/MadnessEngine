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

    Heap_Allocator* allocator;

#ifndef NDEBUG
    const char* type_name;
#endif
} Dynamic_Array;

#define DYNAMIC_ARRAY_TYPE(type) Dynamic_Array


Dynamic_Array* _dynamic_array_create(u32 data_stride, u64 capacity, Heap_Allocator* allocator);

void dynamic_array_free(Dynamic_Array* array);


void dynamic_array_resize(Dynamic_Array* array, u64 new_capacity);


void dynamic_array_push(Dynamic_Array* array, void* data);
void dynamic_array_push_multi(Dynamic_Array* array, void* data, u32 count);


void dynamic_array_pop(Dynamic_Array* array);


void dynamic_array_remove_swap(Dynamic_Array* array, u32 index);
void dynamic_array_remove_shift_left(Dynamic_Array* array, u32 index);


void* _dynamic_array_get(Dynamic_Array* array, u32 index);
void dynamic_array_set(Dynamic_Array* array, void* data, u32 index);


void dynamic_array_clear(Dynamic_Array* array);
void dynamic_array_zero(Dynamic_Array* array);

bool dynamic_array_is_empty(Dynamic_Array* array);


Dynamic_Array* dynamic_array_copy(Dynamic_Array* array_to_copy);
Array* dynamic_array_copy_fixed_size(Dynamic_Array* array_to_copy, Allocator* allocator);


void _dynamic_array_push_c_array(Dynamic_Array* dynamic_array, void* c_array,  u32 c_array_count);

bool dynamic_array_serialize(Dynamic_Array* array, FILE* fptr);

bool dynamic_array_deserialize(Dynamic_Array* array, FILE* fptr);



#define dynamic_array_push_c_array(dynamic_array, c_array) \
    _dynamic_array_push_c_array(dynamic_array, c_array, ARRAY_SIZE(c_array))



#ifndef NDEBUG

Dynamic_Array* _dynamic_array_create_debug(u64 data_stride, u64 capacity, Heap_Allocator* allocator,
                                           const char* type_name);
    void* _dynamic_array_get_debug(Dynamic_Array* array, u64 index, const char* type_name);

    #define dynamic_array_create(type, initial_capacity, allocator) \
        _dynamic_array_create_debug(sizeof(type), initial_capacity, allocator, #type);

    #define dynamic_array_get(arr, type, index)\
        (*(type*)_dynamic_array_get_debug(arr, index, #type))

#else

    #define dynamic_array_create(type, initial_capacity, allocator) \
        _dynamic_array_create(sizeof(type), initial_capacity, allocator);
    #define dynamic_array_get(arr, type, index)\
        (*(type*)_dynamic_array_get(arr, index))


#endif


#endif
