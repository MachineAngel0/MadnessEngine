#version 450

#extension GL_GOOGLE_include_directive : require
#include "shader_includes/2d_structs.glsl"
#include "shader_includes/test_uniform.glsl"

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 intex;

layout(location = 0) out vec3 fragColor;
//layout(location = 1) out vec2 out_uv;


void main() {

    uint vertex_index = gl_VertexIndex % 6;

    uint sprite_instance_buffer_idx = PC_2D.instance_buffer_idx;

    Sprite_Data inst_data =
        Sprite_Instance_Buffer[nonuniformEXT(sprite_instance_buffer_idx)].sprite_instance_data[nonuniformEXT(gl_InstanceIndex)];

//    inst_data.pos += inst_data.size;
//    vec2 ndc =  vec2(inPosition) * 2.0 - 1.0;

    vec2 world = inst_data.pos + (inPosition * inst_data.size);
    vec2 ndc = world * 2.0 - 1.0;

    gl_Position = vec4(ndc, 0.0, 1.0);
    //gl_Position = (inPosition, 1.0, 1.0);
    fragColor = inst_data.color;
}