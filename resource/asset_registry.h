#ifndef ASSET_REGISTRY_H
#define ASSET_REGISTRY_H

#include "asset_serialization.h"
#include "resource_types.h"

//AR = madness asset registry
#define ASSET_REGISTRY_BIN_PATH "../z_assets_engine/MAR.bin"
#define ASSET_REGISTRY_MAGIC_NUMBER "MARS"
#define ASSET_REGISTRY_VERSION 1


typedef struct Asset_Registry_Header
{
    u8 magic[4];
    u32 version;
    u64 asset_count;
} Asset_Registry_Header;


#define TEXTURE_VERSION 1
#define FONTS_VERSION 1
#define MESH_VERSION 1
#define SKMESH_VERSION 1
#define SHADER_VERSION 1
#define MATERIAL_INSTANCE_VERSION 1
#define AUDIO_EXTENSION_VERSION 1
#define PARTICLE_EFFECT_VERSION 1
#define PARTICLE_EMITTER_VERSION 1


typedef struct Texture_MetaData
{
    u32 version;
    Texture_Type type; // used to identify if we have are a font or a texture

    //
    u32 width;
    u32 height;
    u8 channels;

    //unused for rn
    Texture_Format format;
    Texture_Filter filter;
    Texture_Wrap wrap;
    Texture_Sampler sampler;
    bool create_sampler;


    MADNESS_UUID uuid;

    String* source_path; //256 in length max
    String* engine_path; //256 in length max
} Texture_MetaData;


typedef struct Mesh_MetaData
{
    u32 version;
    //
    u32 mesh_count;
    String* mesh_names;
    MADNESS_UUID* material_uuids;

    vec3s initial_pos;
    vec3s initial_rotation;
    vec3s initial_scale;
} Mesh_MetaData;

typedef struct Skinned_Mesh_MetaData
{
    u32 version;
    //
    u32 mesh_count;
    String* mesh_names;
    MADNESS_UUID* material_uuids;

    vec3s initial_pos;
    vec3s initial_rotation;
    vec3s initial_scale;
} Skinned_Mesh_MetaData;

typedef struct Material_MetaData
{
    u32 version;
    //
    MADNESS_UUID shader_uuid;
    MADNESS_UUID material_uuid;

    String* material_name;
    String* instance_name;
} Material_MetaData;


typedef struct Shader_MetaData
{
    u32 version;
    //
    u32 reflection_hash;
    MADNESS_UUID uuid;
    Shader_Key shader_key;

    Path_String* shader_name;
    String* material_name;

    Shader_Info shader_info;
} Shader_MetaData;


bool asset_registry_init(Asset_System* asset_system, Asset_Registry* asset_registry, Heap_Allocator* allocator,
                         Memory_System* memory_system);

void asset_registry_shutdown(Asset_Registry* asset_registry);

void asset_registry_scan_for_new_assets(Asset_System* asset_system, Asset_Registry* asset_registry,
                                        Memory_System* memory_system, Asset_Type asset_type);

//
void asset_registry_append_to_file(Asset_Registry* asset_registry, Asset_Record* asset_meta_data);

void asset_registry_add_asset_from_uuid(Asset_Registry* asset_registry,
                                        const char* source_path,
                                        const char* engine_path,
                                        Asset_Type asset_type, Heap_Allocator* allocator,
                                        MADNESS_UUID uuid);
void asset_registry_add_asset_and_generate_uuid(Asset_Registry* asset_registry, const char* source_path,
                                                const char* engine_path,
                                                Asset_Type asset_type, Heap_Allocator* allocator,
                                                MADNESS_UUID* out_uuid);

bool asset_registry_overwrite_file(Asset_Registry* asset_registry);


//functions to find the asset and return the metadata associated with it
bool asset_registry_exists_by_engine_path(Asset_Registry* asset_registry, String* engine_path,
                                          Asset_Record* out_record_data);

bool asset_registry_exists_by_source_path(Asset_Registry* asset_registry, String* source_path,
                                          Asset_Type asset_type, Asset_Record* out_record_data);

bool asset_registry_exists_by_uuid(Asset_Registry* asset_registry, MADNESS_UUID uuid,
                                   Asset_Record* out_record_data);


Dynamic_Array* asset_registry_get_all_assets_of_type(Asset_Registry* asset_registry, Asset_Type type,
                                                     Allocator* allocator);


//new functions


//type specific functions
void asset_registry_add_texture_metadata(Asset_Registry* asset_registry, Texture_MetaData meta_data)
{
    dynamic_array_push(asset_registry->texture_table, &meta_data);
}

void asset_registry_remove_texture_metadata(Asset_Registry* asset_registry, const char* path)
{
    //loop throught and find the texture, then remove it
    u32 texture_index = 0;
    dynamic_array_remove_swap(asset_registry->texture_table, texture_index);
}


void asset_registry_texture_metadata_serialize(Asset_Registry* asset_registry, FILE* fptr)
{
    fwrite(&asset_registry->texture_table->num_items, 1, sizeof(asset_registry->texture_table->num_items), fptr);

    Texture_MetaData meta_data = {0};
    fwrite(&meta_data.version, 1, sizeof(meta_data.version), fptr);
    fwrite(&meta_data.width, 1, sizeof(meta_data.width), fptr);
    fwrite(&meta_data.height, 1, sizeof(meta_data.height), fptr);
    fwrite(&meta_data.channels, 1, sizeof(meta_data.channels), fptr);
    fwrite(&meta_data.format, 1, sizeof(meta_data.format), fptr);
    fwrite(&meta_data.wrap, 1, sizeof(meta_data.wrap), fptr);
    fwrite(&meta_data.sampler, 1, sizeof(meta_data.sampler), fptr);
    fwrite(&meta_data.create_sampler, 1, sizeof(meta_data.create_sampler), fptr);
    madness_uuid_serialize(meta_data.uuid, fptr);
    string_serialize(meta_data.engine_path, fptr);
}

void asset_registry_texture_metadata_deserialize(Asset_Registry* asset_registry)
{
}


void asset_registry_validate_textures()
{
    //goes through our mtex and makes sure they are in the registry
    //get assets in texture asset list
    //for each texture in asset list, open and get the uuid
    //check our records for the asset, if exist continue, otherwise add it in
    //might also want to validate by checking for any source/engine paths as well to make sure uuid's match up
}


void asset_registry_get_texture_metadata_uuid(MADNESS_UUID uuid);

void asset_registry_get_texture_metadata_path(const char* path);


///////////// VALIDATION FUNCTIONS /////////////

void asset_registry_validate_shaders(Asset_System* asset_system)
{
    Asset_List_Scan list_scan = {0};
    list_scan.count = 0;
    list_scan.allocator = asset_system->frame_allocator;
    list_scan.max_count = MAX_ASSETS_STRINGS;
    list_scan.strings = allocator_alloc(asset_system->frame_allocator,
                                        sizeof(String) * MAX_ASSETS_STRINGS);

    platform_get_assets_from_directory(ENGINE_SHADER_PATH_NO_SLASH, &list_scan);

    for (u32 i = 0; i < list_scan.count; i++)
    {
        Scratch_Allocator scratch = scratch_allocator_begin(asset_system->scratch_allocator);
        const char* file_path = string_to_c_string_allocator(&list_scan.strings[i], scratch.allocator);

        FILE* fptr = fopen(file_path, "rb");
        MASSERT(fptr);

        Shader_Asset asset;
        asset_shader_deserialize(&asset, fptr, scratch.allocator);

        Asset_Record record = {0};
        if (asset_registry_exists_by_engine_path(asset_system->asset_registry, &list_scan.strings[i], &record))
        {
            //validate all the information like uuid's and if source files exist
            // MASSERT_FALSE();
            if (!madness_uuid_compare(record.uuid, asset.uuid))
            {
                MASSERT_FALSE();
            }
        }
        else
        {
            //add it to the asset registry
            //i want to manually check this
            MASSERT_FALSE();
            asset_registry_add_asset_from_uuid(asset_system->asset_registry, file_path, file_path, ASSET_SHADER,
                                               asset_system->heap_allocator, asset.uuid);
        }

        fclose(fptr);

        scratch_allocator_end(scratch);
    }

    asset_registry_overwrite_file(asset_system->asset_registry);

}

void asset_registry_validate_materials(Asset_System* asset_system)
{
    Asset_List_Scan list_scan = {0};
    list_scan.count = 0;
    list_scan.allocator = asset_system->frame_allocator;
    list_scan.max_count = MAX_ASSETS_STRINGS;
    list_scan.strings = allocator_alloc(asset_system->frame_allocator,
                                        sizeof(String) * MAX_ASSETS_STRINGS);

    platform_get_assets_from_directory(ENGINE_MATERIAL_PATH_NO_SLASH, &list_scan);

    for (u32 i = 0; i < list_scan.count; i++)
    {
        Scratch_Allocator scratch = scratch_allocator_begin(asset_system->scratch_allocator);
        const char* file_path = string_to_c_string_allocator(&list_scan.strings[i], scratch.allocator);

        FILE* fptr = fopen(file_path, "rb");
        MASSERT(fptr);

        Material asset = {0};
        asset_material_deserialize(&asset, fptr, asset_system->heap_allocator);

        Asset_Record record = {0};
        if (asset_registry_exists_by_engine_path(asset_system->asset_registry, &list_scan.strings[i], &record))
        {
            //validate all the information like uuid's and if source files exist
            // MASSERT_FALSE();
            if (!madness_uuid_compare(record.uuid, asset.meta_data.material_uuid))
            {
                //NOTE: basically this is really bad if you hit this
                // MASSERT_FALSE();

                record.uuid = asset.meta_data.material_uuid;

            }
        }
        else
        {
            //add it to the asset registry
            //i want to manually check this
            MASSERT_FALSE();
            asset_registry_add_asset_from_uuid(asset_system->asset_registry, file_path, file_path, ASSET_MATERIAL,
                                               asset_system->heap_allocator, asset.meta_data.material_uuid);
        }

        fclose(fptr);

        scratch_allocator_end(scratch);
    }


    asset_registry_overwrite_file(asset_system->asset_registry);

}


#endif
