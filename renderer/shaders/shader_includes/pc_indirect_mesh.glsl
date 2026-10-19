#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_scalar_block_layout: require
#extension GL_EXT_buffer_reference : require
#extension GL_ARB_shader_draw_parameters : enable

#include "materials.glsl"
#include "draw_data.glsl"


layout(push_constant, std430) uniform PC_MESH{
    Mesh_Renderable_Buffer mesh_renderable_buffer;
    uint padding1;
    uint padding2;
}pc_mesh;




