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
layout(location = 5) out mat4 out_skinned_mat;
layout(location = 9) out flat uint out_local_vert;
layout(location = 10) out flat uint out_offset;
layout(location = 11) out flat uint out_vert_index;
layout(location = 12) out flat uint out_base;
layout(location = 13) out vec4 out_joints;
layout(location = 14) out  vec4 out_weights;

void main() {


    uint draw_idx = gl_VertexIndex;
    uint instance_idx = gl_DrawIDARB;
    //    uint instance_idx = gl_InstanceIndex; // at some point this needs to be used for the mesh data

    Skinned_Mesh_Draw_Data cur_mesh_data = pc_skinned_mesh.skinned_draw_data_buffer.skinned_draw_data[instance_idx];

    uint local_vertex_idx = gl_VertexIndex -  cur_mesh_data.vertex_offset;
//    cur_mesh_data.vertex_offset;
    out_offset = cur_mesh_data.vertex_offset;
    out_vert_index = gl_VertexIndex;
    out_base = gl_BaseVertexARB;
    out_local_vert = local_vertex_idx;


    //global mesh data
    vec3 vertex = ubo.vertex_buffer.vertex_data[nonuniformEXT(draw_idx)];

    out_normal = ubo.normal_buffer.normal_data[nonuniformEXT(draw_idx)];
    out_uv = ubo.uv_buffer.uv_data[nonuniformEXT(draw_idx)];
    out_tangent = ubo.tangent_buffer.tangent_data[nonuniformEXT(draw_idx)];



    //shoult not be draw idx
    vec4 joints = ubo.joint_buffer.joint_data[nonuniformEXT(cur_mesh_data.joint_idx + local_vertex_idx)];
    vec4 weights = ubo.weight_buffer.weights_data[nonuniformEXT(cur_mesh_data.weight_idx + local_vertex_idx)];

    out_joints = joints;
    out_weights = weights;

    //need to index into the correct buffer
    mat4 skin_matrix =
    ubo.skinned_matrix_buffer.skinned_matrix_data[uint(joints.x)] * weights.x +
    ubo.skinned_matrix_buffer.skinned_matrix_data[uint(joints.y)] * weights.y +
    ubo.skinned_matrix_buffer.skinned_matrix_data[uint(joints.z)] * weights.z +
    ubo.skinned_matrix_buffer.skinned_matrix_data[uint(joints.w)] * weights.w;

    out_skinned_mat = skin_matrix;

    //get transform data
    mat4 model = ubo.transform_buffer.transform_data[nonuniformEXT(cur_mesh_data.transform_idx)];
    vec4 skinned_vertex = skin_matrix *  vec4(vertex, 1.0);
    gl_Position = ubo.proj * ubo.view * model * skinned_vertex;

    //Material
    Pbr mat_data = pc_skinned_mesh.material_buffer.pbr_data[nonuniformEXT(cur_mesh_data.material_idx)];

    out_color_idx = mat_data.color_index;



    out_world_position = vec3(model * skinned_vertex);


}
