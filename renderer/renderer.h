#ifndef RENDERER_H
#define RENDERER_H


#include <stdbool.h>
#include "../app_types.h"

MAPI bool renderer_init(struct renderer_app* renderer_inst, Arena* arena);

MAPI void renderer_update(struct renderer_app* renderer_inst, Clock* clock);

MAPI void renderer_shutdown(struct renderer_app* renderer_inst);

MAPI void renderer_on_resize(struct renderer_app* renderer_inst, u32 width, u32 height);

#endif
