#ifndef SPV_REFLECT_H
#define SPV_REFLECT_H


//Todo: create an out struct for all the data i would want
typedef struct spirv_refect_info
{
    int descriptor_set_count;
    int descriptor_binding_count;
    int descriptor_set_types;
    //etc...
} spirv_refect_info;

typedef struct spirv_reflect_descriptor_set_info
{
    SpvReflectShaderStageFlagBits* stage_flags;
    u32 descriptor_set_count;
    u32* binding_number;
    SpvReflectDescriptorType* descriptor_set_types;
} spirv_reflect_descriptor_set_info;

typedef struct spirv_reflect_input_variable_info
{
    u32 input_count;
    u32* locations;
    SpvReflectFormat* formats;
    u32* offsets;
} spirv_reflect_input_variable_info;

typedef struct
{
    uint32_t flag;
    const char* name;
} ShaderStageName;

static const ShaderStageName ShaderStageTable[] = {
    {SPV_REFLECT_SHADER_STAGE_VERTEX_BIT, "VERTEX"},
    {SPV_REFLECT_SHADER_STAGE_TESSELLATION_CONTROL_BIT, "TESSELLATION_CONTROL"},
    {SPV_REFLECT_SHADER_STAGE_TESSELLATION_EVALUATION_BIT, "TESSELLATION_EVALUATION"},
    {SPV_REFLECT_SHADER_STAGE_GEOMETRY_BIT, "GEOMETRY"},
    {SPV_REFLECT_SHADER_STAGE_FRAGMENT_BIT, "FRAGMENT"},
    {SPV_REFLECT_SHADER_STAGE_COMPUTE_BIT, "COMPUTE"},
    {SPV_REFLECT_SHADER_STAGE_TASK_BIT_NV, "TASK_NV"},
    {SPV_REFLECT_SHADER_STAGE_TASK_BIT_EXT, "TASK_EXT"},
    {SPV_REFLECT_SHADER_STAGE_MESH_BIT_NV, "MESH_NV"},
    {SPV_REFLECT_SHADER_STAGE_MESH_BIT_EXT, "MESH_EXT"},
    {SPV_REFLECT_SHADER_STAGE_RAYGEN_BIT_KHR, "RAYGEN_KHR"},
    {SPV_REFLECT_SHADER_STAGE_ANY_HIT_BIT_KHR, "ANY_HIT_KHR"},
    {SPV_REFLECT_SHADER_STAGE_CLOSEST_HIT_BIT_KHR, "CLOSEST_HIT_KHR"},
    {SPV_REFLECT_SHADER_STAGE_MISS_BIT_KHR, "MISS_KHR"},
    {SPV_REFLECT_SHADER_STAGE_INTERSECTION_BIT_KHR, "INTERSECTION_KHR"},
    {SPV_REFLECT_SHADER_STAGE_CALLABLE_BIT_KHR, "CALLABLE_KHR"}
};


const char* SpvReflectDescriptorType_TABLE[] = {
    "SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER",
    "SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER",
    "SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE",
    "SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE",
    "SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER",
    "SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER",
    "SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER",
    "SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER",
    "SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC",
    "SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC",
    "SPV_REFLECT_DESCRIPTOR_TYPE_INPUT_ATTACHMENT",
    "SPV_REFLECT_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR",
};


static const int ShaderStageTableCount = sizeof(ShaderStageTable) / sizeof(ShaderStageTable[0]);


spirv_refect_info* spriv_reflection_testing(const char* shader_path);


const char* GetShaderStageName(uint32_t flag);

spirv_reflect_descriptor_set_info* spriv_reflect_get_descriptor_set(Arena* frame_arena,
                                                                    const char* vertex_shader_path,
                                                                    const char* fragment_shader_path);


spirv_reflect_input_variable_info* spriv_reflect_get_input_variable(Arena* frame_arena, const char* shader_path);


#endif //SPV_REFLECT_H
