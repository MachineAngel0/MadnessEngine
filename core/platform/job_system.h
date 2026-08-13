#ifndef JOB_SYSTEM_H
#define JOB_SYSTEM_H

#include "defines.h"
#include "ring_queue.h"

#if HAS_ATOMICS
#include "stdatomic.h"
#endif


// typedef bool (job_start)(void* param, void* result);
typedef bool (*fptr_job_start)(void* param);
typedef void (*fptr_job_complete)(void*);


typedef enum Job_Type
{
    //can be literally anything
    JOB_TYPE_GENERAL = BITFLAG(0),
    //IO operation
    JOB_TYPE_RESOURCE_LOAD = BITFLAG(1),
    //Renderer
    JOB_TYPE_GPU_RESOURCE = BITFLAG(2),
} Job_Type;

typedef struct Job_Counter
{
    Madness_Atomic_U32 atomic_jobs_remaining;
} Job_Counter;

alignas(64) typedef struct /*alignas(64)*/ Job_Info
{
    Job_Type job_type;

    fptr_job_start job_start;
    fptr_job_complete job_success;
    fptr_job_complete job_fail;

    void* param_data;
    u32 param_data_size;

    Job_Counter* job_counter;

    /*
    u16* dependencies;
    u16 dependencies_count;
    */
} Job_Info;


alignas(64) typedef struct /*alignas(64)*/ Job_Result
{
    u8 job_thread_id; // thread this finished on
    fptr_job_complete completion_callback;

    u32 params_size;
    void* params;
} Job_Result;


typedef struct Job_Thread
{
    Madness_Thread thread;
    u8 job_thread_index;

    Job_Type jobs_can_run;

    Allocator allocator;
} Job_Thread;


#define MAX_MADNESS_THREADS 32  // not likely to have that many threads

typedef struct Job_System
{
    s32 thread_count;

    Job_Thread job_threads[MAX_MADNESS_THREADS];

    Madness_Mutex ring_mutex;
    Madness_Semaphore ring_semaphore;
    RING_QUEUE_TYPE(Job_Info)* work_queue; // not likely to have that many threads

    RING_QUEUE_TYPE(Job_Info)* background_work_queue; // not likely to have that many threads

    bool running;


    Pool_Allocator job_counter_pool;
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


Job_Counter* job_counter_create(u32 initial_job_count);

/**
 * @brief for use inside jobs that need to submit more jobs, and increase the jobs remaining count
 *
 */
void job_counter_add_jobs(Job_Counter* job_counter, u32 job_count);
void job_counter_decrement(Job_Counter* job_counter);
void job_system_wait_free(Job_Counter* job_counter);


//TODO: no work stealing queues for now
// just one queue for now, until we add work stealing
// have a way for right now to explicity wait for the job types to finish
// the main thread in the wait function should execute a job, so its not bieng idle
//


typedef struct Job_Test_Param
{
    bool is_successful;
    u32 val;
    u32 loop_index;
} Job_Test_Param;

void job_system_test();


#endif //JOB_SYSTEM_H
