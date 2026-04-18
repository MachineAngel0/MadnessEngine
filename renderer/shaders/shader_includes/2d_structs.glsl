#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_scalar_block_layout: require
#extension GL_EXT_buffer_reference : require


layout(push_constant, scalar) uniform PushConstant_2D{
    uint ubo_buffer_idx;
    uint instance_buffer_idx;
}PC_2D;

#define BITFLAG(x) (1 << (x))
#define SPRITE_FLAG_NONE BITFLAG(1)
#define SPRITE_FLAG_TEXTURE BITFLAG(2)
#define SPRITE_FLAG_COLOR BITFLAG(3)
#define SPRITE_FLAG_TEXT BITFLAG(4)
#define SPRITE_FLAG_CIRCLE BITFLAG(5)

struct Sprite_Data {
    uint flags;

    vec2 pos;
    vec2 size;
    float rotation;

    float thickness;


    vec2 uv_offset;
    vec2 uv_size;

    vec3 color;
    uint texture_index;
    uint _padding0;
};


// binding 0 stores our textures
// binding 1 stores our params and indexes into the descriptor
layout (set = 1, binding = 0) uniform sampler2D texture_samples[];
layout (set = 1, binding = 0) uniform texture2D textures[];
layout (set = 1, binding = 0) uniform texture2DMS texturesMS[];
layout (set = 1, binding = 0) uniform textureCube textureCubes[];
layout (set = 1, binding = 0) uniform texture2DArray textureArrays[];
//layout (set = 1, binding = 1) uniform sampler samplers[];

// binding 2 stores all our buffers
layout(set = 2, binding = 0, scalar) readonly buffer SPRITE_INSTANCE_BUFFER{
    Sprite_Data sprite_instance_data[];
}Sprite_Instance_Buffer[];
