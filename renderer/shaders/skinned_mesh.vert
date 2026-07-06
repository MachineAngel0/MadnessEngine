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


    //i would probably need this as joint and weight data dont line up with the vertex offset
    //localVertex = gl_VertexIndex - pc_skinned_mesh.[instance_idx].vertex_offset;

    //global
    uint ubo_index = pc_skinned_mesh.ubo_buffer_idx;


    //global mesh data
    vec3 vertex = pc_skinned_mesh.vertex_buffer.vertex_data[nonuniformEXT(draw_idx)];

    out_normal = pc_skinned_mesh.normal_buffer.normal_data[nonuniformEXT(instance_idx)];
    out_uv = pc_skinned_mesh.uv_buffer.uv_data[nonuniformEXT(instance_idx)];
    out_tangent = pc_skinned_mesh.tangent_buffer.tangent_data[nonuniformEXT(instance_idx)];


    //get object draw data
    skinned_mesh_draw_data cur_mesh_data = pc_skinned.skinned_draw_data_buffer[instance_idx];

    //get transform data
    mat4 model = pc_skinned_mesh.transform_buffer.transform_data[nonuniformEXT(cur_mesh_data.transform_idx)];

    gl_Position = ubo[nonuniformEXT(ubo_index)].proj * ubo[nonuniformEXT(ubo_index)].view * model * vec4(vertex, 1.0);

    //Material
    PBR mat_data = pc_skinned_mesh.material_buffer.pbr_data[nonuniformEXT(cur_mesh_data.material_idx)];

    uint material_instance_buffer_idx = ubo[nonuniformEXT(ubo_index)].material_instance_idx;
    Material_Instance material_instance = MATERIAL_INSTANCE[nonuniformEXT(material_instance_buffer_idx)].material_instance[nonuniformEXT(cur_mesh_data.material_instance_idx)];


    uint pbr_buffer_idx = ubo[nonuniformEXT(ubo_index)].material_pbr_idx;
    Pbr pbr_material = PBR[nonuniformEXT(pbr_buffer_idx)].pbr[material_instance.pbr_idx];

    out_color_idx = pbr_material.color_index;



    out_world_position = vec3(model * vec4(vertex, 1.0));


}

//NOTE: for reference
//vec4 local_pos  = vec4(in_pos, 1.0);
//vec4 world_pos  = model * local_pos;       // local → world
//vec4 view_pos   = view * world_pos;        // world → camera/eye space
//vec4 clip_pos   = proj * view_pos;         // eye → clip space