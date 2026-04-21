#ifndef MADNESSENGINE_EDITOR_H
#define MADNESSENGINE_EDITOR_H
#include <stdbool.h>

#include "UI.h"
#include "vulkan_types.h"
#include "memory/memory_system.h"

typedef enum Editor_UI_State
{
    EDITOR_UI_STATE_DEBUG,
    EDITOR_UI_STATE_SCENE,
    EDITOR_UI_STATE_MATERIAL,
    EDITOR_UI_STATE_MAX,
} Editor_UI_State;

typedef struct Editor
{
    Madness_UI* madness_ui; // ref
    Renderer* renderer; // ref
    Resource_System* resource_system; // ref

    Arena* editor_arena;
    Frame_Arena* editor_frame_arena;

    //checked by the applicaiton to see if we should run the game dll
    bool run_game_application;

    Editor_UI_State state;
} Editor;


MAPI Editor* editor_init(Memory_System* memory_system, Renderer* renderer, Madness_UI* madness_ui, Resource_System* resource_system);
MAPI bool editor_update(Editor* editor);
MAPI bool editor_shutdown(Editor* editor);


void editor_ui(Editor* editor);
//UI_States
void editor_ui_debug(Editor* editor);
void editor_ui_scene(Editor* editor);
void editor_material_nodes(Editor* editor);



#endif //MADNESSENGINE_EDITOR_H
