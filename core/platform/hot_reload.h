

#ifndef HOT_RELOAD_H
#define HOT_RELOAD_H

#include "../core/defines.h"
#include <windows.h>
#include "../dsa/array.h"

//FUTURE: rn its just for windows, linux at a later time
// the only thing i would want to abstract out is the dll reload, and having the dll handles stored somewhere
// but the setting of the function pointers need to be manually done


MAPI void hot_reload_test(void)
{
    printf("hot_reload_test 10\n");
}

//macro for bieng able to use function names, but with func_ptr appended
#define func_ptr(name) name##_func_ptr

typedef void (func)(void);
static func* hot_reload_var = {0};


//NOTE: idk if this is the design i would want
typedef struct dll_handles
{
    //array of dll's
    Array dll;
}dll_handles;

static HMODULE temp_dll;

//example: reload_dll("libMADNESSDSA.dll", "libMADNESSDSA_.dll");
//a change the copy fails and thus the dll loading fails
MAPI void reload_dll_test(const char* dll_file_name, const char* temp_dll_name)
{
    HMODULE dll = 0;

    //call this every frame for now just to make sure its working
    while (true)
    {
        //unload the dll
        if (dll)
        {
            if (!FreeLibrary(dll))
            {
                WARN("FAILED TO UNLOAD DLL")
            }
        }
        //copy data from our new dll to temp
        CopyFile(dll_file_name, temp_dll_name, 0);
        //load the temp file
        dll = LoadLibrary(temp_dll_name);

        if (!dll)
        {
            WARN("FAILED TO DLL")
            continue;
        }

        //NOTE: i dont know how to pass in a function for the cast, without some macro garbage outside my understanding
        // so as a compromise, ill just have to load the functions myslef

        // EX: //retrieve our function
        hot_reload_var = (func*)GetProcAddress(dll, "hot_reload_test");
        // call the function to make sure its working
        hot_reload_var();

    }

}


MAPI bool load_dll(const char* dll_file_name, const char* temp_dll_name, HMODULE* dll_handle)
{
    if (*dll_handle)
    {
        if (FreeLibrary(*dll_handle) == 0)
        {
            WARN("FAILED TO UNLOAD DLL\n")
            WARN("%d", GetLastError());
        }
    }

    CopyFile(dll_file_name, temp_dll_name, 0);

    return *dll_handle = LoadLibraryA(temp_dll_name);
}

MAPI void* load_function_from_dll(HMODULE* dll_handle, const char* function_name)
{
    return GetProcAddress(*dll_handle, function_name);
}

void* reload_dll_and_function(const char* dll_file_name, const char* temp_dll_name, const char* function_name)
{
    HMODULE dll_handle;

    do
    {
        if (dll_handle)
        {
            if (FreeLibrary(dll_handle) == 0)
            {
                WARN("FAILED TO UNLOAD DLL\n")
                WARN("%d", GetLastError());
            }
        }

        CopyFileA(dll_file_name, temp_dll_name, 0);

        dll_handle = LoadLibraryA(temp_dll_name);
    }while ((!dll_handle));
    return GetProcAddress(dll_handle, function_name);

}






#endif //HOT_RELOAD_H
