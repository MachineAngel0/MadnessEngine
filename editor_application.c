#include "application.h"
#include "platform.h"




typedef struct application_state {
    game* game;
    editor* editor;

    platform_state platform;

    b8 is_running;
    b8 is_suspended;

    i16 width;
    i16 height;

} application_state;

static application_state app_state;

bool application_editor_create(struct editor* editor)
{

};

MAPI void application_editor_run()
{

};