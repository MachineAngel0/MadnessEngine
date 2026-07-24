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

typedef struct Material_Default
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

    MADNESS_UUID color_texture;
    MADNESS_UUID normal_texture;
    MADNESS_UUID metallic_texture;
    MADNESS_UUID roughness_texture;
    MADNESS_UUID ambient_occlusion_texture;
    MADNESS_UUID emissive_texture;
    u32 padding1;
    u32 padding2;

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

    u32 padding1;
    u32 padding2;
} Material_Default_CPU;




//this is mostly a test material
typedef struct Material_Flat_Color
{
    vec4 color;
} Material_Flat_Color;

typedef struct Material_Spherical_Billboard
{
    vec3s point;
    u32 texture_idx;
    vec2s size; //also a radius
    vec2s rotation;
    // vec4 color; //tint basically
} Material_Spherical_Billboard;


#endif //MATERIAL_TYPES_H
