#ifndef MATERIAL_SYSTEM_H
#define MATERIAL_SYSTEM_H


#include "asset_system.h"
#include "resource_types.h"



bool material_system_init(Material_System* material_system, Asset_System* asset_system, Memory_System* memory_system);

bool material_system_shutdown(Material_System* material_system, Memory_System* memory_system);

bool material_system_generate_render_packet(Material_System* material_system,
                                            Render_Packet_3D* render_packet_3d);



//////////// SHADERS ////////////
void material_system_add_shader_material_mapping(Asset_System* asset_system, Material_System* material_system,
                                                 const char* shader_name, const char* material_name);

/**
 * @note expects a null string
 */
bool shader_material_mapping_get_material_name_and_defintion(Asset_System* asset_system,
                                                             String* shader_name,
                                                             String* out_string,
                                                             Material_Definition* out_definition,
                                                             Allocator* allocator);
bool shader_material_mapping_get_index(Asset_System* asset_system, String* shader_name,
                                                    u32* index);




bool shader_material_mapping_get_material_defintion(Asset_System* asset_system,
                                                             String* shader_name,
                                                             Material_Definition* out_definition);

String_Builder* shader_asset_construct_path(Material_System* material_system,
                               Shader_Asset* asset, Allocator* allocator);


Shader_Key shader_generate_key(Shader_Info* shader_info);

Shader_Asset* shader_asset_get(Material_System* material_system,
                               Shader_Handle out_shader_handle);

bool shader_asset_acquire_load_data(Asset_System* asset_system, Shader_Asset* loaded_asset, Shader_Handle* out_handle);


void shader_get_or_create(Asset_System* asset_system,
                          Shader_Info* shader_info,
                          Shader_Handle* out_handle);

//TODO: basically scan all our material instances and remove any shaders we dont find
void shader_purge_unused(Asset_System* asset_system,
                         Shader_Info* material_info,
                         Shader_Handle* out_handle);


//////////// MATERIALS ////////////



void material_upload(Asset_System* asset_system, void* data, u64 byte_size, Material_Handle* out_handle)
{
    Material_System* material_system = asset_system->material_system;


    u32 handle_index = material_system->material_record_count++;
    Material_Record* material_record = &material_system->material_record[handle_index];
    *material_record = (Material_Record){
        .offset = handle_index,
        .size = byte_size,
    };

    *out_handle = (Material_Handle){
        .handle = handle_index,
        .generation = material_system->generation[handle_index],
    };

    memcpy(material_system->material_buffer + material_system->material_buffer_offset, data, byte_size);
    material_system->material_buffer_offset += byte_size;
}

void material_free(Asset_System* asset_system, Material_Handle handle)
{
    MASSERT(false);
    Material_System* material_system = asset_system->material_system;

    material_system->material_record[handle.handle];
    material_system->generation[handle.handle]++;
    //TODO: add to a free list
}

void* material_get(Asset_System* asset_system, Material_Handle handle)
{
    Material_System* material_system = asset_system->material_system;
    //TODO: if generation fails or some other reason, pass back a blob of data
    u32 generation[MAX_MATERIAL_COUNT];

    if (asset_system->material_system->generation[handle.handle] != handle.generation)
    {
        M_ERROR("MATERIAL GET: INVALID GENERTATION PASSING BACK DATA BLOB")
        MASSERT(false);
        return NULL;
    }

    Material_Record* record = &material_system->material_record[handle.handle];
    return ((u8*)material_system->material_buffer + record->offset);
}


Material* material_get_metadata(Asset_System* asset_system,
                                Material_Handle handle)
{
    Material_System* material_system = asset_system->material_system;

    //TODO: if generation fails or some other reason, pass back a blob of data
    if (asset_system->material_system->generation[handle.handle] != handle.generation)
    {
        M_ERROR("MATERIAL GET METADATA: INVALID GENERTATION PASSING BACK DATA BLOB")
        MASSERT(false);
        return NULL;
    }

    return &material_system->materials[handle.handle];
}


//TODO:
/*
Material_Definition material_get_definition(Asset_System* asset_system, Material_Handle handle)
{

}

Shader_Asset material_get_shader_asset(Asset_System* asset_system,Material_Handle handle)
{
}
*/

u64 material_create_sort_key(Asset_System* asset_system,
                             Material_Handle handle, float z_value)
{
    Material* material = material_get_metadata(asset_system, handle);

    return sort_key_make_opaque(material->meta_data.shader_id, material->meta_data.permutation, z_value);
}


//TODO: update the material buffer
// bool material_system_update(Material_System* material_system, Asset_System* asset_system, Memory_System* memory_system);






bool material_load_gpu_data(Asset_System* asset_system, Shader_Handle handle, Material* material,
                            Material_Data* out_gpu_data, Scratch_Allocator* scratch_allocator)
{
    Material_System* material_system = asset_system->material_system;


    // Shader_Asset* material_asset = &material_system->shader_asset[handle.handle];
    u32 def_index = asset_system->material_system->shader_asset_to_mapping[handle.handle];
    Material_Definition* material_definition = &asset_system->material_system->shader_to_material_mapping.material_definition[def_index];

    out_gpu_data->material_data = allocator_alloc(scratch_allocator->allocator,
                                                  material_definition->material_gpu_definition.struct_size);
    out_gpu_data->data_size = material_definition->material_gpu_definition.struct_size;

    //resolve any uuid for texture loading
    for (u32 i = 0; i < material_definition->reflection_material_data.field_count; i++)
    {
        Reflection_Runtime_Struct_Field* field = &material_definition->reflection_material_data.fields[i];

        if (field->type == REFLECTION_TYPE_UUID)
        {
            MASSERT(material_definition->material_gpu_definition.types[i] == REFLECTION_TYPE_U32);

            //load in texture and use the bindless index
            MADNESS_UUID uuid_data = *(MADNESS_UUID*)((u8*)material->cpu_data.material_data + field->offset);
            //sometimes we dont have a texture so we just set the value to zero
            if (uuid_data.high == 0 && uuid_data.low == 0)
            {
                u32 default_texture = 0;
                //copy into the gpu struct
                memcpy(
                    ((u8*)out_gpu_data->material_data + material_definition->material_gpu_definition.field_offsets[i]),
                    &default_texture,
                    sizeof(u32));
            }
            else
            {
                Texture_Handle texture_handle = asset_load_texture_uuid(asset_system, uuid_data);

                //copy into the gpu struct
                memcpy(
                    ((u8*)out_gpu_data->material_data + material_definition->material_gpu_definition.field_offsets[i]),
                    &texture_handle.handle,
                    sizeof(u32));
            }
        }
        else if (material_definition->material_gpu_definition.types[i] == field->type)
        {
            //copy into the gpu struct
            memcpy((u8*)out_gpu_data->material_data + material_definition->material_gpu_definition.field_offsets[i],
                   (u8*)material->cpu_data.material_data + field->offset,
                   reflection_type_get_size(material_definition->material_gpu_definition.types[i]));
        }
        else
        {
            MASSERT(false); //this should never happen
        }
    }

    return true;
}

/**
 * the shader asset needs to be loaded or given back a handle before fully loading the material instance,
 * you can assume that the shader is loaded, as long as it was valid
 * you have to load the material
 */
bool material_acquire(Asset_System* asset_system, Shader_Handle shader_handle, Material* in_material,
                      Material_Handle* out_material_handle)
{
    Scratch_Allocator scratch = scratch_allocator_begin(asset_system->scratch_allocator);
    Material_System* material_system = asset_system->material_system;


    //load in the gpu data/representation of our struct, also loads any textures needed
    Material_Data gpu_data;
    material_load_gpu_data(asset_system, shader_handle, in_material, &gpu_data, &scratch);

    material_upload(asset_system, gpu_data.material_data, gpu_data.data_size, out_material_handle);
    Material* mat = &material_system->materials[out_material_handle->handle];
    mat = in_material;


    scratch_allocator_end(scratch);


    return true;
}





void material_create_gpu_definition(Asset_System* asset_system, Reflection_Runtime_Struct* reflection_material,
                                    Material_GPU_Definition* out_material_gpu_definition);
/**
 * @brief: sets any uuids in the materials to the default texture uuid
 */
void material_instance_set_default_textures(Asset_System* asset_system,
                                            Material* material,
                                            Material_Definition* material_definition);




void material_definition_create(Asset_System* asset_system,
                                Material_Definition* material_definition,
                                const char* material_name, Allocator* allocator);
/**
 * @note: allocates for a material instance with all values to 0, user has to fill it out
 * @note: its required that there be a material asset for creating an instance
 */

void material_create_from_data(Asset_System* asset_system,
                               Shader_Handle* shader_handle,
                               Material* out_material,
                               const char* material_asset_name,
                               void* data);
#endif //MATERIAL_SYSTEM_H
