#ifndef MATERIAL_TYPES_H
#define MATERIAL_TYPES_H


#include "defines.h"
#include "math_types.h"
#include "UUID.h"

#define MATERIAL_DEFAULT_NAME "Material_Default"
#define MESH_DEFAULT_SHADER "mesh"
#define SKINNED_MESH_DEFAULT_SHADER "skinned_mesh"

// MADNESS_UUID these are texture id's which get converted into u32 for bindless at runtime
// NOTE: MADNESS_UUID must always be last in the structure

typedef u32 Permutation_Mask;

typedef enum Permutation_PBR
{
    PBR_PIPELINE_SKINNED = BITFLAG(0), // will save me a lot of headache by using one shader for this
    PBR_PIPELINE_COLOR = BITFLAG(1),
    PBR_PIPELINE_NORMAL = BITFLAG(2),
    PBR_PIPELINE_EMISSIVE = BITFLAG(3),
    PBR_PIPELINE_ROUGHNESS = BITFLAG(4),
    PBR_PIPELINE_METALLIC = BITFLAG(5),
    PBR_PIPELINE_AO = BITFLAG(6),
    PBR_PIPELINE_ENUM_MAX,
} Permutation_PBR;


typedef struct Material_Default
{
    Permutation_PBR flags;

    vec4s color; // this will be at a default of 1.0, which is white but won't affect the material
    //ALL FROM RANGES 0-1
    float ambient_strength; // optional for now we can remove it later
    float roughness_strength;
    float metallic_strength;
    float normal_strength;
    float ambient_occlusion_strength;
    float emissive_strength;

    u32 padding0;

    MADNESS_UUID color_texture;
    MADNESS_UUID normal_texture;
    MADNESS_UUID metallic_texture;
    MADNESS_UUID roughness_texture;
    MADNESS_UUID ambient_occlusion_texture;
    MADNESS_UUID emissive_texture;
} Material_Default;

typedef struct Material_Default_CPU
{
    u32 flags;

    vec4s color; // this will be at a default of 1.0, which is white but won't affect the material
    //ALL FROM RANGES 0-1
    float ambient_strength; // optional for now we can remove it later
    float roughness_strength;
    float metallic_strength;
    float normal_strength;
    float ambient_occlusion_strength;
    float emissive_strength;

    u32 padding0;


    u32 color_texture;
    u32 normal_texture;
    u32 metallic_texture;
    u32 roughness_texture;
    u32 ambient_occlusion_texture;
    u32 emissive_texture;
} Material_Default_CPU;


typedef struct Material_Black_Hole
{
    float distortion;
    float speed;
    float size;
    u32 padding;
} Material_Black_Hole;

//this is a test material
typedef struct Material_Flat_Color
{
    vec4 color;
    Path_String* color_texture;
} Material_Flat_Color;

typedef struct Material_Spherical_Billboard
{
    u32 texture_idx;
    vec2 uv_offset;
    vec2 uv_size;
} Material_Spherical_Billboard;

typedef struct Material_Spherical_Billboard_CPU
{
    MADNESS_UUID texture_idx;
    vec2 uv_offset;
    vec2 uv_size;
} Material_Spherical_Billboard_CPU;

#endif //MATERIAL_TYPES_H
