

#ifndef HOT_RELOAD_H
#define HOT_RELOAD_H

#include "../core/defines.h"
#include <windows.h>

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


//example: reload_dll("libMADNESSDSA.dll", "libMADNESSDSA_.dll");
//a change the copy fails and thus the dll loading fails
MAPI void reload_dll(const char* dll_file_name, const char* temp_dll_name)
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

        //EX: //retrieve our function
        hot_reload_var = (func*)GetProcAddress(dll, "hot_reload_test");
        // call the function to make sure its working
        hot_reload_var();

    }


}




#endif //HOT_RELOAD_H
