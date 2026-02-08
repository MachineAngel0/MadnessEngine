#ifndef MEMORY_H
#define MEMORY_H
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "../logger.h"
#include "../defines.h"

//TODO: quick unit test
//TODO: hook up to the containers


//used for containers like arrays
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
    "MEMORY_CONTAINER_ARENA",
    "MEMORY_CONTAINER_ARENA_STACK",
    "MEMORY_CONTAINER_ARENA_POOl",
    "MEMORY_CONTAINER_ARENA_FREELIST",
    "MEMORY_ARRAY",
    "MEMORY_DARRAY",
    "MEMORY_STRING",
    "MEMORY_HASHSET",
    "MEMORY_HASHMAP",
    "MEMORY_QUEUE",
    "MEMORY_STACK",
};


//used for subsystems like the render or entity systems
typedef enum memory_subsystem_type
{
    MEMORY_SUBSYSTEM_APPLICATION_ARENA,
    MEMORY_SUBSYSTEM_GAME,
    MEMORY_SUBSYSTEM_ENTITY,
    MEMORY_SUBSYSTEM_ANIMATION,
    MEMORY_SUBSYSTEM_RENDERER,
    MEMORY_SUBSYSTEM_EVENT,
    MEMORY_SUBSYSTEM_INPUT,

    MEMORY_SUBSYSTEM_MAX,
} memory_subsystem_type;

//used for subsystems like animation or entity subsystems
static const char* memory_subsystem_type_string[MEMORY_SUBSYSTEM_MAX] =
{
    "MEMORY_SUBSYSTEM_APPLICATION_ARENA",
    "MEMORY_GAME",
    "MEMORY_ENTITY",
    "MEMORY_ANIMATION",
    "MEMORY_RENDERER",
    "MEMORY_EVENT",
    "MEMORY_INPUT",
};


typedef struct memory_tracker
{
    //keep track of memory in use
    u64 memory_container_usage[MEMORY_CONTAINER_MAX];
    u64 memory_container_size;
    u64 memory_subsystem_usage[MEMORY_SUBSYSTEM_MAX];
    u64 memory_subsystem_size;
} memory_tracker;


static memory_tracker memory;
static bool memory_tracker_initialized = false;

bool memory_tracker_init(void)
{
    memset(&memory, 0, sizeof(memory_tracker));
    memory.memory_container_size = 0;
    memory.memory_subsystem_size = 0;
    memory_tracker_initialized = true;
    return true;
}

bool memory_tracker_shutdown(void)
{
    if (!memory_tracker_initialized)
    {
        WARN("MEMORY TRACKER NOT INITIALIZED, NOTHING TO SHUTDOWN");
        return false;
    }
    INFO("MEMORY SYSTEM SHUTDOWN")
    return true;
}

bool memory_container_alloc(const memory_container_type type, const u64 size)
{
    if (!memory_tracker_initialized)
    {
        WARN("MEMORY TRACKER NOT INITIALIZED");
        return false;
    }


    memory.memory_container_usage[type] += size;
    memory.memory_container_size += size;
    return true;
}


bool memory_container_free(const memory_container_type type, const u64 size)
{
    if (!memory_tracker_initialized)
    {
        WARN("MEMORY TRACKER NOT INITIALIZED");
        return false;
    }
    memory.memory_container_usage[type] -= size;
    memory.memory_container_size -= size;
    return true;
}


bool memory_subsystem_alloc(const memory_subsystem_type type, const u64 size)
{
    if (!memory_tracker_initialized)
    {
        WARN("MEMORY TRACKER NOT INITIALIZED");
        return false;
    }

    memory.memory_subsystem_usage[type] += size;
    memory.memory_subsystem_size += size;
    return true;
}

bool memory_subsystem_free(const memory_subsystem_type type, const u64 size)
{
    if (!memory_tracker_initialized)
    {
        WARN("MEMORY TRACKER NOT INITIALIZED");
        return false;
    }

    memory.memory_subsystem_usage[type] -= size;
    memory.memory_subsystem_size -= size;
    return true;
}


void memory_tracker_print_memory_usage(void)
{
    if (!memory_tracker_initialized)
    {
        WARN("MEMORY TRACKER NOT INITIALIZED");
        return;
    }

    DEBUG("CONTAINER MEMORY");
    for (i32 i = 0; i < MEMORY_CONTAINER_MAX; i++)
    {
        u32 container_usage = 0;
        if (memory.memory_container_usage[i] >= GB(1))
        {
            container_usage = memory.memory_container_usage[i] / GB(1);
            DEBUG("%s : %d GB", memory_container_type_string[i], container_usage);
        }
        else if (memory.memory_container_usage[i] >= MB(1))
        {
            container_usage = memory.memory_container_usage[i] / MB(1);
            DEBUG("%s : %d MB", memory_container_type_string[i], container_usage);
        }
        else if (memory.memory_container_usage[i] >= KB(1))
        {
            container_usage = memory.memory_container_usage[i] / KB(1);
            DEBUG("%s : %d KB", memory_container_type_string[i], container_usage);
        }
        else if (memory.memory_container_usage[i] < KB(1))
        {
            DEBUG("%s : %d BYTES", memory_container_type_string[i], memory.memory_container_usage[i]);
        }
        else
        {
            FATAL("MEMORY CONTAINER PRINT ERROR");
        }
    }
    DEBUG("TOTAL CONTAINER MEMORY USAGE: %llu HB", (memory.memory_container_size / GB(1)));
    DEBUG("TOTAL CONTAINER MEMORY USAGE: %llu MB", (memory.memory_container_size / MB(1)));
    DEBUG("TOTAL CONTAINER MEMORY USAGE: %llu KB", (memory.memory_container_size / KB(1)));
    DEBUG("TOTAL CONTAINER MEMORY USAGE: %llu BYTES\n", memory.memory_container_size);


    DEBUG("SUBSYSTEM MEMORY");
    for (i32 i = 0; i < MEMORY_SUBSYSTEM_MAX; i++)
    {
        u32 container_usage = 0;
        if (memory.memory_subsystem_usage[i] >= GB(1))
        {
            container_usage = memory.memory_subsystem_usage[i] / GB(1);
            DEBUG("%s : %d GB", memory_subsystem_type_string[i], container_usage);
        }
        else if (memory.memory_subsystem_usage[i] >= MB(1))
        {
            container_usage = memory.memory_subsystem_usage[i] / MB(1);
            DEBUG("%s : %d MB", memory_subsystem_type_string[i], container_usage);
        }
        else if (memory.memory_subsystem_usage[i] >= KB(1))
        {
            container_usage = memory.memory_subsystem_usage[i] / KB(1);
            DEBUG("%s : %d KB", memory_subsystem_type_string[i], container_usage);
        }
        else if (memory.memory_subsystem_usage[i] < KB(1))
        {
            DEBUG("%s : %d BYTES", memory_subsystem_type_string[i], memory.memory_subsystem_usage[i]);
        }
        else
        {
            FATAL("MEMORY SUBSYSTEM PRINT ERROR");
        }
    }
    u64 mem_size = memory.memory_subsystem_size;
    DEBUG("TOTAL SUBSYSTEM MEMORY USAGE: %llu GB", (mem_size / GB(1)));
    DEBUG("TOTAL SUBSYSTEM MEMORY USAGE: %llu MB", (mem_size / MB(1)));
    DEBUG("TOTAL SUBSYSTEM MEMORY USAGE: %llu KB", (mem_size / KB(1)));
;    DEBUG("TOTAL SUBSYSTEM MEMORY USAGE: %llu BYTES\n", mem_size);
}

void memory_tracker_unit_test(void)
{
    //insta fail
    TEST_INFORM(memory_tracker_initialized);

    // memory_container_alloc(MEMORY_CONTAINER_UNKNOWN, 1024);
    // memory_subsystem_alloc(MEMORY_SUBSYSTEM_UNKNOWN, 1024);
    memory_subsystem_alloc(MEMORY_SUBSYSTEM_GAME, KB(1));

    memory_tracker_print_memory_usage();
}


#endif
