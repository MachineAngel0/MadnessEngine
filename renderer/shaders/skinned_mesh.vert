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
layout(location = 3) out vec3 out_world_position;
layout(location = 4) out flat uint64_t material_offset;
layout(location = 5) out mat4 out_model;

void main() {


    uint vertex_index = gl_VertexIndex;
    uint instance_idx = gl_DrawIDARB;
    //    uint instance_idx = gl_InstanceIndex; // at some point this needs to be used for the mesh data

    Skinned_Renderable mesh_renderable = pc_skinned_mesh.skinned_draw_data_buffer.skinned_draw_data[instance_idx];

    uint local_vertex_idx = gl_VertexIndex -  mesh_renderable.vertex_offset;
    //    cur_mesh_data.vertex_offset;


    //shoult not be draw idx
    vec4 joints = ubo.joint_buffer.joint_data[(mesh_renderable.joint_idx + local_vertex_idx)];
    vec4 weights = ubo.weight_buffer.weights_data[(mesh_renderable.weight_idx + local_vertex_idx)];
//    vec4 joints = ubo.joint_buffer.joint_data[cur_mesh_data.joint_idx];
//    vec4 weights = ubo.weight_buffer.weights_data[cur_mesh_data.weight_idx];


    //need to index into the correct buffer
    mat4 skin_matrix =
    ubo.skinned_matrix_buffer.skinned_matrix_data[mesh_renderable.skinned_matrix_idx + uint(joints.x)] * weights.x +
    ubo.skinned_matrix_buffer.skinned_matrix_data[mesh_renderable.skinned_matrix_idx + uint(joints.y)] * weights.y +
    ubo.skinned_matrix_buffer.skinned_matrix_data[mesh_renderable.skinned_matrix_idx + uint(joints.z)] * weights.z +
    ubo.skinned_matrix_buffer.skinned_matrix_data[mesh_renderable.skinned_matrix_idx + uint(joints.w)] * weights.w;


    //global mesh data
    vec3 vertex = ubo.vertex_buffer.vertex_data[vertex_index];

    out_normal = mat3(skin_matrix) * ubo.normal_buffer.normal_data[vertex_index];
    out_uv = ubo.uv_buffer.uv_data[vertex_index];
    out_tangent = /*mat3(skin_matrix) * */ubo.tangent_buffer.tangent_data[vertex_index];


    //get transform data
    mat4 model = ubo.transform_buffer.transform_data[mesh_renderable.transform_idx];
    out_model = model;


    gl_Position = ubo.proj * ubo.view * model * skin_matrix * vec4(vertex, 1.0);

    //Material
    Material_Record record = ubo.material_record_buffer.data[mesh_renderable.material_idx];
    material_offset = record.offset;




    out_world_position = vec3(model * skin_matrix * vec4(vertex, 1.0));


}
