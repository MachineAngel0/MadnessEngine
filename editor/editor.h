#ifndef MADNESSENGINE_EDITOR_H
#define MADNESSENGINE_EDITOR_H
#include <stdbool.h>

#include "ui_madness.h"
#include "memory/memory_system.h"

typedef enum Editor_UI_Views
{
    // EDITOR_UI_STATE_DEBUG,
    EDITOR_UI_VIEW_ENGINE_STATS,
    EDITOR_UI_VIEW_PARTICLE,
    EDITOR_UI_VIEW_RENDERER,
    EDITOR_UI_VIEW_MATERIAL_CREATION,
    EDITOR_UI_VIEW_MATERIAL_EDITOR,
    // EDITOR_UI_STATE_MADNESS_UI_TEST,
    EDITOR_UI_VIEW_SCENE,
    EDITOR_UI_VIEW_TEXTURE_VIEWER,
    EDITOR_UI_VIEW_ANIMATION,
    EDITOR_UI_VIEW_ASSET_METADATA,
    EDITOR_UI_VIEW_MESH_VIEWER,
    EDITOR_UI_VIEW_AUDIO,
    EDITOR_UI_VIEW_REFLECTION_ABILITY,
    EDITOR_UI_VIEW_INSANITY_UI_TEST,
    // EDITOR_UI_STATE_CHAOS_UI_TEST,
    EDITOR_UI_VIEW_MAX,
} Editor_UI_Views;

const char* Editor_UI_State_ENUM_String[EDITOR_UI_VIEW_MAX] = {
    // [EDITOR_UI_STATE_DEBUG] = "EDITOR_UI_STATE_DEBUG",
    [EDITOR_UI_VIEW_ENGINE_STATS] = "Engine Stats",
    [EDITOR_UI_VIEW_PARTICLE] = "Particle View",
    [EDITOR_UI_VIEW_RENDERER] = "Renderer View",
    [EDITOR_UI_VIEW_MATERIAL_CREATION] = "Material Creation View ",
    // [EDITOR_UI_STATE_MADNESS_UI_TEST] = "EDITOR_UI_STATE_MADNESS_UI_TEST",
    [EDITOR_UI_VIEW_SCENE] = "Scene View",
    [EDITOR_UI_VIEW_TEXTURE_VIEWER] = "Texture View",
    [EDITOR_UI_VIEW_ANIMATION] = "Animation View",
    [EDITOR_UI_VIEW_ASSET_METADATA] = "Asset Metadata View",
    [EDITOR_UI_VIEW_MESH_VIEWER] = "Mesh View",
    [EDITOR_UI_VIEW_AUDIO] = "Audio View",
    [EDITOR_UI_VIEW_REFLECTION_ABILITY] = "Reflection Ability View",
    [EDITOR_UI_VIEW_INSANITY_UI_TEST] = "Insanity UI View",
    [EDITOR_UI_VIEW_MATERIAL_EDITOR] = "Materiel Editor View",
    // [EDITOR_UI_STATE_CHAOS_UI_TEST] = "EDITOR_UI_STATE_CHAOS_UI_TEST",
};

typedef struct Editor_Serialization
{
    bool views[EDITOR_UI_VIEW_MAX];
}Editor_Serialization;

typedef struct Editor
{
    Renderer* renderer; // ref
    Asset_System* asset_system; // ref
    Clock* clock; // ref
    Reflection_Registry* reflection_registry; // ref
    Memory_System* memory_system; // ref


    Madness_txt* madness_txt; // ref
    Madness_Txt_Handle editor_config_file_handle;

    Allocator* editor_allocator;
    Frame_Allocator* frame_allocator;


    //checked by the applicaiton to see if we should run the game dll
    bool run_game_application;

    float lowest_ms;
    float highest_ms;


    //editor windows - tells us if a window is open/should be opened
    bool views[EDITOR_UI_VIEW_MAX];



    Texture_Handle directional_light_texture;


    //meshes
    Asset_List_Scan* texture_list;

    //meshes
    Asset_List_Scan* madness_mesh_list;

    //skeletal meshes
    Asset_List_Scan* madness_skmesh_list;

    //scene
    Asset_List_Scan* scene_list;

    //particles
    Asset_List_Scan* particle_effect_list;
    Asset_List_Scan* particle_emitter_list;

    //Material asset
    Asset_List_Scan* material_asset_list;


} Editor;


MAPI Editor* editor_init(Memory_System* memory_system, Renderer* renderer,
                         Asset_System* asset_system, Clock* clock,
                         Reflection_Registry* reflection_registry,
                         Madness_txt* txt);

Asset_List_Scan* asset_lists_generate(Memory_System* memory_system, u32 max_asset_count,
                                      const char* relative_asset_path);
bool editor_generate_asset_lists(Editor* editor, Memory_System* memory_system);


bool editor_deserialize_state(Editor* editor);
bool editor_serialize_state(Editor* editor);

MAPI bool editor_update(Editor* editor);
MAPI bool editor_shutdown(Editor* editor);


//UI_States
void editor_madness_ui_debug(Editor* editor);
void editor_ui_scene(Editor* editor);
void editor_texture_view(Editor* editor);
void editor_meta_data_view(Editor* editor);
void editor_ui_stats(Editor* editor);
void editor_ui_animation(Editor* editor);
void editor_material_asset_view(Editor* editor);
void editor_mesh_view(Editor* editor);
void editor_particle_view(Editor* editor);
void editor_render_view(Editor* editor);
void editor_audio_view(Editor* editor);
void editor_ui_reflection_ability_view(Editor* editor);


void editor_material_editor(Editor* editor);


#endif //MADNESSENGINE_EDITOR_H
