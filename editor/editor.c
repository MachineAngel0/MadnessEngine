#include "editor.h"

#include "../app_types.h"
#include "memory/memory_system.h"

Editor* editor_initialize(Memory_System* memory_system, Renderer* renderer, Madness_UI* madness_ui)
{
    // editor // allocate memory for the editor
    Editor* editor = memory_system_alloc(memory_system, sizeof(Editor));
    editor->renderer = renderer;
    editor->madness_ui = madness_ui;

    return editor;
}

bool editor_update(Editor* editor)
{
    //do the ui and stuff
    //manage a bunch of ui state

}

bool editor_shutdown(Editor* editor)
{
    return true;
}
