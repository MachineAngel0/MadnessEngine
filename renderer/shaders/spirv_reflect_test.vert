#version 450

layout(std140, set = 6, binding = 6) uniform RAY{
    vec3 ran;
} r;

layout(set = 5, binding = 1) uniform sampler2D texSampler;


layout(std140, set = 0, binding = 0) uniform UniformBufferObject{
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;


layout(std140, set = 1, binding = 0) uniform Blah{
    vec3 model;
    mat4 view;
    mat4 proj;
} blah;

layout(push_constant) uniform Push{
    vec2 offset;
    vec3 color;
}push;



layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTex;

layout(location = 0) out vec3 outColor;
layout(location = 1) out vec2 outTex;

void main() {
}