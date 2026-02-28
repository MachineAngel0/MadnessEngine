#ifndef RENDERER_H
#define RENDERER_H


#include <stdbool.h>
#include "../app_types.h"

MAPI bool renderer_init(Application_Base* application_base);

MAPI void renderer_update(Clock* clock);

MAPI void renderer_shutdown(void);

MAPI void renderer_on_resize(u32 width, u32 height);

#endif
