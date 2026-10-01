#ifndef ASSET_REGISTRY2_H
#define ASSET_REGISTRY2_H

#include "resource_types.h"

//AR = madness asset registry
// #define ASSET_REGISTRY_BIN_PATH "../z_assets_engine/MAR.bin"
// #define ASSET_REGISTRY_MAGIC_NUMBER "MARS"


typedef struct Asset_Registry_Header
{
    u8 magic[4];
    u32 version;
    u64 asset_count;
} Asset_Registry_Header;

typedef struct Texture_MetaData
{
    //
    u32 width;
    u32 height;
    u8 channels;
    Texture_Format format;
    u64 pixels_size;
    Asset_Type type; // used to identify if we have are a font
    // runtime only data
    u32 font_index;
    // what is queried when we get the bindless slot, so that we can use a temp texture until the actual texture loads
    u32 generation;

    Asset_Load_State texture_load_state;
    //TODO:
    //bool has_sampler;
    // Texture_Sampler sampler;

} Texture_MetaData;

typedef struct Mesh_MetaData
{
    //
    u32 mesh_count;
    String* mesh_names;
    MADNESS_UUID* material_uuids;

    vec3s initial_pos;
    vec3s initial_rotation;
    vec3s initial_scale;
} Mesh_MetaData;


typedef struct Particle_MetaData
{
    //


    u32 material_uuids;

} Particle_MetaData;


bool asset_registry_init(Asset_System* asset_system, Asset_Registry* asset_registry, Heap_Allocator* allocator,
                         Memory_System* memory_system);

void asset_registry_shutdown(Asset_Registry* asset_registry);


void ar_get_metadata();
void ar_hot_reload_asset();



#endif
