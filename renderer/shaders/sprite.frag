#version 450


layout(location = 0) in vec2 in_uv;
layout(location = 1) in uint in_color_idx;
layout(location = 2) in uint out_texture_idx;

layout(location = 0) out vec4 outColor;


void main() {

    outColor = vec4(fragColor, 1.0);
}