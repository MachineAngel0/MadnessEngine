#ifndef STRING_ALLOCATOR_H
#define STRING_ALLOCATOR_H


#include <stdbool.h>

#include "defines.h"

//main idea is that we want to hand out string of the smallest size possible
//these are for string who's lifetimes may not be known

typedef struct String_Allocator
{
    bool not_yet;

} String_Allocator;



void string_allocator_init(String_Allocator* string_allocator, void* backing_memory,  size_t memory_size);

void* allocator_heap_alloc(String_Allocator* string_allocator,  size_t size);
void* allocator_heap_alloc_aligned(String_Allocator* string_allocator, size_t size, size_t alignment);


void allocator_heap_free(String_Allocator* string_allocator, void* ptr);
void allocator_heap_free_all(String_Allocator* string_allocator);

void allocator_heap_debug_print(String_Allocator* string_allocator);


void allocator_heap_test(void);


#endif //STRING_ALLOCATOR_H
