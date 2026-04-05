#ifndef RENDERER_H
#define RENDERER_H

#include "platform.h"


typedef Renderer* (*renderer_initialize)(
                        Platform_State* platform_state, Platform_Config platform_config,
                        Memory_System* memory_system, Input_System* input_system,
                        Event_System* event_system, Resource_System* resource_system);
typedef void (*renderer_run)(Renderer* renderer, float delta_time);
typedef void (*renderer_terminate)(Renderer* renderer);
typedef void (*renderer_resize)(Renderer* renderer, u32, u32);



MAPI Renderer* renderer_init(
                        Platform_State* platform_state, Platform_Config platform_config,
                        Memory_System* memory_system, Input_System* input_system,
                        Event_System* event_system, Resource_System* resource_system);

MAPI void renderer_update(Renderer* renderer, float delta_time);

MAPI void renderer_shutdown(Renderer* renderer);

MAPI void renderer_on_resize(Renderer* renderer, u32 width, u32 height);

#endif
