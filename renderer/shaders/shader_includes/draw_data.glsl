#extension GL_EXT_buffer_reference : require
#extension GL_EXT_scalar_block_layout: require

struct Draw_Object_Data{
    uint object_index;
    uint padding0;
};
struct Mesh_Renderable{
    uint64_t sort_key; // pipeline, permutation <-> depth(if needed for transparency)
    uint pipeline_hash;
    uint permutation;

    uint material_idx;
    uint mesh_asset_idx;
// u32 submesh_idx;
    uint transform_handle;
    uint index_count;
    uint index_offset;
    uint vertex_offset;

//maybe, we can just switch this off in the shader if we want
    uint joint_idx;
    uint weight_idx;
    uint skinned_matrix_idx;
    uint padding0;
};


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
    uint skinned_matrix_idx;
    uint vertex_offset;
};

layout(buffer_reference, scalar) readonly buffer Skinned_Mesh_Draw_Data_Buffer{
    Skinned_Mesh_Draw_Data skinned_draw_data[];
};












