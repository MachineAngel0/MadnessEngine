#ifndef JOB_SYSTEM_H
#define JOB_SYSTEM_H


#include "defines.h"
#include "ring_queue.h"

// A function pointer definition for jobs.
typedef bool (*fpn_job_start)(void*, void*);

// function pointer definition for completion of a job
typedef void (*fpn_job_on_complete)(void*);

typedef enum job_type
{
    // A general job that does not have any specific thread requirements.
    // This means it matters little which job thread this job runs on.
    JOB_TYPE_GENERAL = 0x02,

    // A resource loading job. Resources should always load on the same thread
    // to avoid potential disk thrashing.
    JOB_TYPE_RESOURCE_LOAD = 0x04,

    // Jobs using GPU resources should be bound to a thread using this job type. Multithreaded
    // renderers will use a specific job thread, and this type of job will run on that thread.
    // For single-threaded renderers, this will be on the main thread.

    JOB_TYPE_GPU_RESOURCE = 0x08,
} job_type;

// Determines which job queue a job uses. The high-priority queue is always
// exhausted first before processing the normal-priority queue, which must also
// be exhausted before processing the low-priority queue.
//
typedef enum job_priority
{
    // The lowest-priority job, used for things that can wait to be done if need be, such as log flushing.
    JOB_PRIORITY_LOW,
    // A normal-priority job. Should be used for medium-priority tasks such as loading assets.
    JOB_PRIORITY_NORMAL,
    // The highest-priority job. Should be used sparingly, and only for time-critical operations.
    JOB_PRIORITY_HIGH
} job_priority;

typedef struct job_info
{
    //  Describes a job to be run.

    // The type of job. Used to determine which thread the job executes on. */
    job_type type;

    // The priority of this job. Higher priority jobs obviously run sooner. */
    job_priority priority;

    // A function pointer to be invoked when the job starts. Required. */
    fpn_job_start entry_point;

    // A function pointer to be invoked when the job successfully completes. Optional. */
    fpn_job_on_complete on_success;

    // A function pointer to be invoked when the job successfully fails. Optional. */
    fpn_job_on_complete on_fail;

    // Data to be passed to the entry point upon execution. */
    void* param_data;

    // The size of the data passed to the job. */
    u32 param_data_size;

    // Data to be passed to the success/fail function upon execution, if exists. */
    void* result_data;

    // The size of the data passed to the success/fail function. */
    u32 result_data_size;
} job_info;

typedef struct job_thread {
    u8 index;
    Madness_Thread thread;
    job_info info;
    // A mutex to guard access to this thread's info.
    Madness_Mutex info_mutex;

    // The types of jobs this thread can handle.
    u32 type_mask;
} job_thread;

typedef struct job_result_entry {
    u16 id;
    fpn_job_on_complete callback;
    u32 param_size;
    void* params;
} job_result_entry;

// The max number of job results that can be stored at once.
#define MAX_JOB_RESULTS 512

typedef struct job_system_state {
    bool running;
    u8 thread_count;
    job_thread job_threads[32];

    ring_queue low_priority_queue;
    ring_queue normal_priority_queue;
    ring_queue high_priority_queue;

    // Mutexes for each queue, since a job could be kicked off from another job (thread).
    Madness_Mutex low_pri_queue_mutex;
    Madness_Mutex normal_pri_queue_mutex;
    Madness_Mutex high_pri_queue_mutex;

    job_result_entry pending_results[MAX_JOB_RESULTS];
    Madness_Mutex result_mutex;
    // A mutex for the result array
} job_system_state;


// Initializes the job system. Call once to retrieve job_system_memory_requirement, passing 0 to state. Then
//  call a second time with allocated state memory block.
bool job_system_initialize(u64* job_system_memory_requirement, void* state, u8 max_job_thread_count, u32 type_masks[]);

// Shuts the job system down.
void job_system_shutdown(void* state);

// Updates the job system. Should happen once an update cycle.
void job_system_update();

// Submits the provided job to be queued for execution.
MAPI void job_system_submit(job_info info);

// Creates a new job with default type (Generic) and priority (Normal).
MAPI job_info job_create(fpn_job_start entry_point, fpn_job_on_complete on_success, fpn_job_on_complete on_fail,
                         void* param_data, u32 param_data_size, u32 result_data_size);

// Creates a new job with default priority (Normal).
MAPI job_info job_create_type(fpn_job_start entry_point, fpn_job_on_complete on_success, fpn_job_on_complete on_fail,
                              void* param_data, u32 param_data_size, u32 result_data_size, job_type type);

//  Creates a new job with the provided priority.
MAPI job_info job_create_priority(fpn_job_start entry_point, fpn_job_on_complete on_success,
                                  fpn_job_on_complete on_fail, void* param_data, u32 param_data_size,
                                  u32 result_data_size, job_type type, job_priority priority);


#endif //JOB_SYSTEM_H
