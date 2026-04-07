#include "editor.h"

#include "../app_types.h"
#include "memory/memory_system.h"

Editor* editor_init(Memory_System* memory_system, Renderer* renderer, Madness_UI* madness_ui)
{
    // editor // allocate memory for the editor
    Editor* editor = memory_system_alloc(memory_system, sizeof(Editor), TODO);
    editor->renderer = renderer;
    editor->madness_ui = madness_ui;

    return editor;
}

bool editor_update(Editor* editor)
{
    //do the ui and stuff
    //manage a bunch of ui state

    editor_ui(editor);

    return true;
}

bool editor_shutdown(Editor* editor)
{
    return true;
}

void editor_ui(Editor* editor)
{
    Madness_UI* madness_ui = editor->madness_ui;
    // madness_ui_test(madness_ui);


    madness_ui_begin_layout(madness_ui, "Madness Editor", (vec2){5, 5}, (vec2){20, 90});

    if (madness_button_text(madness_ui, "Editor Button", STRING("Editor Button GO BRRR")))
    {
        FATAL("DO A BARREL ROLL");
    };
}
