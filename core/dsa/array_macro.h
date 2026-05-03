#ifndef ARRAY_NEW_H
#define ARRAY_NEW_H

#include <stdlib.h>
#include "logger.h"
#include "memory/memory_tracker.h"
#include "allocator.h"


#define _array_struct_macro(type)\
    typedef struct type##_array{ \
    u64 capacity; \
    u64 stride; \
    u64 num_items;\
    type* data;\
    bool using_arena;\
    }type##_array;

#define _array_create_macro(type)\
    type##_array* type##_array_create(u64 capacity, Allocator* arena) \
    { \
        if (capacity <= 0)\
        {\
            WARN("ARRAY MACRO CREATE: INVALID CAPACITY")\
            return NULL;\
        } \
        type##_array* arr; \
        if (arena)\
        {\
            arr = allocator_alloc(arena, sizeof(type##_array));\
            arr->data = allocator_alloc(arena, sizeof(type) * capacity); \
            arr->using_arena = true;\
        }\
        else\
        {\
            arr = malloc(sizeof(type##_array));\
            memset(arr, 0, sizeof(type##_array));\
            arr->data = malloc(sizeof(type) * capacity); \
            memset(arr->data, 0, sizeof(u8) * capacity);\
            arr->using_arena = false;\
         \
        }\
        arr->capacity = capacity;\
        arr->stride = sizeof(type); \
        arr->num_items = 0; \
        return arr; \
    };

#define _array_free_macro(type)\
    void type##_array_free(type##_array* array)\
        {\
        MASSERT_MSG(array, "ARRAY MACRO FREE: NULL ARRAY")\
        if (array->using_arena)\
            {\
                return;\
            }\
        free(array->data);\
        free(array);\
        };



#define _array_push_macro(type)\
    void type##_array_push(type##_array* array, const type* data) \
    {\
        MASSERT_MSG(array, "ARRAY MACRO PUSH: NULL ARRAY");\
        if (array->num_items >= array->capacity)\
        {\
            WARN("ARRAY MACRO PUSH: NULL ARRAY")\
            return;\
        }\
        array->data[array->num_items] = *data;\
        array->num_items++;\
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
        memset(array->data, 0, array->stride * array->capacity);\
    }
#define _array_get_bytes_used(type)\
    u64 type##_array_get_bytes_used(type##_array* array)\
    {\
        MASSERT_MSG(array, "ARRAY MACRO GET BYTES USED: NULL ARRAY");\
        return array->num_items * array->stride;\
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
    _array_create_macro(type);\
    _array_free_macro(type)\
    _array_push_macro(type)\
    _array_pop_macro(type)\
    _array_clear_macro(type)\
    _array_zero_macro(type)\
    _array_get_bytes_used(type) \
    _array_slice_macro(type)\


//TODO: functions
// free, get, set, print, remove and remove_swap_back, sort function


ARRAY_GENERATE_TYPE(u8)
ARRAY_GENERATE_TYPE(u16)
ARRAY_GENERATE_TYPE(u32)
ARRAY_GENERATE_TYPE(u64)

ARRAY_GENERATE_TYPE(i8)
ARRAY_GENERATE_TYPE(i16)
ARRAY_GENERATE_TYPE(i32)
ARRAY_GENERATE_TYPE(i64)

ARRAY_GENERATE_TYPE(f32)
ARRAY_GENERATE_TYPE(f64)

ARRAY_GENERATE_TYPE(bool)


void array_macro_test();


#endif
