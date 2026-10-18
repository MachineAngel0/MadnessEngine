#ifndef MATERIAL_SYSTEM_H
#define MATERIAL_SYSTEM_H


#include "asset_system.h"
#include "resource_types.h"
#include "shader_system.h"


//ideal api
void material_upload_data(Material_System2* material_system, void* data, u64 data_size,
                          Material_Record_Handle* out_handle);

void material_system_init(Material_System2* material_system, Memory_System* memory_system)
{
    memset(material_system->materials, 0, sizeof(Madness_Material) * MAX_MATERIAL_COUNT);
    material_system->material_count = 0;


    u64 material_memory = MB(8);
    material_system->material_allocator = memory_system_allocator_create(
        memory_system, material_memory, MEMORY_SUBSYSTEM_MATERIAL, "material");

    material_system->material_buffer = allocator_alloc(material_system->material_allocator, MATERIAL_BUFFER_SIZE);
    material_system->material_buffer_offset = 0;
    material_system->material_buffer_max_byte_size = MATERIAL_BUFFER_SIZE;


    memset(material_system->material_record, 0, sizeof(Material_Record) * MAX_MATERIAL_COUNT);
    material_system->default_handle = (Material_Handle){0, 0};
    memset(material_system->default_data_blob, 0, MATERIAL_FALLBACK_BLOB_SIZE);
}


void material_system_deinit(Material_System2* material_system2, Memory_System* memory_system)
{
    return;
}

void material_system_update()
{
    //TODO: defrag the material buffer
}

void material_system_render_packet(Material_System2* material_system, Render_Packet* render_packet)
{
    render_packet->material_packet.material_buffer = material_system->material_buffer;
    render_packet->material_packet.material_buffer_byte_size = material_system->material_buffer_offset;
}


bool material_asset_serialize(Material_Asset* material, FILE* fptr)
{
    madness_uuid_serialize(material->shader_uuid, fptr);
    madness_uuid_serialize(material->material_uuid, fptr);
    fwrite(&material->reflection_hash, sizeof(material->reflection_hash), 1, fptr);
    string_serialize(material->material_name, fptr);
    string_serialize(material->name, fptr);

    fwrite(&material->data_size, sizeof(material->data_size), 1, fptr);
    fwrite(material->data, material->data_size, 1, fptr);
    return true;
}

bool material_asset_deserialize(Material_Asset* material, FILE* fptr, Allocator* allocator)
{
    material->name = allocator_alloc(allocator, sizeof(String));
    material->material_name = allocator_alloc(allocator, sizeof(String));

    madness_uuid_deserialize(&material->shader_uuid, fptr);
    madness_uuid_deserialize(&material->material_uuid, fptr);

    fread(&material->reflection_hash, sizeof(material->reflection_hash), 1, fptr);

    string_deserialize(material->material_name, fptr, allocator);
    string_deserialize(material->name, fptr, allocator);

    fread(&material->data_size, sizeof(material->data_size), 1, fptr);

    material->data = allocator_alloc(allocator, material->data_size);
    fread(material->data, material->data_size, 1, fptr);
    return true;
}


String_Builder* material_asset_construct_path(Material_Asset* material_asset, Allocator* allocator)
{
    //write out the file
    String_Builder* str_builder = string_builder_create(256, allocator);
    string_builder_append_c_string(str_builder, ENGINE_MATERIAL_PATH);
    string_builder_append_string(str_builder, material_asset->material_name);
    string_builder_append_c_string(str_builder, "_");
    string_builder_append_string(str_builder, material_asset->name);
    string_builder_append_c_string(str_builder, ENGINE_MATERIAL_EXTENSION);

    return str_builder;
}


/**
 * @brief: sets any uuids in the materials to the default texture uuid
 */
void material_asset_set_default_textures(Asset_System* asset_system,
                                         Material_Asset* material,
                                         Material_Definition* material_definition)
{
    MASSERT(asset_system);
    MASSERT(material);
    MASSERT(material->material_name);
    MASSERT(material->name);
    MASSERT(material->data);
    MASSERT(material_definition->reflection_material_data.name);


    for (u32 i = 0; i < material_definition->reflection_material_data.field_count; i++)
    {
        if (material_definition->reflection_material_data.fields[i].type == REFLECTION_TYPE_UUID)
        {
            MADNESS_UUID* uuid_data = (MADNESS_UUID*)((u8*)material->data + material_definition->
                                                                            reflection_material_data.fields[i].offset);
            *uuid_data = asset_system->texture_system->default_texture_uuid;
        }
    }
}


void material_create_or_get(Asset_System* asset_system, Shader_Handle shader_handle,
                            const char* name, Material_Handle* out_handle)
{
    //TODO: check that this material, with the same material name and user defined name has not been create already, if it has just load that and return the handle

    //get the shader
    Shader_Asset* shader_asset = shader_asset_get(asset_system->shader_system, shader_handle);

    Material_Definition material_definition;
    shader_mapping_get_definition_from_shader_asset(asset_system->shader_system, shader_asset, &material_definition);
    //I guess we would want to create both the material asset and the runtime material

    // create the asset
    Material_Asset material_asset = {
        .shader_uuid = shader_asset->uuid,
        .material_uuid = madness_uuid_generate_return(),
        .reflection_hash = material_definition.runtime_reflection_hash,
        .material_name = STRING_CREATE_FROM_BUFFER_ALLOCATOR(material_definition.reflection_material_data.name,
                                                                asset_system->frame_allocator),
        .name = STRING_CREATE_FROM_BUFFER_ALLOCATOR(name, asset_system->frame_allocator),
        .data_size = material_definition.reflection_material_data.struct_size,
        .data = allocator_alloc(asset_system->material_system->material_allocator,
                                material_definition.reflection_material_data.struct_size),
    };

    String_Builder* str_builder =
        material_asset_construct_path(&material_asset, asset_system->frame_allocator);
    const char* path = string_builder_to_c_string(str_builder);
    //if it exists load it in, otherwise create it
    if (filesystem_does_file_exists(path))
    {
        asset_load_material_path(asset_system, path, out_handle);
    }
    else
    {

        //set default cpu data
        material_asset_set_default_textures(asset_system,
                                            &material_asset,
                                            &material_definition);
        // save it
        asset_converter_material(asset_system,
                                 &material_asset);


        //load it in
        asset_load_material_uuid(asset_system, material_asset.material_uuid, out_handle);
    }



}


//it expects a cpu based material
void material_create_from_data(Asset_System* asset_system, Shader_Handle shader_handle,
                               const char* name, void* data, u64 data_size, Material_Handle* out_handle)
{
    //TODO: check that this material, with the same name has not been create already


    //get the shader
    Shader_Asset* shader_asset = shader_asset_get(asset_system->shader_system, shader_handle);


    Material_Definition material_definition;
    shader_mapping_get_definition_from_shader_asset(asset_system->shader_system, shader_asset, &material_definition);
    //I guess we would want to create both the material asset and the runtime material

    MASSERT(data_size == material_definition.reflection_material_data.struct_size);


    Material_Asset material_asset = {
        .shader_uuid = shader_asset->uuid,
        .material_uuid = madness_uuid_generate_return(),
        .reflection_hash = material_definition.runtime_reflection_hash,
        .material_name = STRING_CREATE_FROM_BUFFER_ALLOCATOR(material_definition.reflection_material_data.name,
                                                             asset_system->material_system->material_allocator),
        .name = STRING_CREATE_FROM_BUFFER_ALLOCATOR(name, asset_system->material_system->material_allocator),
        .data_size = material_definition.reflection_material_data.struct_size,
        .data = data,
    };


    String_Builder* material_path = material_asset_construct_path(&material_asset, asset_system->frame_allocator);
    const char* path = string_builder_to_c_string(material_path);
    //if it exists load it in, otherwise create it
    if (filesystem_does_file_exists(path))
    {
        asset_load_material_path(asset_system, path, out_handle);
    }
    else
    {
        material_asset_set_default_textures(asset_system,
                                            &material_asset,
                                            &material_definition);

        asset_converter_material(asset_system,
                                 &material_asset);

        asset_load_material_uuid(asset_system, material_asset.material_uuid, out_handle);
    }
}

bool material_convert_and_upload_gpu_data(Asset_System* asset_system, Material_Asset* material_asset,
                                          Madness_Material* material,
                                          Material_Definition* material_definition,
                                          Scratch_Allocator* scratch_allocator)
{
    Material_System2* material_system = asset_system->material_system;

    //NOTE: as an optimization, instead of allocating, we just get the back of the material buffer modify the data there

    u32 data_size = material_definition->material_gpu_definition.struct_size;
    void* gpu_data = allocator_alloc(scratch_allocator->allocator, data_size);


    //resolve any uuid for texture loading
    for (u32 i = 0; i < material_definition->reflection_material_data.field_count; i++)
    {
        Reflection_Runtime_Struct_Field* field = &material_definition->reflection_material_data.fields[i];

        if (field->type == REFLECTION_TYPE_UUID)
        {
            MASSERT(material_definition->material_gpu_definition.types[i] == REFLECTION_TYPE_U32);

            //load in texture and use the bindless index
            MADNESS_UUID uuid_data = *(MADNESS_UUID*)((u8*)material_asset->data + field->offset);

            //sometimes we dont have a texture so we just set the value to the default texture
            if (uuid_data.high == 0 && uuid_data.low == 0)
            {
                uuid_data = texture_system_get_default_uuid(asset_system);
            }
            Texture_Handle texture_handle = asset_load_texture_uuid(asset_system, uuid_data);

            //copy into the gpu struct
            memcpy(
                ((u8*)gpu_data + material_definition->material_gpu_definition.field_offsets[
                    i]),
                &texture_handle.handle,
                sizeof(u32));
        }
        else if (material_definition->material_gpu_definition.types[i] == field->type)
        {
            //copy into the gpu struct
            memcpy((u8*)gpu_data + material_definition->material_gpu_definition.field_offsets[i],
                   (u8*)material_asset->data + field->offset,
                   reflection_type_get_size(material_definition->material_gpu_definition.types[i]));
        }
        else
        {
            MASSERT(false); //this should never happen
        }
    }

    material_upload_data(material_system, gpu_data, data_size, &material->material_record_handle);

    return true;
}

bool material_acquire(Asset_System* asset_system, Material_System2* material_system, FILE* fptr,
                      Asset_Record* asset_record,
                      Material_Handle* out_handle)
{
    //deserialize the data
    //check to see if the shader is loaded
    Material_Asset* material_asset = &material_system->material_assets[material_system->material_count];
    material_asset_deserialize(material_asset, fptr, material_system->material_allocator);

    //set material data from the file
    Madness_Material* material = &material_system->materials[material_system->material_count];
    material->material_name = material_asset->material_name;
    material->name = material_asset->name;
    material->material_uuid = material_asset->material_uuid;

    //get the shader handle and set the shader key
    asset_load_shader_asset_uuid(asset_system, material_asset->shader_uuid, &material->shader_handle);
    Shader_Asset* asset = shader_asset_get(asset_system->shader_system, material->shader_handle);

    Material_Definition definition;
    shader_mapping_get_definition_from_shader_asset(asset_system->shader_system, asset, &definition);


    //TODO: chech the hash and change the data to ensure its valid
    if (definition.runtime_reflection_hash != material_asset->reflection_hash)
    {
        MASSERT(false);
    }

    //convert to gpu format and upload the data, and get material gets a handle back to the asset record
    Scratch_Allocator scratch = scratch_allocator_begin(asset_system->allocator);
    material_convert_and_upload_gpu_data(asset_system, material_asset, material,
                                         &definition,
                                         &scratch);
    scratch_allocator_end(scratch);


    //NOTE: the only we would want to check is an asset reloaded
    Asset_Runtime* runtime = &material_system->runtime_asset[material_system->runtime_asset_count++];
    asset_runtime_set_info_from_record(runtime, asset_record, ASSET_MATERIAL);
    runtime->asset_handles.material_handle = (Material_Handle){material_system->material_count, 0};

    //fill out the handle
    *out_handle = (Material_Handle){material_system->material_count, runtime->generation};


    //todo: push into the material buffer
    material_system->material_count++;

    return true;
}


void material_release(Material_System2* material_system, Material_Handle handle)
{
}


void material_upload_data(Material_System2* material_system, void* data, u64 data_size,
                          Material_Record_Handle* out_handle)
{
    Material_Record* record = &material_system->material_record[material_system->material_record_count];


    record->offset = material_system->material_buffer_offset;
    record->size = data_size;

    memcpy(material_system->material_buffer + material_system->material_buffer_offset, data, data_size);

    material_system->material_buffer_offset += data_size;

    *out_handle = (Material_Record_Handle){material_system->material_record_count, 0};


    material_system->material_record_count++;
}

Shader_Asset* material_system_get_shader_asset(Asset_System* asset_system, Material_Handle handle)
{
    Shader_Handle shader_handle = asset_system->material_system->materials[handle.handle].shader_handle;
    return shader_asset_get(asset_system->shader_system, shader_handle);
}


Madness_Material* material_system_get_material(Material_System2* material_system, Material_Handle handle)
{
    return &material_system->materials[handle.handle];
}

Material_Asset* material_system_get_asset(Material_System2* material_system, Material_Handle handle)
{
    return &material_system->material_assets[handle.handle];
}

//TODO: it would make sense to add a macro for this to ensure we are getting the struct we think we are getting
//also would allow us to cast to our material type for easy modification
//NOTE: do not keep a reference to the pointer
void* material_system_get_material_data(Material_System2* material_system, Material_Handle handle)
{
    Madness_Material* material = &material_system->materials[handle.handle];
    Material_Record_Handle record_handle = material->material_record_handle;

    Material_Record* record = &material_system->material_record[record_handle.handle];

    return material_system->material_buffer + record->offset;
}

Material_Definition material_system_get_material_definition(Asset_System* asset_system, Material_Handle handle)
{
    Material_System2* material_system = asset_system->material_system;
    Shader_System* shader_system = asset_system->shader_system;

    Madness_Material* material = &material_system->materials[handle.handle];

    //get the shader handle and set the shader key
    Shader_Asset* asset = shader_asset_get(shader_system, material->shader_handle);

    Material_Definition definition;
    shader_mapping_get_definition_from_shader_asset(asset_system->shader_system, asset, &definition);

    return definition;
}


void material_change_param(Material_System2* material_system, Material_Handle handle);
void material_change_texture(Material_System2* material_system, Material_Handle handle);


#endif //MATERIAL_SYSTEM_H
