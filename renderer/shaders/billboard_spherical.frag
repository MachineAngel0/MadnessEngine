#version 450

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_nonuniform_qualifier : require

#include "shader_includes/test_uniform.glsl"
#include "shader_includes/2d_structs.glsl"


struct Spherical_Billboard{
    vec3 point;
    uint texture_idx;
    vec2 size;//also a radius
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
layout(location = 2) in flat uint in_texture_idx;



layout(location = 0) out vec4 outColor;



void main() {

    vec4 texture_result = texture(texture_samples[(nonuniformEXT(in_texture_idx))], in_uv);

//    texture_result.xyz *= texture_result.a;

    //uses the alpha, which doens't always work
    /*    if(texture_result.a < 0.01)
            { discard; }*/


//    float threshold = 0.1;
//
//    if (length(texture_result.rgb) < threshold)
//    {
//        discard;
//    }

//        float alpha = smoothstep(
//        0.0,
//        0.1,
//        length(texture_result.rgb)
//        );
//        texture_result.a *= alpha;


    texture_result *= vec4(in_color, 1.0);


    outColor = texture_result;
}