#include "renderer_entry.h"

#include "app_types.h"
#include "clock.h"

typedef bool (renderer_initialize)(renderer_app*);
typedef void (renderer_run)(renderer_app*, Clock* clock);
typedef void (renderer_terminate)(renderer_app*);
typedef void (renderer_resize)(renderer_app*, u32, u32);



void create_renderer(struct renderer_app* renderer_out)
{
    renderer_out->app_config.start_pos_x = 100;
    renderer_out->app_config.start_pos_y = 100;
    renderer_out->app_config.start_width = 1280;
    renderer_out->app_config.start_height = 720;
    renderer_out->app_config.name = "Madness Engine Renderer";


    DLL_HANDLE render_lib_handle = platform_load_dynamic_library("./libMADNESSRENDERER");

    renderer_out->renderer_initialize = (renderer_initialize *) platform_get_function_address(render_lib_handle, "renderer_init");
    renderer_out->renderer_run = (renderer_run *) platform_get_function_address(render_lib_handle, "renderer_update");
    renderer_out->renderer_shutdown = (renderer_terminate *)platform_get_function_address(render_lib_handle,"renderer_shutdown");
    renderer_out->on_resize = (renderer_resize *) platform_get_function_address(render_lib_handle, "renderer_on_resize");
    if (!renderer_out->renderer_initialize)
    {
        FATAL("FAILED TO SET FUNCTION POINTER RENDER INITIALIZATION")
    }
    if (!renderer_out->renderer_run)
    {
        FATAL("FAILED TO SET FUNCTION POINTER RENDER RUN")
    }
    if (!renderer_out->renderer_shutdown)
    {
        FATAL("FAILED TO SET FUNCTION POINTER RENDER SHUTDOWN")
    }
    if (!renderer_out->on_resize)
    {
        FATAL("FAILED TO SET FUNCTION POINTER RENDER RESIZE")
    }
}
