#include "editor.h"

#include "resource_types.h"
#include "asset_system.h"
#include "asset_registry.h"
#include "scene.h"
#include "material_system.h"
#include "texture_system.h"
#include "particle_system.h"
#include "profiler.h"
#include "memory/memory_system.h"
#include "ui_madness.h"
#include "ui_insanity.h"
#include "../madness_imgui.h"
#include "filesystem.h"

#include "madness_txt.h"
#include "clock.h"

Editor* editor_init(Memory_System* memory_system, Renderer* renderer,
                    Asset_System* asset_system, Clock* clock, Reflection_Registry* reflection_registry,
                    Madness_txt* txt)
{
    // editor // allocate memory for the editor
    Editor* editor = memory_system_alloc(memory_system, sizeof(Editor), MEMORY_SUBSYSTEM_EDITOR);

    u64 editor_memory_size = MB(4);

    editor->editor_allocator = memory_system_allocator_create(memory_system, editor_memory_size,
                                                              MEMORY_SUBSYSTEM_EDITOR, "EDITOR PERSISTENT");

    editor->frame_allocator = memory_system_allocator_create(memory_system, editor_memory_size,
                                                             MEMORY_SUBSYSTEM_EDITOR, "EDITOR FRAME");


    editor->renderer = renderer;
    editor->asset_system = asset_system;
    editor->clock = clock;
    editor->reflection_registry = reflection_registry;
    editor->memory_system = memory_system;

    editor->lowest_ms = INT_MAX;
    editor->highest_ms = 0;
    // editor->state = EDITOR_UI_STATE_INSANITY_UI_TEST;
    // editor->state = EDITOR_UI_STATE_CHAOS_UI_TEST;
    // editor->state = EDITOR_UI_STATE_MADNESS_UI_TEST;
    // editor->state = EDITOR_UI_STATE_PARTICLE;

    editor->madness_txt = txt;
    editor_generate_asset_lists(editor, memory_system);

    //reads in editor state and writes it out as well
    editor->editor_config_file_handle = madness_txt_schema_create(txt, "scheme a test");
    madness_txt_schema_add_type(txt, editor->editor_config_file_handle, "editor state", REFLECTION_TYPE_ENUM, 1);

    editor_deserialize_state(editor);


    return editor;
}


bool editor_deserialize_state(Editor* editor)
{
    madness_txt_schema_read(editor->madness_txt, &editor->editor_config_file_handle, &editor->state, "../editor.yaml");
}

bool editor_serialize_state(Editor* editor)
{
    madness_txt_schema_write(editor->madness_txt, &editor->editor_config_file_handle, &editor->state, "../editor.yaml");
}


bool editor_update(Editor* editor)
{
    PROFILE_ZONE(editor_update)

    allocator_clear(editor->frame_allocator);

    if (ImGui_BeginMainMenuBar())
    {
        if (ImGui_BeginMenu("Editor"))
        {
            int state = (int)editor->state; /* ImGui_Combo wants an int*, not your enum */
            if (ImGui_ComboChar("editor states", &state,
                                Editor_UI_State_ENUM_String,
                                ARRAY_SIZE(Editor_UI_State_ENUM_String)))
            {
                editor->state = (Editor_UI_State)state; /* use your enum's real type name */
            }
            ImGui_EndMenu();
        }

        ImGui_EndMainMenuBar();
    }


    //do the ui and stuff
    //manage a bunch of ui state
    if (input_key_released_unique(KEY_E))
    {
        editor->state = (editor->state + 1) % EDITOR_UI_STATE_MAX;
        editor_serialize_state(editor);
    }

    if (input_key_released_unique(KEY_Q))
    {
        if (editor->state == 0)
        {
            editor->state = EDITOR_UI_STATE_MAX - 1;
        }
        else
        {
            editor->state = (editor->state - 1);
        }
        editor_serialize_state(editor);
    }

    editor_ui(editor);

    PROFILE_ZONE_END(editor_update);

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


    //particles
    editor->particle_effect_list =
        asset_lists_generate(memory_system,
                             MAX_ASSETS_STRINGS,
                             "../z_assets_engine/particle/particle_effect");
    editor->particle_emitter_list =
        asset_lists_generate(memory_system,
                             MAX_ASSETS_STRINGS,
                             "../z_assets_engine/particle/particle_emitter");


    editor->material_asset_list =
        asset_lists_generate(memory_system,
                             MAX_ASSETS_STRINGS,
                             "../z_assets_engine/material");


    return true;
}


static float thick = 1.0f;


void editor_ui(Editor* editor)
{
    switch (editor->state)
    {
    case EDITOR_UI_STATE_MAX:
        break;
    // case EDITOR_UI_STATE_DEBUG:
    // editor_madness_ui_debug(editor);
    // break;
    case EDITOR_UI_STATE_ENGINE_STATS:
        editor_ui_stats(editor);
        break;
    case EDITOR_UI_STATE_RENDERER:
        editor_render_view(editor);
        break;
    case EDITOR_UI_STATE_SCENE:
        editor_ui_scene(editor);
        break;
    case EDITOR_UI_STATE_TEXTURE_VIEWER:
        editor_texture_view(editor);
        break;
    case EDITOR_UI_STATE_ANIMATION:
        editor_ui_animation(editor);
        break;
    // case EDITOR_UI_STATE_CHAOS_UI_TEST:
    //     chaos_ui_test(editor->clock->delta_time, editor->clock->time_elapsed);
    //     break;
    case EDITOR_UI_STATE_INSANITY_UI_TEST:
        insanity_ui_test(editor->clock->delta_time, editor->clock->time_elapsed);
        break;
    // case EDITOR_UI_STATE_MADNESS_UI_TEST:
    //     // madness_ui_test();
    //     madness_ui_example();
    //     // madness_ui_window_testing();
    //     break;

    case EDITOR_UI_STATE_REFLECTION_ABILITY:
        madness_ui_window_begin(STRING("RUNTIME TESTING"), 0);
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
        madness_ui_window_begin(STRING("Material Menu"), 0);
        {
            Reflection_Registry* material_reflection_registry = editor->asset_system->material_reflection_registry;

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
    case EDITOR_UI_STATE_MATERIAL_EDITOR:
        editor_material_editor(editor);
        break;
    }
}

//testing bezier curves
static vec2s pos1 = {400, 400};
static vec2s pos2 = {500, 600};
static vec2s pos3 = {500, 100};
static vec2s pos4 = {600, 150};

void editor_madness_ui_debug(Editor* editor)
{
    // madness_ui_test(madness_ui);
    madness_ui_window_begin(STRING("Madness Editor"), 0);
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
        array_get(madness_ui->ui_nodes, 0, UI_Node).rotation = rot;
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
    Memory_System* memory_system = editor->memory_system;

    ImGui_SetNextWindowPos((ImVec2){50.0f, 50.0f}, ImGuiCond_FirstUseEver);
    if (ImGui_Begin("Stats", NULL, 0))
    {
        float ms = clock_delta_time_in_ms(clock);
        float fps = clock_delta_time_to_fps(clock);

        editor->lowest_ms = min_f(editor->lowest_ms, ms);
        editor->highest_ms = max_f(editor->highest_ms, ms);

        ImGui_Text("ms:      %.3f", ms);
        ImGui_Text("fps:     %.1f", fps);
        ImGui_Text("low ms:  %.3f", editor->lowest_ms);
        ImGui_Text("high ms: %.3f", editor->highest_ms);
    }
    ImGui_End(); /* always called, even when Begin returns false (window collapsed) */


    ImGui_SetNextWindowPos((ImVec2){200.0f, 50.0f}, ImGuiCond_FirstUseEver);
    if (ImGui_Begin("Memory", NULL, 0))
    {
        u64 mb_used = memory_system->application_allocator.used / MB(1);
        u64 mb_capacity = memory_system->application_allocator.capacity / MB(1);
        ImGui_Text("application memory used MB: %llu", (unsigned long long)mb_used);
        ImGui_Text("application memory capacity MB: %llu", (unsigned long long)mb_capacity);

        ImGui_SeparatorText("SUBSYSTEMS");
        for (u32 i = 0; i < MEMORY_SUBSYSTEM_MAX; i++)
        {
            u64 memory_usage = memory_system->memory_subsystem_usage[i];
            const char* subsystem_name = memory_subsystem_type_string[i];
            u32 container_usage = 0;
            if (memory_usage >= GB(1))
            {
                container_usage = memory_usage / GB(1);
                ImGui_Text("%s, MEMORY: %llu GB", subsystem_name, container_usage);
            }
            else if (memory_usage >= MB(1))
            {
                container_usage = memory_usage / MB(1);
                ImGui_Text("%s, MEMORY: %llu MB", subsystem_name, container_usage);
            }
            else if (memory_usage >= KB(1))
            {
                container_usage = memory_usage / KB(1);
                ImGui_Text("%s : %llu KB", subsystem_name, container_usage);
            }
            else if (memory_usage < KB(1))
            {
                ImGui_Text("%s : %llu BYTES", subsystem_name, memory_usage);
            }
            else
            {
                MASSERT_MSG_FALSE("MEMORY SUBSYSTEM PRINT ERROR");
            }
        }

        ImGui_SeparatorText("ALLOCATORS");
        for (u32 i = 0; i < memory_system->allocator_count; i++)
        {
            ImGui_Text("%s", memory_system->allocator_list[i].name);
            ImGui_Text("  used bytes:     %llu BYTES",
                       (unsigned long long)memory_system->allocator_list[i].current_offset);
            ImGui_Text("  capacity bytes: %llu BYTES", memory_system->allocator_list[i].capacity);
            ImGui_Spacing();
        }

        ImGui_SeparatorText("HEAP ALLOCATORS");
        for (u32 i = 0; i < memory_system->heap_allocator_count; i++)
        {
            ImGui_Text("%s", memory_system->heap_allocator_list[i].name);
            ImGui_Text("  used bytes:     %llu BYTES", memory_system->heap_allocator_list[i].used);
            ImGui_Text("  capacity bytes: %llu BYTES",
                       (unsigned long long)memory_system->heap_allocator_list[i].capacity);
            ImGui_Spacing();
        }
    }
    ImGui_End(); /* always called, even if Begin returned false */
}

void editor_ui_animation(Editor* editor)
{
    Animation_System* animation_system = editor->asset_system->animation_system;
    ImGui_Begin("Animation Data", 0, 0);
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

            ImGui_Text("Animation Current Time: %f", &madness_animation->current_time);

            if (ImGui_DragScalar(buffer, ImGuiDataType_U32, &madness_animation->current_animation_index))
            {
                madness_animation->current_animation_index = clamp_u32(
                    madness_animation->current_animation_index, 0,
                    animation_data->animations_count - 1);

                madness_animation->current_time = 0;
            }
            if (!ImGui_Checkbox(buffer2, &madness_animation->looping))
            {
                madness_animation->current_time = 0;
            }
        }
    }
    ImGui_End();
}


void editor_ui_scene(Editor* editor)
{
    Light_System* light_system = editor->renderer->light_system;

    ImGui_SetWindowPos((ImVec2){50, 50}, ImGuiCond_FirstUseEver);
    ImGui_Begin("Scene", 0, 0);
    {
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

            if (ImGui_DragFloat3(buffer_transform, editor->asset_system->scene->transforms[i].position.raw))
            {
                transform_mark_dirty(&editor->asset_system->scene->transforms[i]);
            }
            if (ImGui_DragFloat3(buffer_rotation, editor->asset_system->scene->transforms[i].euler_angles.raw))
            {
                transform_rotate_euler(&editor->asset_system->scene->transforms[i]);
            }

            if (ImGui_DragFloat3(buffer_scale, editor->asset_system->scene->transforms[i].scale.raw))
            {
                transform_mark_dirty(&editor->asset_system->scene->transforms[i]);
            }
        }

        // madness_scroll_box_end();
    }
    ImGui_End();

    ImGui_SetWindowPos((ImVec2){400, 50}, ImGuiCond_FirstUseEver);
    ImGui_Begin("Lights", 0, 0);
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
                ImGui_DragFloat3(buffer, directional_light->direction.raw);
                ImGui_ColorEdit3(buffer2, directional_light->color.raw, 0.1f);
            }
            for (u32 i = 0; i < light_system->point_light_count; ++i)
            {
                char buffer[50];
                char buffer2[50];
                sprintf(buffer, "point_light%d", i);
                sprintf(buffer2, "point_light_color%d", i);
                Point_Light* point_light = &light_system->point_lights[i];
                ImGui_DragFloat4(buffer, point_light->position.raw);
                ImGui_ColorEdit4(buffer2, point_light->color.raw, 0.1f);
            }


            for (u32 i = 0; i < light_system->spot_light_count; ++i)
            {
                char buffer[50];
                char buffer2[50];
                sprintf(buffer, "spot_light%d", i);
                sprintf(buffer2, "spot_light_color%d", i);
                Spot_Light* spot_light = &light_system->spot_lights[i];
                ImGui_DragFloat4(buffer, spot_light->position.raw);
                ImGui_ColorEdit4(buffer2, spot_light->color.raw, 0.1f);
            }
        }
    }
    ImGui_End();


    // scene serialization

    ImGui_SetWindowPos((ImVec2){1200, 50}, ImGuiCond_FirstUseEver);
    ImGui_Begin("Scene Serialization", 0, 0);
    {
        if (ImGui_Button("SCENE SAVE"))
        {
            scene_save(editor->asset_system->scene);
        }

        static char scene_name_buffer[1024];
        ImGui_InputText("SCENE NAME TO LOAD", scene_name_buffer, 1024, 0);

        if (ImGui_Button("SCENE LOAD"))
        {
            scene_load(editor->asset_system->scene,
                       STRING_CREATE_FROM_BUFFER_ALLOCATOR(scene_name_buffer, editor->frame_allocator));
        }

        static char mesh_path[1024];
        ImGui_file_picker("MESH PATH", mesh_path, 1024,
                          "C:/Users/Adams Humbert/Documents/Clion/MadnessEngine/z_assets_engine/mesh");
        if (ImGui_Button("LOAD MESH ASSET"))
        {
            Mesh_Handle handle;
            asset_load_mesh_path(editor->asset_system,
                                 mesh_path,
                                 &handle);
        }


        static u32 path_index;
        const char* combo_preview_value = editor->madness_mesh_list->strings[path_index].chars;
        if (ImGui_BeginCombo("Mesh Paths", combo_preview_value, 0))
        {
            for (u32 i = 0; i < editor->madness_mesh_list->count; i++)
            {
                const bool is_selected = (path_index == i);
                if (ImGui_SelectableEx(editor->madness_mesh_list->strings[i].chars, is_selected, 0, (ImVec2){0, 0}))
                {
                    path_index = i;
                }

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                {
                    ImGui_SetItemDefaultFocus();
                }
            }
            ImGui_EndCombo();
        }

        if (ImGui_BeginListBox("Mesh Path List", (ImVec2){0, 0}))
        {
            for (u32 i = 0; i < editor->madness_mesh_list->count; i++)
            {
                const bool is_selected = (path_index == i);
                if (ImGui_SelectableEx(editor->madness_mesh_list->strings[i].chars, is_selected, 0, (ImVec2){0, 0}))
                {
                    path_index = i;
                }
                if (is_selected && ImGui_IsItemHovered(0))
                {
                    path_index = i;
                }
                if (is_selected)
                {
                    ImGui_SetItemDefaultFocus();
                }
            }
            ImGui_EndListBox();
        }

        // String mesh_path2 = STRING("Mesh Path2");
        // madness_ui_combo_box(mesh_path2, &path_index,
        //                      editor->madness_mesh_list->strings,
        //                      editor->madness_mesh_list->count);

        String scene_name2 = STRING("SCENE LOAD2");
        if (ImGui_Button("LOAD MESH ASSET2"))
        {
            Mesh_Handle handle;
            asset_load_mesh_path(editor->asset_system,
                                 string_to_c_string(&editor->madness_mesh_list->strings[path_index]), &handle);
        }
    }
    ImGui_End();
}


void editor_texture_view(Editor* editor)
{
    Asset_System* asset_system = editor->asset_system;
    ImGui_Begin("Texture View", 0, 0);
    {
        for (u32 i = 0; i < asset_system->asset_registry->asset_record->num_items; i++)
        {
            Asset_Record* record = _dynamic_array_get(asset_system->asset_registry->asset_record, i);
            if (record->type != ASSET_TEXTURE && record->type != ASSET_FONT)
            {
                continue;
            }


            Texture_Handle texture_handle;
            if (texture_system_does_texture_exist(asset_system->texture_system, record, &texture_handle))
            {
                ImGui_String(record->source_file);
                ImGui_String(record->engine_path);
                // madness_ui_image_handle(texture_handle);

                Vulkan_Texture* vk_texture = &editor->renderer->texture_system->textures[texture_handle.handle];
                if (vk_texture->ready_for_read_by_imgui)
                {
                    ImGui_Text("Width: %d", vk_texture->width);
                    ImGui_Text("Height: %d", vk_texture->height);

                    ImTextureRef ref = {0};
                    ref._TexID = (ImTextureID)(uintptr_t)vk_texture->imgui_descriptor_set;
                    // you dont pass in the actual size, you pass in the size you want it displayed
                    // ImGui_Image(ref, (ImVec2){ vk_texture->width, vk_texture->height });

                    //give the image a border so its easier to see
                    ImGui_PushStyleVar(ImGuiStyleVar_ImageBorderSize, 1.0f);

                    //whites a good color for seeing the textures better
                    ImGui_PushStyleColorImVec4(ImGuiCol_Border, (ImVec4){1.0f, 1.0f, 1.0f, 1.0f});

                    ImGui_Image(ref, (ImVec2){128, 128});

                    ImGui_PopStyleColor();
                    ImGui_PopStyleVar();
                }

                ImGui_NewLine();
            }
        }
    }
    ImGui_End();

    ImGui_Begin("Texture Metatable", 0, 0);
    {
        for (u32 i = 0; i < asset_system->asset_registry->texture_table->num_items; i++)
        {
            Texture_MetaData* meta_data = &dynamic_array_get(asset_system->asset_registry->texture_table, i,
                                                             Texture_MetaData);

            ImGui_Text("Texture Width: %d", &meta_data->width);
            ImGui_Text("height %d", &meta_data->height);
        }
    }
    ImGui_End();
}

void editor_meta_data_view(Editor* editor)
{
    Asset_System* asset_system = editor->asset_system;
    ImGui_Begin("MetaData View", 0, ImGuiWindowFlags_AlwaysVerticalScrollbar);
    {
        for (u32 i = 0; i < asset_system->asset_registry->asset_record->num_items; i++)
        {
            Asset_Record* meta_data = _dynamic_array_get(asset_system->asset_registry->asset_record, i);
            ImGui_Text("UUID: High: %llu, Low: %llu", meta_data->uuid.high, meta_data->uuid.low);

            ImGui_Text("UUID Hash: %llu", &meta_data->hash);
            ImGui_Text("Type: %s", ASSET_TYPE_LUT[meta_data->type]);

            ImGui_Text("Source Path: %.*s", meta_data->source_file->length, meta_data->source_file->chars);
            ImGui_Text("Engine Path: %.*s", meta_data->engine_path->length, meta_data->engine_path->chars);
            ImGui_NewLine();
        }
    }
    ImGui_End();

    ImGui_Begin("MetaData Asset Filter", 0, 0);
    {
        static Asset_Type asset_type_filter;
        ImGui_ComboChar("Asset Filter Type", &asset_type_filter, Asset_Type_enum_string,
                        ARRAY_SIZE(Asset_Type_enum_string));
        ImGui_Separator();

        //making it a child always keeps the asset type filter visible
        if (ImGui_BeginChild("asset_list", (ImVec2){0, 0}, 0, 0))
        {
            for (u32 i = 0; i < asset_system->asset_registry->asset_record->num_items; i++)
            {
                Asset_Record* meta_data = _dynamic_array_get(asset_system->asset_registry->asset_record, i);

                if ((int)meta_data->type != asset_type_filter) { continue; }

                ImGui_Text("UUID: High: %llu, Low: %llu",
                           meta_data->uuid.high,
                           meta_data->uuid.low);
                ImGui_Text("UUID Hash: %llu", meta_data->hash);
                ImGui_Text("Type: %s", ASSET_TYPE_LUT[meta_data->type]);

                ImGui_Text("Source Path: %.*s",
                           (int)meta_data->source_file->length, meta_data->source_file->chars);
                ImGui_Text("Engine Path: %.*s",
                           (int)meta_data->engine_path->length, meta_data->engine_path->chars);
                ImGui_NewLine();
            }
        }
        ImGui_EndChild();
    }
    ImGui_End();
}

void editor_material_asset_view(Editor* editor)
{
    PROFILE_ZONE(editor_material_asset_view)
    Asset_System* asset_system = editor->asset_system;
    Material_System2* material_system = editor->asset_system->material_system;
    Shader_System* shader_system = editor->asset_system->shader_system;


    static Shader_Handle handle;
    static u32 selected_shader;
    static u32 selected_blend_mode;
    static bool double_sided;
    madness_ui_window_begin(STRING("Shader Creation"), 0);
    {
        madness_ui_combo_box2(STRING("Shader: "), &selected_shader,
                              shader_system->shader_to_material_mapping.shader_name,
                              shader_system->shader_to_material_mapping.count);

        madness_ui_combo_box_char(STRING("Blend Mode: "), &selected_blend_mode, Shader_Blend_Mode_enum_string,
                                  ARRAY_SIZE(Shader_Blend_Mode_enum_string));

        madness_ui_check_box(STRING("Double Sided"), &double_sided);


        if (madness_ui_button(STRING("CREATE SHADER")))
        {
            Shader_PSO shader_info = {
                .shader_name = shader_system->shader_to_material_mapping.shader_name[selected_shader],
                .blend_mode = selected_blend_mode,
                .two_sided = double_sided,

            };
            shader_create_or_get(asset_system, &shader_info, &handle);
        }


        madness_ui_padding();

        madness_ui_string(STRING("Material Name: "));
        madness_ui_same_line();
        madness_ui_string(*shader_system->shader_to_material_mapping.material_name[selected_shader]);

        static u8 material_memory[1024];
        Reflection_Runtime_Struct material_reflect_struct = reflection_registry_get_struct(
            asset_system->global_reflection_registry, string_to_c_string_allocator(
                shader_system->shader_to_material_mapping.material_name[selected_shader], editor->frame_allocator));
        madness_ui_padding();


        madness_ui_reflect_material(editor->asset_system->asset_registry,
                                    asset_system->global_reflection_registry,
                                    material_reflect_struct,
                                    &material_memory,
                                    "mat",
                                    editor->texture_list);
    }
    madness_ui_window_end();

    madness_ui_window_begin(STRING("Additional Shader Info"), 0);
    {
        madness_ui_string(STRING("ADDITIONAL INFO"));
        madness_ui_padding();

        madness_ui_string(STRING("MATERIAL NAME: "));
        madness_ui_same_line();
        madness_ui_string(*shader_system->shader_to_material_mapping.material_name[selected_shader]);

        Reflection_Runtime_Struct mat_def_reflect_struct = reflection_registry_get_struct(
            asset_system->global_reflection_registry, TYPE_STRING(Material_Definition));
        // shader_asset
        madness_ui_reflect_using_data(asset_system->global_reflection_registry, mat_def_reflect_struct,
                                      &shader_system->shader_to_material_mapping.material_definition[selected_shader],
                                      "Mat");


        Shader_Asset* shader_asset = shader_asset_get(shader_system, handle);
        Reflection_Runtime_Struct shader_reflect_struct = reflection_registry_get_struct(
            asset_system->global_reflection_registry, TYPE_STRING(Shader_Asset));
        // shader_asset
        madness_ui_reflect_using_data(asset_system->global_reflection_registry, shader_reflect_struct, shader_asset,
                                      "Shader");
    }
    madness_ui_window_end();


    /*static Shader_Handle handle;
    String material_path;
    madness_ui_window_begin(STRING("Material Reflection View"));
    {
        madness_ui_combo_box_string(STRING("selected material asset"), &material_path,
                                    editor->material_asset_list->strings,
                                    editor->material_asset_list->count);


        if (madness_ui_button(STRING("LOAD SHADER")))
        {
            asset_load_shader_asset_path(asset_system,
                                         string_to_c_string_allocator(&material_path, editor->editor_frame_allocator),
                                         &handle);
        }

        madness_ui_padding();

        if (madness_ui_button(STRING("SAVE SHADER")))
        {
            asset_load_shader_asset_path(asset_system,
                                         string_to_c_string_allocator(&material_path, editor->editor_frame_allocator),
                                         &handle);
        }

        madness_ui_padding();

        static u32 shader_mapping_index;
        madness_ui_u32(STRING("asdsa"), &shader_mapping_index, 1);
        shader_mapping_index = clamp_uint(shader_mapping_index, 0,
                                          editor->asset_system->material_system->shader_to_material_count - 1);
        // madness_ui_string(
        // *material_system->shader_to_material_mapping.shader_name[shader_mapping_index]);

        madness_ui_combo_box2(STRING("STRINGasdasd"), &shader_mapping_index,
                              material_system->shader_to_material_mapping.shader_name,
                              editor->asset_system->material_system->shader_to_material_count);
        madness_ui_string(
            *material_system->shader_to_material_mapping.material_name[shader_mapping_index]);


        madness_ui_padding();

        if (handle.handle != 0)
        {
            Shader_Asset* material_asset = shader_asset_get(asset_system->material_system, handle);

            madness_ui_u64(STRING("High:"), &material_asset->uuid.high, 0);
            madness_ui_u64(STRING("Low:"), &material_asset->uuid.low, 0);
            madness_ui_u32(STRING("Version:"), &material_asset->version, 0);


            // madness_ui_string(STRING("shader_name"), &material_asset->material_info, 0);
            // madness_ui_string(STRING("material_name"), &material_asset->material_info, 0);
            //

            Reflection_Runtime_Struct reflection_asset = reflection_registry_get_struct(
                asset_system->global_reflection_registry, TYPE_STRING(Material_Info));

            madness_ui_reflect_using_data(asset_system->global_reflection_registry, reflection_asset,
                                          &material_asset->shader_info, "mat asset");
        }



    }
    madness_ui_window_end();


    madness_ui_set_window_pos(600, 200);

    static Shader_Info material_info;
    static u8 fuck_you_memory[512];
    static Shader_Info shader_info;
    if (!shader_info.material_name)
    {
        shader_info.material_name = STRING_CREATE("0");
    }
    if (!shader_info.shader_name)
    {
        shader_info.shader_name = STRING_CREATE("0");
    }
    madness_ui_window_begin(STRING("Shader Creation"));
    {

        Reflection_Runtime_Struct material_info_struct = reflection_registry_get_struct(
            editor->reflection_registry, TYPE_STRING(Material_Info));

        madness_ui_reflect_using_data(editor->reflection_registry, material_info_struct, &shader_info, "new");

        madness_ui_button(STRING("Create Shader Asset"));
        {
            Shader_Handle shader_handle;
            shader_get_or_create(asset_system, &shader_info, &shader_handle);
        }


        madness_ui_padding();

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
    }
    madness_ui_window_end();*/


    PROFILE_ZONE_END(editor_material_asset_view)
}

void editor_mesh_view(Editor* editor)
{
    editor->asset_system->mesh_system;

    madness_ui_window_begin(STRING("Mesh View"), 0);
    {
    }
    madness_ui_window_end();
}

void editor_render_view(Editor* editor)
{
    Renderer* renderer = editor->renderer;

    madness_ui_window_begin(STRING("Renderer View"), 0);
    {
        madness_ui_check_box(STRING("DEBUG AXIS"), &renderer->draw_debug_axis);

        madness_ui_combo_box_char(STRING("Render Mode"), &renderer->mode, render_mode_enum_string,
                                  ARRAY_SIZE(render_mode_enum_string));
    }
    madness_ui_window_end();
}


void editor_particle_view(Editor* editor)
{
    PROFILE_ZONE(editor_particle_view);

    Particle_System* particle_system = editor->asset_system->particle_system;


    asset_list_regenerate(editor->particle_effect_list, "../z_assets_engine/particle/particle_effect");
    asset_list_regenerate(editor->particle_emitter_list, "../z_assets_engine/particle/particle_emitter");


    ImGui_Begin("Emitter Editor View", 0, 0);
    {
        ImGui_Text("EMITTER SELECTED");

        static u32 emitter_path_index;
        ImGui_Combo_String("selected emitter", emitter_path_index, editor->particle_emitter_list->strings,
                           editor->particle_emitter_list->count);


        static Particle_Emitter_Handle emitter_to_edit_handle;
        if (ImGui_Button("LOAD EMITTER"))
        {
            asset_load_particle_emitter(editor->asset_system,
                                        string_to_c_string_allocator(
                                            &editor->particle_emitter_list->strings[emitter_path_index],
                                            editor->frame_allocator),
                                        &emitter_to_edit_handle);
        }
        if (ImGui_Button("UNLOAD EMITTER"))
        {
            /*
            asset_unload_particle_emitter(editor->asset_system,
                                        string_to_c_string_allocator(&emitter_path, editor->editor_frame_allocator),
                                        &emitter_to_edit_handle);
                                        */
        }
        ImGui_NewLine();

        Particle_Emitter* emitter_to_edit = &particle_system->emitters[emitter_to_edit_handle.handle];


        if (ImGui_Button("SAVE EMITTER"))
        {
            asset_converter_particle_emitter(editor->asset_system, emitter_to_edit, NULL);
        }

        ImGui_Text("NAME: %.*s", emitter_to_edit->name->length, emitter_to_edit->name->chars);

        Reflection_Runtime_Struct emitter_runtime_struct = reflection_registry_get_struct(
            editor->asset_system->global_reflection_registry, TYPE_STRING(Particle_Emitter_Data));
        Imgui_Reflect_Using_Data(editor->asset_system->global_reflection_registry, emitter_runtime_struct,
                                      &emitter_to_edit->data, "emitter_edit", editor->frame_allocator);


        ImGui_NewLine();


        /*
        Material_Asset* material_asset = material_asset_get(editor->asset_system->material_system,
                                                    emitter_to_edit->material_instance.shader_handle);
        */


        static u32 material_path_index;
        ImGui_Combo_String("selected material asset", material_path_index,
                             editor->material_asset_list->strings,
                             editor->material_asset_list->count);


        /*
        madness_ui_string(STRING("NAME: "));
        madness_ui_same_line();
        madness_ui_string(*emitter->name);

        Reflection_Runtime_Struct emitter_runtime_struct = reflection_registry_get_struct(
            editor->asset_system->global_reflection_registry, TYPE_STRING(Particle_Emitter_Data));
        madness_ui_reflect_using_data(editor->asset_system->global_reflection_registry, emitter_runtime_struct,
                                      &emitter->data, "emitter");*/
    }
    ImGui_End();


    static u32 effect_path_index;
    static u32 emitter_path_index;
    static u32 effect_index;
    static u32 effect_emitter_index;
    ImGui_Begin("Particle Effects Editor", 0, 0);
    {
        //TODO:
        // create effect asset
        // load effect asset
        // show list of emitters on the effect
        // add emitters -> load in the asset if its not already
        // remove emitter -> just remove from the effect, no need to unload it
        // select particle to edit
        // select emitter to edit
        // load in a particle effect
        //
        static char emitter_name_text_box[1024];
        ImGui_InputText("emitter name", emitter_name_text_box, 1024, 0);

        if (ImGui_Button("PARTICLE EMITTER CREATE"))
        {
            if (strcmp(emitter_name_text_box, "") == 0)
            {
                memcpy(emitter_name_text_box, "emitter_no_name\0", sizeof("emitter_no_name\0"));
                // strcpy_s(emitter_name_text_box, "emitter_no_name\0");
            }

            particle_emitter_create_default(editor->asset_system, editor->asset_system->particle_system,
                                            emitter_name_text_box);
        }

        ImGui_NewLine();


        static char effect_name_text_box[1024];
        ImGui_InputText("EFFECT NAME", effect_name_text_box, 1024, 0);

        if (ImGui_Button("PARTICLE EFFECT CREATE ASSET"))
        {
            if (strcmp(effect_name_text_box, "") == 0)
            {
                memcpy(effect_name_text_box, "effect_no_name\0", sizeof("effect_no_name\0"));
            }
            particle_effect_create_empty(editor->asset_system, editor->asset_system->particle_system,
                                         effect_name_text_box);
        }


        ImGui_NewLine();


        ImGui_Text("EFFECT SELECTED");
        ImGui_Combo_String("selected effect", effect_path_index,
                             editor->particle_effect_list->strings,
                             editor->particle_effect_list->count);
        if (ImGui_Button("LOAD EFFECT"))
        {
            Particle_Effect_Handle discard_handle;
            asset_load_particle_effect_by_path(editor->asset_system,
                                               string_to_c_string_allocator(
                                                   &editor->particle_effect_list->strings[effect_path_index],
                                                   editor->frame_allocator), &discard_handle);
        }


        ImGui_NewLine();


        ImGui_Text("EMITTER TO ADD");

        ImGui_Combo_String("selected emitter to add", emitter_path_index,
                             editor->particle_emitter_list->strings,
                             editor->particle_emitter_list->count);

        //add from a global list
        if (ImGui_Button("ADD EMITTER"))
        {
            Particle_Effect* particle_effect = &particle_system->particle_effects[effect_index];

            //load an emitter
            Particle_Emitter_Handle emitter_handle = {0};
            asset_load_particle_emitter(editor->asset_system,
                                        string_to_c_string_allocator(
                                            &editor->particle_emitter_list->strings[emitter_path_index],
                                            editor->frame_allocator),
                                        &emitter_handle);
            particle_effect_add_emitter_by_handle(editor->asset_system->particle_system, particle_effect,
                                                  emitter_handle);
        }


        //remove from the current emitters list


        ImGui_NewLine();


        //particle selection
        ImGui_DragScalar("PARTICLE INDEX", ImGuiDataType_U32, &effect_index);
        effect_index = clamp_u32(effect_index, 0, particle_system->active_effects->num_items - 1);


        Particle_Effect* particle_effect = &particle_system->particle_effects[effect_index];

        if (particle_effect->name)
        {
            ImGui_Text("PARTICLE EFFECT NAME: %s.", particle_effect->name);

            ImGui_DragScalar("EMITTER INDEX", ImGuiDataType_U32, &effect_emitter_index);
            effect_emitter_index = clamp_u32(effect_emitter_index, 0, particle_effect->emitter_count - 1);


            if (ImGui_Button("REMOVE EMITTER AT INDEX"))
            {
                //remove an emitter
                Particle_Effect* particle_effect = &particle_system->particle_effects[effect_index];
                particle_effect_remove_emitter(particle_effect, effect_emitter_index);
                effect_emitter_index = clamp_u32(effect_index, 0, particle_system->active_effects->num_items - 1);
            }


            ImGui_NewLine();
        }
    }
    ImGui_End();


    ImGui_Begin("Particle Effect View", 0, 0);
    {
        Particle_Effect* effect = &particle_system->particle_effects[effect_index];
        if (effect->name)
        {
            if (ImGui_Button("SAVE EFFECT"))
            {
                asset_converter_particle_effect(editor->asset_system, effect, NULL);
            }

            ImGui_String(effect->name);


            ImGui_DragScalar("effect current time", ImGuiDataType_U32, &effect->effect_current_time);
            ImGui_DragScalar("effect length", ImGuiDataType_U32, &effect->effect_length);
            ImGui_Checkbox("infinite duration", &effect->infinite);
            ImGui_Checkbox("is visible", &effect->is_visible);


            ImGui_DragScalar("Emitter Count", ImGuiDataType_U32, &effect->emitter_count);
            for (u32 emitter_idx = 0; emitter_idx < effect->emitter_count; emitter_idx++)
            {
                ImGui_String(effect->emitters[emitter_idx]->name);
                ImGui_DragScalar("emitter_start", ImGuiDataType_U32, &effect->emitters_start[emitter_idx]);
                ImGui_DragScalar("emitter_end", ImGuiDataType_U32, &effect->emitters_end[emitter_idx]);
            }
        }
    }
    ImGui_End();


    ImGui_Begin("Particle Effect Emitter View", 0, 0);
    {
        Particle_Effect* effect = &particle_system->particle_effects[effect_index];


        if (effect->emitter_count > 0)
        {
            Particle_Emitter* emitter = effect->emitters[effect_emitter_index];

            ImGui_Text("NAME: ");
            ImGui_SameLine();
            ImGui_String(emitter->name);

            Reflection_Runtime_Struct emitter_runtime_struct = reflection_registry_get_struct(
                editor->asset_system->global_reflection_registry, TYPE_STRING(Particle_Emitter_Data));
            Imgui_Reflect_Using_Data(editor->asset_system->global_reflection_registry, emitter_runtime_struct,
                                          &emitter->data, "emitter", editor->frame_allocator);
        }
    }
    ImGui_End();

    ImGui_Begin("Particle Active View", 0, 0);
    {
        for (u32 i = 0; i < particle_system->active_emitters->num_items; i++)
        {
            u32 index = array_get(particle_system->active_emitters, i, u32);

            ImGui_String(particle_system->emitters[index].name);
        }

        for (u32 i = 0; i < particle_system->active_effects->num_items; i++)
        {
            u32 index = array_get(particle_system->active_effects, i, u32);

            ImGui_String(particle_system->particle_effects[index].name);
        }
    }
    ImGui_End();


    PROFILE_ZONE_END(editor_particle_view);
}


void editor_material_editor(Editor* editor)
{
    Asset_System* asset_system = editor->asset_system;

    static Material_Handle handle;
    ImGui_Begin("Material Table", 0, 0);
    {
        //
        if (ImGui_Button("VALIDATE MATERIALS IN REGISTRY"))
        {
            asset_registry_validate_materials(asset_system);
        }


        static char path[1024];
        ImGui_file_picker("material path", path, 1024,
                          "C:/Users/Adams Humbert/Documents/Clion/MadnessEngine/z_assets_engine/material");
        if (ImGui_Button("LOAD MATERIAL ASSET"))
        {
            String_Builder* builder_path = string_builder_create(256, editor->frame_allocator);
            string_builder_append_c_string(builder_path, path);
            string_builder_convert_path_to_proper_path(builder_path);
            string_builder_strip_path_to_base_name(builder_path);


            String_Builder* final_path = string_builder_create(256, editor->frame_allocator);
            string_builder_append_c_string(final_path, ENGINE_MATERIAL_PATH);
            string_builder_append_string_builder(final_path, builder_path);

            asset_load_material_path(editor->asset_system,
                                     string_builder_to_c_string(final_path),
                                     &handle);
        }

        if (handle.handle != 0)
        {
            Material_Asset* mat = material_system_get_asset(asset_system->material_system, handle);
            material_system_get_asset(asset_system->material_system, handle);
            // Material_Spherical_Billboard_CPU* mat_cast = mat->data;
            if (ImGui_Button("SAVE MATERIAL ASSET"))
            {
                asset_converter_material(asset_system, mat);
            }


            Reflection_Runtime_Struct material_reflect_struct = reflection_registry_get_struct(
                asset_system->global_reflection_registry,
                string_to_c_string_allocator(mat->material_name, editor->frame_allocator));

            ImGui_Spacing();

            ImGui_Reflect_Material(editor->asset_system->asset_registry,
                                   asset_system->global_reflection_registry,
                                   material_reflect_struct,
                                   mat->data,
                                   "mat",
                                   editor->texture_list, editor->frame_allocator);


            //TODO: show gpu data
            Material_Definition material_definition = material_system_get_material_definition(asset_system, handle);
            void* mat_data = material_system_get_material_data(asset_system->material_system, handle);
        }
    }
    ImGui_End();


    static Shader_Handle shader_handle;
    ImGui_Begin("Shader Table", NULL, 0);
    {
        if (ImGui_Button("VALIDATE SHADERS IN REGISTRY"))
        {
            asset_registry_validate_shaders(asset_system);
        }

        static char shader_path[1024];
        ImGui_file_picker("Shader Path", shader_path, 1024,
                          "C:/Users/Adams Humbert/Documents/Clion/MadnessEngine/z_assets_engine/shader");
        if (ImGui_Button("LOAD SHADER ASSET"))
        {
            String_Builder* path = string_builder_create(256, editor->frame_allocator);
            string_builder_append_c_string(path, shader_path);
            string_builder_convert_path_to_proper_path(path);
            string_builder_strip_path_to_base_name(path);


            String_Builder* final_path = string_builder_create(256, editor->frame_allocator);
            string_builder_append_c_string(final_path, ENGINE_SHADER_PATH);
            string_builder_append_string_builder(final_path, path);

            asset_load_shader_asset_path(editor->asset_system,
                                         string_builder_to_c_string(final_path),
                                         &shader_handle);
        }

        if (shader_handle.handle != 0)
        {
            Shader_Asset* asset = shader_asset_get(asset_system->shader_system,
                                                   shader_handle);

            ImGui_Text("VERSION: %d", &asset->version);
            ImGui_Text("UUID HIGH: %llu", &asset->uuid.high);
            ImGui_Text("UUID LOW: %llu", &asset->uuid.low);

            ImGui_TextUnformatted("shader name:");
            ImGui_SameLine();
            ImGui_String(asset->shader_pipeline_state.shader_name);

            ImGui_Text("BLEND_MODE: #%d ->", (u32)asset->shader_pipeline_state.blend_mode, 0);
            ImGui_SameLine();
            ImGui_TextUnformatted(Shader_Blend_Mode_enum_string[asset->shader_pipeline_state.blend_mode]);

            // disables input to make this read only
            ImGui_BeginDisabled(true);
            ImGui_Checkbox("TWO_SIDED", &asset->shader_pipeline_state.two_sided);
            ImGui_EndDisabled();

            ImGui_Text("SHADER PIPELINE HASH: %llu", &asset->shader_pipeline_hash, 0);
            ImGui_Text("SHADER INFO: KEY %llu", &asset->shader_key, 0);
        }
    }
    ImGui_End();
}
