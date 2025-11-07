#ifndef APPLICATION_H
#define APPLICATION_H

#include "app_types.h"
#include "defines.h"

 bool application_game_create(struct game* game);
 void application_game_run();


 bool application_renderer_create(struct renderer* renderer);
 void application_renderer_run();


 bool application_editor_create(struct editor* editor);
 void application_editor_run();


#endif