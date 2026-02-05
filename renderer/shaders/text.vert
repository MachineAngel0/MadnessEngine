#version 450

#extension GL_GOOGLE_include_directive : require

#include "shader_includes/test_uniform.glsl"


layout(location = 0) in vec2 in_pos;
layout(location = 1) in vec3 in_color;
layout(location = 2) in vec2 in_uv;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {
    gl_Position = vec4(in_pos, 0.0, 1.0);
    fragColor = in_color;
    fragTexCoord = in_uv;
}