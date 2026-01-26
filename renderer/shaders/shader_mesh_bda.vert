#version 450

// required for including other shader files
#extension GL_GOOGLE_include_directive : require

#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_scalar_block_layout: require
#extension GL_EXT_buffer_reference : require


#include "shader_includes/test_uniform.glsl"

//layout(std140, set = 0, binding = 0) uniform UniformBufferObject{
//    mat4 model;
//    mat4 view;
//    mat4 proj;
//} ubo[];


layout (buffer_reference, scalar) readonly buffer PositionBuffer {
    vec3 position[];
};
layout (buffer_reference, scalar) readonly buffer NormalBuffer {
    vec3 normal[];
};
layout (buffer_reference, scalar) readonly buffer TangentBuffer {
    vec4 tangent[];
};
layout (buffer_reference, scalar) readonly buffer UVBuffer {
    vec2 uv[];
};


layout (push_constant, scalar) uniform push_constants
{
    PositionBuffer position_buffer;
    NormalBuffer normal_buffer;
    TangentBuffer tangent_buffer;
    UVBuffer uv_buffer;

    uint albedo_idx;
    uint padding;
} pc;


layout(location = 0) out vec3 out_normal;
layout(location = 1) out vec4 out_tangent;
layout(location = 2) out vec2 out_uv;

void main() {

    uint idx = gl_VertexIndex;
    vec3 in_pos = pc.position_buffer.position[idx];
    gl_Position = ubo[nonuniformEXT(0)].proj * ubo[nonuniformEXT(0)].view * ubo[nonuniformEXT(0)].model * vec4(in_pos, 1.0);

    out_normal = pc.normal_buffer.normal[idx];
    out_tangent = pc.tangent_buffer.tangent[idx];

    out_uv = pc.uv_buffer.uv[idx];

}