#version 450

// required for including other shader files
#extension GL_GOOGLE_include_directive : require

#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_scalar_block_layout: require
#extension GL_EXT_buffer_reference : require
#extension GL_ARB_shader_draw_parameters : enable

#include "shader_includes/test_uniform.glsl"
#include "shader_includes/materials.glsl"


layout(location = 0) in vec3 in_pos;


layout(location = 0) out vec3 out_normal;
layout(location = 1) out vec4 out_tangent;
layout(location = 2) out vec2 out_uv;
layout(location = 3) out flat uint out_color_idx;
layout(location = 4) out vec3 out_world_position;

void main() {


    uint draw_idx = gl_VertexIndex;
    uint instance_idx = gl_DrawIDARB;


    //global
    uint ubo_index = pc_mesh.ubo_buffer_idx;

    //global mesh data
    uint normal_buffer_index = ubo[nonuniformEXT(ubo_index)].normal_idx;
    uint tangent_buffer_index = ubo[nonuniformEXT(ubo_index)].tangent_idx;
    uint uv_buffer_index = ubo[nonuniformEXT(ubo_index)].uv_idx;
    out_normal = NORMAL[nonuniformEXT(normal_buffer_index)].normal[nonuniformEXT(gl_VertexIndex)];
    out_uv = UV[nonuniformEXT(uv_buffer_index)].uv[nonuniformEXT(gl_VertexIndex)];

    out_tangent = vec4(1.0, 1.0, 1.0, 1.0);
    //out_tangent = TANGENT[nonuniformEXT(tangent_index)].tangent[nonuniformEXT(gl_VertexIndex)];


    //get object draw data
    uint draw_data_buffer_idx = ubo[nonuniformEXT(ubo_index)].draw_data_idx;
    mesh_draw_data cur_mesh_data = MESH_DRAW_DATA[nonuniformEXT(draw_data_buffer_idx)].mesh_data[nonuniformEXT(instance_idx)];

    //get transform data
    uint transform_buffer_idx = ubo[nonuniformEXT(ubo_index)].transform_idx;
    uint transform_idx =  cur_mesh_data.transform_idx;


    mat4 model = TRANSFORM[nonuniformEXT(transform_buffer_idx)].model_transforms[nonuniformEXT(transform_idx)];
    gl_Position = ubo[nonuniformEXT(ubo_index)].proj * ubo[nonuniformEXT(ubo_index)].view * model * vec4(in_pos, 1.0);

    //Material
    uint material_instance_buffer_idx = ubo[nonuniformEXT(ubo_index)].material_instance_idx;
    Material_Instance material_instance = MATERIAL_INSTANCE[nonuniformEXT(material_instance_buffer_idx)].material_instance[nonuniformEXT(cur_mesh_data.material_instance_idx)];

    //if(material_instance.flags & PBR){};
    uint pbr_buffer_idx = ubo[nonuniformEXT(ubo_index)].material_pbr_idx;
    Pbr pbr_material = PBR[nonuniformEXT(pbr_buffer_idx)].pbr[material_instance.pbr_idx];

    out_color_idx = pbr_material.color_index;



    out_world_position = vec3(model * vec4(in_pos, 1.0));


    /*
    //    uint color_idx = Material.material_data[0].color_id;

    uint draw_idx = gl_VertexIndex;
    uint draw_other_idx = gl_DrawIDARB;
    out_index = gl_DrawIDARB;

    uint ubo_index = pc_mesh.ubo_buffer_idx;
    uint normal_index = pc_mesh.normal_buffer_idx;
    uint tangent_index =  pc_mesh.tangent_buffer_idx;
    uint uv_index =  pc_mesh.uv_buffer_idx;
    uint transform_index =  pc_mesh.transform_buffer_idx;
    uint material_index =  pc_mesh.material_buffer_idx;

    mat4 model = TRANSFORM[nonuniformEXT(transform_index)].model_transforms[nonuniformEXT(gl_DrawIDARB)];

    gl_Position = ubo[nonuniformEXT(ubo_index)].proj * ubo[nonuniformEXT(ubo_index)].view * model * vec4(in_pos, 1.0);
//    gl_Position = ubo[nonuniformEXT(ubo_index)].proj * ubo[nonuniformEXT(ubo_index)].view * ubo[nonuniformEXT(ubo_index)].model * vec4(in_pos, 1.0);
    //    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(in_pos, 1.0);

    //    out_normal = NORMAL[nonuniformEXT(normal_index)].normal[nonuniformEXT(draw_idx)];
    out_normal = NORMAL[nonuniformEXT(normal_index)].normal[nonuniformEXT(gl_VertexIndex)];
    //out_normal = mat3(transpose(inverse(model))) * normal); // TODO: this would get you a correct normal

    out_tangent = vec4(1.0, 1.0, 1.0, 1.0);

    out_uv = UV[nonuniformEXT(uv_index)].uv[nonuniformEXT(gl_VertexIndex)];

    out_color_idx = MATERIAL[nonuniformEXT(material_index)].material_data[nonuniformEXT(gl_DrawIDARB)].color_index;
    //    out_color_idx = MATERIAL[nonuniformEXT(material_index)].material_data[nonuniformEXT(0)].color_index;


    out_world_position = vec3(model * vec4(in_pos, 1.0));
*/
}

//NOTE: for reference
//vec4 local_pos  = vec4(in_pos, 1.0);
//vec4 world_pos  = model * local_pos;       // local → world
//vec4 view_pos   = view * world_pos;        // world → camera/eye space
//vec4 clip_pos   = proj * view_pos;         // eye → clip space