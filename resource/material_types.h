#ifndef MATERIAL_TYPES_H
#define MATERIAL_TYPES_H


#include "defines.h"
#include "math_types.h"


typedef enum Pipeline_Type
{
    PIPELINE_TYPE_MESH_BPR_OPAQUE,
    PIPELINE_TYPE_MESH_BPR_TRANSPARENCY,
    PIPELINE_TYPE_SHADOW,
    // PIPELINE_TYPE_TERRAIN,
} Pipeline_Type;


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
} Material_PBR;


typedef struct Material_UV_Anim_Data
{
    vec2 offset;
    float speed;
} Material_UV_Anim_Data;

typedef struct Material_Instance
{
    Material_Flag flags;

    u32 pbr_idx;
    u32 uv_anim_idx;
    u32 blach_hole_anim_idx;

} Material_Instance;

#define MAX_MATERIAL 100

typedef struct Material_System
{
    u32 internal_count;
    Material_Instance material_instance[MAX_MATERIAL];

    Material_PBR prb[MAX_MATERIAL]; // pretty much manditory for all meshes
    u32 pbr_count;
    Material_UV_Anim_Data uv_anim[MAX_MATERIAL];
    u32 uv_anim_count;

} Material_System;




#endif //MATERIAL_TYPES_H
