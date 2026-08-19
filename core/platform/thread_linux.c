#include "thread_madness.h"


#if MPLATFORM_LINUX

#include "logger.h"
#include "platform.h"

#include <pthread.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
// #include <sys/sem.h> //legacy
#include <semaphore.h>
#include <errno.h>
#include <sys/sysinfo.h>

//TODO: https://www.youtube.com/watch?v=NxtqrN6Jw-4&list=PLv8Ddw9K0JPg1BEO-RS-0MYs423cvLVtj&index=70
s32 get_threads_available(void)
{
    s32 processor_count = get_nprocs_conf();
    s32 processor_available = get_nprocs();
    INFO("%i processor cores, cores available: %i", processor_count, processor_available)
    return processor_available;
}

typedef void* (*linux_thread_start_callback)(void*);
bool thread_create(fptr_thread_start start_function_ptr, void* params, bool auto_detach, Madness_Thread* out_thread)
{
    if (!start_function_ptr)
    {
        M_ERROR("THREAD CREATE: INVALID START FUNCTION PTR", out_thread->thread_id);
        return false;
    }
    s32 result = pthread_create((pthread_t*)&out_thread->thread_id, 0, (linux_thread_start_callback*)start_function_ptr,
                                params);
    if (result != 0)
    {
        switch (result)
        {
        case EAGAIN:
            M_ERROR("Failed to create thread: insufficient resources to create another thread.");
            return false;
        case EINVAL:
            M_ERROR("Failed to create thread: invalid settings were passed in attributes..");
            return false;
        default:
            M_ERROR("Failed to create thread: an unhandled error has occurred. errno=%i", result);
            return false;
        }
    }


    DEBUG("Starting process on OS thread id: %#x", out_thread->thread_id);
    if (auto_detach)
    {
        s32 thread_detach_result = pthread_detach((pthread_t)out_thread->thread_id);
        if (thread_detach_result != 0)
        {
            switch (thread_detach_result)
            {
            case EINVAL:
                M_ERROR("Failed thread_detach_resulto detach newly-created thread: thread is not a joinable thread.");
                return false;
            case ESRCH:
                M_ERROR("Failed to detach newly-created thread: no thread with the id %#x could be found.",
                        out_thread->thread_id);
                return false;
            default:
                M_ERROR("Failed to detach newly-created thread: an unknown error has occurred. errno=%i", result);
                return false;
            }
        }
    }
    else
    {
        out_thread->data = platform_allocate(sizeof(u64),true);
        *(u64*)out_thread->data = out_thread->thread_id;
    }

    return true;
}


void thread_destroy(Madness_Thread* madness_thread)
{
    MASSERT_FALSE()
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

u64 thread_get_id(void)
{
    return (u64)pthread_self();
}

bool mutex_create(Madness_Mutex* out_mutex)
{
    if (!out_mutex)
    {
        MASSERT_FALSE()
        return false;
    }

    pthread_mutex_t mutex;
    pthread_mutexattr_t mutex_attr;
    pthread_mutexattr_init(&mutex_attr);
    pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_RECURSIVE);
    s32 result = pthread_mutex_init(&mutex, &mutex_attr);
    if (result != 0)
    {
        M_ERROR("Mutex creation failure!");
        return false;
    }

    // Save off the mutex handle.
    out_mutex->data = platform_allocate(sizeof(pthread_mutex_t), false);
    *(pthread_mutex_t*)out_mutex->data = mutex;
    return true;
}

void mutex_destroy(Madness_Mutex* madness_mutex)
{
    if (!madness_mutex)
    {
        MASSERT_FALSE()
        return;
    }
    s32 result = pthread_mutex_destroy((pthread_mutex_t*)madness_mutex->data);
    switch (result)
    {
    case 0:
        // KTRACE("Mutex destroyed.");
        break;
    case EBUSY:
        M_ERROR("Unable to destroy mutex: mutex is locked or referenced.");
        break;
    case EINVAL:
        M_ERROR("Unable to destroy mutex: the value specified by mutex is invalid.");
        break;
    default:
        M_ERROR("An handled error has occurred while destroy a mutex: errno=%i", result);
        break;
    }

    platform_free(madness_mutex->data);
    madness_mutex->data = 0;
}


bool mutex_lock(Madness_Mutex* madness_mutex)
{
    if (!madness_mutex)
    {
        MASSERT_FALSE()
        return false;
    }

    s32 result = pthread_mutex_lock((pthread_mutex_t*)madness_mutex->data);
    switch (result)
    {
    case 0:
        // Success, everything else is a failure.
        // KTRACE("Obtained mutex lock.");
        return true;
    case EOWNERDEAD:
        M_ERROR("Owning thread terminated while mutex still active.");
        return false;
    case EAGAIN:
        M_ERROR("Unable to obtain mutex lock: the maximum number of recursive mutex locks has been reached.");
        return false;
    case EBUSY:
        M_ERROR("Unable to obtain mutex lock: a mutex lock already exists.");
        return false;
    case EDEADLK:
        M_ERROR("Unable to obtain mutex lock: a mutex deadlock was detected.");
        return false;
    default:
        M_ERROR("An handled error has occurred while obtaining a mutex lock: errno=%i", result);
        return false;
    }
}

bool mutex_unlock(Madness_Mutex* madness_mutex)
{
    if (!madness_mutex)
    {
        return false;
    }
    if (madness_mutex->data)
    {
        s32 result = pthread_mutex_unlock((pthread_mutex_t*)madness_mutex->data);
        switch (result)
        {
        case 0:
            // KTRACE("Freed mutex lock.");
            return true;
        case EOWNERDEAD:
            M_ERROR("Unable to unlock mutex: owning thread terminated while mutex still active.");
            return false;
        case EPERM:
            M_ERROR("Unable to unlock mutex: mutex not owned by current thread.");
            return false;
        default:
            M_ERROR("An handled error has occurred while unlocking a mutex lock: errno=%i", result);
            return false;
        }
    }

    return false;
}

typedef struct Linux_Semaphore_Internal
{
    sem_t* semaphore;
    char* name;
} Linux_Semaphore_Internal;

static u32 linux_semaphore_id = 0;
bool semaphore_create(Madness_Semaphore* out_semaphore, u32 max_count, u32 start_count)
{
    if (!out_semaphore)
    {
        return false;
    }

    char name_buf[20] = {0};
    c_string_format(name_buf, "/madness_sem_%u", linux_semaphore_id);
    linux_semaphore_id++;

    out_semaphore->data = platform_allocate(sizeof(Linux_Semaphore_Internal), true);
    Linux_Semaphore_Internal* internal = out_semaphore->data;

    if ((internal->semaphore = sem_open(name_buf, O_CREAT, 0664, 0)) == SEM_FAILED)
    {
        M_ERROR("Failed to open semaphore");
        return false;
    }
    internal->name = c_string_duplicate(name_buf);

    return true;
}

void semaphore_destroy(Madness_Semaphore* madness_semaphore)
{
    if (!madness_semaphore)
    {
        return;
    }

    Linux_Semaphore_Internal* internal = madness_semaphore->data;
    if (sem_close(internal->semaphore) == -1)
    {
        M_ERROR("Failed to close semaphore.");
    }

    if (sem_unlink(internal->name) == -1)
    {
        M_ERROR("Failed to unlink semaphore");
    }

    // c_string_free(internal->name);
    platform_free(madness_semaphore->data);
    madness_semaphore->data = 0;
}

bool semaphore_signal(Madness_Semaphore* madness_semaphore)
{
    if (!madness_semaphore || !madness_semaphore->data)
    {
        MASSERT_FALSE()
        return false;
    }

    Linux_Semaphore_Internal* internal = madness_semaphore->data;
    if (sem_post(internal->semaphore) != 0)
    {
        M_ERROR("Semaphore failed to post!");
        MASSERT_FALSE()
        return false;
    }

    return true;
}

bool semaphore_wait(Madness_Semaphore* madness_semaphore, u64 wait_ms)
{
    if (!madness_semaphore || !madness_semaphore->data)
    {
        MASSERT_FALSE()
        return false;
    }


    Linux_Semaphore_Internal* internal = madness_semaphore->data;
    // TODO: handle timeout value using sem_timedwait()
    if (sem_wait(internal->semaphore) != 0)
    {
        M_ERROR("Semaphore failed to wait!");
        return false;
    }

    return true;
}


#include <stdatomic.h>

typedef struct Linux_Atomic_U32
{
    // atomic_uint atomic;
    _Atomic u32 atomic;
} Linux_Atomic_U32;

typedef struct Linux_Atomic_U64
{
    // atomic_uintmax_t atomic;
    _Atomic u64 atomic;
} Linux_Atomic_U64;

void atomic_u32_init(Madness_Atomic_U32* atomic, u32 value, Allocator* allocator)
{
    atomic->data = allocator_alloc(allocator, sizeof(Linux_Atomic_U32));
    Linux_Atomic_U32* linux_atomic = (Linux_Atomic_U32*)atomic->data;
    atomic_init(&linux_atomic->atomic, value);
}

u32 atomic_u32_load(Madness_Atomic_U32* atomic)
{
    Linux_Atomic_U32* linux_atomic = (Linux_Atomic_U32*)atomic->data;
    return atomic_load(&linux_atomic->atomic);
}

void atomic_u32_store(Madness_Atomic_U32* atomic, u32 value)
{
    Linux_Atomic_U32* linux_atomic = (Linux_Atomic_U32*)atomic->data;
    return atomic_store(&linux_atomic->atomic, value);
}

u32 atomic_u32_fetch_add(Madness_Atomic_U32* atomic, u32 value)
{
    Linux_Atomic_U32* linux_atomic = (Linux_Atomic_U32*)atomic->data;
    return atomic_fetch_add(&linux_atomic->atomic, value);
}

u32 atomic_u32_fetch_sub(Madness_Atomic_U32* atomic, u32 value)
{
    Linux_Atomic_U32* linux_atomic = (Linux_Atomic_U32*)atomic->data;
    return atomic_fetch_sub(&linux_atomic->atomic, value);
}

u32 atomic_u32_compare_and_swap(Madness_Atomic_U32* atomic, u32 new_val, u32 old_val)
{
    Linux_Atomic_U64* linux_atomic = (Linux_Atomic_U64*)atomic->data;

    atomic_compare_exchange_strong(&linux_atomic->atomic, &new_val, old_val);
    return new_val;
}

void atomic_u64_init(Madness_Atomic_U64* atomic, u64 value, Allocator* allocator)
{
    atomic->data = allocator_alloc(allocator, sizeof(Linux_Atomic_U64));
    Linux_Atomic_U64* linux_atomic = (Linux_Atomic_U64*)atomic->data;
    atomic_init(&linux_atomic->atomic, value);
}

u32 atomic_u64_load(Madness_Atomic_U64* atomic)
{
    Linux_Atomic_U64* linux_atomic = (Linux_Atomic_U64*)atomic->data;
    return atomic_load(&linux_atomic->atomic);
}

void atomic_u64_store(Madness_Atomic_U64* atomic, u64 value)
{
    Linux_Atomic_U64* linux_atomic = (Linux_Atomic_U64*)atomic->data;
    return atomic_store(&linux_atomic->atomic, value);
}

u32 atomic_u64_fetch_add(Madness_Atomic_U64* atomic, u64 value)
{
    Linux_Atomic_U64* linux_atomic = (Linux_Atomic_U64*)atomic->data;
    return atomic_fetch_sub(&linux_atomic->atomic, value);
}

u32 atomic_u64_fetch_sub(Madness_Atomic_U64* atomic, u64 value)
{
    Linux_Atomic_U64* linux_atomic = (Linux_Atomic_U64*)atomic->data;
    return atomic_fetch_sub(&linux_atomic->atomic, value);
}


#endif
