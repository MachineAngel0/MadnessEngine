#version 450

#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_scalar_block_layout: require
#extension GL_EXT_buffer_reference : require

layout(std140, set = 0, binding = 0) uniform UniformBufferObject{
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo[];


layout (buffer_reference, scalar) readonly buffer PositionBuffer {
    vec3 position[];
};

layout (buffer_reference, scalar) readonly buffer TexBuffer {
    vec2 tex_coord[];
};



layout (push_constant, scalar) uniform push_constants
{
    PositionBuffer position_buffer;
    TexBuffer tex_buffer;
//    int index;
} pc;




//layout(location = 0) in vec3 in_position;
//layout(location = 1) in vec3 in_normal;
//layout(location = 2) in vec4 in_tangent;
//layout(location = 3) in vec2 in_tex;

layout(location = 0) out vec3 out_normal;
layout(location = 1) out vec4 out_tangent;
layout(location = 2) out vec2 out_tex;

void main() {

//    MatrixReference model_data = push_constant.model;
    uint idx = gl_VertexIndex;
    vec3 in_pos = pc.position_buffer.position[idx];
    gl_Position = ubo[nonuniformEXT(0)].proj * ubo[nonuniformEXT(0)].view * ubo[nonuniformEXT(0)].model * vec4(in_pos, 1.0);

    out_normal = vec3(1,1,1);
    out_tangent = vec4(1,1,1,1);
//    out_tex = vec2(0.5,0.5);
    out_tex = pc.tex_buffer.tex_coord[idx];
}