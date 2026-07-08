#ifndef MATERIAL_TYPES_H
#define MATERIAL_TYPES_H


#include "defines.h"
#include "math_types.h"



typedef struct Draw_Data
{
    u32 transform_index;
    u32 material_index;
} Draw_Data;

typedef struct PC_Mesh
{
    //per instance data change
    u32 ubo_buffer_idx;
} PC_Mesh;

typedef struct PC_Skinned_Mesh
{
    //per instance data change
    u32 ubo_buffer_idx;
    u32 _padding;

    // buffer device addresses
    VkDeviceAddress vertex_buffer;
    // VkDeviceAddress index_buffer;
    VkDeviceAddress normal_buffer;
    VkDeviceAddress tangent_buffer;
    VkDeviceAddress uv_buffer;
    VkDeviceAddress joint_buffer;
    VkDeviceAddress weight_buffer;
    VkDeviceAddress skinned_matrix_buffer;

    VkDeviceAddress transform_buffer;
    VkDeviceAddress material_buffer;
    VkDeviceAddress skinned_draw_data_buffer;


    //128 byte limit
} PC_Skinned_Mesh;

typedef struct PC_Material_Mesh
{
    u32 ubo_buffer_idx;
    PC_Mesh pc_mesh;
    VkDeviceAddress material_buffer;
} PC_Material_Mesh;


typedef enum Shader_Pass_Type
{
    PIPELINE_TYPE_MESH_PBR_OPAQUE,
    PIPELINE_TYPE_MESH_PBR_TRANSPARENCY,
    PIPELINE_TYPE_SHADOW,
    // PIPELINE_TYPE_TERRAIN,
} Shader_Pass_Type;


typedef enum Material_Flag
{
    MATERIAL_FLAG_PBR = BITFLAG(0),
    MATERIAL_FLAG_UV_ANIM = BITFLAG(1),
} Material_Flag;

typedef enum Mesh_PBR_Flags
{
    MESH_PIPELINE_COLOR = BITFLAG(1),
    MESH_PIPELINE_NORMAL = BITFLAG(2),
    MESH_PIPELINE_EMISSIVE = BITFLAG(3),
    MESH_PIPELINE_ROUGHNESS = BITFLAG(4),
    MESH_PIPELINE_METALLIC = BITFLAG(5),
    MESH_PIPELINE_AO = BITFLAG(6),
    MESH_PIPELINE_ENUM_MAX,
} Mesh_PBR_Flags;


typedef struct Material_PBR
{
    u32 flags;

    vec4 color; // this will be at a default of 1.0, which is white but won't affect the material
    //ALL FROM RANGES 0-1
    float ambient_strength; // optional for now we can remove it later
    float roughness_strength;
    float metallic_strength;
    float normal_strength;
    float ambient_occlusion_strength;
    float emissive_strength;

    u32 color_index;
    u32 normal_index;
    u32 metallic_index;
    u32 roughness_index;
    u32 ambient_occlusion_index;
    u32 emissive_index;
    u32 _padding0;
    u32 _padding1;
    u32 _padding2;
} Material_Default;




#define MAX_MATERIAL 100

typedef struct Material_System
{


    //Hardcode pipelines
    Material_Default prb[MAX_MATERIAL]; // pretty much manditory for all meshes
    u32 pbr_count;


    //material data should get passed in through a push constant

    //Shaders/Pipelines should always be first, they define what the material needs are
    //or we could have material in one spot, that get generated
    /*struct mat_definition{
        name* (float, u32, mat4) etc... (for the prototype, or base version)
        type* (float, u32, mat4) etc...
        void* data;
     }

     //shader_pass -> hash_map(idx or name) -> array of materials/mat instances
     //shader_pass -> hash_map(idx or name) -> material defintion


     mat_info{
        mat_def* mat_def_array;
     }

     //pipeline/shader info
     {
        pipeline*

        //passes data
        opaque/transparent
        has_shadow
        blend_mode

        mat_def*
        cpu_material_array_data* // void* data
        storage_buffer*

        push_constant_data* // only one needed per pipeline
        push_constant_size*
     }


     */

    //Generated pipelines

    //each material has a u32 idx, which we assign at runtime
    //each mateiral will have its own pipeline with its own material


} Material_System;



typedef struct Material_System2
{

    //Hardcode pipelines
    Material_Default prb[MAX_MATERIAL]; // pretty much manditory for all meshes
    u32 pbr_count;

    //generated materials, that get auto managaed



} Material_System2;


#endif //MATERIAL_TYPES_H
