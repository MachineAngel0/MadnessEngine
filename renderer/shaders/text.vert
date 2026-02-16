#version 450

#extension GL_GOOGLE_include_directive : require
#extension GL_ARB_shader_draw_parameters : enable

#include "shader_includes/test_uniform.glsl"
#include "shader_includes/2d_structs.glsl"


layout(location = 0) in vec2 in_pos;
layout(location = 1) in vec3 in_color;
layout(location = 2) in vec2 in_uv;

layout(location = 0) out vec3 out_color;
layout(location = 1) out vec2 out_uv;
layout(location = 2) out vec2 out_tex;
layout(location = 3) out flat uint out_texture_idx;

void main() {

    vec2 ndc =  vec2(in_pos) * 2.0 - 1.0;
    gl_Position = vec4(ndc, 0.0, 1.0);
    out_color = in_color;
    out_uv = in_uv;

    uint draw_other_idx = gl_DrawIDARB;

    out_texture_idx = MATERIAL_2D[nonuniformEXT(PC_2D.material_buffer_idx)].material_data[nonuniformEXT(gl_DrawIDARB)].texture_index;
//    out_material_idx = 1;


}