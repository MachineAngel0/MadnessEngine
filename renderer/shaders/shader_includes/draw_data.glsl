#extension GL_EXT_buffer_reference : require
#extension GL_EXT_scalar_block_layout: require



struct Mesh_Draw_Data{
    uint transform_idx;
    uint material_idx;
};

layout(buffer_reference, scalar) readonly buffer Mesh_Draw_Data_Buffer{
    Mesh_Draw_Data mesh_data[];
};


struct Skinned_Mesh_Draw_Data{
    uint transform_idx;
    uint material_idx;
    uint joint_idx;
    uint weight_idx;
    uint vertex_offset;
};

layout(buffer_reference, scalar) readonly buffer Skinned_Mesh_Draw_Data_Buffer{
    Skinned_Mesh_Draw_Data skinned_draw_data[];
};












