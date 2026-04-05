#ifndef RENDERER_H
#define RENDERER_H

#include "platform.h"

MAPI bool renderer_init(Renderer* renderer,
                        Platform_State* platform_state, Platform_Config platform_config,
                        Memory_System* memory_system, Input_System* input_system,
                        Event_System* event_system, Resource_System* resource_system);

MAPI void renderer_update(Renderer* renderer, float delta_time);

MAPI void renderer_shutdown(Renderer* renderer);

MAPI void renderer_on_resize(Renderer* renderer, u32 width, u32 height);

#endif
