#version 450

#extension GL_EXT_buffer_reference : require
#extension GL_EXT_scalar_block_layout : require
#extension GL_GOOGLE_include_directive : require

#include "shader_includes/test_uniform.glsl"
#include "shader_includes/2d_structs.glsl"


layout(location = 0) in vec2 in_pos;

layout(location = 0) out vec3 out_color;
layout(location = 1) out vec2 out_uv;
layout(location = 2) out flat uint out_texture_idx;
layout(location = 3) out flat uint out_material_idx;
layout(location = 4) out vec2 out_local_pos;



void main() {

    uint instance_idx = gl_InstanceIndex;
    out_material_idx = instance_idx;

    UI_Data inst_data = PC_2D.material_buffer.ui_data[nonuniformEXT(instance_idx)];

    out_color = inst_data.color;
    out_texture_idx = inst_data.texture_handle;

    int indices[6] = int[6](0, 1, 2, 2, 3, 0);
    int idx = indices[gl_VertexIndex];
    vec2 local_positions[4] = vec2[4](
    vec2(0.0, 0.0),  // 0 top-left
    vec2(0.0, 1.0),  // 1 bottom-left
    vec2(1.0, 1.0),  // 2 bottom-right
    vec2(1.0, 0.0)   // 3 top-right
    );


    vec2 v =  inst_data.pos + local_positions[idx] * inst_data.size;
    vec2 ndc = v * 2.0 - 1.0;
    gl_Position = vec4(ndc, 0.0, 1.0);


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

    out_uv = uvs[idx];
    out_local_pos = local_positions[idx];
}