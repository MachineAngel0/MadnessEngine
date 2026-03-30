#pragma once

#include "app_types.h"

extern void renderer_dev_create_fpn(Renderer_Dev_Application* renderer_app);


void main(void)
{
    Renderer_Dev_Application renderer_dev_app;
    renderer_dev_create_fpn(&renderer_dev_app);
    renderer_dev_run(&renderer_dev_app);
}
