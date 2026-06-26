#ifndef MADNESS_THREAD_H
#define MADNESS_THREAD_H

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

typedef struct Madness_Semaphore
{
    void* data;
} Madness_Semaphore;

typedef u32 (fpn_thread_start)(void*);

// Thread

bool thread_create(fpn_thread_start start_function_ptr, void* params, bool auto_detach, Madness_Thread* out_thread);
void thread_destroy(Madness_Thread* madness_thread);
void thread_detach(Madness_Thread* madness_thread);
bool thread_join(Madness_Thread* madness_thread);
bool thread_join_timeout(Madness_Thread* madness_thread, u64 wait_ms);
void thread_cancel(Madness_Thread* madness_thread);
bool thread_is_active(Madness_Thread* madness_thread);
void thread_sleep(Madness_Thread* madness_thread, u32 milliseconds);
u64 thread_get_id();


//get logical processor cores
s32 get_threads_available();

// Mutex
bool mutex_create(Madness_Mutex* out_mutex);
void mutex_destroy(Madness_Mutex* madness_mutex);
bool mutex_lock(Madness_Mutex* madness_mutex);
bool mutex_unlock(Madness_Mutex* madness_mutex);


// Semaphore
bool semaphore_create(Madness_Semaphore* out_semaphore, u32 max_count, u32 start_count);
void semaphore_destroy(Madness_Semaphore* madness_semaphore);
bool semaphore_signal(Madness_Semaphore* madness_semaphore);
bool semaphore_wait(Madness_Semaphore* madness_semaphore, u64 wait_ms);


#endif //MADNESS_THREAD_H
