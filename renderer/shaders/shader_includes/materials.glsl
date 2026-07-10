#extension GL_EXT_buffer_reference : require
#extension GL_EXT_scalar_block_layout: require

#include "macros.glsl"


struct Material_Instance{
    uint flags;

    uint pbr_idx;
    uint uv_anim_idx;
    uint black_hole_idx;
};

layout(set = 2, binding = 0, scalar) readonly buffer MATERIAL_INSTANCE_BUFFER{
    Material_Instance material_instance[];
}MATERIAL_INSTANCE[];

#define MESH_PIPELINE_COLOR  BITFLAG(1)
#define MESH_PIPELINE_NORMAL  BITFLAG(2)
#define MESH_PIPELINE_EMISSIVE  BITFLAG(3)
#define MESH_PIPELINE_ROUGHNESS  BITFLAG(4)
#define MESH_PIPELINE_METALLIC  BITFLAG(5)
#define MESH_PIPELINE_AO  BITFLAG(6)




struct Pbr{
    uint flags;

    vec4 color;
//ALL FROM RANGES 0-1
    float ambient_strength;
    float roughness_strength;
    float metallic_strength;
    float normal_strength;
    float ambient_occlusion_strength;
    float emissive_strength;

    uint color_index;
    uint normal_index;
    uint metallic_index;
    uint roughness_index;
    uint ambient_occlusion_index;
    uint emissive_index;
    uint _padding0;
    uint _padding1;
    uint _padding2;
};



layout(set = 2, binding = 0, scalar) readonly buffer PBR_BUFFER{
    Pbr pbr[];
}PBR[];

layout(buffer_reference, scalar) readonly buffer PRB_Buffer{
    Pbr pbr_data[];
};













