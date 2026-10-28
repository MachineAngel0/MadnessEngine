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
layout(location = 4) out flat uint64_t material_offset;
layout(location = 5) out vec4 out_world_transform;
layout(location = 6) out mat4 out_local_transform;


void main() {

    uint vertex_idx = gl_VertexIndex;
    uint instance_idx = gl_DrawIDARB;
//    uint instance_idx = gl_InstanceIndex;

    //global mesh data
    vec3 vertex = ubo.vertex_buffer.vertex_data[vertex_idx];
    out_normal = ubo.normal_buffer.normal_data[vertex_idx];
//    out_normal = normalize(mat3(transpose(inverse(model))) * normal_data[vertex_idx]);
    out_uv = ubo.uv_buffer.uv_data[vertex_idx];
    out_tangent = ubo.tangent_buffer.tangent_data[vertex_idx];


    //get object draw data
    Mesh_Renderable mesh_renderable = pc_mesh.mesh_renderable_buffer.mesh_data[instance_idx];
    //get transform data
    mat4 model = ubo.transform_buffer.transform_data[mesh_renderable.transform_idx];
    out_local_transform = model;

    out_world_transform = model * vec4(vertex, 1.0);

    gl_Position = ubo.proj * ubo.view * model * vec4(vertex, 1.0);

    //Material
    //if(material_instance.flags & PBR){};

    Material_Record record = ubo.material_record_buffer.data[mesh_renderable.material_idx];
    material_offset = record.offset;


    out_world_position = vec3(model * vec4(vertex, 1.0));

}

//NOTE: for reference
//vec4 local_pos  = vec4(in_pos, 1.0);
//vec4 world_pos  = model * local_pos;       // local → world
//vec4 view_pos   = view * world_pos;        // world → camera/eye space
//vec4 clip_pos   = proj * view_pos;         // eye → clip space










