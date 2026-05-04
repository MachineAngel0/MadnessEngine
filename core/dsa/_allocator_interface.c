#include "_allocator_interface.h"

#include "asserts.h"


Allocator_Interface allocator_inferface_create(Allocator* allocator)
{
    MASSERT(allocator);
    return (Allocator_Interface){
        .alloc = allocator_interface_alloc,
        .free = allocator_interface_free,
        .allocator = allocator
    };
}

Allocator_Interface frame_allocator_interface_create(Frame_Allocator* allocator)
{
    MASSERT(allocator);
    return (Allocator_Interface){
        .alloc = allocator_interface_alloc,
        .free = allocator_interface_free,
        .allocator = allocator
    };
}

Allocator_Interface pool_allocator_interface_create(Pool_Allocator* allocator)
{
    MASSERT(allocator);
    return (Allocator_Interface){
        .alloc = pool_allocator_interface_alloc,
        .free = pool_allocator_interface_free,
        .allocator = allocator
    };
}

Allocator_Interface stack_allocator_interface_create(Stack_Allocator* allocator)
{
    MASSERT(allocator);
    return (Allocator_Interface){
        .alloc = stack_allocator_interface_alloc,
        .free = stack_allocator_interface_free,
        .allocator = allocator
    };
}

Allocator_Interface free_list_allocator_interface_create(Arena_Free_List* allocator)
{
    MASSERT(allocator);
    return (Allocator_Interface){
        .alloc = free_list_allocator_interface_alloc,
        .free = free_list_allocator_interface_free,
        .allocator = allocator
    };
}


