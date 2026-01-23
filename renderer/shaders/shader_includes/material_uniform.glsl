#extension GL_EXT_nonuniform_qualifier : require
#extension GL_EXT_scalar_block_layout: require
#extension GL_EXT_buffer_reference : require




struct Material_Data {
    vec4 color;
    uint roughness;
    uint normal_strenght;
    uint normalTex;
    uint metallicRoughnessTex;
    uint emissiveTex;
};
struct Material_Index {
    uint albedo_index;
    uint normal_index;
    uint roughness_index;
    uint metallic_index;
    uint roughness_index;
    uint emissive_index;
};


layout (buffer_reference, std430) readonly buffer MaterialsDataBuffer {
    Material_Data material_data[];
} materialsBuffer;
layout (buffer_reference, std430) readonly buffer MaterialsIndexBuffer {
    Material_Index material_indexes[];
} materialsBuffer;



// binding 0 stores our textures
// binding 1 stores our params and indexes into the descriptor
layout (set = 1, binding = 0) uniform sampler2D texture_samples[];


layout(std140, set = 1, binding = 1) uniform Material_Buffer{
    MaterialsDataBuffer material_data;
    MaterialsIndexBuffer material_indexes;
} MB[];
