#ifndef APPLICATION_H
#define APPLICATION_H

#include "app_types.h"
#include "defines.h"


MAPI bool application_game_create(struct game_fake* game);
MAPI void application_game_run();



MAPI void application_editor_create(struct editor_fake* editor);
MAPI void application_editor_run();


#endif