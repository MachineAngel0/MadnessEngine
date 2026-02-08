#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_scalar_block_layout: require
#extension GL_EXT_buffer_reference : require




struct Material_Data {
    vec4 color;
//ALL FROM RANGES 0-1
    float ambient_strength;
    float roughness_strength;
    float metallic_strength;
    float normal_strength;
    float ambient_occlusion_strength;
    float emissive_strength;

    uint color_index;
    uint normal_index;
    uint metallic_index;
    uint roughness_index;
    uint ambient_occlusion_index;
    uint emissive_index;

};

layout (buffer_reference, std430) readonly buffer MaterialsDataBuffer {
    Material_Data material_data[];
} materialsBuffer;


// binding 0 stores our textures
// binding 1 stores our params and indexes into the descriptor
layout (set = 1, binding = 0) uniform sampler2D texture_samples[];
layout (set = 1, binding = 0) uniform texture2D textures[];
layout (set = 1, binding = 0) uniform texture2DMS texturesMS[];
layout (set = 1, binding = 0) uniform textureCube textureCubes[];
layout (set = 1, binding = 0) uniform texture2DArray textureArrays[];
layout (set = 1, binding = 1) uniform sampler samplers[];


