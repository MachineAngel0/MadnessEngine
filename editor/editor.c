#include "editor.h"

#include "../app_types.h"
#include "memory/memory_system.h"

Editor* editor_init(Memory_System* memory_system, Renderer* renderer, Madness_UI* madness_ui,
                    Resource_System* resource_system)
{
    // editor // allocate memory for the editor
    Editor* editor = memory_system_alloc(memory_system, sizeof(Editor), MEMORY_SUBSYSTEM_EDITOR);
    editor->renderer = renderer;
    editor->madness_ui = madness_ui;
    editor->resource_system = resource_system;
    editor->state = EDITOR_UI_STATE_DEBUG;

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

static float thick = 1.0f;


void editor_ui(Editor* editor)
{
    if (input_key_released_unique(editor->renderer->input_system, KEY_Q))
    {
        editor->state = (editor->state + 1) % EDITOR_UI_STATE_MAX;
    }

    switch (editor->state)
    {
    case EDITOR_UI_STATE_DEBUG:
        editor_ui_debug(editor);
        break;
    case EDITOR_UI_STATE_SCENE:
        editor_ui_scene(editor);
        break;
    case EDITOR_UI_STATE_MATERIAL:
    editor_material_nodes(editor);

        break;
    case EDITOR_UI_STATE_MAX:
        break;
    }
}


void editor_ui_debug(Editor* editor)
{
    Madness_UI* madness_ui = editor->madness_ui;
    // madness_ui_test(madness_ui);

    madness_ui_begin_layout(madness_ui, "Madness Editor", (vec2){5, 5}, (vec2){20, 90});

    if (madness_button_text(madness_ui, "Editor Button", STRING("Editor Button GO BRRR")))
    {
        FATAL("DO A BARREL ROLL");
    };

    if (madness_ui_vec3(madness_ui, "pos", STRING("pos"), &editor->resource_system->scene->transforms[0].position,
                        1.0f))
    {
        transform_mark_dirty(&editor->resource_system->scene->transforms[0]);
    }
    if (madness_ui_vec3(madness_ui, "scale", STRING("scale"), &editor->resource_system->scene->transforms[0].scale,
                        1.0f))
    {
        transform_mark_dirty(&editor->resource_system->scene->transforms[0]);
    }
    // madness_ui_vec3(madness_ui, "pos", STRING("translate"), &translate, 1.0f);

    if (madness_button_text(madness_ui, "button", STRING("translate by 1")))
    {
        vec3 translate = {1, 1, 1};
        transform_translate(&editor->resource_system->scene->transforms[0], translate);
    }

    if (madness_button_text(madness_ui, "material flags enable", STRING("material flags enable")))
    {
        material_system_enable_flag(editor->resource_system->material_system, (Material_Handle){0}, MATERIAL_FLAG_PBR);
    }
    if (madness_button_text(madness_ui, "material flags disable", STRING("material flags disable")))
    {
        material_system_disable_flag(editor->resource_system->material_system, (Material_Handle){0}, MATERIAL_FLAG_PBR);
    }
    // editor->resource_system->scene->transforms[1].is_dirty = true;
    // transform_translate(&editor->resource_system->scene->transforms[1], translate);
    // transform_set_scale(&editor->resource_system->scene->transforms[1], translate);
    // transform_scale(&editor->resource_system->scene->transforms[1], translate);
    // transform_rotate(&editor->resource_system->scene->transforms[0], translate);


    madness_ui_float(madness_ui, "thickess", &thick, 1.f);
    madness_ui_circle(madness_ui, "circle", &thick);
}


void editor_ui_scene(Editor* editor)
{
    Madness_UI* madness_ui = editor->madness_ui;
    // madness_ui_test(madness_ui);

    madness_ui_begin_layout(madness_ui, "Scene", (vec2){5, 5}, (vec2){20, 90});

    static scroll_box_state scroll_box_state_test;
    scroll_box_state_test.max_nodes_to_display = 10;
    madness_scroll_box_begin(madness_ui, "Scene Scroll Box", &scroll_box_state_test);

    for (int i = 0; i < editor->resource_system->scene->transform_count; i++)
    {
        char buffer[50];
        sprintf(buffer, "pos%d", i);

        if (madness_ui_vec3(madness_ui, buffer, STRING(buffer), &editor->resource_system->scene->transforms[i].position,
                            1.0f))
        {
            transform_mark_dirty(&editor->resource_system->scene->transforms[i]);
        }
    }

    madness_scroll_box_end(madness_ui, "Scene Scroll Box", &scroll_box_state_test);


}

void editor_material_nodes(Editor* editor)
{
    Madness_UI* madness_ui = editor->madness_ui;
    String inputs_String[] = {STRING("in1"), STRING("in2")};
    String output_String[] = {STRING("out 1"), STRING("out 2"), STRING("out 3")};
    madness_ui_node(madness_ui, "node", inputs_String, ARRAY_SIZE(inputs_String), output_String,
                    ARRAY_SIZE(output_String));

    static vec2 pos;
    madness_ui_drag_test(madness_ui, &pos);
}
