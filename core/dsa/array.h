#ifndef ARRAY_H
#define ARRAY_H

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

#include "_allocator_interface.h"

//VOID* VERSION


//fixed sized array, so no reallocating more space
typedef struct Array
{
    u64 capacity; // size of the array
    u32 stride; // size/stride of each void* data
    u32 num_items; // current/top index in our array
    void* data; //array of void* data

#ifndef NDEBUG
    char* type_name;
#endif
} Array;

#define ARRAY_TYPE(type) Array

//TODO: use the allocator interface
Array* _array_create(const u64 data_stride, const u64 capacity);

#define array_create(type, capacity)\
    _array_create(sizeof(type), capacity)

void array_free(Array* array);
void array_clear(Array* array);
void array_zero(Array* array);

void array_push(Array* array, const void* new_data);
void array_pop(Array* array);

bool array_serialize(Array* array, FILE* fptr);
bool array_deserialize(Array* array, FILE* file);



void array_print(Array* array, void (*print_func)(void*));
void array_print_range(Array* array, u64 start, u64 end, void (*print_func)(void*));

bool array_is_empty(const Array* array);
bool array_is_full(const Array* array);
bool array_valid_index(const Array* array, const u64 index);
void* _array_get(Array* array, const u64 index);

#define array_top(arr, type)\
    (*(type*)_array_get(arr, arr->num_items-1))

#define array_get(arr, type, index)\
    (*(type*)_array_get(arr, index))

#define array_top_free(arr, type)\
    (*(type*)_array_get(arr, arr->num_items))

void array_set(Array* array, const void* data, const u64 pos);


//TODO: TEST
// fills entire array
void array_fill(const Array* array, const void* data);

//TODO: TEST
// fills on a range array
void array_fill_range(Array* array, u64 start, u64 end, void* data);


//shift the array, maintaining order
void array_remove(Array* array, const u64 index);

//replaces the index value with the last item in the array, then decrementing the array num count
void array_remove_swap(Array* array, u64 index);

//TODO: all the sorting functions
void array_counting_sort(Array* array, int (*cmp_func)(void*, void*));

void array_merge_sort(Array* array, int (*cmp_func)(void*, void*));

void array_radix_sort(Array* array, int (*cmp_func)(void*, void*));


void array_test();


// #define STBDS_ADDRESSOF(typevar, value)     ((__typeof__(typevar)[1]){value}) // literal array decays to pointer to value
// // #define array_new_contains_or_add_test(array, value)    \
// {                                                     \
//     temp_value = (void*) STBDS_ADDRESSOF((array, (value)) \
//     array_new_contains_or_add_other(array, temp_value); \
// }


//MACRO VERSION

#define _array_struct_macro(type)\
    typedef struct type##_array{ \
        Allocator_Interface allocator;\
        u64 capacity; \
        u64 num_items;\
        type* data;\
    }type##_array;

#define _array_create_macro(type)\
    type##_array* type##_array_create(u64 capacity, Allocator_Interface allocator) \
    { \
        if (capacity <= 0)\
        {\
            WARN("ARRAY MACRO CREATE: INVALID CAPACITY")\
            return NULL;\
        } \
        MASSERT(allocator.alloc);\
        MASSERT(allocator.free_memory);\
        MASSERT(allocator.allocator);\
        type##_array* arr = allocator.alloc(allocator.allocator, sizeof(type##_array), DEFAULT_ALIGNMENT);\
        arr->data = allocator.alloc(allocator.allocator, sizeof(type) * capacity, DEFAULT_ALIGNMENT); \
        arr->allocator = allocator;\
        arr->capacity = capacity;\
        arr->num_items = 0; \
        return arr; \
    }

#define _array_free_macro(type)\
    void type##_array_free(type##_array* array)\
    {\
        MASSERT_MSG(array, "ARRAY MACRO FREE: NULL ARRAY")\
        array->allocator.free_memory(array->allocator.allocator, array->data); \
        array->allocator.free_memory(array->allocator.allocator, array);\
    }


#define _array_push_macro(type)\
    void type##_array_push(type##_array* array, const type* data) \
    {\
    MASSERT_MSG(array, "ARRAY MACRO INTRUSIVE PUSH: NULL ARRAY");\
    if (array->num_items >= array->capacity)\
    {\
        WARN("ARRAY MACRO PUSH: FULL ARRAY")\
        return;\
    }\
    array->data[array->num_items] = *data;\
    array->num_items++;\
    }

#define _array_push_multi_macro(type)\
void type##_array_push_multi(type##_array* array, const type* data, const u32 array_size) \
{\
    MASSERT_MSG(array, "ARRAY MACRO PUSH MULTI: NULL ARRAY");\
    if (array->num_items >= array->capacity)\
    {\
        return;\
    }\
    if(array->num_items + array_size >=  array->capacity)\
    {\
        WARN("ARRAY MACRO PUSH MULTI: WILL OVERFLOW ARRAY")\
        return;\
    }\
    for (int i = 0; i < array_size; i++)\
    {\
        array->data[array->num_items] = data[i];\
        array->num_items++;\
    }\
}


#define _array_pop_macro(type)\
    void type##_array_pop(type##_array* array) \
    {\
        MASSERT_MSG(array, "ARRAY MACRO POP: NULL ARRAY");\
        if (array->num_items <= 0)\
        {\
            WARN("ARRAY MACRO POP: NOTHING TO POP")\
            return;\
        }\
        array->num_items--;\
    }
#define _array_clear_macro(type)\
    void type##_array_clear(type##_array* array) \
    {\
        MASSERT_MSG(array, "ARRAY MACRO CLEAR: NULL ARRAY");\
        array->num_items = 0;\
    }
#define _array_zero_macro(type)\
    void type##_array_zero(type##_array* array)\
    {\
        MASSERT_MSG(array, "ARRAY MACRO ZERO: NULL ARRAY");\
        memset(array->data, 0, sizeof(type) * array->capacity);\
    }
#define _array_get_bytes_used(type)\
    u64 type##_array_get_bytes_used(type##_array* array)\
    {\
        MASSERT_MSG(array, "ARRAY MACRO GET BYTES USED: NULL ARRAY");\
        return array->num_items * sizeof(type);\
    }
#define _array_slice_macro(type) \
    typedef struct type##_array_slice{\
        u64 length;\
        type* ptr;\
    } type##_array_slice;\
    \
    type##_array_slice  type##_array_slice_create(type##_array* array, u64 slice_length)\
    {\
        MASSERT(slice_length <= array->capacity);\
        return (type##_array_slice){.ptr = array->data, .length = slice_length};\
    }\
    \
     type##_array_slice type##_array_slice_create_offset(type##_array* array, u64 slice_start, u64 slice_length)\
    {\
        MASSERT(slice_start < array->capacity)\
        MASSERT(slice_length <= array->capacity);\
        MASSERT(slice_start + slice_length <= array->capacity);\
        return (type##_array_slice){.ptr = array->data + slice_start, .length = slice_length};\
    }


#define ARRAY_GENERATE_TYPE(type) \
    _array_struct_macro(type)\
    _array_create_macro(type)\
    _array_free_macro(type)\
    _array_push_macro(type)\
    _array_push_multi_macro(type)\
    _array_pop_macro(type)\
    _array_clear_macro(type)\
    _array_zero_macro(type)\
    _array_get_bytes_used(type) \
    _array_slice_macro(type)

//TODO: functions
// free, get, set, print, remove and remove_swap_back, sort function


ARRAY_GENERATE_TYPE(u8)
ARRAY_GENERATE_TYPE(u16)
ARRAY_GENERATE_TYPE(u32)
ARRAY_GENERATE_TYPE(u64)

ARRAY_GENERATE_TYPE(s8)
ARRAY_GENERATE_TYPE(s16)
ARRAY_GENERATE_TYPE(s32)
ARRAY_GENERATE_TYPE(s64)

ARRAY_GENERATE_TYPE(f32)
ARRAY_GENERATE_TYPE(f64)

ARRAY_GENERATE_TYPE(bool)


void array_macro_test();


#endif //ARRAY_H
