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
}Editor;

Editor* editor_initialize(Memory_System* memory_system, Renderer* renderer, Madness_UI* madness_ui);
bool editor_update(Editor* editor);
bool editor_shutdown(Editor* editor);


#endif //MADNESSENGINE_EDITOR_H