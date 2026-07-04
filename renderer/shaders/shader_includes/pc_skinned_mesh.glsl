#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_scalar_block_layout: require
#extension GL_EXT_buffer_reference : require
#extension GL_ARB_shader_draw_parameters : enable

#include "materials.glsl"




layout(push_constant, scalar) uniform PC_SKINNED_MESH{
    uint ubo_buffer_idx;
    uint _padding;

/*    uint64_t vertex_buffer;
    uint64_t index_buffer; */

    Vertex_Buffer vertex_buffer;
    Normal_Buffer normal_buffer;
    Tangent_Buffer tangent_buffer;
    UV_Buffer uv_buffer;
    Joint_Buffer joint_buffer;
    Weight_Buffer weight_buffer;

    Transform_Buffer transform_buffer;
    PRB_Buffer material_buffer; // use a default for now
}pc_skinned_mesh;


