#extension GL_EXT_buffer_reference : require
#extension GL_EXT_scalar_block_layout: require


struct Mesh_Renderable{
    uint material_idx;
    uint transform_idx;


    uint normal_offset;
    uint uv_offset;
    uint tangent_offset;
    uint padding0;
};




layout(buffer_reference, scalar) readonly buffer Mesh_Renderable_Buffer{
    Mesh_Renderable mesh_data[];
};


struct Skinned_Renderable{
    uint material_idx;
    uint transform_idx;

    uint normal_offset;
    uint uv_offset;
    uint tangent_offset;

    uint joint_idx;
    uint weight_idx;
    uint vertex_offset;
    uint skinned_matrix_idx;

};



layout(buffer_reference, scalar) readonly buffer Skinned_Renderable_Buffer{
    Skinned_Renderable skinned_draw_data[];
};












