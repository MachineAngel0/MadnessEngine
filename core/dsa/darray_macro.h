#ifndef DARRAY_H
#define DARRAY_H

#include "allocator_freelist.h"
#include "unit_test.h"


#define _darray_struct_macro(type)\
typedef struct type##_darray{ \
    u64 capacity; \
    u64 stride; \
    u64 num_items;\
    type* data;\
    Arena_Free_List* free_list; \
}type##_darray;

#define _darray_create(type)\
type##_darray* type##_darray_create(u64 capacity, Arena_Free_List* free_list)\
{\
    MASSERT_MSG(capacity > 0, "DARRAY MACRO CREATE: INVALID SIZE");\
    type##_darray* arr;\
    if (free_list)\
    {\
        arr = arena_free_list_alloc(free_list, sizeof(type##_darray));\
        arr->data = arena_free_list_alloc(free_list, sizeof(type) * capacity);\
        arr->free_list = free_list;\
    }\
    else\
    {\
        arr = malloc(sizeof(type));\
        memset(arr, 0, sizeof(type));\
        arr->data = malloc(sizeof(type) * capacity);\
        memset(arr->data, 0, sizeof(type) * capacity);\
    }\
    arr->capacity = capacity;\
    arr->stride = sizeof(type);\
    arr->num_items = 0;\
    return arr;\
}

#define _darray_free(type)\
void type##_darray_free(type##_darray* array)\
{\
    MASSERT_MSG(array, "DARRAY MACRO FREE: NULL ARRAY")\
    if (array->free_list)\
    {\
        arena_free_list_free(array->free_list, array->data);\
        arena_free_list_free(array->free_list, array);\
        return;\
    }\
    free(array->data);\
    free(array);\
}


#define _darray_realloc(type)\
void type##_darray_realloc(type##_darray* array, u64 new_capacity)\
{\
    MASSERT_MSG(array, "DARRAY MACRO PUSH: NULL ARRAY");\
    if (array->free_list)\
    {\
        void* new_data = arena_free_list_alloc(array->free_list, new_capacity * array->stride);\
        memcpy(new_data, array->data, array->stride * array->num_items);\
        arena_free_list_free(array->free_list, array->data);\
        array->data = new_data;\
        return;\
    }\
    void* new_data = malloc(new_capacity * array->stride);\
    memcpy(new_data, array->data, array->stride * array->num_items);\
    free(array->data);\
    array->data = new_data;\
}

#define _darray_push(type)\
void type##_darray_push(type##_darray* array, const u8* data)\
{\
    MASSERT_MSG(array, "DARRAY MACRO PUSH: NULL ARRAY");\
    if (array->num_items >= array->capacity)\
    {\
        type##_darray_realloc(array, array->capacity * 2);\
    }\
    array->data[array->num_items] = *data;\
    array->num_items++;\
}\

#define _darray_pop(type)\
void type##_darray_pop(type##_darray* array)\
{\
    MASSERT_MSG(array, "DARRAY MACRO POP: NULL ARRAY");\
    if (array->num_items <= 0)\
    {\
        WARN("DARRAY MACRO POP: NOTHING TO POP")\
        return;\
    }\
    array->num_items--;\
}


#define _darray_clear(type)\
void type##_darray_clear(type##_darray* array)\
{\
    MASSERT_MSG(array, "DARRAY MACRO CLEAR: NULL ARRAY");\
    array->num_items = 0;\
}


#define _darray_zero_memory(type)\
void type##_darray_zero_memory(type##_darray* array)\
{\
    MASSERT_MSG(array, "DARRAY MACRO ZERO: NULL ARRAY");\
    memset(array->data, 0, array->stride * array->capacity);\
}

#define _darray_get_bytes_used(type)\
u64 type##_darray_get_bytes_used(type##_darray* array)\
{\
    MASSERT_MSG(array, "DARRAY MACRO GET BYTES USED: NULL ARRAY");\
    return array->num_items * array->stride;\
}

#define _darray_slice(type)\
typedef struct type##_darray_slice\
{\
    u64 length;\
    type##* ptr;\
} type##_darray_slice;\
type##_darray_slice type##_darray_slice_create(type##_darray* array, u64 slice_length)\
{\
    MASSERT(slice_length <= array->capacity);\
    return (type##_darray_slice){.ptr = array->data, .length = slice_length};\
}\
type##_darray_slice type##_darray_slice_create_offset(type##_darray* array, u64 slice_start,\
                                                                u64 slice_length)\
{\
    MASSERT(slice_start < array->capacity);\
    MASSERT(slice_length <= array->capacity);\
    MASSERT(slice_start + slice_length <= array->capacity);\
    return (type##_darray_slice){.ptr = array->data + slice_start, .length = slice_length};\
}





#define _darray_macro(type)\
     _darray_struct_macro(type)\
     _darray_create(type)\
     _darray_free(type)\
     _darray_realloc(type)\
     _darray_push(type)\
     _darray_pop(type)\
     _darray_clear(type)\
     _darray_zero_memory(type)\
    _darray_get_bytes_used(type)\
    _darray_slice(type)\
    \


#define DARRAY_GENERATE_TYPE(type) \
    _darray_macro(type)

DARRAY_GENERATE_TYPE(u8)

DARRAY_GENERATE_TYPE(u16)

DARRAY_GENERATE_TYPE(u32)

DARRAY_GENERATE_TYPE(u64)

DARRAY_GENERATE_TYPE(i8)

DARRAY_GENERATE_TYPE(i16)

DARRAY_GENERATE_TYPE(i32)

DARRAY_GENERATE_TYPE(i64)

DARRAY_GENERATE_TYPE(f32)

DARRAY_GENERATE_TYPE(f64)

DARRAY_GENERATE_TYPE(bool)


#endif
