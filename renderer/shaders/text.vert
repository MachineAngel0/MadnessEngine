#version 450

#extension GL_GOOGLE_include_directive : require
#extension GL_ARB_shader_draw_parameters : enable

#include "shader_includes/test_uniform.glsl"
#include "shader_includes/2d_structs.glsl"


layout(location = 0) in vec2 in_pos;

layout(location = 0) out vec3 out_color;
layout(location = 1) out vec2 out_uv;
layout(location = 2) out flat uint out_texture_idx;

void main() {

    uint sprite_instance_buffer_idx = PC_2D.instance_buffer_idx;

    Sprite_Data inst_data =
    Sprite_Instance_Buffer[nonuniformEXT(sprite_instance_buffer_idx)].sprite_instance_data[nonuniformEXT(gl_InstanceIndex)];

    vec2 vertices[6] =
    {
    inst_data.pos,
    vec2(inst_data.pos + vec2(0.0, inst_data.size.y)),
    vec2(inst_data.pos + vec2(inst_data.size.x, inst_data.size.y)),
    vec2(inst_data.pos + vec2(inst_data.size.x, 0.0)),
    vec2(inst_data.pos + vec2(0.0, inst_data.size.y)),
    vec2(inst_data.pos + inst_data.size),
    };



    vec2 ndc = vec2(vertices[gl_VertexIndex]) * 2.0 - 1.0;
    gl_Position = vec4(ndc, 0.0, 1.0);


    out_color = inst_data.color;

    out_texture_idx = inst_data.texture_index;


    float left = inst_data.uv_offset.x;
    float top = inst_data.uv_offset.y;
    float right = inst_data.uv_offset.x + inst_data.uv_size.x;
    float bottom = inst_data.uv_offset.y+ inst_data.uv_size.y;

    //only 4 since this is indexed
    vec2 uvs[4]=
    {
    vec2(left, top),
    vec2(left, bottom),
    vec2(right, bottom),
    vec2(right, top),
    };

    out_uv = uvs[gl_VertexIndex];


}