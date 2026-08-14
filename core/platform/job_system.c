#include "job_system.h"
#include "thread_madness.h"


static Job_System* job_system;

#define THREAD_STACK_MEMORY_SIZE KB(4)
#define MAX_JOB_COUNTER_POOL 128 // I doubt it will be more than this

u32 job_thread_run(void* data)
{
    u32 thread_index = *(u32*)data;
    Job_Thread* job_thread = &job_system->job_threads[thread_index];
    // Madness_Thread thread = job_system->job_threads[thread_index].thread;
    u64 thread_id = thread_get_id();
    // thread.thread_id;

    //allocate some memory on the stack for this thread
    /*
    u8 memory[THREAD_STACK_MEMORY_SIZE];
    allocator_init(&job_thread.allocator, memory, THREAD_STACK_MEMORY_SIZE);
    */

    while (true)
    {
        if (!job_system || !job_system->running || !job_thread)
        {
            break;
        }

        Job_Info job_task = {0};


        //wait until a job is available
        if (!semaphore_wait(&job_system->general_work_semaphore, INFINITE))
        {
            M_ERROR("Failed waiting on job semaphore");
            break;
        }

        if (!mutex_lock(&job_system->general_work_mutex))
        {
            INFO("FAILED TO OBTAIN LOCK ON MUTEX ON JOB THREAD");
        }

        ring_dequeue(job_system->general_work_queue, &job_task);
        if (!mutex_unlock(&job_system->general_work_mutex))
        {
            INFO("FAILED TO RELEASE MUTEX ON JOB THREAD");
        }


        if (job_task.job_start)
        {
            bool result = job_task.job_start(job_task.param_data);

            if (job_task.job_counter)
            {
                job_counter_decrement(job_task.job_counter);
            }

            //TODO: replace, also not thread safe if we use our own allocator
            free(job_task.param_data);
        }
    }
    return true;
}

/**
 * @brief these thread runs our io operations and asset loading
 */
u32 job_thread_run_resource(void* data)
{
    u32 thread_index = *(u32*)data;
    Job_Thread* job_thread = &job_system->job_threads[thread_index];
    // Madness_Thread thread = job_system->job_threads[thread_index].thread;
    u64 thread_id = thread_get_id();
    // thread.thread_id;

    //create our worker queues



    while (true)
    {
        if (!job_system || !job_system->running || !job_thread)
        {
            break;
        }

        Job_Info job_task = {0};


        //wait until a job is available
        if (!semaphore_wait(&job_system->general_resource_semaphore, INFINITE))
        {
            M_ERROR("Failed waiting on job semaphore");
            break;
        }

        if (!mutex_lock(&job_system->general_resource_mutex))
        {
            INFO("FAILED TO OBTAIN LOCK ON MUTEX ON JOB THREAD");
        }

        ring_dequeue(job_system->general_resource_queue, &job_task);
        if (!mutex_unlock(&job_system->general_resource_mutex))
        {
            INFO("FAILED TO RELEASE MUTEX ON JOB THREAD");
        }


        if (job_task.job_start)
        {
            bool result = job_task.job_start(job_task.param_data);

            if (job_task.job_counter)
            {
                job_counter_decrement(job_task.job_counter);
            }

            //TODO: replace, also not thread safe if we use our own allocator
            free(job_task.param_data);
        }
    }
    return true;
}


bool job_system_init(Memory_System* memory_system)
{
    job_system = memory_system_alloc(memory_system, sizeof(Job_System), MEMORY_SUBSYSTEM_THREAD);

    //minus one cause we don't count the main thread
    job_system->thread_count = get_threads_available() - 1;

    Job_Type thread_default = JOB_TYPE_GENERAL;
    memcpy(job_system->job_threads, &thread_default, sizeof(Job_Type));







    //set up our background resource loader threads
    if (job_system->thread_count >= 15)
    {
        job_system->job_threads[0].jobs_can_run = JOB_TYPE_RESOURCE_LOAD;
        job_system->job_threads[1].jobs_can_run = JOB_TYPE_RESOURCE_LOAD;
        job_system->job_threads[2].jobs_can_run = JOB_TYPE_RESOURCE_LOAD;
        job_system->job_threads[3].jobs_can_run = JOB_TYPE_RESOURCE_LOAD;
    }
    else if (job_system->thread_count >= 7)
    {
        job_system->job_threads[0].jobs_can_run = JOB_TYPE_RESOURCE_LOAD;
        job_system->job_threads[1].jobs_can_run = JOB_TYPE_RESOURCE_LOAD;
    }
    else if (job_system->thread_count >= 2)
    {
        job_system->job_threads[0].jobs_can_run = JOB_TYPE_RESOURCE_LOAD;
    }

    INFO("MAIN THREAD ID: %llu", thread_get_id());
    INFO("Spawning Thread Count: %llu", job_system->thread_count);
    job_system->running = true;


    if (!mutex_create(&job_system->general_work_mutex))
    {
        FATAL("OS ERROR CANNOT CREATE MUTEX");
        MASSERT_FALSE()
    }
    if (!semaphore_create(&job_system->general_work_semaphore, 1024, 0))
    {
        FATAL("OS ERROR CANNOT CREATE SEMAPHORE");
        MASSERT_FALSE()
    }


    //large amount can lower it later
    job_system->general_work_queue = ring_queue_create(sizeof(Job_Info), 1024);

    u64 job_counter_memory_size = MAX_JOB_COUNTER_POOL * sizeof(Job_Counter);
    void* job_counter_pool_memory =
        memory_system_alloc(memory_system, job_counter_memory_size, MEMORY_SUBSYSTEM_THREAD);

    pool_allocator_init(&job_system->job_counter_pool, job_counter_pool_memory, job_counter_memory_size,
                        sizeof(Job_Counter), 8);

    const u64 job_counter_string_size = 256;
    const u64 job_counter_memory_string_size = MAX_JOB_COUNTER_POOL * job_counter_string_size; //small string
    void* job_counter_string_memory_pool =
            memory_system_alloc(memory_system, job_counter_memory_size, MEMORY_SUBSYSTEM_THREAD);
    pool_allocator_init(&job_system->job_counter_string_pool, job_counter_string_memory_pool, job_counter_memory_string_size,
                      job_counter_string_size, 8);


    u64 total_thread_allocator_memory = MB(64);
    u64 per_thread_allocator_memory = total_thread_allocator_memory / job_system->thread_count;

    for (u32 i = 0; i < job_system->thread_count; i++)
    {
        Job_Thread* job_thread = &job_system->job_threads[i];
        job_thread->job_thread_index = i;


       job_thread->allocator = memory_system_allocator_create(memory_system, per_thread_allocator_memory,
                                                     MEMORY_SUBSYSTEM_THREAD);
        switch (job_thread->jobs_can_run)
        {
        case JOB_TYPE_GENERAL:
            if (!thread_create(job_thread_run, &job_system->job_threads[i].job_thread_index, false,
                               &job_thread->thread))
            {
                FATAL("OS ERROR CANNOT SPAWN THREAD");
                return false;
            }
            break;
        case JOB_TYPE_RESOURCE_LOAD:
            if (!thread_create(job_thread_run_resource, &job_system->job_threads[i].job_thread_index, false,
                               &job_thread->thread))
            {
                FATAL("OS ERROR CANNOT SPAWN THREAD");
                return false;
            }
            break;
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
    switch (job_info->job_type)
    {
    case JOB_TYPE_GENERAL:
        mutex_lock(&job_system->general_work_mutex);
        ring_enqueue(job_system->general_work_queue, job_info);
        mutex_unlock(&job_system->general_work_mutex);
        semaphore_signal(&job_system->general_work_semaphore);
        break;
    case JOB_TYPE_RESOURCE_LOAD:
        mutex_lock(&job_system->general_resource_mutex);
        ring_enqueue(job_system->general_resource_queue, job_info);
        mutex_unlock(&job_system->general_resource_mutex);
        semaphore_signal(&job_system->general_resource_semaphore);
        break;
    }
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
    job.job_counter = NULL;

    /**/
    return job;
}

Job_Info job_create_with_counter(Job_Type type, fptr_job_start entry_point, fptr_job_complete on_success,
                                 fptr_job_complete on_fail, void* job_param_data, u32 job_param_data_size,
                                 Job_Counter* counter)
{
    Job_Info job;
    job.job_start = entry_point;
    job.job_success = on_success;
    job.job_fail = on_fail;
    job.job_type = type;


    //TODO: replace, also not thread safe if we use our own allocator
    job.param_data = malloc(job_param_data_size); //TODO: free and replace with an allocator


    memcpy(job.param_data, job_param_data, job_param_data_size);
    job.param_data_size = job_param_data_size;
    job.job_counter = counter;

    /**/
    return job;
}

Job_Counter* job_counter_create(const char* job_name, u32 initial_job_count)
{
    //allocate a new job
    Job_Counter* job_counter = pool_allocator_alloc(&job_system->job_counter_pool);
    atomic_u32_init(&job_counter->atomic_jobs_remaining, 0, NULL);

    job_counter->job_name = pool_allocator_alloc(&job_system->job_counter_string_pool);
    strcpy(job_counter->job_name, job_name);


    job_counter_add_jobs(job_counter, initial_job_count);
    return job_counter;
}

void job_counter_add_jobs(Job_Counter* job_counter, u32 job_count)
{
    //add to the counter
    atomic_u32_fetch_add(&job_counter->atomic_jobs_remaining, job_count);
}

void job_counter_decrement(Job_Counter* job_counter)
{
    //add to the counter
    atomic_u32_fetch_sub(&job_counter->atomic_jobs_remaining, 1);
}


void job_system_wait_free(Job_Counter* job_counter)
{
    //wait until the specific job finishes
    while (atomic_u32_load(&job_counter->atomic_jobs_remaining) != 0)
    {
        //TODO: have the main thread do some work
        // INFO("JOB SYSTEM WAIT FREE: WAITING FOR WORK TO FINISH")
        INFO("JOB SYSTEM WAIT FREE: MAIN THREADING GOING TO WORK")

        Job_Info job_task = {0};

        //we use a zero here because it is opertunisitc, it's not necessarily going to do many jobs
        if (!semaphore_wait(&job_system->general_work_semaphore, 0))
        {
            INFO("JOB SYSTEM WAIT FREE: Failed waiting on job semaphore");
            continue;
        }

        if (!mutex_lock(&job_system->general_work_mutex))
        {
            INFO("FAILED TO OBTAIN LOCK ON MUTEX ON JOB THREAD");
        }

        ring_dequeue(job_system->general_work_queue, &job_task);
        if (!mutex_unlock(&job_system->general_work_mutex))
        {
            INFO("FAILED TO RELEASE MUTEX ON JOB THREAD");
        }


        if (job_task.job_start)
        {
            bool result = job_task.job_start(job_task.param_data);

            if (job_task.job_counter)
            {
                job_counter_decrement(job_task.job_counter);
            }

            //TODO: replace, also not thread safe if we use our own allocator
            free(job_task.param_data);
        }
    }


    //once its done we can free the counter
    pool_allocator_free(&job_system->job_counter_string_pool, job_counter->job_name);
    pool_allocator_free(&job_system->job_counter_pool, job_counter);
}


bool job_system_test_start(void* params)
{
    Job_Test_Param* job_params = (Job_Test_Param*)params;

    TRACE("Test Job: %d, %d, Loop Index: %d", job_params->is_successful, job_params->val, job_params->loop_index);

    //simiulates fake expensive work
    /*
    volatile u64 x = 0;

    for (u64 i = 0; i < 10000000; ++i)
    {
        x += i;
    }*/


    return true;
}

void job_system_test()
{
    Job_Counter* job_counter = job_counter_create("test job", 200);
    for (u32 i = 0; i < 100; i++)
    {
        Job_Test_Param job1 = {.is_successful = false, .val = rand_range_i(0, 100), .loop_index = i};
        Job_Test_Param job2 = {.is_successful = true, .val = rand_range_i(0, 100), .loop_index = i};

        Job_Info job_info1 = job_create_with_counter(JOB_TYPE_GENERAL, job_system_test_start,
                                                     NULL, NULL,
                                                     &job1, sizeof(Job_Test_Param), job_counter);
        Job_Info job_info2 = job_create_with_counter(JOB_TYPE_GENERAL, job_system_test_start,
                                                     NULL, NULL,
                                                     &job2, sizeof(Job_Test_Param), job_counter);


        job_system_submit(&job_info1);
        job_system_submit(&job_info2);
    }


    job_system_wait_free(job_counter);

    DEBUG("JOB TEST: all jobs ran");
}
