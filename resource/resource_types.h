#ifndef RESOURCE_TYPES_H
#define RESOURCE_TYPES_H


#include <stdalign.h>

#include "material_types.h"
#include "ring_queue.h"
#include "runtime_registry.h"
#include "sprite_type.h"
#include "stb_image.h"
#include "stb_truetype.h"
#include "transforms.h"
#include "ui_types.h"


///////////////// HANDLES  //////////////////////

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
    // u32 batch_handle;
} Material_Handle;


typedef struct Mesh_Asset_Handle
{
    u32 handle;
} Mesh_Asset_Handle;

typedef struct Sk_Mesh_Asset_Handle
{
    u32 handle;
} Sk_Mesh_Asset_Handle;


/*//used by the game/editor to modify meshes
typedef struct Mesh_Handle
{
   u32 handle;
   u32 submesh_idx;
} Mesh_Handle;

typedef struct Sk_Mesh_Handle
{
   u32 handle;
   u32 submesh_idx;
} Sk_Mesh_Handle;*/


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


///////////////// RESOURCES  //////////////////////


typedef enum Resource_Type
{
    RESOURCE_NONE,

    RESOURCE_TEXTURE,
    RESOURCE_FONT,
    RESOURCE_SPRITE,
    RESOURCE_STATIC_MESH,
    RESOURCE_SKINNED_MESH,
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

///////////////// Texture  //////////////////////


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

///////////////// Particle  //////////////////////


typedef struct Particle_Animation_vec3
{
    float time;
    vec3s value;
} Particle_Animation_vec3;

typedef struct Particle_Animation_vec2
{
    float time;
    vec2s value;
} Particle_Animation_vec2;

typedef struct Particle_Animation_float
{
    float time;
    float value;
} Particle_Animation_float;


// i guess we just fat struct the emitter for now
/*typedef struct Particle_Emitter
{
    vec3s Spawn_area;
    Particle_Animation_vec3 Color_over_lifetime;
    vec3s color_variance;
    Particle_Animation_float Scale_over_lifetime;
    float Scale_variance;
    Particle_Animation_float Emitter_wide_velocity_over_time;
    vec3s Per_particle_velocity_variance;
    vec3s Gravity;
    Particle_Animation_vec3 Emitter_wide_rotational_velocity_over_time;
    vec3s Per_particle_rotation_variance;
    vec3s Per_particle_rotation_rate_variance;
    Particle_Animation_float Emission_per_second;
    float Emitter_lifetime;
    float Particle_lifetime;
    float Particle_lifetime_variance;
    int Blend_mode;
    //(either 3D or 2D billboard)
    vec3s Facing_type_3D;
    vec3s Facing_type_2D;
    float Depth_softening;
    // Material intensity over lifetime (AnimatedValue);
    // Material intensity variance;
    Particle_Animation_vec2 UV_scrolling;
    vec2s UV_offsets;
    vec2s uv_variance;
    Texture_Handle Textures;
    // Animated Textures
    u32 Maximum_Particles;
    u32 Minimum_Particles;
    bool loop;
    bool toggle_visibility;
} Particle_Emitter;*/

//start simple, then we can use the complex_version
typedef struct Particle_Emitter
{
    bool nothingl;
} Particle_Emitter;


typedef struct Particle
{
    vec3s position;
    vec2s rotation;
    vec2s scale;

    float life_left;


    //draw data?
    //Texture
    Texture_Handle texture_handle;
    vec2s tex_offset;
    vec2s tex_size;
} Particle;

typedef struct Particle_Mesh
{
    vec3 position;
    vec3s rotation;
    vec3s scale;

    float lifetime_left;


    //draw data?
    //Texture
    Mesh_Asset_Handle Mesh;
    Texture_Handle Textures;
    vec2s tex_offset;
} Particle_Mesh;


///////////////// MESH  //////////////////////


// m1|m2|m3|m4|m5
// batcn/mat1 = m1|m2|m5
// batcn/ = m3|m4


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
        vec3s* trs_vec3;
        vec4s* trs_vec4;
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

    mat4s* resting_pose_local_matrix;
    mat4s* inverse_bind_matrix;

    Animation* animations;
    u32 animations_count;
} Animation_Data;


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
    vec4s* tangent;
    vec3s* pos;
    vec3s* normal;
    vec2s* uv;
    u8* indices;
} Mesh_Upload_Data;


typedef struct Skinned_Mesh_Upload_Data
{
    u64 joint_bytes;
    u64 weight_bytes;

    u64 joint_offset;
    u64 weight_offset;


    vec4s* joints;
    vec4s* weights;
} Sk_Mesh_Upload_Data;


typedef struct Mesh_Indirect_Draw
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

    //TODO: another time, for when i want to do instancing, which might not be worth it
    // uint32_t firstInstance; // 0
    // uint32_t instanceCount; // 1

    /*//used as a storage buffer
    u32 transform_idx;
    u32 material_instance_handle;*/
} Mesh_Indirect_Draw;

typedef struct Mesh_GPU_Draw
{
    u32 transform_idx;
    u32 material_instance_handle;
} Mesh_GPU_Draw;

typedef struct SKMesh_GPU_Draw
{
    u32 transform_idx;
    u32 material_instance_handle;
    u32 joint_idx;
    u32 weight_idx;
    u32 vertex_offset;
} SKMesh_GPU_Draw;


typedef struct Mesh_Data
{
    //info for indirect draw
    u32 vertex_offset; //in vec3
    u32 index_offset; //uint32_t    firstIndex; // offset into the index buffer
    u32 index_count; // u32 count


    //info for vertex pulling
    u32 normal_offset;
    // u32 normal_bytes;

    u32 tangent_offset;
    // u32 tangent_bytes;

    u32 uv_offset;
    // u32 uv_bytes;

    //material info
    Material_Handle default_material_handle;
} Mesh_Data;

typedef struct Sk_Mesh_Data
{
    u64 joint_bytes;
    u64 joint_offset_vec4;
    u64 joint_offset_bytes;

    u64 weight_bytes;
    u64 weight_offset_vec4;
    u64 weight_offset_bytes;
} Sk_Mesh_Data;

typedef struct Mesh_Instance
{
    Mesh_Indirect_Draw mesh_indirect_draw;
    Mesh_GPU_Draw mesh_gpu_draw;
    Material_Handle material_handle;
    Transform_Handle parent_transform_handle;
} Mesh_Instance;

typedef struct Mesh_Parent_Instance
{
    //this generally is only for changing materials and transforms, and not for the renderer
    Mesh_Asset_Handle mesh_asset;
    Transform_Handle transform_handle;

    u32 mesh_count;
    Mesh_Instance* mesh_instances_array;
} Mesh_Parent_Instance;

typedef struct Sk_Mesh_Instance
{
    Mesh_Indirect_Draw mesh_indirect_draw;
    SKMesh_GPU_Draw sk_mesh_gpu_draw;
    Material_Handle material_handle;
    Transform_Handle parent_transform_handle;
} Sk_Mesh_Instance;

typedef struct Sk_Mesh_Parent_Instance
{
    Sk_Mesh_Asset_Handle sk_mesh_handle;
    Transform_Handle transform_handle;
    u32 mesh_count;
    Sk_Mesh_Instance* sk_mesh_instance_array;

    //generated every frame
    mat4s* gpu_matrix;

    vec3s* local_translation;
    versors* local_rotation;
    vec3s* local_scale;

    u32 joint_count;

    u32 current_animation_index;
    float current_time;
    bool looping;
} Sk_Mesh_Parent_Instance;


typedef struct Mesh_Asset
{
    const char* file_path;
    Mesh_Data* mesh_data;
    u32 mesh_count;
} Mesh_Asset;

typedef struct Sk_Mesh_Asset
{
    const char* file_path;
    Mesh_Data* mesh_data;
    u32 mesh_count;

    Sk_Mesh_Data* skinned_mesh_data;
    u32 skinned_mesh_count;

    Animation_Data* animation_data;
} Sk_Mesh_Asset;


///////////////////////MATERIAL/SHADER/////////////////////////

typedef enum Shader_Mesh_Type
{
    Shader_Mesh_Type_Mesh,
    Shader_Mesh_Type_Skinned,
    // Shader_Pass_Type_Particle,
} Shader_Mesh_Type;

typedef enum Shader_Pass_Type
{
    Shader_Pass_Type_MESH_PBR_OPAQUE,
    Shader_Pass_Type_MESH_PBR_TRANSPARENCY,
    Shader_Pass_Type_SHADOW,
    // PIPELINE_TYPE_TERRAIN,
} Shader_Pass_Type;

typedef enum Shader_Blend
{
    Shader_Blend_Mode_Default, // oqaque
    Shader_Blend_Mode_Alpha,
    Shader_Blend_Mode_PreMultiplied_Alpha,

    Shader_Blend_Mode_additive,
    // Shader_Blend_Mode_Soft_Additive,

    //  Shader_Blend_Mode_MULTIPLY,
    //  Shader_Blend_Mode_SCREEN,

    //  Shader_Blend_Mode_SUBTRACT,
    //  Shader_Blend_Mode_REVERSE_SUBTRACT,

    //  Shader_Blend_Mode_MIN,
    //  Shader_Blend_Mode_MAX,
} Shader_Blend_Mode;

typedef enum Shader_Stage_Type
{
    Shader_Stage_Type_Graphics,
    Shader_Stage_Type_Compute,
} Shader_Stage_Type;


typedef enum Material_Flag
{
    MATERIAL_FLAG_PBR = BITFLAG(0),
    MATERIAL_FLAG_UV_ANIM = BITFLAG(1),
} Material_Flag;


typedef enum Mesh_PBR_Flags
{
    MESH_PIPELINE_COLOR = BITFLAG(1),
    MESH_PIPELINE_NORMAL = BITFLAG(2),
    MESH_PIPELINE_EMISSIVE = BITFLAG(3),
    MESH_PIPELINE_ROUGHNESS = BITFLAG(4),
    MESH_PIPELINE_METALLIC = BITFLAG(5),
    MESH_PIPELINE_AO = BITFLAG(6),
    MESH_PIPELINE_ENUM_MAX,
} Mesh_PBR_Flags;



typedef struct PC_Mesh
{
    VkDeviceAddress draw_data_buffer;
    VkDeviceAddress material_buffer;
} PC_Mesh;

typedef struct PC_Particle
{
    VkDeviceAddress draw_material_buffer;
    VkDeviceAddress unused;
} PC_Particle;

typedef struct PC_General
{
    VkDeviceAddress draw_data_buffer;
    VkDeviceAddress material_buffer;
} PC_General;

typedef struct PC_Skinned_Mesh
{
    VkDeviceAddress draw_data_buffer;
    VkDeviceAddress material_buffer;
} PC_Skinned_Mesh;

typedef struct Material_Batch
{
    const char* shader_name;
    Reflection_Runtime_Struct* material_struct;

    Dynamic_Array* material_data;
    // PC_General* pc_general; // TODO:

    //TODO: you could use a union for the mesh types
    Dynamic_Array* mesh_instances;

    Shader_Stage_Type shader_stage;
    Shader_Pass_Type shader_pass;
    Shader_Mesh_Type mesh_type;
    Shader_Blend_Mode blend_mode;
} Material_Batch;


#define MAX_DEFAULT_MATERIAL 100

typedef struct Material_System
{
    //Hardcode pipelines
    Material_Default prb[MAX_DEFAULT_MATERIAL]; // pretty much manditory for all meshes
    u32 pbr_count;

    Heap_Allocator* heap_allocator;
    Reflection_System* reflection_system;
    Reflection_Registry* reflection_registry;

    //for now all the push constants are going to be hardcoded, there shouldn't be much varation between them most likely


    //sort material batches by their pass type
    Material_Batch oqaque_batch[100];
    u32 oqaque_batch_count;
} Material_System;


//SPRITE

#define MAX_SPRITE_COUNT 1024

typedef struct Sprite_System
{
    Allocator* allocator;
    Frame_Allocator* frame_arena;
    vec2s screen_size; // grab every frame on start


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

typedef struct Scene
{
    Transform* transforms;
    int transform_count;

    mat4s* world_transforms; //the count is the same as the transform_count

    //TODO: i dont need it rn but could be useful
    // since we know static doesn't change we can cache the transforms
    // Transform* static_transform;
    // Transform* dynamic_transform;
} Scene;


typedef struct Mesh_System
{
#define MAX_MESH_COUNT 1000
#define MAX_SKINNED_MESH_COUNT 1000

    //TODO: at some point im gonna need a free list cpu side, if i am to dynamically remove and add meshes,
    // fragmentation would also be a concern, unless i pool size, or split the pool into many different pool sizes

    //mesh_asset (has all the data) -> mesh_parent_instance (contains pointers to all the submeshes, is the thing we want the game to manipulate, also allows us to make material changes quickly)
    //-> mesh_instance (inside the material batches)


    Mesh_Asset mesh_asset_data[MAX_MESH_COUNT];
    u32 mesh_asset_count;

    Sk_Mesh_Asset skinned_mesh_asset_data[MAX_MESH_COUNT];
    u32 sk_mesh_asset_count;

    Mesh_Parent_Instance mesh_parent_instance[MAX_MESH_COUNT];
    u32 mesh_parent_instance_count;

    Sk_Mesh_Parent_Instance skinned_mesh_instance[MAX_MESH_COUNT];
    u32 skinned_mesh_instance_count;


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


    size_t skinned_matrix_offset_size; //offset by mat4's


    DYNAMIC_ARRAY_TYPE(mat4)* skinned_matrix_array;


    // data*, offset, byte_size ->for all the types
    RING_QUEUE_TYPE(Mesh_Upload_Data)* mesh_ring_queue;
    RING_QUEUE_TYPE(Skinned_Mesh_Upload_Data)* skinned_mesh_ring_queue;


    //TODO:
    //anything that couldn't be loaded in this frame
    RING_QUEUE_TYPE(const char*)* load_queue;
} Mesh_System;


typedef struct Particle_System
{
    //memory, arena, frame_arena

    //individual particles
    //animation data

    //vertex/index buffer

    Particle* particles;
    u32 particles_count;
} Particle_System;


//RENDER PACKET

//these are all just references to the data, they do not own anything
typedef struct Render_Packet_3D
{
    //geometry data for indirect draws

    //TODO: we should have a dirty bit for generating any new batches
    Material_Batch* oqaque_batch;
    u32 oqaque_batch_count;

    Material_Default* prb;
    u32 prb_count;
    u32 prb_bytes;

    mat4s* world_space_matrix_array;
    u32 world_space_matrix_count;

    DYNAMIC_ARRAY_TYPE(mat4)* skinned_matrix;
} Render_Packet_3D;


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

typedef struct Render_Packet_Particle
{
    Particle* particles;
    u32 particle_count;
} Render_Packet_Particle;

typedef struct Render_Packet
{
    //just references
    RING_QUEUE_TYPE(Texture)* texture_queue;
    RING_QUEUE_TYPE(Mesh_Upload_Data)* mesh_queue;
    RING_QUEUE_TYPE(Skinned_Mesh_Upload_Data)* skinned_mesh_queue;


    //FOR RENDERING

    //rn we just have one of each,
    Render_Packet_Sprite sprite_data_packet;
    Render_Packet_UI ui_data_packet;
    Render_Packet_3D draw_3d_data_packet;
    Render_Packet_Particle particle_packet;
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

    Particle_System* particle_system;

    Render_Packet* render_packet;
} Resource_System;


#endif //RESOURCE_TYPES_H
