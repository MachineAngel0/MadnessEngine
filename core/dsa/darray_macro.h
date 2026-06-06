#ifndef DARRAY_MACRO_H
#define DARRAY_MACRO_H

#include "allocator_stack.h"
#include "unit_test.h"
#include "_allocator_interface.h"
#include "../memory/memory_tracker.h"



//NOTE: NOT IN USE, AND I GENERALLY DECIDED AGAINST THIS APPROACH OF USING MACROS, MORE HEADACHES THAN ITS WORTH

/* Array layout
 * HEADER -> num_items, stride, capacity, allocator
 * void* data
 */



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

    //
    Memory_Tracker* memory_tracker;
} array_header;

void* _darray_create(const u64 capacity, const u64 stride, Memory_Tracker* memory_tracker);
void* _darray_create_arena(const u64 initial_capacity, const u64 stride, Stack_Allocator* arena,
                           Memory_Tracker* memory_tracker);

#define darray_create(type) \
    (type*) _darray_create(DARRAY_DEFAULT_CAPACITY, sizeof(type), NULL)

#define darray_create_reserve(type, capacity) \
    (type*) _darray_create(capacity, sizeof(type), NULL)

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


void darray_test(void);


void darray_macro_test();


#define _dynamic_array_struct_macro(type)\
typedef struct type##_dynamic_array{ \
    Allocator_Interface allocator;\
    u64 capacity; \
    u64 num_items;\
    type* data;\
}type##_dynamic_array;

#define _dynamic_array_create_macro(type)\
type##_dynamic_array* type##_dynamic_array_create(u64 capacity, Allocator_Interface allocator)\
{\
    MASSERT_MSG(capacity > 0, "DARRAY MACRO CREATE: INVALID SIZE");\
    MASSERT_MSG(allocator.allocator, "DARRAY MACRO INVALID ALLOCATOR");\
    MASSERT_MSG(allocator.alloc, "DARRAY MACRO INVALID ALLOC");\
    MASSERT_MSG(allocator.free_memory, "DARRAY MACRO INVALID FREE");\
    \
    type##_dynamic_array*  arr = allocator.alloc(allocator.allocator, sizeof(type##_dynamic_array), DEFAULT_ALIGNMENT);\
    arr->data = allocator.alloc(allocator.allocator, sizeof(type) * capacity, DEFAULT_ALIGNMENT);\
    arr->allocator = allocator;\
    arr->capacity = capacity;\
    arr->num_items = 0;\
    return arr;\
}

#define _dynamic_array_free_macro(type)\
void type##_dynamic_array_free(type##_dynamic_array* array)\
{\
    MASSERT_MSG(array, "DARRAY MACRO FREE: NULL ARRAY");\
    array->allocator.free_memory(array->allocator.allocator, array->data);\
    array->allocator.free_memory(array->allocator.allocator, array);\
}


#define _dynamic_array_realloc_macro(type)\
void type##_dynamic_array_resize(type##_dynamic_array* array, u64 new_capacity)\
{\
    MASSERT_MSG(array, "DARRAY MACRO PUSH: NULL ARRAY");\
    void* new_data = array->allocator.alloc(array->allocator.allocator, new_capacity * sizeof(type), DEFAULT_ALIGNMENT);\
    memcpy(new_data, array->data, sizeof(type) * array->num_items);\
    array->allocator.free_memory(array->allocator.allocator, array->data);\
    array->data = new_data;\
}

#define _dynamic_array_push_macro(type)\
void type##_dynamic_array_push(type##_dynamic_array* array, const type* data)\
{\
    MASSERT_MSG(array, "DARRAY MACRO PUSH: NULL ARRAY");\
    if (array->num_items >= array->capacity)\
    {\
        type##_dynamic_array_resize(array, array->capacity * 2);\
    }\
    memcpy((u8*)array->data + (sizeof(type) * array->num_items), data, sizeof(type));\
    array->num_items++;\
}
#define _dynamic_array_pop_macro(type)\
void type##_dynamic_array_pop(type##_dynamic_array* array)\
{\
    MASSERT_MSG(array, "DARRAY MACRO POP: NULL ARRAY");\
    if (array->num_items <= 0)\
    {\
        WARN("DARRAY MACRO POP: NOTHING TO POP")\
        return;\
    }\
    array->num_items--;\
}


#define _dynamic_array_clear_macro(type)\
void type##_dynamic_array_clear(type##_dynamic_array* array)\
{\
    MASSERT_MSG(array, "DARRAY MACRO CLEAR: NULL ARRAY");\
    array->num_items = 0;\
}


#define _dynamic_array_zero_memory_macro(type)\
void type##_dynamic_array_zero_memory(type##_dynamic_array* array)\
{\
    MASSERT_MSG(array, "DARRAY MACRO ZERO: NULL ARRAY");\
    memset(array->data, 0, sizeof(type) * array->capacity);\
}

#define _dynamic_array_get_bytes_used_macro(type)\
u64 type##_dynamic_array_get_bytes_used(type##_dynamic_array* array)\
{\
    MASSERT_MSG(array, "DARRAY MACRO GET BYTES USED: NULL ARRAY");\
    return array->num_items * sizeof(type);\
}

#define _dynamic_array_slice_macro(type)\
typedef struct type##_dynamic_array_slice\
{\
    u64 length;\
    type* ptr;\
} type##_dynamic_array_slice;\
type##_dynamic_array_slice type##_dynamic_array_slice_create(type##_dynamic_array* array, u64 slice_length)\
{\
    MASSERT(slice_length <= array->capacity);\
    return (type##_dynamic_array_slice){.ptr = array->data, .length = slice_length};\
}\
type##_dynamic_array_slice type##_dynamic_array_slice_create_offset(type##_dynamic_array* array, u64 slice_start,\
                                                                u64 slice_length)\
{\
    MASSERT(slice_start < array->capacity);\
    MASSERT(slice_length <= array->capacity);\
    MASSERT(slice_start + slice_length <= array->capacity);\
    return (type##_dynamic_array_slice){.ptr = array->data + slice_start, .length = slice_length};\
}

#define _dynamic_array_macro(type)\
     _dynamic_array_struct_macro(type)\
     _dynamic_array_create_macro(type)\
     _dynamic_array_free_macro(type)\
     _dynamic_array_realloc_macro(type)\
     _dynamic_array_push_macro(type)\
     _dynamic_array_pop_macro(type)\
     _dynamic_array_clear_macro(type)\
     _dynamic_array_zero_memory_macro(type)\
    _dynamic_array_get_bytes_used_macro(type)\
    _dynamic_array_slice_macro(type)

#define DARRAY_GENERATE_TYPE(type) \
    _dynamic_array_macro(type)

DARRAY_GENERATE_TYPE(u8)

DARRAY_GENERATE_TYPE(u16)

DARRAY_GENERATE_TYPE(u32)

DARRAY_GENERATE_TYPE(u64)

DARRAY_GENERATE_TYPE(s8)

DARRAY_GENERATE_TYPE(s16)

DARRAY_GENERATE_TYPE(s32)

DARRAY_GENERATE_TYPE(s64)

DARRAY_GENERATE_TYPE(f32)

DARRAY_GENERATE_TYPE(f64)

DARRAY_GENERATE_TYPE(bool)

DARRAY_GENERATE_TYPE(char)

DARRAY_GENERATE_TYPE(String)



void dynamic_array_macro_test()
{
    TEST_START("DARRAY MACRO");

    Allocator* allocator = malloc(sizeof(Allocator));
    size_t memory_size = MB(1);
    void* memory = malloc(memory_size);
    allocator_init(allocator, memory, memory_size);

    u64 test_capacity = 100;
    u8_dynamic_array* arr = u8_dynamic_array_create(test_capacity,allocator_inferface_create(allocator));


    TEST_DEBUG(arr->capacity == test_capacity);


    for (u64 i = 0; i < test_capacity; i++)
    {
        u8 val = (u8)(rand() % test_capacity);
        u8_dynamic_array_push(arr, &val);
    }

    TEST_DEBUG((sizeof(u8) * test_capacity) == u8_dynamic_array_get_bytes_used(arr));


    for (int i = 0; i < test_capacity; i++)
    {
        u8_dynamic_array_pop(arr);
    }

    for (u64 i = 0; i < test_capacity; i++)
    {
        u8 val = (u8)(rand() % test_capacity);
        u8_dynamic_array_push(arr, &val);
    }

    u8_dynamic_array_clear(arr);

    //TODO: test slices
    u8_dynamic_array_slice slice = u8_dynamic_array_slice_create(arr, test_capacity / 2);

    for (u64 i = 0; i < slice.length; i++)
    {
        TEST_DEBUG(slice.ptr[i] == slice.ptr[i]);
    }

    free(memory);
    free(allocator);

    TEST_END("DARRAY MACRO");
}

/*
//another type of macro version where you inline the procedure,
//but i dont really like it, weak type safety, but it does allow you to use something like char*
#define vector(T, Name) \
typedef struct {\
T* data; \
size_t length; \
size_t capacity; \
}Name;

#define vector_create(v, intial_capacity){\
v = malloc(sizeof(v));\
v->data = malloc(sizeof(v->data) * intial_capacity);\
v->length = 0;\
v->capacity = initial_capacity;\
}
#define vector_resize(v, new_capacity){\
    v->data = realloc(v->data, v->capacity*2);\
}

#define vector_append(v, element) ({ \
v->data[v->length] = (element); \
v->length++; \
})

#define vector_pop(v) ({ \
v->length--; \
})

#define vector_clear(v) { \
free((v)->data); \
(v)->data = NULL; \
(v)->capacity = 0; \
(v)->length = 0; \
}

typedef vector(char*, Vector_str);


void inline_macro_testing()
{
    Vector_str* v = {0};
    int initial_capacity = 10;
    vector_create(v, initial_capacity);
    vector_resize(v);
    vector_append(v, "abc");
    vector_clear(v);
}
*/


#endif //DARRAY_MACRO_H
