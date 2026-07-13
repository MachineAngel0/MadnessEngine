#ifndef MADNESSENGINE_EDITOR_H
#define MADNESSENGINE_EDITOR_H
#include <stdbool.h>

#include "../resource/ui_madness.h"
#include "memory/memory_system.h"

typedef enum Editor_UI_State
{
    EDITOR_UI_STATE_DEBUG,
    EDITOR_UI_STATE_ENGINE_STATS,
    EDITOR_UI_STATE_SCENE,
    EDITOR_UI_STATE_TEXTURE_VIEWER,
    EDITOR_UI_STATE_MATERIAL,
    EDITOR_UI_STATE_ANIMATION,
    EDITOR_UI_STATE_MADNESS_UI_TEST,
    EDITOR_UI_STATE_REFLECTION_ABILITY,
    EDITOR_UI_STATE_INSANITY_UI_TEST,
    EDITOR_UI_STATE_MAX,
} Editor_UI_State;


typedef struct Editor
{
    Renderer* renderer; // ref
    Resource_System* resource_system; // ref
    Clock* clock; // ref
    Reflection_Registry* reflection_registry; // ref

    Allocator* editor_arena;
    Frame_Allocator* editor_frame_arena;

    //checked by the applicaiton to see if we should run the game dll
    bool run_game_application;

    float lowest_ms;
    float highest_ms;

    Editor_UI_State state;
} Editor;


MAPI Editor* editor_init(Memory_System* memory_system, Renderer* renderer,
                         Resource_System* resource_system, Clock* clock, Reflection_Registry* reflection_registry);
MAPI bool editor_update(Editor* editor);
MAPI bool editor_shutdown(Editor* editor);


void editor_ui(Editor* editor);
//UI_States
void editor_ui_debug(Editor* editor);
void editor_ui_scene(Editor* editor);
void editor_material_nodes(Editor* editor);
void editor_texture_view(Editor* editor);
void editor_ui_stats(Editor* editor);
void editor_ui_animation(Editor* editor);


#endif //MADNESSENGINE_EDITOR_H
