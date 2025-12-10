#ifndef RENDERER_H
#define RENDERER_H


#include <stdbool.h>
#include "../app_types.h"

MAPI bool renderer_init(struct renderer* renderer_inst);

MAPI void renderer_update(struct renderer* renderer_inst, Clock* clock);

MAPI void renderer_shutdown(struct renderer* renderer_inst);

MAPI void renderer_on_resize(struct renderer* renderer_inst, u32 width, u32 height);

#endif
