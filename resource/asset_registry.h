#ifndef ASSET_REGISTRY_H
#define ASSET_REGISTRY_H

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


bool asset_registry_init(Asset_System* asset_system, Asset_Registry* asset_registry, Heap_Allocator* allocator,
                         Memory_System* memory_system);

void asset_registry_shutdown(Asset_Registry* asset_registry);

void asset_registry_scan_for_new_assets(Asset_System* asset_system, Asset_Registry* asset_registry,
                                        Memory_System* memory_system, Asset_Type asset_type);

//
void asset_registry_append_to_file(Asset_Registry* asset_registry, Asset_MetaData* asset_meta_data);

void asset_registry_add_asset_from_uuid(Asset_Registry* asset_registry,
                                        const char* source_path,
                                        const char* engine_path,
                                        Asset_Type asset_type, Heap_Allocator* allocator,
                                        MADNESS_UUID uuid);
void asset_registry_add_asset_and_generated_uuid(Asset_Registry* asset_registry, const char* source_path,
                                                 const char* engine_path,
                                                 Asset_Type asset_type, Heap_Allocator* allocator,
                                                 MADNESS_UUID* out_uuid);

bool asset_registry_overwrite_file(Asset_Registry* asset_registry);

void asset_registry_remove(Asset_Registry* asset_registry);


//functions to find the asset and return the metadata associated with it
bool asset_registry_exists_by_engine_path(Asset_Registry* asset_registry, String* engine_path,
                                          Asset_MetaData* out_meta_data);

bool asset_registry_exists_by_source_path(Asset_Registry* asset_registry, String* source_path,
                                          Asset_MetaData* out_meta_data);

bool asset_registry_exists_by_uuid(Asset_Registry* asset_registry, MADNESS_UUID uuid,
                                   Asset_MetaData* out_meta_data);


Dynamic_Array* asset_registry_get_all_assets_of_type(Asset_Registry* asset_registry, Asset_Type type, Allocator* allocator)
{
    Dynamic_Array* array = dynamic_array_create_allocator(Asset_MetaData*, 128, allocator)
    for (u32 i = 0; i < asset_registry->asset_meta_data->num_items; i++)
    {
        Asset_MetaData* asset_meta_data = _dynamic_array_get(asset_registry->asset_meta_data, i);
        if (asset_meta_data->type == type)
        {
            dynamic_array_push(array, &asset_meta_data);
        }

    }
    return array;
}


#endif
