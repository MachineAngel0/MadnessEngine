#ifndef VULKAN_STRUCT_DRAW_TYPES_H
#define VULKAN_STRUCT_DRAW_TYPES_H



typedef struct Vulkan_Shader_Handle
{
    u32 handle;
} Vulkan_Shader_Handle;

typedef struct Pipeline_Handle
{
    u32 handle;
} Pipeline_Handle;

typedef struct Buffer_Handle
{
    u32 handle;
} Buffer_Handle;

typedef struct Buffer_Per_Frame_Handle
{
    u32 handle;
} Buffer_Frame_Handle;

typedef struct Staging_Buffer_Handle
{
    u32 handle;
} Staging_Buffer_Handle;

typedef struct Descriptor_Handle
{
    u32 handle;
} Descriptor_Handle;



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

typedef enum vulkan_renderable_change_command_type
{
    vk_change_visibility_off,
    vk_change_remove,
    vk_change_modify_renderpass_masks,
}vulkan_renderable_change_command_type;

typedef struct  vulkan_renderable_change_command
{
    vulkan_renderable_change_command_type type;
    union
    {
        bool visible;
        u32 masks_bit;
        //etc
    };

}vulkan_renderable_change_command;



typedef struct Vulkan_Particle_Renderable
{
    u64 shader_key;
    u32 material_index;
    u32 particle_index;
} Vulkan_Particle_Renderable;

typedef struct Vulkan_Mesh_Renderable
{
    u64 sort_key; // pipeline, permutation <-> depth(if needed for transparency)

    u32 material_idx;
    u32 mesh_instance_idx;
    u32 mesh_asset_idx;
    // u32 submesh_idx;
    u32 transform_handle;
    u32 index_count;
    u32 index_offset;
    u32 vertex_offset;

    //maybe, we can just switch this off in the shader if we want
    u32 joint_idx;
    u32 weight_idx;
    u32 skinned_matrix_idx;
} Vulkan_Mesh_Renderable;


typedef struct Vulkan_Renderable
{
    u32 draw_object_index;
    u32 padding0;
}Vulkan_Renderable;


typedef enum Renderable_Type {
    Renderable_Type_Mesh,
    Renderable_Type_Particle,
}Renderable_Type;


//TODO: these need to be kept persistent eventually instead of created every frame
typedef struct Vulkan_Render_Item
{
    Shader_Key shader_key;
    Shader_Pipeline_Hash shader_hash;
    u32 render_pass_masks;

    u32 id;
    bool visible;

    Renderable_Type type;


    union render_item
    {
        Vulkan_Mesh_Renderable mesh_render_item;
        Vulkan_Particle_Renderable particle_render_item;
    };

}Vulkan_Render_Item;

struct Vulkan_Render_List
{

    Vulkan_Render_Item opaque[1000];
    // Vulkan_Render_Item transparent[1000];
    // Vulkan_Render_Item shadow_list[1000];
    //etc. other passes and what not

    //TODO: you can condense these all down to one giant buffer with starting offsets for each renderpass
    Buffer_Frame_Handle indirect_opaque;
    // Buffer_Frame_Handle indirect_tranparent;
    // Buffer_Frame_Handle indirect_shadow;


    //OPTIMIZE: these should be one buffer and not per frame eventually
    Buffer_Frame_Handle mesh_draw_data;
    Buffer_Frame_Handle mesh_buffer;

    Vulkan_Mesh_Renderable mesh_renderables[1000];
    Vulkan_Particle_Renderable particle_renderables[1000];
    Vulkan_Renderable renderables[2000];

};

//mesh item -> mesh draw item



#endif //VULKAN_STRUCT_DRAW_TYPES_H
