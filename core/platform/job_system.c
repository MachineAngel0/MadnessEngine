#include "job_system.h"
#include "thread_madness.h"


static Job_System* job_system;

u32 job_thread_run(void* data)
{
    u32 thread_index = *(u32*)data;
    Madness_Thread thread = job_system->job_threads[thread_index].thread;
    u64 thread_id = thread_get_id();
    thread.thread_id;

    while (true)
    {
        if (!job_system || !job_system->running || !thread.data)
        {
            break;
        }

        Job_Info info = {0};
        if (!mutex_lock(&job_system->mutex))
        {
            INFO("FAILED TO OBTAIN LOCK ON MUTEX ON JOB THREAD");
        }
        if (!ring_queue_is_empty(job_system->work_queue))
        {
            ring_dequeue(job_system->work_queue, &info);
        }
        if (mutex_unlock(&job_system->mutex))
        {
            // INFO("FAILED TO RELEASE MUTEX ON JOB THREAD");
        }

        if (info.job_start)
        {
            bool result = info.job_start(info.param_data);
        }
    }
    return 1;
}


bool job_system_init(Memory_System* memory_system)
{
    job_system = memory_system_alloc(memory_system, sizeof(Job_System), MEMORY_SUBSYSTEM_THREAD);

    //minus one cause we dont count the main thread
    job_system->thread_count = get_threads_available() - 1;

    INFO("MAIN THREAD ID: %llu", thread_get_id());
    INFO("Spawning Thread Count: %llu", job_system->thread_count);
    job_system->running = true;


    if (!mutex_create(&job_system->mutex))
    {
        FATAL("OS ERROR CANNOT CREATE MUTEX");
    }

    //large amount can lower it later
    job_system->work_queue = ring_queue_create(sizeof(Job_Info), 1024);


    for (u32 i = 0; i < job_system->thread_count; i++)
    {
        Job_Thread* job_thread = &job_system->job_threads[i];
        job_thread->job_thread_index = i;

        u64 thread_allocator_memory_size = MB(1);
        void* allocator_memory = memory_system_alloc(memory_system, thread_allocator_memory_size, MEMORY_SUBSYSTEM_THREAD);
        allocator_init(&job_thread->allocator, allocator_memory, thread_allocator_memory_size);
        job_thread->allocator;

        if (!thread_create(job_thread_run, &job_system->job_threads[i].job_thread_index, false, &job_thread->thread))
        {
            FATAL("OS ERROR CANNOT SPAWN THREAD");
            return false;
        }
    }


    return true;
}

void job_system_deinit(Memory_System* memory_system)
{
    //TODO:
}

void job_system_submit(Job_Info* job_info)
{
    ring_enqueue(job_system->work_queue, job_info);
}

Job_Info job_create(Job_Type type, fptr_job_start entry_point, fptr_job_complete on_success, fptr_job_complete on_fail,
                    void* job_param_data, u32 job_param_data_size)
{
    Job_Info job;
    job.job_start = entry_point;
    job.job_success = on_success;
    job.job_fail = on_fail;
    job.job_type = type;


    job.param_data = allocator_malloc(job_param_data_size); //TODO: free and replace with an allocator
    memcpy(job.param_data, job_param_data, job_param_data_size);
    job.param_data_size = job_param_data_size;

/**/
    return job;
}


bool job_system_test_start(void* params)
{
    Job_Test_Param* job_params = (Job_Test_Param*)params;

    DEBUG("Test Job: %d, %d, Loop Index: %d", job_params->is_successful, job_params->val, job_params->loop_index);

    return true;
}

void job_system_test()
{
    for (u32 i = 0; i < 100; i++)
    {
        Job_Test_Param job1 = {.is_successful = false, .val = rand_range_i(0, 100), .loop_index = i};
        Job_Test_Param job2 = {.is_successful = true, .val = rand_range_i(0, 100), .loop_index = i};

        Job_Info job_info1 = job_create(JOB_TYPE_GENERAL, job_system_test_start,
                                        NULL, NULL,
                                        &job1, sizeof(Job_Test_Param));
        Job_Info job_info2 = job_create(JOB_TYPE_GENERAL, job_system_test_start,
                                        NULL, NULL,
                                        &job2, sizeof(Job_Test_Param));

        job_system_submit(&job_info1);
        job_system_submit(&job_info2);
    }
}
