#version 450

#extension GL_GOOGLE_include_directive : require
#include "shader_includes/2d_structs.glsl"
#include "shader_includes/test_uniform.glsl"

layout(location = 0) in vec2 in_pos;

layout(location = 0) out vec3 out_color;
layout(location = 1) out vec2 out_uv;
layout(location = 2) out vec2 out_local_pos;
layout(location = 3) out flat uint out_texture_idx;
layout(location = 4) out flat uint out_sprite_buffer_location;


void main() {


    uint sprite_instance_buffer_idx = PC_2D.instance_buffer_idx;
    out_sprite_buffer_location = gl_InstanceIndex;

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

    //rotations
    // using a rotation matrix, calculated on the gpu
    //| cos θ  -sin θ | * | x |
    //| sin θ   cos θ | × | y |

    //vec2 center = inst_data.pos + inst_data.pivot * inst_data.size // when i want abritrary pivot points
    vec2 center = inst_data.pos + inst_data.size * 0.5; // find the center
    float c = cos(inst_data.rotation);
    float s = sin(inst_data.rotation);

    vec2 v = vertices[gl_VertexIndex];
    v -= center;
    v = vec2(v.x * c - v.y * s, v.x * s + v.y * c);
    v += center;

    vec2 ndc = v * 2.0 - 1.0;
//    vec2 ndc = (vec2(vertices[gl_VertexIndex])) * 2.0 - 1.0;
    gl_Position = vec4(ndc, 0.0, 1.0);
    out_color = inst_data.color;

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

    out_uv = uvs[gl_VertexIndex];

    // Always output local UVs
    vec2 local_position[6] = {
    vec2(0.0, 0.0),
    vec2(0.0, 1.0),
    vec2(1.0, 1.0),
    vec2(1.0, 0.0),
    vec2(0.0, 1.0),
    vec2(1.0, 1.0),
    };
    out_local_pos = local_position[gl_VertexIndex];

}