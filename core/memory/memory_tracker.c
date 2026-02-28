#include "memory_tracker.h"

bool memory_tracker_init(void)
{
    memset(&memory_tracker, 0, sizeof(Memory_Tracker));
    memory_tracker.memory_container_size = 0;
    memory_tracker.memory_subsystem_size = 0;
    memory_tracker_initialized = true;
    return true;
}

bool memory_tracker_shutdown(void)
{
    memory_tracker_print_memory_usage();
    if (!memory_tracker_initialized)
    {
        WARN("MEMORY TRACKER NOT INITIALIZED, NOTHING TO SHUTDOWN");
        return false;
    }
    INFO("MEMORY TRACKER SHUTDOWN")
    return true;
}

bool memory_tracker_container_alloc(const memory_container_type type, const u64 size)
{
    if (!memory_tracker_initialized)
    {
        WARN("MEMORY TRACKER NOT INITIALIZED");
        return false;
    }


    memory_tracker.memory_container_usage[type] += size;
    memory_tracker.memory_container_size += size;
    return true;
}


bool memory_tracker_container_free(const memory_container_type type, const u64 size)
{
    if (!memory_tracker_initialized)
    {
        WARN("MEMORY TRACKER NOT INITIALIZED");
        return false;
    }
    memory_tracker.memory_container_usage[type] -= size;
    memory_tracker.memory_container_size -= size;
    return true;
}


bool memory_tracker_subsystem_alloc(const memory_subsystem_type type, const u64 size)
{
    if (!memory_tracker_initialized)
    {
        WARN("MEMORY TRACKER NOT INITIALIZED");
        return false;
    }

    memory_tracker.memory_subsystem_usage[type] += size;
    memory_tracker.memory_subsystem_size += size;
    return true;
}

bool memory_tracker_subsystem_free(const memory_subsystem_type type, const u64 size)
{
    if (!memory_tracker_initialized)
    {
        WARN("MEMORY TRACKER NOT INITIALIZED");
        return false;
    }

    memory_tracker.memory_subsystem_usage[type] -= size;
    memory_tracker.memory_subsystem_size -= size;
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
        if (memory_tracker.memory_container_usage[i] >= GB(1))
        {
            container_usage = memory_tracker.memory_container_usage[i] / GB(1);
            DEBUG("%s : %d GB", memory_container_type_string[i], container_usage);
        }
        else if (memory_tracker.memory_container_usage[i] >= MB(1))
        {
            container_usage = memory_tracker.memory_container_usage[i] / MB(1);
            DEBUG("%s : %d MB", memory_container_type_string[i], container_usage);
        }
        else if (memory_tracker.memory_container_usage[i] >= KB(1))
        {
            container_usage = memory_tracker.memory_container_usage[i] / KB(1);
            DEBUG("%s : %d KB", memory_container_type_string[i], container_usage);
        }
        else if (memory_tracker.memory_container_usage[i] < KB(1))
        {
            DEBUG("%s : %d BYTES", memory_container_type_string[i], memory_tracker.memory_container_usage[i]);
        }
        else
        {
            FATAL("MEMORY CONTAINER PRINT ERROR");
        }
    }
    DEBUG("TOTAL CONTAINER MEMORY USAGE: %llu KB", (memory_tracker.memory_container_size / GB(1)));
    DEBUG("TOTAL CONTAINER MEMORY USAGE: %llu MB", (memory_tracker.memory_container_size / MB(1)));
    DEBUG("TOTAL CONTAINER MEMORY USAGE: %llu KB", (memory_tracker.memory_container_size / KB(1)));
    DEBUG("TOTAL CONTAINER MEMORY USAGE: %llu BYTES\n", memory_tracker.memory_container_size);


    DEBUG("SUBSYSTEM MEMORY");
    for (i32 i = 0; i < MEMORY_SUBSYSTEM_MAX; i++)
    {
        u32 container_usage = 0;
        if (memory_tracker.memory_subsystem_usage[i] >= GB(1))
        {
            container_usage = memory_tracker.memory_subsystem_usage[i] / GB(1);
            DEBUG("%s : %d GB", memory_subsystem_type_string[i], container_usage);
        }
        else if (memory_tracker.memory_subsystem_usage[i] >= MB(1))
        {
            container_usage = memory_tracker.memory_subsystem_usage[i] / MB(1);
            DEBUG("%s : %d MB", memory_subsystem_type_string[i], container_usage);
        }
        else if (memory_tracker.memory_subsystem_usage[i] >= KB(1))
        {
            container_usage = memory_tracker.memory_subsystem_usage[i] / KB(1);
            DEBUG("%s : %d KB", memory_subsystem_type_string[i], container_usage);
        }
        else if (memory_tracker.memory_subsystem_usage[i] < KB(1))
        {
            DEBUG("%s : %d BYTES", memory_subsystem_type_string[i], memory_tracker.memory_subsystem_usage[i]);
        }
        else
        {
            FATAL("MEMORY SUBSYSTEM PRINT ERROR");
        }
    }
    u64 mem_size = memory_tracker.memory_subsystem_size;
    DEBUG("TOTAL SUBSYSTEM MEMORY USAGE: %llu GB", (mem_size / GB(1)));
    DEBUG("TOTAL SUBSYSTEM MEMORY USAGE: %llu MB", (mem_size / MB(1)));
    DEBUG("TOTAL SUBSYSTEM MEMORY USAGE: %llu KB", (mem_size / KB(1)));
    DEBUG("TOTAL SUBSYSTEM MEMORY USAGE: %llu BYTES\n", mem_size);
}

void memory_tracker_unit_test(void)
{
    //insta fail
    TEST_INFORM(memory_tracker_initialized);

    // memory_container_alloc(MEMORY_CONTAINER_UNKNOWN, 1024);
    // memory_subsystem_alloc(MEMORY_SUBSYSTEM_UNKNOWN, 1024);
    memory_tracker_subsystem_alloc(MEMORY_SUBSYSTEM_GAME, KB(1));

    memory_tracker_print_memory_usage();
}
