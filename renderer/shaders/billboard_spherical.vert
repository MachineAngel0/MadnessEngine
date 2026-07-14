#version 450

#extension GL_GOOGLE_include_directive : require
#extension GL_ARB_shader_draw_parameters : enable



#include "shader_includes/2d_structs.glsl"
#include "shader_includes/test_uniform.glsl"

//https://www.flipcode.com/archives/Billboarding-Excerpt_From_iReal-Time_Renderingi_2E.shtml


layout(location = 0) out vec3 out_color;
layout(location = 1) out vec2 out_uv;
layout(location = 2) out vec2 out_tex;
layout(location = 3) out flat uint out_texture_idx;
layout(location = 4) out flat uint out_flags;


void main() {

    //spherical billboards are meant to always be looking at the camera, not matter the angle or axis

    //for generating the vertex's
    // p = point
    // r = radius (or size)
    // rx = half width, ry = half height
    // x = M-object(inverse), M-Camera[0]
    // y = M-object(inverse), M-Camera[1]

    // v0 = p - rxX + ryY
    // v1 = p + rxX + ryY
    // v2 = p + rxX - ryY
    // v3 = p - rxX - ryY


    //properly orienting the camera in z space
    // cam_position = Mobject(inv)*Mcamera[3]

    //normal
    // n = nrm(c-p)
    //tangent
    // a = nrm(n*y)

    // v0 = p - rxa - ryb
    // v1 = p + rxa - ryb
    // v2 = p + rxa + ryb
    // v3 = p - rxa + ryb

    //including rotations

    // v0 = p - (rx cos() - ry sin() )a - (ry cos() + rx sin() ) b
    // v1 = p + (rx cos() + ry sin() )a - (ry cos() - rx sin() ) b
    // v2 = p + (rx cos() - ry sin() )a + (ry cos() + rx sin() ) b
    // v3 = p - (rx cos() + ry sin() )a + (ry cos() - rx sin() ) b





    uint instance_idx = gl_InstanceIndex;
    out_material_buffer_location = instance_idx;
    UI_Data inst_data = PC_2D.material_buffer.ui_data[nonuniformEXT(instance_idx)];



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
    vec2 center = inst_data.pos + inst_data.size * 0.5; // find the center
    float c = cos(inst_data.rotation);
    float s = sin(inst_data.rotation);

    //    vec2 v = vertices[gl_VertexIndex];
    vec2 v =  inst_data.pos + local_positions[idx] * inst_data.size;
    v -= center;
    v = vec2(v.x * c - v.y * s, v.x * s + v.y * c);
    v += center;

    vec2 ndc = v * 2.0 - 1.0;
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

    out_uv = uvs[idx];
    out_texture_idx = inst_data.texture_handle;


    out_local_pos = local_positions[idx];
}