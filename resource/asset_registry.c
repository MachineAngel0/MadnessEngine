#include "asset_registry.h"
#include "asserts.h"
#include "asset_converter.h"


bool asset_registry_init(Asset_System* asset_system, Asset_Registry* asset_registry, Heap_Allocator* allocator,
                         Memory_System* memory_system)
{

    asset_registry->asset_record_allocator = memory_system_allocator_create(
        memory_system, MB(16), MEMORY_SUBSYSTEM_RESOURCE, "asset record");


    //make sure all base paths exist
    platform_create_directory(ENGINE_AUDIO_PATH);
    platform_create_directory(ENGINE_FONTS_PATH);
    platform_create_directory(ENGINE_SHADER_PATH);
    platform_create_directory(ENGINE_MATERIAL_PATH);
    platform_create_directory(ENGINE_MESH_PATH);
    platform_create_directory(ENGINE_PARTICLE_PATH);
    platform_create_directory(ENGINE_PARTICLE_EFFECT_PATH);
    platform_create_directory(ENGINE_PARTICLE_EMITTER_PATH);
    platform_create_directory(ENGINE_SCENE_PATH);
    platform_create_directory(ENGINE_SK_MESH_PATH);
    platform_create_directory(ENGINE_TEXTURE_PATH);



    FILE* fptr = fopen(ASSET_REGISTRY_BIN_PATH, "rb");

    if (!fptr)
    {
        asset_registry->asset_record = dynamic_array_create_heap(Asset_Record, 1024, allocator);
        FILE* temp_file = fopen(ASSET_REGISTRY_BIN_PATH, "wb");
        Asset_Registry_Header header = {
            .magic = ASSET_REGISTRY_MAGIC_NUMBER,
            .version = ASSET_REGISTRY_VERSION,
            .asset_count = asset_registry->asset_record->num_items,
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


        asset_registry->asset_record = dynamic_array_create_allocator(Asset_Record, header.asset_count, asset_registry->asset_record_allocator);
        asset_registry->asset_record->num_items = header.asset_count;
        fseek(fptr, 0, SEEK_SET);

        for (u64 asset_idx = 0; asset_idx < asset_registry->asset_record->num_items; asset_idx++)
        {
            Asset_Record* asset = &dynamic_array_get(asset_registry->asset_record,
                                                       asset_idx, Asset_Record);
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


    asset_registry->texture_table_allocator = memory_system_allocator_create(memory_system, MB(8), MEMORY_SUBSYSTEM_RESOURCE, "texture meta table");
    asset_registry->texture_table = dynamic_array_create_allocator(Texture_MetaData, 1024, asset_registry->texture_table_allocator);


    return true;
}

void asset_registry_scan_for_new_assets(Asset_System* asset_system, Asset_Registry* asset_registry,
                                        Memory_System* memory_system, Asset_Type asset_type)
{
    //loop throught all files in the import path, and check to see if any new assets have been added

    const char* asset_path;
    switch (asset_type)
    {
    case ASSET_TEXTURE:
        asset_path = IMPORT_TEXTURE_PATH;
        break;
    case ASSET_FONT:
        asset_path = IMPORT_FONTS_PATH;
        break;
    case ASSET_SPRITE:
        break;
    case ASSET_STATIC_MESH:
        asset_path = IMPORT_MESH_PATH;
        break;
    case ASSET_SKINNED_MESH:
        break;
    case ASSET_AUDIO:
        break;
    case ASSET_SHADER:
        break;
    case ASSET_MATERIAL:
        break;
    case ASSET_SCENE:
        break;
    case ASSET_TYPE_MAX:
        break;
    }

    Asset_List_Scan* list_scan = asset_lists_generate(memory_system, MAX_ASSETS_STRINGS, asset_path);
    for (u32 i = 0; i < list_scan->count; i++)
    {
        Scratch_Allocator scratch = scratch_allocator_begin(asset_system->scratch_allocator);
        const char* file_path = string_to_c_string_allocator(&list_scan->strings[i], scratch.allocator);

        if (asset_registry_exists_by_source_path(asset_registry, &list_scan->strings[i], asset_type, NULL))
        {
            scratch_allocator_end(scratch);
            continue;
        }

        switch (asset_type)
        {
        case ASSET_TEXTURE:
            MADNESS_UUID uuid = {0};
            asset_converter_texture(asset_system, file_path, &uuid);
            break;
        case ASSET_FONT:
            asset_converter_msdf_font(asset_system, file_path);
            break;
        case ASSET_SPRITE:
            break;
        case ASSET_STATIC_MESH:
            asset_converter_mesh(asset_system, file_path);
            break;
        case ASSET_SKINNED_MESH:
            asset_converter_mesh(asset_system, file_path);
            break;
        case ASSET_AUDIO:
            break;
        case ASSET_SHADER:
            break;
        case ASSET_MATERIAL:
            break;
        case ASSET_SCENE:
            break;
        case ASSET_TYPE_MAX:
            break;
        case ASSET_PARTICLE_EFFECT:
            break;
        case ASSET_PARTICLE_EMITTER:
            break;
        }


        scratch_allocator_end(scratch);
    }
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


    for (u64 asset_idx = 0; asset_idx < asset_registry->asset_record->num_items; asset_idx++)
    {
        Asset_Record* asset = &dynamic_array_get(asset_registry->asset_record,
                                                   asset_idx, Asset_Record);

        fwrite(&asset->uuid, sizeof(asset->uuid), 1, fptr);
        fwrite(&asset->hash, sizeof(asset->hash), 1, fptr);
        fwrite(&asset->type, sizeof(asset->type), 1, fptr);
        string_serialize(asset->source_file, fptr);
        string_serialize(asset->engine_path, fptr);
    }


    Asset_Registry_Header header = {
        .magic = ASSET_REGISTRY_MAGIC_NUMBER,
        .version = ASSET_REGISTRY_VERSION,
        .asset_count = asset_registry->asset_record->num_items,
    };
    fwrite(&header, sizeof(Asset_Registry_Header), 1, fptr);

    fclose(fptr);

    return true;
}


void asset_registry_append_to_file(Asset_Registry* asset_registry, Asset_Record* asset_meta_data)
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
        .asset_count = asset_registry->asset_record->num_items,
    };
    fwrite(&header, sizeof(Asset_Registry_Header), 1, fptr);
}


void asset_registry_add_asset_from_uuid(Asset_Registry* asset_registry,
                                                          const char* source_path,
                                                          const char* engine_path,
                                                          Asset_Type asset_type, Heap_Allocator* allocator,
                                                          MADNESS_UUID uuid)
{
    //find if the asset already exists
    String* str_source_file = STRING_CREATE_FROM_BUFFER_HEAP_ALLOCATOR(source_path, allocator);
    String* str_engine_path = STRING_CREATE_FROM_BUFFER_HEAP_ALLOCATOR(engine_path, allocator);

    Asset_Record meta_data = {0};
    bool exists_by_source_path = asset_registry_exists_by_source_path(asset_registry, str_source_file, asset_type, &meta_data);
    bool exists_by_engine_path = asset_registry_exists_by_engine_path(asset_registry, str_engine_path, &meta_data);
    if (exists_by_source_path  || exists_by_engine_path)
    {
        //update these just in case
        if (!string_compare(meta_data.source_file, str_source_file))
        {
            M_ERROR("CHANGING ASSET SOURCE FILE")
            string_print(meta_data.source_file);
            string_print(str_source_file);
        }
        if (!string_compare(meta_data.engine_path, str_engine_path))
        {
            M_ERROR("CHANGING ASSET ENGINE FILE")
            string_print(meta_data.engine_path);
            string_print(str_engine_path);
        }
    }
    else
    {
        meta_data.source_file = str_source_file;
        meta_data.engine_path = str_engine_path;
        meta_data.uuid = uuid;
        meta_data.hash = madness_uuid_hash(&meta_data.uuid);
        meta_data.type = asset_type;

        dynamic_array_push(asset_registry->asset_record, &meta_data);
        // asset_registry_append_to_file(asset_registry);
    }

    asset_registry_overwrite_file(asset_registry);
}


void asset_registry_add_asset_and_generate_uuid(Asset_Registry* asset_registry, const char* source_path,
                              const char* engine_path,
                              Asset_Type asset_type, Heap_Allocator* allocator, MADNESS_UUID* out_uuid)
{
    MADNESS_UUID uuid = madness_uuid_generate_return();

    if (out_uuid)
    {
        *out_uuid = uuid;
    }

    asset_registry_add_asset_from_uuid(asset_registry,
                                                         source_path,
                                                         engine_path,
                                                         asset_type, allocator,
                                                         uuid);
}



bool asset_registry_exists_by_source_path(Asset_Registry* asset_registry, String* source_path,
                                          Asset_Type asset_type, Asset_Record* out_record_data)
{
    for (u64 i = 0; i < asset_registry->asset_record->num_items; i++)
    {

        Asset_Record* meta_data = _dynamic_array_get(asset_registry->asset_record, i);
        if (meta_data->type != asset_type) {continue;};

        if (string_compare(meta_data->source_file, source_path))
        {
            //found
            if (out_record_data)
            {
                *out_record_data = *meta_data;
            }
            return true;
        }
    }
    return false;
}

bool asset_registry_exists_by_engine_path(Asset_Registry* asset_registry, String* engine_path,
                                          Asset_Record* out_record_data)
{
    for (u64 i = 0; i < asset_registry->asset_record->num_items; i++)
    {
        Asset_Record* meta_data = _dynamic_array_get(asset_registry->asset_record, i);
        if (string_compare(meta_data->engine_path, engine_path))
        {
            //found
            *out_record_data = *meta_data;
            return true;
        }
    }
    return false;
}


bool asset_registry_exists_by_uuid(Asset_Registry* asset_registry, MADNESS_UUID uuid, Asset_Record* out_record_data)
{

    for (u64 i = 0; i < asset_registry->asset_record->num_items; i++)
    {
        Asset_Record* meta_data = _dynamic_array_get(asset_registry->asset_record, i);
        if (madness_uuid_compare(meta_data->uuid, uuid))
        {
            *out_record_data = *meta_data;
            return true;
        }
    }
    return false;
}

Dynamic_Array* asset_registry_get_all_assets_of_type(Asset_Registry* asset_registry, Asset_Type type,
    Allocator* allocator)
{
    Dynamic_Array* array = dynamic_array_create_allocator(Asset_Record*, 128, allocator)
    for (u32 i = 0; i < asset_registry->asset_record->num_items; i++)
    {
        Asset_Record* asset_meta_data = _dynamic_array_get(asset_registry->asset_record, i);
        if (asset_meta_data->type == type)
        {
            dynamic_array_push(array, &asset_meta_data);
        }

    }
    return array;
}
