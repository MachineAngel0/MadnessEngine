#version 450

#extension GL_EXT_buffer_reference : require
#extension GL_EXT_scalar_block_layout : require
#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_nonuniform_qualifier : require

#include "shader_includes/test_uniform.glsl"
#include "shader_includes/cui_structs.glsl"

//sdfs
float sdRoundBox(in vec2 p, in vec2 b, in vec4 r)
{
    //TODO: you should really comment this at asome point
    r.xy = (p.x > 0.0)?r.xy : r.zw;
    r.x = (p.y > 0.0)?r.x  : r.y;
    vec2 q = abs(p) - b + r.x;
    return min(max(q.x, q.y), 0.0) + length(max(q, 0.0)) - r.x;
}




layout(location = 0) in vec4 in_color;
layout(location = 1) in vec2 in_local_pos;
layout(location = 2) in vec2 out_uv;
layout(location = 3) in flat uint in_instance_idx;
layout(location = 4) in flat uint out_texture_idx;


layout(location = 0) out vec4 outColor;




void main() {
    CUI_Data cui_data = PC_CUI.cui_ssbo.cui_data[in_instance_idx];

//    vec2 screen_dimensions = ubo.screen_dimensions;


    outColor = vec4(in_color.rgb, in_color.a * 1.f);


}