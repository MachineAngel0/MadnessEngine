#version 450

#extension GL_GOOGLE_include_directive : require
#extension GL_ARB_shader_draw_parameters : enable

#include "shader_includes/2d_structs.glsl"
#include "shader_includes/test_uniform.glsl"

layout(location = 0) out vec3 out_color;
layout(location = 1) out vec2 out_uv;
layout(location = 2) out vec2 out_tex;
layout(location = 3) out flat uint out_texture_idx;
layout(location = 4) out flat uint out_flags;


vec2 rotate(vec2 pos, float a)
{
    float c = cos(a);
    float s = sin(a);
    return vec2(
    pos.x * c - pos.y * s,
    pos.x * s + pos.y * c
    );
}


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

//    vec2 scaled = in_pos * inst_data.scale;
//    vec2 rotated = rotate(scaled, inst_data.rotation);
//    vec2 world_position =  inst_data.translation + rotated;

    // gl_Position = camera.viewProj * vec4(worldPos, 0.0, 1.0);
//    gl_Position = vec4(world_position, 0.0, 1.0);

    vec2 ndc = vec2(vertices[gl_VertexIndex]) * 2.0 - 1.0;
    gl_Position = vec4(ndc, 0.0, 1.0);

    out_color = vec3(1.f,1.f,1.f); // default color

    out_texture_idx = inst_data.texture_index;

    out_flags = inst_data.flags;

    if((inst_data.flags & SPRITE_PIPELINE_COLOR) != 0){
        out_color = inst_data.color;

    }


    if((inst_data.flags & SPRITE_PIPELINE_TEXTURE) != 0){

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

}