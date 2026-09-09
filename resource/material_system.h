#ifndef MATERIAL_SYSTEM_H
#define MATERIAL_SYSTEM_H


#include "asset_system.h"
#include "resource_types.h"


bool material_system_init(Material_System* material_system, Asset_System* asset_system, Memory_System* memory_system);

bool material_system_shutdown(Material_System* material_system, Memory_System* memory_system);

bool material_system_generate_render_packet(Material_System* material_system,
                                            Render_Packet_3D* render_packet_3d);

bool material_system_add_shader_material_mapping(Asset_System* asset_system, Material_System* material_system,
                                                 const char* shader_name, const char* material_name,
                                                 Shader_Mesh_Type shader_type);

bool material_system_get_shader_material_mapping(Asset_System* asset_system,
                                                 const char* shader_name, Path_String* out_string);



Shader_Asset* shader_asset_acquire(Material_System* material_system,
                                   Shader_Handle* out_shader_handle);

bool shader_asset_release(Material_System* material_system, Shader_Handle shader_handle);

Shader_Asset* shader_asset_get(Material_System* material_system,
                               Shader_Handle out_shader_handle);

bool shader_asset_load_definitions(Asset_System* asset_system, Shader_Asset* shader_asset);

bool material_load_gpu_data(Asset_System* asset_system, Shader_Handle handle, Material* material,
                            Material_Data* out_gpu_data, Scratch_Allocator* scratch_allocator)
{
    Material_System* material_system = asset_system->material_system;


    // Shader_Asset* material_asset = &material_system->shader_asset[handle.handle];
    Material_Definition* material_definition = &material_system->material_definition[handle.handle];


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
    Scratch_Allocator scratch = scratch_allocator_begin(asset_system->allocator);
    Material_System* material_system = asset_system->material_system;


    //load in the gpu data/representation of our struct, also loads any textures needed
    Material_Data gpu_data;
    material_load_gpu_data(asset_system, shader_handle, in_material, &gpu_data, &scratch);


    //TODO: for modifying the material during development, we can just keep reloading the gpu definition and uploading into the buffer
    // for the actual game, it should modify the gpu data directly and in general flag the update as dirty

    //TODO: check the free list, and insert into that index
    //TODO: keep an array of dirty bits for material uploads/changes
    Material_Batch* material_batch = &material_system->material_batch[shader_handle.handle];
    dynamic_array_push(material_batch->material_meta_data, &in_material); // meta data
    dynamic_array_push(material_batch->material_data, gpu_data.material_data); // actual material data


    *out_material_handle = (Material_Handle){
        .material_batch_index = shader_handle.handle,
        .material_index = material_batch->material_data->num_items, // temp
        .generation = 0,

    };

    scratch_allocator_end(scratch);


    return true;
}

bool material_release(Material_System* material_system, Material_Handle material_handle)
{
    // material_system
    //TODO: no point in actually freeing the data, we can just mark it with a free list
    // when to free the whole thing or down size the array is another matter
}

bool material_get_metadata(Material_System* material_system,
                           Material_Handle material_handle, Material_Meta_Data* out_material_meta_data)
{
    Material_Batch* material_batch = &material_system->material_batch[material_handle.material_batch_index];

    out_material_meta_data = _dynamic_array_get(material_batch->material_meta_data, /*Material_Meta_Data,*/
                                                material_handle.material_index);
}

bool material_get_data(Material_System* material_system,
                       Material_Handle material_handle, void** out_material_data)
{
    Material_Batch* material_batch = &material_system->material_batch[material_handle.material_batch_index];
    *out_material_data = _dynamic_array_get(material_batch->material_data, material_handle.material_index);
}


//you have to create a material before requesting an add material type to it
// Material_Handle material_system_create_material(Material_System* material_system);

//shaders I want in the game:
// black hole, decals, screen space color gradient, multiple blend modes
// wireframe version for all shaders


/*
void material_system_add_skmesh_instance_to_default_material_batch(Asset_System* resource_system,
                                                                   Madness_SkMesh_Instance* parent_instance)
{
    Material_System* material_system = resource_system->material_system;

    // Mesh_Asset* mesh_asset = &mesh_system->mesh_asset_data[parent_instance->mesh_asset.handle];

    for (u32 batch_idx = 0; batch_idx < material_system->skinned_batch_count; batch_idx++)
    {
        Material_Batch* batch = &material_system->skinned_batch[batch_idx];

        if (string_compare_c_string(batch->material_info.shader_name, "skinned_mesh") != 0) { continue; }
        if (strcmp(batch->material_struct->name, "Material_Default") != 0) { continue; }

        for (u32 mesh_inst = 0; mesh_inst < parent_instance->mesh_count; ++mesh_inst)
        {
            Madness_Skinned_SubMesh_Instance* mesh_instance = &parent_instance->sk_mesh_instance_array[mesh_inst];
            dynamic_array_push(batch->mesh_instances, mesh_instance);
            dynamic_array_push(batch->material_data,
                               &material_system->prb[mesh_instance->material_handle.handle]);
        }
        return;
    }
}
*/

bool material_system_material_exist_by_uuid(Asset_System* asset_system, MADNESS_UUID uuid);

bool material_system_shader_exists_by_material_key(Asset_System* asset_system, Material_Key material_id,
                                                   Shader_Asset* out_asset, Shader_Handle* out_handle);



//NOTE: changing textures requires more elaborate steps
bool material_system_change_material_param(Asset_System* asset_system, Material_Handle material_handle,
                                           const char* param_name, const void* new_data);
void material_system_change_material_texture(Asset_System* asset_system, Material_Handle material_handle,
                                             const char* param_name, const char* texture_name);

void material_system_get_material_data(Asset_System* asset_system, Material_Handle handle);


void material_system_swap_material(Asset_System* asset_system, Material_Handle material_handle,
                                   const char* material_name);


//

Material_Key material_generate_id(Material_Info* material_info);


void material_create_gpu_definition(Asset_System* asset_system, Reflection_Runtime_Struct* reflection_material,
                                    Material_GPU_Definition* out_material_gpu_definition);
/**
 * @brief: sets any uuids in the materials to the default texture uuid
 */
void material_instance_set_default_textures(Asset_System* asset_system,
                                            Material* material,
                                            Material_Definition* material_definition);

void shader_asset_create(Asset_System* asset_system,
                         Material_Info* material_info,
                         Shader_Asset* shader_asset);

void material_definition_create(Asset_System* asset_system,
                                Material_Definition* material_definition,
                                const char* material_name);
/**
 * @note: allocates for a material instance with all values to 0, user has to fill it out
 * @note: its required that there be a material asset for creating an instance
 */
void material_create(Asset_System* asset_system, Shader_Asset* shader_asset,
                     Material_Definition* material_definition,
                     Material* out_material,
                     const char* material_name);

void material_create_from_data(Asset_System* asset_system,
                                        Shader_Asset* shader_asset,
                                        Material_Definition* material_definition,
                                        Material* out_material,
                                        const char* material_name,
                                        void* data);
#endif //MATERIAL_SYSTEM_H
