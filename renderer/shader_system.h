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


//pass out the texture index
Material_Handle shader_system_add_material(vulkan_context* context, Shader_System* system, char const* filepath);
void shader_system_remove_material(Shader_System* system, Texture_Handle* handle);


Material_Param_Data* shader_system_get_material(Shader_System* system, Material_Handle* handle);
Material_Param_Data* shader_system_get_unused_material_param(Shader_System* system);


void material_param_data_init(Material_Param_Data* out_data);

#endif //SHADER_SYSTEM_H
