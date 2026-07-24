#ifndef RESOURCE_SYSTEM_H
#define RESOURCE_SYSTEM_H

#include <stdbool.h>

#include "asset_converter.h"
#include "asset_registry.h"
#include "asset_serialization.h"
#include "resource_types.h"



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
Texture_Handle asset_load_texture_path(Asset_System* asset_system, const char* asset_path);
bool asset_system_unload_texture(Asset_System* asset_system, Texture_Handle texture_handle);

bool asset_load_texture_uuid(Asset_System* asset_system, MADNESS_UUID uuid, Texture_Handle* out_handle)
{
    Asset_MetaData meta_data = {0};

    if (uuid.high == 0 && uuid.low == 0)
    {
        WARN("UUID OF 0,0 passed in ")
        *out_handle = (Texture_Handle){0};
        return true;
    }

    if (!asset_registry_get_metadata_from_uuid(asset_system, uuid, &meta_data))
    {
        MASSERT_MSG(false, "PLZ CONVERT ASSET")
        *out_handle = (Texture_Handle){0};
        return out_handle;
    }


    //has asset already been loaded
    if (texture_system_exists(asset_system, out_handle, meta_data.hash))
    {
        return true;
    }


    FILE* fptr = fopen(string_to_c_string_allocator(meta_data.binary_file, asset_system->frame_allocator), "rb");
    if (!fptr)
    {
        MASSERT(false);
        *out_handle = (Texture_Handle){0};
        return false;
    }

    bool editor = true;
    if (editor)
    {
        Madness_Texture_Runtime runtime = {0};
        asset_texture_deserialize_heap(&runtime, fptr, asset_system->heap_allocator);
        texture_system_upload_new_texture(asset_system, meta_data.uuid, meta_data.hash, runtime.texture, runtime.pixel_data, out_handle);
        return true;
    }else
    {
        MASSERT(false);

    }


    return true;
}



bool asset_load_font(Asset_System* asset_system, const char* engine_asset_path, Texture_Handle* out_handle);
Texture_Handle asset_unload_font(Asset_System* asset_system, const char* asset_name); //TODO:

bool asset_load_mesh_uuid(Asset_System* asset_system, MADNESS_UUID* uuid, Madness_Mesh_Handle* out_handle)
{
    MASSERT(false); // TOOD:
}
bool asset_load_mesh_path(Asset_System* asset_system, const char* engine_asset_path, Madness_Mesh_Handle* out_handle)
{
    MADNESS_UUID uuid = {0, 0};
    u64 hash = 0;

    String* asset_path_string = STRING_CREATE_FROM_BUFFER_ALLOCATOR(engine_asset_path, asset_system->frame_allocator);

    if (!asset_registry_exists_by_engine_path(asset_system, asset_path_string, &uuid, &hash))
    {
        MASSERT_MSG(false, "PLZ CONVERT ASSET")
        *out_handle = (Madness_Mesh_Handle){0};
        return out_handle;
    }

    //has asset already been loaded
    if (mesh_system_exists_mesh(asset_system, out_handle, hash))
    {
        return true;
    }


    FILE* fptr = NULL;

    //load from individal binary
    bool debug = true;
    if (debug)
    {
        Madness_Mesh_Runtime runtime_mesh = {0};

        fptr = fopen(engine_asset_path, "rb");

        asset_mesh_deserialize_heap(&runtime_mesh, fptr, asset_system->heap_allocator);

        mesh_system_load_mesh(asset_system, &runtime_mesh);
    }
    else
    {
        //TODO:
        MASSERT(false);
        // search for asset by its hash name and its offset, then load it in with our format
        // u64 asset_offset = asset_system_find_asset(asset_system, scene_id, hash_id);
        // Madness_Texture_Runtime runtime_texture = {0};
        // texture_system_upload_new_texture(asset_system, hash_id, editor_texture.texture, editor_texture.pixel_data, &texture_handle);
    }
    fclose(fptr);

    return true;
}

bool asset_load_material(Asset_System* asset_system, MADNESS_UUID uuid, Material_Asset_Handle* out_handle)
{
    Asset_MetaData meta_data = {0};
    if (!asset_registry_get_metadata_from_uuid(asset_system,uuid, &meta_data))
    {
        MASSERT(false);
        return false;
    }


    //material system does exists function
    //has asset already been loaded
    // if (material_system_exists(asset_system, out_handle, hash))
    // {
    // return true;
    // }

    FILE* fptr = NULL;
    bool debug = true;
    if (debug)
    {
        fptr = fopen(string_to_c_string_allocator(meta_data.binary_file, asset_system->frame_allocator), "rb");

        Material_Asset_Runtime runtime_material = {0};
        runtime_material.asset = allocator_heap_alloc(asset_system->heap_allocator, sizeof(Madness_Mesh));
        asset_material_deserialize_heap(&runtime_material, fptr, asset_system->heap_allocator);
        material_system_load_material(asset_system, meta_data.uuid, meta_data.hash, &runtime_material, out_handle);
    }
    else
    {
        MASSERT(false);
        //TODO:
    }



    return true;

}

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
