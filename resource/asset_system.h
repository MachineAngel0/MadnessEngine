#ifndef RESOURCE_SYSTEM_H
#define RESOURCE_SYSTEM_H

#include <stdbool.h>

#include "asset_converter.h"
#include "resource_types.h"
#include "ring_queue.h"
#include "texture_system.h"


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
//assets imported with different names, are treated as a different asset, even if they are the same as another asset

//TODO:
// METADATA
// ENGINE FORMAT
// WAY TO CREATE MULTIPLE OF A THING for modification



Asset_System* asset_system_init(Memory_System* memory_system);

bool asset_system_shutdown(Asset_System* resource_system, Memory_System* memory_system);


bool asset_system_update_and_create_render_packet(Asset_System* resource_system);

bool asset_system_generate_render_packet(Asset_System* resource_system);
MAPI void render_packet_clear(Render_Packet* renderer_packets);


//NEW


void asset_system_reload_texture(Asset_System* a);

//should we have the asset system be responsible for basically everything, kinda, it should probably be able to touch everything
Texture_Handle asset_load_texture(Asset_System* asset_system, const char* engine_asset_path);
bool asset_system_unload_texture(Asset_System* asset_system, Texture_Handle texture_handle);

bool asset_load_font(Asset_System* asset_system, const char* engine_asset_path, Texture_Handle* out_handle);
Texture_Handle asset_unload_font(Asset_System* asset_system, const char* asset_name);//TODO:


//Data format stuff
typedef struct Asset_MetaData_Header
{
    u8 magic_number[4];
    u32 version;
    size_t offset;
    size_t size;
} Asset_MetaData_Header;


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

    //NOTE: you can technically sort this since its by id and then just binary search through the array
    //list of assets names and their info
    u64* asset_id;
    // we will want to hash the source file name, and somehow handle id collisions (or pray we dont have any)
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
