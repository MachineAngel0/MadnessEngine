#version 450

// required for including other shader files
#extension GL_GOOGLE_include_directive : require

#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_scalar_block_layout: require
#extension GL_EXT_buffer_reference : require
#extension GL_ARB_shader_draw_parameters : enable

#include "shader_includes/test_uniform.glsl"
#include "shader_includes/pc_indirect_mesh.glsl"



layout(location = 0) out vec3 out_normal;
layout(location = 1) out vec4 out_tangent;
layout(location = 2) out vec2 out_uv;
layout(location = 3) out vec3 out_world_position;
layout(location = 4) out flat Pbr out_material;


void main() {

    uint draw_idx = gl_VertexIndex;
    uint instance_idx = gl_DrawIDARB;
//    uint instance_idx = gl_InstanceIndex;


    //global mesh data
    vec3 vertex = ubo.vertex_buffer.vertex_data[nonuniformEXT(draw_idx)];
    out_normal = ubo.normal_buffer.normal_data[nonuniformEXT(draw_idx)];
    out_uv = ubo.uv_buffer.uv_data[nonuniformEXT(draw_idx)];
    out_tangent = ubo.tangent_buffer.tangent_data[nonuniformEXT(draw_idx)];


    //get object draw data
    Mesh_Draw_Data cur_mesh_data = pc_mesh.mesh_draw_data_buffer.mesh_data[nonuniformEXT(instance_idx)];

    //get transform data
    mat4 model = ubo.transform_buffer.transform_data[nonuniformEXT(cur_mesh_data.transform_idx)];


    gl_Position = ubo.proj * ubo.view * model * vec4(vertex, 1.0);

    //Material
    //if(material_instance.flags & PBR){};
    Pbr mat_data = pc_mesh.material_buffer.pbr_data[nonuniformEXT(cur_mesh_data.material_idx)];
    out_material = mat_data;


    out_world_position = vec3(model * vec4(vertex, 1.0));

}

//NOTE: for reference
//vec4 local_pos  = vec4(in_pos, 1.0);
//vec4 world_pos  = model * local_pos;       // local → world
//vec4 view_pos   = view * world_pos;        // world → camera/eye space
//vec4 clip_pos   = proj * view_pos;         // eye → clip space