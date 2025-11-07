#include "renderer_entry.h"

#include "hot_reload.h"
#include "renderer.h"


typedef bool (renderer_initialize)(renderer*);
typedef void (renderer_run)(renderer*);
typedef void (renderer_terminate)(renderer*);

static HMODULE renderer_dll_handle;


void create_renderer(struct renderer* renderer_out)
{
    renderer_out->app_config.start_pos_x = 100;
    renderer_out->app_config.start_pos_y = 100;
    renderer_out->app_config.start_width = 1280;
    renderer_out->app_config.start_height = 720;
    renderer_out->app_config.name = "Madness Engine Renderer";


    load_dll("libMADNESSRENDERER.dll", "libMADNESSRENDERER_TEMP.dll", &renderer_dll_handle);

    renderer_out->renderer_initialize = (renderer_initialize *) GetProcAddress(renderer_dll_handle, "renderer_init");
    renderer_out->renderer_run = (renderer_run *) GetProcAddress(renderer_dll_handle, "renderer_update");
    renderer_out->renderer_shutdown = (renderer_terminate *) GetProcAddress(renderer_dll_handle, "renderer_shutdown");
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
}
