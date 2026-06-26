#include "thread_madness.h"


#if MPLATFORM_WINDOWS
#include <windows.h>
#include <windowsx.h>
#include "logger.h"
#include "platform.h"


s32 get_threads_available()
{
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    INFO("%i processor cores detected.", info.dwNumberOfProcessors);
    return info.dwNumberOfProcessors;
}


bool thread_create(fpn_thread_start start_function_ptr, void* params, bool auto_detach, Madness_Thread* out_thread)
{
    if (!start_function_ptr) return false;

    out_thread->data = CreateThread(0, 0,
                                    (LPTHREAD_START_ROUTINE)start_function_ptr, params, 0,
                                    (DWORD*)&out_thread->thread_id);
    DEBUG("Starting process on thread id: %#x", out_thread->thread_id);
    if (!out_thread->data)
    {
        return false;
    }

    if (auto_detach)
    {
        CloseHandle(out_thread->data);
    }



    return true;
}


void thread_destroy(Madness_Thread* madness_thread)
{
    if (!madness_thread || !madness_thread->data)
    {
        WARN("THREAD DESTROY: INVALID THREAD")
        return;
    }

    DWORD exit_code;
    GetExitCodeThread(madness_thread->data, &exit_code);
    //this is here is it ever becomes an issue
    // if (exit_code == STILL_ACTIVE)
    // {
    // TerminateThread(madness_thread->data, 0);
    // }
    CloseHandle((HANDLE)madness_thread->data);
    madness_thread->data = 0;
    madness_thread->thread_id = 0;
}

void thread_detach(Madness_Thread* madness_thread)
{
    if (!madness_thread || !madness_thread->data)
    {
        WARN("THREAD DETACH: INVALID THREAD")
        return;
    }

    CloseHandle(madness_thread->data);
    madness_thread->data = 0;
}

bool thread_join(Madness_Thread* thread)
{
    if (thread && thread->data)
    {
        DWORD exit_code = WaitForSingleObject(thread->data, INFINITE);
        if (exit_code == WAIT_OBJECT_0)
        {
            return true;
        }
    }
    return false;
}

bool thread_join_timeout(Madness_Thread* thread, u64 wait_ms)
{
    if (thread && thread->data)
    {
        DWORD exit_code = WaitForSingleObject(thread->data, wait_ms);
        if (exit_code == WAIT_OBJECT_0)
        {
            return true;
        }
        else if (exit_code == WAIT_TIMEOUT)
        {
            return false;
        }
    }
    return false;
}


void thread_cancel(Madness_Thread* madness_thread)
{
    if (!madness_thread || !madness_thread->data)
    {
        WARN("THREAD CANCEL: INVALID THREAD")
        return;
    }

    TerminateThread(madness_thread->data, 0);
    madness_thread->data = 0;
}

bool thread_is_active(Madness_Thread* madness_thread)
{
    if (!madness_thread || !madness_thread->data)
    {
        WARN("THREAD IS ACTIVE: INVALID THREAD")
        return false;
    }

    DWORD exit_code = WaitForSingleObject(madness_thread->data, 0);
    if (exit_code == WAIT_TIMEOUT)
    {
        return true;
    }

    return false;
}

void thread_sleep(Madness_Thread* madness_thread, u32 milliseconds)
{
    platform_sleep(milliseconds);
}

u64 thread_get_id()
{
    return (u64)GetCurrentThreadId();
}


// MUTEX

bool mutex_create(Madness_Mutex* out_mutex)
{
    if (!out_mutex)
    {
        WARN("Mutex Create: INVALID MUTEX")
        return false;
    }

    out_mutex->data = CreateMutex(0, false, 0);
    MASSERT_MSG(!out_mutex->data, "Mutex Create: FAILED TO CREATE MUTEX")

    return true;
}

void mutex_destroy(Madness_Mutex* madness_mutex)
{
    MASSERT_MSG(!madness_mutex || !madness_mutex->data, "Mutex Destroy: INVALID MUTEX")

    CloseHandle(madness_mutex->data);
    madness_mutex->data = 0;
}

bool mutex_lock(Madness_Mutex* madness_mutex)
{
    if (!madness_mutex || !madness_mutex->data)
    {
        WARN("Mutex Lock: INVALID MUTEX")
        return false;
    }

    DWORD result = WaitForSingleObject(madness_mutex->data, INFINITE);
    switch (result)
    {
    case WAIT_OBJECT_0: // thread got successful ownership of the mutex
        return true;
    case WAIT_ABANDONED: // thread got ownership of an abandoned mutex
        M_ERROR("Mutex Lock: ABANDONED")
        return false;
    }
    return true;
}

bool mutex_unlock(Madness_Mutex* madness_mutex)
{
    if (!madness_mutex || !madness_mutex->data)
    {
        WARN("Mutex Unlock: INVALID MUTEX")
        return false;
    }
    s32 result = ReleaseMutex(madness_mutex->data);
    return result != 0;
}


// SEMAPHORE

bool semaphore_create(Madness_Semaphore* out_semaphore, u32 max_count, u32 start_count)
{
    if (!out_semaphore)
    {
        return false;
    }

    out_semaphore->data = CreateSemaphore(0, start_count, max_count, 0);

    return true;
}

void ksemaphore_destroy(Madness_Semaphore* semaphore)
{
    if (semaphore && semaphore->data)
    {
        CloseHandle(semaphore->data);
        TRACE("Destroyed semaphore handle.");
        semaphore->data = 0;
    }
}

bool semaphore_signal(Madness_Semaphore* semaphore)
{
    if (!semaphore || !semaphore->data)
    {
        return false;
    }
    // W: release/Increment
    LONG previous_count = 0;
    // NOTE: release 1 at a time.
    if (!ReleaseSemaphore(semaphore->data, 1, &previous_count))
    {
        M_ERROR("Failed to release semaphore.");
        return false;
    }
    return true;
    // L: post/Increment
}

bool semaphore_wait(Madness_Semaphore* semaphore, u64 timeout_ms)
{
    if (!semaphore || !semaphore->data)
    {
        return false;
    }

    DWORD result = WaitForSingleObject(semaphore->data, timeout_ms);
    switch (result)
    {
    case WAIT_ABANDONED:
        M_ERROR(
            "The specified object is a mutex object that was not released by the thread that owned the mutex object before the owning thread terminated. Ownership of the mutex object is granted to the calling thread and the mutex state is set to nonsignaled. If the mutex was protecting persistent state information, you should check it for consistency.");
        return false;
    case WAIT_OBJECT_0:
        // The state is signaled.
        return true;
    case WAIT_TIMEOUT:
        M_ERROR("Semaphore wait timeout occurred.");
        return false;
    case WAIT_FAILED:
        M_ERROR("WaitForSingleObject failed.");
        // TODO: GetLastError and print message.
        return false;
    default:
        M_ERROR("An unknown error occurred while waiting on a semaphore.");
        // TODO: GetLastError and print message.
        return false;
    }
    // W: wait/decrement, blocks when 0
    // L: wait/decrement, blocks when 0
    return true;
}


#endif
