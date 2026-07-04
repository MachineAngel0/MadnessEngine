#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_scalar_block_layout: require
#extension GL_EXT_buffer_reference : require
#extension GL_ARB_shader_draw_parameters : enable

#include "materials.glsl"


layout(push_constant, scalar) uniform PC_MESH{
    uint ubo_buffer_idx;
}pc_mesh;




