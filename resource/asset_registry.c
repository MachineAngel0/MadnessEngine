#include "asset_registry.h"



bool asset_registry_init(Asset_Registry* asset_registry, Heap_Allocator* allocator, Memory_System* memory_system)
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
            asset->engine_path = allocator_heap_alloc(allocator, sizeof(String));

            fread(&asset->uuid, sizeof(asset->uuid), 1, fptr);
            fread(&asset->hash, sizeof(asset->hash), 1, fptr);
            fread(&asset->type, sizeof(asset->type), 1, fptr);
            string_deserialize_heap(asset->source_file, fptr, allocator);
            string_deserialize_heap(asset->engine_path, fptr, allocator);
        }

        fclose(fptr);
    }


    return true;
}

void asset_registry_shutdown(Asset_Registry* asset_registry)
{
    asset_registry_overwrite_file(asset_registry);
}

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
        string_serialize(asset->engine_path, fptr);
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


void asset_registry_append_to_file(Asset_Registry* asset_registry, Asset_MetaData* asset_meta_data)
{
    // ab = append binary
    FILE* fptr = fopen(ASSET_REGISTRY_BIN_PATH, "ab");
    fseek(fptr, -(sizeof(Asset_Registry_Header)), SEEK_END);
    fwrite(&asset_meta_data->uuid, sizeof(asset_meta_data->uuid), 1, fptr);
    fwrite(&asset_meta_data->hash, sizeof(asset_meta_data->hash), 1, fptr);
    fwrite(&asset_meta_data->type, sizeof(asset_meta_data->type), 1, fptr);
    string_serialize(asset_meta_data->source_file, fptr);
    string_serialize(asset_meta_data->engine_path, fptr);


    Asset_Registry_Header header = {
        .magic = ASSET_REGISTRY_MAGIC_NUMBER,
        .version = 1,
        .asset_count = asset_registry->asset_meta_data->num_items,
    };
    fwrite(&header, sizeof(Asset_Registry_Header), 1, fptr);
}

void asset_registry_add_asset(Asset_Registry* asset_registry, const char* source_path,
                              const char* engine_path,
                              Asset_Type asset_type, Heap_Allocator* allocator, MADNESS_UUID* out_uuid)
{
    //find if the asset already exists
    String* str_source_file = STRING_CREATE_FROM_BUFFER_HEAP_ALLOCATOR(source_path, allocator);
    String* str_engine_path = STRING_CREATE_FROM_BUFFER_HEAP_ALLOCATOR(engine_path, allocator);

    Asset_MetaData meta_data = {0};
    if (asset_registry_exists_by_source_path(asset_registry, str_source_file, &meta_data) ||
        asset_registry_exists_by_engine_path(asset_registry, str_engine_path, &meta_data))
    {
        //update these just in case
        meta_data.source_file = str_source_file;
        meta_data.engine_path = str_engine_path;
    }
    else
    {
        meta_data.source_file = str_source_file;
        meta_data.engine_path = str_engine_path;
        meta_data.uuid = madness_uuid_generate_return();
        meta_data.hash = madness_uuid_hash(&meta_data.uuid);
        meta_data.type = asset_type;

        dynamic_array_push(asset_registry->asset_meta_data, &meta_data);
        // asset_registry_append_to_file(asset_registry);
    }

    asset_registry_overwrite_file(asset_registry);

    if (out_uuid)
    {
        *out_uuid = meta_data.uuid;
    }
}

void asset_registry_remove(Asset_Registry* asset_registry)
{
    //TODO: just do a remove swap
}


bool asset_registry_exists_by_source_path(Asset_Registry* asset_registry, String* source_path,
                                          Asset_MetaData* out_meta_data)
{
    for (u64 i = 0; i < asset_registry->asset_meta_data->num_items; i++)
    {
        Asset_MetaData* meta_data = _dynamic_array_get(asset_registry->asset_meta_data, i);
        if (string_compare(meta_data->source_file, source_path))
        {
            //found
            *out_meta_data = *meta_data;
            return true;
        }
    }
    return false;
}

bool asset_registry_exists_by_engine_path(Asset_Registry* asset_registry, String* engine_path,
                                          Asset_MetaData* out_meta_data)
{
    for (u64 i = 0; i < asset_registry->asset_meta_data->num_items; i++)
    {
        Asset_MetaData* meta_data = _dynamic_array_get(asset_registry->asset_meta_data, i);
        if (string_compare(meta_data->engine_path, engine_path))
        {
            //found
            *out_meta_data = *meta_data;
            return true;
        }
    }
    return false;
}


bool asset_registry_exists_by_uuid(Asset_Registry* asset_registry, MADNESS_UUID uuid, Asset_MetaData* out_meta_data)
{
    //pass in a frame allocator
    for (u64 i = 0; i < asset_registry->asset_meta_data->num_items; i++)
    {
        Asset_MetaData* meta_data = _dynamic_array_get(asset_registry->asset_meta_data, i);
        if (madness_uuid_compare(meta_data->uuid, uuid))
        {
            *out_meta_data = *meta_data;
            return true;
        }
    }
    return false;
}
