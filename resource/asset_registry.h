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


bool asset_registry_overwrite_file(Asset_Registry* asset_registry)
{
    FILE* fptr = fopen(ASSET_REGISTRY_BIN_PATH, "wb");
    if (!fptr)
    {
        MASSERT(false);
        return false;
    }


    for (u64 asset_idx = 0; asset_idx < asset_registry->asset_meta_data->num_items; asset_idx++)
    {
        Asset_MetaData* asset = dynamic_array_get_ptr(asset_registry->asset_meta_data, Asset_MetaData,
                                                      asset_idx);

        fwrite(&asset->uuid, sizeof(asset->uuid), 1, fptr);
        fwrite(&asset->hash, sizeof(asset->hash), 1, fptr);
        fwrite(&asset->type, sizeof(asset->type), 1, fptr);
        string_serialize(asset->source_file, fptr);
        string_serialize(asset->binary_file, fptr);
    }


    Asset_Registry_Header header = {
        .magic = ASSET_REGISTRY_MAGIC_NUMBER,
        .version = 1,
        .asset_count = asset_registry->asset_meta_data->num_items,
    };
    fwrite(&header, sizeof(Asset_Registry_Header), 1, fptr);

    fclose(fptr);

    return true;
}


bool asset_registry_init(Asset_Registry* asset_registry, Heap_Allocator* allocator)
{
    FILE* fptr = fopen(ASSET_REGISTRY_BIN_PATH, "rb");

    if (!fptr)
    {
        asset_registry->asset_meta_data = dynamic_array_create(Asset_MetaData, 1024, allocator);
        FILE* temp_file = fopen(ASSET_REGISTRY_BIN_PATH, "wb");
        Asset_Registry_Header header = {
            .magic = ASSET_REGISTRY_MAGIC_NUMBER,
            .version = 1,
            .asset_count = asset_registry->asset_meta_data->num_items,
        };
        fwrite(&header, sizeof(Asset_Registry_Header), 1, temp_file);
        fclose(temp_file);
    }
    else
    {
        //you have to read backwards
        fseek(fptr, -(sizeof(Asset_Registry_Header)), SEEK_END);
        Asset_Registry_Header header = {0};
        fread(&header, sizeof(Asset_Registry_Header), 1, fptr);

        MASSERT(header.magic[0] == 'M')
        MASSERT(header.magic[1] == 'A')
        MASSERT(header.magic[2] == 'R')
        MASSERT(header.magic[3] == 'S')


        if (header.asset_count < 1024)
        {
            asset_registry->asset_meta_data = dynamic_array_create(Asset_MetaData, 1024, allocator);
        }
        else
        {
            asset_registry->asset_meta_data = dynamic_array_create(Asset_MetaData, header.asset_count, allocator);
        }
        asset_registry->asset_meta_data->num_items = header.asset_count;
        fseek(fptr, 0, SEEK_SET);

        for (u64 asset_idx = 0; asset_idx < asset_registry->asset_meta_data->num_items; asset_idx++)
        {
            Asset_MetaData* asset = dynamic_array_get_ptr(asset_registry->asset_meta_data, Asset_MetaData,
                                                          asset_idx);
            asset->source_file = allocator_heap_alloc(allocator, sizeof(String));
            asset->binary_file = allocator_heap_alloc(allocator, sizeof(String));

            fread(&asset->uuid, sizeof(asset->uuid), 1, fptr);
            fread(&asset->hash, sizeof(asset->hash), 1, fptr);
            fread(&asset->type, sizeof(asset->type), 1, fptr);
            string_deserialize_heap(asset->source_file, fptr, allocator);
            string_deserialize_heap(asset->binary_file, fptr, allocator);
        }

        fclose(fptr);
    }

    return true;
}

void asset_registry_shutdown(Asset_Registry* asset_registry)
{
    asset_registry_overwrite_file(asset_registry);
}


void asset_registry_append_to_file(Asset_Registry* asset_registry, Asset_MetaData* asset_meta_data)
{
    // ab = append binary
    FILE* fptr = fopen(ASSET_REGISTRY_BIN_PATH, "ab");
    fseek(fptr, -(sizeof(Asset_Registry_Header)), SEEK_END);
    fwrite(&asset_meta_data->uuid, sizeof(asset_meta_data->uuid), 1, fptr);
    fwrite(&asset_meta_data->hash, sizeof(asset_meta_data->hash), 1, fptr);
    fwrite(&asset_meta_data->type, sizeof(asset_meta_data->type), 1, fptr);
    string_serialize(asset_meta_data->source_file, fptr);
    string_serialize(asset_meta_data->binary_file, fptr);


    Asset_Registry_Header header = {
        .magic = ASSET_REGISTRY_MAGIC_NUMBER,
        .version = 1,
        .asset_count = asset_registry->asset_meta_data->num_items,
    };
    fwrite(&header, sizeof(Asset_Registry_Header), 1, fptr);
}

void asset_registry_add_asset(Asset_Registry* asset_registry, Asset_MetaData* meta_data)
{
    dynamic_array_push(asset_registry->asset_meta_data, meta_data);
    asset_registry_overwrite_file(asset_registry);
    // asset_registry_append_to_file(asset_registry);
}

void asset_registry_remove(Asset_Registry* asset_registry)
{
    //TODO: just do a remove swap
}


bool asset_registry_exists_by_source_path(Asset_System* asset_system, String* source_path, MADNESS_UUID* out_uuid)
{
    for (u64 i = 0; i < asset_system->asset_registry->asset_meta_data->num_items; i++)
    {
        Asset_MetaData* meta_data = _dynamic_array_get(asset_system->asset_registry->asset_meta_data, i);
        if (string_compare(meta_data->source_file, source_path))
        {
            //found
            *out_uuid = meta_data->uuid;
            return true;
        }
    }
    return false;
}

bool asset_registry_exists_by_engine_path(Asset_System* asset_system, String* engine_path, MADNESS_UUID* out_uuid,
                                          u64* out_hash)
{
    for (u64 i = 0; i < asset_system->asset_registry->asset_meta_data->num_items; i++)
    {
        Asset_MetaData* meta_data = _dynamic_array_get(asset_system->asset_registry->asset_meta_data, i);
        if (string_compare(meta_data->binary_file, engine_path))
        {
            //found
            *out_uuid = meta_data->uuid;
            *out_hash = meta_data->hash;
            return true;
        }
    }
    return false;
}


bool asset_registry_get_path_from_uuid(Asset_System* asset_system, MADNESS_UUID uuid, String* out_string,
                                       Allocator* allocator)
{
    //pass in a frame allocator
    for (u64 i = 0; i < asset_system->asset_registry->asset_meta_data->num_items; i++)
    {
        Asset_MetaData* meta_data = _dynamic_array_get(asset_system->asset_registry->asset_meta_data, i);
        if (madness_uuid_compare(meta_data->uuid, uuid))
        {
            out_string = string_duplicate_alloc(meta_data->binary_file, allocator);
            return true;
        }
    }
    return false;
}


#endif
