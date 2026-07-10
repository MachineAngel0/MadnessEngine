#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_scalar_block_layout: require
#extension GL_EXT_buffer_reference : require
#extension GL_ARB_shader_draw_parameters : enable

#include "materials.glsl"
#include "draw_data.glsl"


layout(push_constant, std430) uniform PC_MESH{
    Mesh_Draw_Data_Buffer mesh_draw_data_buffer;
    PRB_Buffer material_buffer; // use a default for now
}pc_mesh;




