#ifndef RESOURCE_TYPES_H
#define RESOURCE_TYPES_H


#include "ring_queue.h"
#include "stb_image.h"
#include "stb_truetype.h"
#include "transforms.h"


/// HANDLES ///

#define INVALID_HANDLE 0

typedef struct Texture_Handle
{
    u32 handle;
} Texture_Handle;

typedef struct Material_Handle
{
    u32 handle;
} Material_Handle;


typedef struct Mesh_Handle
{
    u32 handle;
} Mesh_Handle;


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
}Resource_Type;

typedef struct Resource_MetaData
{
    //TODO:
    Resource_Type type;
    const char* file_path;
    u64 handle;
}Resource_MetaData;


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

typedef struct Texture_Reference
{
    Texture_Handle handle;
    u64 reference_count;
} Texture_Reference;


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


typedef struct vertex_mesh
{
    vec3* pos;
    vec3* normal;
    vec4* tangent;
    vec2* uv;

    // vec4* color; //might not support
} vertex_mesh;

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
    Transform transform;

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


    u32 mesh_pipeline_mask; // determines what material features this submesh has
} submesh;


typedef struct static_mesh
{
    submesh* mesh;
    // the number of meshes in the model
    u32 mesh_size;
} static_mesh;

// TODO: skinned mesh
typedef struct skinned_mesh
{
    submesh* mesh;
    // the number of meshes in the model
    u32 mesh_size;
    VkDrawIndexedIndirectCommand* indirect_draw_array; // has the same size as mesh size

    //Joints joints;
    //Weights weights;
} skinned_mesh;




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
    Resource_MetaData texture_meta_data[MAX_TEXTURE_COUNT];
    //TODO: count up for now, releasing is another issue, use an arena pool with a freelist
    u32 available_texture_indexes;
    u32 max_textures;

    hash_table* texture_file_to_handle;
    // hash_table* texture_file_to_usage_count; or // hash_table* handle_to_usage_count

    //textures that the renderer needs to upload to the gpu
    //type: Texture Data // renderer should also unload the texture pixel data
    ring_queue* textures_ring_queue;

    //TODO: probably change this to a hash table, handle->font_data
    //rn this corresponds to the same indexes of the textures_array
    Madness_Font font_array[MAX_TEXTURE_COUNT];


}Texture_System;

typedef struct Mesh_System
{
    //NOTE: this is just me figuring out how to handle buffers for a particular system
    //it would also make sense to keep reuploading the meshes into the buffers every frame, well see if its a performance penality

    //global count of all the data
    static_mesh static_mesh_array[1000];
    u32 static_mesh_array_size;
    u32 static_mesh_submesh_size;

    //total size of all mesh data
    size_t vertex_byte_size;
    size_t index_byte_size;
    size_t index_count_size;
    size_t normals_byte_size;
    size_t tangent_byte_size;
    size_t uv_byte_size;



    //NOTE: NOT IN USE
    skinned_mesh* skinned_meshes;
    u32 skinned_mesh_size;


    // darray_type(VkDrawIndexedIndirectCommand*) indirect_draw_data;
} Mesh_System;


//RENDER PACKET

//these are all just references to the data, they do not own anything
typedef struct Render_Packet_Mesh
{
    const char* system_name;

    static_mesh* static_mesh_array;
    u32 static_mesh_array_size;
    u32 static_mesh_submesh_size;


    //TODO: geometry data
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

}Render_Packet_Sprite;


typedef struct Render_Packet
{
    //
    ring_queue* texture_queue;



    //FOR RENDERING

    //rn we just have one of each,
    Render_Packet_Sprite sprite_data_packet;
    Render_Packet_UI ui_data_packet;
    Render_Packet_Mesh mesh_data_packet;
    // Render_Packet_Game game_data_packet;

} Render_Packet;





#endif //RESOURCE_TYPES_H