#ifndef RESOURCE_TYPES_H
#define RESOURCE_TYPES_H


#include "ring_queue.h"
#include "stb_image.h"
#include "stb_truetype.h"
#include "transforms.h"


/// HANDLES ///

#define INVALID_HANDLE 0

typedef struct Handle
{
    u32 id;
    u32 generation;
} Handle;

typedef Handle T_Handle;

typedef struct Texture_Handle
{
    u32 handle;
    u32 generation;
} Texture_Handle;

typedef struct Material_Handle
{
    u32 handle;
} Material_Handle;


typedef struct Mesh_Handle
{
    u32 handle;
} Mesh_Handle;


typedef struct Transform_Handle
{
    u32 handle;
    u32 gen;
} Transform_Handle;


typedef struct Sprite_Handle
{
    u32 handle;
} Sprite_Handle;


/// RESOURCES ///

typedef enum Resource_Type
{
    RESOURCE_NONE,

    RESOURCE_TEXTURE,
    RESOURCE_FONT,
    RESOURCE_SPRITE,
    RESOURCE_STATIC_MESH,
    RESOURCE_AUDIO,

    RESOURCE_MAX,
} Resource_Type;

typedef struct Resource_MetaData
{
    Resource_Type type;
    const char* file_path;
    u64 id;
    u64 reference_count;
} Resource_MetaData;


//Texture
typedef struct Texture
{
    u32 width;
    u32 height;
    u8 channels; // rgb? rgba? etc... TODO:  might be cases where you want just something like rb, enum for that
    u64 image_size;
    //image_type
    stbi_uc* pixels;

    // for the renderer
    Texture_Handle handle;
    bool is_font;
} Texture;


//FONT/TEXT
#define DEFAULT_FONT_CREATION_SIZE 128.0f

typedef struct Glyph
{
    int width, height;
    int xoff, yoff;
    float advance;
    float u0, v0, u1, v1; // UV coordinates in atlas
} Glyph;


//called Madness font cause a linux library uses the struct font
typedef struct Madness_Font
{
    stbtt_fontinfo font_info; // NOTE: idk if i even need to store this
    // float font_creation_size; // the larger the more clear the text looks
    //NOTE: this will have to be larger if i support other languages or non standard characters
    Glyph glyphs[96]; // idk why this is 96, im assuming for all the ascii characters
    Texture_Handle font_texture_handle;
} Madness_Font;


/// MESH ///

typedef struct Material_Param_Data
{
    u32 feature_mask;

    vec4 color; // this will be at a default of 1.0, which is white but won't affect the material
    //ALL FROM RANGES 0-1
    float ambient_strength; // optional for now we can remove it later
    float roughness_strength;
    float metallic_strength;
    float normal_strength;
    float ambient_occlusion_strength;
    float emissive_strength;

    u32 color_index;
    u32 normal_index;
    u32 metallic_index;
    u32 roughness_index;
    u32 ambient_occlusion_index;
    u32 emissive_index;
    u32 _padding0;
    u32 _padding1;
    u32 _padding2;
} Material_Param_Data;


typedef struct PC_Mesh
{
    //per instance data change
    u32 ubo_buffer_idx;
    u32 normal_buffer_idx;
    u32 tangent_buffer_idx;
    u32 uv_buffer_idx;
    u32 transform_buffer_idx;
    u32 material_buffer_idx;
    u32 _padding;
    u32 _padding1;
} PC_Mesh;


typedef struct submesh
{
    // Transform transform;

    // vertex_mesh vertices;
    vec3* pos;
    vec3* normal;
    vec4* tangent;
    vec2* uv;

    // size_t* indices;
    u8* indices;
    // u8* indices_2;
    u32 indices_count;
    u32 indices_bytes;
    VkIndexType index_type;

    u64 vertex_bytes;
    u64 normal_bytes;
    u64 tangent_bytes;
    u64 uv_bytes;

    Material_Param_Data material_params;
    Texture_Handle color_texture;
    Texture_Handle normal_texture;
    Texture_Handle metallic_texture;
    Texture_Handle roughness_texture;
    Texture_Handle ambient_occlusion_texture;
    Texture_Handle emissive_texture;

    u32 offset_into_material_data_buffer;

    // mesh_pipeline_flags
    u32 mesh_pipeline_mask; // determines what material features this submesh has
} submesh;


typedef struct static_mesh
{
    submesh* mesh;
    // the number of meshes in the model
    u32 mesh_size;
} static_mesh;

// TODO: skinned mesh


typedef struct skinned_submesh
{
    Transform transform;

    // vertex_mesh vertices;
    vec3* pos;
    vec3* normal;
    vec4* tangent;
    vec2* uv;
    vec4* joints;
    vec4* weights;

    // size_t* indices;
    u8* indices;
    // u8* indices_2;
    u32 indices_count;
    u32 indices_bytes;
    VkIndexType index_type;

    u64 vertex_bytes;
    u64 normal_bytes;
    u64 tangent_bytes;
    u64 uv_bytes;
    u64 weight_bytes;
    u64 joint_bytes;

    Material_Param_Data material_params;
    Texture_Handle color_texture;
    Texture_Handle normal_texture;
    Texture_Handle metallic_texture;
    Texture_Handle roughness_texture;
    Texture_Handle ambient_occlusion_texture;
    Texture_Handle emissive_texture;

    u32 offset_into_material_data_buffer;


    u32 mesh_pipeline_mask; // determines what material features this submesh has
} skinned_submesh;

typedef struct Joint
{
    const char* joint_name;
    int id;
    struct Joint* parent;
    struct Joint** children;
    u8 children_count;
    mat4 inverse_bind_matrix;
} Joint;


typedef struct skinned_mesh
{
    skinned_submesh* mesh;
    // the number of meshes in the model
    u32 mesh_size;

    Joint* joints;
    int joint_count;
} skinned_mesh;


typedef enum Animation_Path_Type
{
    //translated directly from cgltf_animation_path_type
    animation_path_type_invalid,
    animation_path_type_translation,
    animation_path_type_rotation,
    animation_path_type_scale,
    animation_path_type_weights,
    animation_path_type_max_enum
} Animation_Path_Type;

typedef enum Animation_Interpolation_Type
{
    //translated directly from cgltf_interpolation_type
    interpolation_type_linear,
    interpolation_type_step,
    interpolation_type_cubic_spline,
    interpolation_type_max_enum
} Animation_Interpolation_Type;

typedef struct Animation_Sampler
{
    float* timestamps;
    u32 timestamps_count;

    //trs = translation rotation scale
    union
    {
        float* trs_interpolation_values_f;
        vec3* trs_interpolation_values_v3;
        vec4* trs_interpolation_values_v4;
    };

    u32 trs_interpolation_count;
    Animation_Interpolation_Type interpolation_type;
} Animation_Sampler;


typedef struct Animation_Channel
{
    // the index of the channel maps directly to the index of the sampler ex: channel[1].sampler = sampler[1]
    u32 sampler_idx;
    Animation_Path_Type animation_path_type;
    const char* child_joint_name; //for debug
    u32 child_joint_reference_count;
} Animation_Channel;


typedef struct Animation
{
    const char* animation_name;
    Animation_Channel* channels;
    u32 channel_count;
    Animation_Sampler* samplers;
    u32 sampler_count;
} Animation;


//SPRITE

#define MAX_SPRITE_COUNT 1024

typedef struct Sprite_System
{
    Arena* arena;
    Frame_Arena* frame_arena;
    vec2 screen_size; // grab every frame on start


    Sprite sprites[4]; // literally just need one quad for a vertex buffer
    Sprite_Data_array* sprites_data; //TODO: keep a free list of available indicies
    Sprite_Data_array* sprites_data_transient; // this only lasts for the frame
    u16 sprite_indices[6];


    //probably gonna want this
    Sprite_Data_array* ui_sprite_data;
    Sprite_Data_array* text_sprite_data;
} Sprite_System;


#define MAX_TEXTURE_COUNT 1024
#define MAX_FONT_COUNT 10


typedef struct Texture_System
{
    //handle 0 is always the default texture, it should never be allowed to be modified
    Texture_Handle default_texture_handle;

    Texture textures_array[MAX_TEXTURE_COUNT];
    Texture_Handle texture_handles[MAX_TEXTURE_COUNT];
    Resource_MetaData texture_meta_data[MAX_TEXTURE_COUNT];

    ring_queue* available_idx_queue;
    u32 in_use_textures_count; // technically this data is in the ring queue
    u32 max_textures;


    hash_table* texture_filepath_to_idx;
    // hash_table* texture_file_to_usage_count; or // hash_table* handle_to_usage_count

    //textures that the renderer needs to upload to the gpu
    ring_queue* textures_ring_queue;

    //TODO: probably change this to a hash table, handle->font_data
    //rn this corresponds to the same indexes of the textures_array
    Madness_Font font_array[MAX_TEXTURE_COUNT];
} Texture_System;


typedef struct Submesh_Upload_Data
{
    //information to upload into the associated buffer
    u32 vertex_offset;
    u32 vertex_bytes;

    u32 indices_bytes;
    VkIndexType index_type;

    u32 normal_offset;
    u32 normal_bytes;

    u32 tangent_offset;
    u32 tangent_bytes;

    u32 uv_offset;
    u32 uv_bytes;

    //TODO: these technically could just be u8's
    vec4* tangent;
    vec3* pos;
    vec3* normal;
    vec2* uv;
    u8* indices;

    Material_Param_Data material_params;

} Mesh_Upload_Data;

typedef struct Mesh_Draw_Data
{
    /*typedef struct VkDrawIndexedIndirectCommand {
    uint32_t    indexCount;
    uint32_t    instanceCount;
    uint32_t    firstIndex;
    int32_t     vertexOffset;
    uint32_t    firstInstance;
} VkDrawIndexedIndirectCommand;*/

    u32 vertex_offset; //in vec3
    u32 index_offset; //uint32_t    firstIndex; // offset into the index buffer
    u32 index_count; // u32 count

    //TODO: another time, for when i want to do instancing
    // uint32_t firstInstance; // 0
    // uint32_t instanceCount; // 1
} Mesh_Indirect_Draw_Data;


typedef struct submesh_gameplay
{
    Material_Handle material_handle;

    Texture_Handle color_texture;
    Texture_Handle normal_texture;
    Texture_Handle metallic_texture;
    Texture_Handle roughness_texture;
    Texture_Handle ambient_occlusion_texture;
    Texture_Handle emissive_texture;
} submesh_gameplay;

typedef struct static_mesh_gameplay
{
    //each submesh shares the same transform
    Transform_Handle transforms_handle;

    //NOTE: if I wanted to change a submeshes material, i would have to index into it
    submesh_gameplay* submeshes;
    u32 submeshes_size;

    bool visible;
} static_mesh_gameplay;

typedef struct Mesh_System
{
#define MAX_MESH_COUNT 1000
    //TODO: at some point im gonna need a free list cpu side, if i am to dynamically remove and add meshes,
    // fragmentation would also be a concern

    //unneccessary upload from cpu into the gpu should be avoided
    //indirect commands will be regenerated every frame, at some point it can also be moved to the gpu


    Transform transforms[MAX_MESH_COUNT];
    Mesh_Indirect_Draw_Data draw_data[MAX_MESH_COUNT];
    u32 draw_data_index;


    //global count of all the data
    static_mesh static_mesh_array[MAX_MESH_COUNT];
    u32 static_mesh_array_size;
    u32 static_mesh_submesh_size;


    //total size of all mesh data
    size_t vertex_byte_size;
    size_t vertex_count_size;
    size_t index_byte_size;
    size_t index_count_size;
    size_t normals_byte_size;
    size_t tangent_byte_size;
    size_t uv_byte_size;


    //NOTE: NOT IN USE RN
    skinned_mesh* skinned_meshes;
    u32 skinned_mesh_size;

    size_t joints_byte_size;
    size_t weight_byte_size;



    u32 vertex_buffer_size;
    u32 index_buffer_size;
    u32 normal_buffer_size;
    u32 tangent_buffer_size;
    u32 uv_buffer_size;
    // u32 joint_buffer_size;
    // u32 weight_buffer_size;

    // data*, offset, byte_size ->for all the types
    ring_queue* mesh_ring_queue;
} Mesh_System;


//RENDER PACKET

//these are all just references to the data, they do not own anything
typedef struct Render_Packet_Mesh
{
    const char* system_name;

    //geometry data for indirect draws
    Mesh_Indirect_Draw_Data* draw_data;
    u32 draw_data_size;

    //TODO: remove these
    static_mesh* static_mesh_array;
    u32 static_mesh_array_size;
    u32 static_mesh_submesh_size;

} Render_Packet_Mesh;

typedef struct Render_Packet_UI
{
    const char* system_name;
    Sprite_Data_array* ui_data_packet;
    VkIndexType ui_index_type;

    Sprite_Data_array* text_data_packet;
    VkIndexType text_index_type;
} Render_Packet_UI;

typedef struct Render_Packet_Sprite
{
    const char* system_name;
    Sprite_Data_array* sprite_data;
    Sprite_Data_array* sprite_data_transient;
    u16 sprite_indices[6];
} Render_Packet_Sprite;


typedef struct Render_Packet
{
    //just references
    ring_queue* texture_queue;
    ring_queue* mesh_queue;


    //FOR RENDERING

    //rn we just have one of each,
    Render_Packet_Sprite sprite_data_packet;
    Render_Packet_UI ui_data_packet;
    Render_Packet_Mesh mesh_data_packet;
    // Render_Packet_Game game_data_packet;
} Render_Packet;

typedef struct Resource_System
{
    //the resource system is just a container for all the system,
    //each system can just access the system it need to load assets

    //TODO: it would make sense to double buffer our frame allocators for transient resources or for cpu->gpu uploads


    // Shader_System* shader_system;
    // Material_System* shader_system; //probably want a material system, but not a shader system here, but in the renderer
    Sprite_System* sprite_system;
    Mesh_System* mesh_system;
    Texture_System* texture_system;


    Render_Packet* render_packet;

} Resource_System;

typedef struct Scene
{
    Transform* transforms;
    int transform_count;

    //i dont need rn but could be useful
    // since we know static doesn't change we can cache the transforms
    // Transform* static_transform;
    // Transform* dynamic_transform;

} Scene;





#endif //RESOURCE_TYPES_H
