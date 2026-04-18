#version 450

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_nonuniform_qualifier : require

#include "shader_includes/test_uniform.glsl"
#include "shader_includes/2d_structs.glsl"



layout(location = 0) in vec3 in_color;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in flat uint in_texture_idx;


layout(location = 0) out vec4 outColor;

void main() {

    outColor = vec4(in_color, 1.0f) * texture(texture_samples[(nonuniformEXT(in_texture_idx))], in_uv); // if we want colors overlayed



}