#ifndef RESOURCE_TYPES_H
#define RESOURCE_TYPES_H


#include <stdalign.h>

#include "array_freelist.h"
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

#define IMPORT_RESOURCE_PATH "../z_assets/asset_list"
#define IMPORT_TEXTURE_PATH "../z_assets/texture"
#define IMPORT_FONTS_PATH "../z_assets/fonts"
#define IMPORT_MESH_PATH "../z_assets/mesh"
#define IMPORT_SK_MESH_PATH "../z_assets/skinned_mesh"
#define IMPORT_AUDIO_PATH "../z_assets/audio"
#define IMPORT_RESOURCE_PATH "../z_assets/asset_list"

#define ENGINE_RESOURCE_PATH "../z_assets_engine/asset_list/"
#define ENGINE_TEXTURE_PATH "../z_assets_engine/texture/"
#define ENGINE_FONTS_PATH "../z_assets_engine/fonts/"
#define ENGINE_MESH_PATH "../z_assets_engine/mesh/"
#define ENGINE_SK_MESH_PATH "../z_assets_engine/skinned_mesh/"
#define ENGINE_MATERIAL_PATH "../z_assets_engine/material/"
#define ENGINE_AUDIO_PATH "../z_assets_engine/audio/"

#define ENGINE_TEXTURE_EXTENSION ".mtex"
#define ENGINE_FONTS_EXTENSION ".mfont"
#define ENGINE_MESH_EXTENSION ".mmesh"
#define ENGINE_SKMESH_EXTENSION ".mskin"
#define ENGINE_Material_EXTENSION ".mmat"
#define ENGINE_AUDIO_EXTENSION ".maudio"


///////////////// RESOURCES  //////////////////////

typedef enum Asset_Type
{
    ASSET_TEXTURE,
    ASSET_FONT,
    ASSET_SPRITE,
    ASSET_STATIC_MESH,
    ASSET_SKINNED_MESH,
    ASSET_AUDIO,
    // RESOURCE_PARTICLE,

    ASSET_MAX,
} Asset_Type;

typedef struct Madness_Asset
{
    //runtime format for assets
    u64 hash_id; //future: might need to add this back
    Asset_Type type;
    u64 handle_lookup; // if we wanted to access this item within the specific system
    u64 reference_count;
} Madness_Asset;


typedef struct Asset_MetaData
{
    //meta data for our editor/debug builds
    MADNESS_UUID uuid;
    Asset_Type type;
    String* source_file; // TODO: change to string
    String* binary_file; // TODO: change to string
} Asset_MetaData;





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
    Asset_Type type; // font or texture
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








///////////////// Texture  //////////////////////

typedef enum Texture_Format
{
    //TODO: when you figure it out properly
    Texture_Format_Default,
    // Texture_Format_Default,
} Texture_Format;


//Texture
typedef struct Madness_Texture
{
    u32 width;
    u32 height;
    u8 channels;
    Texture_Format format;
    u64 pixels_size;
    Asset_Type type; // used to identify if we have are a font
    u32 font_index; // runtime only data
} Madness_Texture;

typedef struct Texture_GPU_Upload
{
    Madness_Texture* madness_texture;
    u8* pixel_data;
    u32 bindless_location;
}Texture_GPU_Upload;

typedef struct Madness_Texture_Editor
{
    Madness_Texture texture;
    u8 version;
    u8* pixel_data;
}Madness_Texture_Editor;

typedef struct Madness_Texture_Runtime
{
    Madness_Texture texture;
    u8* pixel_data;
}Madness_Texture_Runtime;


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


typedef struct Madness_Font
{
    //called Madness font cause a linux library uses the struct font
    // float font_creation_size; // the larger the more clear the text looks
    //NOTE: this will have to be larger if i support other languages or non standard characters
    Glyph glyphs[GLYPH_LENGTH]; //all ascii characters (that we would actually want to present) 128-32 = 96
} Madness_Font;

typedef struct Madness_Font_Editor
{
    Madness_Font font_texture;
    Madness_Texture texture;
    u8 version;
    u8* pixel_data;
}Madness_Font_Editor;

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
    vec3s velocity;


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

typedef enum Index_Type
{
    INDEX_TYPE_U16,
    INDEX_TYPE_U32,
} Index_Type;


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

} Mesh_Indirect_Draw;

typedef struct SKMesh_GPU_Draw
{
    u32 transform_idx;
    u32 material_instance_handle;
    u32 joint_idx;
    u32 weight_idx;
    u32 vertex_offset;
} SKMesh_GPU_Draw;

typedef struct Mesh_GPU_Draw
{
    u32 transform_idx;
    u32 material_instance_handle;
} Mesh_GPU_Draw;



typedef struct Skinned_Mesh_GPU_Upload
{
    u64 joint_bytes;
    u64 weight_bytes;

    u64 joint_offset;
    u64 weight_offset;


    vec4s* joints;
    vec4s* weights;
} Skinned_Mesh_GPU_Upload;

typedef struct Mesh_GPU_Upload
{
    //information to upload into the associated buffer
    u32 vertex_offset;
    u32 vertex_bytes;

    u32 vertex_color_offset;
    u32 vertex_color_bytes;

    u32 indices_bytes;
    VkIndexType index_type;

    u32 normal_offset;
    u32 normal_bytes;

    u32 tangent_offset;
    u32 tangent_bytes;

    u32 uv_offset;
    u32 uv_bytes;


    vec4s* tangent;
    vec4s* vertex_color;
    vec3s* vertex;
    vec3s* normal;
    vec2s* uv;
    u8* indices;
} Mesh_GPU_Upload;

typedef struct Madness_Skinned_SubMesh_Instance
{
    //for the renderer
    Mesh_Indirect_Draw mesh_indirect_draw;
    SKMesh_GPU_Draw sk_mesh_gpu_draw;
    Material_Handle material_handle;
    Transform_Handle parent_transform_handle;
} Madness_Skinned_SubMesh_Instance;

typedef struct Madness_Skinned_Mesh_Instance
{
    u32 mesh_count;
    Madness_Skinned_SubMesh_Instance* sk_mesh_instance_array;

    Sk_Mesh_Asset_Handle sk_mesh_handle;
    Transform_Handle transform_handle;

    //OPTIMIZE: we would want a handle to the data, and access if needed, so that the updates are more cache friendly
    //generated every frame
    mat4s* gpu_matrix;

    //stored in memory
    vec3s* local_translation;
    versors* local_rotation;
    vec3s* local_scale;

    u32 joint_count;

    //current animation data
    u32 current_animation_index;
    float current_time;
    bool looping;
} Madness_SkMesh_Instance;



typedef struct Madness_SubMesh_Instance
{
    //for the renderer
    Mesh_Indirect_Draw mesh_indirect_draw;
    Mesh_GPU_Draw mesh_gpu_draw;
    Material_Handle material_handle;
    Transform_Handle parent_transform_handle;
} Madness_SubMesh_Instance;

typedef struct Madness_Mesh_Instance
{
    //this generally is only for changing materials and transforms, and not for the renderer
    Mesh_Asset_Handle mesh_asset;
    Transform_Handle transform_handle;
    u32 mesh_count;
    Madness_SubMesh_Instance* submesh_instances;
} Madness_Mesh_Instance;




typedef struct Madness_Skinned_SubMesh
{
    u64 joint_bytes;
    u64 weight_bytes;
    u64 skinned_matrix_bytes;
    u64 skinned_matrix_count;

    //runtime data
    u64 joint_offset_vec4;
    u64 joint_offset_bytes;

    u64 weight_offset_vec4;
    u64 weight_offset_bytes;

    u64 skinned_matrix_offset;
} Madness_Skinned_SubMesh;

typedef struct Madness_SubMesh{
    u64 tangent_bytes;
    u64 vertex_color_bytes;
    u64 vertex_bytes;
    u64 normal_bytes;
    u64 uv_bytes;
    u64 indices_bytes;

    u32 vertex_count; // this is also the count for basically every field except the index
    u32 index_count;
    Index_Type index_type;

    //Runtime Data and for Unloading the Mesh
    //info for indirect draw
    u32 vertex_offset; //in vec3
    u32 index_offset; //uint32_t    firstIndex; // offset into the index buffer

    u32 tangent_offset;
    u32 vertex_color_offset;
    u32 normal_offset;
    u32 uv_offset;
}Madness_SubMesh;



typedef struct Madness_Mesh
{
    //asset file data
    u32 mesh_count;
    Madness_SubMesh* mesh_data;
    //material data probably
} Madness_Mesh;



typedef struct Madness_Skinned_Mesh
{
    u32 mesh_count;
    Madness_SubMesh* mesh_data;
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
}Madness_Mesh_GPU_Data;



typedef struct Madness_Mesh_Editor
{
    u32 version;
    u32 mesh_count;
    Madness_SubMesh* sub_mesh;
    Madness_Mesh_GPU_Data* mesh_gpu_upload;
}Madness_Mesh_Editor;


typedef struct Madness_SkMesh_Editor
{
    u32 version;
    u32 mesh_count;
    Madness_SubMesh* sub_mesh;
    Madness_Mesh_GPU_Data* mesh_gpu_upload;

}Madness_SkMesh_Editor;







///////////////////////MATERIAL/SHADER/////////////////////////

typedef enum Shader_Mesh_Type
{
    Shader_Mesh_Type_Mesh,
    Shader_Mesh_Type_Skinned,
} Shader_Mesh_Type;

typedef enum Shader_Pass_Type
{
    Shader_Pass_Type_Opaque = BITFLAG(0),
    Shader_Pass_Type_Transparent = BITFLAG(1),
    Shader_Pass_Type_Shadow = BITFLAG(2),
} Shader_Pass_Type;

typedef enum Renderpass_Single_Type
{
    //only to be used internally and not by the material system
    Depth_Test_Type_Predepth,
    Renderpass_Type_Shadow,
    Renderpass_Type_Opaque,
    Renderpass_Type_Transparent,
} Renderpass_Single_Type;


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

typedef enum Shader_Stage_Type
{
    Shader_Stage_Type_Vert,
    Shader_Stage_Type_Graphics, // both vert and frag
    Shader_Stage_Type_Compute,
    // Shader_Stage_Type_Tesselation,
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
}PC_Shadow_Mapping;

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
    Material_Batch mesh_batch[100];
    u32 mesh_batch_count;

    Material_Batch skinned_batch[100];
    u32 skinned_batch_count;

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
#define MAX_FONT_COUNT 16


typedef struct Texture_System
{
    //handle 0 is always the default texture, it should never be allowed to be modified
    Texture_Handle default_texture_handle;

    //Textures
    Madness_Texture texture_array[MAX_TEXTURE_COUNT];
    Madness_Font font_array[MAX_FONT_COUNT];

    Texture_Handle texture_handles[MAX_TEXTURE_COUNT];

    u32 in_use_textures_count;
    u32 max_textures;
    RING_QUEUE_TYPE(u32)* available_texture_queue;
    RING_QUEUE_TYPE(u32)* available_font_queue;

    hash_map* texture_hash_map;

    //textures that the renderer needs to upload to the gpu
    RING_QUEUE_TYPE(Texture_GPU_Upload)* texture_upload_queue;

    Madness_Asset texture_asset[MAX_TEXTURE_COUNT];
    u32 texture_asset_count;

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


    Madness_Mesh madness_mesh[MAX_MESH_COUNT];
    u32 mesh_asset_count;

    Madness_Skinned_Mesh skinned_mesh_asset_data[MAX_MESH_COUNT];
    u32 sk_mesh_asset_count;

    Madness_Mesh_Instance mesh_parent_instance[MAX_MESH_COUNT];
    u32 mesh_parent_instance_count;

    Madness_SkMesh_Instance skinned_mesh_instance[MAX_MESH_COUNT];
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
    Material_Batch* mesh_batch;
    u32 mesh_batch_count;

    Material_Batch* skinned_batch;
    u32 skinned_batch_count;

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


typedef struct Asset_Registry
{
    DYNAMIC_ARRAY_TYPE(Asset_MetaData)* asset_meta_data;

}Asset_Registry;


typedef struct Resource_System
{
    //the asset system is just a container for all the system,
    //gather the cpu-gpu resources and send them to renderer

    //Systems
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

    //Render Packet
    Render_Packet* render_packet;

    Asset_Registry* asset_registry;

} Asset_System;


#endif //RESOURCE_TYPES_H
