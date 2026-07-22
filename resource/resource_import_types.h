#ifndef RESOURCE_IMPORT_TYPES_H
#define RESOURCE_IMPORT_TYPES_H
#include "cgltf.h"


//////////// GLTF ////////////

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

Animation_Path_Type Animation_Path_Type_gltf_to_engine[cgltf_animation_path_type_max_enum + 1] =
{
    [cgltf_animation_path_type_invalid] = Animation_Path_Type_Invalid,
    [cgltf_animation_path_type_translation] = Animation_Path_Type_Translation,
    [cgltf_animation_path_type_rotation] = Animation_Path_Type_Rotation,
    [cgltf_animation_path_type_scale] = Animation_Path_Type_Scale,
    [cgltf_animation_path_type_weights] = Animation_Path_Type_Weights,
    [cgltf_animation_path_type_max_enum] = Animation_Path_Type_Max
};


Animation_Interpolation_Type Animation_Interpolation_Type_gltf_to_engine[cgltf_interpolation_type_max_enum + 1] =
{
    [cgltf_interpolation_type_linear] = Animation_Interpolation_Type_Linear,
    [cgltf_interpolation_type_step] = Animation_Interpolation_Type_Step,
    [cgltf_interpolation_type_cubic_spline] = Animation_Interpolation_Type_Cubic_Spline,
    [cgltf_interpolation_type_max_enum] = Animation_Interpolation_Type_Max_enum,

};


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

typedef struct Joint
{
    const char* joint_name;
    u32 id;
    u32 parent_idx;
} Joint;

typedef struct GLTF_Animation_Data
{
    u32 joint_count; // also the weight count
    u32 animations_count;

    Joint* joints;
    mat4s* resting_pose_local_matrix;
    mat4s* inverse_bind_matrix;

    Animation* animations;
} GLTF_Animation_Data;


typedef struct GLTF_Material
{
    u32 flags; // Mesh_PBR_Flags
    vec4s color; // this will be at a default of 1.0, which is white but won't affect the material
    //ALL FROM RANGES 0-1
    float ambient_strength; // optional for now we can remove it later
    float roughness_strength;
    float metallic_strength;
    float normal_strength;
    float ambient_occlusion_strength;
    float emissive_strength;

    String* color_texture;
    String* normal_texture;
    String* metallic_texture;
    String* roughness_texture;
    String* ambient_occlusion_texture;
    String* emissive_texture;
} GLTF_Material;


typedef struct GLTF_SubMesh
{
    u64 tangent_bytes;
    u64 vertex_color_bytes;
    u64 vertex_bytes;
    u64 normal_bytes;
    u64 uv_bytes;
    u64 indices_bytes;

    u32 vertex_count; // this is also the count for basically every field except the index
    u32 index_count;
    u8 index_type; // enum

    //the material specifics what to load out of the file ideally
    GLTF_Material material_default;

    vec4s* tangent;
    vec4s* vertex_color;
    vec3s* vertex;
    vec3s* normal;
    vec2s* uv;
    u8* indices;
} GLTF_SubMesh;


typedef struct GLTF_Skinned_SubMesh
{
    u32 joint_bytes;
    u32 weight_bytes;

    vec4s* joints;
    vec4s* weights;
} GLTF_Skinned_SubMesh;

typedef struct GLTF_Mesh
{
    u32 mesh_count;
    GLTF_SubMesh* submesh;
    //might not exist, just depends on the file
    bool has_skeleton;
    GLTF_Skinned_SubMesh* skinned_submesh;
    GLTF_Animation_Data* animation_data;
} GLTF_Mesh;



#endif
