#version 450



// Bindless support
// Enable non uniform qualifier extension
#extension GL_EXT_nonuniform_qualifier : require
layout (set = 1, binding = 0) uniform sampler2D texture_samples[];



layout(location = 0) in vec3 inColor;
layout(location = 1) in vec2 inTex;


layout(location = 0) out vec4 outColor;

void main() {
//    outColor = vec4(inColor, 1.0) * texture(texture_samples[0], inTex); // if we want colors overlayed
    outColor = texture(texture_samples[(nonuniformEXT(0))], inTex);
//    outColor = vec4(inColor, 1.0);
}