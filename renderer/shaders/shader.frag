#version 450

#extension GL_EXT_nonuniform_qualifier : require
layout (set = 1, binding = 0) uniform sampler2D texture_samples[];


layout(location = 0) in vec3 inColor;
//layout(location = 1) in vec3 inTex;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(inColor, 1.0);
//    outColor = texture(texture_samples[(nonuniformEXT(0))], inTex);

}