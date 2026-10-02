#ifndef ASSET_REGISTRY2_H
#define ASSET_REGISTRY2_H

#include "asserts.h"
#include "resource_types.h"

//AR = madness asset registry
// #define ASSET_REGISTRY_BIN_PATH "../z_assets_engine/MAR.bin"
// #define ASSET_REGISTRY_MAGIC_NUMBER "MARS"
#define ASSET_REGISTRY_BIN_PATH "../z_assets_engine/MARS2.bin"
#define ASSET_REGISTRY_BIN_TEMP_PATH "../z_assets_engine/MARS2_TEMP.bin"
#define ASSET_REGISTRY_MAGIC_NUMBER "MARS"

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

    //
    u32 width;
    u32 height;
    u8 channels;
    Texture_Format format;
    Asset_Type type; // used to identify if we have are a font or a texture
    bool create_sampler;
} Texture_MetaData;

typedef struct Texture
{
    u32 width;
    u32 height;
    u8 channels;
    u8* pixel_data;
} Texture;

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



typedef struct Asset_Registry_Header2
{
    u8 magic[4];
    u32 version;
    u64 asset_count;
} Asset_Registry_Header2;

typedef struct Asset_Record
{
    //meta data for our editor/debug builds
    MADNESS_UUID uuid; // not in use rn, but will be useful if i ever integrate asset renaming
    u64 hash; // hashes the engine_file
    Asset_Type type;
    u64 timestamp;
    String* source_file; //256 in length max
    String* engine_path; //256 in length max
} Asset_Record;

typedef struct Asset_Runtime
{
    MADNESS_UUID uuid;
    u64 hash; // hashes the engine_file
    Asset_Type type;
    String* engine_path; //256 in length max
    void* handle; //256 in length max
} Asset_Runtime;


typedef struct Asset_Registry2
{
    DYNAMIC_ARRAY_TYPE(Asset_Record)* asset_record;

    //metadata tables
    DYNAMIC_ARRAY_TYPE(Texture_MetaData)* texture_table;
    DYNAMIC_ARRAY_TYPE(Mesh_MetaData)* mesh_table;
    DYNAMIC_ARRAY_TYPE(Mesh_MetaData)* skinned_mesh_table;
    DYNAMIC_ARRAY_TYPE(Material_MetaData)* material_table;
    DYNAMIC_ARRAY_TYPE(Shader_MetaData)* shader_table;

    //runtime data
    DYNAMIC_ARRAY_TYPE(Madness_Asset)* texture_record_runtime;
    DYNAMIC_ARRAY_TYPE(Madness_Asset)* mesh_runtime;
    DYNAMIC_ARRAY_TYPE(Madness_Asset)* skinned_runtime;
    DYNAMIC_ARRAY_TYPE(Madness_Asset)* shader_runtime;
    DYNAMIC_ARRAY_TYPE(Madness_Asset)* material_runtime;

} Asset_Registry2;


bool asset_registry2_init(Asset_System* asset_system, Asset_Registry2* asset_registry, Memory_System* memory_system)
{
    //TODO: make sure all base paths exist
    platform_create_directory(ENGINE_AUDIO_PATH);
    platform_create_directory(ENGINE_FONTS_PATH);
    platform_create_directory(ENGINE_SHADER_PATH);
    platform_create_directory(ENGINE_MATERIAL_INSTANCE_PATH);
    platform_create_directory(ENGINE_MESH_PATH);
    platform_create_directory(ENGINE_PARTICLE_PATH);
    platform_create_directory(ENGINE_PARTICLE_EFFECT_PATH);
    platform_create_directory(ENGINE_PARTICLE_EMITTER_PATH);
    platform_create_directory(ENGINE_SCENE_PATH);
    platform_create_directory(ENGINE_SK_MESH_PATH);
    platform_create_directory(ENGINE_TEXTURE_PATH);



    FILE* fptr = fopen(ASSET_REGISTRY_BIN_PATH, "rb");




    return true;
}

void asset_registry2_shutdown(Asset_Registry* asset_registry)
{
}


void asset_registry2_get_metadata()
{
}

void asset_registry2_hot_reload_asset()
{
}


#endif
