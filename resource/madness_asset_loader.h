#ifndef MADNESS_ASSET_LOADER_H
#define MADNESS_ASSET_LOADER_H


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


typedef struct Madness_Texture
{
    u32 width;
    u32 height;
    u8 channels;
    //enum texture format

    u64 pixels_size;
    u8* pixels_data;

    //bool free_after_use

} Madness_Texture;


typedef struct Madness_Mesh
{
    u64 tangent_byte_size;
    u64 vertex_byte_size;
    u64 normal_byte_size;
    u64 uv_byte_size;
    u64 indices_byte_size;

    vec4s* tangent;
    vec3s* vertex;
    vec3s* normal;
    vec2s* uv;
    u8* indices;

    String* material_name;


    Madness_Texture* texture;
    u8 texture_count;

} Madness_Mesh;

typedef struct Madness_SkMesh
{
    u64 joint_byte_size;
    u64 weight_byte_size;
    u64 tangent_byte_size;
    u64 vertex_byte_size;
    u64 normal_byte_size;
    u64 uv_byte_size;
    u64 indices_byte_size;


    vec4s* joints;
    vec4s* weights;
    vec4s* tangent;
    vec3s* vertex;
    vec3s* normal;
    vec2s* uv;
    u8* indices;

    //TODO: animation_data

    String* material_name;


    Madness_Texture* texture;
    u8 texture_count;

} Madness_SkMesh;


/*
typedef struct Madness_Audio
{


} Madness_Audio;
*/


#define ENGINE_RESOURCE_PATH "../z_assets_engine_format/asset_list"

#define ENGINE_TEXTURE_PATH "../z_assets_engine_format/texture"
#define ENGINE_FONTS_PATH "../z_assets_engine_format/fonts"
#define ENGINE_MESH_PATH "../z_assets_engine_format/mesh"
#define ENGINE_SK_MESH_PATH "../z_assets_engine_format/skinned_mesh"
#define ENGINE_AUDIO_PATH "../z_assets_engine_format/audio"


















#endif //MADNESS_ASSET_LOADER_H
