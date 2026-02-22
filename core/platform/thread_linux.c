#include "thread_madness.h"


#if MPLATFORM_LINUX

#include "core/thread.h"
#include "logger.h"

#include <pthread.h>
#include <errno.h>
#include <sys/sysinfo.h>

//TODO: https://www.youtube.com/watch?v=NxtqrN6Jw-4&list=PLv8Ddw9K0JPg1BEO-RS-0MYs423cvLVtj&index=70
i32 get_threads_available()
{
    i32 processor_count = get_nprocs_conf();
    i32 processor_available = get_nprocs();
    INFO("%i processor cores, cores available: %i", processor_count, processor_available)
    return processor_available;
}

bool thread_create(pfn_thread_start start_function_ptr, void* params, bool auto_detach, Madness_Thread* out_thread)
{
}


void thread_destroy(Madness_Thread* madness_thread)
{
}

void thread_detach(Madness_Thread* madness_thread)
{
}


void thread_cancel(Madness_Thread* madness_thread)
{
}

bool thread_is_active(Madness_Thread* madness_thread)
{
}

void thread_sleep(Madness_Thread* madness_thread, u32 milliseconds)
{
}

u64 thread_get_id()
{
}

bool mutex_create(Madness_Mutex* out_mutex)
{
}

void mutex_destroy(Madness_Mutex* madness_mutex)
{
}

bool mutex_lock(Madness_Mutex* madness_mutex)
{
}

bool mutex_unlock(Madness_Mutex* madness_mutex)
{
}


#endif
