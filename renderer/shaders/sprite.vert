#version 450

#extension GL_GOOGLE_include_directive : require
#include "shader_includes/test_uniform.glsl"

layout(location = 0) in vec2 in_pos;
layout(location = 1) in vec2 in_uv;

layout(location = 0) out vec2 out_uv;
layout(location = 1) out uint out_color_idx;
layout(location = 2) out uint out_texture_idx;


//TODO: assume a storage buffer of transform data

struct sprite_instance_params{
    vec2 translation;
    vec2 rotation;
    vec2 scale;
};

layout(set = 2, binding = 0, scalar) readonly buffer SPRITE_INSTANCE_BUFFER{
    sprite_instance_params sprite_instance[];
}sprite_instance_buffer;


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
    //TODO: remove test
    //    vec2 ndc =  vec2(in_pos) * 2.0 - 1.0;
    //    gl_Position = vec4(ndc, 0.0, 1.0);

    uint instance_idx = gl_InstanceIndex;


    sprite_instance_buffer_idx = 0;// TODO:
    sprite_instance_params inst_data = sprite_instance_buffer[nonuniformEXT(sprite_instance_buffer_idx)].sprite_instance_params[nonuniformEXT(gl_InstanceIndex)];


    inst_data.rotation;
    inst_data.scale;

    //    Sprite_Instance_Data inst = sprite_instance_buffer[instance_idx];

    vec2 scaled = in_pos * inst_data.scale;
    vec2 rotated = rotate(scaled, inst_data.rotation);
    vec2 world_position =  inst_data.translation; + rotated;

    // gl_Position = camera.viewProj * vec4(worldPos, 0.0, 1.0);
    gl_Position = vec4(world_position, 0.0, 1.0);
    out_uv = in_uv;

}