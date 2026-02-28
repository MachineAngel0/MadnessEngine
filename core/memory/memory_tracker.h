#ifndef MEMORY_H
#define MEMORY_H

#include <stdbool.h>
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


typedef struct Memory_Tracker
{
    //keep track of memory in use
    u64 memory_container_usage[MEMORY_CONTAINER_MAX];
    u64 memory_container_size;
    u64 memory_subsystem_usage[MEMORY_SUBSYSTEM_MAX];
    u64 memory_subsystem_size;
} Memory_Tracker;


static Memory_Tracker memory_tracker;
static bool memory_tracker_initialized = false;

bool memory_tracker_init(void);

bool memory_tracker_shutdown(void);

bool memory_tracker_container_alloc(memory_container_type type, u64 size);


bool memory_tracker_container_free(memory_container_type type, u64 size);

bool memory_tracker_subsystem_alloc(memory_subsystem_type type, u64 size);

bool memory_tracker_subsystem_free(memory_subsystem_type type, u64 size);


void memory_tracker_print_memory_usage(void);

void memory_tracker_unit_test(void);


#endif
