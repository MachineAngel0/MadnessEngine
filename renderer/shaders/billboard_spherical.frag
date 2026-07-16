#version 450

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_nonuniform_qualifier : require

#include "shader_includes/test_uniform.glsl"
#include "shader_includes/2d_structs.glsl"


struct Spherical_Billboard{
    vec3 point;
    uint texture_idx;
    vec2 size; //also a radius
    vec2 rotation;
//    vec4 color; //tint basically
};

layout(buffer_reference, scalar) readonly buffer Spherical_Billboard_Buffer{
    Spherical_Billboard data[];
};

struct PC_Particle{
    Spherical_Billboard_Buffer material_buffer;
    Spherical_Billboard_Buffer unused;
};



layout(location = 0) in vec3 in_color;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in flat Spherical_Billboard material_data;



layout(location = 0) out vec4 outColor;



void main() {
    vec4 texture_result = texture(texture_samples[(nonuniformEXT(material_data.texture_idx))], in_uv);

    outColor = vec4(texture_result);
}