#version 450

#extension GL_GOOGLE_include_directive : require
#extension GL_ARB_shader_draw_parameters : enable


#include "shader_includes/test_uniform.glsl"



layout(location = 0) in vec4 in_color;

layout(location = 0) out vec4 out_color;


void main() {

    out_color = in_color;

}