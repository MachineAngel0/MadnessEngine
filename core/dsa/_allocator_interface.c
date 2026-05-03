#include "_allocator_interface.h"


Allocator_Interface allocator_inferface_create(Allocator* allocator)
{
    return (Allocator_Interface){
        .alloc = allocator_interface_alloc,
        .free = allocator_interface_free,
        .allocator = allocator
    };
}

Allocator_Interface frame_allocator_interface_create(Frame_Allocator* allocator)
{
    return (Allocator_Interface){
        .alloc = allocator_interface_alloc,
        .free = allocator_interface_free,
        .allocator = allocator
    };
}

Allocator_Interface pool_allocator_interface_create(Pool_Allocator* allocator)
{
    return (Allocator_Interface){
        .alloc = pool_allocator_interface_alloc,
        .free = pool_allocator_interface_free,
        .allocator = allocator
    };
}

Allocator_Interface stack_allocator_interface_create(Stack_Allocator* allocator)
{
    return (Allocator_Interface){
        .alloc = stack_allocator_interface_alloc,
        .free = stack_allocator_interface_free,
        .allocator = allocator
    };
}

Allocator_Interface free_list_allocator_interface_create(Arena_Free_List* allocator)
{
    return (Allocator_Interface){
        .alloc = free_list_allocator_interface_alloc,
        .free = free_list_allocator_interface_free,
        .allocator = allocator
    };
}


