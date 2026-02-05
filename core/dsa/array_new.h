#ifndef ARRAY_NEW_H
#define ARRAY_NEW_H

#include <stdio.h>
#include <stdlib.h>
#include "logger.h"
#include "misc_util.h"

#include "unit_test.h"
#include "arena_stack.h"
#include "memory_tracker.h"



/* Array layout
 * HEADER -> num_items, stride, capacity, allocator
 * void* data
 */

typedef struct array_new_header
{
    //this header will be located at the front of the array
    u64 num_items; // length or top index in our array
    u64 stride; // size of each void* data
    u64 capacity; // max elements allowed in the array
    //TODO: allocator
} array_new_header;

void* array_new_create(const u64 capacity, const u64 stride)
{
    const u64 header_size = sizeof(array_new_header);
    const u64 array_size = capacity * stride;
    void* new_array = malloc(header_size + array_size);
    if (!new_array)
    {
        MASSERT("array_new CREATE, MALLOC FAILED");
        return NULL;
    }
    memset(new_array, 0, header_size + array_size);

    memory_container_alloc(MEMORY_CONTAINER_ARRAY, header_size + array_size);


    array_new_header* header = new_array;
    header->num_items = 0;
    header->stride = stride;
    header->capacity = capacity;

    //return the array without the header in it, so that we can use it like a normal array
    return (void *) ((u8 *) new_array + header_size);
}

void* array_new_arena(const u64 intial_capacity, const u64 stride, Arena_Stack* arena)
{
    const u64 header_size = sizeof(array_new_header);
    const u64 array_size = intial_capacity * stride;
    void* new_array = arena_stack_alloc(arena, header_size + array_size);;
    memset(new_array, 0, header_size + array_size);

    memory_container_alloc(MEMORY_CONTAINER_ARRAY, header_size + array_size);

    array_new_header* header = new_array;
    header->num_items = 0;
    header->stride = stride;
    header->capacity = intial_capacity;

    //return the array without the header in it, so that we can use it like a normal array
    return (void *) ((u8 *) new_array + header_size);
}

#define array_new_create(type) \
    (type*) _array_new_create(array_new_DEFAULT_CAPACITY, sizeof(type))

#define array_new_create_reserve(type, capacity) \
    (type*) _array_new_create(capacity, sizeof(type))

void array_new_free(void* array)
{
    if (!array)
    {
        WARN("array_new FREE: NULL ARRAY");
        return;
    }
    u64 header_size = sizeof(array_new_header);


    //get the array header
    array_new_header* header = (array_new_header *) ((u8 *) array - header_size);

    memory_container_free(MEMORY_CONTAINER_ARRAY, header->capacity + header_size);

    free(header);
    array = NULL;
    header = NULL;
}

u64 array_new_get_capacity(void* array)
{
    if (!array)
    {
        WARN("array_new EMPTY: Invalid Array");
        return false;
    }
    //get the array header
    u64 header_size = sizeof(array_new_header);
    array_new_header* header = (array_new_header *) ((u8 *) array - header_size);

    return header->capacity;
}

u64 array_new_get_size(void* array)
{
    if (!array)
    {
        WARN("array_new EMPTY: Invalid Array");
        return false;
    }
    //get the array header
    u64 header_size = sizeof(array_new_header);
    array_new_header* header = (array_new_header *) ((u8 *) array - header_size);

    return header->num_items;
}

u64 array_new_get_num_count(void* array)
{
    return array_new_get_size(array);
}


u64 array_new_get_stride(void* array)
{
    if (!array)
    {
        WARN("array_new EMPTY: Invalid Array");
        return false;
    }
    //get the array header
    u64 header_size = sizeof(array_new_header);
    array_new_header* header = (array_new_header *) ((u8 *) array - header_size);

    return header->stride;
}

bool array_new_is_valid_index(void* array, const u64 index)
{
    if (!array)
    {
        WARN("array_new EMPTY: Invalid Array");
        return false;
    }
    //get the array header
    u64 header_size = sizeof(array_new_header);
    array_new_header* header = (array_new_header *) ((u8 *) array - header_size);

    return header->num_items >= index;
}

bool array_new_is_empty(void* array)
{
    if (!array)
    {
        WARN("array_new EMPTY: Invalid Array");
        return false;
    }
    //get the array header
    u64 header_size = sizeof(array_new_header);
    array_new_header* header = (array_new_header *) ((u8 *) array - header_size);

    return header->num_items == 0;
}


//NOTE: get and set are not technically needed
// but they provide a nice layer of making sure the index is valid, which is nice debugging
void* array_new_get(void* array, const u64 index)
{
    //get the array header
    u64 header_size = sizeof(array_new_header);
    array_new_header* header = (array_new_header *) ((u8 *) array - header_size);

    //make sure were not accessing invalid memory
    if (index > header->capacity)
    {
        M_ERROR("array_new GET: INDEX OUT OF BOUNDS");
        return NULL;
    }

    //return the locations value
    return (void *) ((u8) array + (header->stride * index));
}

void array_new_set(void* array, void* new_data, const u64 index)
{
    //get the array header
    u64 header_size = sizeof(array_new_header);
    array_new_header* header = (array_new_header *) ((u8 *) array - header_size);

    //make sure were not accessing invalid memory
    if (index > header->capacity)
    {
        M_ERROR("array_new GET:  INDEX OUT OF BOUNDS");
    }

    //return the locations value
    memcpy(((u8 *) array + (header->stride * index)), new_data, header->stride);
}

void* _array_new_push(void* array, const void* new_data)
{
    if (!array)
    {
        WARN("array_new PUSH: NULL ARRAY");
        return NULL;
    }

    //get the array header
    u64 header_size = sizeof(array_new_header);
    array_new_header* header = (array_new_header *) ((u8 *) array - header_size);

    //check for size overflow, and resize if needed
    if (header->num_items >= header->capacity)
    {
        array = _array_new_resize(array);
        header = (array_new_header *) ((u8 *) array - header_size);
    }

    //go to the end of the array
    u64 addr = (u64) array;
    addr += header->num_items * header->stride;
    //copy data onto the end of the array that is not in use
    memcpy((void *) addr, new_data, header->stride);
    header->num_items++;
    return array;
}

//lets us use non stack values
#define array_new_push(array, value)                          \
{                                                          \
    __typeof__(value) temp_value = value;         \
    array = _array_new_push(array, &temp_value); \
}

void* _array_new_push_range(void* array, const void* new_data, u64 new_items_count)
{
    if (!array)
    {
        WARN("array_new PUSH: NULL ARRAY");
        return NULL;
    }

    //get the array header
    u64 header_size = sizeof(array_new_header);
    array_new_header* header = (array_new_header *) ((u8 *) array - header_size);

    //check for size overflow, and resize if needed
    if (header->num_items >= header->capacity)
    {
        // INFO("RESIZING array_new")
        array = _array_new_resize(array);
        header = (array_new_header *) ((u8 *) array - header_size);
    }

    //go to the end of the array
    u64 end_addr = (u64) array;
    end_addr += header->num_items * header->stride;
    //copy data onto the end of the array that is not in use
    memcpy((void *) end_addr, new_data, header->stride * new_items_count);
    header->num_items += new_items_count;
    return array;
}

//lets us use non stack values
#define array_new_push_range(array, value, new_items_count)   \
{                                                          \
    __typeof__(value) temp_value = value;         \
    array = _array_new_push_range(array, &temp_value, new_items_count); \
}

void* _array_new_insert_at(void* array, const void* new_data, const u64 index)
{
    u64 header_size = sizeof(array_new_header);
    array_new_header* header = (array_new_header *) ((u8 *) array - header_size);

    u64 size = header->num_items;
    u64 stride = header->stride;
    if (index >= size)
    {
        M_ERROR("Index outside the bounds of this array! Length: %llu, index: %llu", size, index);
        return array;
    }
    if (size >= header->capacity)
    {
        array = _array_new_resize(array);
    }

    u64 addr = (u64) array;

    // Push element(s) from index forward out by one. This should
    // even happen if inserted at the last index.
    memcpy((void *) (addr + ((index + 1) * stride)),
           (void *) (addr + (index * stride)),
           stride * (size - index));

    // Set the value at the index
    memcpy((void *) (addr + (index * stride)), new_data, stride);
    header->num_items++;

    return array;
}

//lets us use non stack values
#define array_new_insert_at(array, value, index)                    \
{                                                           \
    __typeof__(value) temp_value = value;                   \
    array = _array_new_insert_at(array, &temp_value, index);   \
}

void array_new_pop(void* array)
{
    if (!array)
    {
        WARN("array_new POP: NULL ARRAY");
        return;
    }

    //get the array header
    u64 header_size = sizeof(array_new_header);
    array_new_header* header = (array_new_header *) ((u8 *) array - header_size);

    if (header->num_items <= 0)
    {
        WARN("array_new POP: Nothing to pop");
        return;
    }
    //we just decrement the value and that's it
    header->num_items--;
}
void* array_new_pop_return(void* array)
{
    if (!array)
    {
        WARN("array_new POP: NULL ARRAY");
        return NULL;
    }

    //get the array header
    u64 header_size = sizeof(array_new_header);
    array_new_header* header = (array_new_header *) ((u8 *) array - header_size);

    if (header->num_items <= 0)
    {
        WARN("array_new POP: Nothing to pop");
        return NULL;
    }
    //we just decrement the value and that's it
    u64 back = header->num_items;
    header->num_items--;


    return ((u8*)array + (back * header->stride));
}




void array_new_remove_shift(void* arr, const u64 index)
{
    //shift the array left from the index spot of removal
    //maintains order

    //get the array header
    u64 header_size = sizeof(array_new_header);
    array_new_header* header = (array_new_header *) ((u8 *) arr - header_size);
    const u64 stride = header->stride;

    memcpy((u8 *) arr + (index * stride),
           (u8 *) arr + ((index + 1) * stride),
           stride * header->num_items - (index + 1));

    header->num_items--;
}

void array_new_remove_swap(void* array, const u64 index)
{
    //replaces the index value with the last item in the array, then decrementing the array num count
    //does not maintain order

    //get the array header
    u64 header_size = sizeof(array_new_header);
    array_new_header* header = (array_new_header *) ((u8 *) array - header_size);

    //copy the last val into the cur index
    memcpy(((u8 *) array + (index * header->stride)),
           ((u8 *) array + (header->stride * (header->num_items - 1))),
           header->stride
    );

    header->num_items--;
}

bool array_new_contains(void* array,  void* key, int (*cmp_func)( void*,  void*))
{
    if (!array)
    {
        M_ERROR("array_new CONTAINS: NULL ARRAY");
        return false;
    }

    //get the array header
    u64 header_size = sizeof(array_new_header);
    array_new_header* header = (array_new_header *) ((u8 *) array - header_size);

    //check if we have the value, otherwise return false
    for (int i = 0; i < header->num_items; i++)
    {
        //0 means they are equal to each other
        if (cmp_func(((u8 *) (array ) + (i * header->stride)), key) == 0)
        {
            return true;
        }
    }
    return false;
}


bool array_new_contains_or_add(void* array, void* key, int (*cmp_func)(const void*, const void*))
{
    //return tells us if we added to the array or not

    // check if the value exists, if it doesn't then add it
    if (!array)
    {
        M_ERROR("array_new CONTAINS OR ADD: NULL ARRAY");
        return false;
    }

    //get the array header
    u64 header_size = sizeof(array_new_header);
    array_new_header* header = (array_new_header *) ((u8 *) array - header_size);

    //check if we have the value, otherwise return false
    for (int i = 0; i < header->num_items; i++)
    {
        void* element = (u8*)array + (i * header->stride);
        //0 means they are equal to each other, which means we can exit
        if (cmp_func(element, key) == 0)
        {
            return false;
        }
    }

    //if we reach this point the value is not in the array
   array_new_push(array, key);
    return true;

}


bool array_new_contains_or_add_other(void* array, void* key)
{
    //return tells us if we added to the array or not

    // check if the value exists, if it doesn't then add it
    if (!array)
    {
        M_ERROR("array_new CONTAINS OR ADD: NULL ARRAY");
        return false;
    }

    //get the array header
    u64 header_size = sizeof(array_new_header);
    array_new_header* header = (array_new_header *) ((u8 *) array - header_size);

    //check if we have the value, otherwise return false
    for (int i = 0; i < header->num_items; i++)
    {
        void* element = (u8*)array + (i * header->stride);
        //0 means they are equal to each other, which means we can exit
        if (memcmp(element, key, header->stride) == 0)
        {
            return false;
        }
    }

    //if we reach this point the value is not in the array
    array_new_push(array, key);
    return true;

}

#define STBDS_ADDRESSOF(typevar, value)     ((__typeof__(typevar)[1]){value}) // literal array decays to pointer to value
#define array_new_contains_or_add_test(array, value)    \
{                                                     \
    temp_value = (void*) STBDS_ADDRESSOF((array, (value)) \
    array_new_contains_or_add_other(array, temp_value); \
}



void* _array_new_duplicate(const void* array)
{
    u64 header_size = sizeof(array_new_header);
    array_new_header* source_header = (array_new_header *) ((u8 *) array - header_size);

    void* copy = _array_new_create(source_header->capacity, source_header->stride);
    array_new_header* new_header = (array_new_header *) ((u8 *) copy - header_size);
    MASSERT_MSG(new_header->capacity == source_header->capacity, "capacity mismatch while duplicating array_new.");

    // Copy internal header fields.
    new_header->stride = source_header->stride;
    new_header->num_items = source_header->num_items;

    // Copy internal memory.
    memcpy(copy, array, new_header->capacity * new_header->stride);

    return copy;
}


void array_new_clear(void* array)
{
    if (!array)
    {
        WARN("array_new POP: NULL ARRAY");
        return;
    }

    //get the array header
    u64 header_size = sizeof(array_new_header);
    array_new_header* header = (array_new_header *) ((u8 *) array - header_size);

    if (header->num_items <= 0)
    {
        WARN("array_new POP: Nothing to pop");
        return;
    }
    //just set it to zero and that is it
    header->num_items = 0;
}


void array_new_debug_header(void* array)
{
    if (!array)
    {
        WARN("array_new DEBUG HEADER: NULL ARRAY");
        return;
    }

    u64 header_size = sizeof(array_new_header);
    array_new_header* header = (array_new_header *) ((u8 *) array - header_size);
    DEBUG("CAPACITY: %llu, STRIDE: %llu, ITEMS #: %llu", array_new_get_capacity(array), array_new_get_stride(array),
          array_new_get_size(array));
    if (!header)
    {
        WARN("array_new DEBUG HEADER: INVALID ARRAY");
    }
}


void array_new_debug_print(void* array, void (*print_func)(void*))
{
    if (!array)
    {
        WARN("array_new POP: NULL ARRAY");
        return;
    }

    //get the array header
    u64 header_size = sizeof(array_new_header);
    array_new_header* header = (array_new_header *) ((u8 *) array - header_size);

    for (u64 i = 0; i < header->num_items; i++)
    {
        print_func((u8 *) array + (i * header->stride));
    }
    printf("\n");
}

void array_new_debug_print_range(void* array, size_t start, size_t end, void (*print_func)(void*))
{
    if (!array)
    {
        WARN("array_new POP: NULL ARRAY");
        return;
    }


    //get the array header
    u64 header_size = sizeof(array_new_header);
    array_new_header* header = (array_new_header *) ((u8 *) array - header_size);

    //technically there should be a check to make sure the values are not zero but seriouly come on
    if (start > end)
    {
        WARN("array_new PRINT RANGE: START IS GREATER THAN END");
        return;
    }
    if (start > header->num_items)
    {
        WARN("array_new PRINT RANGE: START IS GREATER ARRAY SIZE");
        return;
    }
    if (end > header->num_items)
    {
        WARN("array_new PRINT RANGE: END IS GREATER ARRAY SIZE");
        return;
    }


    for (u64 i = start; i < end; i++)
    {
        print_func((u8 *) array + (i * header->stride));
    }
    printf("\n");
}

//TODO: one of them whenever I need it
// void array_new_counting_sort(void* array);
// void array_new_merge_sort(void* array);
// void array_new_radix_sort(void* array);


void _array_new_test()
{
    TEST_START(array_new);

    int* arr = array_new_create(int);
    array_new_push(arr, 10);
    array_new_push(arr, 15);
    TEST_INFORM(array_new_get_size(arr) == 2);
    TEST_INFORM(array_new_get_capacity(arr) == 2);
    TEST_INFORM(array_new_get_stride(arr) == 4);
    array_new_debug_print(arr, print_int);
    TEST_INFORM(arr[0] == 10);
    TEST_INFORM(arr[1] == 15);
    TEST_INFORM(array_new_get_size(arr) == 2);

    array_new_pop(arr);
    TEST_INFORM(array_new_get_size(arr) == 1);
    TEST_INFORM(array_new_get_capacity(arr) == 2);
    TEST_INFORM(array_new_get_stride(arr) == 4);

    array_new_debug_print(arr, print_int);

    int a = 50;
    array_new_set(arr, &a, 0);
    arr[0] = 40;
    array_new_debug_print(arr, print_int);
    printf("%d\n", arr[0]);

    array_new_push(arr, 20);
    array_new_push(arr, 30);
    array_new_push(arr, 50);
    array_new_push(arr, 80);
    array_new_debug_print(arr, print_int);
    array_new_remove_shift(arr, 2);
    array_new_debug_print(arr, print_int);

    array_new_push(arr, 100);
    array_new_push(arr, 456);
    array_new_push(arr, 91);
    array_new_debug_print(arr, print_int);
    array_new_remove_swap(arr, 2);
    array_new_debug_print(arr, print_int);

    int contain_num1 = 456;
    int contain_num2 = 80;
    int contain_num2_invalid = 99;
    TEST_INFORM(array_new_contains(arr, &contain_num1, cmp_int));
    TEST_INFORM(!array_new_contains(arr, &contain_num2_invalid, cmp_int));
    TEST_INFORM(array_new_contains(arr, &contain_num2, cmp_int));


    array_new_pop(arr);
    array_new_debug_print(arr, print_int);


    array_new_free(arr); // i cant check this, it can't be nulled

    /*
    vec2* vec2_arr = array_new_create(vec2);
    vec2 tea;
    tea.x = 420.0f;
    tea.y = 69.0f;
    vec2 tea2;
    tea2.x = 128.0f;
    tea2.y = 298.0f;
    array_new_push(vec2_arr, tea);
    array_new_push(vec2_arr, tea2);
    array_new_debug_print(vec2_arr, print_vec2);
*/


    TEST_REPORT(array_new);
}

#endif
