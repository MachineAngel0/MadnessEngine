#ifndef MADNESSENGINE_EDITOR_H
#define MADNESSENGINE_EDITOR_H
#include <stdbool.h>

#include "UI.h"
#include "vulkan_types.h"
#include "memory/memory_system.h"


typedef struct Editor
{
    Madness_UI* madness_ui; // ref
    Renderer* renderer; // ref

    //checked by the applicaiton to see if we should run the game dll
    bool run_game_application;
} Editor;


MAPI Editor* editor_init(Memory_System* memory_system, Renderer* renderer, Madness_UI* madness_ui);
MAPI bool editor_update(Editor* editor);
MAPI bool editor_shutdown(Editor* editor);


void editor_ui(Editor* editor);


typedef Editor* (*editor_init_fpn)(Memory_System*, Renderer*, Madness_UI*);
typedef void (*editor_run_fpn)(Editor* );
typedef void (*editor_shutdown_fpn)(Editor* );


#endif //MADNESSENGINE_EDITOR_H
