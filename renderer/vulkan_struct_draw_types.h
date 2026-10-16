#ifndef VULKAN_STRUCT_DRAW_TYPES_H
#define VULKAN_STRUCT_DRAW_TYPES_H

//ubo
// vertex
// index
// vertex color
// tangent
// normals
//
// transforms
// material records
// materials

// per draw buffers ->


typedef struct Vulkan_Particle_Render_Item
{
    u64 shader_key;
    u32 material_index;
    u32 particle_index;
} Vulkan_Particle_Render_Item;

typedef struct Mesh_Render_Item
{
    u64 sort_key; // pipeline, permutation <-> depth(if needed for transparency)

    u32 material_idx;
    u32 mesh_instance_idx;
    u32 mesh_asset_idx;
    // u32 submesh_idx;
    u32 transform_handle;
    // u32 cull_bounds_handle;
    u32 index_count;
    u32 index_offset;
    u32 vertex_offset;
} Vulkan_Mesh_Render_Item;


typedef struct Skinned_Render_Item
{
    u64 material_key;
    u32 mesh_handle;
    u32 submesh_handle;
    u32 material_handle;
    u32 transform_handle;
    // u32 cull_bounds_handle;
    u32 index_count;
    u32 index_offset;
    u32 vertex_offset;

    u32 joint_idx;
    u32 weight_idx;
    u32 skinned_matrix_idx;
} Vulkan_Skinned_Render_Item;


typedef struct Vulkan_Renderable
{
    u64 shader_key;
    u32 buffer_index;
}Vulkan_Renderable;


typedef enum Render_Item_Type {
    Render_Item_Type_Particle,
    Render_Item_Type_Skinned,
    Render_Item_Type_Mesh,
}Render_Item_Type;


typedef struct Vulkan_Render_Item
{
    u64 shader_hash;
    u8 pipeline_masks;
    Render_Item_Type type;
    union
    {
        Vulkan_Mesh_Render_Item mesh_render_item;
        Vulkan_Skinned_Render_Item skinned_render_item;
        Vulkan_Particle_Render_Item particle_render_item;
    };

}Vulkan_Render_Item;

struct Vulkan_Render_List
{
    Vulkan_Render_Item opaque[1000];
    Vulkan_Render_Item transparent[1000];
    Vulkan_Render_Item shadow_list[1000];
    //etc other passes and what not
};

//mesh item -> mesh draw item



#endif //VULKAN_STRUCT_DRAW_TYPES_H
