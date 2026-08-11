#ifndef ASSET_REGISTRY_H
#define ASSET_REGISTRY_H

#include "asserts.h"
#include "resource_types.h"

//AR = madness asset registry
#define ASSET_REGISTRY_BIN_PATH "../z_assets_engine/MAR.bin"
#define ASSET_REGISTRY_MAGIC_NUMBER "MARS"


typedef struct Asset_Registry_Header
{
    u8 magic[4];
    u32 version;
    u64 asset_count;
} Asset_Registry_Header;


bool asset_registry_init(Asset_Registry* asset_registry, Heap_Allocator* allocator, Memory_System* memory_system);

void asset_registry_shutdown(Asset_Registry* asset_registry);


void asset_registry_append_to_file(Asset_Registry* asset_registry, Asset_MetaData* asset_meta_data);

void asset_registry_add_asset(Asset_Registry* asset_registry, const char* source_path,
                              const char* engine_path,
                              Asset_Type asset_type, Heap_Allocator* allocator, MADNESS_UUID* out_uuid);

bool asset_registry_overwrite_file(Asset_Registry* asset_registry);

void asset_registry_remove(Asset_Registry* asset_registry);


//functions to find the asset and return the metadata associated with it
bool asset_registry_exists_by_engine_path(Asset_Registry* asset_registry, String* engine_path,
                                          Asset_MetaData* out_meta_data);

bool asset_registry_exists_by_source_path(Asset_Registry* asset_registry, String* source_path,
                                          Asset_MetaData* out_meta_data);

bool asset_registry_exists_by_uuid(Asset_Registry* asset_registry, MADNESS_UUID uuid,
                                   Asset_MetaData* out_meta_data);





#endif
