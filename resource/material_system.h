#ifndef MATERIAL_SYSTEM_H
#define MATERIAL_SYSTEM_H

#include "asset_converter.h"
#include "asset_registry.h"
#include "asset_serialization.h"
#include "resource_types.h"


bool material_system_init(Material_System* material_system, Asset_System* asset_system, Memory_System* memory_system);

bool material_system_shutdown(Material_System* material_system, Memory_System* memory_system);

bool material_system_generate_render_packet(Material_System* material_system,
                                            Render_Packet_3D* render_packet_3d);

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

bool material_system_exists(Asset_System* asset_system, MADNESS_UUID uuid);
bool material_system_load_material_instance(Asset_System* asset_system, Material_Instance* material_instance,
                                            Material_Handle* out_handle);


bool material_system_load_material_asset(Asset_System* asset_system, MADNESS_UUID uuid, u64 uuid_hash,
                                         Material_Asset_Runtime* material_asset);


//NOTE: changing textures requires more elaborate steps
bool material_system_change_material_param(Asset_System* asset_system, Material_Handle material_handle,
                                           const char* param_name, const void* new_data);
void material_system_change_material_texture(Asset_System* asset_system, Material_Handle material_handle,
                                             const char* param_name, const char* texture_name);

void material_system_get_material_data(Asset_System* asset_system, Material_Handle handle);


void material_system_swap_material(Asset_System* asset_system, Material_Handle material_handle,
                                   const char* material_name);


void material_system_add_mesh_instance_and_material(Asset_System* asset_system, Madness_Mesh* madness_mesh,
                                                    Madness_Mesh_Instance* parent_instance);
void material_system_add_skinned_instance_and_material(Asset_System* asset_system, Madness_Skinned_Mesh* madness_mesh,
                                                       Madness_Skinned_Mesh_Instance* parent_instance);


//

Material_ID material_system_generate_id(Material_Info* material_info);


void material_create_gpu_definition(Asset_System* asset_system,
                                    Reflection_Runtime_Struct reflection_material,
                                    Material_GPU_Definition* out_material_gpu_definition)
{
    out_material_gpu_definition = allocator_alloc(asset_system->frame_allocator,
                                                  sizeof(Material_GPU_Definition));
    out_material_gpu_definition->field_count = reflection_material.field_count;
    out_material_gpu_definition->name_hashes = allocator_alloc(asset_system->frame_allocator,
                                                               sizeof(u64) *
                                                               reflection_material.field_count);
    out_material_gpu_definition->field_offsets = allocator_alloc(asset_system->frame_allocator,
                                                                 sizeof(u32) *
                                                                 reflection_material.field_count);
    out_material_gpu_definition->types = allocator_alloc(asset_system->frame_allocator,
                                                         sizeof(Reflection_Type) *
                                                         reflection_material.field_count);

    out_material_gpu_definition->struct_size = 0;

    u32 offset = 0;
    for (u32 i = 0; i < reflection_material.field_count; i++)
    {
        out_material_gpu_definition->name_hashes[i] =
            c_string_hash_u64(reflection_material.fields[i].name);

        if (reflection_material.fields[i].type == REFLECTION_TYPE_UUID)
        {
            out_material_gpu_definition->types[i] = REFLECTION_TYPE_U32;
        }
        else
        {
            out_material_gpu_definition->types[i] = reflection_material.fields[i].type;
        }
        out_material_gpu_definition->field_offsets[i] = offset;
        out_material_gpu_definition->struct_size += reflection_type_get_size(out_material_gpu_definition->types[i]);
        offset += reflection_type_get_size(out_material_gpu_definition->types[i]);
    }
}


void material_system_create_material_instance(Asset_System* asset_system,
                                              Heap_Allocator* heap_allocator,
                                              Material_Asset* material_asset,
                                              Material_Instance* out_material_instance, const char* asset_name)
{
    out_material_instance->material_data = allocator_heap_alloc(heap_allocator,
                                                                material_asset->reflection_material_data->struct_size);
    out_material_instance->data_size = material_asset->reflection_material_data->struct_size;


    //create the material instance
    asset_converter_material_instance_from_material_asset(asset_system,  out_material_instance,
                                                           &material_asset->material_info,
                                                           asset_name);

}

void material_system_create_material_asset(Asset_System* asset_system,
                                           Material_Info* material_info,
                                           Material_Asset* material_asset)
{
    material_info->material_id = material_system_generate_id(material_info);


    Reflection_Runtime_Struct reflection_material = reflection_registry_get_struct(
        asset_system->global_reflection_registry, MATERIAL_DEFAULT_NAME);

    Material_GPU_Definition* material_gpu_definition = NULL;
    material_create_gpu_definition(asset_system, reflection_material, material_gpu_definition);

    Material_Asset asset = {
        .material_info = asset.material_info = *material_info,
        .reflection_material_data = &reflection_material,
        .material_gpu_definition = material_gpu_definition,
    };

    *material_asset = asset;


    //we only want to serialize the material asset if it does not exist
    //NOTE: we serialize material instances separately
    String_Builder* str_builder = string_builder_create(256, asset_system->frame_allocator);
    string_builder_append_c_string(str_builder, ENGINE_MATERIAL_PATH);
    string_builder_append_string(str_builder, material_info->material_name);
    string_builder_append_c_string(str_builder, "_");
    string_builder_append_string(str_builder, material_info->shader_name);
    string_builder_append_c_string(str_builder, "_");
    string_builder_append_u64(str_builder, asset.material_info.material_id, asset_system->frame_allocator);
    string_builder_append_c_string(str_builder, ENGINE_MATERIAL_EXTENSION);

    if (filesystem_does_file_exists(string_builder_to_c_string(str_builder)))
    {
        const char* output_path = string_builder_to_c_string(str_builder);
        FILE* fptr = fopen(output_path, "wb");
        if (!fptr)
        {
            MASSERT(false);
        }

        Material_Asset_Runtime asset_editor = {0};
        asset_editor.version = 1;
        asset_editor.asset = &asset;
        asset_material_serialize(&asset_editor, fptr);

        asset_registry_add_asset(asset_system->asset_registry, output_path, output_path,
                                 ASSET_MATERIAL, asset_system->heap_allocator, NULL);
        fclose(fptr);
    }
}


#endif //MATERIAL_SYSTEM_H
