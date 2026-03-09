#version 450

#extension GL_GOOGLE_include_directive : require
#include "shader_includes/2d_structs.glsl"
#include "shader_includes/test_uniform.glsl"

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 intex;

layout(location = 0) out vec3 fragColor;
//layout(location = 1) out vec2 out_uv;


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
    //gl_Position = (inPosition, 1.0, 1.0);
    fragColor = inst_data.color;

    if((inst_data.flags & SPRITE_PIPELINE_TEXTURE) != 0){

        float left = inst_data.uv_offset.x;
        float top = inst_data.uv_offset.y;
        float right = inst_data.uv_offset.x + inst_data.uv_size.x;
        float bottom = inst_data.uv_offset.y+ inst_data.uv_size.y;

        vec2 uvs[4]=
        {
        vec2(left, top),
        vec2(left, bottom),
        vec2(right, bottom),
        vec2(right, top),
        };

//        out_uv = uvs[gl_VertexIndex]; // todo
    }
}