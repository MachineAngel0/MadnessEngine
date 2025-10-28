#version 450


// Simple conversion in vertex shader
layout(push_constant) uniform PushConstants {
    vec2 screenSize; // e.g., (1920, 1080)
} pc;


layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 incolor;

layout(location = 0) out vec3 fragColor;

void main() {
    vec2 ndc =  vec2(inPosition / pc.screenSize) * 2.0 - 1.0;
    gl_Position = vec4(ndc, 0.0, 1.0);
    //gl_Position = (inPosition, 1.0, 1.0);
    fragColor = incolor;
}