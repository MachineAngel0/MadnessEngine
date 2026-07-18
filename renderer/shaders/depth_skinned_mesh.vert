#version 450

// required for including other shader files
#extension GL_GOOGLE_include_directive : require

#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_scalar_block_layout: require
#extension GL_EXT_buffer_reference : require
#extension GL_ARB_shader_draw_parameters : enable

#include "shader_includes/test_uniform.glsl"
#include "shader_includes/pc_skinned_mesh.glsl"



void main() {


    uint draw_idx = gl_VertexIndex;
    uint instance_idx = gl_DrawIDARB;
    //    uint instance_idx = gl_InstanceIndex; // at some point this needs to be used for the mesh data

    Skinned_Mesh_Draw_Data cur_mesh_data = pc_skinned_mesh.skinned_draw_data_buffer.skinned_draw_data[instance_idx];

    uint local_vertex_idx = gl_VertexIndex -  cur_mesh_data.vertex_offset;

    //shoult not be draw idx
    vec4 joints = ubo.joint_buffer.joint_data[nonuniformEXT(cur_mesh_data.joint_idx + local_vertex_idx)];
    vec4 weights = ubo.weight_buffer.weights_data[nonuniformEXT(cur_mesh_data.weight_idx + local_vertex_idx)];

    //need to index into the correct buffer
    mat4 skin_matrix =
    ubo.skinned_matrix_buffer.skinned_matrix_data[uint(joints.x)] * weights.x +
    ubo.skinned_matrix_buffer.skinned_matrix_data[uint(joints.y)] * weights.y +
    ubo.skinned_matrix_buffer.skinned_matrix_data[uint(joints.z)] * weights.z +
    ubo.skinned_matrix_buffer.skinned_matrix_data[uint(joints.w)] * weights.w;


    //global mesh data
    vec3 vertex = ubo.vertex_buffer.vertex_data[nonuniformEXT(draw_idx)];


    //get transform data
    mat4 model = ubo.transform_buffer.transform_data[nonuniformEXT(cur_mesh_data.transform_idx)];

    gl_Position = ubo.proj * ubo.view * model * skin_matrix * vec4(vertex, 1.0);


}
