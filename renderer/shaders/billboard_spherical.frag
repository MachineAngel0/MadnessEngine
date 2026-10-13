#version 450

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_nonuniform_qualifier : require

#include "shader_includes/test_uniform.glsl"
#include "shader_includes/2d_structs.glsl"




layout(location = 0) in vec4 in_color;
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


    texture_result *= vec4(in_color);


    outColor = texture_result;
}