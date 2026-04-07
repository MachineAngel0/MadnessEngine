#ifndef MEMORY_SYSTEM_H
#define MEMORY_SYSTEM_H

#include "arena_freelist.h"
#include "memory_tracker.h"

typedef enum memory_subsystem_type
{
    MEMORY_SUBSYSTEM_INPUT,
    MEMORY_SUBSYSTEM_GAMEPAD,
    MEMORY_SUBSYSTEM_EVENT,
    MEMORY_SUBSYSTEM_RENDERER,
    MEMORY_SUBSYSTEM_AUDIO,
    MEMORY_SUBSYSTEM_GAME,
    MEMORY_SUBSYSTEM_RESOURCE,
    MEMORY_SUBSYSTEM_SPRITE,
    MEMORY_SUBSYSTEM_TEXTURE,
    MEMORY_SUBSYSTEM_MESH,
    MEMORY_SUBSYSTEM_UI,


    MEMORY_SUBSYSTEM_MAX,
} Memory_Subsystem_Type;

static const char* memory_subsystem_type_string[MEMORY_SUBSYSTEM_MAX] =
{
    [MEMORY_SUBSYSTEM_INPUT] = "MEMORY_SUBSYSTEM_INPUT",
    [MEMORY_SUBSYSTEM_GAMEPAD] = "MEMORY_SUBSYSTEM_GAMEPAD",
    [MEMORY_SUBSYSTEM_EVENT] = "MEMORY_SUBSYSTEM_EVENT",
    [MEMORY_SUBSYSTEM_RENDERER] = "MEMORY_SUBSYSTEM_RENDERER",
    [MEMORY_SUBSYSTEM_AUDIO] = "MEMORY_SUBSYSTEM_AUDIO",
    [MEMORY_SUBSYSTEM_GAME] = "MEMORY_SUBSYSTEM_GAME",
    [MEMORY_SUBSYSTEM_RESOURCE] = "MEMORY_SUBSYSTEM_RESOURCE",
    [MEMORY_SUBSYSTEM_SPRITE] = "MEMORY_SUBSYSTEM_SPRITE",
    [MEMORY_SUBSYSTEM_TEXTURE] = "MEMORY_SUBSYSTEM_TEXTURE",
    [MEMORY_SUBSYSTEM_MESH] = "MEMORY_SUBSYSTEM_MESH",
    [MEMORY_SUBSYSTEM_UI] = "MEMORY_SUBSYSTEM_UI",
};


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
    Arena_Free_List application_arena_free_list;

    u64 memory_subsystem_usage[MEMORY_SUBSYSTEM_MAX];

    //TODO: likely have to create a free list, with this as a backup just to clear memory on resets
    Memory_Tracker_System* memory_tracker_system;
} Memory_System;


MAPI void memory_system_init(Memory_System* memory_system, u64 memory_request_size);

//should be the last system shutdown
MAPI void memory_system_shutdown(Memory_System* memory_system);

//should only be called by larger subsystems like the renderer, game, or resource system
MAPI void* memory_system_alloc(Memory_System* memory_system, u64 memory_request_size, Memory_Subsystem_Type memory_subsystem_type);
MAPI void memory_system_memory_free(Memory_System* memory_system, void* memory_block);

//hand out memory trackers
MAPI void memory_system_print_subsystem_memory_usage(Memory_System* memory_system);
MAPI void memory_system_print_all_memory_usage(Memory_System* memory_system);


MAPI void memory_system_zero_memory(void* memory, u64 size);

MAPI void memory_system_copy(void* dest, void* source, u64 size);

MAPI void memory_system_set(void* dest, i32 val, u64 size);


#endif //MEMORY_SYSTEM_H
