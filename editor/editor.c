#include "editor.h"

#include "memory/memory_system.h"

Editor* editor_init(Memory_System* memory_system, Renderer* renderer,
                    Resource_System* resource_system, Clock* clock, Reflection_Registry* reflection_registry)
{
    // editor // allocate memory for the editor
    Editor* editor = memory_system_alloc(memory_system, sizeof(Editor), MEMORY_SUBSYSTEM_EDITOR);

    editor->editor_arena = memory_system_alloc(memory_system, sizeof(Allocator), MEMORY_SUBSYSTEM_EDITOR);
    u64 editor_memory_size = MB(5);

    void* editor_memory = memory_system_alloc(memory_system, editor_memory_size, MEMORY_SUBSYSTEM_EDITOR);
    allocator_init(editor->editor_arena, editor_memory, editor_memory_size);


    editor->editor_frame_arena = memory_system_alloc(memory_system, sizeof(Allocator), MEMORY_SUBSYSTEM_EDITOR);
    void* editor_memory_frame = memory_system_alloc(memory_system, editor_memory_size, MEMORY_SUBSYSTEM_EDITOR);
    allocator_init(editor->editor_frame_arena, editor_memory_frame, editor_memory_size);


    editor->renderer = renderer;
    editor->resource_system = resource_system;
    editor->clock = clock;
    editor->reflection_registry = reflection_registry;

    editor->lowest_ms = INT_MAX;
    editor->highest_ms = 0;
    editor->state = EDITOR_UI_STATE_ENGINE_STATS;
    // editor->state = EDITOR_UI_STATE_INSANITY_UI_TEST;
    // editor->state = EDITOR_UI_STATE_MATERIAL;

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
    case EDITOR_UI_STATE_MAX:
        MASSERT(false);
        break;
    case EDITOR_UI_STATE_DEBUG:
        editor_ui_debug(editor);
        break;
    case EDITOR_UI_STATE_SCENE:
        editor_ui_scene(editor);
        break;
    case EDITOR_UI_STATE_MATERIAL:
        editor_material_nodes(editor);
        break;
    case EDITOR_UI_STATE_TEXTURE_VIEWER:
        editor_texture_view(editor);
        break;
    case EDITOR_UI_STATE_ANIMATION:
        editor_ui_animation(editor);
        break;
    case EDITOR_UI_STATE_INSANITY_UI_TEST:
        // insanity_ui_test();
        break;
    case EDITOR_UI_STATE_MADNESS_UI_TEST:
        // madness_ui_test(editor->madness_ui);
        madness_ui_example();
        break;
    case EDITOR_UI_STATE_ENGINE_STATS:
        editor_ui_stats(editor);
        break;
    case EDITOR_UI_STATE_REFLECTION_ABILITY:
        madness_ui_window_begin(STRING("RUNTIME TESTING"));
        {
            // static u32 i = 0;
            // madness_ui_combo_box_char(renderer_plugin->madness_ui, STRING("combo box box"), &i,
            // Ability_Icon_Type_enum_string, ARRAY_SIZE(Ability_Icon_Type_enum_string));

            if (madness_ui_button(STRING("Serialize Runtime Data")))
            {
                reflection_registry_runtime_serialize_all_data_to_txt_format(editor->reflection_registry);
            }

            madness_ui_reflection_test(editor->reflection_registry, "Heal_Component", "1");
            madness_ui_reflection_test(editor->reflection_registry, "Heal_Component", "2");
            madness_ui_reflection_test(editor->reflection_registry, "Damage_Component", "1");

            // madness_ui_reflection_test(editor->madness_ui, editor->reflection_registry, "Game_Level_Data", "1");


            // madness_ui_reflection_test(renderer_plugin->madness_ui, reflection_registry, "Ability_Component", "1");
            // madness_ui_reflection_test(renderer_plugin->madness_ui, reflection_registry, "Ability_Info", "1");
            // madness_ui_reflection_test(renderer_plugin->madness_ui, reflection_registry, "Ability", "1");
        }
        madness_ui_window_end();


        madness_ui_set_window_pos(700, 100);
        madness_ui_window_begin(STRING("Material Menu"));
        {
            Reflection_Registry* material_reflection_registry = editor->resource_system->material_system->
                                                                        reflection_registry;

            if (madness_ui_button(STRING("Serialize Runtime Data")))
            {
                reflection_registry_runtime_serialize_all_data_to_txt_format(material_reflection_registry);
            }

            madness_ui_reflection_test(material_reflection_registry, "Material_Default", "1");
        }
        madness_ui_window_end();


        break;
    }
}

//testing bezier curves
static vec2s pos1 = {400, 400};
static vec2s pos2 = {500, 600};
static vec2s pos3 = {500, 100};
static vec2s pos4 = {600, 150};

void editor_ui_debug(Editor* editor)
{
    // madness_ui_test(madness_ui);

    madness_ui_window_begin(STRING("Madness Editor"));
    {
        if (madness_ui_button(STRING("Editor Button GO BRRR")))
        {
            FATAL("DO A BARREL ROLL");
        };

        if (madness_ui_vec3(STRING("pos"), &editor->resource_system->scene->transforms[0].position, 1.0f))
        {
            transform_mark_dirty(&editor->resource_system->scene->transforms[0]);
        }
        if (madness_ui_vec3(STRING("scale"), &editor->resource_system->scene->transforms[0].scale, 1.0f))
        {
            transform_mark_dirty(&editor->resource_system->scene->transforms[0]);
        }
        // madness_ui_vec3(madness_ui, "pos", STRING("translate"), &translate, 1.0f);

        if (madness_ui_button(STRING("translate by 1")))
        {
            vec3s translate = {1, 1, 1};
            transform_translate(&editor->resource_system->scene->transforms[0], translate);
        }

        /*if (madness_ui_button(madness_ui, STRING("material flags enable")))
        {
            material_system_enable_flag(editor->resource_system->material_system, (Material_Handle){0},
                                        MATERIAL_FLAG_PBR);
        }
        if (madness_ui_button(madness_ui, STRING("material flags disable")))
        {
            material_system_disable_flag(editor->resource_system->material_system, (Material_Handle){0},
                                         MATERIAL_FLAG_PBR);
        }*/
        // editor->resource_system->scene->transforms[1].is_dirty = true;
        // transform_translate(&editor->resource_system->scene->transforms[1], translate);
        // transform_set_scale(&editor->resource_system->scene->transforms[1], translate);
        // transform_scale(&editor->resource_system->scene->transforms[1], translate);
        // transform_rotate(&editor->resource_system->scene->transforms[0], translate);


        madness_ui_float(STRING("thickess"), &thick, 1.f);
        madness_ui_circle(STRING("circle"), &thick);

        static f32 rot;
        array_get(madness_ui->ui_nodes, UI_Node, 0).rotation = rot;
        if (madness_ui_float(STRING("material flags disable"), &rot, 15.f))
        {
        }

        // madness_ui_quadratic_bezier(madness_ui, &pos1, &pos2, &pos3);
        madness_ui_cubic_bezier(&pos1, &pos2, &pos3, &pos4);
    }
    madness_ui_window_end();
}

void editor_ui_stats(Editor* editor)
{
    Clock* clock = editor->clock;

    madness_ui_set_window_pos(50, 50);

    madness_ui_window_begin(STRING("Stats"));
    {
        float ms = clock_delta_time_in_ms(clock);
        float fps = clock_delta_time_to_fps(clock);
        madness_ui_float(STRING("ms"), &ms, 1);
        madness_ui_float(STRING("fps:"), &fps, 1);

        editor->lowest_ms = min_f(editor->lowest_ms, ms);
        editor->highest_ms = max_f(editor->highest_ms, ms);

        madness_ui_float(STRING("low ms"), &editor->lowest_ms, 1);
        madness_ui_float(STRING("high ms"), &editor->highest_ms, 1);
    }
    madness_ui_window_end();
}

void editor_ui_animation(Editor* editor)
{
    Mesh_System* mesh_system = editor->resource_system->mesh_system;
    madness_ui_window_begin(STRING("Animation Data"));
    {
        for (u32 i = 0; i < mesh_system->skinned_mesh_instance_count; i++)
        {
            Sk_Mesh_Parent_Instance* sk_mesh = &mesh_system->skinned_mesh_instance[i];
            Animation_Data* animation_data = sk_mesh_parent_instance_get_animation_data(
                mesh_system, sk_mesh);
            sk_mesh->current_animation_index;

            char buffer[100];
            char buffer2[100];
            snprintf(buffer2, 100, "Animation Index %d", i);
            snprintf(buffer, 100, "Animation Looping %d", i);

            if (madness_ui_u32(STRING_STRLEN(buffer), &sk_mesh->current_animation_index, 1))
            {
                sk_mesh->current_animation_index = clamp_uint(
                    mesh_system->skinned_mesh_instance[i].current_animation_index, 0,
                    animation_data->animations_count-1);


            }
            madness_ui_check_box(STRING_STRLEN(buffer2) ,&sk_mesh->looping);


        }
    }
    madness_ui_window_end();
}


void editor_ui_scene(Editor* editor)
{
    Light_System* light_system = editor->renderer->light_system;

    madness_ui_set_window_pos(50, 50);
    madness_ui_window_begin(STRING("Scene"));
    {
        madness_scroll_box_begin(STRING("Scene Scroll Box"));

        for (int i = 0; i < editor->resource_system->scene->transform_count; i++)
        {
            char buffer_transform[50];
            char buffer_rotation[50];
            char buffer_scale[50];
            snprintf(buffer_transform, 50, "pos%d", i);
            snprintf(buffer_rotation, 50, "rotation%d", i);
            snprintf(buffer_scale, 50, "scale%d", i);

            if (madness_ui_vec3(STRING(buffer_transform), &editor->resource_system->scene->transforms[i].position,
                                1.0f))
            {
                transform_mark_dirty(&editor->resource_system->scene->transforms[i]);
            }
            if (madness_ui_vec3(STRING(buffer_rotation), &editor->resource_system->scene->transforms[i].euler_angles,
                                1.0f))
            {
                transform_rotate_euler(&editor->resource_system->scene->transforms[i]);
            }

            if (madness_ui_vec3(STRING(buffer_scale), &editor->resource_system->scene->transforms[i].scale, 1.0f))
            {
                transform_mark_dirty(&editor->resource_system->scene->transforms[i]);
            }
        }

        madness_scroll_box_end();
    }
    madness_ui_window_end();

    madness_ui_set_window_pos(400, 50);
    madness_ui_window_begin(STRING("Lights"));
    {
        madness_scroll_box_begin(STRING("Lights Scroll Box"));
        {
            for (u32 i = 0; i < light_system->directional_light_count; ++i)
            {
                char buffer[50];
                char buffer2[50];
                snprintf(buffer, 50, "dir_light direction%d", i);
                snprintf(buffer2, 50, "dir_light color%d", i);
                Directional_Light* directional_light = &light_system->directional_lights[i];
                madness_ui_vec3(STRING(buffer), &directional_light->direction, 1.0f);
                madness_ui_vec3(STRING(buffer2), &directional_light->color, 0.1f);
            }
            for (u32 i = 0; i < light_system->point_light_count; ++i)
            {
                char buffer[50];
                char buffer2[50];
                sprintf(buffer, "point_light%d", i);
                sprintf(buffer2, "point_light_color%d", i);
                Point_Light* point_light = &light_system->point_lights[i];
                madness_ui_vec3(STRING(buffer), &point_light->position, 1.0f);
                madness_ui_vec3(STRING(buffer2), &point_light->color, 0.1f);
            }
        }
        madness_scroll_box_end();
    }
    madness_ui_window_end();
}

void editor_material_nodes(Editor* editor)
{
    String inputs_String[] = {STRING("in1"), STRING("in2")};
    String output_String[] = {STRING("out 1"), STRING("out 2"), STRING("out 3")};
    madness_ui_node(STRING("node"), inputs_String, ARRAY_SIZE(inputs_String), output_String, ARRAY_SIZE(output_String));

    static vec2s pos;
    madness_ui_drag_test(&pos);

    //think of it like a param node
    madness_ui_node_simple(STRING("node"), (vec2s){200, 200}, NULL, 0, output_String,
                           ARRAY_SIZE(output_String), 1);


    String inputs_String2[] = {STRING("other in 1"), STRING("other in 2")};

    //takes inputs from the param
    madness_ui_node_simple(STRING("node"), (vec2s){500, 200}, inputs_String2, ARRAY_SIZE(inputs_String2), output_String,
                           ARRAY_SIZE(output_String), 2);

    // madness_ui_node_complex(madness_ui, "node", inputs_String, ARRAY_SIZE(inputs_String), output_String,
    // ARRAY_SIZE(output_String));
}

void editor_texture_view(Editor* editor)
{
    Texture_System* texture_system = editor->resource_system->texture_system;
    madness_ui_window_begin(STRING("Texture View"));
    {
        for (u32 i = 0; i < texture_system->in_use_textures_count; i++)
        {
            Resource_MetaData* meta_data = &texture_system->texture_meta_data[i];
            madness_ui_c_string(meta_data->file_path);
            madness_image_handle((Texture_Handle){.handle = meta_data->id});
        }
    }
    madness_ui_window_end();
}
