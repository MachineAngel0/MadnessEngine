#extension GL_EXT_buffer_reference : require
#extension GL_EXT_scalar_block_layout: require

struct Material_Instance{
    uint flags;

    uint pbr_idx;
    uint uv_anim_idx;
    uint black_hole_idx;
};

layout(set = 2, binding = 0, scalar) readonly buffer MATERIAL_INSTANCE_BUFFER{
    Material_Instance material_instance[];
}MATERIAL_INSTANCE[];

layout(push_constant, scalar) uniform PC_MESH{
    uint ubo_buffer_idx;
}pc_mesh;


struct mesh_draw_data{
    uint transform_idx;
    uint material_instance_idx;
};


layout(set = 2, binding = 0, scalar) readonly buffer MESH_DRAW_DATA_BUFFER{
    mesh_draw_data mesh_data[];
}MESH_DRAW_DATA[];

struct Pbr{
    uint flags;

    vec4 color;
//ALL FROM RANGES 0-1
    float ambient_strength;
    float roughness_strength;
    float metallic_strength;
    float normal_strength;
    float ambient_occlusion_strength;
    float emissive_strength;

    uint color_index;
    uint normal_index;
    uint metallic_index;
    uint roughness_index;
    uint ambient_occlusion_index;
    uint emissive_index;
    uint _padding0;
    uint _padding1;
    uint _padding2;
};



layout(set = 2, binding = 0, scalar) readonly buffer PBR_BUFFER{
    Pbr pbr[];
}PBR[];


