#ifndef RESOURCE_TYPES_H
#define RESOURCE_TYPES_H


#include <stdalign.h>

#include "material_types.h"
#include "ring_queue.h"
#include "sprite_type.h"
#include "stb_image.h"
#include "stb_truetype.h"
#include "transforms.h"
#include "ui_types.h"


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
    u32 gen;
} Sprite_Handle;

typedef struct Animation_Handle
{
    u32 handle;
    u32 gen;
} Animation_Handle;


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
    // bool free_after_gpu_upload;
} Texture;


//FONT/TEXT
#define DEFAULT_FONT_CREATION_SIZE 32.0f // this should realy be 40
#define GLYPH_LENGTH 96
#define GLYPH_START 32
#define GLYPH_END 128

typedef struct Glyph
{
    float width, height;
    float xoff, yoff;
    float advance;
    float u0, v0, u1, v1; // UV coordinates in atlas
} Glyph;


//called Madness font cause a linux library uses the struct font
typedef struct Madness_Font
{
    // float font_creation_size; // the larger the more clear the text looks
    //NOTE: this will have to be larger if i support other languages or non standard characters
    Glyph glyphs[GLYPH_LENGTH]; //all ascii characters (that we would actually want to present) 128-32 = 96
    Texture_Handle font_texture_handle;
} Madness_Font;


/// MESH ///

typedef struct Mesh_Data
{
    /*typedef struct VkDrawIndexedIndirectCommand {
    uint32_t    indexCount;
    uint32_t    instanceCount;
    uint32_t    firstIndex;
    int32_t     vertexOffset;
    uint32_t    firstInstance;
} VkDrawIndexedIndirectCommand;*/

    //info for indirect draw
    u32 vertex_offset; //in vec3
    u32 index_offset; //uint32_t    firstIndex; // offset into the index buffer
    u32 index_count; // u32 count

    //TODO: another time, for when i want to do instancing
    // uint32_t firstInstance; // 0
    // uint32_t instanceCount; // 1

    //info for vertex pulling
    u32 normal_offset;
    // u32 normal_bytes;

    u32 tangent_offset;
    // u32 tangent_bytes;

    u32 uv_offset;
    // u32 uv_bytes;

    //material info
    Transform_Handle transform_handle;
    Material_Handle material_handle;
} Mesh_Data;

typedef struct Mesh_Upload_Data
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
} Mesh_Upload_Data;



typedef struct Skinned_Mesh_Upload_Data
{
    u64 joint_bytes;
    u64 weight_bytes;

    u64 joint_offset;
    u64 weight_offset;

    vec4* joints;
    vec4* weights;
} Skinned_Mesh_Upload_Data;


//mesh -> submesh



typedef struct Mesh_Draw_Data
{
    //used as a storage buffer
    u32 transform_idx;
    u32 material_instance_handle;
} Mesh_Draw_Data;



typedef struct Skinned_Draw_Data
{
    //offset into the buffer
    u32 vertex_idx;
    u32 uv_idx;
    u32 normal_idx;
    u32 tangent_idx;
    u32 joint_idx;
    u32 weight_idx;
    u32 material_instance_handle;
    u32 transform_idx;
} Skinned_Mesh_Draw_Data;



typedef struct Joint
{
    const char* joint_name;
    u32 id;
    u32 parent_idx;
} Joint;

typedef enum Animation_Path_Type
{
    //translated directly from cgltf_animation_path_type
    Animation_Path_Type_Invalid,
    Animation_Path_Type_Translation, // vec3
    Animation_Path_Type_Rotation, // vec4
    Animation_Path_Type_Scale, // vec3
    Animation_Path_Type_Weights, // float
    Animation_Path_Type_Max
} Animation_Path_Type;


typedef enum Animation_Interpolation_Type
{
    //translated directly from cgltf_interpolation_type
    Animation_Interpolation_Type_Linear,
    Animation_Interpolation_Type_Step,
    Animation_Interpolation_Type_Cubic_Spline,
    Animation_Interpolation_Type_Max_enum
} Animation_Interpolation_Type;

typedef struct Animation_Channel
{
    u32 sampler_idx;
    Animation_Path_Type animation_path_type;
    u32 joint_index;
} Animation_Channel;

typedef struct Animation_Sampler
{
    float* timestamps;
    u32 timestamps_count;

    float sampler_start;
    float sampler_end;

    //trs = translation rotation scale, and weights
    union
    {
        float* trs_float;
        vec3* trs_vec3;
        vec4* trs_vec4;
    } interperlation_data;

    u32 trs_interpolation_count;
    Animation_Interpolation_Type interpolation_type;
} Animation_Sampler;

typedef struct Animation
{
    String* animation_name;
    Animation_Channel* channels;
    u32 channel_count;
    Animation_Sampler* samplers;
    u32 sampler_count;

    float anim_start;
    float anim_end;

    //TODO: current time is only here for testing
    // float current_time;
} Animation;

typedef struct Animation_Data
{
    Joint* joints;
    u32 joint_count; // also the weight count

    mat4* resting_pose_local_matrix;
    mat4* inverse_bind_matrix;

    Animation* animations;
    u32 animations_count;

} Animation_Data;

typedef struct Skinned_Mesh_Data
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

    u32 vertex_count; //in vec3


    //NOTE: shaders need the type size offset into the array, while the free list probably needs the bytes, we will see
    u32 uv_offset;
    u32 normal_offset;
    u32 tangent_count;

    u64 joint_bytes;
    u64 joint_offset;

    u64 weight_bytes;
    u64 weight_offset;


    Transform_Handle transform_handle;
    Material_Handle material_handle;

} Skinned_Mesh_Data;




typedef struct Mesh_Meta_Data
{
    const char* file_path;
    u32* mesh_index;
    u32 mesh_count;
}Mesh_Meta_Data;

typedef struct Skinned_Mesh_Meta_Data
{
    const char* file_path;
    u32* submesh_index;
    u32 submesh_count;

    u32* skinned_submesh_indexs;
    u32 skinned_mesh_count;

    Animation_Handle anim_handle;
}Skinned_Mesh_Meta_Data;

typedef struct skinned_mesh_instance
{
    //reference back to the animation data
    //TODO: replace with a handle
    // skinned_mesh_handle* handle;
    Animation_Handle anim_handle;


    vec3* local_translation;
    quat* local_rotation;
    vec3* local_scale;

    u32 joint_count;

    //generated every frame
    mat4* gpu_matrix;


    u32 current_animation_index;
    float current_time;
    bool looping;
} Skinned_Mesh_Instance;


//SPRITE

#define MAX_SPRITE_COUNT 1024

typedef struct Sprite_System
{
    Allocator* allocator;
    Frame_Allocator* frame_arena;
    vec2 screen_size; // grab every frame on start


    Sprite sprites[4]; // literally just need one quad for a vertex buffer
    u16 sprite_indices[6];


    Free_List_ARRAY_TYPE(Sprite_Data)* sprites_data;
    ARRAY_TYPE(Sprite_Data)* sprites_frame_data;
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


typedef struct Mesh_System
{
#define MAX_MESH_COUNT 1000
#define MAX_SKINNED_MESH_COUNT 1000
    //TODO: at some point im gonna need a free list cpu side, if i am to dynamically remove and add meshes,
    // fragmentation would also be a concern, unless i pool size, or split the pool into many different pool sizes

    //unneccessary upload from cpu into the gpu should be avoided
    //indirect commands will be regenerated every frame, at some point it can also be moved to the gpu

    //we want one continous stream of buffers for the data types
    //we upload the data to the gpu then we free it

    //the mesh system needs to ideally have a list of meshes which point to submeshes, but the meshes themselves are linear in the array
    //mesh and skinned mesh need to be treated differently


    Mesh_Meta_Data mesh_meta_data[MAX_MESH_COUNT];
    u32 mesh_meta_data_count;
    Skinned_Mesh_Meta_Data skinned_mesh_meta_data[MAX_MESH_COUNT];
    u32 skinned_mesh_meta_data_count;


    Mesh_Data mesh_data[MAX_MESH_COUNT];
    u32 mesh_data_count;

    Skinned_Mesh_Data skinned_mesh_data[MAX_MESH_COUNT];
    u32 skinned_mesh_data_count;

    Animation_Data animation_data[MAX_MESH_COUNT];
    u32 animation_data_count;


    //FUTURE: seperate list of meshes we wish to not draw, when we do we add it back to our original list
    // Mesh_Indirect_Draw_Data not_in_use_draw_data[MAX_MESH_COUNT];
    // Mesh_Indirect_Draw_Data not_in_use_skinned_draw_data[MAX_MESH_COUNT];


    //total size of all mesh data
    size_t vertex_byte_size;
    size_t vertex_count_size;
    size_t index_byte_size;
    size_t index_count_size;
    size_t normals_byte_size;
    size_t tangent_byte_size;
    size_t uv_byte_size;

    size_t joints_byte_size;
    size_t weight_byte_size;


    // data*, offset, byte_size ->for all the types
    RING_QUEUE_TYPE(Mesh_Upload_Data)* mesh_ring_queue;
    RING_QUEUE_TYPE(Skinned_Mesh_Upload_Data)* skinned_mesh_ring_queue;

    Skinned_Mesh_Instance* test_skinned_mesh_instance;

    //TODO:
    //anything that couldn't be loaded in this frame
    RING_QUEUE_TYPE(const char*)* load_queue;
} Mesh_System;

typedef struct Scene
{
    Transform* transforms;
    int transform_count;

    mat4* world_transforms; //the count is the same as the transform_count

    //TODO: i dont need it rn but could be useful
    // since we know static doesn't change we can cache the transforms
    // Transform* static_transform;
    // Transform* dynamic_transform;
} Scene;


//RENDER PACKET

//these are all just references to the data, they do not own anything
typedef struct Render_Packet_Mesh
{
    //geometry data for indirect draws
    Mesh_Data* draw_data;
    u32 draw_data_size;


    Mesh_Data* skinned_draw_data;
    u32 skinned_draw_data_size;
} Render_Packet_Mesh;

typedef struct Render_Packet_Transform
{
    mat4* world_space_matrix_array;
    u32 world_space_matrix_count;
} Render_Packet_Transform;


typedef struct Render_Packet_Material
{
    Material_Instance* material_instance;
    u32 material_instance_count;
    u32 material_instance_bytes;

    Material_Default* prb;
    u32 prb_count;
    u32 prb_bytes;

    Material_UV_Anim_Data* uv_anim;
    u32 uv_anime_count;
    u32 uv_anime_bytes;
} Render_Packet_Material;


typedef struct Render_Packet_UI
{
    UI_Render_Packet madness_ui_render_packet;
    UI_Render_Packet insanity_ui_render_packet;
} Render_Packet_UI;


typedef struct Render_Packet_Sprite
{
    const char* system_name;
    ARRAY_TYPE(Sprite_Data)* sprite_data;
    ARRAY_TYPE(Sprite_Data)* sprite_data_transient;
    u16 sprite_indices[6];
} Render_Packet_Sprite;


typedef struct Render_Packet
{
    //just references
    RING_QUEUE_TYPE(Texture)* texture_queue;
    RING_QUEUE_TYPE(Mesh_Upload_Data)* mesh_queue;
    RING_QUEUE_TYPE(Skinned_Mesh_Upload_Data)* skinned_mesh_queue;


    //FOR RENDERING

    //rn we just have one of each,
    Render_Packet_Transform transform_data_packet;
    Render_Packet_Material material_data_packet;
    Render_Packet_Sprite sprite_data_packet;
    Render_Packet_UI ui_data_packet;
    Render_Packet_Mesh mesh_data_packet;
} Render_Packet;


typedef struct Resource_System
{
    //the resource system is just a container for all the system,
    //each system manages itself,
    //this does gather the cpu-gpu resources and sends it to the renderer

    // TODO: might change this into a pool allocator, or even segregated list allocator
    Heap_Allocator* heap_allocator;
    Frame_Allocator* frame_allocator;

    // Shader_System* shader_system;
    // Material_System* shader_system; //probably want a material system, but not a shader system here, but in the renderer
    Sprite_System* sprite_system;
    Mesh_System* mesh_system;
    Texture_System* texture_system;
    Material_System* material_system;
    Scene* scene;


    Render_Packet* render_packet;
} Resource_System;


#endif //RESOURCE_TYPES_H
