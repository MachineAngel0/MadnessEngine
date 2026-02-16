#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_scalar_block_layout: require
#extension GL_EXT_buffer_reference : require


layout(push_constant, scalar) uniform PushConstant_2D{
    uint material_buffer_idx;
}PC_2D;


struct Material_2D_Param_Data {
    uint feature_mask;
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
layout(set = 2, binding = 0, scalar) readonly buffer MATERIAL_2D_BUFFER{
    Material_2D_Param_Data material_data[];
}MATERIAL_2D[];