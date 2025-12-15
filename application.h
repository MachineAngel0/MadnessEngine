#ifndef APPLICATION_H
#define APPLICATION_H

#include "app_types.h"




 bool application_game_create(struct game_app* game);
 void application_game_run();


 bool application_renderer_create(struct renderer_app* renderer);
 void application_renderer_run();


 bool application_editor_create(struct editor_app* editor);
 void application_editor_run();


#endif