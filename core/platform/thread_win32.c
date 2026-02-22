#include "thread_madness.h"


#if MPLATFORM_WINDOWS
#include <windows.h>
#include <windowsx.h>
#include "logger.h"
#include "platform.h"


i32 get_threads_available()
{
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    return info.dwNumberOfProcessors;
}


bool thread_create(pfn_thread_start start_function_ptr, void* params, bool auto_detach, Madness_Thread* out_thread)
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

bool mutex_create(Madness_Mutex* out_mutex)
{
    if (!out_mutex)
    {
        WARN("Mutex Create: INVALID MUTEX")
        return false;
    }

    out_mutex->data = CreateMutex(0, false, 0);
    if (!out_mutex->data)
    {
        M_ERROR("Mutex Create: FAILED TO CREATE MUTEX")
        return false;
    }
    return true;
}

void mutex_destroy(Madness_Mutex* madness_mutex)
{
    if (!madness_mutex || !madness_mutex->data)
    {
        WARN("Mutex Destroy: INVALID MUTEX")
    }

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
    i32 result = ReleaseMutex(madness_mutex->data);
    return result != 0;
}


#endif
