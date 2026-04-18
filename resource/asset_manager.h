#ifndef RESOURCE_SYSTEM_H
#define RESOURCE_SYSTEM_H


#include "arena.h"
#include "resource_types.h"




//TODO: rn its not in use,
// but this most likely should handle loading assets in the editor
// then save them to game/engine specific format
// resource system will then load the actual game/engine specific format

typedef struct Asset_Editor_File_Header
{

    u64 count;
}Asset_Editor_File_Header;


typedef struct Asset_Manager
{
    bool unimplemented;


}Asset_Manager;

#define ENGINE_RESOURCE_PATH "../z_assets_engine_format/asset_list"

#define ENGINE_TEXTURE_PATH "../z_assets_engine_format/texture"
#define ENGINE_FONTS_PATH "../z_assets_engine_format/fonts"
#define ENGINE_MESH_PATH "../z_assets_engine_format/mesh"
#define ENGINE_SK_MESH_PATH "../z_assets_engine_format/skinned_mesh"
#define ENGINE_AUDIO_PATH "../z_assets_engine_format/audio"

void static_mesh_to_madness_mesh(static_mesh* s_mesh, const char* file_name, Frame_Arena* frame_arena);
void sk_mesh_to_sk_madness_mesh(skinned_mesh* sk_mesh, const char* file_name, Frame_Arena* frame_arena);


void texture_to_madness_texture(Frame_Arena* frame_arena);


void sound_to_madness_sound(Frame_Arena* frame_arena);





#endif //RESOURCE_SYSTEM_H
