#ifndef SHADER_SYSTEM_H
#define SHADER_SYSTEM_H


//TODO: increase later when it becomes relevant
#define SHADER_SYSTEM_CAPACITY 100lu

//shader is the pipeline and descriptors ubos/ssbos needed
//material is all the param data
//texture is the physical image


Shader_System* shader_system_init(Renderer* renderer);


void shader_system_shutdown(Shader_System* system);


Vulkan_Texture* shader_system_get_texture(Shader_System* system, const Texture_Handle handle);

void shader_system_update(Renderer* renderer, Shader_System* system);

//TODO: Texture_Handle shader_system_add_texture_data(renderer* renderer, Shader_System* system, void* pixel_data, u32 width, u32 height);

void shader_system_load_textures_into_gpu(Renderer* renderer, Shader_System* shader_system,
                                          Descriptor_System* descriptor_system, Render_Packet* render_packet);




void shader_system_load_textures_into_gpu(Renderer* renderer, Shader_System* shader_system,
                                          Descriptor_System* descriptor_system, Render_Packet* render_packet);


//NOTE: Testing a shader creation system for funs and learning

typedef enum Shader_Input
{
    SHADER_INPUT_UINT32,
    SHADER_INPUT_INT32,
    SHADER_INPUT_FLOAT,
    SHADER_INPUT_VEC2,
    SHADER_INPUT_VEC3,
    SHADER_INPUT_MAT3,
    SHADER_INPUT_MAT4,
} Shader_Input;

typedef enum Shader_Output
{
    SHADER_OUTPUT_UINT32,
    SHADER_OUTPUT_INT32,
    SHADER_OUTPUT_FLOAT,
    SHADER_OUTPUT_VEC2,
    SHADER_OUTPUT_VEC3,
    SHADER_OUTPUT_MAT3,
    SHADER_OUTPUT_MAT4,
} Shader_Output;

typedef enum Shader_Stage
{
    SHADER_STAGE_VERTEX,
    SHADER_STAGE_FRAGMENT,
    SHADER_STAGE_TESSELATION,
    //NOTE: idk what geo is, and compute should be its own thing
    // SHADER_STAGE_GEOMETRY,
    // SHADER_STAGE_COMPUTE,
    SHADER_STAGE_ALL,
} Shader_Stage;

typedef enum BUFFER_LAYOUT_TYPE
{
    BUFFER_LAYOUT_st140, // structs rounds up alignments to 16 bytes
    BUFFER_LAYOUT_st430,
    BUFFER_LAYOUT_scalar, //use until performance becomes an issue, then align structs properly
} BUFFER_LAYOUT_TYPE;

//NOTE: these are assumed to be graphic shaders and not compute shaders
//NOTE: i should not be using a shader handle for this

Shader_Handle shader_system_shader_create(Shader_System* shader_system, const char* shader_name);
bool shader_system_add_input(Shader_System* shader_system, Shader_Handle handle, Shader_Input shader_type, const char* shader_input_name);
bool shader_system_add_output(Shader_System* shader_system, Shader_Handle handle, Shader_Output shader_type, const char* shader_output_name);

Shader_Handle shader_system_struct_create(Shader_System* shader_system);
bool shader_system_struct_add_field(Shader_System* shader_system);

//these would be globals generally, and then pipeline shaders can just specify which one they need
Shader_Handle shader_system_uniform_create(Shader_System* shader_system);
bool shader_system_uniform_add_field(Shader_System* shader_system, Shader_Handle handle);

Shader_Handle shader_system_push_constant_create(Shader_System* shader_system, Shader_Handle handle, Shader_Input shader_type);
bool shader_system_push_constant_add_field(Shader_System* shader_system, Shader_Handle handle);







/* MATERIAL STUFF FOR ANOTHER TIME
//pass out the texture index
Material_Handle shader_system_add_material(vulkan_context* context, Shader_System* system, char const* filepath);
void shader_system_remove_material(Shader_System* system, Texture_Handle* handle);


Material_Param_Data* shader_system_get_material(Shader_System* system, Material_Handle* handle);
Material_Param_Data* shader_system_get_unused_material_param(Shader_System* system);


void material_param_data_init(Material_Param_Data* out_data);
*/

#endif //SHADER_SYSTEM_H
