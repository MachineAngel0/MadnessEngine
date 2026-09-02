#include "editor.h"

#include "memory/memory_system.h"

Editor* editor_init(Memory_System* memory_system, Renderer* renderer,
                    Asset_System* resource_system, Clock* clock, Reflection_Registry* reflection_registry,
                    Reflection_Registry* material_registry)
{
    // editor // allocate memory for the editor
    Editor* editor = memory_system_alloc(memory_system, sizeof(Editor), MEMORY_SUBSYSTEM_EDITOR);

    editor->editor_arena = memory_system_alloc(memory_system, sizeof(Allocator), MEMORY_SUBSYSTEM_EDITOR);
    u64 editor_memory_size = MB(5);

    void* editor_memory = memory_system_alloc(memory_system, editor_memory_size, MEMORY_SUBSYSTEM_EDITOR);
    allocator_init(editor->editor_arena, editor_memory, editor_memory_size);


    editor->editor_frame_allocator = memory_system_alloc(memory_system, sizeof(Allocator), MEMORY_SUBSYSTEM_EDITOR);
    void* editor_memory_frame = memory_system_alloc(memory_system, editor_memory_size, MEMORY_SUBSYSTEM_EDITOR);
    allocator_init(editor->editor_frame_allocator, editor_memory_frame, editor_memory_size);


    editor->renderer = renderer;
    editor->asset_system = resource_system;
    editor->clock = clock;
    editor->reflection_registry = reflection_registry;
    editor->memory_system = memory_system;

    editor->lowest_ms = INT_MAX;
    editor->highest_ms = 0;
    editor->state = EDITOR_UI_STATE_PARTICLE;;
    // editor->state = EDITOR_UI_STATE_INSANITY_UI_TEST;
    // editor->state = EDITOR_UI_STATE_MATERIAL;

    editor_generate_asset_lists(editor, memory_system);


    return editor;
}

bool editor_update(Editor* editor)
{
    PROFILE_ZONE(editor_update)


    //do the ui and stuff
    //manage a bunch of ui state
    if (input_key_released_unique(KEY_E))
    {
        editor->state = (editor->state + 1) % EDITOR_UI_STATE_MAX;
    }

    if (input_key_released_unique(KEY_Q))
    {
        if (editor->state == 0)
        {
            editor->state = EDITOR_UI_STATE_MAX;
        }
        else
        {
            editor->state = (editor->state - 1);
        }
    }

    editor_ui(editor);

    PROFILE_ZONE_END(editor_update)

    return true;
}

bool editor_shutdown(Editor* editor)
{
    return true;
}

bool editor_generate_asset_lists(Editor* editor, Memory_System* memory_system)
{
    //meshes
    editor->texture_list =
        asset_lists_generate(memory_system,
                             MAX_ASSETS_STRINGS,
                             "../z_assets_engine/texture");

    //meshes
    editor->madness_mesh_list =
        asset_lists_generate(memory_system,
                             MAX_ASSETS_STRINGS,
                             "../z_assets_engine/mesh");


    //skeletal meshes
    editor->madness_skmesh_list =
        asset_lists_generate(memory_system,
                             MAX_ASSETS_STRINGS,
                             "../z_assets_engine/skinned_mesh");

    //scenes
    editor->scene_list =
        asset_lists_generate(memory_system,
                             MAX_ASSETS_STRINGS,
                             "../z_assets_engine/scene");


    //scenes
    editor->particle_effect_list =
        asset_lists_generate(memory_system,
                             MAX_ASSETS_STRINGS,
                             "../z_assets_engine/particle/particle_effect");
    editor->particle_emitter_list =
        asset_lists_generate(memory_system,
                             MAX_ASSETS_STRINGS,
                             "../z_assets_engine/particle/particle_emitter");

    madness_ui_add_asset_list(editor->texture_list, ASSET_TEXTURE);
    madness_ui_add_asset_list(editor->madness_mesh_list, ASSET_STATIC_MESH);
    madness_ui_add_asset_list(editor->madness_skmesh_list, ASSET_SKINNED_MESH);
    madness_ui_add_asset_list(editor->scene_list, ASSET_SCENE);


    return true;
}


static float thick = 1.0f;


void editor_ui(Editor* editor)
{
    switch (editor->state)
    {
    case EDITOR_UI_STATE_MAX:
        break;
    case EDITOR_UI_STATE_DEBUG:
        editor_ui_debug(editor);
        break;
    case EDITOR_UI_STATE_RENDERER:
        editor_render_view(editor);
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
        insanity_ui_test();
        break;
    case EDITOR_UI_STATE_MADNESS_UI_TEST:
        // madness_ui_test();
        madness_ui_example();
        // madness_ui_window_testing();
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

            madness_ui_reflection_runtime_registry(editor->reflection_registry, "Heal_Component", "1");
            madness_ui_reflection_runtime_registry(editor->reflection_registry, "Heal_Component", "2");
            madness_ui_reflection_runtime_registry(editor->reflection_registry, "Damage_Component", "1");

            // madness_ui_reflection_test(editor->madness_ui, editor->reflection_registry, "Game_Level_Data", "1");


            // madness_ui_reflection_test(renderer_plugin->madness_ui, reflection_registry, "Ability_Component", "1");
            // madness_ui_reflection_test(renderer_plugin->madness_ui, reflection_registry, "Ability_Info", "1");
            // madness_ui_reflection_test(renderer_plugin->madness_ui, reflection_registry, "Ability", "1");
        }
        madness_ui_window_end();


        madness_ui_set_window_pos(700, 100);
        madness_ui_window_begin(STRING("Material Menu"));
        {
            Reflection_Registry* material_reflection_registry = editor->asset_system->material_system->
                                                                        reflection_registry;

            if (madness_ui_button(STRING("Serialize Runtime Data")))
            {
                reflection_registry_runtime_serialize_all_data_to_txt_format(material_reflection_registry);
            }

            madness_ui_reflection_runtime_registry(material_reflection_registry, "Material_Default", "1");
        }
        madness_ui_window_end();


        break;
    case EDITOR_UI_STATE_ASSET_METADATA:
        editor_meta_data_view(editor);

        break;
    case EDITOR_UI_STATE_MATERIAL_CREATION:
        editor_material_asset_view(editor);
        break;
    case EDITOR_UI_STATE_MESH_VIEWER:
        editor_mesh_view(editor);
        break;
    case EDITOR_UI_STATE_PARTICLE:
        editor_particle_view(editor);
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

        if (madness_ui_vec3(STRING("pos"), &editor->asset_system->scene->transforms[0].position, 1.0f))
        {
            transform_mark_dirty(&editor->asset_system->scene->transforms[0]);
        }
        if (madness_ui_vec3(STRING("scale"), &editor->asset_system->scene->transforms[0].scale, 1.0f))
        {
            transform_mark_dirty(&editor->asset_system->scene->transforms[0]);
        }
        // madness_ui_vec3(madness_ui, "pos", STRING("translate"), &translate, 1.0f);

        if (madness_ui_button(STRING("translate by 1")))
        {
            vec3s translate = {1, 1, 1};
            transform_translate(&editor->asset_system->scene->transforms[0], translate);
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
    Animation_System* animation_system = editor->asset_system->animation_system;
    madness_ui_window_begin(STRING("Animation Data"));
    {
        for (u32 i = 0; i < animation_system->animation_count; i++)
        {
            Madness_Animation* madness_animation = &animation_system->animation_data[i];
            GLTF_Animation_Data* animation_data = madness_animation->animation_data;
            madness_animation->current_animation_index;

            char buffer[100];
            char buffer2[100];
            snprintf(buffer, 100, "Animation Index %d", i);
            snprintf(buffer2, 100, "Animation Looping %d", i);

            madness_ui_float(STRING("Animation Current Time:"), &madness_animation->current_time, 0);
            if (madness_ui_u32(STRING_STRLEN(buffer), &madness_animation->current_animation_index, 1))
            {
                madness_animation->current_animation_index = clamp_uint(
                    madness_animation->current_animation_index, 0,
                    animation_data->animations_count - 1);

                madness_animation->current_time = 0;
            }
            if (!madness_ui_check_box(STRING_STRLEN(buffer2), &madness_animation->looping))
            {
                madness_animation->current_time = 0;
            }
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
        // madness_scroll_box_begin(STRING("Scene Scroll Box"));

        for (int i = 0; i < editor->asset_system->scene->transform_count; i++)
        {
            char buffer_transform[50];
            char buffer_rotation[50];
            char buffer_scale[50];
            snprintf(buffer_transform, 50, "pos%d", i);
            snprintf(buffer_rotation, 50, "rotation%d", i);
            snprintf(buffer_scale, 50, "scale%d", i);

            //TODO: have the uuid here to identify the thing in the world

            /*if (madness_ui_vec3(STRING(buffer_transform), &editor->asset_system->scene->transforms[i].position,
                                1.0f))
            {
                transform_mark_dirty(&editor->asset_system->scene->transforms[i]);
            }*/

            if (madness_ui_vec3(STRING(buffer_transform), &editor->asset_system->scene->transforms[i].position,
                                1.0f))
            {
                transform_mark_dirty(&editor->asset_system->scene->transforms[i]);
            }
            if (madness_ui_vec3(STRING(buffer_rotation), &editor->asset_system->scene->transforms[i].euler_angles,
                                1.0f))
            {
                transform_rotate_euler(&editor->asset_system->scene->transforms[i]);
            }

            if (madness_ui_vec3(STRING(buffer_scale), &editor->asset_system->scene->transforms[i].scale, 1.0f))
            {
                transform_mark_dirty(&editor->asset_system->scene->transforms[i]);
            }
        }

        // madness_scroll_box_end();
    }
    madness_ui_window_end();

    madness_ui_set_window_pos(400, 50);
    madness_ui_window_begin(STRING("Lights"));
    {
        // madness_scroll_box_begin(STRING("Lights Scroll Box"));
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
                madness_ui_vec4(STRING(buffer), &point_light->position, 1.0f);
                madness_ui_vec4(STRING(buffer2), &point_light->color, 0.1f);
            }


            for (u32 i = 0; i < light_system->spot_light_count; ++i)
            {
                char buffer[50];
                char buffer2[50];
                sprintf(buffer, "spot_light%d", i);
                sprintf(buffer2, "spot_light_color%d", i);
                Spot_Light* spot_light = &light_system->spot_lights[i];
                madness_ui_vec4(STRING(buffer), &spot_light->position, 1.0f);
                madness_ui_vec4(STRING(buffer2), &spot_light->color, 0.1f);
            }
        }
        // madness_scroll_box_end();
    }
    madness_ui_window_end();


    // scene serialization
    madness_ui_set_window_pos(1200, 50);
    madness_ui_window_begin(STRING("Scene Serialization"));
    {
        if (madness_ui_button(STRING("SCENE SAVE")))
        {
            scene_save(editor->asset_system->scene);
        }

        String scene_name = STRING("SCENE LOAD");
        madness_ui_text_box(scene_name);

        if (madness_ui_button(STRING("SCENE LOAD")))
        {
            String_Builder* string_builder = madness_ui_text_box_get_string(scene_name);
            if (string_builder)
            {
                scene_load(editor->asset_system->scene,
                           string_builder_to_string_allocator(string_builder, editor->editor_frame_allocator));
            }
        }

        String scene_mesh_asset_name = STRING("MESH PATH");
        madness_ui_file_picker(scene_mesh_asset_name);
        if (madness_ui_button(STRING("LOAD MESH ASSET")))
        {
            asset_load_mesh_path(editor->asset_system,
                                 string_builder_to_c_string(madness_ui_text_box_get_string(scene_mesh_asset_name)));
        }

        String mesh_path2 = STRING("Mesh Path2");
        Path_String path_string;
        madness_ui_combo_box_string(mesh_path2, &path_string,
                                    editor->madness_mesh_list->strings,
                                    editor->madness_mesh_list->count);

        String scene_name2 = STRING("SCENE LOAD2");
        if (madness_ui_button(STRING("LOAD MESH ASSET2")))
        {
            asset_load_mesh_path(editor->asset_system,
                                 string_to_c_string(&path_string));
        }
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
    Asset_System* asset_system = editor->asset_system;
    madness_ui_window_begin(STRING("Texture View"));
    {
        for (u32 i = 0; i < asset_system->asset_registry->asset_meta_data->num_items; i++)
        {
            Asset_MetaData* meta_data = _dynamic_array_get(asset_system->asset_registry->asset_meta_data, i);
            if (meta_data->type != ASSET_TEXTURE && meta_data->type != ASSET_FONT)
            {
                continue;
            }


            Texture_Handle texture_handle;
            if (texture_system_exists(asset_system, &texture_handle, meta_data->hash))
            {
                madness_ui_string(*meta_data->source_file);
                madness_ui_string(*meta_data->engine_path);
                madness_image_handle(texture_handle);
            }


            // madness_image_handle((Texture_Handle){.handle = meta_data.handle_lookup});
        }
    }
    madness_ui_window_end();
}

void editor_meta_data_view(Editor* editor)
{
    Asset_System* asset_system = editor->asset_system;
    madness_ui_window_begin(STRING("MetaData View"));
    {
        for (u32 i = 0; i < asset_system->asset_registry->asset_meta_data->num_items; i++)
        {
            Asset_MetaData* meta_data = _dynamic_array_get(asset_system->asset_registry->asset_meta_data, i);
            madness_ui_string(STRING("UUID:"));
            madness_ui_u32(STRING("id1"), &meta_data->uuid.high, 0);
            madness_ui_u32(STRING("id2"), &meta_data->uuid.low, 0);
            madness_ui_same_line();

            madness_ui_c_string(ASSET_TYPE_LUT[meta_data->type]);

            madness_ui_string(*meta_data->source_file);
            madness_ui_string(*meta_data->engine_path);
        }
    }
    madness_ui_window_end();
}

void editor_material_asset_view(Editor* editor)
{
    Asset_System* asset_system = editor->asset_system;
    madness_ui_window_begin(STRING("Material Reflection View"));
    {
        for (u32 i = 0; i < asset_system->material_system->material_batch_count; i++)
        {
            Material_Batch* batch = &asset_system->material_system->material_batch[i];
            batch->material_asset->material_info.blend_mode;
            batch->material_asset->material_info.mesh_type;
            batch->material_asset->material_info.renderpass;
            batch->material_asset->material_info.transluency;


            /*
            madness_ui_reflect_data(Reflection_Registry* reflection_registry, struct_info,
                                   void* passin_data, const char* id)
            */
        }
    }
    madness_ui_window_end();


    madness_ui_set_window_pos(600, 200);

    static Material_Info material_info;
    static u8 fuck_you_memory[512];
    static Material_Info mat_info;
    if (!mat_info.material_name)
    {
        mat_info.material_name = STRING_CREATE("0");
    }
    if (!mat_info.shader_name)
    {
        mat_info.shader_name = STRING_CREATE("0");
    }
    madness_ui_window_begin(STRING("Material Creation"));
    {
        madness_ui_button(STRING("Create Material Asset"));
        {
            //TODO:
        }


        Reflection_Runtime_Struct material_info_struct = reflection_registry_get_struct(
            editor->reflection_registry, TYPE_STRING(Material_Info));

        madness_ui_reflect_using_data(editor->reflection_registry, material_info_struct, &mat_info, "bye");

        madness_ui_padding("mat padding");

        static u32 selected_index;
        if (madness_ui_combo_box_char(STRING("Material Struct"), &selected_index,
                                      material_struct_string_list,
                                      ARRAY_SIZE(material_struct_string_list)))
        {
            memset(fuck_you_memory, 0, 512);
        }

        Reflection_Runtime_Struct material_struct_runtime = reflection_registry_get_struct(editor->reflection_registry,
            material_struct_string_list[selected_index]);

        madness_ui_reflect_using_data(editor->reflection_registry, material_struct_runtime, fuck_you_memory, "hi");


        /*
        Reflection_Runtime_Struct runtime_struct = reflection_registry_get_struct(editor->reflection_registry,
            "Material_Info");

        madness_ui_reflect_using_data(editor->reflection_registry, runtime_struct, &material_info, "hi");
        */
    }
    madness_ui_window_end();
}

void editor_mesh_view(Editor* editor)
{
    editor->asset_system->mesh_system;

    madness_ui_window_begin(STRING("Mesh View"));
    {
    }
    madness_ui_window_end();
}

void editor_render_view(Editor* editor)
{
    Renderer* renderer = editor->renderer;

    madness_ui_window_begin(STRING("Renderer View"));
    {
        madness_ui_check_box(STRING("DEBUG AXIS"), &renderer->draw_debug_axis);

        madness_ui_combo_box_char(STRING("RENDER_MODE"), &renderer->mode, render_mode_enum_string,
                                  ARRAY_SIZE(render_mode_enum_string));
    }
    madness_ui_window_end();
}


void editor_particle_view(Editor* editor)
{
    Particle_System* particle_system = editor->asset_system->particle_system;

    madness_ui_window_begin(STRING("Particle Creation"));
    {

    }
    madness_ui_window_end();



    madness_ui_window_begin(STRING("Particle View"));
    {
        Particle_Emitter* emitter = &particle_system->emitters[0];

        Reflection_Runtime_Struct emitter_runtime_struct = reflection_registry_get_struct(
            editor->asset_system->global_reflection_registry, TYPE_STRING(Particle_Emitter_Data));
        madness_ui_reflect_using_data(editor->asset_system->global_reflection_registry, emitter_runtime_struct,
                                      &emitter->data, "emitter");
    }
    madness_ui_window_end();

    String effect_path;
    madness_ui_window_begin(STRING("Particle Effects"));
    {
        madness_ui_combo_box_string(STRING("selected effect"), &effect_path,
                                    editor->particle_effect_list->strings,
                                    editor->particle_effect_list->count);
    }
    madness_ui_window_end();

    madness_ui_window_begin(STRING("Particle Emitters"));
    {
        madness_ui_combo_box_string(STRING("selected emitter"), &effect_path,
                                    editor->particle_emitter_list->strings,
                                    editor->particle_emitter_list->count);
    }
    madness_ui_window_end();

    madness_ui_window_begin(STRING("emitter material"));
    {
        static Material_Info material_info;
        static u8 fuck_you_memory[1024];
        static Material_Info mat_info;
        if (!mat_info.material_name)
        {
            mat_info.material_name = STRING_CREATE("0");
        }
        if (!mat_info.shader_name)
        {
            mat_info.shader_name = STRING_CREATE("0");
        }
        Reflection_Runtime_Struct material_info_struct = reflection_registry_get_struct(
            editor->reflection_registry, TYPE_STRING(Material_Info));

        madness_ui_reflect_using_data(editor->reflection_registry, material_info_struct, &mat_info, "bye");

        madness_ui_padding("mat padding");

        static u32 selected_index;
        if (madness_ui_combo_box_char(STRING("Material Struct"), &selected_index,
                                      material_struct_string_list,
                                      ARRAY_SIZE(material_struct_string_list)))
        {
            memset(fuck_you_memory, 0, 1024);
        }

        Reflection_Runtime_Struct material_struct_runtime = reflection_registry_get_struct(editor->reflection_registry,
            material_struct_string_list[selected_index]);

        madness_ui_reflect_using_data(editor->reflection_registry, material_struct_runtime, fuck_you_memory, "hi");
    }
    madness_ui_window_end();
}
