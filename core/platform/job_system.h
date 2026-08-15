#ifndef JOB_SYSTEM_H
#define JOB_SYSTEM_H

#include "defines.h"
#include "ring_queue.h"
#include "work_steal_queue.h"
#include <stdalign.h>

#if HAS_ATOMICS
#include "stdatomic.h"
#endif


// typedef bool (job_start)(void* param, void* result);
typedef bool (*fptr_job_start)(void* param);
typedef void (*fptr_job_complete)(void*);


typedef enum Job_Type
{
    //can be literally anything
    JOB_TYPE_GENERAL,
    //IO operation and general asset loading
    JOB_TYPE_RESOURCE_LOAD,
    /*//Renderer
    JOB_TYPE_GPU_RESOURCE,*/ // well see about having support for this, might not be neccessary
} Job_Type;

typedef struct Job_Counter
{
    Madness_Atomic_U32 atomic_jobs_remaining;
    char* job_name;
} Job_Counter;

/*alignas(64)*/
typedef struct /*alignas(64)*/ Job_Info
{
    fptr_job_start job_start;
    fptr_job_complete job_success;
    fptr_job_complete job_fail;

    void* param_data;
    u32 param_data_size;

    Job_Type job_type;

    Job_Counter* job_counter;

    /*
    u16* dependencies;
    u16 dependencies_count;
    */

    u8 padding[80];
} Job_Info;


/*alignas(64)*/ typedef struct Job_Result
{
    u8 job_thread_id; // thread this finished on
    fptr_job_complete completion_callback;

    u32 params_size;
    void* params;
}   Job_Result;

typedef struct Job_Thread
{
    Madness_Thread thread;
    u8 job_thread_array_index;

    u64 job_thread_platform_id;
    Job_Type jobs_can_run;


    Allocator* allocator;
    Work_Steal_Queue* thread_work_queue;
} Job_Thread;


#define MAX_MADNESS_THREADS 32  // not likely to have that many threads

typedef struct Job_System
{
    s32 thread_count;

    Job_Thread job_threads[MAX_MADNESS_THREADS];

    Madness_Mutex general_work_mutex;
    Madness_Semaphore general_work_semaphore;
    RING_QUEUE_TYPE(Job_Info)* general_work_queue;

    Madness_Mutex general_resource_mutex;
    Madness_Semaphore general_resource_semaphore;
    RING_QUEUE_TYPE(Job_Info)* general_resource_queue; // limited amount of threads given to io operations

    bool running;

    Pool_Allocator job_counter_pool;
    Pool_Allocator job_counter_string_pool;


    // Allocator allocator[MAX_MADNESS_THREADS];
} Job_System;

bool job_system_init(Memory_System* memory_system);
void job_system_deinit(Memory_System* memory_system);
void job_system_set_dependencies(Memory_System* memory_system);


void job_system_submit(Job_Info* job_info);

Job_Info job_create(Job_Type type, fptr_job_start entry_point, fptr_job_complete on_success, fptr_job_complete on_fail,
                    void* job_param_data, u32 job_param_data_size);

Job_Info job_create_with_counter(Job_Type type, fptr_job_start entry_point, fptr_job_complete on_success,
                                 fptr_job_complete on_fail,
                                 void* job_param_data, u32 job_param_data_size, Job_Counter* counter);


Job_Counter* job_counter_create(const char* job_name, u32 initial_job_count);

/**
 * @brief for use inside jobs that need to submit more jobs, and increase the jobs remaining count
 *
 */
void job_counter_add_jobs(Job_Counter* job_counter, u32 job_count);
void job_counter_decrement(Job_Counter* job_counter);
void job_system_wait_free(Job_Counter* job_counter);


Allocator* job_system_get_thread_allocator();


typedef struct Job_Test_Param
{
    bool is_successful;
    u32 val;
    u32 loop_index;
} Job_Test_Param;

void job_system_test();


#endif //JOB_SYSTEM_H
