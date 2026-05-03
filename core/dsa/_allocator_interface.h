#ifndef ALLOCATOR_INTERFACE_H
#define ALLOCATOR_INTERFACE_H

#include "defines.h"
#include "allocator_freelist.h"
#include "allocator_pool.h"
#include "allocator_stack.h"
#include "allocator.h"



typedef struct Allocator_Interface
{
    void*(*alloc)(void* allocator, u64 memory_byte_request, u8 alignment);
    void (*free)(void* allocator, void* memory_block);
    void* allocator;
} Allocator_Interface;


Allocator_Interface allocator_inferface_create(Allocator* allocator);
Allocator_Interface frame_allocator_interface_create(Frame_Allocator* allocator);
Allocator_Interface pool_allocator_interface_create(Pool_Allocator* allocator);
Allocator_Interface stack_allocator_interface_create(Stack_Allocator* allocator);
Allocator_Interface free_list_allocator_interface_create(Arena_Free_List* free_list);


#endif //ALLOCATOR_INTERFACE_H
