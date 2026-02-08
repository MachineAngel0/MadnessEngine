#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_scalar_block_layout: require
#extension GL_EXT_buffer_reference : require


layout(push_constant, scalar) uniform PC_MESH{
    uint ubo_buffer_idx;
    uint normal_buffer_idx;
    uint tangent_buffer_idx;
    uint uv_buffer_idx;
    uint transform_buffer_idx;
    uint material_buffer_idx;
    uint _padding;
    uint _padding1;
}pc_mesh;


struct Material_Param_Data {
    uint feature_mask;

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

layout (buffer_reference, scalar) readonly buffer MaterialsDataBuffer {
    Material_Param_Data material_data[];
} Material_Data;


// binding 0 stores our textures
// binding 1 stores our params and indexes into the descriptor
layout (set = 1, binding = 0) uniform sampler2D texture_samples[];
layout (set = 1, binding = 0) uniform texture2D textures[];
layout (set = 1, binding = 0) uniform texture2DMS texturesMS[];
layout (set = 1, binding = 0) uniform textureCube textureCubes[];
layout (set = 1, binding = 0) uniform texture2DArray textureArrays[];
//layout (set = 1, binding = 1) uniform sampler samplers[];

layout(set = 2, binding = 0, scalar) readonly buffer UV_BUFFER{
    vec2 uv[];
}UV[];

layout(set = 2, binding = 0, scalar) readonly buffer NORMAL_BUFFER{
    vec3 normal[];
}NORMAL[];

layout(set = 2, binding = 0, scalar) readonly buffer MATERIAL_BUFFER{
    Material_Param_Data material_data[];
}MATERIAL[];