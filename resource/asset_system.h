#ifndef ASSET_SYSTEM_H
#define ASSET_SYSTEM_H

#include <stdbool.h>
#include "asset_converter.h"
#include "asset_registry.h"
#include "asset_serialization.h"
#include "material_system.h"
#include "resource_types.h"
#include "particle_system.h"


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



typedef struct Asset_Load_Handle
{
    //for when we use a more generic load function call
    //we use the type to get the type of handle we were given back
    Asset_Type type;
    void* handle;
} Asset_Load_Handle;

Asset_System* asset_system_init(Memory_System* memory_system,
                                Reflection_Registry* global_reflection_registry,
                                Reflection_Registry* material_reflection_registry);

bool asset_system_shutdown(Asset_System* resource_system, Memory_System* memory_system);


bool asset_system_update_and_create_render_packet(Asset_System* asset_system);

MAPI void render_packet_clear(Render_Packet* renderer_packets);


//ASSET MANAGER
//NOTE: these only load from the engine path, as a fallback they will try to look for the import path file
// you only need to pass in the asset name, like error_texture and not error_texture.mtex

bool asset_system_does_source_already_exist(const char* import_file_path);
bool asset_system_does_engine_already_exist(const char* engine_file_path);

Texture_Handle asset_load_texture_path(Asset_System* asset_system, const char* asset_path);
Texture_Handle asset_load_texture_uuid(Asset_System* asset_system, MADNESS_UUID uuid);
bool asset_system_unload_texture(Asset_System* asset_system, Texture_Handle texture_handle);


Texture_Handle asset_load_font_path(Asset_System* asset_system, const char* engine_asset_path);
Texture_Handle asset_load_font_uuid(Asset_System* asset_system, MADNESS_UUID uuid);
bool asset_unload_font(Asset_System* asset_system, Texture_Handle texture_handle);


bool asset_load_mesh_path(Asset_System* asset_system, const char* engine_asset_path, Mesh_Handle* out_handle);
Mesh_Handle asset_load_mesh_uuid(Asset_System* asset_system, MADNESS_UUID uuid);

Skinned_Mesh_Handle asset_load_skmesh(Asset_System* asset_system, const char* engine_asset_path);


bool _asset_load_shader_asset(Asset_System* asset_system, Asset_MetaData* meta_data,
                             Shader_Handle* out_handle, Scratch_Allocator* scratch_allocator);
bool asset_load_shader_asset_path(Asset_System* asset_system, const char* asset_path,
                                  Shader_Handle* out_shader_handle);
bool asset_load_shader_asset_uuid(Asset_System* asset_system, MADNESS_UUID uuid, Shader_Handle* out_handle);

bool asset_unload_shader_asset(Asset_System* asset_system, Shader_Handle shader_handle);


bool _asset_load_material(Asset_System* asset_system, Asset_MetaData* meta_data, Material_Handle* out_material_handle);
bool asset_load_material_path(Asset_System* asset_system,  const char* asset_path,  Material_Handle* out_material);
bool asset_load_material_uuid(Asset_System* asset_system, MADNESS_UUID madness_uuid, Material_Handle* out_material);
bool asset_unload_material(Asset_System* asset_system, const char* asset_path);


bool asset_load_particle_effect_by_path(Asset_System* asset_system, const char* asset_path,
                                        Particle_Effect_Handle* out_handle);


bool asset_unload_particle_effect(Asset_System* asset_system, Particle_Effect_Handle out_handle);


bool asset_load_particle_emitter(Asset_System* asset_system, const char* asset_path,
                                 Particle_Emitter_Handle* out_handle)
{
    MASSERT(asset_system);
    MASSERT(asset_path);
    MASSERT(out_handle);

    PROFILE_ZONE(asset_load_particle_emitter)


    Scratch_Allocator scratch = scratch_allocator_begin(asset_system->allocator);

    String* path_string = STRING_CREATE_FROM_BUFFER_ALLOCATOR(asset_path, scratch.allocator);

    Asset_MetaData* out_meta_data = allocator_alloc(asset_system->frame_allocator, sizeof(Asset_MetaData));
    if (!asset_registry_exists_by_engine_path(asset_system->asset_registry, path_string, out_meta_data))
    {
        MASSERT_MSG(false, "PLZ CONVERT ASSET")
        *out_handle = (Particle_Emitter_Handle){0, 0};
        PROFILE_ZONE_END(asset_load_particle_emitter)

        return false;
    }

    //check if the emitter has already been loaded
    for (u32 asset_idx = 0; asset_idx < MAX_PARTICLE_EMITTER_COUNT; asset_idx++)
    {
        if (asset_system->asset_registry->particle_emitter_asset[asset_idx].path_hash == 0) { continue; }
        if (string_compare(asset_system->asset_registry->particle_emitter_asset[asset_idx].engine_path, path_string))
        {
            asset_system->asset_registry->particle_emitter_asset[asset_idx].reference_count++;

            *out_handle = (Particle_Emitter_Handle){
                .handle = asset_idx,
                .gen = asset_system->particle_system->emitter_generation[asset_idx],
            };
            PROFILE_ZONE_END(asset_load_particle_emitter)

            return true;
        }
    }

    //the asset isn't loaded, so we load it in asset
    FILE* fptr = fopen(string_to_c_string_allocator(path_string, scratch.allocator), "rb");

    if (!fptr)
    {
        MASSERT(false);
        PROFILE_ZONE_END(asset_load_particle_emitter)

        return false;
    }

    //effect -> emitter[]
    //emitter[] -> asset or part of the effect


    //grab an available particle effect, with its handle
    Particle_Emitter* particle_emitter = particle_emitter_acquire(asset_system->particle_system, out_handle);
    if (!particle_emitter)
    {
        MASSERT_MSG_FALSE("COULD NOT FIND A PARTICLE EFFECT NOT LOADING IN PARTICLE");

        *out_handle = asset_system->particle_system->default_emitter_handle;
        PROFILE_ZONE_END(asset_load_particle_emitter)

        return false;
    }

    particle_emitter_deserialize(particle_emitter, fptr, asset_system->heap_allocator);
    fclose(fptr);
    scratch_allocator_end(scratch);

    //we also have to load in the material
    if (!asset_load_material_uuid(asset_system, particle_emitter->material_uuid, &particle_emitter->material_handle))
    {
        MASSERT_FALSE();
    }



    Madness_Asset* madness_asset = &asset_system->asset_registry->particle_emitter_asset[out_handle->handle];
    madness_asset->path_hash = out_meta_data->hash;
    madness_asset->engine_path = out_meta_data->engine_path;
    madness_asset->type = ASSET_PARTICLE_EMITTER;
    madness_asset->reference_count = 1;



    PROFILE_ZONE_END(asset_load_particle_emitter)


    return true;
}

bool asset_unload_particle_emitter(Asset_System* asset_system, Particle_Emitter_Handle handle);



void madness_asset_set_info(Madness_Asset* asset, Asset_MetaData* meta_data, Asset_Type asset_type)
{
    asset->reference_count = 1;
    asset->engine_path = meta_data->engine_path;
    asset->path_hash = meta_data->hash;
    asset->type = asset_type;
}


/**
 * check the bool, as that means we need to free this asset
 * decrements ref count
 */
bool madness_asset_release(Madness_Asset* asset)
{
    asset->reference_count--;
    if (asset->reference_count <= 0)
    {
        //unload the asset
        asset->path_hash = 0;
        asset->engine_path = NULL;
        asset->type = ASSET_TYPE_MAX;
        return true;
    }

    return false;
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


void asset_system_convert_to_runtime_format(void)
{
}


#endif //ASSET_SYSTEM_H
