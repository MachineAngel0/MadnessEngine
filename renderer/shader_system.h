#ifndef SHADER_SYSTEM_H
#define SHADER_SYSTEM_H


//TODO: increase later when it becomes relevant
#define shader_system_texture_amount 100;
#include "vk_image.h"


shader_handle shader_system_add_texture(vulkan_context* context, Shader_System* system, char const* filepath);


void shader_system_init(renderer* renderer, Shader_System** out_shader_system)
{
    (*out_shader_system) = arena_alloc(&renderer->arena, sizeof(Shader_System));

    // create_texture_image(&renderer->context, renderer->context.graphics_command_buffer,
    //                      "../renderer/texture/error_texture.png",
    //                      &(*out_shader_system)->error_texture);

    (*out_shader_system)->available_texture_indexes = 0;
    (*out_shader_system)->material_indexes = 0;

    //create our debug texture
    (*out_shader_system)->default_texture_handle = shader_system_add_texture(&renderer->context, *out_shader_system,
                                                       "../renderer/texture/error_texture.png");
}

Texture* shader_system_get_texture(Shader_System* system, shader_handle handle)
{
    return &system->textures[handle.handle];
}

//pass out the texture index
shader_handle shader_system_add_texture(vulkan_context* context, Shader_System* system, char const* filepath)
{
    //get an available index
    shader_handle out_texture_handle;
    out_texture_handle.handle = system->available_texture_indexes;

    //create the texture
    Texture* out_texture = &system->textures[out_texture_handle.handle];
    create_texture_image(context, context->graphics_command_buffer, filepath, out_texture);

    //increment index for next usage
    system->available_texture_indexes++;

    return out_texture_handle;
}

void shader_system_remove_texture(Shader_System* system)
{
}

void shader_system_update_texture(renderer* renderer, Shader_System* system, shader_handle* handle,
                                  const char* filepath)
{
    // create texture if it's not already loaded
    // update descriptor set
    // update shader handle to point to new location

    /* TODO:
    create_texture_image(&renderer->context, renderer->context.graphics_command_buffer,
                         "../renderer/texture/error_texture.png",
                         &renderer->context.shader_texture.texture_test_object);
    update_descriptors_texture_reflect_test(&renderer->context, &renderer->context.global_descriptor_pool,
                                            &renderer->context.shader_texture); // need to update the need for a shader texture
                                            */
}


void shader_system_add_material(Shader_System* system)
{
}

void shader_system_remove_material(Shader_System* system)
{
}


//
void material_param_data_init(Material_Param_Data* out_data)
{
    out_data->color = vec4_one();
    out_data->ambient_strength = 1.0;
    out_data->roughness_strength = 1.0;
    out_data->metallic_strength = 1.0;
    out_data->normal_strength = 1.0;
    out_data->emissive_strength = 1.0;

}



#endif //SHADER_SYSTEM_H
