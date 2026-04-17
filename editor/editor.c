#include "editor.h"

#include "../app_types.h"
#include "memory/memory_system.h"

Editor* editor_init(Memory_System* memory_system, Renderer* renderer, Madness_UI* madness_ui, Resource_System* resource_system)
{
    // editor // allocate memory for the editor
    Editor* editor = memory_system_alloc(memory_system, sizeof(Editor), MEMORY_SUBSYSTEM_EDITOR);
    editor->renderer = renderer;
    editor->madness_ui = madness_ui;
    editor->resource_system = resource_system;

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

    if (madness_ui_vec3(madness_ui, "pos", STRING("pos"), &editor->resource_system->scene->transforms[1].position, 1.0f))
    {
        transform_mark_dirty(&editor->resource_system->scene->transforms[1]);
    }
    if (madness_ui_vec3(madness_ui, "scale", STRING("scale"), &editor->resource_system->scene->transforms[1].scale, 1.0f))
    {
        transform_mark_dirty(&editor->resource_system->scene->transforms[1]);
    }
    // madness_ui_vec3(madness_ui, "pos", STRING("translate"), &translate, 1.0f);

    if (madness_button(madness_ui, "button"))
    {
        vec3 translate = {1,1,1};
        transform_translate(&editor->resource_system->scene->transforms[1], translate);
    }
    // editor->resource_system->scene->transforms[1].is_dirty = true;
    // transform_translate(&editor->resource_system->scene->transforms[1], translate);
    // transform_set_scale(&editor->resource_system->scene->transforms[1], translate);
    // transform_scale(&editor->resource_system->scene->transforms[1], translate);
    // transform_rotate(&editor->resource_system->scene->transforms[0], translate);


}
