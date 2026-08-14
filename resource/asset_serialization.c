#include "asset_serialization.h"

bool asset_texture_serialize(Madness_Texture_Runtime* runtime, FILE* fptr)
{
    fwrite(&runtime->version, sizeof(runtime->version), 1, fptr);
    fwrite(&runtime->texture, sizeof(Madness_Texture), 1, fptr);
    fwrite(runtime->pixel_data, runtime->texture.pixels_size, 1, fptr);
    return true;
}

bool asset_texture_deserialize(Madness_Texture_Runtime* runtime, FILE* fptr, Allocator* allocator)
{
    fread(&runtime->version, sizeof(runtime->version), 1, fptr);
    fread(&runtime->texture, sizeof(Madness_Texture), 1, fptr);
    runtime->pixel_data = allocator_alloc(allocator, runtime->texture.pixels_size);
    fread(runtime->pixel_data, runtime->texture.pixels_size, 1, fptr);
    return true;
}

bool asset_texture_deserialize_heap(Madness_Texture_Runtime* runtime, FILE* fptr, Heap_Allocator* allocator)
{

    /* unsuccessful attempt, wierd padding/alignment issues
    u64 size = filesystem_file_size(fptr);
    void* data = allocator_heap_alloc(allocator, size);

    fread(data, size, 1, fptr);
    runtime = (Madness_Texture_Runtime*)data;
    memcpy(&runtime->texture, (u8*)data + sizeof(runtime->version), sizeof(Madness_Texture));
    runtime->pixel_data = (u8*)((u8*)data + (runtime->version) +sizeof(Madness_Texture));
    /*runtime->version = (u8*)data;
    runtime->texture = *(Madness_Texture*)((u8*)data + sizeof(runtime->version));
    runtime->pixel_data = (u8*)((u8*)data + sizeof(runtime->version) + sizeof(runtime->texture));#1#
    */

    fread(&runtime->version, sizeof(runtime->version), 1, fptr);
    fread(&runtime->texture, sizeof(Madness_Texture), 1, fptr);
    runtime->pixel_data = allocator_heap_alloc(allocator, runtime->texture.pixels_size);
    fread(runtime->pixel_data, runtime->texture.pixels_size, 1, fptr);
    return true;
}


bool asset_font_serialize(Madness_Font_Runtime* runtime, FILE* fptr)
{
    fwrite(&runtime->version, sizeof(runtime->version), 1, fptr);
    fwrite(&runtime->font_texture, sizeof(Madness_Font), 1, fptr);
    fwrite(&runtime->texture, sizeof(Madness_Texture), 1, fptr);
    fwrite(runtime->pixel_data, runtime->texture.pixels_size, 1, fptr);
    return true;
}

bool asset_font_deserialize(Madness_Font_Runtime* runtime, FILE* fptr, Allocator* allocator)
{
    fread(&runtime->version, sizeof(runtime->version), 1, fptr);
    fread(&runtime->font_texture, sizeof(Madness_Font), 1, fptr);
    fread(&runtime->texture, sizeof(Madness_Texture), 1, fptr);
    runtime->pixel_data = allocator_alloc(allocator,
                                          runtime->texture.pixels_size);
    fread(runtime->pixel_data, runtime->texture.pixels_size, 1, fptr);
    return true;
}

bool asset_font_deserialize_heap(Madness_Font_Runtime* runtime, FILE* fptr, Heap_Allocator* allocator)
{
    fread(&runtime->version, sizeof(runtime->version), 1, fptr);
    fread(&runtime->font_texture, sizeof(Madness_Font), 1, fptr);
    fread(&runtime->texture, sizeof(Madness_Texture), 1, fptr);
    runtime->pixel_data = allocator_heap_alloc(allocator, runtime->texture.pixels_size);
    fread(runtime->pixel_data, runtime->texture.pixels_size, 1, fptr);
    return true;
}


bool asset_material_serialize(Material_Asset_Runtime* runtime, FILE* fptr)
{
    fwrite(&runtime->version, sizeof(runtime->version), 1, fptr);
    string_serialize(runtime->asset->material_info.shader_name, fptr);
    string_serialize(runtime->asset->material_info.material_name, fptr);
    fwrite(&runtime->asset->material_info.renderpass, sizeof(runtime->asset->material_info.renderpass), 1, fptr);
    fwrite(&runtime->asset->material_info.transluency, sizeof(runtime->asset->material_info.transluency), 1, fptr);
    fwrite(&runtime->asset->material_info.mesh_type, sizeof(runtime->asset->material_info.mesh_type), 1, fptr);
    fwrite(&runtime->asset->material_info.blend_mode, sizeof(runtime->asset->material_info.blend_mode), 1, fptr);
    fwrite(&runtime->asset->material_info.material_id, sizeof(runtime->asset->material_info.material_id), 1, fptr);

    reflection_registry_serialize_runtime_struct(runtime->asset->reflection_material_data, fptr);

    fwrite(&runtime->asset->material_gpu_definition->field_count, sizeof(u32), 1, fptr);
    fwrite(&runtime->asset->material_gpu_definition->struct_size, sizeof(u32), 1, fptr);
    fwrite(runtime->asset->material_gpu_definition->name_hashes,
           sizeof(u64) * runtime->asset->material_gpu_definition->field_count, 1,
           fptr);
    fwrite(runtime->asset->material_gpu_definition->field_offsets,
           sizeof(u32) * runtime->asset->material_gpu_definition->field_count, 1,
           fptr);
    fwrite(runtime->asset->material_gpu_definition->types,
           sizeof(Reflection_Type) * runtime->asset->material_gpu_definition->field_count,
           1, fptr);


    return true;
}

bool asset_material_deserialize(Material_Asset_Runtime* runtime, FILE* fptr, Allocator* allocator)
{
    fread(&runtime->version, sizeof(runtime->version), 1, fptr);
    string_deserialize(runtime->asset->material_info.shader_name, fptr, allocator);
    string_deserialize(runtime->asset->material_info.material_name, fptr, allocator);
    fread(&runtime->asset->material_info.renderpass, sizeof(runtime->asset->material_info.renderpass), 1, fptr);
    fread(&runtime->asset->material_info.transluency, sizeof(runtime->asset->material_info.transluency), 1, fptr);
    fread(&runtime->asset->material_info.mesh_type, sizeof(runtime->asset->material_info.mesh_type), 1, fptr);
    fread(&runtime->asset->material_info.blend_mode, sizeof(runtime->asset->material_info.blend_mode), 1, fptr);
    fread(&runtime->asset->material_info.material_id, sizeof(runtime->asset->material_info.material_id), 1, fptr);

    reflection_registry_deserialize_runtime_struct(runtime->asset->reflection_material_data, fptr, allocator);


    fread(&runtime->asset->material_gpu_definition->field_count, sizeof(u32), 1, fptr);
    fread(&runtime->asset->material_gpu_definition->struct_size, sizeof(u32), 1, fptr);
    fread(runtime->asset->material_gpu_definition->name_hashes,
          sizeof(u64) * runtime->asset->material_gpu_definition->field_count, 1,
          fptr);
    fread(runtime->asset->material_gpu_definition->field_offsets,
          sizeof(u32) * runtime->asset->material_gpu_definition->field_count, 1,
          fptr);
    fread(runtime->asset->material_gpu_definition->types,
          sizeof(Reflection_Type) * runtime->asset->material_gpu_definition->field_count,
          1, fptr);


    return true;
}

bool asset_material_deserialize_heap(Material_Asset_Runtime* runtime, FILE* fptr,
                                     Heap_Allocator* allocator)
{
    fread(&runtime->version, sizeof(runtime->version), 1, fptr);

    runtime->asset->material_info.shader_name = allocator_heap_alloc(allocator, sizeof(String));
    runtime->asset->material_info.material_name = allocator_heap_alloc(allocator, sizeof(String));
    string_deserialize_heap(runtime->asset->material_info.shader_name, fptr, allocator);
    string_deserialize_heap(runtime->asset->material_info.material_name, fptr, allocator);
    fread(&runtime->asset->material_info.renderpass, sizeof(runtime->asset->material_info.renderpass), 1, fptr);
    fread(&runtime->asset->material_info.transluency, sizeof(runtime->asset->material_info.transluency), 1, fptr);
    fread(&runtime->asset->material_info.mesh_type, sizeof(runtime->asset->material_info.mesh_type), 1, fptr);
    fread(&runtime->asset->material_info.blend_mode, sizeof(runtime->asset->material_info.blend_mode), 1, fptr);
    fread(&runtime->asset->material_info.material_id, sizeof(runtime->asset->material_info.material_id), 1, fptr);

    runtime->asset->reflection_material_data = allocator_heap_alloc(allocator, sizeof(Reflection_Runtime_Struct));
    runtime->asset->material_gpu_definition = allocator_heap_alloc(allocator, sizeof(Material_GPU_Definition));
    reflection_registry_deserialize_runtime_struct_heap(runtime->asset->reflection_material_data, fptr, allocator);

    fread(&runtime->asset->material_gpu_definition->field_count, sizeof(u32), 1, fptr);
    fread(&runtime->asset->material_gpu_definition->struct_size, sizeof(u32), 1, fptr);

    runtime->asset->material_gpu_definition->name_hashes = allocator_heap_alloc(
        allocator, sizeof(u64) * runtime->asset->material_gpu_definition->field_count);
    runtime->asset->material_gpu_definition->field_offsets = allocator_heap_alloc(
        allocator, sizeof(u32) * runtime->asset->material_gpu_definition->field_count);
    runtime->asset->material_gpu_definition->types = allocator_heap_alloc(
        allocator, sizeof(Reflection_Type) * runtime->asset->material_gpu_definition->field_count);

    fread(runtime->asset->material_gpu_definition->name_hashes,
          sizeof(u64) * runtime->asset->material_gpu_definition->field_count, 1,
          fptr);
    fread(runtime->asset->material_gpu_definition->field_offsets,
          sizeof(u32) * runtime->asset->material_gpu_definition->field_count, 1,
          fptr);
    fread(runtime->asset->material_gpu_definition->types,
          sizeof(Reflection_Type) * runtime->asset->material_gpu_definition->field_count,
          1, fptr);


    return true;
}

MAPI bool asset_material_instance_serialize(Material_Instance* instance, FILE* fptr)
{
    fwrite(&instance->material_asset_uuid, sizeof(instance->material_asset_uuid), 1, fptr);
    fwrite(&instance->data_size, sizeof(instance->data_size), 1, fptr);
    fwrite(instance->material_data, instance->data_size, 1, fptr);
    return true;
}

bool asset_material_instance_deserialize(Material_Instance* instance, FILE* fptr, Allocator* allocator)
{
    fread(&instance->material_asset_uuid, sizeof(instance->material_asset_uuid), 1, fptr);
    fread(&instance->data_size, sizeof(instance->data_size), 1, fptr);
    instance->material_data = allocator_alloc(allocator, instance->data_size);
    fread(instance->material_data, instance->data_size, 1, fptr);

    return true;
}

MAPI bool asset_material_instance_deserialize_heap(Material_Instance* instance, FILE* fptr, Heap_Allocator* allocator)
{
    fread(&instance->material_asset_uuid, sizeof(instance->material_asset_uuid), 1, fptr);
    fread(&instance->data_size, sizeof(instance->data_size), 1, fptr);
    instance->material_data = allocator_heap_alloc(allocator, instance->data_size);
    fread(instance->material_data, instance->data_size, 1, fptr);

    return true;
}


bool asset_mesh_serialize(Madness_Mesh_Runtime* runtime, FILE* fptr)
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
        asset_material_instance_serialize(material_instance, fptr);
    }
    return true;
}

bool asset_mesh_deserialize(Madness_Mesh_Runtime* runtime, FILE* fptr, Allocator* allocator)
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
        asset_material_instance_deserialize(material_instance, fptr, allocator);
    }
    return true;
}

bool asset_mesh_deserialize_heap(Madness_Mesh_Runtime* runtime, FILE* fptr, Heap_Allocator* allocator)
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
        asset_material_instance_deserialize_heap(material_instance, fptr, allocator);
    }
    return true;
}

bool asset_skmesh_serialize(Madness_SkMesh_Runtime* runtime, FILE* fptr)
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
        asset_material_instance_serialize(material_instance, fptr);
    }

    //sk mesh data
    fwrite(runtime->skinned_submeshes, sizeof(Madness_Skinned_SubMesh) * runtime->mesh_count, 1, fptr);

    for (u32 i = 0; i < runtime->mesh_count; ++i)
    {
        Madness_Skinned_SubMesh* sk_sub_mesh = &runtime->skinned_submeshes[i];
        fwrite(runtime->skmesh_gpu_upload[i].joints, sk_sub_mesh->joint_bytes, 1, fptr);
        fwrite(runtime->skmesh_gpu_upload[i].weights, sk_sub_mesh->weight_bytes, 1, fptr);
    }

    //Animation Data

    fwrite(&runtime->animation_data->joint_count, sizeof(runtime->animation_data->joint_count), 1, fptr);
    fwrite(&runtime->animation_data->animations_count, sizeof(runtime->animation_data->animations_count), 1, fptr);

    for (u32 i = 0; i < runtime->animation_data->joint_count; i++)
    {
        Joint* joint = &runtime->animation_data->joints[i];
        fwrite(&joint->id, sizeof(joint->id), 1, fptr);
        fwrite(&joint->parent_idx, sizeof(joint->id), 1, fptr);
        string_serialize(joint->joint_name, fptr);
    }

    fwrite(&runtime->animation_data->resting_pose_local_matrix, sizeof(mat4s) * runtime->animation_data->joint_count, 1,
           fptr);
    fwrite(&runtime->animation_data->inverse_bind_matrix, sizeof(mat4s) * runtime->animation_data->joint_count, 1,
           fptr);


    for (u32 i = 0; i < runtime->animation_data->animations_count; i++)
    {
        Animation* animation = &runtime->animation_data->animations[i];

        fwrite(&animation->channel_count, sizeof(animation->channel_count), 1, fptr);
        fwrite(&animation->sampler_count, sizeof(animation->sampler_count), 1, fptr);
        fwrite(&animation->anim_start, sizeof(animation->anim_start), 1, fptr);
        fwrite(&animation->anim_end, sizeof(animation->anim_end), 1, fptr);
        string_serialize(animation->animation_name, fptr);


        fwrite(&animation->channels, sizeof(Animation_Channel) * animation->channel_count, 1, fptr);

        for (u32 sampler_idx = 0; sampler_idx < animation->channel_count; sampler_idx++)
        {
            Animation_Sampler* sampler = &animation->samplers[sampler_idx];
            fwrite(&sampler->timestamps_count, sizeof(sampler->timestamps_count), 1, fptr);
            fwrite(sampler->timestamps, sizeof(float) * sampler->timestamps_count, 1, fptr);
            fwrite(&sampler->sampler_start, sizeof(sampler->sampler_start), 1, fptr);
            fwrite(&sampler->sampler_end, sizeof(sampler->sampler_end), 1, fptr);
            fwrite(&sampler->trs_interpolation_bytes, sizeof(sampler->trs_interpolation_bytes), 1, fptr);
            fwrite(&sampler->interpolation_type, sizeof(sampler->interpolation_type), 1, fptr);
            fwrite(sampler->interperlation_data.trs_float, sampler->trs_interpolation_bytes, 1, fptr);
        }
    }
    return true;
}

bool asset_skmesh_deserialize(Madness_SkMesh_Runtime* runtime, FILE* fptr, Allocator* allocator)
{
    fread(&runtime->version, sizeof(runtime->version), 1, fptr);
    fread(&runtime->mesh_count, sizeof(runtime->mesh_count), 1, fptr);

    runtime->submeshes = allocator_alloc(allocator, sizeof(Madness_SubMesh) * runtime->mesh_count);
    runtime->mesh_gpu_upload = allocator_alloc(allocator, sizeof(Madness_Mesh_GPU_Data) * runtime->mesh_count);
    runtime->material_instance = allocator_alloc(allocator, sizeof(Material_Instance) * runtime->mesh_count);
    runtime->skinned_submeshes = allocator_alloc(allocator, sizeof(Madness_Skinned_SubMesh) * runtime->mesh_count);


    fread(runtime->submeshes, sizeof(Madness_SubMesh) * runtime->mesh_count, 1, fptr);
    //submesh contains the material uuid

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
        asset_material_instance_deserialize(material_instance, fptr, allocator);
    }


    //skinned mesh data
    fread(runtime->skinned_submeshes, sizeof(Madness_Skinned_SubMesh) * runtime->mesh_count, 1, fptr);

    for (u32 i = 0; i < runtime->mesh_count; ++i)
    {
        Madness_Skinned_SubMesh* sk_sub_mesh = &runtime->skinned_submeshes[i];
        Madness_SkMesh_GPU_Data* gpu_data = &runtime->skmesh_gpu_upload[i];
        gpu_data->joints = allocator_alloc(allocator, sk_sub_mesh->joint_bytes);
        gpu_data->weights = allocator_alloc(allocator, sk_sub_mesh->weight_bytes);
        fread(gpu_data->joints, sk_sub_mesh->joint_bytes, 1, fptr);
        fread(gpu_data->weights, sk_sub_mesh->weight_bytes, 1, fptr);
    }

    //Animation Data
    fread(&runtime->animation_data->joint_count, sizeof(runtime->animation_data->joint_count), 1, fptr);
    fread(&runtime->animation_data->animations_count, sizeof(runtime->animation_data->animations_count), 1, fptr);

    runtime->animation_data->joints = allocator_alloc(
        allocator, sizeof(Joint) * runtime->animation_data->joint_count);
    runtime->animation_data->animations = allocator_alloc(
        allocator, sizeof(Animation) * runtime->animation_data->animations_count);
    runtime->animation_data->resting_pose_local_matrix = allocator_alloc(
        allocator, sizeof(mat4s) * runtime->animation_data->joint_count);
    runtime->animation_data->inverse_bind_matrix = allocator_alloc(
        allocator, sizeof(mat4s) * runtime->animation_data->joint_count);

    for (u32 i = 0; i < runtime->animation_data->joint_count; i++)
    {
        Joint* joint = &runtime->animation_data->joints[i];
        fread(&joint->id, sizeof(joint->id), 1, fptr);
        fread(&joint->parent_idx, sizeof(joint->id), 1, fptr);
        string_deserialize(joint->joint_name, fptr, allocator);
    }


    fread(&runtime->animation_data->resting_pose_local_matrix, sizeof(mat4s) * runtime->animation_data->joint_count, 1,
          fptr);
    fread(&runtime->animation_data->inverse_bind_matrix, sizeof(mat4s) * runtime->animation_data->joint_count, 1,
          fptr);

    for (u32 i = 0; i < runtime->animation_data->animations_count; i++)
    {
        Animation* animation = &runtime->animation_data->animations[i];

        fread(&animation->channel_count, sizeof(animation->channel_count), 1, fptr);
        fread(&animation->sampler_count, sizeof(animation->sampler_count), 1, fptr);
        fread(&animation->anim_start, sizeof(animation->anim_start), 1, fptr);
        fread(&animation->anim_end, sizeof(animation->anim_end), 1, fptr);
        string_deserialize(animation->animation_name, fptr, allocator);

        animation->channels = allocator_alloc(allocator, sizeof(Animation_Channel) * animation->channel_count);
        animation->samplers = allocator_alloc(allocator, sizeof(Animation_Channel) * animation->sampler_count);

        fread(&animation->channels, sizeof(Animation_Channel) * animation->channel_count, 1, fptr);

        for (u32 sampler_idx = 0; sampler_idx < animation->channel_count; sampler_idx++)
        {
            Animation_Sampler* sampler = &animation->samplers[sampler_idx];
            fread(&sampler->timestamps_count, sizeof(sampler->timestamps_count), 1, fptr);

            sampler->timestamps = allocator_alloc(allocator, sizeof(float) * sampler->timestamps_count);
            fread(&sampler->timestamps, sizeof(float) * sampler->timestamps_count, 1, fptr);

            fread(&sampler->sampler_start, sizeof(sampler->sampler_start), 1, fptr);
            fread(&sampler->sampler_end, sizeof(sampler->sampler_end), 1, fptr);

            fread(&sampler->trs_interpolation_bytes, sizeof(sampler->trs_interpolation_bytes), 1, fptr);
            fread(&sampler->interpolation_type, sizeof(sampler->interpolation_type), 1, fptr);

            sampler->interperlation_data.trs_float = allocator_alloc(allocator, sampler->trs_interpolation_bytes);
            fread(&sampler->interperlation_data, sampler->trs_interpolation_bytes, 1, fptr);
        }
    }


    return true;
}

bool asset_skmesh_deserialize_heap(Madness_SkMesh_Runtime* runtime, FILE* fptr, Heap_Allocator* allocator)
{
    fread(&runtime->version, sizeof(runtime->version), 1, fptr);
    fread(&runtime->mesh_count, sizeof(runtime->mesh_count), 1, fptr);

    runtime->submeshes = allocator_heap_alloc(allocator, sizeof(Madness_SubMesh) * runtime->mesh_count);
    runtime->mesh_gpu_upload = allocator_heap_alloc(allocator, sizeof(Madness_Mesh_GPU_Data) * runtime->mesh_count);
    runtime->material_instance = allocator_heap_alloc(allocator, sizeof(Material_Instance) * runtime->mesh_count);
    runtime->skinned_submeshes = allocator_heap_alloc(allocator, sizeof(Madness_Skinned_SubMesh) * runtime->mesh_count);
    runtime->skmesh_gpu_upload = allocator_heap_alloc(allocator, sizeof(Madness_SkMesh_GPU_Data) * runtime->mesh_count);
    runtime->animation_data = allocator_heap_alloc(allocator, sizeof(GLTF_Animation_Data));


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
        asset_material_instance_deserialize_heap(material_instance, fptr, allocator);
    }


    //skinned mesh data
    fread(runtime->skinned_submeshes, sizeof(Madness_Skinned_SubMesh) * runtime->mesh_count, 1, fptr);

    for (u32 i = 0; i < runtime->mesh_count; ++i)
    {
        Madness_Skinned_SubMesh* sk_sub_mesh = &runtime->skinned_submeshes[i];
        Madness_SkMesh_GPU_Data* gpu_data = &runtime->skmesh_gpu_upload[i];
        gpu_data->joints = allocator_heap_alloc(allocator, sk_sub_mesh->joint_bytes);
        gpu_data->weights = allocator_heap_alloc(allocator, sk_sub_mesh->weight_bytes);
        fread(gpu_data->joints, sk_sub_mesh->joint_bytes, 1, fptr);
        fread(gpu_data->weights, sk_sub_mesh->weight_bytes, 1, fptr);
    }

    //Animation Data
    fread(&runtime->animation_data->joint_count, sizeof(runtime->animation_data->joint_count), 1, fptr);
    fread(&runtime->animation_data->animations_count, sizeof(runtime->animation_data->animations_count), 1, fptr);

    runtime->animation_data->joints = allocator_heap_alloc(
        allocator, sizeof(Joint) * runtime->animation_data->joint_count);
    runtime->animation_data->animations = allocator_heap_alloc(
        allocator, sizeof(Animation) * runtime->animation_data->animations_count);
    runtime->animation_data->resting_pose_local_matrix = allocator_heap_alloc(
        allocator, sizeof(mat4s) * runtime->animation_data->joint_count);
    runtime->animation_data->inverse_bind_matrix = allocator_heap_alloc(
        allocator, sizeof(mat4s) * runtime->animation_data->joint_count);

    for (u32 i = 0; i < runtime->animation_data->joint_count; i++)
    {
        Joint* joint = &runtime->animation_data->joints[i];
        fread(&joint->id, sizeof(joint->id), 1, fptr);
        fread(&joint->parent_idx, sizeof(joint->id), 1, fptr);
        joint->joint_name = allocator_heap_alloc(allocator, sizeof(String));
        string_deserialize_heap(joint->joint_name, fptr, allocator);
    }


    fread(&runtime->animation_data->resting_pose_local_matrix, sizeof(mat4s) * runtime->animation_data->joint_count, 1,
          fptr);
    fread(&runtime->animation_data->inverse_bind_matrix, sizeof(mat4s) * runtime->animation_data->joint_count, 1,
          fptr);

    for (u32 i = 0; i < runtime->animation_data->animations_count; i++)
    {
        Animation* animation = &runtime->animation_data->animations[i];

        fread(&animation->channel_count, sizeof(animation->channel_count), 1, fptr);
        fread(&animation->sampler_count, sizeof(animation->sampler_count), 1, fptr);
        fread(&animation->anim_start, sizeof(animation->anim_start), 1, fptr);
        fread(&animation->anim_end, sizeof(animation->anim_end), 1, fptr);
        string_deserialize_heap(animation->animation_name, fptr, allocator);

        animation->channels = allocator_heap_alloc(allocator, sizeof(Animation_Channel) * animation->channel_count);
        animation->samplers = allocator_heap_alloc(allocator, sizeof(Animation_Channel) * animation->sampler_count);

        fread(&animation->channels, sizeof(Animation_Channel) * animation->channel_count, 1, fptr);

        for (u32 sampler_idx = 0; sampler_idx < animation->channel_count; sampler_idx++)
        {
            Animation_Sampler* sampler = &animation->samplers[sampler_idx];
            fread(&sampler->timestamps_count, sizeof(sampler->timestamps_count), 1, fptr);

            sampler->timestamps = allocator_heap_alloc(allocator, sizeof(float) * sampler->timestamps_count);
            fread(sampler->timestamps, sizeof(float) * sampler->timestamps_count, 1, fptr);

            fread(&sampler->sampler_start, sizeof(sampler->sampler_start), 1, fptr);
            fread(&sampler->sampler_end, sizeof(sampler->sampler_end), 1, fptr);

            fread(&sampler->trs_interpolation_bytes, sizeof(sampler->trs_interpolation_bytes), 1, fptr);
            fread(&sampler->interpolation_type, sizeof(sampler->interpolation_type), 1, fptr);

            sampler->interperlation_data.trs_float = allocator_heap_alloc(allocator, sampler->trs_interpolation_bytes);
            fread(sampler->interperlation_data.trs_float, sampler->trs_interpolation_bytes, 1, fptr);
        }
    }


    return true;
}
