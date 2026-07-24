#ifndef ASSET_SERIALIZATION_H
#define ASSET_SERIALIZATION_H

#include "resource_types.h"


MAPI bool asset_texture_serialize(Madness_Texture_Runtime* runtime, FILE* fptr)
{
    fwrite(&runtime->version, sizeof(runtime->version), 1, fptr);
    fwrite(&runtime->texture, sizeof(Madness_Texture), 1, fptr);
    fwrite(runtime->pixel_data, runtime->texture.pixels_size, 1, fptr);
    return true;
}

MAPI bool asset_texture_deserialize(Madness_Texture_Runtime* runtime, FILE* fptr, Allocator* allocator)
{
    fread(&runtime->version, sizeof(runtime->version), 1, fptr);
    fread(&runtime->texture, sizeof(Madness_Texture), 1, fptr);
    runtime->pixel_data = allocator_alloc(allocator, runtime->texture.pixels_size);
    fread(runtime->pixel_data, runtime->texture.pixels_size, 1, fptr);
    return true;
}

MAPI bool asset_texture_deserialize_heap(Madness_Texture_Runtime* runtime, FILE* fptr, Heap_Allocator* allocator)
{
    fread(&runtime->version, sizeof(runtime->version), 1, fptr);
    fread(&runtime->texture, sizeof(Madness_Texture), 1, fptr);
    runtime->pixel_data = allocator_heap_alloc(allocator, runtime->texture.pixels_size);
    fread(runtime->pixel_data, runtime->texture.pixels_size, 1, fptr);
    return true;
}


MAPI bool asset_font_serialize(Madness_Font_Runtime* runtime, FILE* fptr)
{
    fwrite(&runtime->version, sizeof(runtime->version), 1, fptr);
    fwrite(&runtime->font_texture, sizeof(Madness_Font), 1, fptr);
    fwrite(&runtime->texture, sizeof(Madness_Texture), 1, fptr);
    fwrite(runtime->pixel_data, runtime->texture.pixels_size, 1, fptr);
    return true;
}

MAPI bool asset_font_deserialize(Madness_Font_Runtime* runtime, FILE* fptr, Allocator* allocator)
{
    fread(&runtime->version, sizeof(runtime->version), 1, fptr);
    fread(&runtime->font_texture, sizeof(Madness_Font), 1, fptr);
    fread(&runtime->texture, sizeof(Madness_Texture), 1, fptr);
    runtime->pixel_data = allocator_alloc(allocator,
                                          runtime->texture.pixels_size);
    fread(runtime->pixel_data, runtime->texture.pixels_size, 1, fptr);
    return true;
}

MAPI bool asset_font_deserialize_heap(Madness_Font_Runtime* runtime, FILE* fptr, Heap_Allocator* allocator)
{
    fread(&runtime->version, sizeof(runtime->version), 1, fptr);
    fread(&runtime->font_texture, sizeof(Madness_Font), 1, fptr);
    fread(&runtime->texture, sizeof(Madness_Texture), 1, fptr);
    runtime->pixel_data = allocator_heap_alloc(allocator,
                                               runtime->texture.pixels_size);
    fread(runtime->pixel_data, runtime->texture.pixels_size, 1, fptr);
    return true;
}


MAPI bool asset_material_serialize(Material_Asset_Runtime* runtime, FILE* fptr)
{
    fwrite(&runtime->version, sizeof(runtime->version), 1, fptr);
    string_serialize(runtime->asset->material_info.shader_name, fptr);
    string_serialize(runtime->asset->material_info.material_name, fptr);
    fwrite(&runtime->asset->material_info.shader_stage, sizeof(runtime->asset->material_info.shader_stage), 1, fptr);
    fwrite(&runtime->asset->material_info.shader_pass, sizeof(runtime->asset->material_info.shader_pass), 1, fptr);
    fwrite(&runtime->asset->material_info.mesh_type, sizeof(runtime->asset->material_info.mesh_type), 1, fptr);
    fwrite(&runtime->asset->material_info.blend_mode, sizeof(runtime->asset->material_info.blend_mode), 1, fptr);

    reflection_registry_serialize_runtime_struct(runtime->asset->reflection_material_data, fptr);

    fwrite(&runtime->asset->material_gpu_definition->field_count, sizeof(u32), 1, fptr);
    fwrite(&runtime->asset->material_gpu_definition->struct_size, sizeof(u32), 1, fptr);
    fwrite(&runtime->asset->material_gpu_definition->name_hashes,
           sizeof(u64) * runtime->asset->material_gpu_definition->field_count, 1,
           fptr);
    fwrite(&runtime->asset->material_gpu_definition->field_offsets,
           sizeof(u32) * runtime->asset->material_gpu_definition->field_count, 1,
           fptr);
    fwrite(&runtime->asset->material_gpu_definition->types,
           sizeof(Reflection_Type) * runtime->asset->material_gpu_definition->field_count,
           1, fptr);


    fclose(fptr);
    return true;
}

MAPI bool asset_material_deserialize(Material_Asset_Runtime* runtime, FILE* fptr, Allocator* allocator)
{
    fread(&runtime->version, sizeof(runtime->version), 1, fptr);
    string_deserialize(runtime->asset->material_info.shader_name, fptr, allocator);
    string_deserialize(runtime->asset->material_info.material_name, fptr, allocator);
    fread(&runtime->asset->material_info.shader_stage, sizeof(runtime->asset->material_info.shader_stage), 1, fptr);
    fread(&runtime->asset->material_info.shader_pass, sizeof(runtime->asset->material_info.shader_pass), 1, fptr);
    fread(&runtime->asset->material_info.mesh_type, sizeof(runtime->asset->material_info.mesh_type), 1, fptr);
    fread(&runtime->asset->material_info.blend_mode, sizeof(runtime->asset->material_info.blend_mode), 1, fptr);

    reflection_registry_deserialize_runtime_struct(runtime->asset->reflection_material_data, fptr, allocator);


    fread(&runtime->asset->material_gpu_definition->field_count, sizeof(u32), 1, fptr);
    fread(&runtime->asset->material_gpu_definition->struct_size, sizeof(u32), 1, fptr);
    fread(&runtime->asset->material_gpu_definition->name_hashes,
          sizeof(u64) * runtime->asset->material_gpu_definition->field_count, 1,
          fptr);
    fread(&runtime->asset->material_gpu_definition->field_offsets,
          sizeof(u32) * runtime->asset->material_gpu_definition->field_count, 1,
          fptr);
    fread(&runtime->asset->material_gpu_definition->types,
          sizeof(Reflection_Type) * runtime->asset->material_gpu_definition->field_count,
          1, fptr);


    fclose(fptr);
    return true;

}

MAPI bool asset_material_deserialize_heap(Material_Asset_Runtime* runtime, FILE* fptr,
                                          Heap_Allocator* allocator)
{
    fread(&runtime->version, sizeof(runtime->version), 1, fptr);

    runtime->asset->material_info.shader_name = allocator_heap_alloc(allocator, sizeof(String));
    runtime->asset->material_info.material_name = allocator_heap_alloc(allocator, sizeof(String));
    string_deserialize_heap(runtime->asset->material_info.shader_name, fptr, allocator);
    string_deserialize_heap(runtime->asset->material_info.material_name, fptr, allocator);
    fread(&runtime->asset->material_info.shader_stage, sizeof(runtime->asset->material_info.shader_stage), 1, fptr);
    fread(&runtime->asset->material_info.shader_pass, sizeof(runtime->asset->material_info.shader_pass), 1, fptr);
    fread(&runtime->asset->material_info.mesh_type, sizeof(runtime->asset->material_info.mesh_type), 1, fptr);
    fread(&runtime->asset->material_info.blend_mode, sizeof(runtime->asset->material_info.blend_mode), 1, fptr);

    runtime->asset->reflection_material_data = allocator_heap_alloc(allocator, sizeof(Reflection_Runtime_Struct));
    runtime->asset->material_gpu_definition = allocator_heap_alloc(allocator, sizeof(Material_GPU_Definition));
    reflection_registry_deserialize_runtime_struct_heap(runtime->asset->reflection_material_data, fptr, allocator);

    fread(&runtime->asset->material_gpu_definition->field_count, sizeof(u32), 1, fptr);
    fread(&runtime->asset->material_gpu_definition->struct_size, sizeof(u32), 1, fptr);

    runtime->asset->material_gpu_definition->name_hashes = allocator_heap_alloc(allocator, sizeof(u64) * runtime->asset->material_gpu_definition->field_count);
    runtime->asset->material_gpu_definition->field_offsets = allocator_heap_alloc(allocator, sizeof(u32) * runtime->asset->material_gpu_definition->field_count);
    runtime->asset->material_gpu_definition->types = allocator_heap_alloc(allocator, sizeof(Reflection_Type) * runtime->asset->material_gpu_definition->field_count);

    fread(&runtime->asset->material_gpu_definition->name_hashes,
      sizeof(u64) * runtime->asset->material_gpu_definition->field_count, 1,
      fptr);
    fread(&runtime->asset->material_gpu_definition->field_offsets,
      sizeof(u32) * runtime->asset->material_gpu_definition->field_count, 1,
      fptr);
    fread(&runtime->asset->material_gpu_definition->types,
          sizeof(Reflection_Type) * runtime->asset->material_gpu_definition->field_count,
          1, fptr);


    fclose(fptr);
    return true;

}

MAPI bool asset_mesh_serialize(Madness_Mesh_Runtime* runtime, FILE* fptr)
{
    fwrite(&runtime->version, sizeof(runtime->version), 1, fptr);
    fwrite(&runtime->mesh_count, sizeof(runtime->mesh_count), 1, fptr);
    fwrite(runtime->submeshes, sizeof(Madness_SubMesh) * runtime->mesh_count, 1, fptr);
    //submesh contains the material uuid

    //mesh data
    for (u32 i = 0; i < runtime->mesh_count; ++i)
    {
        Madness_SubMesh* sub_mesh = &runtime->submeshes[i];
        fwrite(runtime->mesh_gpu_upload[i].tangent, sub_mesh->tangent_bytes, 1, fptr);
        fwrite(runtime->mesh_gpu_upload[i].vertex_color, sub_mesh->vertex_color_bytes, 1, fptr);
        fwrite(runtime->mesh_gpu_upload[i].vertex, sub_mesh->vertex_bytes, 1, fptr);
        fwrite(runtime->mesh_gpu_upload[i].normal, sub_mesh->normal_bytes, 1, fptr);
        fwrite(runtime->mesh_gpu_upload[i].uv, sub_mesh->uv_bytes, 1, fptr);
        fwrite(runtime->mesh_gpu_upload[i].indices, sub_mesh->indices_bytes, 1, fptr);
    }

    for (u32 i = 0; i < runtime->mesh_count; ++i)
    {
        Material_Instance* material_instance = &runtime->material_instance[i];
        fwrite(&material_instance->uuid_material_asset, sizeof(material_instance->uuid_material_asset), 1, fptr);
        fwrite(&material_instance->data_size, sizeof(material_instance->data_size), 1, fptr);
        fwrite(material_instance->material_data, material_instance->data_size, 1, fptr);
    }
    return true;

}

MAPI bool asset_mesh_deserialize(Madness_Mesh_Runtime* runtime, FILE* fptr, Allocator* allocator)
{
    fread(&runtime->version, sizeof(runtime->version), 1, fptr);
    fread(&runtime->mesh_count, sizeof(runtime->mesh_count), 1, fptr);

    runtime->submeshes = allocator_alloc(allocator, sizeof(Madness_SubMesh) * runtime->mesh_count);
    runtime->mesh_gpu_upload = allocator_alloc(allocator, sizeof(Madness_Mesh_GPU_Data) * runtime->mesh_count);
    runtime->material_instance = allocator_alloc(allocator, sizeof(Material_Instance) * runtime->mesh_count);

    fread(runtime->submeshes, sizeof(Madness_SubMesh) * runtime->mesh_count, 1, fptr);

    //mesh data
    for (u32 i = 0; i < runtime->mesh_count; ++i)
    {
        Madness_SubMesh* sub_mesh = &runtime->submeshes[i];

        runtime->mesh_gpu_upload[i].tangent = allocator_alloc(allocator, sub_mesh->tangent_bytes);
        runtime->mesh_gpu_upload[i].vertex_color = allocator_alloc(allocator, sub_mesh->vertex_color_bytes);
        runtime->mesh_gpu_upload[i].vertex = allocator_alloc(allocator, sub_mesh->vertex_bytes);
        runtime->mesh_gpu_upload[i].normal = allocator_alloc(allocator, sub_mesh->normal_bytes);
        runtime->mesh_gpu_upload[i].uv = allocator_alloc(allocator, sub_mesh->uv_bytes);
        runtime->mesh_gpu_upload[i].indices = allocator_alloc(allocator, sub_mesh->indices_bytes);


        fread(runtime->mesh_gpu_upload[i].tangent, sub_mesh->tangent_bytes, 1, fptr);
        fread(runtime->mesh_gpu_upload[i].vertex_color, sub_mesh->vertex_color_bytes, 1, fptr);
        fread(runtime->mesh_gpu_upload[i].vertex, sub_mesh->vertex_bytes, 1, fptr);
        fread(runtime->mesh_gpu_upload[i].normal, sub_mesh->normal_bytes, 1, fptr);
        fread(runtime->mesh_gpu_upload[i].uv, sub_mesh->uv_bytes, 1, fptr);
        fread(runtime->mesh_gpu_upload[i].indices, sub_mesh->indices_bytes, 1, fptr);
    }

    for (u32 i = 0; i < runtime->mesh_count; ++i)
    {
        Material_Instance* material_instance = &runtime->material_instance[i];
        fwrite(&material_instance->uuid_material_asset, sizeof(material_instance->uuid_material_asset), 1, fptr);
        fwrite(&material_instance->data_size, sizeof(material_instance->data_size), 1, fptr);
        fwrite(material_instance->material_data, material_instance->data_size, 1, fptr);
    }
    return true;

}

MAPI bool asset_mesh_deserialize_heap(Madness_Mesh_Runtime* runtime, FILE* fptr, Heap_Allocator* allocator)
{
    fread(&runtime->version, sizeof(runtime->version), 1, fptr);
    fread(&runtime->mesh_count, sizeof(runtime->mesh_count), 1, fptr);

    runtime->submeshes = allocator_heap_alloc(allocator, sizeof(Madness_SubMesh) * runtime->mesh_count);
    runtime->mesh_gpu_upload = allocator_heap_alloc(allocator, sizeof(Madness_Mesh_GPU_Data) * runtime->mesh_count);
    runtime->material_instance = allocator_heap_alloc(allocator, sizeof(Material_Instance) * runtime->mesh_count);


    fread(runtime->submeshes, sizeof(Madness_SubMesh) * runtime->mesh_count, 1, fptr);
    //submesh contains the material uuid

    //mesh data
    for (u32 i = 0; i < runtime->mesh_count; ++i)
    {
        Madness_SubMesh* sub_mesh = &runtime->submeshes[i];

        runtime->mesh_gpu_upload[i].tangent = allocator_heap_alloc(allocator, sub_mesh->tangent_bytes);
        runtime->mesh_gpu_upload[i].vertex_color = allocator_heap_alloc(allocator, sub_mesh->vertex_color_bytes);
        runtime->mesh_gpu_upload[i].vertex = allocator_heap_alloc(allocator, sub_mesh->vertex_bytes);
        runtime->mesh_gpu_upload[i].normal = allocator_heap_alloc(allocator, sub_mesh->normal_bytes);
        runtime->mesh_gpu_upload[i].uv = allocator_heap_alloc(allocator, sub_mesh->uv_bytes);
        runtime->mesh_gpu_upload[i].indices = allocator_heap_alloc(allocator, sub_mesh->indices_bytes);


        fread(runtime->mesh_gpu_upload[i].tangent, sub_mesh->tangent_bytes, 1, fptr);
        fread(runtime->mesh_gpu_upload[i].vertex_color, sub_mesh->vertex_color_bytes, 1, fptr);
        fread(runtime->mesh_gpu_upload[i].vertex, sub_mesh->vertex_bytes, 1, fptr);
        fread(runtime->mesh_gpu_upload[i].normal, sub_mesh->normal_bytes, 1, fptr);
        fread(runtime->mesh_gpu_upload[i].uv, sub_mesh->uv_bytes, 1, fptr);
        fread(runtime->mesh_gpu_upload[i].indices, sub_mesh->indices_bytes, 1, fptr);
    }


    for (u32 i = 0; i < runtime->mesh_count; ++i)
    {
        Material_Instance* material_instance = &runtime->material_instance[i];
        fread(&material_instance->uuid_material_asset, sizeof(material_instance->uuid_material_asset), 1, fptr);
        fread(&material_instance->data_size, sizeof(material_instance->data_size), 1, fptr);
        material_instance->material_data = allocator_heap_alloc(allocator, material_instance->data_size);
        fread(material_instance->material_data, material_instance->data_size, 1, fptr);
    }
    return true;

}


#endif
