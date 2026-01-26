#version 450

// required for including other shader files
#extension GL_GOOGLE_include_directive : require

#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_scalar_block_layout: require
#extension GL_EXT_buffer_reference : require
#extension GL_ARB_shader_draw_parameters : enable



#include "shader_includes/test_uniform.glsl"

//layout(std140, set = 0, binding = 0) uniform UniformBufferObject{
//    mat4 model;
//    mat4 view;
//    mat4 proj;
//} ubo[];

layout(set = 2, binding = 0) readonly buffer UVBUFFER{
    vec2 uv[];
}UV[];

struct material_data{
    uint color_id;
};

layout(set = 2, binding = 0) readonly buffer MATERIAL_BUFFER{
    material_data color_index[];
}MATERIAL[];


layout(location = 0) in vec3 in_pos;

layout(location = 0) out vec3 out_normal;
layout(location = 1) out vec4 out_tangent;
layout(location = 2) out vec2 out_uv;
layout(location = 3) out flat uint out_color_idx;

void main() {

//    uint color_idx = Material.material_data[0].color_id;



    uint idx = gl_VertexIndex;
    //    vec3 in_pos = pc.position_buffer.position[idx];
    gl_Position = ubo[nonuniformEXT(0)].proj * ubo[nonuniformEXT(0)].view * ubo[nonuniformEXT(0)].model * vec4(in_pos, 1.0);
//    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(in_pos, 1.0);

    out_normal = vec3(1.0,1.0,1.0);
    out_tangent = vec4(1.0,1.0,1.0,1.0);
    out_uv = UV[nonuniformEXT(0)].uv[gl_VertexIndex];
//    out_uv = UV.uv[gl_VertexIndex];

    out_color_idx = MATERIAL[nonuniformEXT(1)].color_index[gl_DrawIDARB].color_id;


    //    out_normal = pc.normal_buffer.normal[idx];
    //    out_tangent = pc.tangent_buffer.tangent[idx];
    //    out_uv = pc.uv_buffer.uv[idx];

}