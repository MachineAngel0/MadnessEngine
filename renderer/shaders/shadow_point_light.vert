#version 450

// required for including other shader files
#extension GL_GOOGLE_include_directive : require

#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_scalar_block_layout: require
#extension GL_EXT_buffer_reference : require
#extension GL_ARB_shader_draw_parameters : enable

#include "shader_includes/test_uniform.glsl"
#include "shader_includes/materials.glsl"
#include "shader_includes/draw_data.glsl"

layout(push_constant, std430) uniform PC_MESH_POINT_LIGHT{
    Mesh_Renderable_Buffer mesh_renderable_buffer;
    uint point_light_index;
    uint padding2;
}pc_mesh;


void main() {


    uint draw_idx = gl_VertexIndex;
    uint instance_idx = gl_DrawIDARB;
    //    uint instance_idx = gl_InstanceIndex;


    //global mesh data
    vec3 vertex = ubo.vertex_buffer.vertex_data[draw_idx];


    //get object draw data
    Mesh_Renderable cur_mesh_data = pc_mesh.mesh_renderable_buffer.mesh_data[instance_idx];

    //get transform data
    mat4 model = ubo.transform_buffer.transform_data[cur_mesh_data.transform_idx];


    //TODO: change the ubo to be from the directional lights pov
    //    gl_Position = ubo.proj * ubo.view * model * vec4(vertex, 1.0);

    uint face_index = pc_mesh.point_light_index;

    gl_Position = ubo.point_light_shadow_data.point_light_shadow_data[0].light_world_view_matrix[face_index] * model * vec4(vertex, 1.0);
}
