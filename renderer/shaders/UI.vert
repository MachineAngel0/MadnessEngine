#version 450

#extension GL_GOOGLE_include_directive : require
#include "shader_includes/test_uniform.glsl"

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 incolor;

layout(location = 0) out vec3 fragColor;

void main() {
    //TODO: remove test
    vec2 ndc =  vec2(inPosition) * 2.0 - 1.0;

    gl_Position = vec4(ndc, 0.0, 1.0);
    //gl_Position = (inPosition, 1.0, 1.0);
    fragColor = incolor;
}