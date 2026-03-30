#ifndef RENDERER_H
#define RENDERER_H


#include "app_types.h"

MAPI bool renderer_init(Renderer_Plugin* renderer_app, Application_Base* application_base);

MAPI void renderer_update(Renderer_Plugin* renderer_app, Application_Base* application_base);

MAPI void renderer_shutdown(Renderer_Plugin* renderer_app);

MAPI void renderer_on_resize(Renderer_Plugin* renderer_app, u32 width, u32 height);

#endif
