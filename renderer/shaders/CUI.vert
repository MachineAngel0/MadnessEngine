#version 450

#extension GL_EXT_buffer_reference : require
#extension GL_EXT_scalar_block_layout : require
#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_nonuniform_qualifier : require

#include "shader_includes/test_uniform.glsl"
#include "shader_includes/cui_structs.glsl"


layout(location = 0) out vec4 out_color;
layout(location = 1) out vec2 out_local_pos;
layout(location = 2) out vec2 out_uv;
layout(location = 3) out flat uint out_instance_idx;
layout(location = 4) out flat uint out_texture_idx;

void main() {

    out_instance_idx = gl_InstanceIndex;
    CUI_Data cui_data = PC_CUI.cui_ssbo.cui_data[gl_InstanceIndex];

    //we are manually generating the index and vertex data, since its always the same just with offsets and positions
    int indices[6] = int[6](0, 1, 2, 2, 3, 0);
    int idx = indices[gl_VertexIndex];

    vec2 local_positions[4] = vec2[4](
            vec2(0.0, 0.0),  // 0 top-left
            vec2(0.0, 1.0),  // 1 bottom-left
            vec2(1.0, 1.0),  // 2 bottom-right
            vec2(1.0, 0.0)   // 3 top-right
    );

    //rotations
    // using a rotation matrix, calculated on the gpu
    //| cos θ  -sin θ | * | x |
    //| sin θ   cos θ | × | y |

    //vec2 center = inst_data.pos + inst_data.pivot * inst_data.size // when i want abritrary pivot points
    vec2 center = cui_data.pos + cui_data.size * 0.5; // find the center
    float c = cos(cui_data.rotation);
    float s = sin(cui_data.rotation);

    //    vec2 v = vertices[gl_VertexIndex];
    vec2 v = cui_data.pos + local_positions[idx] * cui_data.size;
    v -= center;
    v = vec2(v.x * c - v.y * s, v.x * s + v.y * c);
    v += center;


    //pixel range to the  [0,1] range
    v /= ubo.screen_dimensions;
    // transform this into the [-1,1] range
    vec2 ndc = v * 2.0 - 1.0;


    gl_Position = vec4(ndc, 0.0, 1.0);
    out_color = cui_data.color;

    float left = cui_data.uv_offset.x;
    float top = cui_data.uv_offset.y;
    float right = cui_data.uv_offset.x + cui_data.uv_size.x;
    float bottom = cui_data.uv_offset.y+ cui_data.uv_size.y;

    vec2 uvs[4]=
    {
    vec2(left, top),
    vec2(left, bottom),
    vec2(right, bottom),
    vec2(right, top),
    };

    out_uv = uvs[idx];
    out_texture_idx = cui_data.texture_idx;

    out_local_pos = local_positions[idx];

}