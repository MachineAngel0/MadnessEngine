#version 450

// required for including other shader files
#extension GL_GOOGLE_include_directive : require

#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_scalar_block_layout: require
#extension GL_EXT_buffer_reference : require
#extension GL_ARB_shader_draw_parameters : enable

#include "shader_includes/test_uniform.glsl"
#include "shader_includes/pc_skinned_mesh.glsl"



layout(location = 0) out vec3 out_normal;
layout(location = 1) out vec4 out_tangent;
layout(location = 2) out vec2 out_uv;
layout(location = 3) out flat uint out_color_idx;
layout(location = 4) out vec3 out_world_position;

void main() {


    uint draw_idx = gl_VertexIndex;
    uint instance_idx = gl_DrawIDARB;
//    uint instance_idx = gl_InstanceIndex; // at some point this needs to be used for the mesh data



    //global
    uint ubo_index = pc_skinned_mesh.ubo_buffer_idx;


    //global mesh data
    vec3 vertex = pc_skinned_mesh.vertex_buffer.vertex_data[nonuniformEXT(draw_idx)];

    out_normal = pc_skinned_mesh.normal_buffer.normal_data[nonuniformEXT(draw_idx)];
    out_uv = pc_skinned_mesh.uv_buffer.uv_data[nonuniformEXT(draw_idx)];
    out_tangent = pc_skinned_mesh.tangent_buffer.tangent_data[nonuniformEXT(draw_idx)];


    //get object draw data
    Skinned_Mesh_Draw_Data cur_mesh_data = pc_skinned_mesh.skinned_draw_data_buffer.skinned_draw_data[instance_idx];

    //get transform data
    mat4 model = pc_skinned_mesh.transform_buffer.transform_data[nonuniformEXT(cur_mesh_data.transform_idx)];

    gl_Position = ubo[nonuniformEXT(ubo_index)].proj * ubo[nonuniformEXT(ubo_index)].view * model * vec4(vertex, 1.0);

    //Material
    Pbr mat_data = pc_skinned_mesh.material_buffer.pbr_data[nonuniformEXT(cur_mesh_data.material_idx)];

    out_color_idx = mat_data.color_index;



    out_world_position = vec3(model * vec4(vertex, 1.0));


}

//NOTE: for reference
//vec4 local_pos  = vec4(in_pos, 1.0);
//vec4 world_pos  = model * local_pos;       // local → world
//vec4 view_pos   = view * world_pos;        // world → camera/eye space
//vec4 clip_pos   = proj * view_pos;         // eye → clip space