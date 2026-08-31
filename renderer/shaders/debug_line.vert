#version 450

#extension GL_GOOGLE_include_directive : require
#extension GL_ARB_shader_draw_parameters : enable



#include "shader_includes/test_uniform.glsl"


struct DebugLine
{
    vec3 start;
    vec3 end;
    vec4 color;
};

layout(buffer_reference, scalar) readonly buffer DebugLineBuffer
{
    DebugLine lines[];
};

layout(push_constant, scalar) uniform PC_Debug_Line
{
    DebugLineBuffer line_ssbo;
    uint padding0;
    uint padding1;
} pc;



layout(location = 0) out vec4 out_color;

void main() {

    uint line_index = gl_VertexIndex / 2;
    uint endpoint = gl_VertexIndex & 1;

    DebugLine line = pc.line_ssbo.lines[line_index];
    out_color = line.color;


    vec3 position;

    if (endpoint == 0)
    {
        position = line.start;
    }
    else
    {
        position = line.end;
    }

    gl_Position = ubo.proj * ubo.view * vec4(position, 1.0);

}