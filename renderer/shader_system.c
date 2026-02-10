#include "shader_system.h"

#include "vk_buffer.h"


Shader_System* shader_system_init(renderer* renderer)
{
    Shader_System* out_shader_system = arena_alloc(&renderer->arena, sizeof(Shader_System));

    out_shader_system->max_indexes = SHADER_SYSTEM_CAPACITY;
    // create_texture_image(&renderer->context, renderer->context.graphics_command_buffer,
    //                      "../renderer/texture/error_texture.png",
    //                      &(*out_shader_system)->error_texture);

    out_shader_system->pipeline_indexes = 0;
    out_shader_system->available_texture_indexes = 0;
    out_shader_system->material_param_indexes = 0;

    //create our debug texture
    out_shader_system->default_texture_handle = shader_system_add_texture(&renderer->context, out_shader_system,
                                                                          "../renderer/texture/error_texture.png");

    out_shader_system->material_mesh_ssbo_handle = vulkan_buffer_create(renderer, renderer->buffer_system,
                                                                        BUFFER_TYPE_CPU_STORAGE,
                                                                        sizeof(Material_Param_Data) * out_shader_system
                                                                        ->max_indexes);


    return out_shader_system;
}


void shader_system_shutdown(Shader_System* system)
{
    //TODO: create and hookup to the renderer shutdown
}


Texture* shader_system_get_texture(Shader_System* system, const Texture_Handle handle)
{
    return &system->textures[handle.handle];
}

Texture* shader_system_get_default_texture(Shader_System* system)
{
    return &system->default_texture;
}

void shader_system_update(renderer* renderer, Shader_System* system)
{
    //TODO: not in use rn, but idk if i really want to control material uploads here, they should be handled by the system there in
    //updates all the material data, every frame, since they are bound to change possibly every frame
    //or we could just update at a given index, since the offsets are the same

    /*
    vulkan_buffer_reset_offset(renderer, system->material_mesh_ssbo_handle);

    vulkan_buffer_data_copy_and_upload(renderer,
                                       system->material_mesh_ssbo_handle,
                                       system->material_mesh_staging_handle,
                                       system->material_params,
                                       sizeof(Material_Param_Data) * system->material_param_indexes);
                                       */
}

//pass out the texture index
Texture_Handle shader_system_add_texture(vulkan_context* context, Shader_System* system, char const* filepath)
{
    //get an available index
    Texture_Handle out_texture_handle;
    out_texture_handle.handle = system->available_texture_indexes;

    //create the texture
    Texture* out_texture = &system->textures[out_texture_handle.handle];
    create_texture_image(context, context->graphics_command_buffer, filepath, out_texture);

    //increment index for next usage
    system->available_texture_indexes++;

    return out_texture_handle;
}

void shader_system_remove_texture(Shader_System* system, Texture_Handle* handle)
{
    //TODO:
}

Texture_Handle shader_system_update_texture(renderer* renderer, Shader_System* system, Texture_Handle* handle,
                                            const char* filepath)
{
    //overwrite if neccessary
    Texture_Handle out_texture_handle = *handle;

    // create texture if it's not already loaded
    // update descriptor set
    // pass out a new texture handle that points to the new location

    //NOTE: it might not be a good idea to overwrite a texture since another material may be using it,
    // would be better to pass out a new handle for it to use

    /* TODO:
    create_texture_image(&renderer->context, renderer->context.graphics_command_buffer,
                         "../renderer/texture/error_texture.png",
                         &renderer->context.shader_texture.texture_test_object);
    update_descriptors_texture_reflect_test(&renderer->context, &renderer->context.global_descriptor_pool,
                                            &renderer->context.shader_texture); // need to update the need for a shader texture
                                            */


    return out_texture_handle;
}

//pass out the texture index
Material_Handle shader_system_add_material(vulkan_context* context, Shader_System* system, char const* filepath)
{
    //get an available index
    Material_Handle out_material_handle = {0};


    return out_material_handle;
}

void shader_system_remove_material(Shader_System* system, Texture_Handle* handle)
{
}


Material_Param_Data* shader_system_get_material(Shader_System* system, Material_Handle* handle)
{
    return &system->material_params[handle->handle];
}

Material_Param_Data* shader_system_get_unused_material_param(Shader_System* system)
{
    //TODO
    // return &system->material_params[available_count];
    return NULL;
}


void material_param_data_init(Material_Param_Data* out_data)
{
    out_data->color = vec4_one();
    out_data->ambient_strength = 1.0;
    out_data->roughness_strength = 1.0;
    out_data->metallic_strength = 1.0;
    out_data->normal_strength = 1.0;
    out_data->ambient_occlusion_strength = 1.0;
    out_data->emissive_strength = 1.0;

    out_data->color_index = 0;
    out_data->normal_index = 0;
    out_data->metallic_index = 0;
    out_data->roughness_index = 0;
    out_data->ambient_occlusion_index = 0;
    out_data->emissive_index = 0;
}
