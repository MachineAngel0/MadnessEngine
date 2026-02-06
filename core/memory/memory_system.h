#ifndef MEMORY_SYSTEM_H
#define MEMORY_SYSTEM_H


#include "platform.h"

//TODO: if I feel like this is necessary, this would replace the params in the init function
typedef struct
{
    //how much memory we want to use of this application
    u64 memory_request_size;

    bool use_file_config;
    const char* file_config;
} Memory_System_Config;


typedef struct Memory_System
{
    // Memory_System_Config mem_config; // a copy of the data
    Arena application_arena;
} Memory_System;


static Memory_System memory_system;


void memory_system_init(u64 memory_request_size)
{
    void* memory = platform_allocate(memory_request_size, true);
    //this should absolutely hard crash
    MASSERT_MSG(memory, "memory_system_init: FAILED TO ALLOCATE MEMORY FOR THE APPLICATION ");
    arena_init(&memory_system.application_arena, memory, memory_request_size, MEMORY_SUBSYSTEM_APPLICATION_ARENA);
}

//should be the last system shutdown
void memory_system_shutdown()
{
    // arena_clear(memory_system.application_arena);
    platform_free(memory_system.application_arena.memory);
}

//should only be called by larger subsystems like the renderer
void* memory_system_alloc(const u64 memory_request_size)
{
    return arena_alloc(&memory_system.application_arena, memory_request_size);
}


void ms_memory_zero(void* memory, u64 size)
{
    platform_zero_memory(memory, size);
}

void ms_copy(void* dest, void* source, u64 size)
{
    platform_copy_memory(dest, source, size);
}

void ms_set(void* dest, i32 val, u64 size)
{
    platform_set_memory(dest, val, size);
}



#endif //MEMORY_SYSTEM_H
