#ifndef RENDERER_H
#define RENDERER_H


#include "app_types.h"

MAPI bool renderer_init(Renderer* renderer, Application_Base* application_base);

MAPI void renderer_update(Renderer* renderer, Render_Packet* render_packets, Clock* clock);

MAPI void renderer_shutdown(Renderer* renderer);

MAPI void renderer_on_resize(Renderer* renderer, u32 width, u32 height);

#endif
