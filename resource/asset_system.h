#ifndef RESOURCE_SYSTEM_H
#define RESOURCE_SYSTEM_H

#include <stdbool.h>

#include "resource_types.h"
#include "ring_queue.h"


#define IMPORT_RESOURCE_PATH "../z_assets/asset_list"
#define IMPORT_TEXTURE_PATH "../z_assets/texture"
#define IMPORT_FONTS_PATH "../z_assets/fonts"
#define IMPORT_MESH_PATH "../z_assets/mesh"
#define IMPORT_SK_MESH_PATH "../z_assets/skinned_mesh"
#define IMPORT_AUDIO_PATH "../z_assets/audio"
#define IMPORT_RESOURCE_PATH "../z_assets/asset_list"

#define EDITOR_TEXTURE_PATH "../z_assets_editor/texture"
#define EDITOR_FONTS_PATH "../z_assets_editor/fonts"
#define EDITOR_MESH_PATH "../z_assets_editor/mesh"
#define EDITOR_SK_MESH_PATH "../z_assets_editor/skinned_mesh"
#define EDITOR_AUDIO_PATH "../z_assets_editor/audio"
#define EDITOR_RESOURCE_PATH "../z_assets_editor/asset_list"



#define ENGINE_RESOURCE_PATH "../z_assets_engine/asset_list"
#define ENGINE_TEXTURE_PATH "../z_assets_engine/texture"
#define ENGINE_FONTS_PATH "../z_assets_engine/fonts"
#define ENGINE_MESH_PATH "../z_assets_engine/mesh"
#define ENGINE_SK_MESH_PATH "../z_assets_engine/skinned_mesh"
#define ENGINE_AUDIO_PATH "../z_assets_engine/audio"

//TODO/GOALS:
// placing or updating a file in the asset directory should (re)generate the asset,
// I should be able to edit the text format and have it automatically change within the engine
// It most likely during development/in the editor never delete anything
// source file -> intermediate text format (no binary, just metadata)  -> runtime binary blob
// I dont want to refer to the asset by full path name, (NO) dir/other_dir/rose.gltf -> (YES) "rose.mmsh" (rose. madness mesh)
// i dont want a seperate build for the editor and game, just a flag to toggle them on and off, and also toggles the asset manaager, and which functions it uses

//ideally we have an engine format that tells us as much about
//the asset we want to know and is reasonably fast to load
//we store the metadata, but just dump the binary data on load


//downsides:
//no way to rename assets,
//no asset cross-referencing,
//


//design:
//assume we load in mesh1, then we change it and now we have more vertex data, do we delete the old asset?
//or just set a flag to not render and put the new one in,
//basically just find the old data, update the mesh data and point it to the correct vertex spot, no unloading, just restart the engine
//




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
    bool nothing;
} Madness_Audio;
*/




Asset_System* asset_system_init(Memory_System* memory_system);

bool asset_system_shutdown(Asset_System* resource_system, Memory_System* memory_system);


bool asset_system_update_and_create_render_packet(Asset_System* resource_system);

bool asset_system_generate_render_packet(Asset_System* resource_system);
MAPI void render_packet_clear(Render_Packet* renderer_packets);


//NEW


void asset_system_reload_asset();

//should we have the asset system be responsible for basically everything, kinda, it should probably be able to touch everything
Texture_Handle asset_load_texture(Asset_System* a, const char* path)
{
    // return mesh_load_gltf(a, path);
    return texture_system_load_texture(a, path);
}

Texture_Handle asset_loader_texture(Asset_System* a, const char* path)
{
    // return mesh_load_gltf(a, path);
    return texture_system_load_texture(a, path);
}





//Data format stuff



typedef struct Asset_File_Header
{
    Asset_Type type;
    String source_file;
} Asset_File_Header;

typedef struct Asset_Editor_File_Header
{
    u64 count;
} Asset_Editor_File_Header;

#define Asset_Registry_File_Path

typedef struct Asset_Editor_Registry_Header
{
    u64 count;
} Asset_Editor_Registry_Header;


typedef struct Madness_Runtime_Data
{
    u32 magic_number;
    u32 version;

    size_t level_entry_count; // how many levels we have
    size_t level_entry_size; // byte size of all of them

    size_t data_size; // size of our actual binary blob
} Madness_Runtime_Data;

typedef struct Madness_Runtime_Asset
{
    Asset_Type type;
    size_t offset;
    size_t size;
} Madness_Runtime_Asset;

typedef struct Madness_Runtime_Level_Table
{
    char level_name[256]; //might not even need this
    size_t level_id;
    size_t offset;
    size_t size;
    size_t asset_count; // how many assets do we have

    //list of assets names and their info
    u64* asset_id; // we will want to hash the source file name, and somehow handle id collisions (or pray we dont have any)
    Madness_Runtime_Asset* asset_info;
} Madness_Runtime_Level_Table;



void asset_system_read_registry(Asset_System* asset_system)
{
}

void asset_system_update_registry(Asset_System* asset_system)
{
    //TODO:
}


void asset_system_convert_to_runtime_format()
{

}


#endif //RESOURCE_SYSTEM_H
