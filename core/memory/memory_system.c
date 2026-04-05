#include "memory_system.h"


void memory_system_init(Memory_System* memory_system, u64 memory_request_size)
{
    MASSERT(memory_system);
    memset(memory_system, 0, sizeof(Memory_System));
    void* memory = platform_allocate(memory_request_size, true);
    //this should absolutely hard crash
    MASSERT_MSG(memory, "memory_system_init: FAILED TO ALLOCATE MEMORY FOR THE APPLICATION ");


    //TODO: if freeing memory actually becomes a concern then we can replace this with a freelist allocator
    arena_free_list_init(&memory_system->application_arena_free_list, memory, memory_request_size);

    memory_system->memory_tracker_system = arena_free_list_alloc(&memory_system->application_arena_free_list, sizeof(Memory_Tracker_System));
    memory_system->memory_tracker_system->memory_tracker = arena_free_list_alloc(&memory_system->application_arena_free_list, sizeof(Memory_Tracker) * DEFAULT_MEMORY_TRACKERS_COUNT);
    memory_system->memory_tracker_system->capacity = DEFAULT_MEMORY_TRACKERS_COUNT;
    memory_system->memory_tracker_system->size = 0;
    INFO("MEMORY SYSTEM SUCCESSFULLY ALLOCATED")
}

//should be the last system shutdown
void memory_system_shutdown(Memory_System* memory_system)
{
    MASSERT(memory_system);

    //print how much memory we used with all the subsystems


    //nice thing to have to about memory allocations throught out the applications lifetime
    INFO("APP MEMORY INFO: Current amount allocated %llu, Total Amount Available: %llu, Amount Left: %llu",
         memory_system->application_arena_free_list.used,
         memory_system->application_arena_free_list.capacity,
         (memory_system->application_arena_free_list.capacity - memory_system->application_arena_free_list.used))

    arena_free_list_free_all(&memory_system->application_arena_free_list);
    platform_free(memory_system->application_arena_free_list.data);

    INFO("MEMORY SYSTEM SHUTDOWN");
}

//should only be called by larger subsystems like the renderer
void* memory_system_alloc(Memory_System* memory_system, u64 memory_request_size)
{
    MASSERT(memory_system);
    return arena_free_list_alloc(&memory_system->application_arena_free_list, memory_request_size);
}

void memory_system_memory_free(Memory_System* memory_system, void* memory_block)
{
    MASSERT(memory_system);
    MASSERT(memory_block);

    arena_free_list_free(&memory_system->application_arena_free_list, memory_block);

}


void memory_system_print_all_memory_usage(Memory_System* memory_system)
{
    memory_tracker_system_print_all_memory_usage(memory_system->memory_tracker_system);
}

void memory_system_zero_memory(void* memory, u64 size)
{
    platform_zero_memory(memory, size);
}

void memory_system_copy(void* dest, void* source, u64 size)
{
    platform_copy_memory(dest, source, size);
}

void memory_system_set(void* dest, i32 val, u64 size)
{
    platform_set_memory(dest, val, size);
}




