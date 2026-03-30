#include "memory_tracker.h"


Memory_Tracker* memory_system_get_memory_tracker(Memory_Tracker_System* memory_tracker_system, String system_name,
                                                 u64 max_memory_allocated)
{

    if (memory_tracker_system->size >= memory_tracker_system->capacity)
    {
        FATAL("RAN OUT OF MEMORY TRACKERS, INCREASE MAX_MEMORY_TRACKERS IN memory_tracker.h")
        return NULL;
    }

    Memory_Tracker* out_memory_tracker = &memory_tracker_system->memory_tracker[memory_tracker_system->size++];
    memset(out_memory_tracker, 0, sizeof(Memory_Tracker));


    out_memory_tracker->system_name = system_name;
    out_memory_tracker->subsystem_memory_size = max_memory_allocated;

    return out_memory_tracker;

}

bool memory_tracker_track_allocation(Memory_Tracker* memory_tracker, const memory_container_type type, const u64 size)
{
    MASSERT(memory_tracker)
    memory_tracker->memory_container_usage[type] += size;
    return true;
}


bool memory_tracker_free_allocation(Memory_Tracker* memory_tracker,const memory_container_type type, const u64 size)
{
    MASSERT(memory_tracker)

    memory_tracker->memory_container_usage[type] -= size;
    return true;
}

void memory_tracker_print_memory_usage(Memory_Tracker* memory_tracker)
{
    MASSERT(memory_tracker)

    //TODO: track the total usage,

    //print system name
    string_print(&memory_tracker->system_name);

    DEBUG("CONTAINER MEMORY");
    u64 memory_container_total_size = 0;

    for (i32 i = 0; i < MEMORY_CONTAINER_MAX; i++)
    {
        u32 container_usage = 0;
        if (memory_tracker->memory_container_usage[i] >= GB(1))
        {
            container_usage = memory_tracker->memory_container_usage[i] / GB(1);
            DEBUG("%s : %d GB", memory_container_type_string[i], container_usage);
        }
        else if (memory_tracker->memory_container_usage[i] >= MB(1))
        {
            container_usage = memory_tracker->memory_container_usage[i] / MB(1);
            DEBUG("%s : %d MB", memory_container_type_string[i], container_usage);
        }
        else if (memory_tracker->memory_container_usage[i] >= KB(1))
        {
            container_usage = memory_tracker->memory_container_usage[i] / KB(1);
            DEBUG("%s : %d KB", memory_container_type_string[i], container_usage);
        }
        else if (memory_tracker->memory_container_usage[i] < KB(1))
        {
            DEBUG("%s : %d BYTES", memory_container_type_string[i], memory_tracker->memory_container_usage[i]);
        }
        else
        {
            FATAL("MEMORY CONTAINER PRINT ERROR");
        }

        memory_container_total_size += memory_tracker->memory_container_usage[i];

    }
    DEBUG("TOTAL CONTAINER MEMORY USAGE: %llu GB", (memory_container_total_size / GB(1)));
    DEBUG("TOTAL CONTAINER MEMORY USAGE: %llu MB", (memory_container_total_size / MB(1)));
    DEBUG("TOTAL CONTAINER MEMORY USAGE: %llu KB", (memory_container_total_size / KB(1)));
    DEBUG("TOTAL CONTAINER MEMORY USAGE: %llu BYTES\n", memory_container_total_size);
    DEBUG("TOTAL CONTAINER MEMORY ALLOCATED: %llu BYTES\n", memory_tracker->subsystem_memory_size);


}

void memory_tracker_system_print_all_memory_usage(Memory_Tracker_System* memory_tracker)
{
    for (u32 i = 0; i < memory_tracker->size; i++)
    {
        memory_tracker_print_memory_usage(&memory_tracker->memory_tracker[i]);
    }
}
