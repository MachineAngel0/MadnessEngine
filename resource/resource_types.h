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
#define ENGINE_SHADER_PATH "../z_assets_engine/material/"
#define ENGINE_SHADER_PATH_NO_SLASH "../z_assets_engine/material"
#define ENGINE_MATERIAL_INSTANCE_PATH "../z_assets_engine/material_instance/"
#define ENGINE_AUDIO_PATH "../z_assets_engine/audio/"
#define ENGINE_PARTICLE_PATH "../z_assets_engine/particle/"
#define ENGINE_PARTICLE_EFFECT_PATH "../z_assets_engine/particle/particle_effect"
#define ENGINE_PARTICLE_EMITTER_PATH "../z_assets_engine/particle/particle_emitter"



#define ENGINE_TEXTURE_EXTENSION ".mtex"
#define ENGINE_FONTS_EXTENSION ".mfont"
#define ENGINE_MESH_EXTENSION ".mmesh"
#define ENGINE_SKMESH_EXTENSION ".mskin"
#define ENGINE_SHADER_EXTENSION ".mmat"
#define ENGINE_MATERIAL_INSTANCE_EXTENSION ".mmi"
#define ENGINE_AUDIO_EXTENSION ".maudio"
#define ENGINE_PARTICLE_EFFECT_EXTENSION ".mparticle"
#define ENGINE_PARTICLE_EMITTER_EXTENSION ".memitter"




#define MAX_ASSETS_STRINGS 5000u

///////////////// RESOURCES COUNTS //////////////////////
#define MAX_MESH_COUNT 1024u
#define MAX_SKINNED_MESH_COUNT 1024u

#define MAX_TRANSFORM_COUNT (MAX_MESH_COUNT + MAX_SKINNED_MESH_COUNT)

#define MAX_PARTICLE_COUNT 1000
#define MAX_PARTICLE_EMITTER_COUNT 100
#define MAX_PARTICLE_EFFECTS_COUNT 10


#define MATERIAL_BUFFER_SIZE MB(16)



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
    ASSET_PARTICLE_EFFECT,
    ASSET_PARTICLE_EMITTER,

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
    [ASSET_PARTICLE_EFFECT] = "ASSET_PARTICLE_EFFECT",
    [ASSET_PARTICLE_EMITTER] = "ASSET_PARTICLE_EMITTER",

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

typedef struct Madness_Asset
{
    //runtime format for assets
    u64 path_hash;
    u64 reference_count;
    Asset_Type type;
    String* engine_path; // should just be a reference, mainly for debugging
} Madness_Asset;


typedef struct Texture_Handle
{
    u32 handle;
    u32 generation;
} Texture_Handle;





//Renderpass || translucency || Blend || Mesh Type
typedef u64 Shader_Key;

typedef struct Material_Handle
{
    // Material_Key material_id;
    u32 handle;
    u32 generation;
} Material_Handle;


typedef struct Shader_Handle
{
    u32 handle;
} Shader_Handle;


typedef struct Mesh_Handle
{
    u32 handle;
    u32 generation;
} Mesh_Handle;


typedef struct Skinned_Mesh_Handle
{
    u32 handle;
} Skinned_Mesh_Handle;

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

typedef struct Particle_Emitter_Handle
{
    u32 handle;
    u32 gen;
} Particle_Emitter_Handle;

typedef struct Particle_Effect_Handle
{
    u32 handle;
    u32 gen;
} Particle_Effect_Handle;

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


typedef enum Shader_Blend_Mode
{
    // oqaque
    Shader_Blend_Mode_Default,

    Shader_Blend_Mode_Additive,
    Shader_Blend_Mode_Soft_Additive, // soft make it harder for white to blow out the screen

    Shader_Blend_Mode_Multiply,
    Shader_Blend_Mode_Multiply2x,
    //  Shader_Blend_Mode_SCREEN,

    //NOTE: these dont need sorting but may have artifacts
    Shader_Blend_Mode_Subtract,
    Shader_Blend_Mode_Reverse_Subtract,

    //  Shader_Blend_Mode_MIN,
    //  Shader_Blend_Mode_MAX,

    //NOTE: Requires sorting
    Shader_Blend_Mode_Alpha,
    Shader_Blend_Mode_PreMultiplied_Alpha,

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
    VkDeviceAddress material_buffer;
    VkDeviceAddress particle_draw_buffer;
} PC_Particle;


typedef struct PC_Shadow_Mapping
{
    mat4 light_matrix;
    VkDeviceAddress draw_data_buffer;
} PC_Shadow_Mapping;


typedef struct Shader_Info
{
    Path_String* shader_name;
    Shader_Blend_Mode blend_mode;
    bool two_sided;
} Shader_Info;

typedef struct Shader_Asset
{
    //information about the material structure, think of it like the definition of a material/shader
    u32 version;
    u32 reflection_hash;
    MADNESS_UUID uuid;
    Shader_Info shader_info;
    Shader_Key shader_key;
} Shader_Asset;

typedef struct Material_GPU_Definition
{
    u32 field_count;
    u32 struct_size;
    //arrays
    u64* name_hashes;
    u32* field_offsets;
    Reflection_Type* types;
} Material_GPU_Definition;

typedef struct Material_Definition
{
    u32 reflection_hash;
    Reflection_Runtime_Struct reflection_material_data;
    Material_GPU_Definition material_gpu_definition;
} Material_Definition;

typedef struct Material_Meta_Data
{
    // NOTE: the material data is the serialized data containing the UUID for textures
    MADNESS_UUID shader_uuid;
    MADNESS_UUID material_uuid;

    u16 shader_id;
    u16 permutation;

    String* material_name;
    String* name;
} Material_Meta_Data;

typedef struct Material_Data
{
    //we load this in and then unload it once copied into the material array
    u64 data_size;
    void* material_data;
} Material_Data;

typedef struct Material
{
    Material_Meta_Data meta_data;
    Material_Data cpu_data;
} Material;




///////////////// Particle  //////////////////////

typedef struct Particle
{
    //render data
    f32* pos_x;
    f32* pos_y;
    f32* pos_z;

    f32* rot_x;
    f32* rot_y;

    f32* scale_x;
    f32* scale_y;

    vec4s* color;

    Texture_Handle* texture_handle; // TODO: change to a u32 and leave the handle on the emitter
    vec2s* tex_offset;
    vec2s* tex_size;

    //non render data
    f32* life_left;
    f32* vel_x;
    f32* vel_y;
    f32* vel_z;
    f32* gravity_x;
    f32* gravity_y;
    f32* gravity_z;
    // f32* drag;

    u64* material_key;
    u32* material_id;

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
    Mesh_Handle* mesh_handle;
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

typedef struct Particle_Emitter_Data
{
    //lifetime
    f32 emission_rate;
    f32 spawn_trigger;
    f32 particle_lifetime;
    f32 particle_lifetime_variance;


    //properties
    vec4s particle_color;

    vec3s position_variance;


    vec3s scale;
    vec3s scale_variance;
    vec3s rotation;


    vec3s velocity;
    vec3s velocity_variance;

    vec3s gravity;


} Particle_Emitter_Data;


typedef struct Particle_Emitter
{
    //serializable data
    Particle_Emitter_Data data;
    String* name;
    MADNESS_UUID material_uuid;

    //runtime data
    Material_Handle material_handle;

    vec3s position; // given to us by the effect

    bool in_use; // for the editor

    // u32 Maximum_Particles; // it would be good to have a limit for many reasons
    DYNAMIC_ARRAY_TYPE(u32)* particle;
} Particle_Emitter;

typedef struct Particle_Effect
{
    //the thing that manages the Particle_Emitters and their lifetimes

    //NOTE: for more particle emitter types you can use a union

    //TODO: temp count for testing, switch to a dynamic array later if needed,
    // or just max size them to something reasonable like 8
    //manage the emitters, when they start and stop
    MADNESS_UUID emmiter_uuid[4];
    u32 emitter_count;

    u32 emitters_start[4];
    u32 emitters_end[4];

    u32 effect_current_time;
    u32 effect_length;

    Transform transform;

    // emitter location offset in relation to the particle effects base tramsform
    vec3s emitter_position[4];


    String* name;

    //runtime data
    Particle_Emitter* emitters[4];

    bool infinite;
    bool is_visible;


} Particle_Effect;

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
    u32 start_submesh_index; // guaranteed to be continous and have all the data we need when accessed

    u32 mesh_asset_index;
    Transform_Handle transform_handle;

    Animation_Handle animation_handle;
    u32 skinned_matrix_count_offset;
} Madness_Skinned_Mesh_Instance;


typedef struct Madness_SubMesh_Instance
{
    u32 mesh_asset_index;
    u32 parent_instance_index;
    Material_Handle material_handle;
    Transform_Handle parent_transform_handle; // here for easy gpu usage, without the indirect lookup into the parent
} Madness_SubMesh_Instance;

typedef struct Madness_Mesh_Instance
{
    u32 mesh_count;
    u32 start_submesh_index; // guaranteed to be continous and have all the data we need when accessed
    //this generally is only for changing materials and transforms, and not for the renderer
    u32 mesh_asset_index;
    Transform_Handle transform_handle;
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
    MADNESS_UUID* material_uuid;
    Material_Handle* material_handles;
} Madness_Mesh;


typedef struct Madness_Skinned_Mesh
{
    u32 mesh_count;
    Madness_SubMesh* mesh_data;
    MADNESS_UUID* material_instance;
    Material_Handle* material_handles;
    // Madness_Mesh madness_mesh;
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
    u32 submesh_id;
    Madness_SubMesh* submesh;
    Madness_Mesh_GPU_Data* gpu_data;

    Heap_Allocator* mesh_memory_allocator; // ref
} Mesh_GPU_Upload;

typedef struct Skinned_Mesh_GPU_Upload
{

    Madness_Skinned_SubMesh* skinned_submesh;
    Madness_SkMesh_GPU_Data* skinned_gpu_data;
    //TODO: Heap_Allocator* mesh_memory_allocator; // ref

    Heap_Allocator* mesh_memory_allocator; // ref
} Skinned_Mesh_GPU_Upload;


typedef struct Madness_Mesh_Runtime
{
    u32 version;
    u32 mesh_count;
    Madness_SubMesh* submeshes;
    Madness_Mesh_GPU_Data* mesh_gpu_upload;
    MADNESS_UUID* material_uuid;
    MADNESS_UUID mesh_uuid;
} Madness_Mesh_Runtime;

typedef struct Madness_SkMesh_Runtime
{
    u32 version;
    u32 mesh_count;
    Madness_SubMesh* submeshes;
    Madness_Mesh_GPU_Data* mesh_gpu_upload;
    MADNESS_UUID* material_uuid;
    MADNESS_UUID skinned_mesh_uuid;

    Madness_Skinned_SubMesh* skinned_submeshes;
    Madness_SkMesh_GPU_Data* skmesh_gpu_upload;
    GLTF_Animation_Data* animation_data;
} Madness_SkMesh_Runtime;


///////////////// Systems  //////////////////////

typedef struct Shader_Mat_Mapping{
    String* shader_name[MAX_MATERIAL_COUNT];
    String* material_name[MAX_MATERIAL_COUNT];
    Material_Definition material_definition[MAX_MATERIAL_COUNT];
}Shader_Mat_Mapping;

typedef struct Material_Record
{
    u64 offset;
    u64 size;
}Material_Record;


typedef struct Material_System
{

    //for now all the push constants are going to be hardcoded, there shouldn't be much varation between them most likely

    //NOTE: each shader asset get its own material definition,
    // this is technically duplicating what should only exist once but rn its fine

    Shader_Asset shader_asset[MAX_MATERIAL_COUNT];
    u32 shader_asset_to_mapping[MAX_MATERIAL_COUNT];
    u32 shader_count;

    //TODO:
    RING_QUEUE_TYPE(Shader_Asset)* new_shaders_pipeline_creation;

    //tells us the mapping between shader and material, as well as the material definition
    Shader_Mat_Mapping shader_to_material_mapping;
    u32 shader_to_material_count;


    //TODO: some way to get the shader information by using the material handle
    //all use the same index
    Material_Record material_record[MAX_MATERIAL_COUNT];
    Material materials[MAX_MATERIAL_COUNT];
    u32 generation[MAX_MATERIAL_COUNT];
    u64 material_record_count;
    // u32 material_index_to_shader_index[MAX_MATERIAL_COUNT];

    //TODO:
    // u32* record_freelist[MAX_MATERIAL_COUNT];
    // u64 record_freelist_count;

    //we can have two buffers, the active one gets uploaded into the cpu,
    // the second one, gets cleared then, traverses the material record and uploads data into it from the active buffer, updating the offsets
    // then the next frame, we do a swap

    uint8_t* material_buffer;
    u64 material_buffer_offset;
    u64 material_buffer_size;

    //TODO: might not be a bad idea to have a cpu based buffer as well for development

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
    MADNESS_UUID default_texture_uuid;

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


    //mesh
    Madness_Mesh madness_mesh[MAX_MESH_COUNT];
    u32 madness_mesh_count;

    Madness_Mesh_Instance mesh_instance[MAX_MESH_COUNT];
    u32 mesh_instance_count;

    Madness_SubMesh_Instance submesh_instances[MAX_MESH_COUNT];
    u32 submesh_instance_count;

    //skinned
    Madness_Skinned_Mesh madness_skinned_mesh[MAX_SKINNED_MESH_COUNT];
    u32 madness_sk_mesh_count;

    Madness_Skinned_Mesh_Instance skinned_mesh_instance[MAX_MESH_COUNT];
    u32 skinned_mesh_instance_count;

    Madness_Skinned_Submesh_Instance skinned_submesh[MAX_MESH_COUNT];
    u32 skinned_submesh_count;

    // data*, offset, byte_size ->for all the types
    RING_QUEUE_TYPE(Mesh_GPU_Upload)* mesh_ring_queue;
    RING_QUEUE_TYPE(Skinned_Mesh_Upload_Data)* skinned_mesh_ring_queue;


    //TODO:
    //anything that couldn't be loaded in this frame
    RING_QUEUE_TYPE(const char*)* load_queue;


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
    //TODO: change the available effects and emitters to dynamic arrays to save on memory

    //TODO: reserve the particle effect and emitter 0 slots for defaults/invalid handles

    Particle_Effect* particle_effects;
    u32* particle_generation;
    u32 particle_effects_count_max;

    u32* free_particle_effects;
    u32 free_particle_effects_count;

    ARRAY_TYPE(u32)* active_effects;


    Particle_Emitter* emitters;
    u32* emitter_generation;
    u32 emitter_count_max;

    u32* free_emitters;
    u32 free_emitters_count;

    ARRAY_TYPE(u32)* active_emitters;



    //OPTIMIZE: read/consume buffers, agnis square enix article, for compute updates
    //NOTE: for multithreading, we can have each thread manage their own particles pools,
    // each emitter/system(up to me how fine grain i want to be) belongs to a thread and only updated on that thread
    Particle particles;
    u32 particles_count;

    u32* dead_particles;
    u32 dead_particles_available;
    u32 dead_particles_count;


    //defaults/fallbacks
    Particle_Effect* default_effect;
    Particle_Effect_Handle default_effect_handle;

    Particle_Emitter* default_emitter;
    Particle_Emitter_Handle default_emitter_handle;



    //TODO: might want to look into ways to use a pool allocator
    Heap_Allocator* heap_allocator;
} Particle_System;


//RENDER PACKET

//these are all just references to the data, they do not own anything
typedef struct Render_Packet_3D
{

    Scene* scene;


    //geometry data for indirect draws

    //TODO: we should have a dirty bit for generating any new batches
    Shader_Asset* shader_assets;
    u32 material_count;

    u8* material_buffer;
    u64 material_buffer_byte_size;


    Madness_Mesh_Instance* mesh_instances;
    u32 mesh_instances_count;

    Madness_SubMesh_Instance* submesh_instances;
    u32 submesh_instances_count;


    Madness_Skinned_Mesh_Instance* skinned_instances;
    u32 skinned_instances_count;


    mat4s* world_space_matrix_array;
    u32 world_space_matrix_count;

    ARRAY_TYPE(mat4)* skinned_matrix;
} Render_Packet_3D;


typedef struct Render_Packet_UI
{
    UI_Render_Packet madness_ui_render_packet;
    Insanity_UI_Render_Packet insanity_ui_render_packet;
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

    //OPTIMIZE: a hash would make way more sense for this, profile to be sure
    // HASH_TABLE_TYPE(Madness_Asset)* path_to_mesh_asset;

    // DYNAMIC_ARRAY_TYPE(Madness_Asset) loaded_asset;
    // ref counted asset
    Madness_Asset particle_effect_asset[MAX_PARTICLE_EFFECTS_COUNT];

    Madness_Asset particle_emitter_asset[MAX_PARTICLE_EMITTER_COUNT];

    Madness_Asset shader_madness_asset[MAX_MATERIAL_COUNT];

    Madness_Asset mesh_asset[MAX_PARTICLE_EFFECTS_COUNT];


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
    Reflection_Registry* material_reflection_registry; // ref

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
