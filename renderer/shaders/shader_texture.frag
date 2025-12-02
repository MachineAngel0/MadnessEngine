#version 450

layout(set = 0, binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 inColor;
layout(location = 1) in vec2 inTex;


layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(inColor, 1.0) * texture(texSampler, inTex); // if we want colors overlayed
    outColor = texture(texSampler, inTex);
}