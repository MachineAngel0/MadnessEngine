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


struct Joint{
    mat4 joint[4];
};

struct Weight{
    mat4 weight[4];
};

struct skinned_draw_data{
    uint joint_index;
    uint weight_index;
};

layout(buffer_reference, scalar) readonly buffer Vertex_Buffer{
    vec3 vertex_data[];
};
layout(buffer_reference, scalar) readonly buffer UV_Buffer{
    vec2 uv_data[];
};
layout(buffer_reference, scalar) readonly buffer Normal_Buffer{
    vec3 normal_data[];
};
layout(buffer_reference, scalar) readonly buffer Tangent_Buffer{
    vec4 tangent_data[];
};
layout(buffer_reference, scalar) readonly buffer Transform_Buffer{
    mat4 transform_data[];
//model transform to be specific
};
layout(buffer_reference, scalar) readonly buffer Joint_Buffer{
    vec4 joint_data[];
};
layout(buffer_reference, scalar) readonly buffer Weight_Buffer{
    vec4 weights_data[];
};
layout(buffer_reference, scalar) readonly buffer Skinned_Matrix_Buffer{
    mat4 skinned_matrix_data[];
};




struct mesh_draw_data{
    uint transform_idx;
    uint material_idx;
};


struct Skinned_Mesh_Draw_Data{
    uint joint_idx;
    uint weight_idx;
    uint material_idx;
    uint transform_idx;
};

layout(buffer_reference, scalar) readonly buffer Skinned_Mesh_Draw_Data_Buffer{
    Skinned_Mesh_Draw_Data skinned_draw_data[];
};



layout(set = 2, binding = 0, scalar) readonly buffer MESH_DRAW_DATA_BUFFER{
    mesh_draw_data mesh_data[];
}MESH_DRAW_DATA[];










