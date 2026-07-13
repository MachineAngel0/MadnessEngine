#ifndef MATERIAL_TYPES_H
#define MATERIAL_TYPES_H


#include "defines.h"
#include "math_types.h"



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

    u32 color_index;
    u32 normal_index;
    u32 metallic_index;
    u32 roughness_index;
    u32 ambient_occlusion_index;
    u32 emissive_index;
    u32 padding0;
    u32 padding1;
    u32 padding2;
} Material_Default;


//this is mostly a test material
/* TODO: reflection system has no concept of a vec2-4, and likely the same for mat3/4
typedef struct Material_Flat_Color
{
    vec4 color;
} Material_Flat_Color;
*/


#endif //MATERIAL_TYPES_H
