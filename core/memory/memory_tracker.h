#ifndef MEMORY_TRACKER_H
#define MEMORY_TRACKER_H

#include "defines.h"
#include "str.h"

typedef enum memory_container_type
{
    MEMORY_CONTAINER_ARENA,
    MEMORY_CONTAINER_ARENA_STACK,
    MEMORY_CONTAINER_ARENA_POOl,
    MEMORY_CONTAINER_ARENA_FREELIST,
    MEMORY_CONTAINER_ARRAY,
    MEMORY_CONTAINER_DARRAY,
    MEMORY_CONTAINER_STRING,
    MEMORY_CONTAINER_HASHSET,
    MEMORY_CONTAINER_HASHMAP,
    MEMORY_CONTAINER_QUEUE,
    MEMORY_CONTAINER_STACK,


    MEMORY_CONTAINER_MAX,
} memory_container_type;

static const char* memory_container_type_string[MEMORY_CONTAINER_MAX] =
{
    "MEMORY_ARENA",
    "MEMORY_ARENA_STACK",
    "MEMORY_ARENA_POOl",
    "MEMORY_ARENA_FREELIST",
    "MEMORY_ARRAY",
    "MEMORY_DARRAY",
    "MEMORY_STRING",
    "MEMORY_HASHSET",
    "MEMORY_HASHMAP",
    "MEMORY_QUEUE",
    "MEMORY_STACK",
};


//NOTE: the design is not to track how much any given container has been allocated,
// we want to know how much a given system/subsystem was given memory wise,
// and how much the containers have allocated so far, to make better memory decisions
// the containers themselves will tell us if we overallocate

typedef struct Memory_Tracker
{
    String system_name;
    //keep track of the max memory the system is allowed to use
    u64 subsystem_memory_size;
    //keep track of memory in use from the containers
    u64 memory_container_usage[MEMORY_CONTAINER_MAX];

    //TODO: max_allocated_amount????
} Memory_Tracker;

#define DEFAULT_MEMORY_TRACKERS_COUNT 20

typedef struct Memory_Tracker_System
{
    //an array of these hand these out to the subsystems that need them for their own memory tracking
    Memory_Tracker* memory_tracker;
    u32 size; // how many in use
    u32 capacity; // how many we have to give out
} Memory_Tracker_System;

MAPI Memory_Tracker* memory_system_get_memory_tracker(Memory_Tracker_System* memory_tracker_system, String system_name,
                                                 u64 max_memory_allocated);

MAPI bool memory_tracker_track_allocation(Memory_Tracker* memory_tracker, memory_container_type type, u64 size);

MAPI bool memory_tracker_free_allocation(Memory_Tracker* memory_tracker, memory_container_type type, u64 size);

MAPI void memory_tracker_system_print_all_memory_usage(Memory_Tracker_System* memory_tracker);

MAPI void memory_tracker_print_memory_usage(Memory_Tracker* memory_tracker);


#endif
