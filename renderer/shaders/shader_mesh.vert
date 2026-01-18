#version 450

#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_scalar_block_layout: require
#extension GL_EXT_buffer_reference : require

layout(std140, set = 0, binding = 0) uniform UniformBufferObject{
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo[];

layout (buffer_reference, scalar) readonly buffer MatrixReference {
    mat4 matrix;
};

layout (push_constant) uniform PushConstants
{
// Pointer to the buffer with the scene's MVP matrix
    MatrixReference sceneDataReference;
// Pointer to the buffer for the data for each model
    MatrixReference modelDataReference;
} pushConstants;


layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec4 in_tangent;
layout(location = 3) in vec2 in_tex;

layout(location = 0) out vec3 out_normal;
layout(location = 1) out vec4 out_tangent;
layout(location = 2) out vec2 out_tex;

void main() {
    gl_Position = ubo[nonuniformEXT(0)].proj * ubo[nonuniformEXT(0)].view * ubo[nonuniformEXT(0)].model * vec4(in_position, 1.0);

    out_normal = in_normal;
    out_tangent = in_tangent;
    out_tex = in_tex;
}