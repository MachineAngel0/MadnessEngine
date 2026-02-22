#ifndef MADNESS_THREAD_H
#define MADNESS_THREAD_H

#include <stdbool.h>
#include <stddef.h>
#include "defines.h"

typedef struct Madness_Thread
{
    void* data;
    u64 thread_id;
} Madness_Thread;

typedef struct Madness_Mutex
{
    void* data;
} Madness_Mutex;

typedef u32 (pfn_thread_start)(void*);

bool thread_create(pfn_thread_start start_function_ptr, void* params, bool auto_detach, Madness_Thread* out_thread);
void thread_destroy(Madness_Thread* madness_thread);
void thread_detach(Madness_Thread* madness_thread);
void thread_cancel(Madness_Thread* madness_thread);
bool thread_is_active(Madness_Thread* madness_thread);
void thread_sleep(Madness_Thread* madness_thread, u32 milliseconds);
u64 thread_get_id();

// void thread_join();

//get logical processor cores
i32 get_threads_available();


bool mutex_create(Madness_Mutex* out_mutex);
void mutex_destroy(Madness_Mutex* madness_mutex);
bool mutex_lock(Madness_Mutex* madness_mutex);
bool mutex_unlock(Madness_Mutex* madness_mutex);

#endif //MADNESS_THREAD_H
