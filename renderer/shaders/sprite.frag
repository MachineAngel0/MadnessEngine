#version 450


layout(location = 0) in vec2 in_uv;
layout(location = 1) in flat uint in_color_idx;
layout(location = 2) in flat uint out_texture_idx;

layout(location = 0) out vec4 out_color;


void main() {

    vec3 temp_color = vec3(1.0f,1.0f,1.0f);
    out_color = vec4(temp_color, 1.0);
}