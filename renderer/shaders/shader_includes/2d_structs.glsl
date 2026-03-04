#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_scalar_block_layout: require
#extension GL_EXT_buffer_reference : require


layout(push_constant, scalar) uniform PushConstant_2D{
    uint instance_buffer_idx;
}PC_2D;


struct Sprite_Data {
    uint flags;

    vec2 pos;
    vec2 size;
    float rotation;

    vec2 uv_offset;
    vec2 uv_size;

    vec3 color;
    uint texture_index;
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