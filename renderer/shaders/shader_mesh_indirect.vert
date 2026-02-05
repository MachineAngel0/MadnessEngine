#version 450

// required for including other shader files
#extension GL_GOOGLE_include_directive : require

#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_scalar_block_layout: require
#extension GL_EXT_buffer_reference : require
#extension GL_ARB_shader_draw_parameters : enable



#include "shader_includes/test_uniform.glsl"


struct material_data{
    uint color_id;
};


//0
layout(set = 2, binding = 0, scalar) readonly buffer UV_BUFFER{
    vec2 uv[];
}UV[];

//1
layout(set = 2, binding = 0, scalar) readonly buffer NORMAL_BUFFER{
    vec3 normal[];
}NORMAL[];

//2
layout(set = 2, binding = 0, scalar) readonly buffer MATERIAL_BUFFER{
    material_data color_index[];
}MATERIAL[];




layout(location = 0) in vec3 in_pos;

layout(location = 0) out vec3 out_normal;
layout(location = 1) out vec4 out_tangent;
layout(location = 2) out vec2 out_uv;
layout(location = 3) out flat uint out_color_idx;
layout(location = 4) out vec3 out_frag_pos;

void main() {

    //    uint color_idx = Material.material_data[0].color_id;

    uint idx = gl_VertexIndex;

    gl_Position = ubo[nonuniformEXT(0)].proj * ubo[nonuniformEXT(0)].view * ubo[nonuniformEXT(0)].model * vec4(in_pos, 1.0);
    //    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(in_pos, 1.0);

    out_normal = NORMAL[nonuniformEXT(1)].normal[nonuniformEXT(idx)];
    out_tangent = vec4(1.0, 1.0, 1.0, 1.0);

    out_uv = UV[nonuniformEXT(0)].uv[nonuniformEXT(idx)];

//    out_color_idx = MATERIAL[nonuniformEXT(2)].color_index[nonuniformEXT(gl_DrawIDARB)].color_id;
    out_color_idx = MATERIAL[nonuniformEXT(2)].color_index[nonuniformEXT(0)].color_id;


    out_frag_pos = vec3(ubo[nonuniformEXT(0)].model * vec4(in_pos, 1.0));

}