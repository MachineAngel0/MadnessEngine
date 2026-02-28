#ifndef MEMORY_SYSTEM_H
#define MEMORY_SYSTEM_H


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



MAPI void memory_system_init(Memory_System* memory_system, u64 memory_request_size);
//should be the last system shutdown
MAPI void memory_system_shutdown(Memory_System* memory_system);
//should only be called by larger subsystems like the renderer
MAPI void* memory_system_alloc(Memory_System* memory_system,u64 memory_request_size);


MAPI void memory_system_zero_memory(void* memory, u64 size);


MAPI void memory_system_copy(void* dest, void* source, u64 size);


MAPI void memory_system_set(void* dest, i32 val, u64 size);


#endif //MEMORY_SYSTEM_H
