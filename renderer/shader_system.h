#ifndef SHADER_SYSTEM_H
#define SHADER_SYSTEM_H




//TODO: increase later when it becomes relevant
#define SHADER_SYSTEM_CAPACITY 100lu

//shader is the pipeline and descriptors ubos/ssbos needed
//material is all the param data
//texture is the physical image


Shader_System* shader_system_init(renderer* renderer);


void shader_system_shutdown(Shader_System* system);


Texture* shader_system_get_texture(Shader_System* system, const Texture_Handle handle);

Texture* shader_system_get_default_texture(Shader_System* system);

void shader_system_update(renderer* renderer, Shader_System* system);

//pass out the texture index
Texture_Handle shader_system_add_texture_file(renderer* renderer, Shader_System* system, char const* filepath);
Texture_Handle shader_system_add_texture_font(renderer* renderer, Shader_System* system, void* pixel_data, u32 width, u32 height);
//TODO: Texture_Handle shader_system_add_texture_data(renderer* renderer, Shader_System* system, void* pixel_data, u32 width, u32 height);

void shader_system_remove_texture(Shader_System* system, Texture_Handle* handle);

Texture_Handle shader_system_update_texture(renderer* renderer, Shader_System* system, Texture_Handle* handle,
                                            const char* filepath);

//pass out the texture index
Material_Handle shader_system_add_material(vulkan_context* context, Shader_System* system, char const* filepath);
void shader_system_remove_material(Shader_System* system, Texture_Handle* handle);


Material_Param_Data* shader_system_get_material(Shader_System* system, Material_Handle* handle);
Material_Param_Data* shader_system_get_unused_material_param(Shader_System* system);


void material_param_data_init(Material_Param_Data* out_data);

#endif //SHADER_SYSTEM_H
