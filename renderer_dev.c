#include "renderer_dev.h"



extern void renderer_dev_create_function_pointers(Renderer_Dev_Application* renderer_app)
{
    DLL_HANDLE render_lib_handle = platform_load_dynamic_library("./MADNESSRENDERER");
    if (render_lib_handle.handle == 0)
    {
        FATAL("FAILED TO LOAD MADNESSRENDERER DLL");
    }

    renderer_app->renderer_application.renderer_initialize = (renderer_initialize ) platform_get_function_address(render_lib_handle, "renderer_init");
    renderer_app->renderer_application.renderer_run = (renderer_run ) platform_get_function_address(render_lib_handle, "renderer_update");
    renderer_app->renderer_application.renderer_terminate= (renderer_terminate )platform_get_function_address(render_lib_handle,"renderer_shutdown");
    renderer_app->renderer_application.renderer_resize = (renderer_resize ) platform_get_function_address(render_lib_handle, "renderer_on_resize");


    if (!renderer_app->renderer_application.renderer_initialize)
    {
        FATAL("FAILED TO SET FUNCTION POINTER RENDER INITIALIZATION")
    }
    if (!renderer_app->renderer_application.renderer_run)
    {
        FATAL("FAILED TO SET FUNCTION POINTER RENDER RUN")
    }
    if (!renderer_app->renderer_application.renderer_terminate)
    {
        FATAL("FAILED TO SET FUNCTION POINTER RENDER TERMINATE")
    }
    if (!renderer_app->renderer_application.renderer_resize)
    {
        FATAL("FAILED TO SET FUNCTION POINTER RENDER RESIZE")
    }
}


