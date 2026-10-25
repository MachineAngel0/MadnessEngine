#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_scalar_block_layout: require
#extension GL_EXT_buffer_reference : require

#include "macros.glsl"

#define SPRITE_FLAG_NONE BITFLAG(0)
#define SPRITE_FLAG_TEXTURE BITFLAG(1)
#define SPRITE_FLAG_COLOR BITFLAG(2)



struct Sprite {
    uint flags;

    vec4 pos;
//    vec2 pos;
    vec2 size;
    float rotation;



    vec2 uv_offset;
    vec2 uv_size;

    vec3 color;
    uint texture_index;
};

layout(buffer_reference, scalar) readonly buffer SPRITE_SSBO{
    Sprite data[];
};

layout(push_constant, scalar) uniform PC_Sprite{
    SPRITE_SSBO sprite_ssbo;
    uint padding0;
    uint padding1;
}push_sprite;



// binding 0 stores our textures
// binding 1 stores our params and indexes into the descriptor
layout (set = 1, binding = 0) uniform sampler2D texture_samples[];
layout (set = 1, binding = 0) uniform texture2D textures[];
layout (set = 1, binding = 0) uniform texture2DMS texturesMS[];
layout (set = 1, binding = 0) uniform textureCube textureCubes[];
layout (set = 1, binding = 0) uniform texture2DArray textureArrays[];
//layout (set = 1, binding = 1) uniform sampler samplers[];





