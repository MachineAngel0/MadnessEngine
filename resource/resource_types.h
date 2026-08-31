#ifndef RESOURCE_TYPES_H
#define RESOURCE_TYPES_H


#include <stdalign.h>

#include "array_freelist.h"
#include "filesystem.h"
#include "hash_map.h"
#include "material_types.h"
#include "resource_import_types.h"
#include "ring_queue.h"
#include "runtime_registry.h"
#include "sprite_type.h"

#include "transforms.h"
#include "ui_types.h"
#include "UUID.h"


///////////////// PATHS  //////////////////////

#define IMPORT_PATH_NO_SLASH "../z_assets"
#define IMPORT_PATH_SLASH "../z_assets/"

#define IMPORT_RESOURCE_PATH "../z_assets/asset_list"
#define IMPORT_TEXTURE_PATH "../z_assets/textures"
#define IMPORT_FONTS_PATH "../z_assets/msdf_fonts"
#define IMPORT_MESH_PATH "../z_assets/mesh"
#define IMPORT_AUDIO_PATH "../z_assets/audio"
#define IMPORT_RESOURCE_PATH "../z_assets/asset_list"

#define ENGINE_PATH_NO_SLASH "../z_assets_engine"
#define ENGINE_PATH_SLASH "../z_assets_engine/"

#define ENGINE_RESOURCE_PATH "../z_assets_engine/asset_list/"
#define ENGINE_TEXTURE_PATH "../z_assets_engine/texture/"
#define ENGINE_FONTS_PATH "../z_assets_engine/fonts/"
#define ENGINE_MESH_PATH "../z_assets_engine/mesh/"
#define ENGINE_SK_MESH_PATH "../z_assets_engine/skinned_mesh/"
#define ENGINE_MATERIAL_PATH "../z_assets_engine/material/"
#define ENGINE_MATERIAL_INSTANCE_PATH "../z_assets_engine/material_instance/"
#define ENGINE_AUDIO_PATH "../z_assets_engine/audio/"

#define ENGINE_TEXTURE_EXTENSION ".mtex"
#define ENGINE_FONTS_EXTENSION ".mfont"
#define ENGINE_MESH_EXTENSION ".mmesh"
#define ENGINE_SKMESH_EXTENSION ".mskin"
#define ENGINE_MATERIAL_EXTENSION ".mmat"
#define ENGINE_MATERIAL_INSTANCE_EXTENSION ".mmi"
#define ENGINE_AUDIO_EXTENSION ".maudio"


#define MAX_ASSETS_STRINGS 5000u

///////////////// RESOURCES COUNTS //////////////////////
#define MAX_MESH_COUNT 1024u
#define MAX_SKINNED_MESH_COUNT 1024u

#define MAX_TRANSFORM_COUNT (MAX_MESH_COUNT + MAX_SKINNED_MESH_COUNT)

//FONT/TEXT
//NOTE: this should realy be 40, but looks nicer as 32, im doing something wierd cause its casuing text padding
#define DEFAULT_FONT_CREATION_SIZE 32.0f
#define GLYPH_LENGTH 96u
#define GLYPH_START 32u
#define GLYPH_END 128u

#define MAX_MATERIAL_COUNT 100u

#define MAX_SPRITE_COUNT 1024u

#define MAX_TEXTURE_COUNT 1024u
#define MAX_FONT_COUNT 16u

#define MAX_TEXTURE_MEMORY_CPU GB(0.5)
#define MAX_MESH_MEMORY_CPU GB(0.5)


///////////////// RESOURCES AND HANDLES //////////////////////

#define INVALID_HANDLE 0


typedef struct Handle
{
    u32 id;
    u32 generation;
} Handle;

typedef enum Asset_Type
{
    ASSET_TEXTURE,
    ASSET_FONT,
    ASSET_SPRITE,
    ASSET_STATIC_MESH,
    ASSET_SKINNED_MESH,
    ASSET_AUDIO,
    ASSET_MATERIAL,
    ASSET_MATERIAL_INSTANCE,
    ASSET_SCENE,
    // RESOURCE_PARTICLE,

    ASSET_TYPE_MAX,
} Asset_Type;

const char* ASSET_TYPE_LUT[ASSET_TYPE_MAX] = {
    [ASSET_TEXTURE] = "ASSET_TEXTURE",
    [ASSET_FONT] = "ASSET_FONT",
    [ASSET_SPRITE] = "ASSET_SPRITE",
    [ASSET_STATIC_MESH] = "ASSET_STATIC_MESH",
    [ASSET_SKINNED_MESH] = "ASSET_SKINNED_MESH",
    [ASSET_AUDIO] = "ASSET_AUDIO",
    [ASSET_MATERIAL] = "ASSET_MATERIAL",
    [ASSET_MATERIAL_INSTANCE] = "ASSET_MATERIAL_INSTANCE",
    [ASSET_SCENE] = "ASSET_SCENE",
};


typedef enum Asset_Load_State
{
    ASSET_LOAD_STATE_UNLOADED,
    ASSET_LOAD_STATE_QUEUED,
    ASSET_LOAD_STATE_LOADED,
} Asset_Load_State;


typedef struct Asset_MetaData
{
    //meta data for our editor/debug builds
    MADNESS_UUID uuid; // not in use rn, but will be useful if i ever integrate asset renaming
    u64 hash; // hashes the engine_file
    Asset_Type type;
    String* source_file; //256 in length max
    String* engine_path; //256 in length max
} Asset_MetaData;

typedef struct Texture_Handle
{
    u32 handle;
    u32 generation;
} Texture_Handle;


typedef struct Madness_Asset
{
    //runtime format for assets
    u64 path_hash;
    u64 reference_count;
    Asset_Type type;
    String* engine_path; // should just be a reference, mainly for debugging
} Madness_Asset;

//Renderpass || translucency || Blend || Mesh Type
typedef u64 Material_ID;

typedef struct Material_Handle
{
    Material_ID material_id;
    u32 buffer_handle;
} Material_Handle;

typedef struct Madness_Mesh_Handle
{
    u32 handle;
} Madness_Mesh_Handle;

typedef struct Madness_Mesh_Handle_Internal
{
    u32 handle;
} Madness_Mesh_Handle_Internal;

typedef struct Madness_SkMesh_Handle
{
    u32 handle;
} Madness_SkMesh_Handle;

typedef struct Madness_SkMesh_Handle_Internal
{
    u32 handle;
} Madness_SkMesh_Handle_Internal;

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


///////////////// Texture  //////////////////////

/* TODO: replace asset type in madness texture
typedef enum Texture_Type
{
    TEXTURE_TYPE_TEXTURE,
    TEXTURE_TYPE_FONT,
} Texture_Type;
*/

typedef enum Texture_Format
{
    //TODO: when you figure it out properly
    Texture_Format_Default,
    // Texture_Format_Default,
} Texture_Format;

typedef enum Texture_Filter
{
    Texture_Filter_Nearest,
    Texture_Filter_Linear,
} Texture_Filter;

//TODO: wrap and sampler
/*
typedef enum Texture_Wrap
{
    Texture_Wrap_Repeat,
    Texture_Wrap_Mirror_Repeat,
    Texture_Wrap_Clamp_Edge,
    Texture_Wrap_Clamp_Border,
    Texture_Wrap_Mirror_Clamp_To_Edge,
} Texture_Wrap;

typedef struct Texture_Sampler
{
    Texture_Filter mag;
    Texture_Filter min;
    Texture_Wrap u_wrap;
    Texture_Wrap v_wrap;
} Texture_Sampler;
*/


//Texture
typedef struct Madness_Texture
{
    u32 width;
    u32 height;
    u8 channels;
    Texture_Format format;
    u64 pixels_size;
    Asset_Type type; // used to identify if we have are a font
    // runtime only data
    u32 font_index;
    // what is queried when we get the bindless slot, so that we can use a temp texture until the actual texture loads
    u32 generation;

    Asset_Load_State texture_load_state;
    //TODO:
    //bool has_sampler;
    // Texture_Sampler sampler;
} Madness_Texture;

typedef struct Texture_GPU_Upload
{
    Madness_Texture* madness_texture;
    u8* pixel_data;
    Heap_Allocator* texture_memory_allocator;
    u32 texture_index;
} Texture_GPU_Upload;

typedef struct Madness_Texture_Runtime
{
    u8 version;
    Madness_Texture texture;
    u8* pixel_data;
} Madness_Texture_Runtime;


typedef struct Glyph
{
    float width, height;
    float xoff, yoff;
    float advance;
    float u0, v0, u1, v1; // UV coordinates in atlas
} Glyph;


typedef struct Madness_Font
{
    //called Madness font cause a linux library uses the struct font
    // float font_creation_size; // the larger the more clear the text looks
    //NOTE: this will have to be larger if i support other languages or non standard characters
    Glyph glyphs[GLYPH_LENGTH]; //all ascii characters (that we would actually want to present) 128-32 = 96
} Madness_Font;

typedef struct Madness_Font_Runtime
{
    u8 version;
    Madness_Font font_texture;
    Madness_Texture texture;
    u8* pixel_data;
} Madness_Font_Runtime;


//////////////////////MATERIAL/SHADER/////////////////////////

typedef enum Shader_Mesh_Type
{
    Shader_Mesh_Type_Mesh,
    Shader_Mesh_Type_Skinned,
    // Shader_Mesh_Type_Particle,
    // Shader_Mesh_Type_Foilage,
} Shader_Mesh_Type;

typedef enum Shader_Transluency_Type
{
    Shader_Transluency_Type_Opaque,
    Shader_Transluency_Type_Transparent,
} Shader_Transluency_Type;


typedef enum Shader_Renderpass_Type
{
    //only to be used internally and not by the material system
    Renderpass_Type_Predepth = BITFLAG(0),
    Renderpass_Type_Shadow = BITFLAG(1),
    Renderpass_Type_Color = BITFLAG(2), // this is for both opaque and transparent
    Renderpass_Type_Reserved1 = BITFLAG(3),
    Renderpass_Type_Reserved2 = BITFLAG(4),
    Renderpass_Type_Reserved3 = BITFLAG(5),
    Renderpass_Type_Reserved4 = BITFLAG(6),
    Renderpass_Type_Reserved5 = BITFLAG(7),
} Shader_Renderpass_Type;


typedef enum Shader_Blend
{
    Shader_Blend_Mode_Default, // oqaque
    Shader_Blend_Mode_Alpha,
    Shader_Blend_Mode_PreMultiplied_Alpha,

    Shader_Blend_Mode_Additive,
    Shader_Blend_Mode_Soft_Additive, // soft make it harder for white to blow out the screen

    Shader_Blend_Mode_Multiply,
    Shader_Blend_Mode_Multiply2x,
    //  Shader_Blend_Mode_SCREEN,

    Shader_Blend_Mode_Subtract,
    Shader_Blend_Mode_Reverse_Subtract,

    //  Shader_Blend_Mode_MIN,
    //  Shader_Blend_Mode_MAX,
} Shader_Blend_Mode;


typedef enum Mesh_PBR_Flags
{
    //Where's bitflag 0???
    MESH_PIPELINE_COLOR = BITFLAG(1),
    MESH_PIPELINE_NORMAL = BITFLAG(2),
    MESH_PIPELINE_EMISSIVE = BITFLAG(3),
    MESH_PIPELINE_ROUGHNESS = BITFLAG(4),
    MESH_PIPELINE_METALLIC = BITFLAG(5),
    MESH_PIPELINE_AO = BITFLAG(6),
    MESH_PIPELINE_ENUM_MAX,
} Mesh_PBR_Flags;

typedef struct PC_General
{
    VkDeviceAddress draw_data_buffer;
    VkDeviceAddress material_buffer;
} PC_General;

typedef struct PC_Particle
{
    VkDeviceAddress draw_material_buffer;
    VkDeviceAddress unused;
} PC_Particle;


typedef struct PC_Shadow_Mapping
{
    mat4 light_matrix;
    VkDeviceAddress draw_data_buffer;
} PC_Shadow_Mapping;


typedef struct Material_Info
{
    Path_String* shader_name;
    Path_String* material_name;

    Shader_Renderpass_Type renderpass;
    Shader_Transluency_Type transluency;
    Shader_Mesh_Type mesh_type;
    Shader_Blend_Mode blend_mode;

    Material_ID material_id;
} Material_Info;

typedef struct Material_GPU_Definition
{
    u32 field_count;
    u32 struct_size;
    //arrays
    u64* name_hashes;
    u32* field_offsets;
    Reflection_Type* types;
} Material_GPU_Definition;

typedef struct Material_Asset
{
    //information about the material structure, think of it like the definition of a material/shader
    Material_Info material_info;
    Reflection_Runtime_Struct* reflection_material_data;
    Material_GPU_Definition* material_gpu_definition;
} Material_Asset;

typedef struct Material_Instance
{
    // NOTE: the material data is the serialized data containing the UUID for textures
    MADNESS_UUID material_asset_uuid;
    u64 data_size;
    void* material_data;
} Material_Instance;


typedef struct Material_Asset_Runtime
{
    u32 version;
    Material_Asset* asset;
} Material_Asset_Runtime;


typedef struct Material_Batch
{
    //to solve the problem of having different batches for the material types, it would make sense to have a sort key
    // https://realtimecollisiondetection.net/blog/?p=86

    Material_ID material_key;
    MADNESS_UUID material_asset_uuid;
    Material_Asset* material_asset;
    Dynamic_Array* material_data;
} Material_Batch;


///////////////// Particle  //////////////////////

typedef struct Particle
{
    //render data
    f32* pos_x;
    f32* pos_y;
    f32* pos_z;
    vec2s* rotation;
    vec2s* scale;

    vec4s* color;

    Texture_Handle* texture_handle; // TODO: change to a u32 and leave the handle on the emitter
    vec2s* tex_offset;
    vec2s* tex_size;

    //non render data
    f32* life_left;
    f32* vel_x;
    f32* vel_y;
    f32* vel_z;

} Particle;

typedef struct Particle_Mesh
{
    //render data
    f32* pos_x;
    f32* pos_y;
    f32* pos_z;

    f32* rot_x;
    f32* rot_y;
    f32* rot_z;

    f32* scale_x;
    f32* scale_y;
    f32* scale_z;

    Texture_Handle* texture_handle;
    vec2s* tex_offset;
    vec2s* tex_size;

    //not render data
    f32* lifetime_left;
    Madness_Mesh_Handle* mesh_handle;
} Particle_Mesh;

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
    f32 emission_rate;
    f32 spawn_trigger;

    vec4s particle_color;
    vec3s particle_velocity;
    vec3s particle_position;

    f32 particle_lifetime;

    Material_Instance* material_instance;


    bool infinite;
    bool is_visible;

    DYNAMIC_ARRAY_TYPE(u32)* particle;
} Particle_Emitter;



///////////////// MESH  //////////////////////

typedef enum Index_Type
{
    INDEX_TYPE_U16,
    INDEX_TYPE_U32,
} Index_Type;


typedef struct Madness_Animation
{
    GLTF_Animation_Data* animation_data; // pointer to the anim data

    //generated every frame
    mat4s* gpu_matrix;
    u64 skinned_matrix_offset; // offset in the array

    //stored in memory
    vec3s* local_translation;
    versors* local_rotation;
    vec3s* local_scale;

    u32 joint_count;

    //current animation data
    u32 current_animation_index;
    float current_time;
    bool looping;
} Madness_Animation;

typedef struct Madness_Skinned_Submesh_Instance
{
    //for the renderer
    u32 skinned_id;
    Material_Handle material_handle;
    Transform_Handle parent_transform_handle;
} Madness_Skinned_Submesh_Instance;

typedef struct Madness_Skinned_Mesh_Instance
{
    u32 mesh_count;
    Madness_Skinned_Submesh_Instance* submesh_instances;

    Madness_SkMesh_Handle_Internal skinned_mesh_asset;
    Transform_Handle transform_handle;

    Animation_Handle animation_handle;
    u32 skinned_matrix_count_offset;
} Madness_Skinned_Mesh_Instance;


typedef struct Madness_SubMesh_Instance
{
    u32 mesh_id;
    Material_Handle material_handle;
    Transform_Handle parent_transform_handle;
} Madness_SubMesh_Instance;

typedef struct Madness_Mesh_Instance
{
    //this generally is only for changing materials and transforms, and not for the renderer
    u32 mesh_count;
    Madness_Mesh_Handle_Internal mesh_asset;
    Transform_Handle transform_handle;
    Madness_SubMesh_Instance* submesh_instances;
} Madness_Mesh_Instance;


typedef struct Madness_Skinned_SubMesh
{
    u64 joint_bytes;
    u64 weight_bytes;


    //runtime data
    u64 joint_offset_vec4;
    u64 joint_offset_bytes;

    u64 weight_offset_vec4;
    u64 weight_offset_bytes;
} Madness_Skinned_SubMesh;

typedef struct Madness_SubMesh
{
    u64 tangent_bytes;
    u64 vertex_color_bytes;
    u64 vertex_bytes;
    u64 normal_bytes;
    u64 uv_bytes;
    u64 indices_bytes;

    u32 vertex_count; // this is also the count for basically every field except the index
    u32 index_count;
    Index_Type index_type;
} Madness_SubMesh;


typedef struct Madness_Mesh
{
    u32 mesh_count;
    Madness_SubMesh* mesh_data;
    u32* submesh_ids;
    Material_Instance* material_instance;
    Material_Handle* material_handles;

    u64 path_hash;
    String* engine_path;
    u32 generation;
    u32 reference_count;
} Madness_Mesh;


typedef struct Madness_Skinned_Mesh
{
    u32 mesh_count;
    Madness_SubMesh* mesh_data;
    Material_Instance* material_instance;
    //
    Madness_Skinned_SubMesh* skinned_mesh_data;
    GLTF_Animation_Data* animation_data;
} Madness_Skinned_Mesh;


// for loading in
typedef struct Madness_Mesh_GPU_Data
{
    vec4s* tangent;
    vec4s* vertex_color;
    vec3s* vertex;
    vec3s* normal;
    vec2s* uv;
    u8* indices;
} Madness_Mesh_GPU_Data;

typedef struct Madness_SkMesh_GPU_Data
{
    vec4s* joints;
    vec4s* weights;
} Madness_SkMesh_GPU_Data;

typedef struct Mesh_GPU_Upload
{
    u32 mesh_id;
    Madness_SubMesh* submesh;
    Madness_Mesh_GPU_Data* gpu_data;

    Heap_Allocator* mesh_memory_allocator; // ref
} Mesh_GPU_Upload;

typedef struct Skinned_Mesh_GPU_Upload
{
    Madness_Skinned_SubMesh* skinned_submesh;
    Madness_SkMesh_GPU_Data* skinned_gpu_data;
    //TODO: Heap_Allocator* mesh_memory_allocator; // ref
} Skinned_Mesh_GPU_Upload;


typedef struct Madness_Mesh_Runtime
{
    u32 version;
    u32 mesh_count;
    Madness_SubMesh* submeshes;
    Madness_Mesh_GPU_Data* mesh_gpu_upload;
    Material_Instance* material_instance;
} Madness_Mesh_Runtime;

typedef struct Madness_SkMesh_Runtime
{
    u32 version;
    u32 mesh_count;
    Madness_SubMesh* submeshes;
    Madness_Mesh_GPU_Data* mesh_gpu_upload;
    Material_Instance* material_instance;

    Madness_Skinned_SubMesh* skinned_submeshes;
    Madness_SkMesh_GPU_Data* skmesh_gpu_upload;
    GLTF_Animation_Data* animation_data;
} Madness_SkMesh_Runtime;


///////////////// Systems  //////////////////////


typedef struct Material_System
{
    Reflection_Registry* reflection_registry;

    //for now all the push constants are going to be hardcoded, there shouldn't be much varation between them most likely

    //sort material batches by their mesh type, possibly fine grain it later
    Material_Batch material_batch[100];
    u32 material_batch_count;


    Madness_Asset material_madness_asset[MAX_MATERIAL_COUNT];
    u32 material_madness_asset_count;
} Material_System;


//SPRITE


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


typedef struct Texture_System
{
    //handle 0 is always the default texture, it should never be allowed to be modified
    Texture_Handle default_texture_handle;

    //Textures
    Madness_Texture textures[MAX_TEXTURE_COUNT];
    Madness_Font font_textures[MAX_FONT_COUNT];

    u32 in_use_textures_count;
    u32 max_textures;

    RING_QUEUE_TYPE(u32)* available_texture_queue;
    RING_QUEUE_TYPE(u32)* available_font_queue;

    HASH_MAP_TYPE(u64, u32)* texture_hash_map;


    Madness_Asset texture_asset[MAX_TEXTURE_COUNT];
    u32 texture_asset_count;

    RING_QUEUE_TYPE(Texture_GPU_Upload)* texture_gpu_upload_queue;
} Texture_System;

typedef struct Scene
{
    String* scene_name;


    // String** engine_file;
    MADNESS_UUID* asset_uuid;
    u32 uuid_counts;

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
    //TODO: at some point im gonna need a free list cpu side, if i am to dynamically remove and add meshes,
    // fragmentation would also be a concern, unless i pool size, or split the pool into many different pool sizes

    //mesh_asset (has all the data) -> mesh_parent_instance (contains pointers to all the submeshes, is the thing we want the game to manipulate, also allows us to make material changes quickly)
    //-> mesh_instance (inside the material batches)


    Madness_Mesh madness_mesh[MAX_MESH_COUNT];
    u32 madness_mesh_count;

    Madness_Skinned_Mesh madness_skinned_mesh[MAX_SKINNED_MESH_COUNT];
    u32 madness_sk_mesh_count;

    Madness_Mesh_Instance mesh_instance[MAX_MESH_COUNT];
    u32 mesh_instance_count;

    Madness_Skinned_Mesh_Instance skinned_mesh_instance[MAX_MESH_COUNT];
    u32 skinned_mesh_instance_count;

    //TODO: this only counts up rn, likely to change it later, probably just a pool
    u32 mesh_ids;
    u32 skinned_ids;


    // data*, offset, byte_size ->for all the types
    RING_QUEUE_TYPE(Mesh_GPU_Upload)* mesh_ring_queue;
    RING_QUEUE_TYPE(Skinned_Mesh_Upload_Data)* skinned_mesh_ring_queue;


    //TODO:
    //anything that couldn't be loaded in this frame
    RING_QUEUE_TYPE(const char*)* load_queue;


    Madness_Asset skinned_madness_asset[100];
    u32 skinned_madness_asset_count;
} Mesh_System;


typedef struct Animation_System
{
    Frame_Allocator* frame_allocator;

    size_t skinned_matrix_offset_count; //counts in mat4s

    Array* skinned_matrix_array; // sent to the gup

    Madness_Animation animation_data[MAX_SKINNED_MESH_COUNT];
    u32 animation_count;
} Animation_System;


typedef struct Particle_System
{
    //memory, arena, frame_arena

    //individual particles
    //animation data

    //vertex/index buffer
    Particle_Emitter* emitters;
    u32 emitter_count;
    u32 emitter_count_max;

    //OPTIMIZE: read/consume buffers, agnis square enix article, for compute updates
    //NOTE: for multithreading, we can have each thread manage their own particles pools,
    // each emitter/system(up to me how fine grain i want to be) belongs to a thread and only updated on that thread
    Particle particles;
    u32 particles_count;

    u32* dead_particles;
    u32 dead_particles_available;
    u32 dead_particles_count;

    //TODO: might want to look into ways to use a pool allocator
    Heap_Allocator* heap_allocator;

} Particle_System;


//RENDER PACKET

//these are all just references to the data, they do not own anything
typedef struct Render_Packet_3D
{
    //geometry data for indirect draws

    //TODO: we should have a dirty bit for generating any new batches
    Material_Batch* material_batch;
    u32 material_batch_count;

    Madness_Mesh_Instance* mesh_instances;
    u32 mesh_instances_count;

    Madness_Skinned_Mesh_Instance* skinned_instances;
    u32 skinned_instances_count;


    mat4s* world_space_matrix_array;
    u32 world_space_matrix_count;

    ARRAY_TYPE(mat4)* skinned_matrix;
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

    Particle_Emitter* emitters;
    u32 emitter_count;
} Render_Packet_Particle;

typedef struct Render_Packet
{
    //just references
    RING_QUEUE_TYPE(Mesh_Upload_Data)* mesh_queue;
    RING_QUEUE_TYPE(Skinned_Mesh_Upload_Data)* skinned_mesh_queue;
    RING_QUEUE_TYPE(Texture_GPU_Upload)* texture_upload_queue;


    //FOR RENDERING

    //rn we just have one of each,
    Render_Packet_Sprite sprite_data_packet;
    Render_Packet_UI ui_data_packet;
    Render_Packet_3D draw_3d_data_packet;
    Render_Packet_Particle particle_packet;
} Render_Packet;


typedef struct Asset_Registry
{
    DYNAMIC_ARRAY_TYPE(Asset_MetaData)* asset_meta_data;

    //ideally we have another data format which points to the data?? just for easy lookup and display
} Asset_Registry;


typedef struct Asset_System
{
    //the asset system is just a container for all the system,
    //gather the cpu-gpu resources and send them to renderer

    //Systems
    // TODO: might change this into a pool allocator, or even segregated list allocator
    Heap_Allocator* heap_allocator;
    Frame_Allocator* frame_allocator;
    Allocator* allocator;


    //half a gig should be good for now
    //only specifically for pixel data
    //TODO: also hope to god it doesn't fragment badly, otherwise we will need to defrag this
    // alternative is to simply just wait until memory is available for use,
    // or maybe a cache system to evict, but doesn't make sense, since we will literally need that thing in the world
    Heap_Allocator* texture_allocator;
    Heap_Allocator* mesh_allocator;

    Reflection_Registry* global_reflection_registry; // ref

    // Shader_System* shader_system;
    // Material_System* shader_system; //probably want a material system, but not a shader system here, but in the renderer
    Sprite_System* sprite_system;
    Mesh_System* mesh_system;
    Texture_System* texture_system;
    Material_System* material_system;
    Scene* scene;
    Animation_System* animation_system;

    Particle_System* particle_system;

    //Render Packet
    Render_Packet* render_packet;

    Asset_Registry* asset_registry;
} Asset_System;


#endif //RESOURCE_TYPES_H
