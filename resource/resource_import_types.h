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
    u32 joint_index;
    Animation_Path_Type animation_path_type;
} Animation_Channel;

typedef struct Animation_Sampler
{
    u32 timestamps_count;
    float* timestamps;

    float sampler_start;
    float sampler_end;

    //trs = translation rotation scale, and weights
    u32 trs_interpolation_bytes;
    Animation_Interpolation_Type interpolation_type;
    union
    {
        //it might not matter to tag this but idk
        float* trs_float;
        vec3s* trs_vec3;
        vec4s* trs_vec4;
    } interperlation_data;

} Animation_Sampler;

typedef struct Animation
{
    u32 channel_count;
    u32 sampler_count;

    float anim_start;
    float anim_end;

    String* animation_name;
    Animation_Channel* channels;
    Animation_Sampler* samplers;
} Animation;

typedef struct Joint
{
    u32 id;
    u32 parent_idx;
    String* joint_name;
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













#endif
