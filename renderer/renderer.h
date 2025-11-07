#ifndef RENDERER_H
#define RENDERER_H


#include <stdbool.h>
#include "../app_types.h"

MAPI bool renderer_init(struct renderer* renderer_inst);
MAPI void renderer_update(struct renderer* renderer_inst);
MAPI void renderer_shutdown(struct renderer* renderer_inst);


#endif
