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
} vulkan_renderable_change_command_type;

typedef struct vulkan_renderable_change_command
{
    vulkan_renderable_change_command_type type;

    union
    {
        bool visible;
        u32 masks_bit;
        //etc
    };
} vulkan_renderable_change_command;


//TODO: replace this
typedef struct Vulkan_Particle_Renderable
{
    u64 sort_key;
    u32 material_index;
    u32 particle_index;
} Vulkan_Particle_Renderable;


typedef struct Particle_GPU_Renderable
{
    u32 material_index;
    u32 particle_index;
} Particle_GPU_Renderable;



typedef struct Particle_Renderable
{
    Shader_Sort_Key shader_key;
    Shader_Pipeline_Hash pipeline_hash;
    u32 permutation;

    u32 render_pass_masks;

    u32 id;
    bool visible;

    u32 material_index;
    u32 particle_index;
} Particle_Renderable;


typedef struct Mesh_GPU_Renderable
{
    u32 material_idx;
    u32 transform_idx;

    // u32 index_count;
    // u32 index_offset;
    // u32 vertex_offset;
    u32 normal_offset;
    u32 uv_offset;
    u32 tangent_offset;

    u32 padding0;
} Mesh_GPU_Renderable;


typedef struct Skinned_Mesh_GPU_Renderable
{
    u32 material_idx;
    u32 transform_idx;

    // u32 index_count;
    // u32 index_offset;
    // u32 vertex_offset;
    u32 normal_offset;
    u32 uv_offset;
    u32 tangent_offset;

    u32 joint_idx;
    u32 weight_idx;
    u32 vertex_offset; // for finding the correct skinned index
    u32 skinned_matrix_idx; // when we get compute skinned

} Skinned_GPU_Renderable;



typedef struct Vulkan_Skinned_Renderable
{
    // Shader_Sort_Key shader_sort_key; // TODO: for another time
    Shader_Pipeline_Hash pipeline_hash;
    Permutation_Mask permutation;
    u32 mesh_id;

    u32 render_pass_masks;

    bool visible;


    u32 index_count;
    u32 index_offset;
    u32 vertex_offset;
    Skinned_GPU_Renderable skinned_render_item;
} Vulkan_Skinned_Renderable;

typedef struct Vulkav_Mesh_Renderable
{
    // Shader_Sort_Key shader_sort_key; // TODO: for another time
    Shader_Pipeline_Hash pipeline_hash;
    u32 mesh_id;

    u32 render_pass_masks;

    bool visible;

    u32 index_count;
    u32 index_offset;
    u32 vertex_offset;
    Mesh_GPU_Renderable mesh_render_item;
} Vulkan_Mesh_Renderable;


int vulkan_skinned_mesh_sort_key(const void* render_item1, const void* render_item2)
{
    Vulkan_Skinned_Renderable* p1 = (Vulkan_Mesh_Renderable*)render_item1;
    Vulkan_Mesh_Renderable* p2 = (Vulkan_Mesh_Renderable*)render_item2;

    if (p1->pipeline_hash != p2->pipeline_hash)
    {
        return p1->pipeline_hash < p2->pipeline_hash ? -1 : 1;
    }

    if (p1->mesh_id != p2->mesh_id)
    {
        return p1->mesh_id < p2->mesh_id ? -1 : 1;
    }

    return 0;
}

int vulkan_mesh_sort_key(const void* render_item1, const void* render_item2)
{
    Vulkan_Mesh_Renderable* p1 = (Vulkan_Mesh_Renderable*)render_item1;
    Vulkan_Mesh_Renderable* p2 = (Vulkan_Mesh_Renderable*)render_item2;

    if (p1->pipeline_hash != p2->pipeline_hash)
    {
        return p1->pipeline_hash < p2->pipeline_hash ? -1 : 1;
    }


    if (p1->mesh_id != p2->mesh_id)
    {
        return p1->mesh_id < p2->mesh_id ? -1 : 1;
    }

    return 0;
}

typedef struct Vulkan_Render_Command{
    Shader_Pipeline_Hash pipeline_hash;
    u32 offset;
    u32 range;
}Vulkan_Render_Command;

typedef struct Vulkan_Mesh_Render_List
{
    // Vulkan_Render_Item transparent[1000];
    // Vulkan_Render_Item shadow_list[1000];
    // Buffer_Frame_Handle indirect_tranparent;
    // Buffer_Frame_Handle indirect_shadow;
    //etc. other passes and what not

    //TODO: you can condense these all down to one giant buffer with starting offsets for each renderpass
    Vulkan_Mesh_Renderable* mesh_renderable;
    u32 mesh_renderable_count;
    // u32 mesh_renderable_count_max;

    Buffer_Frame_Handle indirect_buffer;
    Buffer_Frame_Handle mesh_draw_data;
    u32 draw_count;

    // Vulkan_Particle_Renderable particle_renderables[1000];
} Vulkan_Mesh_Render_List;


typedef struct Vulkan_Skinned_Mesh_Render_List
{
    Buffer_Frame_Handle skinned_indirect_buffer;
    Buffer_Frame_Handle skinned_draw_data;


    Vulkan_Skinned_Renderable* skinned_mesh_renderable;
    u32 skinned_mesh_renderable_count;
    // u32 skinned_mesh_renderable_count_max;

    u32 draw_count;
} Vulkan_Skinned_Mesh_Render_List;


//vulkan render items[n]
// -> oqaque meshes -> sort by pipeline -> create renderables and indirect draws
// -> transparent meshes -> sort by depth -> create renderables and indirect draws
// -> shadow meshes -> sort by pipeline -> create renderables and indirect draws


#endif //VULKAN_STRUCT_DRAW_TYPES_H
