#include "app_types.h"

#include "platform.h"


bool renderer_plugin_set_default_fpn(Renderer_Plugin* renderer_plugin)
{
    renderer_plugin->renderer_dll_handle = platform_load_dynamic_library("./MADNESSRENDERER");
    DLL_HANDLE render_lib_handle = renderer_plugin->renderer_dll_handle;

    if (render_lib_handle.handle == 0)
    {
        FATAL("FAILED TO LOAD MADNESSRENDERER DYNAMIC LIBRARY");
        return false;
    }

    renderer_plugin->renderer_initialize = (renderer_initialize)platform_get_function_address(
        render_lib_handle, "renderer_init");
    renderer_plugin->renderer_run = (renderer_run)platform_get_function_address(render_lib_handle, "renderer_update");
    renderer_plugin->renderer_terminate = (renderer_terminate)platform_get_function_address(
        render_lib_handle, "renderer_shutdown");
    renderer_plugin->renderer_resize = (renderer_resize)platform_get_function_address(
        render_lib_handle, "renderer_on_resize");


    if (!renderer_plugin->renderer_initialize)
    {
        FATAL("FAILED TO SET FUNCTION POINTER RENDER INITIALIZATION")
        return false;
    }
    if (!renderer_plugin->renderer_run)
    {
        FATAL("FAILED TO SET FUNCTION POINTER RENDER RUN")
        return false;
    }
    if (!renderer_plugin->renderer_terminate)
    {
        FATAL("FAILED TO SET FUNCTION POINTER RENDER TERMINATE")
        return false;
    }
    if (!renderer_plugin->renderer_resize)
    {
        FATAL("FAILED TO SET FUNCTION POINTER RENDER RESIZE")
        return false;
    }



    return true;
}


